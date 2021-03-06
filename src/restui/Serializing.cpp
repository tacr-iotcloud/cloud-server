#include <cmath>

#include <Poco/DateTime.h>
#include <Poco/Logger.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/PrintHandler.h>
#include <Poco/JSON/Array.h>

#include "l10n/Locale.h"
#include "l10n/Translator.h"
#include "model/Device.h"
#include "model/DeviceProperty.h"
#include "model/Gateway.h"
#include "model/GatewayMessage.h"
#include "model/GatewayScan.h"
#include "model/LegacyRoleInGateway.h"
#include "model/Location.h"
#include "model/ModuleInfo.h"
#include "model/RoleInGateway.h"
#include "model/Sensor.h"
#include "model/User.h"
#include "model/VerifiedIdentity.h"
#include "restui/RestValueConsumer.h"
#include "restui/Serializing.h"
#include "util/Loggable.h"

using namespace std;
using namespace Poco;
using namespace Poco::JSON;
using namespace BeeeOn;

static void valueDouble(PrintHandler &output, double d)
{
	if (std::isnan(d) || std::isinf(d))
		output.null();
	else
		output.value(d);
}

static void gatewayStatus(PrintHandler &output, const Gateway &gateway)
{
	output.startObject();

	output.key("lastModified");

	if (gateway.lastChanged().isNull())
		output.null();
	else {
		const DateTime &dt = gateway.lastChanged();
		output.value(dt.timestamp().epochTime());
	}

	output.key("ip");
	output.value(gateway.ipAddress().toString());

	output.key("version");
	output.value(gateway.version());

	output.endObject();
}

void BeeeOn::RestUI::serialize(PrintHandler &output,
		const Gateway &gateway,
		const Locale &locale)
{
	output.startObject();

	output.key("id");
	output.value(gateway.id().toString());
	output.key("name");
	output.value(gateway.name());
	output.key("altitude");

	if (!gateway.altitude().isNull())
		output.value(gateway.altitude().value());
	else
		output.null();

	output.key("latitude");
	valueDouble(output, gateway.latitude());
	output.key("longitude");
	valueDouble(output, gateway.longitude());
	output.key("timezone");
	serialize(output, gateway.timeZone(), locale);

	output.key("state");
	gatewayStatus(output, gateway);

	output.key("last_activity");
	const auto &lastActivity = gateway.lastActivity();

	if (lastActivity.isNull()) {
		output.null();
	}
	else {
		const DateTime dt = lastActivity.value();
		output.value(dt.timestamp().epochTime());
	}

	output.endObject();
}

void BeeeOn::RestUI::serialize(PrintHandler &output,
		const vector<Gateway> &gateways,
		const Locale &locale)
{
	output.startArray();

	for (auto &gateway : gateways)
		serialize(output, gateway, locale);

	output.endArray();
}

static bool scalarToJSON(
		PrintHandler &output,
		const Poco::Dynamic::Var value)
{
	if (value.isEmpty()) {
		output.null();
	}
	else if (value.isBoolean()) {
		output.value(value.convert<bool>());
	}
	else if (value.isNumeric()) {
		if (value.isInteger()) {
			if (value.isSigned())
				output.value(value.convert<long>());
			else
				output.value(value.convert<unsigned long>());
		}
		else {
			output.value(value.convert<double>());
		}
	}
	else if (value.isString()) {
		output.value(value.convert<string>());
	}
	else {
		return false;
	}

	return true;
}

void BeeeOn::RestUI::serialize(
		Poco::JSON::PrintHandler &output,
		Translator &translator,
		const GatewayMessage &message)
{
	output.startObject();

	output.key("id");
	output.value(message.id().toString());

	output.key("gateway_id");
	output.value(message.gateway().id().toString());

	output.key("at");
	output.value(static_cast<unsigned long>(
		message.at().epochMicroseconds() / 1000));

	output.key("severity");
	output.value(message.severity().toString());

	output.key("key");
	output.value(message.key());

	if (!message.context().isNull()) {
		output.key("context");
		output.startObject();

		for (const auto &pair : *message.context()) {
			output.key(pair.first);
			const auto &value = pair.second;

			if (scalarToJSON(output, value))
				continue;

			if (!value.isArray())
				continue;

			Array::Ptr array = value.extract<Array::Ptr>();
			output.startArray();

			for (const auto &entry : *array)
				scalarToJSON(output, entry);

			output.endArray();
		}

		output.endObject();
	}

	const auto &content = message.translate(translator);

	if (!content.empty() && content != message.key()) {
		output.key("display_text");
		output.value(message.translate(translator));
	}

	output.endObject();
}

void BeeeOn::RestUI::serialize(
		Poco::JSON::PrintHandler &output,
		Translator &translator,
		const std::vector<GatewayMessage> &messages)
{
	output.startArray();

	for (const auto &message : messages)
		serialize(output, translator, message);

	output.endArray();
}


void BeeeOn::RestUI::serialize(PrintHandler &output,
		const GatewayScan &scan,
		Translator &translator)
{
	output.startObject();

	if (scan.state() > GatewayScan::SCAN_IDLE) {
		output.key("duration");
		output.value(scan.duration().totalSeconds());

		output.key("started");
		output.value(scan.started().epochTime());
	}

	output.key("state");
	output.startObject();

	switch (scan.state()) {
	case GatewayScan::SCAN_IDLE:
		output.key("name");
		output.value(string("idle"));

		output.key("display_name");
		output.value(translator.formatSure("gateway.scan.state.idle"));
		break;

	case GatewayScan::SCAN_WAITING:
		output.key("name");
		output.value(string("waiting"));

		output.key("display_name");
		output.value(translator.formatSure("gateway.scan.state.waiting"));
		break;

	case GatewayScan::SCAN_PROCESSING:
		output.key("name");
		output.value(string("processing"));

		output.key("display_name");
		output.value(translator.formatSure("gateway.scan.state.processing"));
		break;

	case GatewayScan::SCAN_FINISHED:
		output.key("name");
		output.value(string("finished"));

		output.key("display_name");
		output.value(translator.formatSure("gateway.scan.state.finished"));
		break;

	case GatewayScan::SCAN_FAILED:
		output.key("name");
		output.value(string("failed"));

		output.key("display_name");
		output.value(translator.formatSure("gateway.scan.state.failed"));
		break;

	default:
		throw IllegalStateException("unexpected gateway scan state: "
			+ to_string(scan.state()));
	}

	output.endObject();
	output.endObject();
}

void BeeeOn::RestUI::serialize(PrintHandler &output,
		Translator &translator,
		const VerifiedIdentity &identity)
{
	const User &user = identity.user();
	output.startObject();

	output.key("id");
	output.value(identity.id().toString());

	output.key("first_name");
	output.value(user.firstName());

	output.key("last_name");
	output.value(user.lastName());

	output.key("locale");
	output.value(user.locale().toString());

	output.key("locale_display_name");
	output.value(user.locale().displayName());

	output.key("email");
	output.value(identity.email());

	output.key("picture_url");
	output.value(identity.picture().toString());

	output.key("provider");
	output.value(identity.provider());

	output.key("provider_display_name");
	output.value(translator.format(
		"auth.providers." + identity.provider(),
		identity.provider()
	));

	output.endObject();
}

void BeeeOn::RestUI::serialize(PrintHandler &output,
		Translator &translator,
		const list<VerifiedIdentity> &identities)
{
	output.startArray();

	for (auto &identity : identities)
		serialize(output, translator, identity);

	output.endArray();
}

void BeeeOn::RestUI::serialize(PrintHandler &output, const Location &location)
{
	output.startObject();
	output.key("id");
	output.value(location.id().toString());

	output.key("name");
	output.value(location.name());

	output.key("type");
	output.value(0); //TODO: Location does not have a type
	output.endObject();
}

void BeeeOn::RestUI::serialize(PrintHandler &output, const std::vector<Location> &locations)
{
	output.startArray();

	for (auto location : locations)
		serialize(output, location);

	output.endArray();
}

static void roleCommon(PrintHandler &output, const RoleInGateway &role)
{
	output.key("id");
	output.value(role.id().toString());

	output.key("identity_email");
	output.value(role.identity().email());

	output.key("access_level");
	output.value(role.level().toString());

	output.key("created");
	output.value(role.created().timestamp().epochTime());
}

void BeeeOn::RestUI::serialize(PrintHandler &output, const RoleInGateway &role)
{
	output.startObject();

	roleCommon(output, role);

	output.endObject();
}

void BeeeOn::RestUI::serialize(PrintHandler &output, const LegacyRoleInGateway &role)
{
	output.startObject();

	roleCommon(output, role);

	output.key("user");
	output.startObject();

	output.key("first_name");
	output.value(role.firstName());

	output.key("last_name");
	output.value(role.lastName());

	output.endObject();

	output.endObject();
}

void BeeeOn::RestUI::serialize(PrintHandler &output, const Device &device)
{
	output.startObject();

	output.key("id");
	output.value(device.id().toString());
	output.key("location_id");
	output.value(device.location().id().toString());
	output.key("name");
	output.value(device.name());

	output.key("type");
	output.startObject();

	if (!device.type().isNull()) {
		output.key("name");
		output.value(device.type()->name());
		output.key("display_name");
		output.value(device.type()->name()); // TODO: localization
		output.key("vendor");
		output.value(device.type()->vendor());
	}
	else {
		Loggable::forMethod(__func__)
			.warning("device " + device + " has no type",
				__FILE__, __LINE__);
	}

	output.key("family");
	output.value(device.id().prefix().toString());
	output.endObject();

	const DeviceStatus &status = device.status();

	output.key("first_seen");
	output.value(status.firstSeen().epochTime());
	output.key("last_seen");
	output.value(status.lastSeen().epochTime());

	if (status.active()) {
		output.key("active_since");
		output.value(status.lastChanged().epochTime());
	}

	output.key("last_changed");
	output.value(status.lastChanged().epochTime());

	output.key("state");
	switch (status.state()) {
	case DeviceStatus::STATE_INACTIVE:
		output.value(string("inactive"));
		break;
	case DeviceStatus::STATE_INACTIVE_PENDING:
		output.value(string("inactive-pending"));
		break;
	case DeviceStatus::STATE_ACTIVE:
		output.value(string("active"));
		break;
	case DeviceStatus::STATE_ACTIVE_PENDING:
		output.value(string("active-pending"));
		break;
	default:
		throw IllegalStateException(
			"unexpected state: " + to_string(status.state()));
	}

	const auto refresh = device.refresh();
	if (!refresh.isNone()) {
		output.key("refresh_time");
		output.value(refresh.seconds());
	}

	output.key("available");
	output.value(device.available());

	output.endObject();
}

void BeeeOn::RestUI::serialize(PrintHandler &output, const std::vector<RoleInGateway> &roles)
{
	output.startArray();

	for (auto role : roles)
		serialize(output, role);

	output.endArray();
}

void BeeeOn::RestUI::serialize(Poco::JSON::PrintHandler &output,
		const std::vector<Device> &devices)
{
	output.startArray();

	for (auto &device : devices)
		serialize(output, device);

	output.endArray();
}

static bool isWriteOnly(const DevicePropertyKey &key)
{
	return (!key.isUserReadable() && key.isUserWritable())
		|| (!key.isUserReadable() && !key.isUserWritable());
}

static bool isReadOnly(const DevicePropertyKey &key)
{
	return (key.isUserReadable() && !key.isUserWritable())
		|| (!key.isUserReadable() && !key.isUserWritable());
}

void BeeeOn::RestUI::serialize(
		Poco::JSON::PrintHandler &output,
		Translator &translator,
		const DecryptedDeviceProperty &property)
{
	output.startObject();

	output.key("display_name");
	output.value(translator.formatSure("device.property." + property.key().toString()));

	output.key("key");
	output.value(property.key().toString());

	output.key("value");

	if (property.key().isUserReadable())
		output.value(property.asString());
	else
		output.null();

	if (isWriteOnly(property.key())) {
		output.key("write-only");
		output.value(true);
	}

	if (isReadOnly(property.key())) {
		output.key("read-only");
		output.value(true);
	}

	output.endObject();
}

void BeeeOn::RestUI::serialize(
		Poco::JSON::PrintHandler &output,
		Translator &translator,
		const std::vector<DecryptedDeviceProperty> &properties)
{
	output.startArray();

	for (const auto &property : properties)
		serialize(output, translator, property);

	output.endArray();
}

void BeeeOn::RestUI::serialize(Poco::JSON::PrintHandler &output,
		Translator &translator,
		const DevicePropertyKey &key)
{
	output.startObject();

	output.key("display_name");
	output.value(translator.formatSure("device.property." + key.toString()));

	output.key("key");
	output.value(key.toString());

	if (isWriteOnly(key)) {
		output.key("write-only");
		output.value(true);
	}

	if (isReadOnly(key)) {
		output.key("read-only");
		output.value(true);
	}

	output.endObject();
}

void BeeeOn::RestUI::serialize(Poco::JSON::PrintHandler &output,
		Translator &translator,
		const vector<DevicePropertyKey> &keys)
{
	output.startArray();

	for (const auto &key : keys)
		serialize(output, translator, key);

	output.endArray();
}

static void serializeAttention(Poco::JSON::PrintHandler &output,
		const TypeInfo::Level::Attention attention)
{
	switch (attention) {
	case TypeInfo::Level::SINGLE:
	case TypeInfo::Level::REPEAT:
	case TypeInfo::Level::ALERT:
		output.key("attention");
		output.value(TypeInfo::Level::attentionName(attention));
		break;

	case TypeInfo::Level::NONE:
		break;
	}
}

void BeeeOn::RestUI::serialize(Poco::JSON::PrintHandler &output,
		Translator &translator,
		const TypeInfo &info)
{
	output.startObject();

	output.key("id");
	output.value(info.id().toString());

	output.key("name");
	output.value(translator.format("types." + info.name() + ".label", info.name()));

	if (!info.unit().empty()) {
		output.key("unit");
		output.value(info.unit());
	}

	const TypeInfo::Range &range = info.range();
	if (range.isValid()) {
		output.key("range");
		output.startObject();

		if (range.hasMin()) {
			output.key("min");
			output.value(range.min());
		}

		if (range.hasMax()) {
			output.key("max");
			output.value(range.max());
		}

		if (range.hasStep()) {
			output.key("step");
			output.value(range.step());
		}

		output.endObject();
	}

	if (!info.values().empty()) {
		output.key("values");
		output.startObject();

		for (const auto pair : info.values()) {
			output.key(to_string(pair.first));
			output.value(translator.format(
				"types." + info.name() + ".values." + pair.second,
				pair.second));
		}

		output.endObject();
	}

	if (!info.levels().empty()) {
		output.key("levels");
		output.startArray();

		for (const auto level : info.levels()) {
			if (!level.isValid())
				continue; // should not happen but...

			output.startObject();

			if (!level.label().empty()) {
				output.key("name");
				output.value(translator.format(
					"types." + info.name() + ".levels." + level.label(),
					level.label()));
			}

			if (!std::isnan(level.min())) {
				output.key("min");
				output.value(level.min());
			}

			if (!std::isnan(level.max())) {
				output.key("max");
				output.value(level.max());
			}

			serializeAttention(output, level.attention());
			output.endObject();
		}

		output.endArray();
	}

	output.endObject();
}

void BeeeOn::RestUI::serialize(Poco::JSON::PrintHandler &output,
		Translator &translator,
		const TypeInfoProvider::InfoSet::const_iterator begin,
		const TypeInfoProvider::InfoSet::const_iterator end)
{
	output.startArray();

	for (auto it = begin; it != end; ++it) {
		SharedPtr<TypeInfo> info = *it;
		serialize(output, translator, *info);
	}

	output.endArray();
}

void BeeeOn::RestUI::serialize(Poco::JSON::PrintHandler &output,
		Translator &translator,
		const SubtypeInfo &info)
{
	output.startObject();

	output.key("id");
	output.value(info.id().toString());

	if (info.kind() == SubtypeInfo::KIND_ENUM) {
		output.key("name");
		output.value(translator.format(
			"enums." + info.name() + ".label", info.name()));

		output.key("values");
		output.startObject();

		for (const auto pair : info.enumInfo().values()) {
			output.key(to_string(pair.first));
			output.value(translator.format(
				"enums." + info.name() + ".values." + pair.second,
				info.name()));
		}

		output.endObject();
	}
	else if (info.kind() == SubtypeInfo::KIND_BITMAP) {
		output.key("name");
		output.value(translator.format(
			"bitmaps." + info.name() + ".label", info.name()));

		const auto bitmapInfo = info.bitmapInfo();

		if (!bitmapInfo.flags().empty()) {
			output.key("flags");
			output.startObject();

			for (const auto &pair : bitmapInfo.flags()) {
				output.key(to_string(pair.first));
				output.startObject();

				output.key("name");
				output.value(translator.format(
						"bitmaps." + info.name()
						+ ".flags." + pair.second.name(),
						pair.second.name()));

				output.key("active");
				output.value(string(pair.second.inversed()? "low" : "high"));

				serializeAttention(output, pair.second.attention());

				output.endObject();
			}

			output.endObject();
		}

		if (!bitmapInfo.groups().empty()) {
			output.key("groups");
			output.startArray();

			for (const auto &group : bitmapInfo.groups()) {
				output.startObject();

				output.key("mapping");
				output.startArray();

				for (const auto &bit : group.bits())
					output.value((int) bit);

				output.endArray();

				output.key("name");
				output.value(translator.format(
						"bitmaps." + info.name()
						+ ".groups." + group.name(),
						group.name()));

				output.endObject();
			}

			output.endArray();
		}
	}

	output.endObject();
}

void BeeeOn::RestUI::serialize(Poco::JSON::PrintHandler &output,
		Translator &translator,
		const Sensor &sensor)
{
	const ModuleInfo &info = sensor.info();

	output.startObject();

	output.key("id");
	output.value(sensor.id().toString());

	if (!sensor.name().empty()) {
		output.key("display_name");
		output.value(sensor.name());
	}
	else if (!info.name().empty()) {
		output.key("display_name");
		output.value(translator.format(
			"modules." + info.name() + ".label", info.name()));
	}
	else {
		output.key("display_name");
		output.value(translator.format(
			"types." + info.type()->name() + ".label", info.type()->name()));
	}

	output.key("type_id");
	output.value(info.type()->id().toString());

	if (!info.subtype().isNull()) {
		output.key("subtype_id");
		output.value(info.subtype()->id().toString());
	}

	if (!info.group().empty()) {
		output.key("group");
		output.value(translator.format(
			"modules.groups." + info.group() + ".label", info.group()));
	}

	output.key("order");
	output.value(info.id().toString());

	if (!sensor.at().isNull()) {
		output.key("current");

		RestValueConsumer::format(
			output,
			ValueAt(sensor.at().value(), sensor.value()),
			*sensor.info().type());
	}

	output.endObject();
}

void BeeeOn::RestUI::serialize(Poco::JSON::PrintHandler &output,
		Translator &translator,
		const list<Sensor> &sensors)
{
	output.startArray();

	for (auto &sensor : sensors)
		serialize(output, translator, sensor);

	output.endArray();
}

static void serialize(Poco::JSON::PrintHandler &output,
		Translator &translator,
		const Control::RequestedValue &requested)
{
	if (!requested.requestedAt().isNull()) {
		output.key("requested_value");
		output.value(requested.value());

		output.key("requested_at");
		output.value(requested.requestedAt().value().epochTime());

		if (!requested.acceptedAt().isNull()) {
			output.key("accepted_at");
			output.value(requested.acceptedAt().value().epochTime());
		}

		if (!requested.finishedAt().isNull()) {
			output.key("finished_at");
			output.value(requested.finishedAt().value().epochTime());
		}

		output.key("result");

		switch (requested.result()) {
		case Control::RequestedValue::RESULT_FAILURE:
			output.value(string("failure"));
			break;

		case Control::RequestedValue::RESULT_SUCCESS:
			output.value(string("success"));
			break;

		case Control::RequestedValue::RESULT_UNKNOWN:
		default:
			output.value(string("waiting"));
			break;
		}
	}
}

static void serialize(Poco::JSON::PrintHandler &output,
		const ValueAt &recent)
{
	output.key("value");
	output.value(recent.value());
	output.key("at");
	output.value(recent.at().epochTime());
}

void BeeeOn::RestUI::serialize(Poco::JSON::PrintHandler &output,
		Translator &translator,
		const Control::RequestedValue &requested,
		const ValueAt &recent)
{
	if (requested.hasStarted()) {
		output.key("state");
		output.startObject();
		::serialize(output, translator, requested);
		output.endObject();
	}

	if (recent.isValid()) {
		output.key("current");
		output.startObject();
		::serialize(output, recent);
		output.endObject();
	}
}

void BeeeOn::RestUI::serialize(PrintHandler &output,
		Translator &translator,
		const Control &control)
{
	const ModuleInfo &info = control.info();

	output.startObject();

	output.key("id");
	output.value(control.id().toString());

	if (!control.name().empty()) {
		output.key("display_name");
		output.value(control.name());
	}
	else if (!info.name().empty()) {
		output.key("display_name");
		output.value(translator.format(
			"modules." + info.name() + ".label", info.name()));
	}
	else {
		output.key("display_name");
		output.value(translator.format(
			"types." + info.type()->name() + ".label", info.type()->name()));
	}

	output.key("type_id");
	output.value(info.type()->id().toString());

	if (!info.subtype().isNull()) {
		output.key("subtype_id");
		output.value(info.subtype()->id().toString());
	}

	if (!info.group().empty()) {
		output.key("group");
		output.value(translator.format(
			"modules.groups." + info.group() + ".label", info.group()));
	}

	output.key("order");
	output.value(info.id().toString());

	serialize(output,
		translator,
		control.requestedValue(),
		control.recentValue());

	output.endObject();
}

void BeeeOn::RestUI::serialize(PrintHandler &output,
		Translator &translator,
		const list<Control> &controls)
{
	output.startArray();

	for (auto &control : controls)
		serialize(output, translator, control);

	output.endArray();
}

void BeeeOn::RestUI::serialize(PrintHandler &output,
		const TimeZone &zone,
		const Locale &locale)
{
	Timestamp now;

	output.startObject();

	output.key("id");
	output.value(zone.id());

	output.key("short_name");
	output.value(zone.shortName(locale));

	output.key("display_name");
	output.value(zone.displayName(locale));

	output.key("utc_offset");
	output.value(zone.utcOffset().totalSeconds());

	output.key("dst_offset");
	output.value(zone.dstOffset().totalSeconds());

	output.key("dst_in_effect");
	output.value(zone.appliesDST(now));

	output.endObject();
}

void BeeeOn::RestUI::serialize(PrintHandler &output,
		const vector<TimeZone> &zones,
		const Locale &locale)
{
	output.startArray();

	for (auto &zone : zones)
		serialize(output, zone, locale);

	output.endArray();
}

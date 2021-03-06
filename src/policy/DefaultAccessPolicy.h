#pragma once

#include "policy/ControlAccessPolicy.h"
#include "policy/FCMTokenAccessPolicy.h"
#include "policy/GatewayAccessPolicy.h"
#include "policy/GatewayMessageAccessPolicy.h"
#include "policy/IdentityAccessPolicy.h"
#include "policy/LocationAccessPolicy.h"
#include "policy/DeviceAccessPolicy.h"
#include "policy/RoleAccessPolicy.h"
#include "policy/SensorAccessPolicy.h"
#include "policy/SensorHistoryAccessPolicy.h"
#include "dao/RoleInGatewayDao.h"
#include "dao/FCMTokenDao.h"
#include "dao/GatewayDao.h"
#include "dao/LocationDao.h"
#include "dao/DeviceDao.h"
#include "dao/UserDao.h"

namespace BeeeOn {

class DefaultAccessPolicy :
		public GatewayAccessPolicy,
		public GatewayMessageAccessPolicy,
		public IdentityAccessPolicy,
		public LocationAccessPolicy,
		public DeviceAccessPolicy,
		public RoleAccessPolicy,
		public SensorAccessPolicy,
		public SensorHistoryAccessPolicy,
		public ControlAccessPolicy,
		public FCMTokenAccessPolicy {
public:
	DefaultAccessPolicy();

	void assure(
		const GatewayAccessPolicy::Action action,
		const PolicyContext &context,
		const Gateway &gateway) override;

	void assure(
		const GatewayMessageAccessPolicy::Action action,
		const PolicyContext &context,
		const Gateway &gateway) override;

	void assure(
		const IdentityAccessPolicy::Action action,
		const PolicyContext &context,
		const Identity &identity) override;
	void assure(
		const IdentityAccessPolicy::Action action,
		const PolicyContext &context,
		const VerifiedIdentity &identity) override;
	void assure(
		const IdentityAccessPolicy::Action action,
		const PolicyContext &context,
		const User &user) override;

	void assure(
		const LocationAccessPolicy::Action action,
		const PolicyContext &context,
		const Location &location) override;
	void assure(
		const LocationAccessPolicy::Action action,
		const PolicyContext &context,
		const Gateway &gateway) override;

	void assure(
		const DeviceAccessPolicy::Action action,
		const PolicyContext &context,
		const Device &device,
		const Gateway &gateway) override;
	void assure(
		const DeviceAccessPolicy::Action action,
		const PolicyContext &context,
		const Gateway &gateway) override;
	void assureMany(
		const DeviceAccessPolicy::Action action,
		const PolicyContext &context,
		const std::list<Device> &devices) override;

	void assure(
		const RoleAccessPolicy::Action action,
		const PolicyContext &context,
		const Gateway &gateway) override;
	void assure(
		const RoleAccessPolicy::Action action,
		const PolicyContext &context,
		const RoleInGateway &role) override;

	void assure(
		const SensorHistoryAccessPolicy::Action action,
		const PolicyContext &context,
		const Device &device,
		const TimeInterval &range,
		const Poco::Timespan &interval) override;

	void assure(
		const SensorAccessPolicy::Action action,
		const PolicyContext &context,
		const Sensor &sensor,
		const Device &device) override;

	void assureMany(
		const SensorAccessPolicy::Action action,
		const PolicyContext &context,
		const Device &device) override;

	void assure(
		const ControlAccessPolicy::Action action,
		const PolicyContext &context,
		const Control &control,
		const Device &device) override;

	void assureMany(
		const ControlAccessPolicy::Action action,
		const PolicyContext &context,
		const Device &device) override;

	void assure(
		const FCMTokenAccessPolicy::Action action,
		const PolicyContext &context,
		const FCMToken &token) override;

	void setFCMTokenDao(FCMTokenDao::Ptr dao)
	{
		m_fcmTokenDao = dao;
	}

	void setUserDao(UserDao *dao)
	{
		m_userDao = dao;
	}

	void setGatewayDao(GatewayDao *dao)
	{
		m_gatewayDao = dao;
	}

	void setLocationDao(LocationDao *dao)
	{
		m_locationDao = dao;
	}

	void setDeviceDao(DeviceDao *dao)
	{
		m_deviceDao = dao;
	}

	void setRoleInGatewayDao(RoleInGatewayDao *dao)
	{
		m_roleInGatewayDao = dao;
	}

protected:
	AccessLevel fetchAccessLevel(const PolicyContext &context, const Gateway &gateway);
	void assureAtLeast(
			const AccessLevel &current,
			const AccessLevel &required);

	const User &userFromContext(const PolicyContext &context);

	/**
	 * Test whether the given role represents the PolicyContext.
	 */
	bool representsSelf(const RoleInGateway &role, const PolicyContext &self);
	bool representsSelf(const User &user, const PolicyContext &self);
	bool canSeeIdentity(const Identity &identity, const PolicyContext &self);
	bool canSeeIdentity(const VerifiedIdentity &identity, const PolicyContext &self);

	void doAssure(
		const DeviceAccessPolicy::Action action,
		const PolicyContext &context,
		const Gateway &gateway);

private:
	UserDao *m_userDao;
	FCMTokenDao::Ptr m_fcmTokenDao;
	GatewayDao *m_gatewayDao;
	LocationDao *m_locationDao;
	DeviceDao *m_deviceDao;
	RoleInGatewayDao *m_roleInGatewayDao;
};

}

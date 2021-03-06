#pragma once

#include <string>

#include <Poco/AutoPtr.h>
#include <Poco/Crypto/Cipher.h>
#include <Poco/Net/IPAddress.h>

#include "net/MACAddress.h"
#include "util/CryptoParams.h"
#include "util/Enum.h"

namespace BeeeOn {

class CryptoConfig;

struct DevicePropertyKeyEnum {
public:
	enum Raw {
		KEY_INVALID = 0,
		/**
		 * IP address where the associated device can be found.
		 * The password is encrypted to protect against database
		 * data stealing.
		 */
		KEY_IP_ADDRESS = 1,
		/**
		 * Password of the associated device. The password is
		 * encrypted to protect against database data stealing.
		 */
		KEY_PASSWORD = 2,
		/**
		 * Information about detected firmware in the target device.
		 */
		KEY_FIRMWARE = 3,
		/**
		 * MAC Address of the associated device.
		 */
		KEY_MAC_ADDRESS = 4,
		/**
		 * Serial number of the associated device.
		 */
		KEY_SERIAL_NUMBER = 5,
	};

	/**
	 * @return true if user can write this property
	 */
	bool isUserWritable() const;

	/**
	 * @return true if user can read this property
	 */
	bool isUserReadable() const;

	/**
	 * @return true if gateway can write this property
	 */
	bool isGatewayWritable() const;

	/**
	 * @return true if gateway can read this property
	 */
	bool isGatewayReadable() const;

	/**
	 * @brief Derive crypto params for property of this key.
	 * Some keys might require encryption while others might not.
	 */
	CryptoParams deriveParams(const CryptoConfig &config) const;

	static EnumHelper<Raw>::ValueMap &valueMap();
};

typedef Enum<DevicePropertyKeyEnum> DevicePropertyKey;

/**
 * Every device can have a set of associated properties.
 * The properties are useful especially in case when a device
 * requires some special settings to be configured or found
 * properly in the PAN/LAN/WAN/...
 */
class DeviceProperty {
public:
	DeviceProperty();

	void setKey(const DevicePropertyKey &key);
	void setKey(const DevicePropertyKey::Raw &key)
	{
		setKey(DevicePropertyKey::fromRaw(key));
	}

	const DevicePropertyKey &key() const;

	void setValue(const std::string &value);
	std::string value() const;

	void setIPAddress(const Poco::Net::IPAddress &address, Poco::AutoPtr<Poco::Crypto::Cipher> cipher);
	void setPassword(const std::string &password, Poco::AutoPtr<Poco::Crypto::Cipher> cipher);
	void setFirmware(const std::string &firmware);
	void setMACAddress(const MACAddress &address, Poco::AutoPtr<Poco::Crypto::Cipher> cipher);
	void setSerial(const uint64_t serial, Poco::AutoPtr<Poco::Crypto::Cipher> cipher);

	void setFromString(const std::string &input, const CryptoConfig &config);

	Poco::Net::IPAddress asIPAddress(Poco::AutoPtr<Poco::Crypto::Cipher> cipher) const;
	std::string asPassword(Poco::AutoPtr<Poco::Crypto::Cipher> cipher) const;
	std::string asFirmware() const;
	MACAddress asMACAddress(Poco::AutoPtr<Poco::Crypto::Cipher> cipher) const;
	uint64_t asSerial(Poco::AutoPtr<Poco::Crypto::Cipher> cipher) const;

	std::string asString(Poco::AutoPtr<Poco::Crypto::Cipher> cipher) const;

	void setParams(const CryptoParams &params);
	CryptoParams params() const;

private:
	DevicePropertyKey m_key;
	std::string m_value;
	CryptoParams m_params;
};

class DecryptedDeviceProperty {
public:
	DecryptedDeviceProperty(
		const DeviceProperty &property,
		const CryptoConfig *config);

	const DevicePropertyKey &key() const;
	Poco::Net::IPAddress asIPAddress() const;
	std::string asPassword() const;
	std::string asFirmware() const;
	std::string asString() const;
	MACAddress asMACAddress() const;
	uint64_t asSerial() const;

private:
	const DeviceProperty &m_property;
	Poco::AutoPtr<Poco::Crypto::Cipher> m_cipher;
};

}

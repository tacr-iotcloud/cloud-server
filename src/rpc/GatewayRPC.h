#pragma once

#include <functional>

#include <Poco/Exception.h>
#include <Poco/SharedPtr.h>
#include <Poco/Timespan.h>

#include "model/Gateway.h"
#include "model/Device.h"
#include "model/ModuleInfo.h"
#include "model/OpMode.h"
#include "rpc/GatewayRPCHandler.h"
#include "rpc/GatewayRPCResult.h"

namespace BeeeOn {

/**
 * Class GatewayRPC is used to access gateways remotely.
 * It allows to send primitive commands to gateways. Most of
 * the communication works asynchronously and the results are
 * not returned immediately but must be queries in other ways.
 */
class GatewayRPC {
public:
	typedef Poco::SharedPtr<GatewayRPC> Ptr;

	virtual ~GatewayRPC();

	/**
	 * Send listen command.
	 */
	virtual void sendListen(
		const GatewayRPCHandler::Ptr handler,
		const Gateway &gateway,
		const Poco::Timespan &duration) = 0;

	/**
	 * Send pair device command to the selected gateway.
	 */
	virtual void pairDevice(
		const GatewayRPCHandler::Ptr handler,
		const Gateway &gateway,
		const Device &device) = 0;

	/**
	 * Send unpair device command to the selected gateway.
	 */
	virtual void unpairDevice(
		const GatewayRPCHandler::Ptr handler,
		const Gateway &gateway,
		const Device &device) = 0;

	/**
	 * Send update command for the given module.
	 */
	virtual void updateActor(
		const GatewayRPCHandler::Ptr handler,
		const Gateway &gateway,
		const Device &device,
		const ModuleInfo &module,
		double value,
		const Poco::Timespan &timeout,
		const OpMode &mode = OpMode::TRY_ONCE) = 0;

protected:
	static void doHandle(
		GatewayRPCHandler::Ptr handler,
		GatewayRPCResult::Ptr result);
};

}

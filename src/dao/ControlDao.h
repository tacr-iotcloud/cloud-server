#pragma once

#include <list>

#include <Poco/SharedPtr.h>

#include "model/Control.h"

namespace BeeeOn {

class Device;

class ControlDao {
public:
	typedef Poco::SharedPtr<ControlDao> Ptr;

	virtual ~ControlDao();

	virtual bool fetch(Control &control,
			const Device &device) = 0;
	virtual void fetchBy(std::list<Control> &control,
			const Device &device) = 0;
	virtual bool insert(const Control::RequestedValue &request,
			const Control &control,
			const Device &device) = 0;
	virtual bool markAccepted(const Control::RequestedValue &request,
			const Control &control,
			const Device &device) = 0;
	virtual bool markFinished(const Control::RequestedValue &request,
			const Control &control,
			const Device &device) = 0;
	virtual size_t cancelUnfinished() = 0;
};

}

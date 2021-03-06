#pragma once

#include <Poco/Exception.h>
#include <Poco/SharedPtr.h>

#include "dao/MockDao.h"
#include "model/User.h"
#include "model/Identity.h"

namespace BeeeOn {

class UserDao {
public:
	typedef Poco::SharedPtr<UserDao> Ptr;

	virtual ~UserDao();

	virtual void create(User &user) = 0;
	virtual bool fetch(User &user) = 0;
};

class MockUserDao : public MockDao<User, UserDao> {
protected:
	UserID nextID()
	{
		return UserID::random();
	}
};

}

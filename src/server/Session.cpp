#include "server/Session.h"

using namespace Poco;
using namespace BeeeOn;

Session::Session(const SessionID &id):
	m_sessionID(id)
{
}

Session::~Session()
{
}

void Session::setIdentityID(const VerifiedIdentityID &id)
{
	m_identityID = id;
}

const VerifiedIdentityID &Session::identityID() const
{
	return m_identityID;
}

void Session::setUserID(const UserID &id)
{
	m_userID = id;
}

const UserID Session::userID() const
{
	return m_userID;
}

void Session::setLocale(const Locale &locale)
{
	m_locale = locale;
}

const Locale &Session::locale() const
{
	return m_locale;
}

const SessionID Session::sessionID() const
{
	return m_sessionID;
}

void Session::setExpiration(const Timestamp &expiration)
{
	m_expiration = expiration;
}

const Timestamp& Session::getExpiration() const
{
	return m_expiration;
}

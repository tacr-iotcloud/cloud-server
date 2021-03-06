#include <iostream>

#include <Poco/Exception.h>

#include "rest/RestFlow.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

RestFlow::RestFlow(
		RestLinker &linker,
		URI &uri,
		UriPattern::Params &params,
		PocoRequest request,
		PocoResponse response):
	m_linker(linker),
	m_uri(uri),
	m_params(params),
	m_request(request),
	m_response(response)
{
}

const Poco::URI &RestFlow::uri() const
{
	return m_uri;
}

const UriPattern::Params &RestFlow::params() const
{
	return m_params;
}

string RestFlow::param(const string &key) const
{
	auto it = m_params.find(key);
	if (it == m_params.end())
		throw InvalidArgumentException("no such key " + key);

	return it->second;
}

string RestFlow::param(const string &key, const string &def) const
{
	auto it = m_params.find(key);
	if (it == m_params.end())
		return def;

	return it->second;
}

RestLinker &RestFlow::linker()
{
	return m_linker;
}

const PocoRequest &RestFlow::request() const
{
	return m_request;
}

PocoRequest &RestFlow::request()
{
	return m_request;
}

const PocoResponse &RestFlow::response() const
{
	return m_response;
}

PocoResponse &RestFlow::response()
{
	return m_response;
}

bool RestFlow::hasSession() const
{
	return !m_session.isNull();
}

void RestFlow::setSession(Session::Ptr session)
{
	m_session = session;
}

Session::Ptr RestFlow::session() const
{
	return m_session;
}

void RestFlow::setTranslator(Translator::Ptr translator)
{
	m_translator = translator;
}

Translator::Ptr RestFlow::translator() const
{
	return m_translator;
}

void RestFlow::setLocale(const Locale &locale)
{
	m_locale = locale;
}

const Locale &RestFlow::locale(bool ignoreSessionLocale) const
{
	if (ignoreSessionLocale || m_session.isNull())
		return m_locale;

	return m_session->locale();
}

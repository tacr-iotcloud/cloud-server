#include <Poco/NumberFormatter.h>
#include <Poco/NumberParser.h>
#include <Poco/RegularExpression.h>
#include <Poco/StringTokenizer.h>
#include <Poco/TextEncoding.h>
#include <Poco/TextConverter.h>
#include <Poco/TextIterator.h>
#include <Poco/UTF8Encoding.h>
#include <Poco/UTF8String.h>
#include <Poco/Ascii.h>

#include "util/Sanitize.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

string Sanitize::encoding(const string &bytes,
		const unsigned long sizeLimit,
		const string &inputEncoding)
{
	const TextEncoding &encoding = TextEncoding::byName(inputEncoding);
	const UTF8Encoding utf8;

	if (sizeLimit > 0 && bytes.size() > sizeLimit)
		throw InvalidArgumentException("input is too long");

	TextConverter text(encoding, utf8);
	string result;
	text.convert(bytes, result);

	return result;
}

// Based on:
// https://www.w3.org/TR/html/sec-forms.html#valid-e-mail-address
// http://www.regular-expressions.info/email.html

#define EMAIL_LOCAL_PART_MAX      64
#define EMAIL_DOMAIN_PART_MAX     255
#define EMAIL_LOCAL_PART_REGEX "[a-z0-9\\.!#$%&'*+\\\\/=?^_`{|}~-]+"
#define EMAIL_DOMAIN_PART_REGEX "[a-z0-9\\.-]+\\.[a-z0-9]{2,63}"

static RegularExpression emailLocalPartRegex(EMAIL_LOCAL_PART_REGEX,
	RegularExpression::RE_DOLLAR_ENDONLY  | // $ matchs only End-of-String
	RegularExpression::RE_NO_AUTO_CAPTURE | // do not numbered captures
	RegularExpression::RE_UTF8,
	true
);

static RegularExpression emailDomainPartRegex(EMAIL_DOMAIN_PART_REGEX,
	RegularExpression::RE_DOLLAR_ENDONLY  | // $ matchs only End-of-String
	RegularExpression::RE_NO_AUTO_CAPTURE | // do not numbered captures
	RegularExpression::RE_UTF8,
	true
);

static bool match(const RegularExpression &exp, const string &s, size_t offset = 0)
{
	// implies RE_ANCHORED and RE_NOTEMPTY
	return exp.match(s, offset);
}

string Sanitize::email(const string &bytes,
		const unsigned long sizeLimit,
		const string &inputEncoding)
{
	string email(Sanitize::encoding(bytes, sizeLimit, inputEncoding));
	UTF8::toLowerInPlace(email);

	size_t at = email.find_last_of('@');
	if (at == string::npos)
		throw InvalidAccessException("e-mail is invalid: missing '@'");

	if (at > EMAIL_LOCAL_PART_MAX)
		throw InvalidAccessException("e-mail is invalid: too long local-part");

	if (email.size() - (at + 1) > EMAIL_DOMAIN_PART_MAX)
		throw InvalidAccessException("e-mail is invalid: too long domain-part");

	if (email.find("..") != string::npos)
		throw InvalidAccessException("e-mail is invalid: unexpected characters");

	if (!match(emailLocalPartRegex, email.substr(0, at)))
		throw InvalidAccessException("e-mail is invalid: bad local-part");

	if (!match(emailDomainPartRegex, email, at + 1))
		throw InvalidAccessException("e-mail is invalid: bad domain-part");

	return email;
}

static void xmlNonAmpersand(string &result, int c)
{
	if (c == '<') {
		result.append("&#60;");
	}
	else if (c == '>') {
		result.append("&#62;");
	}
	else if (c == '\"') {
		result.append("&#34;");
	}
	else if (c == '\'') {
		result.append("&#39;");
	}
	else if (c == '/') {
		result.append("&#47;");
	}
	else if (c < 128) {
		result += ((unsigned char) c);
	}
	else {
		result.append("&#");
		NumberFormatter::append(result, c);
		result.append(";");
	}
}

static void xmlAfterAmpersand(string &result, TextIterator &it, const TextIterator &end)
{
	string number;

	if (it == end) {
		result.append("&#38;");
		return;
	}

	int c = *it;

	// skip non-decadic entity
	if (c != '#') {
		result.append("&#38;");
		xmlNonAmpersand(result, c);
		return;
	}

	c = *(++it);
	for (int i = 0; i < 4 && Ascii::isDigit(c) && it != end; ++i) {
		number += ((unsigned char) c);
		c = *(it++);
	}

	if (c != ';') { // at &#_ or &#D_ or &#DD_ or &#DDD_ or &#DDDD_
		result.append("&#38;");
		result.append("#");
		result.append(number);
		xmlNonAmpersand(result, c);
		return;
	}

	// at &#[:digit:]{0,4};
	if (number.size() == 0) {
		result.append("&#38;#;");
		return;
	}

	// bypass decadic entity
	result.append("&#");
	result.append(number);
	result.append(";");
}

string Sanitize::xml(const string &bytes,
		const unsigned long sizeLimit,
		const string &inputEncoding)
{
	const string xml(Sanitize::encoding(bytes, sizeLimit, inputEncoding));
	string result;

	const UTF8Encoding utf8;
	TextIterator it(xml, utf8);
	TextIterator end(xml);

	for (; it != end; ++it) {
		int c = *it;
	
		if (c == '&') {
			if (++it == end) {
				result.append("&#38;");
				break;
			}

			xmlAfterAmpersand(result, it, end);
		}
		else
			xmlNonAmpersand(result, c);
	}

	return result;
}

static RegularExpression commonRegex(
	"[\\p{L}\\p{Nd} \\.:!?()/,\\-_#'$€¥£©®+]*",
	RegularExpression::RE_DOLLAR_ENDONLY  |
	RegularExpression::RE_NO_AUTO_CAPTURE |
	RegularExpression::RE_UTF8,
	true
);

string Sanitize::common(const string &bytes,
		const unsigned long sizeLimit,
		const string &inputEncoding)
{
	string result(Sanitize::encoding(bytes, sizeLimit, inputEncoding));

	if (result.size() == 0)
		return "";

	if (match(commonRegex, result))
		return result;

	throw InvalidArgumentException("detected unexpected content");
}

static RegularExpression strictRegex(
	"[-a-zA-Z0-9 \t]+",
	RegularExpression::RE_DOLLAR_ENDONLY  |
	RegularExpression::RE_NO_AUTO_CAPTURE |
	RegularExpression::RE_UTF8,
	true
);

static void strictSimple(const string &text)
{
	if (!match(strictRegex, text))
		throw InvalidArgumentException(
			"unexpected content for strictly-validated data");
}

string Sanitize::strict(const string &bytes,
		const string &separators,
		const unsigned long sizeLimit,
		const string &inputEncoding)
{
	const string text(Sanitize::encoding(bytes, sizeLimit, inputEncoding));

	if (text.empty())
		return "";

	if (separators.empty()) {
		strictSimple(text);
	}
	else {
		const StringTokenizer toks(bytes, separators);

		for (const auto &token : toks)
			strictSimple(token);
	}

	return text;
}

URI Sanitize::uri(const string &bytes,
		const unsigned long sizeLimit,
		const string &inputEncoding)
{
	string decoded;
	URI::decode(bytes, decoded);

	const string utf8(Sanitize::encoding(decoded, sizeLimit, inputEncoding));

	string result;
	URI::encode(utf8, "", result);

	return URI(result);
}

static RegularExpression base64Regex(
	"[+a-zA-Z0-9/]+={0,2}",
	RegularExpression::RE_DOLLAR_ENDONLY  |
	RegularExpression::RE_NO_AUTO_CAPTURE |
	RegularExpression::RE_UTF8,
	true
);

static RegularExpression base64URLRegex(
	"[-a-zA-Z0-9_]+={0,2}",
	RegularExpression::RE_DOLLAR_ENDONLY  |
	RegularExpression::RE_NO_AUTO_CAPTURE |
	RegularExpression::RE_UTF8,
	true
);

string Sanitize::base64(const string &bytes,
		const std::string &separators,
		const unsigned long sizeLimit,
		const string &inputEncoding)
{
	string base64(Sanitize::encoding(bytes, sizeLimit, inputEncoding));

	if (base64.empty())
		return "";

	if (separators.empty()) {
		if (match(base64Regex, base64))
			return base64;
			
		if (!match(base64URLRegex, base64))
			throw InvalidArgumentException("invalid base64 content");
	}
	else if (!separators.empty()) {
		const StringTokenizer fragments(base64, separators);

		for (const auto &one : fragments) {
			if (match(base64Regex, one))
				continue;

			if (!match(base64URLRegex, one)) {
				throw InvalidArgumentException(
					"invalid base64 content in fragment: " + one);
			}
		}
	}

	return base64;
}

static RegularExpression tokenRegex(
	"[0-9a-zA-Z\\-_\\.:]+",
	RegularExpression::RE_DOLLAR_ENDONLY  |
	RegularExpression::RE_NO_AUTO_CAPTURE |
	RegularExpression::RE_UTF8,
	true
);

string Sanitize::token(const string &bytes,
		const unsigned long sizeLimit,
		const string &inputEncoding)
{
	const string text(Sanitize::encoding(bytes, sizeLimit, inputEncoding));

	if (!match(tokenRegex, text))
		throw InvalidArgumentException(
			"unexpected content");

	return text;
}

static RegularExpression localeRegex(
	"[0-9a-zA-Z\\-_\\.]+",
	RegularExpression::RE_DOLLAR_ENDONLY  |
	RegularExpression::RE_NO_AUTO_CAPTURE |
	RegularExpression::RE_UTF8,
	true
);

string Sanitize::locale(const string &bytes,
		const unsigned long sizeLimit,
		const string &inputEncoding)
{
	const string text(Sanitize::encoding(bytes, sizeLimit, inputEncoding));

	if (!match(localeRegex, text))
		throw InvalidArgumentException("unexpected locale input");

	return text;
}

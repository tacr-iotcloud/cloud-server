#include <unicode/uvernum.h>

#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Timestamp.h>
#include <Poco/DateTime.h>

#include "cppunit/BetterAssert.h"
#include "l10n/IcuLocaleImpl.h"
#include "l10n/IcuTranslator.h"
#include "l10n/Locale.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class IcuTranslatorTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(IcuTranslatorTest);
	CPPUNIT_TEST(testCreateNonExisting);
	CPPUNIT_TEST(testCreateExisting);
	CPPUNIT_TEST(testFormatMissing);
	CPPUNIT_TEST(testFormatSimple);
	CPPUNIT_TEST(testFormatDateTime);
	CPPUNIT_TEST_SUITE_END();

public:
	IcuTranslatorTest();
	void setUp();

	void testAvailableLocales();
	void testCreateNonExisting();
	void testCreateExisting();
	void testFormatMissing();
	void testFormatSimple();
	void testFormatDateTime();

private:
	IcuTranslatorFactory m_factory;
	const Locale root;
	const Locale en_US;
	const Locale en_GB;
	const Locale cs_CZ;
};

CPPUNIT_TEST_SUITE_REGISTRATION(IcuTranslatorTest);

IcuTranslatorTest::IcuTranslatorTest():
	root(new IcuLocaleImpl),
	en_US(new IcuLocaleImpl("en", "US")),
	en_GB(new IcuLocaleImpl("en", "GB")),
	cs_CZ(new IcuLocaleImpl("cs", "CZ"))
{
}

void IcuTranslatorTest::setUp()
{
	// Set environment variable ICU_DATA or assume we are
	// running tests from the root of the project.
	//
	// The bundles are expected to be named as
	//
	//   test/l10n/test/<locale>.res
	//
	if (!strcmp(u_getDataDirectory(), ""))
		u_setDataDirectory("test/l10n");
}

void IcuTranslatorTest::testCreateNonExisting()
{
	Locale locale(new IcuLocaleImpl);

	CPPUNIT_ASSERT_THROW(
		m_factory.create(locale, "non-existing"),
		NotFoundException
	);
}

void IcuTranslatorTest::testCreateExisting()
{
	SharedPtr<Translator> translator;

	CPPUNIT_ASSERT_NO_THROW(translator = m_factory.create(root, "test"));
	CPPUNIT_ASSERT_EQUAL("hello", translator->formatSure("hello"));

	CPPUNIT_ASSERT_NO_THROW(translator = m_factory.create(en_US, "test"));
	CPPUNIT_ASSERT_EQUAL("Hello", translator->formatSure("hello"));

	CPPUNIT_ASSERT_NO_THROW(translator = m_factory.create(en_GB, "test"));
	CPPUNIT_ASSERT_EQUAL("Hello", translator->formatSure("hello"));

	CPPUNIT_ASSERT_NO_THROW(translator = m_factory.create(cs_CZ, "test"));
	CPPUNIT_ASSERT_EQUAL("Ahoj", translator->formatSure("hello"));
}

void IcuTranslatorTest::testFormatMissing()
{
	SharedPtr<Translator> translator;

	CPPUNIT_ASSERT_NO_THROW(translator = m_factory.create(root, "test"));

	// fallback to bypass of the input
	CPPUNIT_ASSERT_EQUAL("helloX", translator->formatSure("helloX"));

	// fallback to root locale
	CPPUNIT_ASSERT_NO_THROW(translator = m_factory.create(en_US, "test"));
	CPPUNIT_ASSERT_EQUAL("hello2", translator->formatSure("hello2"));

	// fallback to root locale
	CPPUNIT_ASSERT_NO_THROW(translator = m_factory.create(cs_CZ, "test"));
	CPPUNIT_ASSERT_EQUAL("hello2", translator->formatSure("hello2"));
}

void IcuTranslatorTest::testFormatSimple()
{
	SharedPtr<Translator> translator;
	CPPUNIT_ASSERT_NO_THROW(translator = m_factory.create(root, "test"));

	CPPUNIT_ASSERT_EQUAL("Any: 1", translator->formatSure("fmtany", 1));
	CPPUNIT_ASSERT_EQUAL("Any: x", translator->formatSure("fmtany", "x"));
	CPPUNIT_ASSERT_EQUAL("Any: 1", translator->formatSure("fmtany", true));
	CPPUNIT_ASSERT_EQUAL("Any: 0", translator->formatSure("fmtany", false));
	CPPUNIT_ASSERT_EQUAL("Any: 2.5", translator->formatSure("fmtany", 2.5));

	CPPUNIT_ASSERT_EQUAL("Int: 5", translator->formatSure("fmtint", 5));
	CPPUNIT_ASSERT_THROW(
		translator->formatSure("fmtint", "5"),
		InvalidArgumentException
	);

	CPPUNIT_ASSERT_EQUAL("Str: text", translator->formatSure("fmtstr", "text"));
	CPPUNIT_ASSERT_EQUAL("Str: 4", translator->formatSure("fmtstr", 4));
}

void IcuTranslatorTest::testFormatDateTime()
{
	SharedPtr<Translator> translator;
	CPPUNIT_ASSERT_NO_THROW(translator = m_factory.create(root, "test"));

	Timestamp epoch(0);
#if U_ICU_VERSION_MAJOR_NUM >= 59
	CPPUNIT_ASSERT_EQUAL("Date: 1970 M01 1", translator->formatSure("fmtdate", epoch));
#else
	CPPUNIT_ASSERT_EQUAL("Date: 1970 1 1", translator->formatSure("fmtdate", epoch));
#endif
	CPPUNIT_ASSERT_EQUAL("Time: 01:00:00", translator->formatSure("fmttime", epoch));
	CPPUNIT_ASSERT_EQUAL("Str: 1970-01-01 01:00", translator->formatSure("fmtstr", epoch));

	DateTime dt(epoch);
#if U_ICU_VERSION_MAJOR_NUM >= 59
	CPPUNIT_ASSERT_EQUAL("Date: 1970 M01 1", translator->formatSure("fmtdate", dt));
#else
	CPPUNIT_ASSERT_EQUAL("Date: 1970 1 1", translator->formatSure("fmtdate", dt));
#endif
	CPPUNIT_ASSERT_EQUAL("Time: 01:00:00", translator->formatSure("fmttime", dt));
	CPPUNIT_ASSERT_EQUAL("Str: 1970-01-01 01:00", translator->formatSure("fmtstr", dt));

	Timestamp recent(1503005688 * Timespan::SECONDS);
#if U_ICU_VERSION_MAJOR_NUM >= 59
	CPPUNIT_ASSERT_EQUAL("Date: 2017 M08 17", translator->formatSure("fmtdate", recent));
#else
	CPPUNIT_ASSERT_EQUAL("Date: 2017 8 17", translator->formatSure("fmtdate", recent));
#endif
	CPPUNIT_ASSERT_EQUAL("Time: 23:34:48", translator->formatSure("fmttime", recent));
	CPPUNIT_ASSERT_EQUAL("Str: 2017-08-17 23:34", translator->formatSure("fmtstr", recent));
}

}

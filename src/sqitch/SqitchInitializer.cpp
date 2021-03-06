#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <Poco/Logger.h>
#include <Poco/StreamCopier.h>

#include "sqitch/SqitchInitializer.h"
#include "sqitch/SqitchLine.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

SqitchInitializer::SqitchInitializer():
	m_extension("sql")
{
}

void SqitchInitializer::loadPlanFile(const string &file)
{
	m_plan.load(file);

	if (m_baseDir.toString().empty())
		m_baseDir = Path(file).parent();
}

void SqitchInitializer::setBaseDir(const string &dir)
{
	m_baseDir = dir;
}

void SqitchInitializer::setExtension(const string &ext)
{
	m_extension = ext;
}

string SqitchInitializer::extension(const string &sep) const
{
	if (m_extension.empty())
		return "";

	return sep + m_extension;
}

void SqitchInitializer::unsafeDeploy(
		const ScriptExecutor &execute)
{
	logger().warning("executing unsafe deploy operation intended only for TESTING",
			__FILE__, __LINE__);

	for (auto &line : m_plan) {
		SqitchChange::Ptr change = line.cast<SqitchChange>();
		if (change.isNull())
			continue;

		switch (change->operation()) {
		case SqitchChange::DEPLOY:
			applyChange(execute, change, m_plan);
			break;
		case SqitchChange::REVERT:
			applyRevert(execute, change, m_plan);
			break;
		default:
			throw IllegalStateException(
				"unknown change operation: "
				+ to_string(change->operation())
			);
		}
	}
}

void SqitchInitializer::applyChange(
	const ScriptExecutor &execute,
	const SqitchChange::Ptr change,
	const SqitchPlan &plan) const
{
	const Path &deploy = resolve("deploy", change, plan);
	const Path &verify = resolve("verify", change, plan);

	string deployScript;

	try {
		logger().debug("loading script from: " + deploy.toString());
		FileInputStream in(deploy.toString());
		StreamCopier::copyToString(in, deployScript);
	}
	catch (const Poco::IOException &e) {
		logger().log(e, __FILE__, __LINE__);
		logger().critical("failed to load change " + change->name(),
				__FILE__, __LINE__);
		e.rethrow();
	}

	string verifyScript;

	try {
		logger().debug("loading script from: " + verify.toString());
		FileInputStream in(verify.toString());
		StreamCopier::copyToString(in, verifyScript);
	}
	catch (const Poco::IOException &e) {
		logger().log(e, __FILE__, __LINE__);
		logger().warning("verify script for change " + change->name() + " is missing",
				__FILE__, __LINE__);
	}

	try {
		logger().information("deploying change " + change->name());
		execute(deployScript);

		if (!verifyScript.empty()) {
			logger().information("verifying change " + change->name());
			execute(verifyScript);
		}

		logger().notice("change " + change->name() + " successfully deployed",
				__FILE__, __LINE__);
	}
	catch (...) {
		logger().critical("failed to deploy change: " + change->name());
		throw;
	}
}

void SqitchInitializer::applyRevert(
	const ScriptExecutor &execute,
	const SqitchChange::Ptr change,
	const SqitchPlan &plan) const
{
	const Path &revert = resolve("revert", change, plan);
	string revertScript;

	try {
		logger().debug("loading script from: " + revert.toString());
		FileInputStream in(revert.toString());
		StreamCopier::copyToString(in, revertScript);
	}
	catch (const Poco::IOException &e) {
		logger().log(e, __FILE__, __LINE__);
		logger().critical("failed to load change " + change->name(),
				__FILE__, __LINE__);
		e.rethrow();
	}

	try {
		logger().information("reverting change " + change->name());
		execute(revertScript);

		logger().notice("change " + change->name() + " successfully reverted",
				__FILE__, __LINE__);
	}
	catch (...) {
		logger().critical("failed to reverted change: " + change->name());
		throw;
	}

}

Path SqitchInitializer::resolve(
		const string &action,
		const SqitchChange::Ptr change,
		const SqitchPlan &plan) const
{
	const SqitchLine::Ptr current = change;

	bool lookup = false;
	for (const auto &line : plan) {
		// skip all before the current change
		if (line == current) {
			lookup = true;
			continue;
		}

		if (!lookup)
			continue;

		SqitchTag::Ptr tag = line.cast<SqitchTag>();
		if (tag.isNull())
			continue;

		Path dir(m_baseDir, action);
		Path file(dir, change->name() + "@" + tag->name() + extension("."));

		File f(file);
		if (f.exists())
			return file;
	}

	Path dir(m_baseDir, action);
	Path file(dir, change->name() + extension("."));

	return file;
}

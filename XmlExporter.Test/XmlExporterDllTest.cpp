#include "stdafx.h"

#include "Plugin/Exporter/CoverageData.hpp"
#include "Plugin/Exporter/ModuleCoverage.hpp"
#include "Plugin/Exporter/FileCoverage.hpp"
#include "Plugin/Exporter/LineCoverage.hpp"
#include "Plugin/OptionsParserException.hpp"

#include "XmlExporter.hpp"

class XmlExporterDllTest : public ::testing::Test {
protected:
	Plugin::IExportPlugin* exporter = nullptr;

	void SetUp() override
	{
		exporter = CreatePlugin();
		ASSERT_TRUE(exporter != nullptr);
	}
};

//export an empty result.
TEST_F(XmlExporterDllTest, ExportEmptyResult)
{
	const std::wstring coverageName = L"";
	constexpr const int exitCode = 0;
	auto out = Plugin::CoverageData(coverageName, exitCode);
	exporter->Export(out, L"coverage.xml");
}

//export empty file result
TEST_F(XmlExporterDllTest, ExportNoLinesFileResult)
{
	const std::wstring coverageName = L"";
	constexpr const int exitCode = 0;
	auto out = Plugin::CoverageData(coverageName, exitCode);
	auto& outModule1 = out.AddModule(L"target1.exe");
	auto& outFile1 = outModule1.AddFile(L"source1.cpp");
	exporter->Export(out, L"coverage.xml");
}

TEST_F(XmlExporterDllTest, ExportWithMerge)
{
	const std::wstring coverageName = L"";
	constexpr const int exitCode = 0;
	auto out = Plugin::CoverageData(coverageName, exitCode);
	auto& outModule1 = out.AddModule(L"target1.exe");
	auto& outFile1 = outModule1.AddFile(L"source1.cpp");
	outFile1.AddLine(18, false);
	outFile1.AddLine(19, true);
	outFile1.AddLine(20, false);
	outFile1.AddLine(21, true);
	auto& outModule2 = out.AddModule(L"target1.exe");
	auto& outFile2 = outModule2.AddFile(L"source1.cpp");
	outFile2.AddLine(18, true);
	outFile2.AddLine(19, false);
	outFile2.AddLine(20, true);
	outFile2.AddLine(21, false);
	outFile2.AddLine(22, true);
	exporter->Export(out, L"coverage.xml");
}

TEST_F(XmlExporterDllTest, ExportBadFilename)
{
	const std::wstring coverageName = L"";
	constexpr const int exitCode = 0;
	auto out = Plugin::CoverageData(coverageName, exitCode);
	exporter->Export(out, L"coverage.xml?q=test");
}

TEST_F(XmlExporterDllTest, CheckArgument)
{
	std::wstring argument;
	argument = L".\\coverage.xml";
	exporter->CheckArgument(argument);
}

TEST_F(XmlExporterDllTest, CheckArgument2)
{
	std::wstring argument;
	argument = L".\\coverage.xml?q=test";
	exporter->CheckArgument(argument);
}

TEST_F(XmlExporterDllTest, CheckArgumentThrow)
{
	std::wstring argument;
	argument = L"..\\";
	ASSERT_THROW(exporter->CheckArgument(argument), Plugin::OptionsParserException);
}

TEST_F(XmlExporterDllTest, GetArgumentHelpDescription)
{
	ASSERT_STREQ(L"output file (optional)", exporter->GetArgumentHelpDescription().c_str());
}

TEST_F(XmlExporterDllTest, GetExportPluginVersion)
{
	ASSERT_EQ(Plugin::CurrentExportPluginVersion, exporter->GetExportPluginVersion());
}

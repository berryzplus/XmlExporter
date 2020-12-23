#include "stdafx.h"
#include "XmlExporter.cpp"

#include <filesystem>
#include <string>
#include <vector>

TEST(XmlExporterTest, EmptyCommandLine)
{
	std::vector<std::filesystem::path> sources = {
		LR"(C:\work)",
		LR"(D:\work)",
	};
	std::filesystem::path path;

	path = std::filesystem::path(LR"(C:\work\test.txt)");
	const auto relative1 = ToRelative(sources.cbegin(), sources.cend(), path);
	ASSERT_STRNE(path.c_str(), relative1.c_str());;

	path = std::filesystem::path(LR"(E:\work\test.txt)");
	const auto relative2 = ToRelative(sources.cbegin(), sources.cend(), path);
	ASSERT_STREQ(path.c_str(), relative2.c_str());;
}

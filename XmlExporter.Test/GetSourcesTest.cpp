#include "stdafx.h"

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

#include "CommandLineToVector.cpp"
#include "GetOptions.cpp"
#include "GetSources.cpp"

TEST(GetSourcesTest, EmptyCommandLine)
{
	auto sources = GetSources(L"");
	ASSERT_TRUE(sources.empty());
}

TEST(GetSourcesTest, SingleSource)
{
	auto sources = GetSources(L"a.exe --sources C:\\work -- test.exe");
	ASSERT_EQ(1, sources.size());
}

TEST(GetSourcesTest, SingleSourceWithPattern)
{
	auto sources = GetSources(L"a.exe --sources C:\\work --sources *.cpp -- test.exe");
	ASSERT_EQ(1, sources.size());
}

#include "stdafx.h"
#include "gmock/gmock.h"

#include <algorithm>
#include <filesystem>

#include "ToRealPath.cpp"

TEST(ToRealPathTest, ExistingPath)
{
	const std::filesystem::path path(__FILE__);
	std::wstring arg(path.wstring());
	std::transform(arg.cbegin(), arg.cend(), arg.begin(), tolower);
	ASSERT_EQ(0, ::_wcsicmp(path.c_str(), arg.c_str()));
	const auto real = ToRealPath(arg);
	ASSERT_EQ(0, ::wcscmp(path.c_str(), real.c_str()));
}

TEST(ToRealPathTest, PathNotExists)
{
	const std::filesystem::path path(__FILE__ ".notFound");
	std::wstring arg(path.wstring());
	const auto real = ToRealPath(arg);
	ASSERT_STREQ(path.c_str(), real.c_str());
}

TEST(ToRealPathTest, NotAbsolutePath)
{
	const std::filesystem::path path(L"test.cpp");
	std::wstring arg(path.wstring());
	const auto real = ToRealPath(arg);
	ASSERT_STREQ(path.c_str(), real.c_str());
}

class MockFile : public winbase::FileInterface {
public:
	MOCK_CONST_METHOD7(Create, HANDLE(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE));
	MOCK_CONST_METHOD4(GetInformationByHandleEx, bool(HANDLE, FILE_INFO_BY_HANDLE_CLASS, LPVOID, DWORD));
	MOCK_CONST_METHOD1(Close, bool(HANDLE));
};

TEST(AbsoluteLocalToRealPathTest, GetInformationFailed)
{
	const std::filesystem::path path(__FILE__);
	std::wstring arg(path.wstring());

	MockFile file;
	using ::testing::_;
	using ::testing::Return;
	EXPECT_CALL(file, Create(_, _, _, _, _, _, _))
		.Times(1)
		.WillOnce(Return((HANDLE)100));
	EXPECT_CALL(file, GetInformationByHandleEx(_, _, _, _))
		.Times(1)
		.WillOnce(Return(false));
	AbsoluteLocalToRealPath(arg, file);
}

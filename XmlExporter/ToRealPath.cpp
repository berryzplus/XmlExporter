// Copyright (C) 2020 berryzplus
 //This file is part of XmlExporter.
//
 //XmlExporter is free software: you can redistribute it and/or modify
 //it under the terms of the GNU General Public License as published by
 //the Free Software Foundation, either version 3 of the License, or
 //(at your option) any later version.
//
 //XmlExporter is distributed in the hope that it will be useful,
 //but WITHOUT ANY WARRANTY; without even the implied warranty of
 //MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 //GNU General Public License for more details.
//
 //You should have received a copy of the GNU General Public License
 //along with XmlExporter.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "ToRealPath.hpp"

#include <stdlib.h>

#include <Windows.h>

#include <stdexcept>
#include <string>
#include <regex>

#include "winbase/File.hpp"

//! a deleter class for HANDLE
class handle_closer
{
	const winbase::FileInterface& m_file;

public:
	explicit handle_closer(const winbase::FileInterface& file)
		: m_file(file)
	{
	}

	void operator()(HANDLE handle) const
	{
		m_file.Close(handle);
	}
};

//! smart pointer for HANDLE
using handleHolder = std::unique_ptr<std::remove_pointer<HANDLE>::type, handle_closer>;

std::wstring AbsoluteLocalToRealPath(const std::wstring_view& unrealpath, const winbase::FileInterface& file)
{
	if (HANDLE hFile = file.Create(
		unrealpath.data(),
		0,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		nullptr,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		nullptr
	); hFile != INVALID_HANDLE_VALUE)
	{
		handleHolder fileHolder(hFile, handle_closer(file));

		constexpr const size_t nMaxChars = 4096;
		const size_t nCount = sizeof(FILE_NAME_INFO) + sizeof(wchar_t[nMaxChars]) / sizeof(FILE_NAME_INFO);
		auto buff = std::vector< FILE_NAME_INFO>(nCount);
		if (auto& info = buff.front(); file.GetInformationByHandleEx(hFile, FileNameInfo, &info, nMaxChars))
		{
			std::wstring realpath(unrealpath.data(), 2);
			if (wchar_t& chDrive = realpath.front(); ::iswlower(chDrive))
			{
				chDrive = ::towupper(chDrive);
			}
			const size_t fileNameLength = info.FileNameLength / sizeof(wchar_t);
			realpath.append(info.FileName, fileNameLength);
			return realpath.data();
		}
	}
	return unrealpath.data();
}

std::wstring ToRealPath(const std::wstring_view& unrealpath)
{
	if (std::regex_search(unrealpath.data(), std::wregex(L"^[A-Za-z]:")))
	{
		winbase::File file;
		return AbsoluteLocalToRealPath(unrealpath, file);
	}
	return unrealpath.data();
}

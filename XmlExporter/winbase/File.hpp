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

#pragma once

#include <WinBase.h>

namespace winbase
{
	struct FileInterface
	{
		virtual ~FileInterface() = default;

		virtual HANDLE Create(
			_In_ LPCWSTR lpFileName,
			_In_ DWORD dwDesiredAccess,
			_In_ DWORD dwShareMode,
			_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			_In_ DWORD dwCreationDisposition,
			_In_ DWORD dwFlagsAndAttributes,
			_In_opt_ HANDLE hTemplateFile
		) const = 0;

		virtual bool GetInformationByHandleEx(
			_In_  HANDLE hFile,
			_In_  FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
			_Out_writes_bytes_(dwBufferSize) LPVOID lpFileInformation,
			_In_  DWORD dwBufferSize
		) const = 0;

		virtual bool Close(
			_In_ _Post_ptr_invalid_ HANDLE hObject
		) const = 0;
	};

	class File : public FileInterface
	{
	public:
		HANDLE Create(
			_In_ LPCWSTR lpFileName,
			_In_ DWORD dwDesiredAccess,
			_In_ DWORD dwShareMode,
			_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			_In_ DWORD dwCreationDisposition,
			_In_ DWORD dwFlagsAndAttributes,
			_In_opt_ HANDLE hTemplateFile
		) const override
		{
			return ::CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
		}

		bool GetInformationByHandleEx(
			_In_  HANDLE hFile,
			_In_  FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
			_Out_writes_bytes_(dwBufferSize) LPVOID lpFileInformation,
			_In_  DWORD dwBufferSize
		) const override
		{
			return ::GetFileInformationByHandleEx(hFile, FileInformationClass, lpFileInformation, dwBufferSize);
		}

		bool Close(
			_In_ _Post_ptr_invalid_ HANDLE hObject
		) const override
		{
			return CloseHandle(hObject);
		}
	};
}

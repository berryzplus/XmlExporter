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

#include "stdafx.h"
#include "GetSources.hpp"

#include <algorithm>
#include <filesystem>
#include <string>
#include <regex>
#include <vector>

#include "GetOptions.hpp"
#include "ToRealPath.hpp"

std::vector<std::filesystem::path> GetSources(const std::wstring_view& cmdLine)
{
	std::vector<std::filesystem::path> vSources;
	if (const auto optionsPtr = GetOptions(cmdLine); optionsPtr)
	{
		const auto& options = optionsPtr.get();
		auto selectedSources = options.GetSourcePatterns().GetSelectedPatterns();
		const auto hasDriveLetter = [](const std::wstring& str) { return !std::regex_search(str.c_str(), std::wregex(L"^[A-Za-z]:")); };
		if (const auto ends = std::remove_if(selectedSources.begin(), selectedSources.end(), hasDriveLetter); ends != selectedSources.end()) {
			selectedSources.erase(ends);
		}
		std::sort(selectedSources.begin(), selectedSources.end(), std::greater<std::wstring>());
		vSources.reserve(selectedSources.size());
		std::transform(selectedSources.cbegin(), selectedSources.cend(), std::back_inserter(vSources), [](const std::wstring& str) { return std::filesystem::path(ToRealPath(str)); });
	}
	return vSources;
}

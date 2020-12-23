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

#include <string>
#include <optional>
#include <filesystem>

#include "Plugin/Exporter/IExportPlugin.hpp"

#ifdef XMLEXPORTER_EXPORTS
#define XMLEXPORTER_API __declspec(dllexport)
#else
#define XMLEXPORTER_API __declspec(dllimport)
#endif

extern "C"
{
	[[nodiscard]] XMLEXPORTER_API Plugin::IExportPlugin* CreatePlugin();
}

#ifdef XMLEXPORTER_EXPORTS
class XmlExporter : public Plugin::IExportPlugin
{
public:
	std::optional<std::filesystem::path> Export(
		const Plugin::CoverageData& coverageData,
		const std::optional<std::wstring>& argument) override;
	void CheckArgument(const std::optional<std::wstring>& argument) override;
	std::wstring GetArgumentHelpDescription() override;
	int GetExportPluginVersion() const override;
};
#endif

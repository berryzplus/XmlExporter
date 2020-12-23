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
#include "GetOptions.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "CppCoverage/OptionsParser.hpp"
#include "CppCoverage/Options.hpp"
#include "CppCoverage/ProgramOptions.hpp"
#include "CppCoverage/ExportOptionParser.hpp"

#include "Exporter/Plugin/ExporterPluginManager.hpp"
#include "Exporter/Plugin/PluginLoader.hpp"

#include "Plugin/Exporter/IExportPlugin.hpp"

#include "Tools/WarningManager.hpp"

#pragma comment(lib, "CppCoverage.lib")
#pragma comment(lib, "Exporter.lib")
#pragma comment(lib, "Plugin.lib")
#pragma comment(lib, "Tools.lib")

std::vector<std::wstring> CommandLineToVector(LPCWSTR lpCmdLine);

std::filesystem::path GetPluginsExportFolder()
{
	return Tools::GetExecutableFolder() / "Plugins" / "Exporter";
}

boost::optional<CppCoverage::Options> GetOptions(const std::wstring_view& cmdLine)
{
	auto warningManager = std::make_shared<Tools::WarningManager>();
	auto exporterPluginManager = Exporter::ExporterPluginManager{
		Exporter::PluginLoader<Plugin::IExportPlugin>{},
		GetPluginsExportFolder() };

	auto optionParsers = std::vector<std::unique_ptr<CppCoverage::IOptionParser>>();
	optionParsers.push_back(std::make_unique<CppCoverage::ExportOptionParser>(
		exporterPluginManager.CreateExportPluginDescriptions()));

	auto vArgList = CommandLineToVector(cmdLine.data());

	auto vArgsBuf = std::vector<std::string>();
	vArgsBuf.reserve(vArgList.size());
	std::transform(vArgList.cbegin(), vArgList.cend(), std::back_inserter(vArgsBuf), [](const std::wstring& str) { return std::filesystem::path(str).string(); });

	auto vArgs = std::vector<const char*>();
	vArgs.reserve(vArgsBuf.size());
	std::transform(vArgsBuf.cbegin(), vArgsBuf.cend(), std::back_inserter(vArgs), [](const std::string& path) { return path.c_str(); });

	CppCoverage::OptionsParser optionsParser{ warningManager, std::move(optionParsers) };
	std::wostringstream emptyOptionsExplanation;
	auto options = optionsParser.Parse(static_cast<int>(vArgs.size()), vArgs.data(), &emptyOptionsExplanation);

	return options;
}

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
#include "XmlExporter.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "GetSources.hpp"
#include "ToRealPath.hpp"

#include "Plugin/Exporter/CoverageData.hpp"
#include "Plugin/Exporter/ModuleCoverage.hpp"
#include "Plugin/Exporter/FileCoverage.hpp"
#include "Plugin/Exporter/LineCoverage.hpp"
#include "Plugin/OptionsParserException.hpp"

#pragma comment(lib, "Plugin.lib")

namespace
{
	namespace property_tree = boost::property_tree;

	//-------------------------------------------------------------------------
	property_tree::wptree& AddChild(property_tree::wptree& tree, const std::wstring& name)
	{
		return tree.add_child(name, property_tree::wptree{});
	}

	//-------------------------------------------------------------------------
	void FillFileTree(
		const Plugin::FileCoverage& file,
		property_tree::wptree& fileTree
	)
	{
		const auto& path = file.GetPath();
		fileTree.put(L"<xmlattr>.path", path.wstring());

		for (const auto& line : file.GetLines())
		{
			property_tree::wptree& lineTree = AddChild(fileTree, L"lineToCover");

			lineTree.put(L"<xmlattr>.lineNumber", std::to_wstring(line.GetLineNumber()));
			lineTree.put(L"<xmlattr>.covered", line.HasBeenExecuted() ? L"true" : L"false");
		}
	}

	//-------------------------------------------------------------------------
	void FillCoverageTree(
		const Plugin::CoverageData& coverageData,
		property_tree::wptree& root
	)
	{
		auto& coverageTree = AddChild(root, L"coverage");
		coverageTree.put(L"<xmlattr>.version", 1);

		for (const auto& module : coverageData.GetModules())
		{
			for (const auto& file : module->GetFiles())
			{
				auto& fileTree = AddChild(coverageTree, L"file");
				FillFileTree(*file, fileTree);
			}
		}
	}

	void Export(
		const Plugin::CoverageData& coverageData,
		std::wostream& ostream
	)
	{
		using namespace property_tree;
		using namespace property_tree::xml_parser;

		wptree root;
		FillCoverageTree(coverageData, root);

		constexpr int indent = 2;
		write_xml(ostream, root, xml_writer_make_settings<wptree::key_type>(' ', indent));
	}

	struct MyFileCoverageData {
		std::unordered_set<int> lineToCover;
		std::unordered_set<int> covered;

		using Me = MyFileCoverageData;
		MyFileCoverageData() = default;
		MyFileCoverageData(const Me&) = default;
		MyFileCoverageData(Me&& other) noexcept = default;
		~MyFileCoverageData() = default;
	};

	void ConvertLineCoverageToDataMap(
		const Plugin::FileCoverage& file,
		const std::vector<Plugin::LineCoverage>& lines,
		std::map<std::filesystem::path, MyFileCoverageData>& dataMap)
	{
		if (lines.empty())
		{
			return;
		}

		const auto& filename = file.GetPath();
		if (dataMap.find(filename) == dataMap.cend())
		{
			dataMap.try_emplace(filename, MyFileCoverageData());
		}

		auto& fileData = dataMap[filename];
		auto& linesToCover = fileData.lineToCover;
		auto& coveredLines = fileData.covered;

		for (const auto& line : lines)
		{
			const int lineNumber = line.GetLineNumber();

			if (linesToCover.find(lineNumber) == linesToCover.cend())
			{
				linesToCover.insert(lineNumber);
			}

			if (line.HasBeenExecuted() && coveredLines.find(lineNumber) == coveredLines.cend())
			{
				coveredLines.insert(lineNumber);
			}
		}
	}

	auto ConvertToDataMap(const Plugin::CoverageData& coverageData) -> std::map<std::filesystem::path, MyFileCoverageData>
	{
		std::map<std::filesystem::path, MyFileCoverageData> dataMap;

		for (const auto& module : coverageData.GetModules())
		{
			const auto& files = module->GetFiles();
			for (const auto& file : files)
			{
				const auto& lines = file->GetLines();
				ConvertLineCoverageToDataMap(*file, lines, dataMap);
			}
		}
		return dataMap;
	}

	template<typename FwdIter>
	std::filesystem::path ToRelative(FwdIter first, FwdIter last, const std::filesystem::path& realpath)
	{
		auto matchedSource = std::find_if(first, last, [realpath](const auto& source) {return source < realpath && 0 == ::wcsncmp(source.c_str(), realpath.c_str(), source.wstring().length());});
		if (matchedSource == last)
		{
			return realpath;
		}
		return realpath.lexically_proximate(*matchedSource);
	}

	Plugin::CoverageData MergeCoverageData(const Plugin::CoverageData& coverageData)
	{
		auto out = Plugin::CoverageData(coverageData.GetName(), coverageData.GetExitCode());
		if (auto dataMap = ConvertToDataMap(coverageData); !dataMap.empty())
		{
			auto sources = GetSources(::GetCommandLineW());

			auto& outModule = out.AddModule(L"target.exe");
			for (auto [filename, filedata] : dataMap)
			{
				auto newFilename = ToRelative(sources.cbegin(), sources.cend(), ToRealPath(filename.wstring()));

				auto& outFile = outModule.AddFile(newFilename);
				for (const auto& lineToCover : filedata.lineToCover)
				{
					const auto found = filedata.covered.find(lineToCover) != filedata.covered.cend();
					outFile.AddLine(lineToCover, found);
				}
			}
		}

		return out;
	}
}

#ifdef XMLEXPORTER_EXPORTS

std::optional<std::filesystem::path> XmlExporter::Export(
	const Plugin::CoverageData& coverageData,
	const std::optional<std::wstring>& argument
)
{
	std::filesystem::path output = argument.value_or( L"coverage.xml" );
	std::wofstream ofs{ output };

	if (!ofs)
	{
		return std::nullopt;
	}

	auto mergedData = MergeCoverageData(coverageData);
	::Export(mergedData, ofs);

	return output;
}

void XmlExporter::CheckArgument(const std::optional<std::wstring>& argument)
{
	// Try to check if the argument is a file.
	if (argument && !std::filesystem::path{ *argument }.has_filename())
		throw Plugin::OptionsParserException("Invalid argument for SonarQube export.");
}

std::wstring XmlExporter::GetArgumentHelpDescription()
{
	return L"output file (optional)";
}

int XmlExporter::GetExportPluginVersion() const
{
	return Plugin::CurrentExportPluginVersion;
}

extern "C"
{
	[[nodiscard]] XMLEXPORTER_API Plugin::IExportPlugin* CreatePlugin()
	{
		return new XmlExporter();
	}
}

#endif

# XmlExporter

[![License](https://img.shields.io/github/license/berryzplus/XmlExporter)](https://github.com/berryzplus/XmlExporter/blob/main/LICENSE)
[![Releases](https://img.shields.io/github/downloads/berryzplus/XmlExporter/total.svg)](https://github.com/berryzplus/XmlExporter/releases "All Releases")
[![build](https://github.com/berryzplus/XmlExporter/workflows/build/badge.svg)](https://github.com/berryzplus/XmlExporter/actions?query=workflow%3Abuild)
[![SonarCloud](https://sonarcloud.io/api/project_badges/measure?project=berryzplus_XmlExporter&metric=alert_status)](https://sonarcloud.io/dashboard?id=berryzplus_XmlExporter)

XmlExporter is a Plugin for [OpenCppCoverage](https://github.com/OpenCppCoverage/OpenCppCoverage)(requires OpenCppCoverage 0.9.8.0 or greater).

# How to use XmlExporter
To use XmlExporter, copy the plugin dll into `{OPENCPPCOVERAGE_INSTALLATION_FOLDER}\Plugins\Exporter`.
I recommend copying and renaming `XmlExporter.dll` as follows.
```powershell
Copy-Item -Path XmlExporter.dll -Destination "C:\Program Files\OpenCppCOverage\Plugins\Exporter\xml.dll"
```

To check the installation, run the command `OpenCppCoverage --help`. In the description of `--export_type` you should see the name of the plugin. 

If you see `xml` in `--export_type` list, you can use XmlExporter.

```cmd
OpenCppCoverage ^
  --export_type xml:coverage.xml ^
  --sources C:\path\to\source ^
  -- ^
  YourProgram.exe
```

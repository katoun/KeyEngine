/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace editor
{
	namespace fs = std::filesystem;

	fs::path ExecutablePath();
	fs::path SdkPath();
	fs::path ToolPath(const std::string& name);
	fs::path EditorConfigPath();

	std::string ShellQuote(const fs::path& path);
	std::string ShellQuote(const std::string& value);

	bool LoadProjectFile(const fs::path& project_file, fs::path& project_path, std::string& project_name);
	bool RunCommand(const std::string& command, std::vector<std::string>& log);
}

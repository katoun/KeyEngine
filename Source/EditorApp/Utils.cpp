/* Copyright (c) 2016 Catalin-Alexandru Nastase
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
* to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, 
* sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
* ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
* THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "Utils.h"

#include <array>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>

#if defined(__linux__)
#include <limits.h>
#include <unistd.h>
#endif

namespace editor
{
	fs::path ExecutablePath()
	{
#if defined(__linux__)
		std::array<char, PATH_MAX> buffer{};
		ssize_t size = readlink("/proc/self/exe", buffer.data(), buffer.size() - 1);
		if (size > 0)
		{
			buffer[static_cast<size_t>(size)] = '\0';
			return fs::canonical(buffer.data());
		}
#endif
		return fs::current_path();
	}

	fs::path SdkPath()
	{
		fs::path path = ExecutablePath().parent_path();
		if (path.filename() == "Linux")
			path = path.parent_path();
		if (path.filename() == "Binaries")
			path = path.parent_path();
		return fs::canonical(path);
	}

	fs::path ToolPath(const std::string& name)
	{
#if defined(_WIN32)
		return SdkPath() / "Binaries" / "Windows" / (name + ".exe");
#else
		return SdkPath() / "Binaries" / "Linux" / name;
#endif
	}

	fs::path EditorConfigPath()
	{
		return SdkPath() / "Config" / "KeyEditor";
	}

	std::string ShellQuote(const fs::path& path)
	{
		std::string input = path.string();
		std::string result = "'";
		for (char c : input)
		{
			if (c == '\'')
				result += "'\\''";
			else
				result += c;
		}
		result += "'";
		return result;
	}

	std::string ShellQuote(const std::string& value)
	{
		return ShellQuote(fs::path(value));
	}

	bool LoadProjectFile(const fs::path& project_file, fs::path& project_path, std::string& project_name)
	{
		if (!fs::exists(project_file))
			return false;

		project_path = fs::canonical(project_file.parent_path());
		project_name = project_file.stem().string();
		return true;
	}

	namespace
	{
		std::string ReadCommandOutput(const std::string& command, int& exit_code)
		{
			std::string output;
			std::array<char, 256> buffer{};

			FILE* pipe = popen((command + " 2>&1").c_str(), "r");
			if (pipe == nullptr)
			{
				exit_code = -1;
				return "Unable to start command.\n";
			}

			while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
				output += buffer.data();

			exit_code = pclose(pipe);
			return output;
		}
	}

	bool RunCommand(const std::string& command, std::vector<std::string>& log)
	{
		log.emplace_back("> " + command);

		int exit_code = 0;
		std::string output = ReadCommandOutput(command, exit_code);
		std::cout << output;

		std::istringstream stream(output);
		std::string line;
		while (std::getline(stream, line))
			log.emplace_back(line);

		if (exit_code != 0)
		{
			log.emplace_back("Command failed with exit code " + std::to_string(exit_code) + ".");
			return false;
		}

		return true;
	}
}

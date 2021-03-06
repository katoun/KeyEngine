/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ParserConfig.h>
#include <ParserOptions.h>
#include <Parser.h>
#include <Core/Utils.h>

#include <chrono>
#include <string>
#include <iostream>

int main(int argc, char *argv[])
{
	auto start_time = std::chrono::system_clock::now();

	std::string current_path = filesystem::current_path().string();

	std::cout << "Current path: " << current_path << std::endl;

	try
	{
		cmd_parser::CmdLine cmd_line("Reflection Parser", ' ', "0.0.1");

		cmd_parser::ValueArg<std::string> name_arg("n", "project-name", "Name of the project", true, "Project", "string");
		cmd_line.add(name_arg);

		cmd_parser::ValueArg<std::string> path_arg("p", "project-path", "Path of the project", true, "", "string");
		cmd_line.add(path_arg);

		cmd_line.parse(argc, argv);

		parser::ParserOptions options;
		options.ProjectName = name_arg.getValue();
		options.ProjectPath = path_arg.getValue();

		options.SDKPath = core::system::GetEnv("KEY_ENGINE_SDK_PATH");

		std::cout << "ProjectName: " << options.ProjectName << " ProjectPath: " << options.ProjectPath << std::endl;

		parser::Parser::Instance().SetOptions(options);
		parser::Parser::Instance().Parse();
	}
	catch (cmd_parser::ArgException &e)  // catch any exceptions
	{
		std::cerr << "Error: " << e.error() << " for arg " << e.argId() << std::endl;

		exit(EXIT_FAILURE);
	}
	catch (std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;

		exit(EXIT_FAILURE);
	}
	catch (...)
	{
		std::cerr << "Error: Unhandled exception occurred!" << std::endl;

		exit(EXIT_FAILURE);
	}

	auto duration = std::chrono::system_clock::now() - start_time;

	std::cout << "Completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	return EXIT_SUCCESS;
}
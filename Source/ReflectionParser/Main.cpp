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
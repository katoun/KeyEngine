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

#include <Creator.h>
#include <Utils.h>
#include <Core/Utils.h>

#include <stdexcept>

namespace creator
{
	void Creator::SetOptions(const CreatorOptions &options)
	{
		m_Options = options;

		auto sdk_path = filesystem::path(m_Options.SDKPath);
		sdk_path = filesystem::canonical(sdk_path);

		m_TemplatePath = (sdk_path / "Content" / "Templates" / "Project");

		auto project_path = filesystem::path(m_Options.ProjectPath);
		project_path = filesystem::canonical(project_path);

		m_ProjectPath = (project_path / m_Options.ProjectName);
	}

	void Creator::Create(void)
	{
		filesystem::copy(m_TemplatePath, m_ProjectPath, filesystem::copy_options::recursive);
		
		RemoveMustacheFiles();

		BuildProjectFile();
		BuildCMakeProjectFile();
	}

	void Creator::RemoveMustacheFiles()
	{
		filesystem::remove(m_ProjectPath / templates::ProjectFile);
		filesystem::remove(m_ProjectPath / templates::CMakeProject);
	}

	void Creator::BuildProjectFile()
	{
		auto output_file_mustache = LoadTemplate(filesystem::path("") ,templates::ProjectFile);

		if (!output_file_mustache.isValid())
		{
			std::stringstream error;

			error << "Unable to compile output project file template." << std::endl;
			error << output_file_mustache.errorMessage();

			throw std::runtime_error(error.str());
		}

		mustache::Data OutputFileData{ mustache::Data::Type::Object };

		OutputFileData["ProjectName"] = m_Options.ProjectName;

		std::string output_data = output_file_mustache.render(OutputFileData);
		std::string file_name = m_Options.ProjectName + ".keyproject";

		std::string file_path = (m_ProjectPath / file_name).string();

		WriteText(file_path, output_data);
	}

	void Creator::BuildCMakeProjectFile()
	{
		auto output_file_mustache = LoadTemplate(filesystem::path(""), templates::CMakeProject);

		if (!output_file_mustache.isValid())
		{
			std::stringstream error;

			error << "Unable to compile output CMake project file template." << std::endl;
			error << output_file_mustache.errorMessage();

			throw std::runtime_error(error.str());
		}

		mustache::Data OutputFileData{ mustache::Data::Type::Object };

		OutputFileData["ProjectName"] = m_Options.ProjectName;

		std::string output_data = output_file_mustache.render(OutputFileData);
		std::string file_path = (m_ProjectPath / "CMakeLists.txt").string();

		WriteText(file_path, output_data);
	}

	mustache Creator::LoadTemplate(const filesystem::path& subpath, const std::string &name) const
	{
		std::string template_path = "";
		if (!subpath.empty())
		{
			template_path = (filesystem::path(m_TemplatePath) / subpath / name).string();
		}
		else
		{
			template_path = filesystem::path(m_TemplatePath).append(name).string();
		}

		try
		{
			std::string text;

			LoadText(template_path, text);

			return text;
		}
		catch (std::exception &e)
		{
			std::cerr << "Error: Unable to load template ";
			std::cerr << name << "." << std::endl;
			std::cerr << "-- " << e.what() << std::endl;

			exit(EXIT_FAILURE);
		}

		return{ "" };
	}

	Creator &Creator::Instance(void)
	{
		static Creator instance;

		return instance;
	}
}

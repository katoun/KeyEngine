/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Creator.h>
#include <Utils.h>
#include <Core/Utils.h>

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
		BuildVisualStudioProjectFile();
	}

	void Creator::RemoveMustacheFiles()
	{
		filesystem::remove(m_ProjectPath / templates::ProjectFile);
		filesystem::remove(m_ProjectPath / "Source" / templates::VisualStudioProject);
	}

	void Creator::BuildProjectFile()
	{
		auto& output_file_mustache = LoadTemplate(filesystem::path("") ,templates::ProjectFile);

		if (!output_file_mustache.isValid())
		{
			std::stringstream error;

			error << "Unable to compile output project file template." << std::endl;
			error << output_file_mustache.errorMessage();

			throw std::exception(error.str().c_str());
		}

		mustache::Data OutputFileData{ mustache::Data::Type::Object };

		OutputFileData["ProjectName"] = m_Options.ProjectName;

		std::string output_data = output_file_mustache.render(OutputFileData);
		std::string file_name = m_Options.ProjectName + ".keyproject";

		std::string file_path = (m_ProjectPath / file_name).string();

		WriteText(file_path, output_data);
	}

	void Creator::BuildVisualStudioProjectFile()
	{
		auto& output_file_mustache = LoadTemplate(filesystem::path("Source"), templates::VisualStudioProject);

		if (!output_file_mustache.isValid())
		{
			std::stringstream error;

			error << "Unable to compile output visual studio solution template." << std::endl;
			error << output_file_mustache.errorMessage();

			throw std::exception(error.str().c_str());
		}

		mustache::Data OutputFileData{ mustache::Data::Type::Object };

		OutputFileData["ProjectName"] = m_Options.ProjectName;

		std::string output_data = output_file_mustache.render(OutputFileData);
		std::string file_path = (m_ProjectPath / "Source" / "Project.vcxproj").string();

		WriteText(file_path, output_data);
	}

	mustache Creator::LoadTemplate(const filesystem::path& subpath, const std::string &name) const
	{
		std::string template_path = "";
		if (!subpath.empty())
		{
			template_path = filesystem::path(m_TemplatePath).append(subpath).append(name).string();
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
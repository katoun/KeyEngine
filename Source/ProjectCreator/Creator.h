/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <CreatorConfig.h>
#include <CreatorOptions.h>
#include <Templates.h>

namespace creator
{
	class Creator
	{
	public:

		void SetOptions(const CreatorOptions &options);
		void Create(void);

		mustache LoadTemplate(const std::string &name) const;

		static Creator &Instance(void);

	private:

		void RemoveMustacheFiles();
		void RenameVisualStudioFiles();
		void BuildProjectFile();
		void BuildVisualStudioSolution();

		CreatorOptions m_Options;

		filesystem::path m_TemplatePath;
		filesystem::path m_ProjectPath;
	};
}
/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <filesystem>

namespace editor
{
	class ContentBrowserWidget
	{
	public:
		void ProjectChanged(const std::filesystem::path& path);
		void Draw();

	private:
		void DrawPath(const std::filesystem::path& path);
		const char* IconNameForFile(const std::filesystem::path& path) const;

		std::filesystem::path m_ProjectPath;
	};
}

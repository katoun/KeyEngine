/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Utils.h>

#include <iostream>
#include <fstream>

namespace creator
{
	mustache::Data::Type ToMustache(bool value)
	{
		return value ? mustache::Data::Type::True : mustache::Data::Type::False;
	}

	filesystem::path GetCanonicalPath(const filesystem::path& p)
	{
		filesystem::path source(p.is_absolute() ? p : filesystem::absolute(p));
		filesystem::path root(p.root_path());
		filesystem::path result;

		std::error_code e_code;

		filesystem::file_status stat(filesystem::status(source, e_code));
		if (stat.type() == filesystem::file_type::not_found)
		{
			if (e_code != std::error_code())
			{
				throw filesystem::filesystem_error("utils::canonical(p): No such file or directory!");
			}

			return result;
		}

		bool scan(true);
		while (scan)
		{
			scan = false;
			result.clear();
			for (auto it = source.begin(); it != source.end(); ++it)
			{
				if (it->c_str()[0] == _FS_PERIOD
					&& it->c_str()[1] == (filesystem::path::value_type)0)
					continue;
				if (it->c_str()[0] == _FS_PERIOD
					&& it->c_str()[1] == _FS_PERIOD
					&& it->c_str()[2] == (filesystem::path::value_type)0)
				{
					if (result != root)
						result.remove_filename();
					continue;
				}

				result /= *it;


				if (filesystem::is_symlink(result))
				{
					filesystem::path link = filesystem::read_symlink(result, e_code);
					if (e_code != std::error_code())
						return filesystem::path();

					result.remove_filename();

					if (link.is_absolute())
					{
						for (++it; it != source.end(); ++it)
							link /= *it;
						source = link;
					}
					else // link is relative
					{
						filesystem::path new_source(result);
						new_source /= link;
						for (++it; it != source.end(); ++it)
							new_source /= *it;
						source = new_source;
					}

					scan = true;  // symlink causes scan to be restarted
					break;
				}
			}
		}
		return result;
	}

	void LoadText(const std::string &filename, std::string &output)
	{
		std::ifstream input;

		input.open(filename);

		if (!input)
		{
			std::stringstream error;

			error << "Unable to open file \"" << filename << "\" for reading." << std::endl;

			throw std::exception(error.str().c_str());
		}

		input.seekg(0, std::ios::end);

		output.reserve(static_cast<std::string::size_type>(input.tellg()));

		input.seekg(0, std::ios::beg);

		// disambiguate function declaration
		output.assign((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

		input.close();
	}

	void WriteText(const std::string &filename, const std::string &text)
	{
		std::ofstream output;
		output.open(filename);

		if (!output)
		{
			std::stringstream error;

			error << "Unable to open file \"" << filename << "\" for writing." << std::endl;

			throw std::exception(error.str().c_str());
		}

		output << text;

		output.close();
	}
}
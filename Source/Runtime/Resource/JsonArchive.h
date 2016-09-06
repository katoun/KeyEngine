/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <RuntimeConfig.h>
#include <Resource/Archive.h>

#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/reader.h>
#include <rapidjson/document.h>
#include <base64.hpp>

namespace resource
{
	namespace serialize
	{
		class RUNTIME_API JsonArchive : public Archive
		{
		public:

			JsonArchive();

			template <class Type>
			void Read(std::ifstream stream, Type& object);

			template <class Type>
			void Write(std::ofstream stream, Type& object);

		private:

		};

		template <class Type>
		void JsonArchive::Read(std::ifstream stream, Type& object)
		{
			//OStreamWrapper stream_wrapper(stream);
			//Reader reader;

			//reader.Parse(stream_wrapper, object);

			//TODO!!!
		}

		template <class Type>
		void JsonArchive::Write(std::ofstream stream, Type& object)
		{
			//StringBuffer string_buffer;
			//PrettyWriter<StringBuffer> writer(string_buffer);

			//writer.StartObject();

			//TODO!!!

			//writer.EndObject();
		}
	}
}

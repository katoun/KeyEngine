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

#pragma once

#include <RuntimeConfig.h>
#include <Resource/Archive.h>

#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/reader.h>
#include <rapidjson/document.h>
#include <base64.hpp>

#include <fstream>

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

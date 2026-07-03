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

#include <ParserConfig.h>
#include <ParserOptions.h>
#include <Cursor.h>
#include <Namespace.h>
#include <Templates.h>

#include <memory>
#include <vector>
#include <unordered_map>

namespace parser
{
	class Class;

	class Parser
	{
	public:

		Parser(void);
		~Parser();

		void SetOptions(const ParserOptions &options);
		void Parse(void);

		mustache LoadTemplate(const std::string &name) const;
		mustache::Data::PartialType LoadTemplatePartial(const std::string &name) const;

		static Parser &Instance(void);

	private:

		class ClangIndex
		{
		public:
			ClangIndex(void) = default;
			explicit ClangIndex(CXIndex handle);
			~ClangIndex(void);

			ClangIndex(const ClangIndex&) = delete;
			ClangIndex& operator=(const ClangIndex&) = delete;

			ClangIndex(ClangIndex&& other) noexcept;
			ClangIndex& operator=(ClangIndex&& other) noexcept;

			CXIndex Get(void) const;
			void Reset(CXIndex handle = nullptr);

		private:
			CXIndex m_Handle = nullptr;
		};

		class TranslationUnit
		{
		public:
			TranslationUnit(void) = default;
			explicit TranslationUnit(CXTranslationUnit handle);
			~TranslationUnit(void);

			TranslationUnit(const TranslationUnit&) = delete;
			TranslationUnit& operator=(const TranslationUnit&) = delete;

			TranslationUnit(TranslationUnit&& other) noexcept;
			TranslationUnit& operator=(TranslationUnit&& other) noexcept;

			CXTranslationUnit Get(void) const;
			void Reset(CXTranslationUnit handle = nullptr);

		private:
			CXTranslationUnit m_Handle = nullptr;
		};

		void BuildClasses(const Cursor &cursor, Namespace &currentNamespace);

		void BuildOutputSourceFile();

		mustache::Data CompileClassDefinition(void) const;

		ParserOptions m_Options;

		ClangIndex m_Index;
		TranslationUnit m_TranslationUnit;

		std::string m_TemplatesPath;
		std::string m_InputSourceFile;
		std::string m_OutputSourceFile;
		std::vector<std::string> m_Arguments;

		std::vector<std::unique_ptr<Class>> m_Classes;

		mutable std::unordered_map<std::string, std::string> m_TemplatePartialCache;
	};
}

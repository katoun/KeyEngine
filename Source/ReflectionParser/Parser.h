/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <ParserConfig.h>
#include <ParserOptions.h>
#include <Cursor.h>
#include <Namespace.h>
#include <Templates.h>

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

		void BuildClasses(const Cursor &cursor, Namespace &currentNamespace);

		void BuildOutputSourceFile();

		mustache::Data CompileClassDeclaration(void) const;
		mustache::Data CompileClassDefinition(void) const;

		ParserOptions m_Options;

		CXIndex m_Index;
		CXTranslationUnit m_TranslationUnit;

		std::string m_TemplatesPath;
		std::string m_InputSourceFile;
		std::string m_OutputSourceFile;
		std::vector<std::string> m_Arguments;

		std::vector<Class*> m_Classes;

		mutable std::unordered_map<std::string, std::string> m_TemplatePartialCache;
	};
}
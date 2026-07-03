/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <Parser.h>
#include <Class.h>
#include <Utils.h>
#include <Core/Utils.h>

#include <stdexcept>

#define RECURSE_NAMESPACES(kind, cursor, method, ns) \
    if (kind == CXCursor_Namespace)                  \
    {                                                \
        auto displayName = cursor.GetDisplayName( ); \
        if (!displayName.empty( ))                   \
        {                                            \
            ns.emplace_back( displayName );          \
            method( cursor, ns );                    \
            ns.pop_back( );                          \
        }                                            \
    }                                                \

namespace parser
{
	Parser::ClangIndex::ClangIndex(CXIndex handle)
		: m_Handle(handle)
	{}

	Parser::ClangIndex::~ClangIndex(void)
	{
		Reset();
	}

	Parser::ClangIndex::ClangIndex(ClangIndex&& other) noexcept
		: m_Handle(other.m_Handle)
	{
		other.m_Handle = nullptr;
	}

	Parser::ClangIndex& Parser::ClangIndex::operator=(ClangIndex&& other) noexcept
	{
		if (this != &other)
		{
			Reset();
			m_Handle = other.m_Handle;
			other.m_Handle = nullptr;
		}

		return *this;
	}

	CXIndex Parser::ClangIndex::Get(void) const
	{
		return m_Handle;
	}

	void Parser::ClangIndex::Reset(CXIndex handle)
	{
		if (m_Handle != nullptr)
		{
			clang_disposeIndex(m_Handle);
		}

		m_Handle = handle;
	}

	Parser::TranslationUnit::TranslationUnit(CXTranslationUnit handle)
		: m_Handle(handle)
	{}

	Parser::TranslationUnit::~TranslationUnit(void)
	{
		Reset();
	}

	Parser::TranslationUnit::TranslationUnit(TranslationUnit&& other) noexcept
		: m_Handle(other.m_Handle)
	{
		other.m_Handle = nullptr;
	}

	Parser::TranslationUnit& Parser::TranslationUnit::operator=(TranslationUnit&& other) noexcept
	{
		if (this != &other)
		{
			Reset();
			m_Handle = other.m_Handle;
			other.m_Handle = nullptr;
		}

		return *this;
	}

	CXTranslationUnit Parser::TranslationUnit::Get(void) const
	{
		return m_Handle;
	}

	void Parser::TranslationUnit::Reset(CXTranslationUnit handle)
	{
		if (m_Handle != nullptr)
		{
			clang_disposeTranslationUnit(m_Handle);
		}

		m_Handle = handle;
	}

	Parser::Parser(void) = default;
	Parser::~Parser() = default;

	void Parser::SetOptions(const ParserOptions &options)
	{
		m_Options = options;

		auto sdk_path = filesystem::path(m_Options.SDKPath);
		sdk_path = filesystem::canonical(sdk_path);
		auto runtime_source = (sdk_path / "Source" / "Runtime").string();
		auto editor_source = (sdk_path / "Source" / "Editor").string();
		auto dependencies_path = sdk_path / "Build" / "Linux-Debug" / "_deps";
		auto glm_source = (dependencies_path / "glm-src").string();
		auto rapidjson_source = (dependencies_path / "rapidjson-src" / "include").string();
		auto base64_source = (dependencies_path / "cereal-src" / "include" / "cereal" / "external").string();

		auto project_path = filesystem::path(m_Options.ProjectPath);
		project_path = filesystem::canonical(project_path);
		auto project_source = (project_path / "Source").string();
		m_InputSourceFile = (project_path / "Source" / "Project.h").string();

		m_OutputSourceFile = (project_path / "Source" / ".Generated" / "ReflectionData.Generated.h").string();

		m_TemplatesPath = (sdk_path / "Content" / "Templates" / "ReflectionParser").string();

		m_Arguments =
		{ {
			"-x",
			"c++",
			"-std=c++20",
			"-D__REFLECTION_PARSER__",
			"-I" + runtime_source,
			"-I" + editor_source,
			"-I" + glm_source,
			"-I" + rapidjson_source,
			"-I" + base64_source,
			"-I" + project_source
		} };

	}

	void Parser::Parse(void)
	{
		std::vector<const char *> arguments;
		for (auto &argument : m_Arguments)
		{
			arguments.emplace_back(argument.c_str());
		}

		m_Classes.clear();
		m_TranslationUnit.Reset();
		m_Index.Reset(clang_createIndex(true, false));
		m_TranslationUnit.Reset(clang_createTranslationUnitFromSourceFile(m_Index.Get(), m_InputSourceFile.c_str(), static_cast<int>(arguments.size()), arguments.data(), 0, nullptr));

		auto cursor = clang_getTranslationUnitCursor(m_TranslationUnit.Get());

		Namespace tempNamespace;

		BuildClasses(cursor, tempNamespace);

		tempNamespace.clear();

		//TODO!!!

		BuildOutputSourceFile();
	}

	mustache Parser::LoadTemplate(const std::string &name) const
	{
		auto path = filesystem::path(m_TemplatesPath).append(name).string();

		try
		{
			std::string text;

			LoadText(path, text);

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

	mustache::Data::PartialType Parser::LoadTemplatePartial(const std::string &name) const
	{
		auto path = filesystem::path(m_TemplatesPath).append(name).string();

		try
		{
			auto partialLoader = [this, path]() -> std::string
			{
				auto cache = m_TemplatePartialCache.find(path);

				if (cache == m_TemplatePartialCache.end())
				{
					std::string text;

					LoadText(path, text);

					m_TemplatePartialCache[path] = text;

					return text;
				}

				return (*cache).second;
			};

			return partialLoader;
		}

		catch (std::exception &e)
		{
			std::cerr << "Error: Unable to load template partial ";
			std::cerr << name << "." << std::endl;
			std::cerr << "-- " << e.what() << std::endl;

			exit(EXIT_FAILURE);
		}

		return nullptr;
	}

	void Parser::BuildClasses(const Cursor &cursor, Namespace &currentNamespace)
	{
		for (auto &child : cursor.GetChildren())
		{
			auto kind = child.GetKind();

			// actual definition and a class or struct
			if (child.IsDefinition() && (kind == CXCursor_ClassDecl || kind == CXCursor_StructDecl))
			{
				m_Classes.emplace_back(std::make_unique<Class>(child, currentNamespace));
			}

			RECURSE_NAMESPACES(kind, child, BuildClasses, currentNamespace);
		}
	}

	void Parser::BuildOutputSourceFile()
	{
		auto output_file_mustache = LoadTemplate(templates::ReflectionData);

		if (!output_file_mustache.isValid())
		{
			std::stringstream error;

			error << "Unable to compile output file template." << std::endl;
			error << output_file_mustache.errorMessage();

			throw std::runtime_error(error.str());
		}

		mustache::Data OutputFileData{ mustache::Data::Type::Object };
		OutputFileData["ProjectName"] = m_Options.ProjectName;

		OutputFileData["ClassDeclaration"] = CompileClassDeclaration();
		OutputFileData["ClassDefinition"] = CompileClassDefinition();

		std::string output = output_file_mustache.render(OutputFileData);

		WriteText(m_OutputSourceFile, output);
	}

	mustache::Data Parser::CompileClassDeclaration(void) const
	{
		mustache::Data data{ mustache::Data::Type::List };

		for (const auto& klass : m_Classes)
		{
			if (klass->IsValid())
			{
				data << klass->CompileDeclaration();
			}
		}

		return data;
	}

	mustache::Data Parser::CompileClassDefinition(void) const
	{
		mustache::Data data{ mustache::Data::Type::List };

		for (const auto& klass : m_Classes)
		{
			if (klass->IsValid())
			{
				data << klass->CompileDefinition();
			}
		}

		return data;
	}

	Parser &Parser::Instance(void)
	{
		static Parser instance;

		return instance;
	}
}

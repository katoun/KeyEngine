// Copyright (c) 2016 Catalin Alexandru Nastase

#pragma once

#include <RuntimeConfig.h>

#include <clang-c/Index.h>

#include <experimental/filesystem>
#include <tclap/CmdLine.h>

#include <Mustache.hpp>

using mustache = Kainjow::BasicMustache<std::string>;

namespace filesystem = std::experimental::filesystem;
namespace cmd_parser = TCLAP;

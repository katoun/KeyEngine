# KeyEngine Codex Context

## Project Summary

KeyEngine is a small C++ game engine with:

- `Runtime`: core/game/reflection/resource runtime library.
- `Editor`: editor support library that dynamically loads project editor modules.
- `EditorApp`: Qt desktop editor application.
- `ProjectCreator`: command-line project generator.
- `ReflectionParser`: libclang-based reflection metadata generator.

The original project was Windows/Visual Studio based. The current working Linux setup uses CMake + Ninja + Clang on Linux Mint.

## Current Build System

Use CMake from the repository root.

Configure:

```bash
cmake -S . -B Build/Linux-Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=clang-20 -DCMAKE_CXX_COMPILER=clang++-20 -DLLVM_DIR=/usr/lib/llvm-20/lib/cmake/llvm
```

Build:

```bash
cmake --build Build/Linux-Debug --parallel
```

VSCode `Ctrl+Shift+B` runs the default task `cmake: build debug`, which first runs `cmake: configure debug` via `dependsOn`.

VSCode files:

- `.vscode/tasks.json`: configure/build tasks.
- `.vscode/settings.json`: CMake extension build directory and compiler settings.
- `.vscode/launch.json`: debug launch for `Binaries/Linux/KeyEditor`.

## Build Layout

Use uppercase `Build/` consistently.

Engine CMake workspace:

```text
Build/Linux-Debug
```

Engine shared libraries:

```text
Build/Linux/libKeyRuntime_d.so
Build/Linux/libKeyEditor_d.so
```

Engine executables:

```text
Binaries/Linux/KeyEditor
Binaries/Linux/ProjectCreator
Binaries/Linux/ReflectionParser
```

Generated project CMake workspace:

```text
<Project>/Build/Linux-Debug
```

Generated project editor module output:

```text
<Project>/Build/Linux/<ProjectName>-EditorModule_d.so
```

The module output path matches `Source/Editor/Editor/Module.cpp`.

## Running

Run the editor from the repo root:

```bash
./Binaries/Linux/KeyEditor
```

If needed:

```bash
LD_LIBRARY_PATH="$PWD/Build/Linux" ./Binaries/Linux/KeyEditor
```

In VSCode, press `F5` after building.

## Dependencies

Vendored source dependencies kept in `Dependencies/`:

- `qtpropertybrowser`

CMake-managed dependency declarations and local include-target setup live in:

```text
Dependencies/CMakeLists.txt
```

The root `CMakeLists.txt` imports that file with `add_subdirectory(Dependencies)`.
Downloaded sources go into the active build tree, usually:

```text
Build/Linux-Debug/_deps
```

CMake-managed dependencies:

- `cereal` 1.3.2, used only for `cereal/external/base64.hpp`.
- Dear ImGui `docking` branch for the experimental `EditorApp2` UI.
- `glm` 0.9.9.8.
- `kainjow/Mustache` commit `506c6d3`, kept at the older API used by the tools.
- `RapidJSON` 1.1.0.
- SDL 3.4.12 for the experimental `EditorApp2` windowing/input.
- `TCLAP` 1.2.5.

Vendored source dependency kept in the repo:

- `qtpropertybrowser`: used by the primary Qt editor app inspector/property UI.

Dependencies removed from the repo because the Linux/CMake build does not use them:

- `LLVM`: now uses system LLVM/libclang from apt.
- `assimp`: unused old Windows binaries/libs.
- `bgfx`: unused old Windows binaries/libs.
- `base64`, `glm`, `mustache`, `rapidjson`, `tclap`: now fetched by CMake.

Expected system packages include CMake, Ninja, Clang 20, LLVM/libclang 20 development packages, Qt 5 development packages, Vulkan headers/loader, and pthreads.

Editor executables:

- `KeyEditor`: primary Qt editor app built from `Source/EditorApp`.
- `KeyEditor2`: experimental Dear ImGui docking editor built from `Source/EditorApp2`.

## Generated Projects

`ProjectCreator` reads `KEY_ENGINE_SDK_PATH`.

Example:

```bash
KEY_ENGINE_SDK_PATH="$PWD" ./Binaries/Linux/ProjectCreator --project-name TestProject --project-path "$PWD/Projects"
```

`ReflectionParser` also reads `KEY_ENGINE_SDK_PATH`.

Example:

```bash
KEY_ENGINE_SDK_PATH="$PWD" ./Binaries/Linux/ReflectionParser --project-name TestProject --project-path "$PWD/Projects/TestProject"
```

Generated projects now receive a `CMakeLists.txt` generated from:

```text
Content/Templates/Project/CMakeLists.txt.mustache
```

Generated projects reuse the SDK dependency downloads from `Build/Linux-Debug/_deps`.
They do not run their own `FetchContent` downloads.
If a generated project cannot find those headers, configure the SDK first or pass:

```bash
-DKEY_ENGINE_SDK_BUILD_DIR=<path-to-sdk-build-dir>
```

The editor compile workflow configures and builds generated projects with CMake, not MSBuild.

## Important Porting Notes

- Do not restore old `.sln`, `.vcxproj`, `.vcxproj.filters`, or `.vcxproj.user` files. CMake can generate Visual Studio files on Windows if needed.
- Do not restore bundled `Dependencies/LLVM`, `Dependencies/assimp`, or `Dependencies/bgfx` unless a new feature actually needs them.
- Do not restore vendored `base64`, `glm`, `mustache`, `rapidjson`, or `tclap`; CMake fetches them.
- Keep `Dependencies/qtpropertybrowser` while the Qt editor is the primary editor.
- Keep `glm`; it is the base math dependency, now provided through CMake.
- The editor action formerly named `OpenVS` is now `OpenVSCode`; it runs `code <project_path>` and falls back to opening the folder.
- `Content/Templates/Project/Source/Main.cpp` was converted from UTF-16LE to UTF-8 so generated projects compile on Linux.
- `ReflectionParser` no longer uses hardcoded Visual Studio include paths and now parses with `-std=c++17`.

## GitHub

Remote:

```text
git@github.com:katoun/KeyEngine.git
```

Branch:

```text
master
```

Current workflow:

```bash
git status --short --branch
git add -A
git commit -m "<message>"
git push origin master
```

## Verification Commands

Engine build:

```bash
cmake --build Build/Linux-Debug --parallel
```

Smoke-test a generated project:

```bash
rm -rf /tmp/keyengine-project-smoke
mkdir -p /tmp/keyengine-project-smoke
KEY_ENGINE_SDK_PATH="$PWD" ./Binaries/Linux/ProjectCreator --project-name SmokeProject --project-path /tmp/keyengine-project-smoke
KEY_ENGINE_SDK_PATH="$PWD" ./Binaries/Linux/ReflectionParser --project-name SmokeProject --project-path /tmp/keyengine-project-smoke/SmokeProject
cmake -S /tmp/keyengine-project-smoke/SmokeProject -B /tmp/keyengine-project-smoke/SmokeProject/Build/Linux-Debug -DCMAKE_BUILD_TYPE=Debug -DKEY_ENGINE_SDK_PATH="$PWD"
cmake --build /tmp/keyengine-project-smoke/SmokeProject/Build/Linux-Debug --parallel
```

Expected generated module:

```text
/tmp/keyengine-project-smoke/SmokeProject/Build/Linux/SmokeProject-EditorModule_d.so
```

## Known Warnings

The build currently succeeds with warnings. Known warning families include:

- `const` on scalar return types.
- GLM old `register`/operator precedence warnings.
- Qt metatype warnings around `reflection::Field` destructor/copy behavior.
- Reorder and unused-variable warnings in old code.

Treat these as cleanup work, not current build blockers.

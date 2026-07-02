/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MainWindow.h"
#include "NewProjectDialog.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDesktopServices>
#include <QProcessEnvironment>
#include <QUrl>

#include <Runtime.h>
#include <Editor/ModuleManager.h>

#include <iostream>
#include <fstream>

namespace
{
	void WriteProcessOutput(QProcess* process, QProcess::ProcessChannel channel)
	{
		QByteArray output = channel == QProcess::StandardError
			? process->readAllStandardError()
			: process->readAllStandardOutput();

		if (output.isEmpty())
			return;

		std::ostream& stream = channel == QProcess::StandardError ? std::cerr : std::cout;
		stream.write(output.constData(), output.size());
		stream.flush();
	}

	QString KeyEngineSDKPath()
	{
		QDir app_dir(QApplication::applicationDirPath());
		app_dir.cdUp();
		app_dir.cdUp();
		return app_dir.canonicalPath();
	}

	QString ToolExecutableName(const QString& name)
	{
#ifdef Q_OS_WIN
		return name + ".exe";
#else
		return name;
#endif
	}

	void ApplyKeyEngineEnvironment(QProcess* process)
	{
		QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
		environment.insert("KEY_ENGINE_SDK_PATH", KeyEngineSDKPath());
		process->setProcessEnvironment(environment);
	}

	void AttachProcessLogging(QProcess* process)
	{
		QObject::connect(process, &QProcess::readyReadStandardOutput, [process]() {
			WriteProcessOutput(process, QProcess::StandardOutput);
		});

		QObject::connect(process, &QProcess::readyReadStandardError, [process]() {
			WriteProcessOutput(process, QProcess::StandardError);
		});

		QObject::connect(process, QOverload<QProcess::ProcessError>::of(&QProcess::errorOccurred), [process](QProcess::ProcessError) {
			std::cerr << "Process error: " << process->errorString().toStdString() << std::endl;
		});
	}

	void StartLoggedProcess(QProcess* process, const QString& program, const QStringList& arguments)
	{
		AttachProcessLogging(process);

		std::cout << "Running: " << program.toStdString();
		for (const QString& argument : arguments)
		{
			std::cout << " " << argument.toStdString();
		}
		std::cout << std::endl;

		process->start(program, arguments);
	}

	void FlushProcessOutput(QProcess* process)
	{
		WriteProcessOutput(process, QProcess::StandardOutput);
		WriteProcessOutput(process, QProcess::StandardError);
	}
}

namespace editor
{
	MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
	{
		ui.setupUi(this);

		m_OutlinerWidget = new OutlinerWidget(this);
		ui.Outliner->setWidget(m_OutlinerWidget);

		m_ContentBrowserWidget = new ContentBrowserWidget(this);
		ui.ContentBrowser->setWidget(m_ContentBrowserWidget);

		m_InspectorWidget = new InspectorWidget(this);
		ui.Inspector->setWidget(m_InspectorWidget);

		QObject::connect(ui.NewProject, SIGNAL(triggered()), this, SLOT(OnNewProject()));
		QObject::connect(ui.OpenProject, SIGNAL(triggered()), this, SLOT(OnOpenProject()));
		//QObject::connect(ui.Save, SIGNAL(triggered()), this, SLOT(OnSave()));

		QObject::connect(ui.Exit, SIGNAL(triggered()), this, SLOT(OnExit()));
		QObject::connect(ui.Compile, SIGNAL(triggered()), this, SLOT(OnCompile()));
		QObject::connect(ui.OpenVSCode, SIGNAL(triggered()), this, SLOT(OpenVSCode()));

		QObject::connect(ui.CreateGameObject, SIGNAL(triggered()), m_OutlinerWidget, SLOT(OnCreateTopLevelGameObject()));

		//Forward edit actions to Outline Widget
		QObject::connect(ui.Save, SIGNAL(triggered()), m_OutlinerWidget, SLOT(OnSave()));
		QObject::connect(ui.Copy, SIGNAL(triggered()), m_OutlinerWidget, SLOT(OnCopyItem()));
		QObject::connect(ui.Paste, SIGNAL(triggered()), m_OutlinerWidget, SLOT(OnPasteItem()));
		QObject::connect(ui.Duplicate, SIGNAL(triggered()), m_OutlinerWidget, SLOT(OnDuplicateItem()));
		QObject::connect(ui.Delete, SIGNAL(triggered()), m_OutlinerWidget, SLOT(OnDeleteItem()));

		RefreshComponentsList();
		RefreshTypesList();

		m_CommandProcess = nullptr;
		m_CurrentProjectModule = nullptr;
	}

	MainWindow::~MainWindow()
	{}

	void MainWindow::OpenProject(QString project_file_path)
	{
		QFileInfo project_file_info = project_file_path;
		current_project_path = project_file_info.path();
		current_project_name = project_file_info.baseName();

		m_ContentBrowserWidget->ProjectChanged(current_project_path);

		ui.StatusBar->showMessage(tr("Project Loaded"), 3000);
	}

	void MainWindow::SelectionChanged(core::Object* selection)
	{
		m_InspectorWidget->SelectionChanged(selection);
	}

	void MainWindow::RefreshComponentsList()
	{
		auto &rm = reflection::ReflectionManager::Instance();
		std::vector<reflection::Type> components = rm.GetTypes<game::Component>();
		ui.MenuComponent->clear();
		for (auto& component : components)
		{
			QAction *component_action = new QAction(this);
			auto display_name_attribute = component.GetAttribute<attribute::DisplayName>();
			std::string component_name = display_name_attribute->Value;
			component_action->setObjectName(QString::fromStdString(component_name));
			component_action->setText(QApplication::translate("MainWindow", component_name.c_str(), 0));

			auto icon = FindComponentIcon(QString::fromStdString(core::string::ToLower(component_name)));

			component_action->setIcon(icon);

			component_action->setData(component.GetID());

			QObject::connect(ui.MenuComponent, SIGNAL(triggered(QAction*)), m_OutlinerWidget, SLOT(OnAttachComponent(QAction*)));

			ui.MenuComponent->addAction(component_action);
		}
	}

	void MainWindow::RefreshTypesList()
	{
		auto &rm = reflection::ReflectionManager::Instance();

		std::vector<reflection::Type> types = rm.GetTypes();

		ui.MenuTypes->clear();

		for (auto& type : types)
		{
			QAction *type_action = new QAction(this);
			std::string type_name = type.GetName();
			QString q_type_name = QString::fromStdString(type.GetName());
			type_action->setObjectName(q_type_name);
			type_action->setText(QApplication::translate("MainWindow", type_name.c_str(), 0));
			type_action->setData(type.GetID());

			ui.MenuTypes->addAction(type_action);
		}

		//TEMP FOR TESTING!!!
		QDir app_dir = QDir(QApplication::applicationDirPath());
		QString types_path = app_dir.absoluteFilePath("types.txt");

		std::ofstream file;
		file.open(types_path.toStdString());

		file << "Types: " << std::endl;
		file << "--------------------------------------- " << std::endl;
		for (auto& type : types)
		{
			file << "Type: " << type.GetName() << std::endl;

			auto fields = type.GetFields();

			for (auto i = fields.begin(); i != fields.end(); ++i)
			{
				auto& field = (*i);

				file << "Field: " << field.GetName() << " type: " << field.GetType().GetName() << std::endl;
			}

			file << "--------------------------------------- " << std::endl;
		}
		file.close();
		//TEMP FOR TESTING!!!
	}

	QIcon MainWindow::FindComponentIcon(const QString& name)
	{
		QIcon icon;

		QString icon_resource_path = ":/icons/components/";
		icon_resource_path.append(name);
		icon_resource_path.append(".png");

		QString icon_project_path = current_project_path;
		icon_project_path.append("/Content/Icons/Components/");
		icon_project_path.append(name);
		icon_project_path.append(".png");

		if (QFile(icon_resource_path).exists())
		{
			icon.addFile(icon_resource_path, QSize(), QIcon::Normal, QIcon::Off);
		}
		else if (QFile(icon_project_path).exists())
		{
			icon.addFile(icon_project_path, QSize(), QIcon::Normal, QIcon::Off);
		}
		else
		{
			icon.addFile(QStringLiteral(":/icons/components/default.png"), QSize(), QIcon::Normal, QIcon::Off);
		}

		return icon;
	}

	void MainWindow::OnNewProject()
	{
		NewProjectDialog dialog(this);
		dialog.exec();
		if (dialog.result() == QDialog::Accepted)
		{
			QString project_path = dialog.GetProjectPath();
			QString project_name = dialog.GetProjectName();

			current_project_path = project_path;
			current_project_name = project_name;

			StartProjectCreator();
		}
	}

	void MainWindow::OnOpenProject()
	{
		QString project_file_path = QFileDialog::getOpenFileName(this, tr("Open Project"), "", tr("Projects (*.keyproject)"));

		OpenProject(project_file_path);
	}

	void MainWindow::OnSave()
	{
		//TODO!!!
	}

	void MainWindow::OnExit()
	{
		QApplication::exit();
	}

	void MainWindow::OpenVSCode()
	{
		if (current_project_path.isEmpty())
			return;

		if (QProcess::startDetached("code", QStringList() << current_project_path))
		{
			ui.StatusBar->showMessage(tr("Opening VSCode"), 3000);
		}
		else
		{
			QDesktopServices::openUrl(QUrl::fromLocalFile(current_project_path));
			ui.StatusBar->showMessage(tr("Opening Project Folder"), 3000);
		}
	}

	void MainWindow::OnCompile()
	{
		if (current_project_path.isEmpty())
			return;

		if (m_CurrentProjectModule != nullptr)
		{
			editor::ModuleManager::Instance().UnloadModule(m_CurrentProjectModule);
			m_CurrentProjectModule = nullptr;

			RefreshComponentsList();
			RefreshTypesList();
		}

		StartReflectionParser();
	}

	void MainWindow::StartProjectCreator()
	{
		QDir app_dir = QDir(QApplication::applicationDirPath());

		QString creator_path = app_dir.absoluteFilePath(ToolExecutableName("ProjectCreator"));

		m_CommandProcess = new QProcess();
		QObject::connect(m_CommandProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(OnProjectCreatorFinished(int, QProcess::ExitStatus)));

		m_CommandProcess->setWorkingDirectory(app_dir.absolutePath());
		ApplyKeyEngineEnvironment(m_CommandProcess);

		QStringList arguments;
		arguments << "-n" << current_project_name << "-p" << current_project_path;
		StartLoggedProcess(m_CommandProcess, creator_path, arguments);

		ui.StatusBar->showMessage(tr("Creating Project"));
	}

	void MainWindow::StartReflectionParser()
	{
		QDir app_dir = QDir(QApplication::applicationDirPath());

		QString parser_path = app_dir.absoluteFilePath(ToolExecutableName("ReflectionParser"));

		m_CommandProcess = new QProcess();
		QObject::connect(m_CommandProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(OnReflectionParserFinished(int, QProcess::ExitStatus)));

		m_CommandProcess->setWorkingDirectory(app_dir.absolutePath());
		ApplyKeyEngineEnvironment(m_CommandProcess);

		QStringList arguments;
		arguments << "-n" << current_project_name << "-p" << current_project_path;
		StartLoggedProcess(m_CommandProcess, parser_path, arguments);

		ui.StatusBar->showMessage(tr("Parsing"));
	}

	void MainWindow::StartProjectCompile()
	{
		//Clear intermediate build directories
		QString build_path = current_project_path;
		QString binaries_path = current_project_path;
		build_path.append("/Build");
		binaries_path.append("/Binaries");

		bool res = false;
		QDir build_dir(build_path);
		res = build_dir.removeRecursively();
		QDir bin_dir(binaries_path);
		res = bin_dir.removeRecursively();

		m_CommandProcess = new QProcess();
		QObject::connect(m_CommandProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(OnProjectConfigureFinished(int, QProcess::ExitStatus)));

		m_CommandProcess->setWorkingDirectory(current_project_path);
		ApplyKeyEngineEnvironment(m_CommandProcess);

		QStringList arguments;
		arguments << "-S" << current_project_path;
		arguments << "-B" << current_project_path + "/Build/Linux-Debug";
		arguments << "-G" << "Ninja";
		arguments << "-DCMAKE_BUILD_TYPE=Debug";
		arguments << "-DCMAKE_CXX_COMPILER=clang++-20";
		arguments << "-DKEY_ENGINE_SDK_PATH=" + KeyEngineSDKPath();
		StartLoggedProcess(m_CommandProcess, "cmake", arguments);

		ui.StatusBar->showMessage(tr("Configuring"));
	}

	void MainWindow::OnProjectCreatorFinished(int exitCode, QProcess::ExitStatus status)
	{
		FlushProcessOutput(m_CommandProcess);
		SAFE_DELETE(m_CommandProcess);

		if (status == QProcess::NormalExit && exitCode == 0)
		{
			ui.StatusBar->clearMessage();

			QString project_file_path = current_project_path;
			project_file_path.append("/");
			project_file_path.append(current_project_name);
			project_file_path.append("/");
			project_file_path.append(current_project_name);
			project_file_path.append(".keyproject");
			OpenProject(project_file_path);
		}

		if (status == QProcess::CrashExit || exitCode != 0)
		{
			current_project_path = tr("");
			current_project_name = tr("");

			ui.StatusBar->showMessage(tr("Project Creation Failed"));
		}
	}

	void MainWindow::OnReflectionParserFinished(int exitCode, QProcess::ExitStatus status)
	{
		FlushProcessOutput(m_CommandProcess);
		SAFE_DELETE(m_CommandProcess);

		if (status == QProcess::NormalExit && exitCode == 0)
		{
			ui.StatusBar->clearMessage();

			StartProjectCompile();
		}

		if (status == QProcess::CrashExit || exitCode != 0)
		{
			ui.StatusBar->showMessage(tr("Parsing Failed"));
		}
	}

	void MainWindow::OnProjectConfigureFinished(int exitCode, QProcess::ExitStatus status)
	{
		FlushProcessOutput(m_CommandProcess);
		SAFE_DELETE(m_CommandProcess);

		if (status != QProcess::NormalExit || exitCode != 0)
		{
			ui.StatusBar->showMessage(tr("Configure Failed"));
			return;
		}

		m_CommandProcess = new QProcess();
		QObject::connect(m_CommandProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(OnProjectCompileFinished(int, QProcess::ExitStatus)));

		m_CommandProcess->setWorkingDirectory(current_project_path);
		ApplyKeyEngineEnvironment(m_CommandProcess);

		QStringList arguments;
		arguments << "--build" << current_project_path + "/Build/Linux-Debug";
		arguments << "--parallel";
		StartLoggedProcess(m_CommandProcess, "cmake", arguments);

		ui.StatusBar->showMessage(tr("Compiling"));
	}

	void MainWindow::OnProjectCompileFinished(int exitCode, QProcess::ExitStatus status)
	{
		FlushProcessOutput(m_CommandProcess);
		SAFE_DELETE(m_CommandProcess);

		if (status == QProcess::NormalExit && exitCode == 0)
		{
			ui.StatusBar->clearMessage();

			m_CurrentProjectModule = editor::ModuleManager::Instance().LoadModule(current_project_path.toStdString(), current_project_name.toStdString());

			RefreshComponentsList();
			RefreshTypesList();
		}

		if (status == QProcess::CrashExit || exitCode != 0)
		{
			ui.StatusBar->showMessage(tr("Compiling Failed"));
		}
	}

}


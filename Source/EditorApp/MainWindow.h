/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtWidgets/QMainWindow>
//#include <QFileSystemModel>
#include <QProcess>
#include "OutlinerWidget.h"
#include "ContentBrowserWidget.h"
#include "InspectorWidget.h"
#include "ui_MainWindow.h"

#include <vector>

namespace game
{
	class GameObject;
}

namespace editor
{
	class Module;
	class ModuleManager;

	class MainWindow : public QMainWindow
	{
		Q_OBJECT

	public:
		MainWindow(QWidget *parent = 0);
		~MainWindow();

		void OpenProject(QString project_file_path);

		void SelectionChanged(core::Object* selection);

	private:
		Ui::MainWindow ui;

		QString current_project_path;
		QString current_project_name;
		editor::Module* m_CurrentProjectModule;

		OutlinerWidget* m_OutlinerWidget;
		ContentBrowserWidget* m_ContentBrowserWidget;
		InspectorWidget* m_InspectorWidget;

		QProcess* m_CommandProcess;

		void RefreshComponentsList();
		void RefreshTypesList();

		QIcon FindComponentIcon(const QString& name);

	public slots:

		void OnNewProject();
		void OnOpenProject();
		void OnSave();
		void OnExit();

		void OpenVS();
		void OnCompile();

		void StartProjectCreator();
		void StartReflectionParser();
		void StartProjectCompile();

		void OnProjectCreatorFinished(int, QProcess::ExitStatus);
		void OnReflectionParserFinished(int, QProcess::ExitStatus);
		void OnProjectCompileFinished(int, QProcess::ExitStatus);
		void OnOpenVSFinished(int, QProcess::ExitStatus);
	};
}

#endif // MAINWINDOW_H

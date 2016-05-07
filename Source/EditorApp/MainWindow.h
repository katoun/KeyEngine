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

		editor::ModuleManager* m_ModuleManager;

		QString current_project_path;
		QString current_project_name;
		editor::Module* m_CurrentProjectModule;

		OutlinerWidget* m_OutlinerWidget;
		ContentBrowserWidget* m_ContentBrowserWidget;
		InspectorWidget* m_InspectorWidget;

		QProcess* m_BuildProcess;

		void RefreshComponentsList();
		void RefreshTypesList();

		QIcon FindComponentIcon(const QString& name);

	public slots:

		void OnOpenProject();
		void OnSave();
		void OnExit();

		void OnCompile();

		void StartParser();
		void StartCompile();

		void OnParserFinished(int, QProcess::ExitStatus);
		void OnCompileFinished(int, QProcess::ExitStatus);
	};
}

#endif // MAINWINDOW_H

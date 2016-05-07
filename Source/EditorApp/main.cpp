#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QDir>
#include <QFileInfo>

int main(int argc, char *argv[])
{
	QApplication application(argc, argv);
	QCoreApplication::setApplicationName("KeyEngine");
	QCoreApplication::setApplicationVersion("0.0.1");

	QDir app_dir = QDir(application.applicationDirPath());
	QString plugins_path = app_dir.absoluteFilePath("plugins");
	QStringList paths;
	paths.append(plugins_path);
	application.setLibraryPaths(paths);

	//Set default style
	QFile style_file(":/darkstyle/style.qss");
	if (style_file.exists())
	{
		style_file.open(QFile::ReadOnly | QFile::Text);
		QString style_sheet = QLatin1String(style_file.readAll());
		application.setStyleSheet(style_sheet);
	}
	//Set default style

	QCommandLineParser parser;
	parser.setApplicationDescription("KeyEngine Commands Parser");
	parser.addHelpOption();
	parser.addVersionOption();

	QCommandLineOption oper_project_option(QStringList() << "o" << "open-project",
		QCoreApplication::translate("main", "Open project file <file>."),
		QCoreApplication::translate("main", "<file>"));

	parser.addOption(oper_project_option);

	parser.process(application);

	bool open_project = parser.isSet(oper_project_option);
	QString project_file_path = parser.value(oper_project_option);

	editor::MainWindow main_window;
	main_window.show();
	if (open_project)
	{
		main_window.OpenProject(project_file_path);
	}
	
	return application.exec();
}

/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QDir>
#include <QFileInfo>

#include <string>

//struct Object
//{
//	std::string Name;
//
//	Object()
//		: Name()
//	{
//	}
//
//	Object(const std::string& name)
//		: Name(name)
//	{
//	}
//
//	virtual ~Object()
//	{
//	}
//
//	template <class Archive> void persist(Archive& archive)
//	{
//		archive.value("name", Name);
//	}
//};
//
//struct LeftObject : public Object
//{
//	int LeftValue;
//
//	LeftObject()
//		: Object()
//		, LeftValue(0)
//	{
//	}
//
//	LeftObject(const std::string& name, int left_value)
//		: Object(name)
//		, LeftValue(left_value)
//	{
//	}
//
//	template <class Archive> void persist(Archive& archive)
//	{
//		Object::persist(archive);
//		archive.value("left_value", LeftValue);
//	}
//};
//
//struct RightObject : public Object
//{
//	float RightValue;
//
//	RightObject()
//		: Object()
//		, RightValue(0.0f)
//	{
//	}
//
//	RightObject(const std::string& name, float right_value)
//		: Object(name)
//		, RightValue(right_value)
//	{
//	}
//
//	template <class Archive> void persist(Archive& archive)
//	{
//		Object::persist(archive);
//		archive.value("right_value", RightValue);
//	}
//};
//
//struct Container
//{
//	std::vector<Object*> Objects;
//
//	Container()
//		: Objects()
//	{
//	}
//
//	~Container()
//	{
//		while (!Objects.empty())
//		{
//			delete Objects.back();
//			Objects.pop_back();
//		}
//	}
//
//	void create()
//	{
//		Objects.push_back(new Object("object_0"));
//		Objects.push_back(new Object("object_1"));
//		Objects.push_back(new Object("object_2"));
//		Objects.push_back(new Object("object_3"));
//		Objects.push_back(new LeftObject("left_0", 0));
//		Objects.push_back(new RightObject("right_0", 0.0f));
//		Objects.push_back(new LeftObject("left_1", 1));
//		Objects.push_back(new LeftObject("left_2", 2));
//		Objects.push_back(new RightObject("right_1", 1.0f));
//		Objects.push_back(new RightObject("right_2", 2.0f));
//		Objects.push_back(new LeftObject("left_3", 3));
//	}
//
//	template <class Archive> void persist(Archive& archive)
//	{
//		archive.enter("Container", 1, *this);
//		archive.value("objects", "object", Objects);
//	}
//};
//
//template <class Archive>
//void declare(Archive& archive)
//{
//	archive.declare<Object>("Object", sweet::persist::PERSIST_POLYMORPHIC);
//	archive.declare<LeftObject>("LeftObject", sweet::persist::PERSIST_POLYMORPHIC);
//	archive.declare<RightObject>("RightObject", sweet::persist::PERSIST_POLYMORPHIC);
//}

int main(int argc, char *argv[])
{
	/*{
		sweet::persist::JsonWriter json_writer;

		declare(json_writer);

		Container container;
		container.create();

		json_writer.write("example.json", "container", container);
	}*/

	/*{
		sweet::persist::JsonReader json_reader;

		declare(json_reader);

		Container container;

		json_reader.read("example.json", "container", container);
	}*/

	//////////////////////////////////////

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

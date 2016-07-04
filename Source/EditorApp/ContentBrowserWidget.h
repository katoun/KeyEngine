/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CONTENT_BROWSER_WIDGET_H
#define CONTENT_BROWSER_WIDGET_H

#include <QtCore>
#include <QFileSystemModel>

#include "ui_ContentBrowserWidget.h"

namespace editor
{
	class ContentBrowserWidget : public QWidget
	{
		Q_OBJECT
	public:
		ContentBrowserWidget(QWidget* parent = 0);

		void ProjectChanged(const QString &path);

	private:

		Ui::ContentBrowserWidget ui;
		QFileSystemModel* m_Model;
		QFileSystemWatcher* m_Watcher;

		void Watch(QString path, bool recursive);

	public slots:

		void DoubleClicked(const QModelIndex &);

		void FileChanged(const QString &path);
		void DirectoryChanged(const QString &path);
	};
}

#endif // CONTENT_BROWSER_WIDGET_H

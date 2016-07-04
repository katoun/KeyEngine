/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ContentBrowserWidget.h"

namespace editor
{
	ContentBrowserWidget::ContentBrowserWidget(QWidget* parent /*= 0*/)
	{
		ui.setupUi(this);

		m_Model = new QFileSystemModel(this);
		m_Model->setReadOnly(false);

		m_Watcher = new QFileSystemWatcher();

		connect(ui.Content, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(DoubleClicked(const QModelIndex &)));
		connect(m_Watcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(DirectoryChanged(const QString&)));
		connect(m_Watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(FileChanged(const QString&)));
	}

	void ContentBrowserWidget::ProjectChanged(const QString &path)
	{
		ui.Content->setModel(m_Model);
		m_Model->sort(0);
		ui.Content->hideColumn(1);
		ui.Content->hideColumn(2);
		ui.Content->hideColumn(3);

		ui.Content->setDragEnabled(true);
		ui.Content->setAcceptDrops(true);
		ui.Content->setDropIndicatorShown(true);

		m_Model->setRootPath(path);
		//m_Model->setNameFilters(QStringList() << "Content" << "Source");
		//m_Model->setNameFilterDisables(false);
		QModelIndex idx = m_Model->index(path);
		ui.Content->setRootIndex(idx);
		Watch(path, true);
	}

	void editor::ContentBrowserWidget::Watch(QString path, bool recursive)
	{
		//qDebug() << "watching dir:" << path;
		if (!m_Watcher->directories().contains(path))
		{
			m_Watcher->addPath(path);
		}
		QDir dir(path);
		foreach(QFileInfo info, dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot))
		{
			QString path = info.absoluteFilePath();
			if (info.isDir())
			{
				Watch(path, true);
			}
		}
	}

	void ContentBrowserWidget::DoubleClicked(const QModelIndex &index)
	{
		auto& path = m_Model->filePath(ui.Content->currentIndex());
		//TODO: open the appropriate editor for the file!!!
	}

	void ContentBrowserWidget::FileChanged(const QString & path)
	{
		//TODO: see what resource files need reimporting!!!
	}

	void ContentBrowserWidget::DirectoryChanged(const QString & path)
	{
		Watch(path, false);
	}
}
/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef OUTLINER_WIDGET_H
#define OUTLINER_WIDGET_H

#include <EditorAppConfig.h>
#include <QtWidgets/QTreeWidget>

namespace core
{
	class Object;
}

namespace editor
{
	class MainWindow;

	enum OutlinerItemType
	{
		FOLDER = QTreeWidgetItem::UserType + 1,
		GAME_OBJECT
	};

	class OutlinerWidget : public QTreeWidget
	{
		Q_OBJECT
	public:

		OutlinerWidget(QWidget* parent = 0);

	private:

		QIcon GetOutlinerIcon(const QString& name);

		void SelectionChanged();
		void CreateGameObjectItem(QTreeWidgetItem*);

		MainWindow* m_MainWindow;

		QTreeWidgetItem* m_Selection;

		QMenu* m_ContextMenu;
		QAction* m_CopyAction;
		QAction* m_PasteAction;
		QAction* m_RenameAction;
		QAction* m_DuplicateAction;
		QAction* m_DeleteAction;
		QAction* m_CreateFolderAction;
		QAction* m_CreateGameObjectAction;

	public slots:

		void OnCreateTopLevelGameObject();
		void OnAttachComponent(QAction* component_action);

		void OnItemSelectionChanged();
		void OnItemChanged(QTreeWidgetItem*, int);

		void OnShowContextMenu(const QPoint&);

		void OnSave();

		void OnCopyItem();
		void OnPasteItem();

		void OnRenameItem();
		void OnDuplicateItem();
		void OnDeleteItem();

		void OnCreateFolderItem();
		void OnCreateGameObjectItem();
	};
}

#endif
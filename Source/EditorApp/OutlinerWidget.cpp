#include "OutlinerWidget.h"
#include "MainWindow.h"
#include <Runtime.h>

#include <QFile>
#include <QEvent>

#include <fstream>

namespace editor
{
	OutlinerWidget::OutlinerWidget(QWidget* parent) : m_Selection(nullptr), m_MainWindow(static_cast<editor::MainWindow*>(parent))
	{
		//Create and set labels
		QStringList labels;
		labels << tr("Scene");

		setColumnCount(1);
		setHeaderLabels(labels);

		//Set selection mode
		setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
		setSelectionBehavior(QTreeView::SelectRows);

		//Create and set custom context menu
		m_ContextMenu = new QMenu(this);
		
		m_CopyAction = new QAction(this);
		m_CopyAction->setText(QStringLiteral("Copy"));
		m_CopyAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
		m_CopyAction->setShortcutContext(Qt::WidgetShortcut);
		
		m_PasteAction = new QAction(this);
		m_PasteAction->setText(QStringLiteral("Paste"));
		m_PasteAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
		m_PasteAction->setShortcutContext(Qt::WidgetShortcut);

		m_RenameAction = new QAction(this);
		m_RenameAction->setText(QStringLiteral("Rename"));
		m_RenameAction->setShortcut(QKeySequence(Qt::Key_F2));
		m_RenameAction->setShortcutContext(Qt::WidgetShortcut);
		
		m_DuplicateAction = new QAction(this);
		m_DuplicateAction->setText(QStringLiteral("Duplicate"));
		m_DuplicateAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
		m_DuplicateAction->setShortcutContext(Qt::WidgetShortcut);
		
		m_DeleteAction = new QAction(this);
		m_DeleteAction->setText(QStringLiteral("Delete"));
		m_DeleteAction->setShortcut(QKeySequence(Qt::Key_Delete));
		m_DeleteAction->setShortcutContext(Qt::WidgetShortcut);
		
		m_CreateFolderAction = new QAction(this);
		m_CreateFolderAction->setText(QStringLiteral("Create Folder"));
		
		m_CreateGameObjectAction = new QAction(this);
		m_CreateGameObjectAction->setText(QStringLiteral("Create Game Object"));

		m_ContextMenu->addAction(m_CopyAction);
		m_ContextMenu->addAction(m_PasteAction);
		m_ContextMenu->addSeparator();
		m_ContextMenu->addAction(m_RenameAction);
		m_ContextMenu->addAction(m_DuplicateAction);
		m_ContextMenu->addAction(m_DeleteAction);
		m_ContextMenu->addSeparator();
		m_ContextMenu->addAction(m_CreateFolderAction);
		m_ContextMenu->addAction(m_CreateGameObjectAction);

		setContextMenuPolicy(Qt::CustomContextMenu);

		QObject::connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnShowContextMenu(const QPoint&)));

		QObject::connect(m_CopyAction, SIGNAL(triggered()), this, SLOT(OnCopyItem()));
		QObject::connect(m_PasteAction, SIGNAL(triggered()), this, SLOT(OnPasteItem()));
		QObject::connect(m_RenameAction, SIGNAL(triggered()), this, SLOT(OnRenameItem()));
		QObject::connect(m_DuplicateAction, SIGNAL(triggered()), this, SLOT(OnDuplicateItem()));
		QObject::connect(m_DeleteAction, SIGNAL(triggered()), this, SLOT(OnDeleteItem()));
		QObject::connect(m_CreateFolderAction, SIGNAL(triggered()), this, SLOT(OnCreateFolderItem()));
		QObject::connect(m_CreateGameObjectAction, SIGNAL(triggered()), this, SLOT(OnCreateGameObjectItem()));

		QObject::connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(OnItemSelectionChanged()));
		QObject::connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(OnItemChanged(QTreeWidgetItem*, int)));
	}

	QIcon OutlinerWidget::GetOutlinerIcon(const QString& name)
	{
		QIcon icon;

		QString icon_path = ":/icons/outliner/";
		icon_path.append(name);
		icon_path.append(".png");

		if (QFile(icon_path).exists())
		{
			icon.addFile(icon_path, QSize(), QIcon::Normal, QIcon::Off);
		}
		else
		{
			icon.addFile(QStringLiteral(":/icons/outliner/gameobject.png"), QSize(), QIcon::Normal, QIcon::Off);
		}

		return icon;
	}

	void OutlinerWidget::SelectionChanged()
	{
		if (m_MainWindow != nullptr)
		{
			core::Object* object = nullptr;
			if (m_Selection != nullptr && m_Selection->type() == OutlinerItemType::GAME_OBJECT)
			{
				QVariant data = m_Selection->data(0, Qt::UserRole);
				game::GameObject* game_object = data.value<game::GameObject*>();
				object = static_cast<core::Object*>(game_object);
			}

			m_MainWindow->SelectionChanged(object);
		}
	}

	void OutlinerWidget::CreateGameObjectItem(QTreeWidgetItem* parent)
	{
		game::GameObject* game_object = new game::GameObject();
		QString name = QString::fromStdString(game_object->GetName());
		QTreeWidgetItem* item = nullptr;

		if (parent != nullptr)
		{
			item = new QTreeWidgetItem(OutlinerItemType::GAME_OBJECT);

			if (parent->type() == OutlinerItemType::GAME_OBJECT)
			{
				QVariant data = parent->data(0, Qt::UserRole);
				game::GameObject* parent_game_object = data.value<game::GameObject*>();
				if (parent_game_object != nullptr)
				{
					parent_game_object->AddChild(game_object);
				}
			}
		}
		else
		{
			item = new QTreeWidgetItem(this, OutlinerItemType::GAME_OBJECT);
		}

		item->setText(0, name);
		item->setIcon(0, GetOutlinerIcon(QStringLiteral("gameobject")));
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		QVariant data;
		data.setValue<game::GameObject*>(game_object);
		item->setData(0, Qt::UserRole, data);

		if (parent != nullptr)
		{
			parent->addChild(item);
			expandItem(parent);
		}
		else
		{
			addTopLevelItem(item);
		}
	}

	void OutlinerWidget::OnCreateTopLevelGameObject()
	{
		CreateGameObjectItem(nullptr);
	}

	void OutlinerWidget::OnAttachComponent(QAction* component_action)
	{
		if (component_action == nullptr)
			return;

		if (m_Selection == nullptr)
			return;

		if (m_Selection->type() != OutlinerItemType::GAME_OBJECT)
			return;

		QVariant data = m_Selection->data(0, Qt::UserRole);
		game::GameObject* game_object = data.value<game::GameObject*>();

		if (game_object == nullptr)
			return;

		auto& type = game_object->GetType();
		auto& game_object_type = typeof(game::GameObject);
		if (type == game_object_type)
		{
			QVariant data = component_action->data();
			reflection::TypeID id = data.value<reflection::TypeID>();

			reflection::Type component_type = reflection::Type(id);

			game_object->AddComponent(component_type);
		}

		SelectionChanged();
	}

	void OutlinerWidget::OnItemSelectionChanged()
	{
		m_Selection = nullptr;

		QList<QTreeWidgetItem*> items = selectedItems();
		if (items.count() != 0)
		{
			for (auto item : items)
			{
				if (item == nullptr)
					continue;

				m_Selection = item;//TODO: add multi selection!!!
			}
		}
		else
		{
			clearSelection();
		}

		SelectionChanged();
	}

	void OutlinerWidget::OnItemChanged(QTreeWidgetItem* item, int column)
	{
		if (item == nullptr)
			return;

		if (item->type() == OutlinerItemType::GAME_OBJECT)
		{
			QString text = item->text(0);

			std::string new_name = text.toStdString();

			QVariant data = item->data(0, Qt::UserRole);
			game::GameObject* game_object = data.value<game::GameObject*>();

			if (game_object == nullptr)
				return;

			if (game_object->GetName() != new_name)
			{
				game_object->SetName(new_name);

				SelectionChanged();
			}
		}
	}

	void OutlinerWidget::OnShowContextMenu(const QPoint& pos)
	{
		m_CopyAction->setEnabled(true);
		m_PasteAction->setEnabled(true);
		
		m_RenameAction->setEnabled(true);
		m_DuplicateAction->setEnabled(true);
		m_DeleteAction->setEnabled(true);

		QTreeWidgetItem* item = itemAt(pos);
		if (item != nullptr)
		{
			switch (item->type())
			{
			case OutlinerItemType::FOLDER:
			{
				m_CopyAction->setEnabled(false);
				m_PasteAction->setEnabled(false);
				m_DuplicateAction->setEnabled(false);
			}
			break;
			case OutlinerItemType::GAME_OBJECT:
			{
				//TODO!!!
			}
			break;
			}
		}
		else
		{
			m_CopyAction->setEnabled(false);

			m_RenameAction->setEnabled(false);
			m_DuplicateAction->setEnabled(false);
			m_DeleteAction->setEnabled(false);
		}

		m_ContextMenu->exec(mapToGlobal(pos));
	}

	void OutlinerWidget::OnSave()
	{
		//TODO!!!
	}

	void OutlinerWidget::OnCopyItem()
	{
		if (!hasFocus())
			return;
		if (m_Selection == nullptr)
			return;

		//TODO!!!
	}

	void OutlinerWidget::OnPasteItem()
	{
		if (!hasFocus())
			return;
		if (m_Selection == nullptr)
			return;

		//TODO!!!
	}

	void OutlinerWidget::OnRenameItem()
	{
		if (m_Selection == nullptr)
			return;

		editItem(m_Selection, 0);

		//TODO!!!
	}

	void OutlinerWidget::OnDuplicateItem()
	{
		if (!hasFocus())
			return;
		if (m_Selection == nullptr)
			return;

		//TODO!!!
		int x = 3;
	}

	void OutlinerWidget::OnDeleteItem()
	{
		if (!hasFocus())
			return;
		if (m_Selection == nullptr)
			return;

		delete m_Selection;
		m_Selection = nullptr;

		SelectionChanged();
	}

	void OutlinerWidget::OnCreateFolderItem()
	{
		QTreeWidgetItem* item = nullptr;

		if (m_Selection != nullptr)
		{
			if (m_Selection->type() == OutlinerItemType::FOLDER)
			{
				item = new QTreeWidgetItem(OutlinerItemType::FOLDER);
			}
		}
		else
		{
			item = new QTreeWidgetItem(this, OutlinerItemType::FOLDER);
		}

		if (item != nullptr)
		{
			item->setText(0, QStringLiteral("New Folder"));
			item->setIcon(0, GetOutlinerIcon(QStringLiteral("folder")));
			item->setFlags(item->flags() | Qt::ItemIsEditable);

			if (m_Selection != nullptr)
			{
				m_Selection->addChild(item);
				expandItem(m_Selection);
			}
			else
			{
				addTopLevelItem(item);
			}
		}
	}

	void OutlinerWidget::OnCreateGameObjectItem()
	{
		CreateGameObjectItem(m_Selection);
	}
}
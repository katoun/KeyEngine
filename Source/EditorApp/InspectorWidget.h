/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INSPECTOR_WIDGET_H
#define INSPECTOR_WIDGET_H

#include "ui_InspectorWidget.h"

#include <EditorAppConfig.h>

#include <QtButtonPropertyBrowser>
#include <QtGroupBoxPropertyBrowser>
#include <qtpropertymanager.h>
#include <qteditorfactory.h>

#include <vector>

namespace core
{
	class Object;
}

namespace reflection
{
	class Field;
}

namespace editor
{
	enum InspectorItemType
	{
		GAME_OBJECT_NAME,
		FIELD
	};

	class InspectorWidget : public QWidget
	{
		Q_OBJECT
	public:
		InspectorWidget(QWidget* parent = 0);

		void SelectionChanged(core::Object* selection);

	private:

		Ui::InspectorWidget ui;

		QtButtonPropertyBrowser* m_PropertyBrowser;
		QtBoolPropertyManager* m_BoolManager;
		QtIntPropertyManager* m_IntManager;
		QtDoublePropertyManager* m_DoubleManager;
		QtStringPropertyManager* m_StringManager;
		QtVec3PropertyManager* m_Vec3Manager;
		QtColorPropertyManager* m_ColorManager;
		QtEnumPropertyManager* m_EnumManager;
		QtGroupPropertyManager* m_GroupManager;

		core::Object* m_Selection;

		QMap<const QtProperty *, reflection::Field*> m_PropertyToField;

	public slots:

		void ValueChanged(QtProperty* property, bool value);
		void ValueChanged(QtProperty* property, int value);
		void ValueChanged(QtProperty* property, double value);
		void ValueChanged(QtProperty* property, const QString &value);
		void ValueChanged(QtProperty* property, const glm::vec3 &value);
		void ValueChanged(QtProperty* property, const QColor &value);
	};
}

#endif // MAINWINDOW_H

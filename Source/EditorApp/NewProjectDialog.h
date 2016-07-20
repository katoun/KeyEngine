/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef NEW_PROJECT_DIALOG_H
#define NEW_PROJECT_DIALOG_H

#include <EditorAppConfig.h>
#include <QDialog>

#include "ui_NewProjectDialog.h"

namespace editor
{
	class MainWindow;

	class NewProjectDialog : public QDialog
	{
		Q_OBJECT
	public:

		NewProjectDialog(QWidget* parent = 0);

		QString GetProjectName() const;
		QString GetProjectPath() const;

	private:

		Ui::DialogNewProject ui;

		void UpdateOkButtonState();

	public slots:

		void OnBrowseProjectPathButtonClicked();
		void OnProjectNameTextChanged(const QString &arg1);
		void OnProjectPathTextChanged(const QString &arg1);

		void OnOkButtonClicked();
		void OnCancelButtonClicked();
	};
}

#endif
/* Copyright (c) 2016 Catalin Alexandru Nastase
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "NewProjectDialog.h"
#include <QFile>
#include <QDir>
#include <QFileDialog>

namespace editor
{
	NewProjectDialog::NewProjectDialog(QWidget* parent): QDialog(parent)
	{
		ui.setupUi(this);
		setModal(true);

		ui.ButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

		QObject::connect(ui.BrowseProjectPathButton, SIGNAL(released()), this, SLOT(OnBrowseProjectPathButtonClicked()));
		QObject::connect(ui.ProjectNameTextBox, SIGNAL(textChanged(const QString&)), this, SLOT(OnProjectNameTextChanged(const QString&)));
		QObject::connect(ui.ProjectPathTextBox, SIGNAL(textChanged(const QString&)), this, SLOT(OnProjectPathTextChanged(const QString&)));
		QObject::connect(ui.ButtonBox->button(QDialogButtonBox::Ok), SIGNAL(released()), this, SLOT(OnOkButtonClicked()));
		QObject::connect(ui.ButtonBox->button(QDialogButtonBox::Cancel), SIGNAL(released()), this, SLOT(OnOkButtonClicked()));
	}

	QString NewProjectDialog::GetProjectName() const
	{
		return ui.ProjectNameTextBox->text();
	}

	QString NewProjectDialog::GetProjectPath() const
	{
		return ui.ProjectPathTextBox->text();
	}

	void NewProjectDialog::UpdateOkButtonState()
	{
		if (!ui.ProjectNameTextBox->text().isEmpty() && !ui.ProjectPathTextBox->text().isEmpty())
		{
			ui.ButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
		}
		else
		{
			ui.ButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
		}
	}

	void NewProjectDialog::OnBrowseProjectPathButtonClicked()
	{
		QString project_path = QFileDialog::getExistingDirectory(this, tr("New Project"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

		if (project_path.isEmpty())
			return;

		ui.ProjectPathTextBox->setText(project_path);
	}

	void NewProjectDialog::OnProjectNameTextChanged(const QString &arg1)
	{
		UpdateOkButtonState();
	}

	void NewProjectDialog::OnProjectPathTextChanged(const QString &arg1)
	{
		UpdateOkButtonState();
	}

	void NewProjectDialog::OnOkButtonClicked()
	{
		accept();
	}

	void NewProjectDialog::OnCancelButtonClicked()
	{
		reject();
	}
}
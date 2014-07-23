/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  W. Haffmans <wouter@simply-life.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "manualsaveconfirmationbox.h"
#include "ui_manualsaveconfirmationbox.h"

#include <QtWidgets/QDialogButtonBox>
#include <QPushButton>

ManualSaveConfirmationBox::ManualSaveConfirmationBox(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f),
      ui(new Ui::ManualSaveConfirmationBox)
{
    ui->setupUi(this);

    QPushButton *tempButton = ui->buttonBox->addButton(tr("Open Temporary Directory..."), QDialogButtonBox::ActionRole);
    connect(tempButton, SIGNAL(clicked(bool)), SIGNAL(openTemporaryDirectory()));
    tempButton = ui->buttonBox->addButton(tr("Open Destination Directory..."), QDialogButtonBox::ActionRole);
    connect(tempButton, SIGNAL(clicked(bool)), SIGNAL(openDestinationDirectory()));
}

ManualSaveConfirmationBox::~ManualSaveConfirmationBox()
{
    delete ui;
}

#include "moc_manualsaveconfirmationbox.cpp"

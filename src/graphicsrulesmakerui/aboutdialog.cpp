/*
 * Graphics Rules Maker
 * Copyright (C) 2014-2020 Wouter Haffmans <wouter@simply-life.net>
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
 */

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include "graphicsrulesmaker/graphicsrulesmaker_config.h"

AboutDialog::AboutDialog(QWidget* parent, Qt::WindowFlags flags) :
    QDialog(parent, flags),
    ui(new Ui::AboutDialog)
{
    this->setSizeGripEnabled(false);
    ui->setupUi(this);

    if (!flags.testFlag(Qt::WindowContextHelpButtonHint)) {
        Qt::WindowFlags newFlags = windowFlags();
        newFlags = newFlags & ~Qt::WindowContextHelpButtonHint;
        setWindowFlags(newFlags);
    }

    ui->label->setText(ui->label->text().replace("GRAPHICSRULESMAKER_VERSION", GRAPHICSRULESMAKER_VERSION));
    ui->label->setText(ui->label->text().replace("GRAPHICSRULESMAKER_COPYRIGHT_YEAR", GRAPHICSRULESMAKER_COPYRIGHT_YEAR));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

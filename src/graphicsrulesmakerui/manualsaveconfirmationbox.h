/*
 * Graphics Rules Maker
 * Copyright (C) 2014 Wouter Haffmans <wouter@simply-life.net>
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

#ifndef CONFIRMATIONBOX_H
#define CONFIRMATIONBOX_H

#include <QtWidgets/QDialog>

namespace Ui
{
class ManualSaveConfirmationBox;
}

class ManualSaveConfirmationBox : public QDialog
{
    Q_OBJECT
public:
    ~ManualSaveConfirmationBox();
    ManualSaveConfirmationBox(QWidget* parent = 0, Qt::WindowFlags f = 0);

signals:
    void openTemporaryDirectory();
    void openDestinationDirectory();

private:
    Ui::ManualSaveConfirmationBox* ui;
};

#endif // CONFIRMATIONBOX_H

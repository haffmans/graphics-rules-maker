/*
 * Graphics Rules Maker
 * Copyright (C) 2014-2018 Wouter Haffmans <wouter@simply-life.net>
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>

namespace Ui {
class MainWindow;
}

class DeviceModel;
class GameWriterFactory;
class GameWriterInterface;
class VideoCardDatabase;
class QAbstractButton;
class QMessageBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(DeviceModel* model, VideoCardDatabase* videoCardDatabase, GameWriterFactory* gamePlugins);
    ~MainWindow();

private slots:
    void selectCard(int row = 0);
    void selectGame(int row = 0);
    void tabOpen(int tabIndex = 0);
    void locateGame();
    void locateGameFiles(const QString &directory);
    void setStatus(const QString &text, bool allok);
    void browseGame();
    void save();
    void saveGraphicRules();
    void saveVideoCards();

    void askAddDevices();
    void addDeviceLink(const QString &link);
    void updateDeviceStatus();

    void openTemporaryDirectory();
    void openDestinationDirectory();

    void switchLocale();
    void setLocale(const QLocale &locale);

    void about();

    void replaceWidget();

private:
    Ui::MainWindow *ui;
    DeviceModel *m_model;
    VideoCardDatabase *m_videoCardDatabase;
    GameWriterFactory *m_gamePlugins;
    GameWriterInterface *m_currentPlugin;
    QWidget *m_currentGameSettingsWidget;

    QTranslator m_libraryTranslator;
    QTranslator m_uiTranslator;
    QTranslator m_pluginTranslator;

    QString formatId(quint16 id) const;

    QStringList translationDirectories() const;
    QList<QLocale> appLocales() const;

    void setLocale(const QLocale &locale, const QString &prefix, QTranslator *translator);
    QLocale m_locale;
};

#endif // MAINWINDOW_H

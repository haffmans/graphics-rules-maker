/*
 * Graphics Rules Maker
 * Copyright (C) 2020 Wouter Haffmans <wouter@simply-life.net>
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

#include "messagehandler.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

namespace {
    int rotateKeepFiles = 5; // TODO: make this value more flexible?
}

MessageHandler* MessageHandler::m_active_instance = nullptr;

MessageHandler::MessageHandler(const QString& destination, QObject* parent)
    : QObject(parent)
    , m_destination(destination)
    , m_previous_instance(nullptr)
    , m_previous_handler(nullptr)
{
}

MessageHandler::~MessageHandler()
{
    uninstall();
}

void MessageHandler::setMessagePattern()
{
    qSetMessagePattern("[%{time}] [%{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}] %{if-category}%{category}: %{endif}%{message}");
}

void MessageHandler::install()
{
    rotate(rotateKeepFiles);

    if (m_active_instance != nullptr) {
        m_previous_instance = m_active_instance;
    }
    m_active_instance = this;

    m_previous_handler = qInstallMessageHandler(&MessageHandler::handleMessage);
    if (m_previous_handler != &MessageHandler::handleMessage) {
        // previous handler was not a MessageHandler; ignore the previous instance, which may be lower on the stack
        m_previous_instance = nullptr;
    }
}

void MessageHandler::uninstall()
{
    if (m_active_instance != this) {
        return;
    }

    // TODO: during destruction, out of order uninstalling may cause weird behavior
    if (m_previous_instance) {
        m_active_instance = m_previous_instance;
    }
    else {
        m_active_instance = nullptr;
    }

    // TODO: If another handler was installed after this one, this may behave strangely
    qInstallMessageHandler(m_previous_handler);
}

void MessageHandler::rotate(int keep)
{
    QFileInfo currentInfo(backupFile(keep));
    if (currentInfo.exists()) {
        QFile::remove(currentInfo.filePath());
    }
    QFileInfo destInfo = currentInfo;

    for (int i = keep - 1; i >= 0; --i) {
        currentInfo = backupFile(i);
        if (currentInfo.exists()) {
            QFile::rename(currentInfo.filePath(), destInfo.filePath());
        }
        destInfo = currentInfo;
    }
}

QString MessageHandler::backupFile(int n) const
{
    if (n == 0) {
        return m_destination;
    }

    QFileInfo info(m_destination);
    QString backupName = info.baseName() + QString::number(n) + "." + info.completeSuffix();
    info.setFile(info.dir(), backupName);
    return info.filePath();
}

void MessageHandler::handleMessage(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    m_active_instance->writeMessage(type, context, message);
}

void MessageHandler::writeMessage(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    QFile f(m_destination);
    f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    QTextStream out(&f);
    out << qPrintable(qFormatLogMessage(type, context, message)) << "\n";
    f.close();
}

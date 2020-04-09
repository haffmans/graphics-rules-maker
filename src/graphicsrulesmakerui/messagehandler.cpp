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

#include <QtCore/QFile>
#include <QtCore/QTextStream>

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

void MessageHandler::install()
{
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

void MessageHandler::handleMessage(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    m_active_instance->writeMessage(type, context, message);
}

void MessageHandler::writeMessage(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    QFile f(m_destination);
    f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    QTextStream out(&f);
    out << message << "\n";
    f.close();
}

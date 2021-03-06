/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef ROLISTEAMAPPLICATION_H
#define ROLISTEAMAPPLICATION_H

#include <QApplication>

#include "controller/gamecontroller.h"

class QQmlApplicationEngine;
class RolisteamApplication : public QApplication
{
    Q_OBJECT
    Q_PROPERTY(ApplicationState state READ state NOTIFY stateChanged)
public:
    enum class ApplicationState
    {
        SelectProfile,
        Playing,
        Exit
    };
    Q_ENUM(ApplicationState)

    RolisteamApplication(const QString& appName, const QString& version, int& argn, char* argv[]);

    bool notify(QObject* receiver, QEvent* e);

    GameController* gameCtrl();

    void readSettings();
    ApplicationState state() const;

    void setTranslator(const QStringList& list);

    void start();

signals:
    void stateChanged();

protected:
    void configureEnginePostLoad(QQmlApplicationEngine* engine);

private:
    void setState(ApplicationState state);

private:
    QList<QTranslator*> m_translators;
    GameController m_game;
    ApplicationState m_state= ApplicationState::SelectProfile;
};

#endif // ROLISTEAMAPPLICATION_H

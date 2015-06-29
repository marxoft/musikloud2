/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "screen.h"
#include <QDBusConnection>

Screen* Screen::self = 0;

Screen::Screen(QObject *parent) :
    QObject(parent)
{
    if (!self) {
        self = this;
    }

    QDBusConnection::systemBus().connect("",
                                         "/com/nokia/mce/signal",
                                         "com.nokia.mce.signal",
                                         "tklock_mode_ind",
                                         this,
                                         SLOT(onScreenLockStateChanged(QString)));
}

Screen::~Screen() {
    if (self == this) {
        self = 0;
    }
}

Screen* Screen::instance() {
    return self;
}

void Screen::onScreenLockStateChanged(const QString &state) {
    if (state == "locked") {
        emit screenLockStateChanged(true);
    }
    else if (state == "unlocked") {
        emit screenLockStateChanged(false);
    }
}

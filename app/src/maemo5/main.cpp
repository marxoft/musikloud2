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

#include "audioplayer.h"
#include "clipboard.h"
#include "database.h"
#include "dbusservice.h"
#include "mainwindow.h"
#include "resourcesplugins.h"
#include "screen.h"
#include "settings.h"
#include "soundcloud.h"
#include "transfers.h"
#include <QApplication>
#include <QSsl>
#include <QSslConfiguration>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setOrganizationName("MusiKloud2");
    app.setApplicationName("MusiKloud2");
    
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1);
    QSslConfiguration::setDefaultConfiguration(config);

    Settings settings;
    AudioPlayer player;
    Clipboard clipboard;
    DBusService dbus;
    ResourcesPlugins plugins;
    Screen screen;
    SoundCloud soundcloud;
    Transfers transfers;
    
    initDatabase();
    plugins.load();
    settings.setNetworkProxy();
    transfers.restoreTransfers();
    
    MainWindow window;
    window.show();
    
    QObject::connect(&clipboard, SIGNAL(textChanged(QString)), &window, SLOT(showResource(QString)));
    QObject::connect(&dbus, SIGNAL(resourceRequested(QVariantMap)), &window, SLOT(showResource(QVariantMap)));
    
    return app.exec();
}

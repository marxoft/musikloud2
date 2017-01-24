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
#include "logger.h"
#include "mainwindow.h"
#include "pluginmanager.h"
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
    
    const QStringList args = app.arguments();
    const int verbosity = args.indexOf("-v") + 1;
    
    if ((verbosity > 1) && (verbosity < args.size())) {
        Logger::setVerbosity(qMax(1, args.at(verbosity).toInt()));
    }
    else {
        Logger::setFileName(Settings::loggerFileName());
        Logger::setVerbosity(Settings::loggerVerbosity());
    }
    
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1);
    QSslConfiguration::setDefaultConfiguration(config);

    QScopedPointer<Settings> settings(Settings::instance());
    QScopedPointer<AudioPlayer> player(AudioPlayer::instance());
    QScopedPointer<Clipboard> clipboard(Clipboard::instance());
    QScopedPointer<PluginManager> plugins(PluginManager::instance());
    QScopedPointer<Screen> screen(Screen::instance());
    QScopedPointer<SoundCloud> soundcloud(SoundCloud::instance());
    QScopedPointer<Transfers> transfers(Transfers::instance());
    DBusService dbus;
    
    initDatabase();
    Settings::setNetworkProxy();
    SoundCloud::init();
    
    clipboard.data()->setEnabled(Settings::clipboardMonitorEnabled());
    plugins.data()->load();
    transfers.data()->restore();
    
    if (Settings::restorePlaybackQueueOnStartup()) {
        player.data()->restoreQueue();
    }
    
    QScopedPointer<MainWindow> window(MainWindow::instance());
    window.data()->show();
    
    QObject::connect(settings.data(), SIGNAL(clipboardMonitorEnabledChanged(bool)),
                     clipboard.data(), SLOT(setEnabled(bool)));
    QObject::connect(clipboard.data(), SIGNAL(textChanged(QString)), window.data(), SLOT(showResource(QString)));
    QObject::connect(&dbus, SIGNAL(resourceRequested(QVariantMap)), window.data(), SLOT(showResource(QVariantMap)));
    
    return app.exec();
}

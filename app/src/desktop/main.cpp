/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
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
#include "database.h"
#include "definitions.h"
#include "mainwindow.h"
#include "pluginmanager.h"
#include "settings.h"
#include "soundcloud.h"
#include "transfers.h"
#include <QApplication>
#include <QIcon>

Q_DECL_EXPORT int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setOrganizationName("MusiKloud2");
    app.setApplicationName("MusiKloud2");
    app.setApplicationVersion(VERSION_NUMBER);
    app.setWindowIcon(QIcon::fromTheme("musikloud2"));
    
    const QStringList args = app.arguments();
    const int verbosity = args.indexOf("-v") + 1;
    
    if ((verbosity > 1) && (verbosity < args.size())) {
        Logger::setVerbosity(qMax(1, args.at(verbosity).toInt()));
    }
    else {
        Logger::setFileName(Settings::loggerFileName());
        Logger::setVerbosity(Settings::loggerVerbosity());
    }
    
    QScopedPointer<AudioPlayer> player(AudioPlayer::instance());
    QScopedPointer<Settings> settings(Settings::instance());
    QScopedPointer<PluginManager> plugins(PluginManager::instance());
    QScopedPointer<SoundCloud> soundcloud(SoundCloud::instance());
    QScopedPointer<Transfers> transfers(Transfers::instance());
        
    initDatabase();
    Settings::setNetworkProxy();
    SoundCloud::init();    
    
    plugins.data()->load();
    transfers.data()->restore();
    
    if (Settings::restorePlaybackQueueOnStartup()) {
        player.data()->restoreQueue();
    }
    
    MainWindow window;
    window.show();
    
    return app.exec();
}

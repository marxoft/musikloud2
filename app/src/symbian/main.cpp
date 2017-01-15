/*
 * Copyright (C) 2017 Stuart Howarth <showarth@marxoft.co.uk>
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
#include "categorymodel.h"
#include "categorynamemodel.h"
#include "clipboard.h"
#include "concurrenttransfersmodel.h"
#include "database.h"
#include "definitions.h"
#include "loggerverbositymodel.h"
#include "maskeditem.h"
#include "mediakeycaptureitem.h"
#include "networkproxytypemodel.h"
#include "pluginartistmodel.h"
#include "plugincategorymodel.h"
#include "plugincommentmodel.h"
#include "pluginconfigmodel.h"
#include "pluginmanager.h"
#include "pluginnavmodel.h"
#include "pluginplaylistmodel.h"
#include "pluginsearchtypemodel.h"
#include "pluginsettings.h"
#include "pluginstreammodel.h"
#include "plugintrackmodel.h"
#include "resources.h"
#include "screenorientationmodel.h"
#include "searchhistorymodel.h"
#include "servicemodel.h"
#include "settings.h"
#include "soundcloud.h"
#include "soundcloudaccountmodel.h"
#include "soundcloudartistmodel.h"
#include "soundcloudcommentmodel.h"
#include "soundcloudnavmodel.h"
#include "soundcloudplaylistmodel.h"
#include "soundcloudsearchtypemodel.h"
#include "soundcloudstreammodel.h"
#include "soundcloudtrackmodel.h"
#include "trackmodel.h"
#include "transfermodel.h"
#include "transferprioritymodel.h"
#include "transfers.h"
#include "utils.h"
#include <qsoundcloud/authenticationrequest.h>
#include <qsoundcloud/resourcesrequest.h>
#include <qsoundcloud/streamsrequest.h>
#include <QApplication>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <qdeclarative.h>
#include <QSsl>
#include <QSslConfiguration>

inline void registerTypes() {
    qmlRegisterType<CategoryModel>("MusiKloud", 2, 0, "CategoryModel");
    qmlRegisterType<CategoryNameModel>("MusiKloud", 2, 0, "CategoryNameModel");
    qmlRegisterType<ConcurrentTransfersModel>("MusiKloud", 2, 0, "ConcurrentTransfersModel");    
    qmlRegisterType<LoggerVerbosityModel>("MusiKloud", 2, 0, "LoggerVerbosityModel");
    qmlRegisterType<MaskedItem>("MusiKloud", 2, 0, "MaskedItem");
    qmlRegisterType<NetworkProxyTypeModel>("MusiKloud", 2, 0, "NetworkProxyTypeModel");
    qmlRegisterType<PluginArtist>("MusiKloud", 2, 0, "PluginArtist");
    qmlRegisterType<PluginArtistModel>("MusiKloud", 2, 0, "PluginArtistModel");
    qmlRegisterType<PluginCategoryModel>("MusiKloud", 2, 0, "PluginCategoryModel");
    qmlRegisterType<PluginComment>("MusiKloud", 2, 0, "PluginComment");
    qmlRegisterType<PluginCommentModel>("MusiKloud", 2, 0, "PluginCommentModel");
    qmlRegisterType<PluginConfigModel>("MusiKloud", 2, 0, "PluginConfigModel");
    qmlRegisterType<PluginNavModel>("MusiKloud", 2, 0, "PluginNavModel");
    qmlRegisterType<PluginPlaylist>("MusiKloud", 2, 0, "PluginPlaylist");
    qmlRegisterType<PluginPlaylistModel>("MusiKloud", 2, 0, "PluginPlaylistModel");
    qmlRegisterType<PluginSearchTypeModel>("MusiKloud", 2, 0, "PluginSearchTypeModel");
    qmlRegisterType<PluginSettings>("MusiKloud", 2, 0, "PluginSettings");
    qmlRegisterType<PluginStreamModel>("MusiKloud", 2, 0, "PluginStreamModel");
    qmlRegisterType<PluginTrack>("MusiKloud", 2, 0, "PluginTrack");
    qmlRegisterType<PluginTrackModel>("MusiKloud", 2, 0, "PluginTrackModel");
    qmlRegisterType<ScreenOrientationModel>("MusiKloud", 2, 0, "ScreenOrientationModel");
    qmlRegisterType<SearchHistoryModel>("MusiKloud", 2, 0, "SearchHistoryModel");
    qmlRegisterType<SelectionModel>("MusiKloud", 2, 0, "SelectionModel");
    qmlRegisterType<ServiceModel>("MusiKloud", 2, 0, "ServiceModel");
    qmlRegisterType<SoundCloudAccountModel>("MusiKloud", 2, 0, "SoundCloudAccountModel");
    qmlRegisterType<SoundCloudArtist>("MusiKloud", 2, 0, "SoundCloudArtist");
    qmlRegisterType<SoundCloudArtistModel>("MusiKloud", 2, 0, "SoundCloudArtistModel");
    qmlRegisterType<SoundCloudComment>("MusiKloud", 2, 0, "SoundCloudComment");
    qmlRegisterType<SoundCloudCommentModel>("MusiKloud", 2, 0, "SoundCloudCommentModel");
    qmlRegisterType<SoundCloudNavModel>("MusiKloud", 2, 0, "SoundCloudNavModel");
    qmlRegisterType<SoundCloudPlaylist>("MusiKloud", 2, 0, "SoundCloudPlaylist");
    qmlRegisterType<SoundCloudPlaylistModel>("MusiKloud", 2, 0, "SoundCloudPlaylistModel");
    qmlRegisterType<SoundCloudSearchTypeModel>("MusiKloud", 2, 0, "SoundCloudSearchTypeModel");
    qmlRegisterType<SoundCloudStreamModel>("MusiKloud", 2, 0, "SoundCloudStreamModel");
    qmlRegisterType<SoundCloudTrack>("MusiKloud", 2, 0, "SoundCloudTrack");
    qmlRegisterType<SoundCloudTrackModel>("MusiKloud", 2, 0, "SoundCloudTrackModel");
    qmlRegisterType<TransferModel>("MusiKloud", 2, 0, "TransferModel");
    qmlRegisterType<TransferPriorityModel>("MusiKloud", 2, 0, "TransferPriorityModel");
    
    qmlRegisterType<QSoundCloud::AuthenticationRequest>("QSoundCloud", 1, 0, "AuthenticationRequest");
    qmlRegisterType<QSoundCloud::ResourcesRequest>("QSoundCloud", 1, 0, "ResourcesRequest");
    qmlRegisterType<QSoundCloud::StreamsRequest>("QSoundCloud", 1, 0, "StreamsRequest");
    
    qmlRegisterUncreatableType<AudioPlayer>("MusiKloud", 2, 0, "AudioPlayer", "");
    qmlRegisterUncreatableType<AudioPlayerMetaData>("MusiKloud", 2, 0, "AudioPlayerMetaData", "");
    qmlRegisterUncreatableType<MKTrack>("MusiKloud", 2, 0, "MKTrack", "");
    qmlRegisterUncreatableType<ResourcesRequest>("MusiKloud", 2, 0, "ResourcesRequest", "");
    qmlRegisterUncreatableType<TrackModel>("MusiKloud", 2, 0, "TrackModel", "");
    qmlRegisterUncreatableType<Transfer>("MusiKloud", 2, 0, "Transfer", "");

    qRegisterMetaType<ResourcesRequest::Status>("ResourcesRequest::Status");
}

Q_DECL_EXPORT int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setOrganizationName("MusiKloud2");
    app.setApplicationName("MusiKloud2");

    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1);
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    QSslConfiguration::setDefaultConfiguration(config);

    QScopedPointer<Settings> settings(Settings::instance());
    QScopedPointer<AudioPlayer> player(AudioPlayer::instance());
    QScopedPointer<Clipboard> clipboard(Clipboard::instance());
    QScopedPointer<PluginManager> plugins(PluginManager::instance());
    QScopedPointer<SoundCloud> soundcloud(SoundCloud::instance());
    QScopedPointer<Transfers> transfers(Transfers::instance());
    
    Logger logger;
    MediakeyCaptureItem volumeKeys;
    Resources resources;
    Utils utils;
    
    Logger::setFileName(Settings::loggerFileName());
    Logger::setVerbosity(Settings::loggerVerbosity());
    
    initDatabase();
    registerTypes();
    Settings::setNetworkProxy();
    SoundCloud::init();
    player.data()->setSleepTimerDuration(Settings::sleepTimerDuration());
    volumeKeys.setVolume(player.data()->volume());
    
    QDeclarativeView view;
    QDeclarativeContext *context = view.rootContext();
    
    context->setContextProperty("clipboard", clipboard.data());
    context->setContextProperty("logger", &logger);
    context->setContextProperty("mkresources", &resources);
    context->setContextProperty("player", player.data());
    context->setContextProperty("plugins", plugins.data());
    context->setContextProperty("settings", settings.data());
    context->setContextProperty("soundcloud", soundcloud.data());
    context->setContextProperty("transfers", transfers.data());
    context->setContextProperty("utils", &utils);
    context->setContextProperty("volumekeys", &volumeKeys);
    context->setContextProperty("MAX_RESULTS", MAX_RESULTS);
    context->setContextProperty("VERSION_NUMBER", VERSION_NUMBER);
        
    view.setSource(QUrl::fromLocalFile(app.applicationDirPath() + "/qml/main.qml"));
    view.showFullScreen();
    Logger::log(view.errors());
    
    QObject::connect(view.engine(), SIGNAL(warnings(QList<QDeclarativeError>)),
                     &logger, SLOT(log(QList<QDeclarativeError>)));
    QObject::connect(settings.data(), SIGNAL(loggerFileNameChanged(QString)), &logger, SLOT(setFileName(QString)));
    QObject::connect(settings.data(), SIGNAL(loggerVerbosityChanged(int)), &logger, SLOT(setVerbosity(int)));
    QObject::connect(settings.data(), SIGNAL(sleepTimerDurationChanged(int)),
                     player.data(), SLOT(setSleepTimerDuration(int)));
    QObject::connect(&volumeKeys, SIGNAL(volumeChanged(int)), player.data(), SLOT(setVolume(int)));
    
    return app.exec();
}

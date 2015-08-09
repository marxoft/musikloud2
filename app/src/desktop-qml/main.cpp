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
#include "categorymodel.h"
#include "categorynamemodel.h"
#include "clipboard.h"
#include "concurrenttransfersmodel.h"
#include "database.h"
#include "dbusservice.h"
#include "definitions.h"
#include "networkproxytypemodel.h"
#include "pluginartistmodel.h"
#include "plugincategorymodel.h"
#include "plugincommentmodel.h"
#include "pluginnavmodel.h"
#include "pluginplaylistmodel.h"
#include "pluginsearchtypemodel.h"
#include "pluginsettingsmodel.h"
#include "pluginstreammodel.h"
#include "plugintrackmodel.h"
#include "resources.h"
#include "resourcesplugins.h"
#include "resourcesrequest.h"
#include "searchhistorymodel.h"
#include "servicemodel.h"
#include "settings.h"
#include "soundcloud.h"
#include "soundcloudaccountmodel.h"
#include "soundcloudartistmodel.h"
#include "soundcloudcommentmodel.h"
#include "soundcloudconnectionmodel.h"
#include "soundcloudnavmodel.h"
#include "soundcloudplaylistmodel.h"
#include "soundcloudsearchtypemodel.h"
#include "soundcloudstreammodel.h"
#include "soundcloudtrackmodel.h"
#include "transfermodel.h"
#include "transfers.h"
#include "utils.h"
#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <qqml.h>

inline void registerTypes() {    
    qmlRegisterType<AudioPlayer>("MusiKloud", 2, 0, "AudioPlayer");
    qmlRegisterType<CategoryModel>("MusiKloud", 2, 0, "CategoryModel");
    qmlRegisterType<CategoryNameModel>("MusiKloud", 2, 0, "CategoryNameModel");
    qmlRegisterType<ConcurrentTransfersModel>("MusiKloud", 2, 0, "ConcurrentTransfersModel");
    qmlRegisterType<MKTrack>("MusiKloud", 2, 0, "Track");
    qmlRegisterType<NetworkProxyTypeModel>("MusiKloud", 2, 0, "NetworkProxyTypeModel");
    qmlRegisterType<PluginArtist>("MusiKloud", 2, 0, "PluginArtist");
    qmlRegisterType<PluginArtistModel>("MusiKloud", 2, 0, "PluginArtistModel");
    qmlRegisterType<PluginCategoryModel>("MusiKloud", 2, 0, "PluginCategoryModel");
    qmlRegisterType<PluginComment>("MusiKloud", 2, 0, "PluginComment");
    qmlRegisterType<PluginCommentModel>("MusiKloud", 2, 0, "PluginCommentModel");
    qmlRegisterType<PluginNavModel>("MusiKloud", 2, 0, "PluginNavModel");
    qmlRegisterType<PluginPlaylist>("MusiKloud", 2, 0, "PluginPlaylist");
    qmlRegisterType<PluginPlaylistModel>("MusiKloud", 2, 0, "PluginPlaylistModel");
    qmlRegisterType<PluginSearchTypeModel>("MusiKloud", 2, 0, "PluginSearchTypeModel");
    qmlRegisterType<PluginSettingsModel>("MusiKloud", 2, 0, "PluginSettingsModel");
    qmlRegisterType<PluginStreamModel>("MusiKloud", 2, 0, "PluginStreamModel");
    qmlRegisterType<PluginTrack>("MusiKloud", 2, 0, "PluginTrack");
    qmlRegisterType<PluginTrackModel>("MusiKloud", 2, 0, "PluginTrackModel");
    qmlRegisterType<ResourcesRequest>("MusiKloud", 2, 0, "ResourcesRequest");
    qmlRegisterType<SearchHistoryModel>("MusiKloud", 2, 0, "SearchHistoryModel");
    qmlRegisterType<SelectionModel>("MusiKloud", 2, 0, "SelectionModel");
    qmlRegisterType<ServiceModel>("MusiKloud", 2, 0, "ServiceModel");
    qmlRegisterType<SoundCloudAccountModel>("MusiKloud", 2, 0, "SoundCloudAccountModel");
    qmlRegisterType<SoundCloudArtist>("MusiKloud", 2, 0, "SoundCloudArtist");
    qmlRegisterType<SoundCloudArtistModel>("MusiKloud", 2, 0, "SoundCloudArtistModel");
    qmlRegisterType<SoundCloudComment>("MusiKloud", 2, 0, "SoundCloudComment");
    qmlRegisterType<SoundCloudCommentModel>("MusiKloud", 2, 0, "SoundCloudCommentModel");
    qmlRegisterType<SoundCloudConnectionModel>("MusiKloud", 2, 0, "SoundCloudConnectionModel");
    qmlRegisterType<SoundCloudNavModel>("MusiKloud", 2, 0, "SoundCloudNavModel");
    qmlRegisterType<SoundCloudPlaylist>("MusiKloud", 2, 0, "SoundCloudPlaylist");
    qmlRegisterType<SoundCloudPlaylistModel>("MusiKloud", 2, 0, "SoundCloudPlaylistModel");
    qmlRegisterType<SoundCloudSearchTypeModel>("MusiKloud", 2, 0, "SoundCloudSearchTypeModel");
    qmlRegisterType<SoundCloudStreamModel>("MusiKloud", 2, 0, "SoundCloudStreamModel");
    qmlRegisterType<SoundCloudTrack>("MusiKloud", 2, 0, "SoundCloudTrack");
    qmlRegisterType<SoundCloudTrackModel>("MusiKloud", 2, 0, "SoundCloudTrackModel");
    qmlRegisterType<TrackModel>("MusiKloud", 2, 0, "TrackModel");
    qmlRegisterType<TransferModel>("MusiKloud", 2, 0, "TransferModel");
    
    qmlRegisterUncreatableType<Transfer>("MusiKloud", 2, 0, "Transfer", "");
}

Q_DECL_EXPORT int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setOrganizationName("MusiKloud2");
    app.setApplicationName("MusiKloud2");
    app.setApplicationVersion(VERSION_NUMBER);
    app.setWindowIcon(QIcon::fromTheme("musikloud2"));

    Settings settings;
    Clipboard clipboard;
    DBusService dbus;
    Resources resources;
    ResourcesPlugins plugins;
    SoundCloud soundcloud;
    Transfers transfers;
    Utils utils;
        
    initDatabase();
    registerTypes();
    plugins.load();
    settings.setNetworkProxy();
    
    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();
    
    context->setContextProperty("Clipboard", &clipboard);
    context->setContextProperty("DBus", &dbus);
    context->setContextProperty("Plugins", &plugins);
    context->setContextProperty("Resources", &resources);
    context->setContextProperty("Settings", &settings);
    context->setContextProperty("SoundCloud", &soundcloud);
    context->setContextProperty("Transfers", &transfers);
    context->setContextProperty("Utils", &utils);
    context->setContextProperty("MAX_RESULTS", MAX_RESULTS);
    context->setContextProperty("VERSION_NUMBER", VERSION_NUMBER);

    engine.load("/opt/musikloud2/qml/main.qml");
    
    return app.exec();
}

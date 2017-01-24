TEMPLATE = app
TARGET = musikloud2

QT += network script sql

INCLUDEPATH += \
    src/audioplayer \
    src/base \
    src/plugins \
    src/soundcloud

HEADERS += \
    src/audioplayer/audioplayer.h \
    src/base/artist.h \
    src/base/categorymodel.h \
    src/base/categorynamemodel.h \
    src/base/clipboard.h \
    src/base/comment.h \
    src/base/concurrenttransfersmodel.h \
    src/base/json.h \
    src/base/loggerverbositymodel.h \
    src/base/networkproxytypemodel.h \
    src/base/playlist.h \
    src/base/resources.h \
    src/base/searchhistorymodel.h \
    src/base/selectionmodel.h \
    src/base/servicemodel.h \
    src/base/track.h \
    src/base/trackmodel.h \
    src/base/transfermodel.h \
    src/base/transferprioritymodel.h \
    src/base/transfers.h \
    src/base/utils.h \
    src/plugins/externalresourcesrequest.h \
    src/plugins/externalserviceplugin.h \
    src/plugins/javascriptresourcesrequest.h \
    src/plugins/javascriptresourcesrequestglobalobject.h \
    src/plugins/javascriptserviceplugin.h \
    src/plugins/pluginartist.h \
    src/plugins/pluginartistmodel.h \
    src/plugins/plugincategorymodel.h \
    src/plugins/plugincomment.h \
    src/plugins/plugincommentmodel.h \
    src/plugins/pluginconfigmodel.h \
    src/plugins/pluginmanager.h \
    src/plugins/pluginnavmodel.h \
    src/plugins/pluginplaylist.h \
    src/plugins/pluginplaylistmodel.h \
    src/plugins/pluginsearchtypemodel.h \
    src/plugins/pluginsettings.h \
    src/plugins/pluginstreammodel.h \
    src/plugins/plugintrack.h \
    src/plugins/plugintrackmodel.h \
    src/plugins/plugintransfer.h \
    src/plugins/resourcesrequest.h \
    src/plugins/serviceplugin.h \
    src/plugins/servicepluginconfig.h \
    src/plugins/xmlhttprequest.h \
    src/soundcloud/soundcloud.h \
    src/soundcloud/soundcloudaccountmodel.h \
    src/soundcloud/soundcloudartist.h \
    src/soundcloud/soundcloudartistmodel.h \
    src/soundcloud/soundcloudcomment.h \
    src/soundcloud/soundcloudcommentmodel.h \
    src/soundcloud/soundcloudconnectionmodel.h \
    src/soundcloud/soundcloudnavmodel.h \
    src/soundcloud/soundcloudplaylist.h \
    src/soundcloud/soundcloudplaylistmodel.h \
    src/soundcloud/soundcloudsearchtypemodel.h \
    src/soundcloud/soundcloudstreammodel.h \
    src/soundcloud/soundcloudtrack.h \
    src/soundcloud/soundcloudtrackmodel.h \
    src/soundcloud/soundcloudtransfer.h

SOURCES += \
    src/audioplayer/audioplayer.cpp \
    src/base/artist.cpp \
    src/base/categorymodel.cpp \
    src/base/clipboard.cpp \
    src/base/comment.cpp \
    src/base/json.cpp \
    src/base/playlist.cpp \
    src/base/resources.cpp \
    src/base/searchhistorymodel.cpp \
    src/base/selectionmodel.cpp \
    src/base/track.cpp \
    src/base/trackmodel.cpp \
    src/base/transfermodel.cpp \
    src/base/transfers.cpp \
    src/base/utils.cpp \
    src/plugins/externalresourcesrequest.cpp \
    src/plugins/externalserviceplugin.cpp \
    src/plugins/javascriptresourcesrequest.cpp \
    src/plugins/javascriptresourcesrequestglobalobject.cpp \
    src/plugins/javascriptserviceplugin.cpp \
    src/plugins/pluginartist.cpp \
    src/plugins/pluginartistmodel.cpp \
    src/plugins/plugincategorymodel.cpp \
    src/plugins/plugincomment.cpp \
    src/plugins/plugincommentmodel.cpp \
    src/plugins/pluginconfigmodel.cpp \
    src/plugins/pluginmanager.cpp \
    src/plugins/pluginplaylist.cpp \
    src/plugins/pluginplaylistmodel.cpp \
    src/plugins/pluginsettings.cpp \
    src/plugins/pluginstreammodel.cpp \
    src/plugins/plugintrack.cpp \
    src/plugins/plugintrackmodel.cpp \
    src/plugins/plugintransfer.cpp \
    src/plugins/servicepluginconfig.cpp \
    src/plugins/xmlhttprequest.cpp \
    src/soundcloud/soundcloud.cpp \
    src/soundcloud/soundcloudaccountmodel.cpp \
    src/soundcloud/soundcloudartist.cpp \
    src/soundcloud/soundcloudartistmodel.cpp \
    src/soundcloud/soundcloudcomment.cpp \
    src/soundcloud/soundcloudcommentmodel.cpp \
    src/soundcloud/soundcloudconnectionmodel.cpp \
    src/soundcloud/soundcloudnavmodel.cpp \
    src/soundcloud/soundcloudplaylist.cpp \
    src/soundcloud/soundcloudplaylistmodel.cpp \
    src/soundcloud/soundcloudstreammodel.cpp \
    src/soundcloud/soundcloudtrack.cpp \
    src/soundcloud/soundcloudtrackmodel.cpp \
    src/soundcloud/soundcloudtransfer.cpp

maemo5 {
    QT += dbus maemo5 webkit
    CONFIG += mobility12
    MOBILITY += multimedia
    
    LIBS += -L/usr/lib -lqsoundcloud
    CONFIG += link_prl
    PKGCONFIG += libqsoundcloud
    
    INCLUDEPATH += \
        src/dbus \
        src/maemo5 \
        src/maemo5/plugins \
        src/maemo5/soundcloud
    
    HEADERS += \
        src/dbus/dbusservice.h \
        src/maemo5/aboutdialog.h \
        src/maemo5/accountdelegate.h \
        src/maemo5/artistdelegate.h \
        src/maemo5/categoriesdialog.h \
        src/maemo5/categorydelegate.h \
        src/maemo5/commentdelegate.h \
        src/maemo5/customcommanddialog.h \
        src/maemo5/database.h \
        src/maemo5/definitions.h \
        src/maemo5/dialog.h \
        src/maemo5/drawing.h \
        src/maemo5/filterbox.h \
        src/maemo5/image.h \
        src/maemo5/imagecache.h \
        src/maemo5/listview.h \
        src/maemo5/logger.h \
        src/maemo5/mainwindow.h \
        src/maemo5/navdelegate.h \
        src/maemo5/networkproxydialog.h \
        src/maemo5/newcategorydialog.h \
        src/maemo5/nowplayingaction.h \
        src/maemo5/nowplayingdelegate.h \
        src/maemo5/nowplayingwindow.h \
        src/maemo5/playlistdelegate.h \
        src/maemo5/pluginsettingsdialog.h \
        src/maemo5/qwebviewselectionsuppressor.h \
        src/maemo5/screen.h \
        src/maemo5/searchhistorydialog.h \
        src/maemo5/settings.h \
        src/maemo5/settingsdialog.h \
        src/maemo5/stackedwindow.h \
        src/maemo5/textbrowser.h \
        src/maemo5/trackdelegate.h \
        src/maemo5/transfer.h \
        src/maemo5/transferswindow.h \
        src/maemo5/valueselector.h \
        src/maemo5/valueselectoraction.h \
        src/maemo5/webview.h \
        src/maemo5/plugins/pluginartistswindow.h \
        src/maemo5/plugins/pluginartistwindow.h \
        src/maemo5/plugins/plugincategorieswindow.h \
        src/maemo5/plugins/plugindownloaddialog.h \
        src/maemo5/plugins/pluginplaylistswindow.h \
        src/maemo5/plugins/pluginplaylistwindow.h \
        src/maemo5/plugins/pluginsearchdialog.h \
        src/maemo5/plugins/plugintrackswindow.h \
        src/maemo5/plugins/plugintrackwindow.h \
        src/maemo5/plugins/pluginview.h \
        src/maemo5/soundcloud/soundcloudaccountswindow.h \
        src/maemo5/soundcloud/soundcloudartistswindow.h \
        src/maemo5/soundcloud/soundcloudartistwindow.h \
        src/maemo5/soundcloud/soundcloudauthdialog.h \
        src/maemo5/soundcloud/soundcloudcommentdialog.h \
        src/maemo5/soundcloud/soundclouddownloaddialog.h \
        src/maemo5/soundcloud/soundcloudplaylistswindow.h \
        src/maemo5/soundcloud/soundcloudplaylistwindow.h \
        src/maemo5/soundcloud/soundcloudsearchdialog.h \
        src/maemo5/soundcloud/soundcloudtrackswindow.h \
        src/maemo5/soundcloud/soundcloudtrackwindow.h \
        src/maemo5/soundcloud/soundcloudview.h
    
    SOURCES += \
        src/dbus/dbusservice.cpp \
        src/maemo5/aboutdialog.cpp \
        src/maemo5/accountdelegate.cpp \
        src/maemo5/artistdelegate.cpp \
        src/maemo5/categoriesdialog.cpp \
        src/maemo5/categorydelegate.cpp \
        src/maemo5/commentdelegate.cpp \
        src/maemo5/customcommanddialog.cpp \
        src/maemo5/dialog.cpp \
        src/maemo5/filterbox.cpp \
        src/maemo5/image.cpp \
        src/maemo5/imagecache.cpp \
        src/maemo5/listview.cpp \
        src/maemo5/logger.cpp \
        src/maemo5/main.cpp \
        src/maemo5/mainwindow.cpp \
        src/maemo5/navdelegate.cpp \
        src/maemo5/networkproxydialog.cpp \
        src/maemo5/newcategorydialog.cpp \
        src/maemo5/nowplayingaction.cpp \
        src/maemo5/nowplayingdelegate.cpp \
        src/maemo5/nowplayingwindow.cpp \
        src/maemo5/playlistdelegate.cpp \
        src/maemo5/pluginsettingsdialog.cpp \
        src/maemo5/screen.cpp \
        src/maemo5/searchhistorydialog.cpp \
        src/maemo5/settings.cpp \
        src/maemo5/settingsdialog.cpp \
        src/maemo5/stackedwindow.cpp \
        src/maemo5/textbrowser.cpp \
        src/maemo5/trackdelegate.cpp \
        src/maemo5/transfer.cpp \
        src/maemo5/transferswindow.cpp \
        src/maemo5/valueselector.cpp \
        src/maemo5/valueselectoraction.cpp \
        src/maemo5/webview.cpp \
        src/maemo5/plugins/pluginartistswindow.cpp \
        src/maemo5/plugins/pluginartistwindow.cpp \
        src/maemo5/plugins/plugincategorieswindow.cpp \
        src/maemo5/plugins/plugindownloaddialog.cpp \
        src/maemo5/plugins/pluginplaylistswindow.cpp \
        src/maemo5/plugins/pluginplaylistwindow.cpp \
        src/maemo5/plugins/pluginsearchdialog.cpp \
        src/maemo5/plugins/plugintrackswindow.cpp \
        src/maemo5/plugins/plugintrackwindow.cpp \
        src/maemo5/plugins/pluginview.cpp \
        src/maemo5/soundcloud/soundcloudaccountswindow.cpp \
        src/maemo5/soundcloud/soundcloudartistswindow.cpp \
        src/maemo5/soundcloud/soundcloudartistwindow.cpp \
        src/maemo5/soundcloud/soundcloudauthdialog.cpp \
        src/maemo5/soundcloud/soundcloudcommentdialog.cpp \
        src/maemo5/soundcloud/soundclouddownloaddialog.cpp \
        src/maemo5/soundcloud/soundcloudplaylistswindow.cpp \
        src/maemo5/soundcloud/soundcloudplaylistwindow.cpp \
        src/maemo5/soundcloud/soundcloudsearchdialog.cpp \
        src/maemo5/soundcloud/soundcloudtrackswindow.cpp \
        src/maemo5/soundcloud/soundcloudtrackwindow.cpp \
        src/maemo5/soundcloud/soundcloudview.cpp
    
    dbus_service.files = dbus/maemo5/org.marxoft.musikloud2.service
    dbus_service.path = /usr/share/dbus-1/services
    
    dbus_interface.files = dbus/maemo5/org.marxoft.musikloud2.xml
    dbus_interface.path = /usr/share/dbus-1/interfaces
    
    desktopfile.files = desktop/maemo5/musikloud2.desktop
    desktopfile.path = /usr/share/applications/hildon
    
    icon.files = desktop/maemo5/64/musikloud2.png
    icon.path = /usr/share/icons/hicolor/64x64/apps
    
    target.path = /opt/musikloud2/bin
    
    INSTALLS += \
        dbus_service \
        dbus_interface \
        desktopfile \
        icon \
        target

} else:symbian {
    QT += declarative
    CONFIG += mobility qtcomponents
    MOBILITY += multimedia
    
    TARGET.UID3 = 0xE77ABF1C
    TARGET.CAPABILITY += NetworkServices ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x8000000
    TARGET.EPOCSTACKSIZE = 0x14000
    
    VERSION = 0.2.0
    ICON = desktop/symbian/musikloud2.svg
    
    MMP_RULES += "DEBUGGABLE_UDEBONLY"

    LIBS += -lqsoundcloud
    LIBS += -L\\epoc32\\release\\armv5\\lib -lremconcoreapi
    LIBS += -L\\epoc32\\release\\armv5\\lib -lremconinterfacebase
    
    INCLUDEPATH += \
        MW_LAYER_SYSTEMINCLUDE \
        src/symbian
    
    HEADERS += \
        src/symbian/database.h \
        src/symbian/definitions.h \
        src/symbian/logger.h \
        src/symbian/maskeditem.h \
        src/symbian/maskeffect.h \
        src/symbian/mediakeycaptureitem.h \
        src/symbian/screenorientationmodel.h \
        src/symbian/settings.h \
        src/symbian/transfer.h
    
    SOURCES += \
        src/symbian/logger.cpp \
        src/symbian/main.cpp \
        src/symbian/maskeditem.cpp \
        src/symbian/maskeffect.cpp \
        src/symbian/mediakeycaptureitem.cpp \
        src/symbian/settings.cpp \
        src/symbian/transfer.cpp
    
    qml_base.sources = \
        src/symbian/qml/AboutPage.qml \
        src/symbian/qml/AboutPluginPage.qml \
        src/symbian/qml/AboutPluginsPage.qml \
        src/symbian/qml/AccountDelegate.qml \
        src/symbian/qml/AddFolderPage.qml \
        src/symbian/qml/AddUrlPage.qml \
        src/symbian/qml/AppWindow.qml \
        src/symbian/qml/ArtistDelegate.qml \
        src/symbian/qml/Avatar.qml \
        src/symbian/qml/BackToolButton.qml \
        src/symbian/qml/CategorySettingsPage.qml \
        src/symbian/qml/CommentDelegate.qml \
        src/symbian/qml/DualTextDelegate.qml \
        src/symbian/qml/EditPage.qml \
        src/symbian/qml/EditCategoryPage.qml \
        src/symbian/qml/FileBrowserPage.qml \
        src/symbian/qml/GeneralSettingsPage.qml \
        src/symbian/qml/HeaderLabel.qml \
        src/symbian/qml/KeyNavFlickable.qml \
        src/symbian/qml/LoggingSettingsPage.qml \
        src/symbian/qml/LogPage.qml \
        src/symbian/qml/main.qml \
        src/symbian/qml/MainPage.qml \
        src/symbian/qml/MyButton.qml \
        src/symbian/qml/MyContextMenu.qml \
        src/symbian/qml/MyDialog.qml \
        src/symbian/qml/MyFlickable.qml \
        src/symbian/qml/MyInfoBanner.qml \
        src/symbian/qml/MyListItem.qml \
        src/symbian/qml/MyListItemText.qml \
        src/symbian/qml/MyListView.qml \
        src/symbian/qml/MyMenu.qml \
        src/symbian/qml/MyPage.qml \
        src/symbian/qml/MyQueryDialog.qml \
        src/symbian/qml/MySelectionDialog.qml \
        src/symbian/qml/MyStatusBar.qml \
        src/symbian/qml/MySwitch.qml \
        src/symbian/qml/MyTextField.qml \
        src/symbian/qml/MyToolButton.qml \
        src/symbian/qml/NetworkSettingsPage.qml \
        src/symbian/qml/NowPlayingButton.qml \
        src/symbian/qml/NowPlayingPage.qml \
        src/symbian/qml/NowPlayingTrackPage.qml \
        src/symbian/qml/PlaybackQueueDelegate.qml \
        src/symbian/qml/PlaybackQueuePage.qml \
        src/symbian/qml/PlaybackSettingsPage.qml \
        src/symbian/qml/PlaylistDelegate.qml \
        src/symbian/qml/PlayFolderPage.qml \
        src/symbian/qml/PlayUrlPage.qml \
        src/symbian/qml/PluginSettingsPage.qml \
        src/symbian/qml/PluginsSettingsPage.qml \
        src/symbian/qml/PopupLoader.qml \
        src/symbian/qml/SettingsPage.qml \
        src/symbian/qml/TextDelegate.qml \
        src/symbian/qml/TextInputPage.qml \
        src/symbian/qml/Thumbnail.qml \
        src/symbian/qml/TrackDelegate.qml \
        src/symbian/qml/TransferDelegate.qml \
        src/symbian/qml/TransfersPage.qml \
        src/symbian/qml/ValueDialog.qml \
        src/symbian/qml/ValueListItem.qml \
        src/symbian/qml/ValueSelector.qml \
        src/symbian/qml/VolumeControl.qml
    
    qml_base.path = !:/Private/e77abf1c/qml
    
    qml_plugins.sources = \
        src/symbian/qml/plugins/PluginArtistPage.qml \
        src/symbian/qml/plugins/PluginArtistsPage.qml \
        src/symbian/qml/plugins/PluginCategoriesPage.qml \
        src/symbian/qml/plugins/PluginCommentsPage.qml \
        src/symbian/qml/plugins/PluginDownloadPage.qml \
        src/symbian/qml/plugins/PluginPlaylistPage.qml \
        src/symbian/qml/plugins/PluginPlaylistsPage.qml \
        src/symbian/qml/plugins/PluginSearchPage.qml \
        src/symbian/qml/plugins/PluginTrackPage.qml \
        src/symbian/qml/plugins/PluginTracksPage.qml \
        src/symbian/qml/plugins/PluginView.qml
    
    qml_plugins.path = !:/Private/e77abf1c/qml/plugins
    
    qml_soundcloud.sources = \
        src/symbian/qml/soundcloud/SoundCloudAccountsPage.qml \
        src/symbian/qml/soundcloud/SoundCloudArtistPage.qml \
        src/symbian/qml/soundcloud/SoundCloudArtistsPage.qml \
        src/symbian/qml/soundcloud/SoundCloudAuthPage.qml \
        src/symbian/qml/soundcloud/SoundCloudCommentPage.qml \
        src/symbian/qml/soundcloud/SoundCloudCommentsPage.qml \
        src/symbian/qml/soundcloud/SoundCloudDownloadPage.qml \
        src/symbian/qml/soundcloud/SoundCloudPlaylistPage.qml \
        src/symbian/qml/soundcloud/SoundCloudPlaylistsPage.qml \
        src/symbian/qml/soundcloud/SoundCloudSearchPage.qml \
        src/symbian/qml/soundcloud/SoundCloudTrackPage.qml \
        src/symbian/qml/soundcloud/SoundCloudTracksPage.qml \
        src/symbian/qml/soundcloud/SoundCloudView.qml
    
    qml_soundcloud.path = !:/Private/e77abf1c/qml/soundcloud
    
    images.sources = \
        src/symbian/qml/images/artist.svg \
        src/symbian/qml/images/avatar-frame.png \
        src/symbian/qml/images/avatar-mask.png \
        src/symbian/qml/images/close.svg \
        src/symbian/qml/images/document.svg \
        src/symbian/qml/images/download.svg \
        src/symbian/qml/images/folder.svg \
        src/symbian/qml/images/music.svg \
        src/symbian/qml/images/musikloud2.png \
        src/symbian/qml/images/next.svg \
        src/symbian/qml/images/ok.svg \
        src/symbian/qml/images/pause-active.png \
        src/symbian/qml/images/play-active.png \
        src/symbian/qml/images/playlist.svg \
        src/symbian/qml/images/previous.svg \
        src/symbian/qml/images/repeat.svg \
        src/symbian/qml/images/shuffle.svg \
        src/symbian/qml/images/stop-active.png \
        src/symbian/qml/images/track.svg \
        src/symbian/qml/images/up.png \
        src/symbian/qml/images/upload.svg \
        src/symbian/qml/images/volume.png \
        src/symbian/qml/images/volume-mute.png
    
    images.path = !:/Private/e77abf1c/qml/images
    
    vendorinfo += "%{\"Stuart Howarth\"}" ":\"Stuart Howarth\""
    qtcomponentsdep = "; Default dependency to Qt Quick Components for Symbian library" \
        "(0x200346DE), 1, 1, 0, {\"Qt Quick components for Symbian\"}"

    musikloud2_deployment.pkg_prerules += vendorinfo qtcomponentsdep
    
    DEPLOYMENT.display_name = MusiKloud2
    
    DEPLOYMENT += \
        musikloud2_deployment \
        qml_base \
        qml_plugins \
        qml_soundcloud \
        images

} else:unix {
    greaterThan(QT_MAJOR_VERSION,4) {
        QT += multimedia widgets
    }
    else {
        CONFIG += mobility
        MOBILITY += multimedia
    }
    
    LIBS += -L/usr/lib -lqsoundcloud
    CONFIG += link_prl
    PKGCONFIG += libqsoundcloud
    
    INCLUDEPATH += \
        src/desktop \
        src/desktop/plugins \
        src/desktop/soundcloud
    
    HEADERS += \
        src/desktop/aboutdialog.h \
        src/desktop/categorysettingspage.h \
        src/desktop/customcommanddialog.h \
        src/desktop/database.h \
        src/desktop/definitions.h \
        src/desktop/drawing.h \
        src/desktop/generalsettingspage.h \
        src/desktop/image.h \
        src/desktop/imagecache.h \
        src/desktop/itemmetadata.h \
        src/desktop/logger.h \
        src/desktop/mainwindow.h \
        src/desktop/networksettingspage.h \
        src/desktop/page.h \
        src/desktop/playbackqueuepage.h \
        src/desktop/pluginsettingspage.h \
        src/desktop/pluginssettingspage.h \
        src/desktop/searchdialog.h \
        src/desktop/settings.h \
        src/desktop/settingsdialog.h \
        src/desktop/settingspage.h \
        src/desktop/transfer.h \
        src/desktop/transferdelegate.h \
        src/desktop/transferspage.h \
        src/desktop/treeview.h \
        src/desktop/plugins/plugindownloaddialog.h \
        src/desktop/plugins/plugintrackspage.h \
        src/desktop/soundcloud/soundclouddownloaddialog.h \
        src/desktop/soundcloud/soundcloudtrackspage.h
        
    SOURCES += \
        src/desktop/aboutdialog.cpp \
        src/desktop/categorysettingspage.cpp \
        src/desktop/customcommanddialog.cpp \
        src/desktop/generalsettingspage.cpp \
        src/desktop/image.cpp \
        src/desktop/imagecache.cpp \
        src/desktop/itemmetadata.cpp \
        src/desktop/logger.cpp \
        src/desktop/main.cpp \
        src/desktop/mainwindow.cpp \
        src/desktop/networksettingspage.cpp \
        src/desktop/page.cpp \
        src/desktop/playbackqueuepage.cpp \
        src/desktop/pluginsettingspage.cpp \
        src/desktop/pluginssettingspage.cpp \
        src/desktop/searchdialog.cpp \
        src/desktop/settings.cpp \
        src/desktop/settingsdialog.cpp \
        src/desktop/settingspage.cpp \
        src/desktop/transfer.cpp \
        src/desktop/transferdelegate.cpp \
        src/desktop/transferspage.cpp \
        src/desktop/treeview.cpp \
        src/desktop/plugins/plugindownloaddialog.cpp \
        src/desktop/plugins/plugintrackspage.cpp \
        src/desktop/soundcloud/soundclouddownloaddialog.cpp \
        src/desktop/soundcloud/soundcloudtrackspage.cpp
    
    desktop.files = desktop/desktop/musikloud2.desktop
    desktop.path = /usr/share/applications
    
    icon64.files = desktop/desktop/64/musikloud2.png
    icon64.path = /usr/share/icons/hicolor/64x64/apps
    
    icon48.files = desktop/desktop/48/musikloud2.png
    icon48.path = /usr/share/icons/hicolor/48x48/apps
    
    icon22.files = desktop/desktop/22/musikloud2.png
    icon22.path = /usr/share/icons/hicolor/22x22/apps
    
    icon16.files = desktop/desktop/16/musikloud2.png
    icon16.path = /usr/share/icons/hicolor/16x16/apps
        
    target.path = /usr/bin
    
    INSTALLS += \
        desktop \
        icon64 \
        icon48 \
        icon22 \
        icon16 \
        target
}

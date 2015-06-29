TEMPLATE = app
TARGET = musikloud2

#DEFINES += MUSIKLOUD_DEBUG

QT += network sql xml

INCLUDEPATH += \
    src/audioplayer \
    src/base \
    src/plugins \
    src/soundcloud

HEADERS += \
    src/audioplayer/audioplayer.h \
    src/audioplayer/trackmodel.h \
    src/base/artist.h \
    src/base/categorymodel.h \
    src/base/categorynamemodel.h \
    src/base/clipboard.h \
    src/base/comment.h \
    src/base/concurrenttransfersmodel.h \
    src/base/database.h \
    src/base/json.h \
    src/base/localtrack.h \
    src/base/networkproxytypemodel.h \
    src/base/playlist.h \
    src/base/resources.h \
    src/base/searchhistorymodel.h \
    src/base/selectionmodel.h \
    src/base/servicemodel.h \
    src/base/settings.h \
    src/base/track.h \
    src/base/transfer.h \
    src/base/transfers.h \
    src/base/utils.h \
    src/plugins/pluginartist.h \
    src/plugins/pluginartistmodel.h \
    src/plugins/plugincategorymodel.h \
    src/plugins/plugincomment.h \
    src/plugins/plugincommentmodel.h \
    src/plugins/pluginnavmodel.h \
    src/plugins/pluginplaylist.h \
    src/plugins/pluginplaylistmodel.h \
    src/plugins/pluginsearchtypemodel.h \
    src/plugins/pluginsettingsmodel.h \
    src/plugins/pluginstreammodel.h \
    src/plugins/plugintrack.h \
    src/plugins/plugintrackmodel.h \
    src/plugins/plugintransfer.h \
    src/plugins/resourcesplugins.h \
    src/plugins/resourcesrequest.h \
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
    src/audioplayer/trackmodel.cpp \
    src/base/artist.cpp \
    src/base/categorymodel.cpp \
    src/base/clipboard.cpp \
    src/base/comment.cpp \
    src/base/json.cpp \
    src/base/localtrack.cpp \
    src/base/playlist.cpp \
    src/base/resources.cpp \
    src/base/searchhistorymodel.cpp \
    src/base/selectionmodel.cpp \
    src/base/settings.cpp \
    src/base/track.cpp \
    src/base/transfer.cpp \
    src/base/transfers.cpp \
    src/base/utils.cpp \
    src/plugins/pluginartist.cpp \
    src/plugins/pluginartistmodel.cpp \
    src/plugins/plugincategorymodel.cpp \
    src/plugins/plugincomment.cpp \
    src/plugins/plugincommentmodel.cpp \
    src/plugins/pluginplaylist.cpp \
    src/plugins/pluginplaylistmodel.cpp \
    src/plugins/pluginstreammodel.cpp \
    src/plugins/plugintrack.cpp \
    src/plugins/plugintrackmodel.cpp \
    src/plugins/plugintransfer.cpp \
    src/plugins/resourcesplugins.cpp \
    src/plugins/resourcesrequest.cpp \
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
    QT += maemo5 webkit
    
    LIBS += -L/usr/lib -lqsoundcloud
    CONFIG += link_prl
    PKGCONFIG += libqsoundcloud
    
    INCLUDEPATH += \
        src/maemo5 \
        src/maemo5/plugins \
        src/maemo5/soundcloud
    
    HEADERS += \
        src/base/transfermodel.h \
        src/base/transferprioritymodel.h \
        src/maemo5/aboutdialog.h \
        src/maemo5/accountdelegate.h \
        src/maemo5/artistdelegate.h \
        src/maemo5/categoriesdialog.h \
        src/maemo5/commentdelegate.h \
        src/maemo5/dialog.h \
        src/maemo5/definitions.h \
        src/maemo5/drawing.h \
        src/maemo5/filterbox.h \
        src/maemo5/image.h \
        src/maemo5/imagecache.h \
        src/maemo5/listview.h \
        src/maemo5/mainwindow.h \
        src/maemo5/navdelegate.h \
        src/maemo5/networkproxydialog.h \
        src/maemo5/newcategorydialog.h \
        src/maemo5/nowplayingaction.h \
        src/maemo5/nowplayingdelegate.h \
        src/maemo5/nowplayingwindow.h \
        src/maemo5/playlistdelegate.h \
        src/maemo5/qwebviewselectionsuppressor.h \
        src/maemo5/screen.h \
        src/maemo5/searchhistorydialog.h \
        src/maemo5/settingsdialog.h \
        src/maemo5/stackedwindow.h \
        src/maemo5/textbrowser.h \
        src/maemo5/trackdelegate.h \
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
        src/maemo5/plugins/pluginsettingscheckbox.h \
        src/maemo5/plugins/pluginsettingsdialog.h \
        src/maemo5/plugins/pluginsettingslineedit.h \
        src/maemo5/plugins/pluginsettingsselector.h \
        src/maemo5/plugins/pluginsettingsslider.h \
        src/maemo5/plugins/pluginsettingsspinbox.h \
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
        src/base/transfermodel.cpp \
        src/maemo5/aboutdialog.cpp \
        src/maemo5/accountdelegate.cpp \
        src/maemo5/artistdelegate.cpp \
        src/maemo5/categoriesdialog.cpp \
        src/maemo5/commentdelegate.cpp \
        src/maemo5/dialog.cpp \
        src/maemo5/filterbox.cpp \
        src/maemo5/image.cpp \
        src/maemo5/imagecache.cpp \
        src/maemo5/listview.cpp \
        src/maemo5/main.cpp \
        src/maemo5/mainwindow.cpp \
        src/maemo5/navdelegate.cpp \
        src/maemo5/networkproxydialog.cpp \
        src/maemo5/newcategorydialog.cpp \
        src/maemo5/nowplayingaction.cpp \
        src/maemo5/nowplayingdelegate.cpp \
        src/maemo5/nowplayingwindow.cpp \
        src/maemo5/playlistdelegate.cpp \
        src/maemo5/screen.cpp \
        src/maemo5/searchhistorydialog.cpp \
        src/maemo5/settingsdialog.cpp \
        src/maemo5/stackedwindow.cpp \
        src/maemo5/textbrowser.cpp \
        src/maemo5/trackdelegate.cpp \
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
        src/maemo5/plugins/pluginsettingscheckbox.cpp \
        src/maemo5/plugins/pluginsettingsdialog.cpp \
        src/maemo5/plugins/pluginsettingslineedit.cpp \
        src/maemo5/plugins/pluginsettingsselector.cpp \
        src/maemo5/plugins/pluginsettingsslider.cpp \
        src/maemo5/plugins/pluginsettingsspinbox.cpp \
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
    
    desktopfile.files = desktop/maemo5/musikloud2.desktop
    desktopfile.path = /usr/share/applications/hildon
    
    icon.files = desktop/maemo5/64/musikloud2.png
    icon.path = /usr/share/icons/hicolor/64x64/apps
    
    INSTALLS += \
        desktopfile \
        icon

} else:unix {
    QT += qml quick widgets
    
    LIBS += -L/usr/lib -lqsoundcloud
    CONFIG += link_prl
    PKGCONFIG += libqsoundcloud
    
    INCLUDEPATH += src/desktop-qml
    
    HEADERS += \
        src/base/transfermodel.h \
        src/base/transferprioritymodel.h \
        src/desktop-qml/definitions.h
    
    SOURCES += \
        src/base/transfermodel.cpp \
        src/desktop-qml/main.cpp
    
    base_qml.files = $$files(src/desktop-qml/qml/*.qml)
    base_qml.path = /opt/musikloud2/qml
    
    plugins_qml.files = $$files(src/desktop-qml/qml/plugins/*.qml)
    plugins_qml.path = /opt/musikloud2/qml/plugins
    
    soundcloud_qml.files = $$files(src/desktop-qml/qml/soundcloud/*.qml)
    soundcloud_qml.path = /opt/musikloud2/qml/soundcloud
    
    desktopfile.files = desktop/desktop/musikloud2.desktop
    desktopfile.path = /usr/share/applications

    icon64.files = desktop/desktop/64/musikloud2.png
    icon64.path = /usr/share/icons/hicolor/64/apps
    
    icon48.files = desktop/desktop/48/musikloud2.png
    icon48.path = /usr/share/icons/hicolor/48x48/apps
    
    icon22.files = desktop/desktop/22/musikloud2.png
    icon22.path = /usr/share/icons/hicolor/22/apps
    
    icon16.files = desktop/desktop/16/musikloud2.png
    icon16.path = /usr/share/icons/hicolor/16/apps
    
    INSTALLS += \
        base_qml \
        plugins_qml \
        soundcloud_qml \
        desktopfile \
        icon64 \
        icon48 \
        icon22 \
        icon16
}

unix:!symbian {
    QT += dbus
    
    INCLUDEPATH += src/dbus
    
    HEADERS += src/dbus/dbusservice.h
    SOURCES += src/dbus/dbusservice.cpp
    
    dbus_service.files = dbus/org.marxoft.musikloud2.service
    dbus_service.path = /usr/share/dbus-1/services

    dbus_interface.files = dbus/org.marxoft.musikloud2.xml
    dbus_interface.path = /usr/share/dbus-1/interfaces

    target.path = /opt/musikloud2/bin
    
    INSTALLS += dbus_service dbus_interface
}

greaterThan(QT_MAJOR_VERSION,4) {
    QT += multimedia
} else:maemo5 {
    CONFIG += mobility12
    MOBILITY += multimedia
} else {
    CONFIG += mobility
    MOBILITY += multimedia
}

contains(DEFINES,FETCH_LOCAL_METADATA) {
    greaterThan(QT_MAJOR_VERSION,4) {
        QT += docgallery
    } else {
        MOBILITY += gallery
    }
}

INSTALLS += target

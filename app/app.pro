TEMPLATE = app
TARGET = musikloud2

DEFINES += MUSIKLOUD_DEBUG

QT += network sql xml

greaterThan(QT_MAJOR_VERSION,4) {
    QT += multimedia
} else:maemo5 {
    CONFIG += mobility12
    MOBILITY += multimedia
} else {
    CONFIG += mobility
    MOBILITY += multimedia
}

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
    src/base/localemodel.h \
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
    src/soundcloud/soundcloudactivity.h \
    src/soundcloud/soundcloudactivitymodel.h \
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
    src/soundcloud/soundcloudactivity.cpp \
    src/soundcloud/soundcloudactivitymodel.cpp \
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

unix {
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
    
    desktopfile.path = /usr/share/applications
    desktopfile.files = desktop/desktop/musikloud2.desktop

    icon64.path = /usr/share/icons/hicolor/64/apps
    icon64.files = desktop/desktop/64/musikloud2.png

    icon48.path = /usr/share/icons/hicolor/48x48/apps
    icon48.files = desktop/desktop/48/musikloud2.png

    icon22.path = /usr/share/icons/hicolor/22/apps
    icon22.files = desktop/desktop/22/musikloud2.png

    icon16.path = /usr/share/icons/hicolor/16/apps
    icon16.files = desktop/desktop/16/musikloud2.png
    
    INSTALLS += \
        base_qml \
        plugins_qml \
        soundcloud_qml \
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

INSTALLS += target

TEMPLATE = app
TARGET = musikloud2-podcasts
QT += network xml
QT -= gui

HEADERS += \
    src/json.h \
    src/rss.h
    
SOURCES += \
    src/json.cpp \
    src/main.cpp \
    src/rss.cpp

symbian {    
    plugin.files = plugin/symbian/podcasts.plugin
    plugin.path = !:/.config/MusiKloud2/plugins
    
    settings.files = settings/podcasts.settings
    settings.path = !:/.config/MusiKloud2/plugins/podcasts
} else:unix {    
    plugin.files = plugin/linux/podcasts.plugin
    plugin.path = /opt/musikloud2/plugins
    
    settings.files = settings/podcasts.settings
    settings.path = /opt/musikloud2/plugins/podcasts
    
    target.path = /opt/musikloud2/plugins/podcasts
    
    INSTALLS += plugin settings target
}
    
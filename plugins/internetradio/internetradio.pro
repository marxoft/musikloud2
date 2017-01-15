TEMPLATE = subdirs

maemo5 {
    config.files = musikloud2-internetradio.json
    config.path = /opt/musikloud2/plugins

    plugin.files = musikloud2-internetradio.js
    plugin.path = /opt/musikloud2/plugins

    INSTALLS += \
        config \
        plugin

} else:symbian {
    config.sources = musikloud2-internetradio.json
    config.path = !:/musikloud2/plugins

    plugin.sources = musikloud2-internetradio.js
    plugin.path = !:/musikloud2/plugins

    vendorinfo += "%{\"Stuart Howarth\"}" ":\"Stuart Howarth\""
    internetradio_deployment.pkg_prerules += vendorinfo

    DEPLOYMENT.display_name = MusiKloud2 Internet Radio
    DEPLOYMENT += \
        internetradio_deployment \
        config \
        plugin

} else:unix {
    config.files = musikloud2-internetradio.json
    config.path = /usr/share/musikloud2/plugins

    plugin.files = musikloud2-internetradio.js
    plugin.path = /usr/share/musikloud2/plugins

    INSTALLS += \
        config \
        plugin
}

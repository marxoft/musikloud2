TEMPLATE = subdirs
SUBDIRS += \
    app

contains(MEEGO_EDITION,harmattan) {
    SUBDIRS += \
        ../qsoundcloud/src

    app.depends += \
        ../qsoundcloud/src
}

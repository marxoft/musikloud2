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

#include "transfers.h"
#include "definitions.h"
#include "logger.h"
#include "plugintransfer.h"
#include "resources.h"
#include "settings.h"
#include "soundcloudtransfer.h"
#include "utils.h"
#include <QNetworkAccessManager>
#include <QSettings>

Transfers* Transfers::self = 0;

inline static Transfer* createTransfer(const QString &service, QObject *parent = 0) {
    if (service == Resources::SOUNDCLOUD) {
        return new SoundCloudTransfer(parent);
    }
    
    return new PluginTransfer(service, parent);
}

Transfers::Transfers() :
    QObject(),
    m_nam(new QNetworkAccessManager(this))
{
    m_queueTimer.setSingleShot(true);
    m_queueTimer.setInterval(1000);
    
    connect(&m_queueTimer, SIGNAL(timeout()), this, SLOT(startNextTransfers()));
    connect(Settings::instance(), SIGNAL(maximumConcurrentTransfersChanged(int)),
            this, SLOT(onMaximumConcurrentTransfersChanged(int)));
}

Transfers::~Transfers() {
    save();
    self = 0;
}

Transfers* Transfers::instance() {
    return self ? self : self = new Transfers;
}

int Transfers::active() const {
    return m_active.size();
}

int Transfers::count() const {
    return m_transfers.size();
}

void Transfers::addDownloadTransfer(const QString &service, const QString &trackId, const QString &streamId,
                                    const QUrl &streamUrl, const QString &title, const QString &category,
                                    const QString &customCommand, bool customCommandOverrideEnabled) {
    Logger::log(QString("Transfers::addDownloadTransfer(). Service: %1, Track ID: %2, Stream ID: %3, Stream URL: %4, Title: %5, Category: %6, Command: %7").arg(service).arg(trackId).arg(streamId).arg(streamUrl.toString())
                .arg(title).arg(category).arg(customCommand));
    Transfer *transfer = createTransfer(service, this);
    transfer->setNetworkAccessManager(m_nam);
    transfer->setId(Utils::createId());
    transfer->setDownloadPath(Settings::downloadPath() + ".incomplete/" + transfer->id());
    transfer->setFileName(title);
    transfer->setCategory(category);
    transfer->setTrackId(trackId);
    transfer->setStreamId(streamId);
    transfer->setStreamUrl(streamUrl);
    transfer->setTitle(title);
    transfer->setCustomCommand(customCommand);
    transfer->setCustomCommandOverrideEnabled(customCommandOverrideEnabled);
    
    connect(transfer, SIGNAL(statusChanged()), this, SLOT(onTransferStatusChanged()));
    
    m_transfers << transfer;
    emit countChanged(count());
    emit transferAdded(transfer);
    
    if (Settings::startTransfersAutomatically()) {
        transfer->queue();
    }
}

Transfer* Transfers::get(int i) const {
    if ((i >= 0) && (i < m_transfers.size())) {
        return m_transfers.at(i);
    }
    
    return 0;
}

Transfer* Transfers::get(const QString &id) const {
    foreach (Transfer *transfer, m_transfers) {
        if (transfer->id() == id) {
            return transfer;
        }
    }
    
    return 0;
}

bool Transfers::start() {
    foreach (Transfer *transfer, m_transfers) {
        transfer->queue();
    }
    
    return true;
}

bool Transfers::pause() {
    Logger::log("Transfers::pause()", Logger::HighVerbosity);
    
    foreach (Transfer *transfer, m_transfers) {
        transfer->pause();
    }
    
    return true;
}

bool Transfers::start(const QString &id) {
    Logger::log("Transfers::start(). ID: " + id, Logger::HighVerbosity);
    
    if (Transfer *transfer = get(id)) {
        transfer->queue();
        return true;
    }
    
    return false;
}

bool Transfers::pause(const QString &id) {
    Logger::log("Transfers::pause(). ID: " + id, Logger::HighVerbosity);
    
    if (Transfer *transfer = get(id)) {
        transfer->pause();
        return true;
    }
    
    return false;
}

bool Transfers::cancel(const QString &id) {
    Logger::log("Transfers::cancel(). ID: " + id, Logger::HighVerbosity);
    
    if (Transfer *transfer = get(id)) {
        transfer->cancel();
        return true;
    }
    
    return false;
}

void Transfers::save() {
    QSettings settings(APP_CONFIG_PATH + "transfers.conf", QSettings::IniFormat);
    settings.clear();
    settings.beginWriteArray("transfers");
    
    for (int i = 0; i < m_transfers.size(); i++) {
        const Transfer *transfer = m_transfers.at(i);
        settings.setArrayIndex(i);
        settings.setValue("id", transfer->id());
        settings.setValue("downloadPath", transfer->downloadPath());
        settings.setValue("fileName", transfer->fileName());
        settings.setValue("category", transfer->category());
        settings.setValue("priority", Transfer::Priority(transfer->priority()));
        settings.setValue("size", transfer->size());
        settings.setValue("service", transfer->service());
        settings.setValue("trackId", transfer->trackId());
        settings.setValue("streamId", transfer->streamId());
        settings.setValue("streamUrl", transfer->streamUrl());
        settings.setValue("title", transfer->title());
        settings.setValue("customCommand", transfer->customCommand());
        settings.setValue("customCommandOverrideEnabled", transfer->customCommandOverrideEnabled());
    }
    
    settings.endArray();
    Logger::log(QString("Transfers::save(). %1 transfers saved").arg(m_transfers.size()), Logger::LowVerbosity);
}

void Transfers::restore() {
    QSettings settings(APP_CONFIG_PATH + "transfers.conf", QSettings::IniFormat);
    const int size = settings.beginReadArray("transfers");
    Logger::log(QString("Transfers::restore(). %1 transfers restored").arg(size), Logger::LowVerbosity);

    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        Transfer *transfer = createTransfer(settings.value("service", Resources::SOUNDCLOUD).toString(), this);
        transfer->setId(settings.value("id").toString());
        transfer->setDownloadPath(settings.value("downloadPath").toString());
        transfer->setFileName(settings.value("fileName").toString());
        transfer->setCategory(settings.value("category").toString());
        transfer->setPriority(Transfer::Priority(settings.value("priority", 1).toInt()));
        transfer->setSize(settings.value("size").toLongLong());
        transfer->setTrackId(settings.value("trackId").toString());
        transfer->setStreamId(settings.value("streamId").toString());
        transfer->setStreamUrl(settings.value("streamUrl").toString());
        transfer->setTitle(settings.value("title").toString());
        transfer->setCustomCommand(settings.value("customCommand").toString());
        transfer->setCustomCommandOverrideEnabled(settings.value("customCommandOverrideEnabled", false).toBool());
        connect(transfer, SIGNAL(statusChanged()), this, SLOT(onTransferStatusChanged()));
    
        m_transfers << transfer;
        emit countChanged(count());
        emit transferAdded(transfer);
    
        if (Settings::startTransfersAutomatically()) {
            transfer->queue();
        }
    }
    
    settings.endArray();
}

void Transfers::getNextTransfers() {
    const int max = Settings::maximumConcurrentTransfers();
    
    for (int priority = Transfer::HighPriority; priority <= Transfer::LowPriority; priority++) {
        foreach (Transfer *transfer, m_transfers) {
            if ((transfer->priority() == priority) && (transfer->status() == Transfer::Queued)) {
                if (active() < max) {
                    addActiveTransfer(transfer);
                }
                else {
                    Logger::log("Transfers::getNextTransfers(). Maximum concurrent transfers reached",
                                Logger::MediumVerbosity);
                    return;
                }
            }
        }
    }
}

void Transfers::startNextTransfers() {
    getNextTransfers();
    
    foreach (Transfer *transfer, m_active) {
        transfer->start();
    }
}

void Transfers::removeTransfer(Transfer *transfer) {
    removeActiveTransfer(transfer);
    m_transfers.removeOne(transfer);
    transfer->deleteLater();
    emit countChanged(count());
}

void Transfers::addActiveTransfer(Transfer *transfer) {
    Logger::log("Transfers::addActiveTransfer(). ID: " + transfer->id(), Logger::MediumVerbosity);
    m_active << transfer;
    emit activeChanged(active());
}

void Transfers::removeActiveTransfer(Transfer *transfer) {
    Logger::log("Transfers::removeActiveTransfer(). ID: " + transfer->id(), Logger::MediumVerbosity);
    m_active.removeOne(transfer);
    emit activeChanged(active());
}

void Transfers::onTransferStatusChanged() {
    if (Transfer *transfer = qobject_cast<Transfer*>(sender())) {
        switch (transfer->status()) {
        case Transfer::Paused:
        case Transfer::Failed:
            removeActiveTransfer(transfer);
            break;
        case Transfer::Canceled:
        case Transfer::Completed:
            removeTransfer(transfer);
            save();
            break;
        case Transfer::Queued:
            break;
        default:
            return;
        }
                
        if (active() < Settings::maximumConcurrentTransfers()) {
            m_queueTimer.start();
        }
    }
}

void Transfers::onMaximumConcurrentTransfersChanged(int maximum) {
    int act = active();
    
    if (act < maximum) {
        startNextTransfers();
    }
    else if (act > maximum) {
        for (int priority = Transfer::LowPriority; priority >= Transfer::HighPriority; priority--) {
            for (int i = m_active.size() - 1; i >= 0; i--) {
                if (m_active.at(i)->priority() == priority) {
                    m_active.at(i)->pause();
                    act--;
                
                    if (act == maximum) {
                        return;
                    }
                }
            }
        }
    }
}

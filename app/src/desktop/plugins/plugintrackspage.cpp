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

#include "plugintrackspage.h"
#include "audioplayer.h"
#include "plugindownloaddialog.h"
#include "resources.h"
#include "settings.h"
#include "transfers.h"
#include "treeview.h"
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QVBoxLayout>

PluginTracksPage::PluginTracksPage(const QString &service, QWidget *parent) :
    Page(parent),
    m_model(new PluginTrackModel(this)),
    m_view(new MKTreeView(this)),
    m_layout(new QVBoxLayout(this))
{
    m_model->setService(service);
    
    m_view->setModel(m_model);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    m_view->setAlternatingRowColors(true);
    m_view->setSelectionBehavior(QTreeView::SelectRows);
    m_view->setEditTriggers(QTreeView::NoEditTriggers);
    m_view->setItemsExpandable(false);
    m_view->setUniformRowHeights(true);
    m_view->setAllColumnsShowFocus(true);
    m_view->setRootIsDecorated(false);
    m_view->header()->restoreState(Settings::tracksHeaderViewState());
    m_view->setItemMetaDataEnabled(true);
    m_view->setItemMetaDataThumbnailRole(PluginTrackModel::LargeThumbnailUrlRole);
    
    QList<ItemMetaDataRole> roles;
    roles << ItemMetaDataRole(tr("Title"), PluginTrackModel::TitleRole);
    roles << ItemMetaDataRole(tr("Artist"), PluginTrackModel::ArtistRole);
    roles << ItemMetaDataRole(tr("Genre"), PluginTrackModel::GenreRole);
    roles << ItemMetaDataRole(tr("Duration"), PluginTrackModel::DurationStringRole);
    m_view->setItemMetaDataTextRoles(roles);

    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_model, SIGNAL(statusChanged(ResourcesRequest::Status)),
            this, SLOT(onModelStatusChanged(ResourcesRequest::Status)));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(playTrack(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

void PluginTracksPage::closeEvent(QCloseEvent *e) {
    Settings::setTracksHeaderViewState(m_view->header()->saveState());
    Page::closeEvent(e);
}

PluginTracksPage::Status PluginTracksPage::status() const {
    return m_status;
}

void PluginTracksPage::setStatus(PluginTracksPage::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

QString PluginTracksPage::statusText() const {
    return m_statusText;
}

void PluginTracksPage::setStatusText(const QString &text) {
    if (text != statusText()) {
        m_statusText = text;
        emit statusTextChanged(text);
    }
}

void PluginTracksPage::cancel() {
    m_model->cancel();
}

void PluginTracksPage::reload() {
    m_model->reload();
}

void PluginTracksPage::list(const QString &resourceId) {
    m_model->list(resourceId);
}

void PluginTracksPage::search(const QString &query, const QString &order) {
    m_model->search(query, order);
}

void PluginTracksPage::downloadTrack(const QModelIndex &index) {
    const QString trackId = index.data(PluginTrackModel::IdRole).toString();
    const QString title = index.data(PluginTrackModel::TitleRole).toString();
    const QUrl streamUrl = index.data(PluginTrackModel::StreamUrlRole).toString();
    
    PluginDownloadDialog dialog(m_model->service(), this);
    dialog.list(trackId, streamUrl.isEmpty());

    if (dialog.exec() == QDialog::Accepted) {
        Transfers::instance()->addDownloadTransfer(m_model->service(), trackId, dialog.streamId(), streamUrl,
                                                   title, dialog.category(), dialog.customCommand(),
                                                   dialog.customCommandOverrideEnabled());
    }
}

void PluginTracksPage::playTrack(const QModelIndex &index) {
    if (PluginTrack *track = m_model->get(index.row())) {
        AudioPlayer::instance()->playTrack(track);
    }
}

void PluginTracksPage::queueTrack(const QModelIndex &index) {
    if (PluginTrack *track = m_model->get(index.row())) {
        AudioPlayer::instance()->addTrack(track);
    }
}

void PluginTracksPage::showContextMenu(const QPoint &pos) {
    const QModelIndex index = m_view->currentIndex();
    
    if (!index.isValid()) {
        return;
    }
    
    if (index.data(PluginTrackModel::DownloadableRole).toBool()) {
        QMenu menu(this);
        QAction *playAction = menu.addAction(QIcon::fromTheme("media-playback-start"), tr("&Play"));
        QAction *queueAction = menu.addAction(QIcon::fromTheme("list-add"), tr("&Queue"));
        QAction *downloadAction = menu.addAction(QIcon::fromTheme("document-save"), tr("&Download"));
        
        foreach (const QVariant &v, index.data(PluginTrackModel::ActionsRole).toList()) {
            const QVariantMap a = v.toMap();
            QAction *pa = menu.addAction(a.value("label").toString());
            pa->setData(a);
        }
        
        QAction *action = menu.exec(m_view->mapToGlobal(pos));
        
        if (!action) {
            return;
        }
    
        if (action == playAction) {
            playTrack(m_view->currentIndex());
        }
        else if (action == queueAction) {
            queueTrack(m_view->currentIndex());
        }
        else if (action == downloadAction) {
            downloadTrack(m_view->currentIndex());
        }
        else if (PluginTrack *track = m_model->get(index.row())) {
            const QVariantMap data = action->data().toMap();
            const QString method = data.value("method").toString();
            
            if (method == "del") {
                track->del(data.value("type").toString(), data.value("id").toString());
            }
            else if (method == "insert") {
                track->insert(data.value("type").toString(), data.value("id").toString());
            }
        }
    }
    else {
        QMenu menu(this);
        QAction *playAction = menu.addAction(QIcon::fromTheme("media-playback-start"), tr("&Play"));
        QAction *queueAction = menu.addAction(QIcon::fromTheme("list-add"), tr("&Queue"));
        
        foreach (const QVariant &v, index.data(PluginTrackModel::ActionsRole).toList()) {
            const QVariantMap a = v.toMap();
            QAction *pa = menu.addAction(a.value("label").toString());
            pa->setData(a);
        }
        
        QAction *action = menu.exec(m_view->mapToGlobal(pos));
        
        if (!action) {
            return;
        }
    
        if (action == playAction) {
            playTrack(m_view->currentIndex());
        }
        else if (action == queueAction) {
            queueTrack(m_view->currentIndex());
        }
        else if (PluginTrack *track = m_model->get(index.row())) {
            const QVariantMap data = action->data().toMap();
            const QString method = data.value("method").toString();
            
            if (method == "del") {
                track->del(data.value("type").toString(), data.value("id").toString());
            }
            else if (method == "insert") {
                track->insert(data.value("type").toString(), data.value("id").toString());
            }
        }
    }
}

void PluginTracksPage::onModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        setStatus(Loading);
        setStatusText(tr("Loading tracks"));
        break;
    case ResourcesRequest::Failed:
        setStatus(Ready);
        setStatusText(m_model->errorString());
        QMessageBox::critical(this, tr("Error"), m_model->errorString());
        break;
    default:
        setStatus(Ready);
        setStatusText(tr("Finished"));
        break;
    }
}

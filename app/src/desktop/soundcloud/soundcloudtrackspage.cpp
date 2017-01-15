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

#include "soundcloudtrackspage.h"
#include "audioplayer.h"
#include "soundclouddownloaddialog.h"
#include "resources.h"
#include "settings.h"
#include "transfers.h"
#include "treeview.h"
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QVBoxLayout>

SoundCloudTracksPage::SoundCloudTracksPage(QWidget *parent) :
    Page(parent),
    m_model(new SoundCloudTrackModel(this)),
    m_view(new MKTreeView(this)),
    m_layout(new QVBoxLayout(this)),
    m_status(Null)
{
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
    m_view->setItemMetaDataThumbnailRole(SoundCloudTrackModel::LargeThumbnailUrlRole);
    
    QList<ItemMetaDataRole> roles;
    roles << ItemMetaDataRole(tr("Title"), SoundCloudTrackModel::TitleRole);
    roles << ItemMetaDataRole(tr("Artist"), SoundCloudTrackModel::ArtistRole);
    roles << ItemMetaDataRole(tr("Genre"), SoundCloudTrackModel::GenreRole);
    roles << ItemMetaDataRole(tr("Duration"), SoundCloudTrackModel::DurationStringRole);
    m_view->setItemMetaDataTextRoles(roles);

    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_model, SIGNAL(statusChanged(QSoundCloud::ResourcesRequest::Status)),
            this, SLOT(onModelStatusChanged(QSoundCloud::ResourcesRequest::Status)));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(playTrack(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

void SoundCloudTracksPage::closeEvent(QCloseEvent *e) {
    Settings::setTracksHeaderViewState(m_view->header()->saveState());
    Page::closeEvent(e);
}

SoundCloudTracksPage::Status SoundCloudTracksPage::status() const {
    return m_status;
}

void SoundCloudTracksPage::setStatus(SoundCloudTracksPage::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

QString SoundCloudTracksPage::statusText() const {
    return m_statusText;
}

void SoundCloudTracksPage::setStatusText(const QString &text) {
    if (text != statusText()) {
        m_statusText = text;
        emit statusTextChanged(text);
    }
}

void SoundCloudTracksPage::cancel() {
    m_model->cancel();
}

void SoundCloudTracksPage::reload() {
    m_model->reload();
}

void SoundCloudTracksPage::get(const QString &resourcePath, const QVariantMap &filters) {
    m_model->get(resourcePath, filters);
}

void SoundCloudTracksPage::downloadTrack(const QModelIndex &index) {
    const QString trackId = index.data(SoundCloudTrackModel::IdRole).toString();
    const QString title = index.data(SoundCloudTrackModel::TitleRole).toString();
    SoundCloudDownloadDialog dialog(this);
    dialog.get(trackId);

    if (dialog.exec() == QDialog::Accepted) {
        Transfers::instance()->addDownloadTransfer(Resources::SOUNDCLOUD, trackId, dialog.streamId(),
                                                   QUrl(), title, dialog.category(), dialog.customCommand(),
                                                   dialog.customCommandOverrideEnabled());
    }
}

void SoundCloudTracksPage::playTrack(const QModelIndex &index) {
    if (SoundCloudTrack *track = m_model->get(index.row())) {
        AudioPlayer::instance()->playTrack(track);
    }
}

void SoundCloudTracksPage::queueTrack(const QModelIndex &index) {
    if (SoundCloudTrack *track = m_model->get(index.row())) {
        AudioPlayer::instance()->addTrack(track);
    }
}

void SoundCloudTracksPage::showContextMenu(const QPoint &pos) {
    if (!m_view->currentIndex().isValid()) {
        return;
    }
    
    QMenu menu(this);
    QAction *playAction = menu.addAction(QIcon::fromTheme("media-playback-start"), tr("&Play"));
    QAction *queueAction = menu.addAction(QIcon::fromTheme("list-add"), tr("&Queue"));
    QAction *downloadAction = menu.addAction(QIcon::fromTheme("document-save"), tr("&Download"));
    QAction *action = menu.exec(m_view->mapToGlobal(pos));
    
    if (action == playAction) {
        playTrack(m_view->currentIndex());
    }
    else if (action == queueAction) {
        queueTrack(m_view->currentIndex());
    }
    else if (action == downloadAction) {
        downloadTrack(m_view->currentIndex());
    }
}

void SoundCloudTracksPage::onModelStatusChanged(QSoundCloud::ResourcesRequest::Status status) {
    switch (status) {
    case QSoundCloud::ResourcesRequest::Loading:
        setStatus(Loading);
        setStatusText(tr("Loading tracks"));
        break;
    case QSoundCloud::ResourcesRequest::Failed:
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

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

#include "nowplayingaction.h"
#include "imagecache.h"
#include "nowplayingwindow.h"
#include "utils.h"
#ifdef MUSIKLOUD_DEBUG
#include <QDebug>
#endif

NowPlayingAction::NowPlayingAction(QWidget *parent) :
    QWidgetAction(parent)
{
    connect(AudioPlayer::instance(), SIGNAL(queueCountChanged(int)), this, SLOT(onCountChanged(int)));
    onCountChanged(AudioPlayer::instance()->queueCount());
}

QWidget* NowPlayingAction::createWidget(QWidget *parent) {
    return new NowPlayingButton(parent);
}

void NowPlayingAction::onCountChanged(int count) {
    setEnabled(count > 0);
}

NowPlayingButton::NowPlayingButton(QWidget *parent) :
    QMaemo5ValueButton(parent),
    m_cache(new ImageCache)
{
    connect(this, SIGNAL(clicked()), this, SLOT(showNowPlayingWindow()));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
}

NowPlayingButton::~NowPlayingButton() {
    delete m_cache;
    m_cache = 0;
}

void NowPlayingButton::showEvent(QShowEvent *e) {
    connectPlaybackSignals();
    QMaemo5ValueButton::showEvent(e);
}

void NowPlayingButton::hideEvent(QHideEvent *e) {
    QMaemo5ValueButton::hideEvent(e);
    disconnectPlaybackSignals();
}

void NowPlayingButton::connectPlaybackSignals() {
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "NowPlayingButton::connectPlaybackSignals";
#endif
    connect(AudioPlayer::instance(), SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    onCurrentIndexChanged(AudioPlayer::instance()->currentIndex());
}

void NowPlayingButton::disconnectPlaybackSignals() {
#ifdef MUSIKLOUD_DEBUG
    qDebug() << "NowPlayingButton::disconnectPlaybackSignals";
#endif
    disconnect(AudioPlayer::instance(), SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
}

void NowPlayingButton::showNowPlayingWindow() {
    NowPlayingWindow *window = new NowPlayingWindow(StackedWindow::currentWindow());
    window->show();
}

void NowPlayingButton::onCurrentIndexChanged(int index) {
    if (MKTrack *track = AudioPlayer::instance()->currentTrack()) {
        const int maxWidth = width() - iconSize().width() - 36;
        QFont small = font();
        small.setPointSize(13);
        
        const QString title = fontMetrics().elidedText(QString("%1/%2 - %3").arg(index + 1)
                              .arg(AudioPlayer::instance()->queueCount()).arg(track->title().isEmpty()
                              ? tr("Unknown title") : track->title()), Qt::ElideRight, maxWidth);
                              
        const QString artist = QFontMetrics(small).elidedText(track->artist().isEmpty()
                               ? tr("Unknown artist") : track->artist(), Qt::ElideRight, maxWidth);
        
        QUrl thumbnailUrl = track->thumbnailUrl();
        
        if (thumbnailUrl.isEmpty()) {
            thumbnailUrl = Utils::findThumbnailUrl(track->url());
        }
                               
        const QImage thumbnail = m_cache->image(thumbnailUrl, iconSize());
    
        if (!thumbnail.isNull()) {
            setIcon(QIcon(QPixmap::fromImage(thumbnail)));
        }
        else {
            setIcon(QIcon::fromTheme("mediaplayer_default_album"));
        }
        
        setText(title);
        setValueText(artist);
    }
}

void NowPlayingButton::onImageReady() {
    if (MKTrack *track = AudioPlayer::instance()->currentTrack()) {
        QUrl thumbnailUrl = track->thumbnailUrl();
        
        if (thumbnailUrl.isEmpty()) {
            thumbnailUrl = Utils::findThumbnailUrl(track->url());
        }
        
        const QImage thumbnail = m_cache->image(thumbnailUrl, iconSize());
    
        if (!thumbnail.isNull()) {
            setIcon(QIcon(QPixmap::fromImage(thumbnail)));
            return;
        }
    }
    
    setIcon(QIcon::fromTheme("mediaplayer_default_album"));
}

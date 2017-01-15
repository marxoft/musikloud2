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

#ifndef NOWPLAYINGACTION_H
#define NOWPLAYINGACTION_H

#include <QWidgetAction>
#include <QMaemo5ValueButton>

class ImageCache;

class NowPlayingAction : public QWidgetAction
{
    Q_OBJECT

public:
    explicit NowPlayingAction(QWidget *parent = 0);
    
    QWidget* createWidget(QWidget *parent);

private Q_SLOTS:
    void onCountChanged(int count);
};

class NowPlayingButton : public QMaemo5ValueButton
{
    Q_OBJECT

public:
    explicit NowPlayingButton(QWidget *parent = 0);
    ~NowPlayingButton();
    
protected:
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);

private Q_SLOTS:
    void showNowPlayingWindow();
    
    void onMetaDataChanged();
    void onImageReady();

private:
    void connectPlaybackSignals();
    void disconnectPlaybackSignals();
    
    ImageCache *m_cache;
};

#endif // NOWPLAYINGACTION_H

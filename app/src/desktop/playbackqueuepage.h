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

#ifndef PLAYBACKQUEUEPAGE_H
#define PLAYBACKQUEUEPAGE_H

#include "page.h"

class MKTreeView;
class QVBoxLayout;

class PlaybackQueuePage : public Page
{
    Q_OBJECT

public:
    explicit PlaybackQueuePage(QWidget *parent = 0);

private Q_SLOTS:
    void downloadTrack(const QModelIndex &index);
    void playTrack(const QModelIndex &index);
    void removeTrack(const QModelIndex &index);
    
    void showContextMenu(const QPoint &pos);
    
    void updateWindowTitle();
    
private:
    virtual void closeEvent(QCloseEvent *e);
    virtual void dragEnterEvent(QDragEnterEvent *e);
    virtual void dropEvent(QDropEvent *e);
    
    MKTreeView *m_view;

    QVBoxLayout *m_layout;
};

#endif // PLAYBACKQUEUEPAGE_H

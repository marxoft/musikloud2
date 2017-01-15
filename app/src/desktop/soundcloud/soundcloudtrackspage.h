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

#ifndef SOUNDCLOUDTRACKSPAGE_H
#define SOUNDCLOUDTRACKSPAGE_H

#include "page.h"
#include "soundcloudtrackmodel.h"

class MKTreeView;
class QVBoxLayout;

class SoundCloudTracksPage : public Page
{
    Q_OBJECT

public:
    explicit SoundCloudTracksPage(QWidget *parent = 0);
    
    virtual Status status() const;
    virtual QString statusText() const;

public Q_SLOTS:    
    virtual void cancel();
    virtual void reload();
    
    void get(const QString &resourcePath, const QVariantMap &filters = QVariantMap());

private Q_SLOTS:
    void downloadTrack(const QModelIndex &index);
    void playTrack(const QModelIndex &index);
    void queueTrack(const QModelIndex &index);
    
    void showContextMenu(const QPoint &pos);
    
    void onModelStatusChanged(QSoundCloud::ResourcesRequest::Status status);

private:
    virtual void closeEvent(QCloseEvent *e);
    
    void setStatus(Status s);
    void setStatusText(const QString &text);
    
    SoundCloudTrackModel *m_model;
    
    MKTreeView *m_view;
        
    QVBoxLayout *m_layout;
    
    Status m_status;
    QString m_statusText;
};

#endif // SOUNDCLOUDTRACKSPAGE_H

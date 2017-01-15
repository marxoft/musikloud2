/*
 * Copyright (C) 2017 Stuart Howarth <showarth@marxoft.co.uk>
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

#ifndef MediakeyCaptureItem_H
#define MediakeyCaptureItem_H
 
#include <QObject>
#include <qdeclarative.h>
#include <remconcoreapitargetobserver.h>
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>
 
class QTimer;
class MediakeyCaptureItemPrivate;
class MediakeyCaptureItem : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(int volume READ volume NOTIFY volumeChanged)

public:
    MediakeyCaptureItem(QObject *parent = 0);
    
    int volume() const;
    void setVolume(int volume);

private:
    void increaseVolume();
    void decreaseVolume();

private Q_SLOTS:
    void onTimerTriggered();

Q_SIGNALS:
    void pressed();
    void volumeChanged(int volume);
 
private:
    MediakeyCaptureItemPrivate *d_ptr;
    
    friend class MediakeyCaptureItemPrivate;
};

QML_DECLARE_TYPE(MediakeyCaptureItem)

#endif // MediakeyCaptureItem_H

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

#include "mediakeycaptureitem.h"
#include <QTimer>

// A private class to access Symbian RemCon API
class MediakeyCaptureItemPrivate : public QObject, public MRemConCoreApiTargetObserver
{
public:
    MediakeyCaptureItemPrivate(MediakeyCaptureItem *parent);
    ~MediakeyCaptureItemPrivate();
    virtual void MrccatoCommand(TRemConCoreApiOperationId aOperationId,
                                TRemConCoreApiButtonAction aButtonAct);

public:
    QTimer *m_timer;
    int m_volume;
    bool m_volumeDownPressed;
    bool m_volumeUpPressed;

private:
    CRemConInterfaceSelector* iInterfaceSelector;
    CRemConCoreApiTarget*     iCoreTarget;
    MediakeyCaptureItem *d_ptr;
};

// Consructor
MediakeyCaptureItem::MediakeyCaptureItem(QObject *parent): QObject(parent)
{
    d_ptr = new MediakeyCaptureItemPrivate(this);
    d_ptr->m_timer = new QTimer(this);
    d_ptr->m_timer->setInterval(150);
    d_ptr->m_timer->setSingleShot(false);
    connect(d_ptr->m_timer, SIGNAL(timeout()), this, SLOT(onTimerTriggered()));
}

// Constructor
MediakeyCaptureItemPrivate::MediakeyCaptureItemPrivate(MediakeyCaptureItem *parent): d_ptr(parent), m_volume(50)
{
    QT_TRAP_THROWING(iInterfaceSelector = CRemConInterfaceSelector::NewL());
    QT_TRAP_THROWING(iCoreTarget = CRemConCoreApiTarget::NewL(*iInterfaceSelector, *this));
    iInterfaceSelector->OpenTargetL();
}

// Destructor
MediakeyCaptureItemPrivate::~MediakeyCaptureItemPrivate(){
    delete iInterfaceSelector;
    delete  iCoreTarget;
}

// Callback when media keys are pressed
void MediakeyCaptureItemPrivate::MrccatoCommand(TRemConCoreApiOperationId aOperationId,
                                                TRemConCoreApiButtonAction aButtonAct)
{
    if (aButtonAct == ERemConCoreApiButtonRelease) {
        m_timer->stop();
        m_volumeDownPressed = false;
        m_volumeUpPressed = false;
        return;
    }

    switch (aOperationId) {
    case ERemConCoreApiVolumeUp:
        m_volumeUpPressed = true;
        m_volumeDownPressed = false;
        emit d_ptr->pressed();
        d_ptr->increaseVolume();
        if (aButtonAct == ERemConCoreApiButtonPress) m_timer->start();
        break;
    case ERemConCoreApiVolumeDown:
        m_volumeDownPressed = true;
        m_volumeUpPressed = false;
        emit d_ptr->pressed();
        d_ptr->decreaseVolume();
        if (aButtonAct == ERemConCoreApiButtonPress) m_timer->start();
        break;
    default:
        m_volumeDownPressed = false;
        m_volumeUpPressed = false;
        return;
    }
}

int MediakeyCaptureItem::volume() const {
    return d_ptr->m_volume;
}

void MediakeyCaptureItem::setVolume(int volume) {
    if (volume != d_ptr->m_volume) {
        d_ptr->m_volume = volume;
        emit volumeChanged(volume);
    }
}

void MediakeyCaptureItem::increaseVolume() {
    if (volume() < 95) {
        setVolume(volume() + 5);
    }
    else {
        setVolume(100);
    }
}

void MediakeyCaptureItem::decreaseVolume() {
    if (volume() > 5) {
        setVolume(volume() - 5);
    }
    else {
        setVolume(0);
    }
}

void MediakeyCaptureItem::onTimerTriggered() {
    emit pressed();

    if (d_ptr->m_volumeUpPressed) {
        increaseVolume();
    }
    else if (d_ptr->m_volumeDownPressed) {
        decreaseVolume();
    }
}

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

#ifndef SOUNDCLOUDDOWNLOADDIALOG_H
#define SOUNDCLOUDDOWNLOADDIALOG_H

#include "dialog.h"
#include "soundcloudstreammodel.h"

class CategoryNameModel;
class ValueSelector;
class QScrollArea;
class QCheckBox;
class QLineEdit;
class QDialogButtonBox;
class QHBoxLayout;

class SoundCloudDownloadDialog : public Dialog
{
    Q_OBJECT

    Q_PROPERTY(QString trackId READ trackId)
    Q_PROPERTY(QString streamId READ streamId)
    Q_PROPERTY(QString category READ category)
    Q_PROPERTY(QString customCommand READ customCommand)
    Q_PROPERTY(bool customCommandOverrideEnabled READ customCommandOverrideEnabled)
    
public:
    explicit SoundCloudDownloadDialog(QWidget *parent = 0);

    QString trackId() const;

    QString streamId() const;

    QString category() const;

    QString customCommand() const;
    bool customCommandOverrideEnabled() const;

public Q_SLOTS:
    virtual void accept();

    void get(const QString &trackId);

private Q_SLOTS:
    void onStreamModelStatusChanged(QSoundCloud::StreamsRequest::Status status);
    
private:
    SoundCloudStreamModel *m_streamModel;
    CategoryNameModel *m_categoryModel;
    
    QScrollArea *m_scrollArea;
    QCheckBox *m_commandCheckBox;
    QLineEdit *m_commandEdit;
    ValueSelector *m_streamSelector;
    ValueSelector *m_categorySelector;
    QDialogButtonBox *m_buttonBox;
    QHBoxLayout *m_layout;

    QString m_trackId;
};

#endif // SOUNDCLOUDDOWNLOADDIALOG_H

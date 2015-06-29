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

#ifndef SOUNDCLOUDACCOUNTSWINDOW_H
#define SOUNDCLOUDACCOUNTSWINDOW_H

#include "stackedwindow.h"

namespace QSoundCloud {
    class AuthenticationRequest;
    class ResourcesRequest;
}

class SoundCloudAccountModel;
class ListView;
class QVBoxLayout;
class QModelIndex;
class QLabel;
class QAction;

class SoundCloudAccountsWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit SoundCloudAccountsWindow(StackedWindow *parent);
    
private:
    void initAuthRequest();
    void initUserRequest();
    
private Q_SLOTS:
    void removeAccount();
    void selectAccount(const QModelIndex &index);
    
    void showAuthDialog();
    void submitCode(const QString &code);
    
    void onAuthRequestFinished();
    void onUserRequestFinished();
    void onModelCountChanged(int count);
    
private:
    QSoundCloud::AuthenticationRequest *m_authRequest;
    QSoundCloud::ResourcesRequest *m_userRequest;
    SoundCloudAccountModel *m_model;
    
    ListView *m_view;
    QAction *m_addAction;
    QAction *m_removeAction;
    QLabel *m_label;
    QVBoxLayout *m_layout;
};
    
#endif // SOUNDCLOUDACCOUNTSWINDOW_H

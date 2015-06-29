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

#include "soundcloudaccountswindow.h"
#include "accountdelegate.h"
#include "listview.h"
#include "soundcloud.h"
#include "soundcloudaccountmodel.h"
#include "soundcloudauthdialog.h"
#include <qsoundcloud/authenticationrequest.h>
#include <qsoundcloud/resourcesrequest.h>
#include <QLabel>
#include <QAction>
#include <QVBoxLayout>
#include <QMaemo5InformationBox>
#include <QMessageBox>
#include <QMenuBar>

SoundCloudAccountsWindow::SoundCloudAccountsWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_authRequest(0),
    m_userRequest(0),
    m_model(new SoundCloudAccountModel(this)),
    m_view(new ListView(this)),
    m_addAction(new QAction(tr("Add account"), this)),
    m_removeAction(new QAction(tr("Remove"), this)),
    m_label(new QLabel(tr("Sign in to your SoundCloud account"), this))
{
    setWindowTitle(tr("Accounts"));
    setCentralWidget(new QWidget);
    
    menuBar()->addAction(m_addAction);
        
    m_view->setModel(m_model);
    m_view->setModelColumn(1);
    m_view->setItemDelegate(new AccountDelegate(SoundCloudAccountModel::ActiveRole, m_view));
    m_view->addAction(m_removeAction);
    
    m_label->setAlignment(Qt::AlignHCenter);
    
    m_layout = new QVBoxLayout(centralWidget());
    m_layout->addWidget(m_label);
    m_layout->addWidget(m_view);
    
    connect(m_model, SIGNAL(countChanged(int)), this, SLOT(onModelCountChanged(int)));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(selectAccount(QModelIndex)));
    connect(m_addAction, SIGNAL(triggered()), this, SLOT(showAuthDialog()));
    connect(m_removeAction, SIGNAL(triggered()), this, SLOT(removeAccount()));
    
    onModelCountChanged(m_model->rowCount());
}

void SoundCloudAccountsWindow::initAuthRequest() {
    if (!m_authRequest) {
        m_authRequest = new QSoundCloud::AuthenticationRequest(this);
        m_authRequest->setClientId(SoundCloud::instance()->clientId());
        m_authRequest->setClientSecret(SoundCloud::instance()->clientSecret());
        m_authRequest->setRedirectUri(SoundCloud::instance()->redirectUri());
        m_authRequest->setScopes(SoundCloud::instance()->scopes());
        connect(m_authRequest, SIGNAL(finished()), this, SLOT(onAuthRequestFinished()));
    }
}

void SoundCloudAccountsWindow::initUserRequest() {
    if (!m_userRequest) {
        m_userRequest = new QSoundCloud::ResourcesRequest(this);
        m_userRequest->setClientId(SoundCloud::instance()->clientId());
        m_userRequest->setClientSecret(SoundCloud::instance()->clientSecret());
        
        if (m_authRequest) {
            QVariantMap token = m_authRequest->result().toMap();
            m_userRequest->setAccessToken(token.value("access_token").toString());
            m_userRequest->setRefreshToken(token.value("refresh_token").toString());
        }
        
        connect(m_userRequest, SIGNAL(finished()), this, SLOT(onUserRequestFinished()));
    }
}

void SoundCloudAccountsWindow::removeAccount() {
    if (m_view->currentIndex().isValid()) {
        QString username = m_view->currentIndex().data(SoundCloudAccountModel::UsernameRole).toString();
        
        if (m_model->removeAccount(m_view->currentIndex().row())) {
            QMaemo5InformationBox::information(this, tr("Account '%1' removed. Please visit the SoundCloud website to \
            revoke the access token").arg(username));
        }
        else {
            QMessageBox::critical(this, tr("Database error"), m_model->errorString());
        }
    }
}

void SoundCloudAccountsWindow::selectAccount(const QModelIndex &index) {
    if (m_model->selectAccount(index.row())) {
        QMaemo5InformationBox::information(this, tr("You have selected account '%1'")
                                                   .arg(index.data(SoundCloudAccountModel::UsernameRole).toString()));
    }
    else {
        QMaemo5InformationBox::information(this, tr("Unable to select account '%1'")
                                                   .arg(index.data(SoundCloudAccountModel::UsernameRole).toString()));
    }
}

void SoundCloudAccountsWindow::showAuthDialog() {
    SoundCloudAuthDialog *dialog = new SoundCloudAuthDialog(this);
    dialog->open();
    connect(dialog, SIGNAL(codeReady(QString)), this, SLOT(submitCode(QString)));
}

void SoundCloudAccountsWindow::submitCode(const QString &code) {
    initAuthRequest();
    showProgressIndicator();
    m_authRequest->exchangeCodeForAccessToken(code);
}

void SoundCloudAccountsWindow::onAuthRequestFinished() {
    QVariantMap result = m_authRequest->result().toMap();
    
    if (m_authRequest->status() == QSoundCloud::AuthenticationRequest::Ready) {        
        if (result.contains("access_token")) {
            initUserRequest();
            m_userRequest->get("/me");
            return;
        }
    }
    
    QMessageBox::critical(this, tr("Authentication failed"), SoundCloud::getErrorString(result));
    
    m_view->setEnabled(true);
    m_addAction->setEnabled(true);
    hideProgressIndicator();
}

void SoundCloudAccountsWindow::onUserRequestFinished() {
    if (m_userRequest->status() == QSoundCloud::ResourcesRequest::Ready) {
        QVariantMap user = m_userRequest->result().toMap();
        
        if (!user.isEmpty()) {
            m_view->setEnabled(true);
            m_addAction->setEnabled(true);
            hideProgressIndicator();
            
            if (m_model->addAccount(user.value("id").toString(), user.value("username").toString(),
                                    m_userRequest->accessToken(), m_userRequest->refreshToken(),
                                    SoundCloud::instance()->scopes().join(" "))) {
                QMaemo5InformationBox::information(this, tr("You are signed in to account '%1'")
                                                           .arg(user.value("username").toString()));
            }
            else {
                QMessageBox::critical(this, tr("Database error"), m_model->errorString());
            }
            
            return;
        }
    }
    
    QMessageBox::critical(this, tr("Authentication failed"),
                          SoundCloud::getErrorString(m_userRequest->result().toMap()));
    
    m_view->setEnabled(true);
    m_addAction->setEnabled(true);
    hideProgressIndicator();
}

void SoundCloudAccountsWindow::onModelCountChanged(int count) {
    m_view->setContextMenuPolicy(count > 0 ? Qt::ActionsContextMenu : Qt::NoContextMenu);
}

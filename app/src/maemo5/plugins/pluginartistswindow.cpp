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

#include "pluginartistswindow.h"
#include "artistdelegate.h"
#include "imagecache.h"
#include "listview.h"
#include "nowplayingaction.h"
#include "pluginartistwindow.h"
#include "settings.h"
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QMenuBar>

PluginArtistsWindow::PluginArtistsWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_model(new PluginArtistModel(this)),
    m_cache(new ImageCache),
    m_nowPlayingAction(new NowPlayingAction(this)),
    m_view(new ListView(this)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_label(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                              .arg(palette().color(QPalette::Mid).name()).arg(tr("No artists found")), this))
{
    setWindowTitle(tr("Artists"));
    setCentralWidget(new QWidget);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(new ArtistDelegate(m_cache, PluginArtistModel::NameRole, PluginArtistModel::ThumbnailUrlRole,
                                               m_view));

    m_reloadAction->setEnabled(false);
    
    m_label->hide();
    
    m_layout = new QVBoxLayout(centralWidget());
    m_layout->addWidget(m_view);
    m_layout->addWidget(m_label);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    menuBar()->addAction(m_reloadAction);
    menuBar()->addAction(m_nowPlayingAction);
    
    connect(m_model, SIGNAL(statusChanged(ResourcesRequest::Status)), this,
            SLOT(onModelStatusChanged(ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showArtist(QModelIndex)));
    connect(m_reloadAction, SIGNAL(triggered()), m_model, SLOT(reload()));
}

PluginArtistsWindow::~PluginArtistsWindow() {
    delete m_cache;
    m_cache = 0;
}

void PluginArtistsWindow::list(const QString &service, const QString &id) {
    m_model->setService(service);
    m_model->list(id);
}

void PluginArtistsWindow::search(const QString &service, const QString &query, const QString &order) {
    m_model->setService(service);
    m_model->search(query, order);
}

void PluginArtistsWindow::showArtist(const QModelIndex &index) {
    if (PluginArtist *artist = m_model->get(index.row())) {
        PluginArtistWindow *window = new PluginArtistWindow(artist, this);
        window->show();
    }
}

void PluginArtistsWindow::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void PluginArtistsWindow::onModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showProgressIndicator();
        m_label->hide();
        m_view->show();
        m_reloadAction->setEnabled(false);
        return;
    case ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_model->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    m_reloadAction->setEnabled(true);
    
    if (m_model->rowCount() == 0) {
        m_view->hide();
        m_label->show();
    }
}

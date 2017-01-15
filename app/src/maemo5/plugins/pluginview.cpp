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

#include "pluginview.h"
#include "listview.h"
#include "navdelegate.h"
#include "pluginartistswindow.h"
#include "pluginartistwindow.h"
#include "plugincategorieswindow.h"
#include "pluginnavmodel.h"
#include "pluginplaylistswindow.h"
#include "pluginplaylistwindow.h"
#include "pluginsearchdialog.h"
#include "plugintrackswindow.h"
#include "plugintrackwindow.h"
#include "resources.h"
#include "stackedwindow.h"
#include <QVBoxLayout>
#include <QMaemo5InformationBox>

PluginView::PluginView(const QString &service, QWidget *parent) :
    QWidget(parent),
    m_model(new PluginNavModel(this)),
    m_view(new ListView(this)),
    m_layout(new QVBoxLayout(this))
{
    m_model->setService(service);
    m_view->setModel(m_model);
    m_view->setItemDelegate(new NavDelegate(m_view));
    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(onItemActivated(QModelIndex)));
}

void PluginView::search(const QString &query, const QString &type, const QString &order) {
    if (type == Resources::PLAYLIST) {
        PluginPlaylistsWindow *window = new PluginPlaylistsWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->search(m_model->service(), query, order);
        window->show();
    }
    else if (type == Resources::ARTIST) {
        PluginArtistsWindow *window = new PluginArtistsWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->search(m_model->service(), query, order);
        window->show();
    }
    else {
        PluginTracksWindow *window = new PluginTracksWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->search(m_model->service(), query, order);
        window->show();
    }
}

void PluginView::showResource(const QString &type, const QString &id) {
    if (type == Resources::PLAYLIST) {
        PluginPlaylistWindow *window = new PluginPlaylistWindow(m_model->service(), id, StackedWindow::currentWindow());
        window->show();
    }
    else if (type == Resources::ARTIST) {
        PluginArtistWindow *window = new PluginArtistWindow(m_model->service(), id, StackedWindow::currentWindow());
        window->show();
    }
    else {
        PluginTrackWindow *window = new PluginTrackWindow(m_model->service(), id, StackedWindow::currentWindow());
        window->show();
    }
}

void PluginView::showArtists(const QString &name, const QString &id) {
    PluginArtistsWindow *window = new PluginArtistsWindow(StackedWindow::currentWindow());
    window->setWindowTitle(name);
    window->list(m_model->service(), id);
    window->show();
}

void PluginView::showCategories(const QString &name, const QString &id) {
    PluginCategoriesWindow *window = new PluginCategoriesWindow(StackedWindow::currentWindow());
    window->setWindowTitle(name);
    window->list(m_model->service(), id);
    window->show();
}

void PluginView::showPlaylists(const QString &name, const QString &id) {
    PluginPlaylistsWindow *window = new PluginPlaylistsWindow(StackedWindow::currentWindow());
    window->setWindowTitle(name);
    window->list(m_model->service(), id);
    window->show();
}

void PluginView::showSearchDialog() {
    PluginSearchDialog dialog(m_model->service(), StackedWindow::currentWindow());

    if (dialog.exec() == QDialog::Accepted) {
        const QVariantMap resource = Resources::getResourceFromUrl(dialog.query());
        
        if (resource.value("service") == m_model->service()) {
            showResource(resource.value("type").toString(), resource.value("id").toString());
        }
        else {
            search(dialog.query(), dialog.type(), dialog.order());
        }
    }
}

void PluginView::showTracks(const QString &name, const QString &id) {
    PluginTracksWindow *window = new PluginTracksWindow(StackedWindow::currentWindow());
    window->setWindowTitle(name);
    window->list(m_model->service(), id);
    window->show();
}

void PluginView::onItemActivated(const QModelIndex &index) {
    const QVariantMap type = index.data(PluginNavModel::ValueRole).toMap();
    
    if (type.isEmpty()) {
        showSearchDialog();
    }
    else if (type.value("type") == Resources::CATEGORY) {
        showCategories(index.data(PluginNavModel::NameRole).toString(), type.value("id").toString());
    }
    else if (type.value("type") == Resources::PLAYLIST) {
        showPlaylists(index.data(PluginNavModel::NameRole).toString(), type.value("id").toString());
    }
    else if (type.value("type") == Resources::ARTIST) {
        showArtists(index.data(PluginNavModel::NameRole).toString(), type.value("id").toString());
    }
    else {
        showTracks(index.data(PluginNavModel::NameRole).toString(), type.value("id").toString());
    }
}

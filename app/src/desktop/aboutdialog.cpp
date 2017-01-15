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

#include "aboutdialog.h"
#include "definitions.h"
#include "pluginconfigmodel.h"
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QLabel>
#include <QTreeView>
#include <QVBoxLayout>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    m_model(new PluginConfigModel(this)),
    m_iconLabel(new QLabel(this)),
    m_textLabel(new QLabel(this)),
    m_view(new QTreeView(this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this)),
    m_layout(new QVBoxLayout(this))
{
    setWindowTitle(tr("About"));

    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setPixmap(QIcon::fromTheme("musikloud2").pixmap(64, 64));
    
    m_textLabel->setWordWrap(true);
    m_textLabel->setTextFormat(Qt::RichText);
    m_textLabel->setOpenExternalLinks(true);
    m_textLabel->setText(QString("<div style='text-align: center'><p style='font-size: 16pt; font-weight: bold;'>MusiKloud2</p><p>Version: %1</p><p>A plugin-extensible SoundCloud client and music player.</p><p>&copy; Stuart Howarth 2016</p><p><a href='http://marxoft.co.uk/projects/musikloud2'>marxoft.co.uk</a></p></div><p>Installed plugins:</p>").arg(VERSION_NUMBER));
    
    m_view->setModel(m_model);
    m_view->setSelectionMode(QTreeView::NoSelection);
    m_view->setEditTriggers(QTreeView::NoEditTriggers);
    m_view->setItemsExpandable(false);
    m_view->setUniformRowHeights(true);
    m_view->setAllColumnsShowFocus(true);
    m_view->setRootIsDecorated(false);
    m_view->header()->setStretchLastSection(true);

    m_layout->addWidget(m_iconLabel);
    m_layout->addWidget(m_textLabel);
    m_layout->addWidget(m_view);
    m_layout->addWidget(m_buttonBox);

    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

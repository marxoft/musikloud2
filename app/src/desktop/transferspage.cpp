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

#include "transferspage.h"
#include "customcommanddialog.h"
#include "definitions.h"
#include "settings.h"
#include "transferdelegate.h"
#include "transfermodel.h"
#include "transfers.h"
#include <QActionGroup>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QToolBar>
#include <QTreeView>
#include <QVBoxLayout>

TransfersPage::TransfersPage(QWidget *parent) :
    Page(parent),
    m_model(new TransferModel(this)),
    m_transferMenu(new QMenu(tr("&Transfer"), this)),
    m_categoryMenu(new QMenu(tr("&Category"), this)),
    m_priorityMenu(new QMenu(tr("&Priority"), this)),
    m_categoryGroup(new QActionGroup(this)),
    m_priorityGroup(new QActionGroup(this)),
    m_transferCommandAction(new QAction(QIcon::fromTheme("system-run"), tr("Set &custom command"), this)),
    m_transferStartAction(new QAction(QIcon::fromTheme("media-playback-start"), tr("&Start"), this)),
    m_transferPauseAction(new QAction(QIcon::fromTheme("media-playback-pause"), tr("&Pause"), this)),
    m_transferRemoveAction(new QAction(QIcon::fromTheme("edit-delete"), tr("&Remove"), this)),
    m_view(new QTreeView(this)),
    m_layout(new QVBoxLayout(this))
{
    updateWindowTitle();
    
    m_transferMenu->addAction(m_transferCommandAction);
    m_transferMenu->addAction(m_transferStartAction);
    m_transferMenu->addAction(m_transferPauseAction);
    m_transferMenu->addMenu(m_categoryMenu);
    m_transferMenu->addMenu(m_priorityMenu);
    m_transferMenu->addAction(m_transferRemoveAction);
    m_transferMenu->setEnabled(false);
    
    setCategoryMenuActions();
    
    const QStringList priorities = QStringList() << tr("High") << tr("Normal") << tr("Low");
    
    for (int i = 0; i < priorities.size(); i++) {
        QAction *action = m_priorityMenu->addAction(priorities.at(i), this, SLOT(setTransferPriority()));
        action->setCheckable(true);
        action->setData(i);
        m_priorityGroup->addAction(action);
    }
        
    m_view->setModel(m_model);
    m_view->setItemDelegate(new TransferDelegate(m_view));
    m_view->setAlternatingRowColors(true);
    m_view->setSelectionBehavior(QTreeView::SelectRows);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    m_view->setEditTriggers(QTreeView::NoEditTriggers);
    m_view->setItemsExpandable(false);
    m_view->setUniformRowHeights(true);
    m_view->setAllColumnsShowFocus(true);
    m_view->setRootIsDecorated(false);
    m_view->header()->restoreState(Settings::transfersHeaderViewState());

    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    connect(m_categoryMenu, SIGNAL(aboutToShow()), this, SLOT(setActiveCategoryMenuAction()));
    connect(m_priorityMenu, SIGNAL(aboutToShow()), this, SLOT(setActivePriorityMenuAction()));
    connect(m_transferCommandAction, SIGNAL(triggered()), this, SLOT(setTransferCustomCommand()));
    connect(m_transferStartAction, SIGNAL(triggered()), this, SLOT(queueTransfer()));
    connect(m_transferPauseAction, SIGNAL(triggered()), this, SLOT(pauseTransfer()));
    connect(m_transferRemoveAction, SIGNAL(triggered()), this, SLOT(removeTransfer()));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_view->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
            this, SLOT(onCurrentTransferChanged(QModelIndex)));
    connect(Settings::instance(), SIGNAL(categoriesChanged()), this, SLOT(setCategoryMenuActions()));
    connect(Transfers::instance(), SIGNAL(activeChanged(int)), this, SLOT(updateWindowTitle()));
    connect(Transfers::instance(), SIGNAL(countChanged(int)), this, SLOT(updateWindowTitle()));
}

void TransfersPage::closeEvent(QCloseEvent *e) {
    Settings::setTransfersHeaderViewState(m_view->header()->saveState());
    Page::closeEvent(e);
}

void TransfersPage::queueTransfer() {
    if (m_view->currentIndex().isValid()) {
        queueTransfer(m_view->currentIndex());
    }
}

void TransfersPage::queueTransfer(const QModelIndex &index) {
    if (Transfer *transfer = Transfers::instance()->get(index.row())) {
        transfer->queue();
    }
}

void TransfersPage::pauseTransfer() {
    if (m_view->currentIndex().isValid()) {
        pauseTransfer(m_view->currentIndex());
    }
}

void TransfersPage::pauseTransfer(const QModelIndex &index) {
    if (Transfer *transfer = Transfers::instance()->get(index.row())) {
        transfer->pause();
    }
}

void TransfersPage::removeTransfer() {
    if (m_view->currentIndex().isValid()) {
        removeTransfer(m_view->currentIndex());
    }
}

void TransfersPage::removeTransfer(const QModelIndex &index) {
    if (Transfer *transfer = Transfers::instance()->get(index.row())) {
        if (QMessageBox::question(this, tr("Remove?"),
            tr("Do you want to remove '%1'?").arg(transfer->title())) == QMessageBox::Yes) {
            transfer->cancel();
        }
    }
}

void TransfersPage::setTransferCategory() {
    if (m_view->currentIndex().isValid()) {
        if (const QAction *action = m_categoryGroup->checkedAction()) {
            setTransferCategory(m_view->currentIndex(), action->text());
        }
    }
}

void TransfersPage::setTransferCategory(const QModelIndex &index, const QString &category) {
    m_model->setData(index, category, TransferModel::CategoryRole);
}

void TransfersPage::setTransferCustomCommand() {
    const QModelIndex index = m_view->currentIndex();
    
    if (index.isValid()) {
        CustomCommandDialog dialog(this);
        dialog.setCommand(index.data(TransferModel::CustomCommandRole).toString());
        dialog.setOverrideEnabled(index.data(TransferModel::CustomCommandOverrideEnabledRole).toBool());
        
        if (dialog.exec() == QDialog::Accepted) {
            setTransferCustomCommand(index, dialog.command(), dialog.overrideEnabled());
        }
    }
}

void TransfersPage::setTransferCustomCommand(const QModelIndex &index, const QString &command, bool overrideEnabled) {
    m_model->setData(index, command, TransferModel::CustomCommandRole);
    m_model->setData(index, overrideEnabled, TransferModel::CustomCommandOverrideEnabledRole);
}

void TransfersPage::setTransferPriority() {
    if (m_view->currentIndex().isValid()) {
        if (const QAction *action = m_priorityGroup->checkedAction()) {
            setTransferPriority(m_view->currentIndex(), action->data().toInt());
        }
    }
}

void TransfersPage::setTransferPriority(const QModelIndex &index, int priority) {
    m_model->setData(index, priority, TransferModel::PriorityRole);
}

void TransfersPage::setCategoryMenuActions() {
    const QStringList categories = Settings::categoryNames();
    m_categoryMenu->clear();
    
    foreach (const QString &category, categories) {
        QAction *action = m_categoryMenu->addAction(category, this, SLOT(setTransferCategory()));
        action->setCheckable(true);
        action->setData(category);
        m_categoryGroup->addAction(action);
    }
}

void TransfersPage::setActiveCategoryMenuAction() {
    if (!m_view->currentIndex().isValid()) {
        return;
    }
    
    const QVariant category = m_view->currentIndex().data(TransferModel::CategoryRole).toString();
    
    foreach (QAction *action, m_categoryGroup->actions()) {
        if (action->data() == category) {
            action->setChecked(true);
            break;
        }
    }
}

void TransfersPage::setActivePriorityMenuAction() {
    if (!m_view->currentIndex().isValid()) {
        return;
    }
    
    const QVariant priority = m_view->currentIndex().data(TransferModel::PriorityRole);
    
    foreach (QAction *action, m_priorityGroup->actions()) {
        if (action->data() == priority) {
            action->setChecked(true);
            break;
        }
    }
}

void TransfersPage::showContextMenu(const QPoint &pos) {
    if (m_view->currentIndex().isValid()) {
        m_transferMenu->popup(m_view->mapToGlobal(pos));
    }
}

void TransfersPage::updateWindowTitle() {
    if (Transfers::instance()->count() > 0) {
        setWindowTitle(tr("Transfers (%1/%2)").arg(Transfers::instance()->active())
                       .arg(Transfers::instance()->count()));
    }
    else {
        setWindowTitle(tr("Transfers"));
    }
}

void TransfersPage::onCurrentTransferChanged(const QModelIndex &index) {
    m_transferMenu->setEnabled(index.isValid());
}

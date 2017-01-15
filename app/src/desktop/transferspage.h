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

#ifndef TRANSFERSPAGE_H
#define TRANSFERSPAGE_H

#include "page.h"

class TransferModel;
class QActionGroup;
class QMenu;
class QTreeView;
class QVBoxLayout;

class TransfersPage : public Page
{
    Q_OBJECT

public:
    explicit TransfersPage(QWidget *parent = 0);
    
private Q_SLOTS:
    void queueTransfer();
    void queueTransfer(const QModelIndex &index);
    
    void pauseTransfer();
    void pauseTransfer(const QModelIndex &index);
    
    void removeTransfer();
    void removeTransfer(const QModelIndex &index);
    
    void setTransferCategory();
    void setTransferCategory(const QModelIndex &index, const QString &category);
    
    void setTransferCustomCommand();
    void setTransferCustomCommand(const QModelIndex &index, const QString &command, bool overrideEnabled);
    
    void setTransferPriority();
    void setTransferPriority(const QModelIndex &index, int priority);
    
    void setCategoryMenuActions();
    
    void setActiveCategoryMenuAction();
    void setActivePriorityMenuAction();
        
    void showContextMenu(const QPoint &pos);
    
    void updateWindowTitle();
    
    void onCurrentTransferChanged(const QModelIndex &index);

private:
    virtual void closeEvent(QCloseEvent *e);
    
    TransferModel *m_model;
    
    QMenu *m_transferMenu;
    QMenu *m_categoryMenu;
    QMenu *m_priorityMenu;
    
    QActionGroup *m_categoryGroup;
    QActionGroup *m_priorityGroup;
    
    QAction *m_transferCommandAction;
    QAction *m_transferStartAction;
    QAction *m_transferPauseAction;
    QAction *m_transferRemoveAction;
            
    QTreeView *m_view;

    QVBoxLayout *m_layout;
};

#endif // TRANSFERSPAGE_H

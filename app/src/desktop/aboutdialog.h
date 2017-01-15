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

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

class PluginConfigModel;
class QDialogButtonBox;
class QLabel;
class QTreeView;
class QVBoxLayout;

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);

private:
    PluginConfigModel *m_model;
    
    QLabel *m_iconLabel;
    QLabel *m_textLabel;

    QTreeView *m_view;
    
    QDialogButtonBox *m_buttonBox;

    QVBoxLayout *m_layout;
};

#endif // ABOUTDIALOG_H

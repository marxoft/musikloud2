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

#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>

class SelectionModel;
class QComboBox;
class QDialogButtonBox;
class QFormLayout;
class QLineEdit;

class SearchDialog : public QDialog
{
    Q_OBJECT
    
    Q_PROPERTY(QString service READ service)
    Q_PROPERTY(QString query READ query WRITE setQuery)
    Q_PROPERTY(QString order READ order)

public:
    explicit SearchDialog(QWidget *parent = 0);
    
    QString service() const;
    
    QString query() const;
    void setQuery(const QString &query);
    
    QString order() const;

private Q_SLOTS:
    void onQueryChanged(const QString &query);

private:
    SelectionModel *m_model;
    
    QLineEdit *m_queryEdit;
    
    QComboBox *m_serviceSelector;
    
    QDialogButtonBox *m_buttonBox;
    
    QFormLayout *m_layout;
};

#endif // SEARCHDIALOG_H

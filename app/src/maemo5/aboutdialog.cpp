/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "aboutdialog.h"
#include "definitions.h"
#include <QDesktopServices>
#include <QUrl>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

AboutDialog::AboutDialog(QWidget *parent) :
    Dialog(parent)
{
    setWindowTitle(tr("About"));

    QLabel *imageLabel = new QLabel(this);
    imageLabel->setPixmap(QPixmap("/usr/share/icons/hicolor/64x64/apps/musikloud2.png"));
    imageLabel->setFixedWidth(64);
    QLabel *titleLabel = new QLabel(QString("<b><font size='20'>MusiKloud2 %1</font></b>").arg(VERSION_NUMBER), this);
    titleLabel->setAlignment(Qt::AlignVCenter);
    QLabel *descriptionLabel = new QLabel("A SoundCloud client and music player that can be extended via plugins.\
    <br><br>&copy; Stuart Howarth 2015", this);
    descriptionLabel->setWordWrap(true);
    QPushButton *bugButton = new QPushButton(tr("Report bug"), this);
    QGridLayout *grid = new QGridLayout(this);
    grid->addWidget(imageLabel, 0, 0);
    grid->addWidget(titleLabel, 0, 1);
    grid->addWidget(descriptionLabel, 1, 0, 1, 2);
    grid->addWidget(bugButton, 1, 2, Qt::AlignBottom);

    connect(bugButton, SIGNAL(clicked()), this, SLOT(reportBug()));
}

void AboutDialog::reportBug() {
    QDesktopServices::openUrl(QUrl(QString("mailto:showarth@marxoft.co.uk?subject=MusiKloud2 %1 for Maemo5")
                                          .arg(VERSION_NUMBER)));
    accept();
}

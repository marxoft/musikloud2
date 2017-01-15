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

#ifndef WEBVIEW_H
#define WEBVIEW_H

#include "qwebviewselectionsuppressor.h"
#include <QWebView>

class WebView : public QWebView
{
    Q_OBJECT

public:
    explicit WebView(QWidget *parent = 0);

public slots:
    void setNetworkAccessManager(QNetworkAccessManager *manager);
    void enableSelectionSuppressor() const;
    void disableSelectionSuppressor() const;
    void setSelectionSuppressorEnabled(bool enable) const;
    
private:
    QWebViewSelectionSuppressor *m_suppressor;
};

#endif // WEBVIEW_H

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

#include "webview.h"
#include <QAbstractKineticScroller>

WebView::WebView(QWidget *parent) :
    QWebView(parent),
    m_suppressor(new QWebViewSelectionSuppressor(this))
{
}

void WebView::setNetworkAccessManager(QNetworkAccessManager *manager) {
    page()->setNetworkAccessManager(manager);
}

void WebView::enableSelectionSuppressor() const {
    m_suppressor->enable();
    property("kineticScroller").value<QAbstractKineticScroller *>()->setEnabled(true);
}

void WebView::disableSelectionSuppressor() const {
    m_suppressor->disable();
    property("kineticScroller").value<QAbstractKineticScroller *>()->setEnabled(false);
}

void WebView::setSelectionSuppressorEnabled(bool enable) const {
    if (enable) {
        enableSelectionSuppressor();
    }
    else {
        disableSelectionSuppressor();
    }
}

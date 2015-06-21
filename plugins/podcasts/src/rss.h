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

#ifndef RSS_H
#define RSS_H

#include <QObject>
#include <QStringList>
#include <QVariantList>

class QNetworkAccessManager;
class QNetworkReply;
class QUrl;

class Rss : public QObject
{
    Q_OBJECT

public:
    explicit Rss(QObject *parent = 0);
    
    void listTracks(const QStringList &urls);    
    void listTracks(const QString &url);
    
private:
    void followRedirect(const QUrl &url);
    
private Q_SLOTS:
    void parseTracks(QNetworkReply *reply);
    void printResult();
    
private:
    QNetworkAccessManager *m_nam;
    
    QStringList m_urls;
    QVariantList m_results;
    int m_redirects;
};
    
#endif // RSS_H

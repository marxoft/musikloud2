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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QStringList>
#include <QVariant>

struct Category {
    QString name;
    QString path;
};

class Settings : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QStringList categoryNames READ categoryNames NOTIFY categoriesChanged)
    Q_PROPERTY(QString defaultCategory READ defaultCategory WRITE setDefaultCategory NOTIFY defaultCategoryChanged)
    Q_PROPERTY(QString customTransferCommand READ customTransferCommand WRITE setCustomTransferCommand
               NOTIFY customTransferCommandChanged)
    Q_PROPERTY(bool customTransferCommandEnabled READ customTransferCommandEnabled WRITE setCustomTransferCommandEnabled
               NOTIFY customTransferCommandEnabledChanged)
    Q_PROPERTY(QString downloadPath READ downloadPath WRITE setDownloadPath NOTIFY downloadPathChanged)
    Q_PROPERTY(QString loggerFileName READ loggerFileName WRITE setLoggerFileName NOTIFY loggerFileNameChanged)
    Q_PROPERTY(int loggerVerbosity READ loggerVerbosity WRITE setLoggerVerbosity NOTIFY loggerVerbosityChanged)
    Q_PROPERTY(QByteArray mainWindowGeometry READ mainWindowGeometry WRITE setMainWindowGeometry)
    Q_PROPERTY(QByteArray mainWindowState READ mainWindowState WRITE setMainWindowState)
    Q_PROPERTY(int maximumConcurrentTransfers READ maximumConcurrentTransfers WRITE setMaximumConcurrentTransfers
               NOTIFY maximumConcurrentTransfersChanged)
    Q_PROPERTY(bool networkProxyAuthenticationEnabled READ networkProxyAuthenticationEnabled
               WRITE setNetworkProxyAuthenticationEnabled NOTIFY networkProxyChanged)
    Q_PROPERTY(bool networkProxyEnabled READ networkProxyEnabled WRITE setNetworkProxyEnabled
               NOTIFY networkProxyChanged)
    Q_PROPERTY(QString networkProxyHost READ networkProxyHost WRITE setNetworkProxyHost NOTIFY networkProxyChanged)
    Q_PROPERTY(QString networkProxyPassword READ networkProxyPassword WRITE setNetworkProxyPassword
               NOTIFY networkProxyChanged)
    Q_PROPERTY(int networkProxyPort READ networkProxyPort WRITE setNetworkProxyPort NOTIFY networkProxyChanged)
    Q_PROPERTY(int networkProxyType READ networkProxyType WRITE setNetworkProxyType NOTIFY networkProxyChanged)
    Q_PROPERTY(QString networkProxyUsername READ networkProxyUsername WRITE setNetworkProxyUsername
               NOTIFY networkProxyChanged)
    Q_PROPERTY(bool restorePlaybackQueueOnStartup READ restorePlaybackQueueOnStartup
               WRITE setRestorePlaybackQueueOnStartup NOTIFY restorePlaybackQueueOnStartupChanged)
    Q_PROPERTY(QStringList searchHistory READ searchHistory WRITE setSearchHistory NOTIFY searchHistoryChanged)
    Q_PROPERTY(bool startTransfersAutomatically READ startTransfersAutomatically WRITE setStartTransfersAutomatically
               NOTIFY startTransfersAutomaticallyChanged)
    Q_PROPERTY(QByteArray tracksHeaderViewState READ tracksHeaderViewState WRITE setTracksHeaderViewState)
    Q_PROPERTY(QByteArray transfersHeaderViewState READ transfersHeaderViewState WRITE setTransfersHeaderViewState)
    
public:
    ~Settings();
    
    static Settings* instance();

    static QStringList categoryNames();
    static QList<Category> categories();
    static void setCategories(const QList<Category> &c);
            
    static QString customTransferCommand();
    static bool customTransferCommandEnabled();
    
    static QString defaultCategory();
    
    static QString defaultDownloadFormat(const QString &service);
    static QString defaultPlaybackFormat(const QString &service);
            
    static QString downloadPath();
    static QString downloadPath(const QString &category);
        
    static QString loggerFileName();
    static int loggerVerbosity();
    
    static QByteArray mainWindowGeometry();
    static QByteArray mainWindowState();
        
    static int maximumConcurrentTransfers();
    
    static bool networkProxyAuthenticationEnabled();
    static bool networkProxyEnabled();
    static QString networkProxyHost();
    static QString networkProxyPassword();
    static int networkProxyPort();
    static int networkProxyType();
    static QString networkProxyUsername();
    
    static bool restorePlaybackQueueOnStartup();
            
    static QStringList searchHistory();
    
    static bool startTransfersAutomatically();

    static QByteArray tracksHeaderViewState();
    static QByteArray transfersHeaderViewState();
    
    static QVariant value(const QString &key, const QVariant &defaultValue = QVariant());

public Q_SLOTS:
    static void addCategory(const QString &name, const QString &path);
    static void setDefaultCategory(const QString &category);
    static void removeCategory(const QString &name);
        
    static void setCustomTransferCommand(const QString &command);
    static void setCustomTransferCommandEnabled(bool enabled);
    
    static void setDefaultDownloadFormat(const QString &service, const QString &format);
    static void setDefaultPlaybackFormat(const QString &service, const QString &format);
            
    static void setDownloadPath(const QString &path);
        
    static void setLoggerFileName(const QString &fileName);
    static void setLoggerVerbosity(int verbosity);
    
    static void setMainWindowGeometry(const QByteArray &geometry);
    static void setMainWindowState(const QByteArray &state);
    
    static void setMaximumConcurrentTransfers(int maximum);
    
    static void setNetworkProxy();
    static void setNetworkProxyAuthenticationEnabled(bool enabled);
    static void setNetworkProxyEnabled(bool enabled);
    static void setNetworkProxyHost(const QString &host);
    static void setNetworkProxyPassword(const QString &password);
    static void setNetworkProxyPort(int port);
    static void setNetworkProxyType(int type);
    static void setNetworkProxyUsername(const QString &username);
    
    static void setRestorePlaybackQueueOnStartup(bool enabled);
    
    static void setSearchHistory(const QStringList &searches);
    static void addSearch(const QString &query);
    static void removeSearch(const QString &query);
    
    static void setStartTransfersAutomatically(bool enabled);
    
    static void setTracksHeaderViewState(const QByteArray &state);
    static void setTransfersHeaderViewState(const QByteArray &state);
    
    static void setValue(const QString &key, const QVariant &value);

Q_SIGNALS:
    void categoriesChanged();
    void defaultCategoryChanged(const QString &category);
    void customTransferCommandChanged(const QString &command);
    void customTransferCommandEnabledChanged(bool enabled);
    void downloadFormatsChanged();
    void downloadPathChanged(const QString &path);
    void loggerFileNameChanged(const QString &fileName);
    void loggerVerbosityChanged(int verbosity);
    void maximumConcurrentTransfersChanged(int maximum);
    void networkProxyChanged();
    void playbackFormatsChanged();
    void restorePlaybackQueueOnStartupChanged(bool enabled);
    void safeSearchEnabledChanged(bool enabled);
    void searchHistoryChanged();
    void startTransfersAutomaticallyChanged(bool enabled);

private:
    Settings();
    
    static Settings *self;
};
    
#endif // SETTINGS_H

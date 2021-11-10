/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWebEngine module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QQUICKWEBENGINEPROFILE_P_H
#define QQUICKWEBENGINEPROFILE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "browser_context_adapter_client.h"
#include "browser_context_adapter.h"
#include "qwebenginebrowsercontext_p.h"
#include "qquickwebengineprofile_p.h"

#include <QExplicitlySharedDataPointer>
#include <QMap>
#include <QPointer>
#include <QSharedPointer>

QT_BEGIN_NAMESPACE

class QQuickWebEngineDownloadItem;
class QQuickWebEngineSettings;

class QQuickWebEngineProfilePrivate : public QtWebEngineCore::BrowserContextAdapterClient {
public:
    Q_DECLARE_PUBLIC(QQuickWebEngineProfile)
    QQuickWebEngineProfilePrivate(QSharedPointer<QtWebEngineCore::BrowserContextAdapter> browserContext);
    ~QQuickWebEngineProfilePrivate();

    QSharedPointer<QtWebEngineCore::BrowserContextAdapter> browserContext() const;
    QQuickWebEngineSettings *settings() const { return m_settings.data(); }

    void cancelDownload(quint32 downloadId);
    void downloadDestroyed(quint32 downloadId);

    void downloadRequested(DownloadItemInfo &info) Q_DECL_OVERRIDE;
    void downloadUpdated(const DownloadItemInfo &info) Q_DECL_OVERRIDE;

    // QQmlListPropertyHelpers
    static void userScripts_append(QQmlListProperty<QQuickWebEngineScript> *p, QQuickWebEngineScript *script);
    static int userScripts_count(QQmlListProperty<QQuickWebEngineScript> *p);
    static QQuickWebEngineScript *userScripts_at(QQmlListProperty<QQuickWebEngineScript> *p, int idx);
    static void userScripts_clear(QQmlListProperty<QQuickWebEngineScript> *p);

private:
    friend class QQuickWebEngineViewPrivate;
    QQuickWebEngineProfile *q_ptr;
    QScopedPointer<QQuickWebEngineSettings> m_settings;
    QPointer<QWebEngineBrowserContext> m_browserContext;
    QMap<quint32, QPointer<QQuickWebEngineDownloadItem> > m_ongoingDownloads;
    QList<QQuickWebEngineScript *> m_userScripts;
};

QT_END_NAMESPACE

#endif // QQUICKWEBENGINEPROFILE_P_H

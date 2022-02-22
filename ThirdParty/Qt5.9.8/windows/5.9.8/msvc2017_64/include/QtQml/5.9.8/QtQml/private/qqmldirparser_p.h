/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtQml module of the Qt Toolkit.
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

#ifndef QQMLDIRPARSER_P_H
#define QQMLDIRPARSER_P_H

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

#include <QtCore/QUrl>
#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <private/qqmljsengine_p.h>
#include <private/qv4global_p.h>

QT_BEGIN_NAMESPACE

class QQmlError;
class QQmlEngine;
class Q_QML_PRIVATE_EXPORT QQmlDirParser
{
public:
    QQmlDirParser();
    ~QQmlDirParser();

    bool parse(const QString &source);

    bool hasError() const;
    void setError(const QQmlError &);
    QList<QQmlError> errors(const QString &uri) const;

    QString typeNamespace() const;
    void setTypeNamespace(const QString &s);

    struct Plugin
    {
        Plugin() {}

        Plugin(const QString &name, const QString &path)
            : name(name), path(path) {}

        QString name;
        QString path;
    };

    struct Component
    {
        Component()
            : majorVersion(0), minorVersion(0), internal(false), singleton(false) {}

        Component(const QString &typeName, const QString &fileName, int majorVersion, int minorVersion)
            : typeName(typeName), fileName(fileName), majorVersion(majorVersion), minorVersion(minorVersion),
            internal(false), singleton(false) {}

        QString typeName;
        QString fileName;
        int majorVersion;
        int minorVersion;
        bool internal;
        bool singleton;
    };

    struct Script
    {
        Script()
            : majorVersion(0), minorVersion(0) {}

        Script(const QString &nameSpace, const QString &fileName, int majorVersion, int minorVersion)
            : nameSpace(nameSpace), fileName(fileName), majorVersion(majorVersion), minorVersion(minorVersion) {}

        QString nameSpace;
        QString fileName;
        int majorVersion;
        int minorVersion;
    };

    QHash<QString,Component> components() const;
    QHash<QString,Component> dependencies() const;
    QList<Script> scripts() const;
    QList<Plugin> plugins() const;
    bool designerSupported() const;

#ifdef QT_CREATOR
    struct TypeInfo
    {
        TypeInfo() {}
        TypeInfo(const QString &fileName)
            : fileName(fileName) {}

        QString fileName;
    };

    QList<TypeInfo> typeInfos() const;
#endif

private:
    bool maybeAddComponent(const QString &typeName, const QString &fileName, const QString &version, QHash<QString,Component> &hash, int lineNumber = -1, bool multi = true);
    void reportError(quint16 line, quint16 column, const QString &message);

private:
    QList<QQmlJS::DiagnosticMessage> _errors;
    QString _typeNamespace;
    QHash<QString,Component> _components; // multi hash
    QHash<QString,Component> _dependencies;
    QList<Script> _scripts;
    QList<Plugin> _plugins;
    bool _designerSupported;
#ifdef QT_CREATOR
    QList<TypeInfo> _typeInfos;
#endif
};

typedef QHash<QString,QQmlDirParser::Component> QQmlDirComponents;
typedef QList<QQmlDirParser::Script> QQmlDirScripts;
typedef QList<QQmlDirParser::Plugin> QQmlDirPlugins;

QDebug &operator<< (QDebug &, const QQmlDirParser::Component &);
QDebug &operator<< (QDebug &, const QQmlDirParser::Script &);

QT_END_NAMESPACE

#endif // QQMLDIRPARSER_P_H

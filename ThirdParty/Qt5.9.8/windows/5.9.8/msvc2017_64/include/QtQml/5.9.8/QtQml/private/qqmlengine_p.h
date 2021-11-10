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

#ifndef QQMLENGINE_P_H
#define QQMLENGINE_P_H

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

#include "qqmlengine.h"

#include "qqmltypeloader_p.h"
#include "qqmlimport_p.h"
#include <private/qpodvector_p.h>
#include "qqml.h"
#include "qqmlvaluetype_p.h"
#include "qqmlcontext.h"
#include "qqmlcontext_p.h"
#include "qqmlexpression.h"
#include "qqmlproperty_p.h"
#include "qqmlpropertycache_p.h"
#include "qqmlmetatype_p.h"
#include "qqmldirparser_p.h"
#include <private/qintrusivelist_p.h>
#include <private/qrecyclepool_p.h>
#include <private/qfieldlist_p.h>

#include <QtCore/qlist.h>
#include <QtCore/qpair.h>
#include <QtCore/qstack.h>
#include <QtCore/qmutex.h>
#include <QtCore/qstring.h>
#include <QtCore/qthread.h>

#include <private/qobject_p.h>

#include <private/qv8engine_p.h>
#include <private/qjsengine_p.h>

QT_BEGIN_NAMESPACE

class QQmlContext;
class QQmlEngine;
class QQmlContextPrivate;
class QQmlExpression;
class QQmlImportDatabase;
class QNetworkReply;
class QNetworkAccessManager;
class QQmlNetworkAccessManagerFactory;
class QQmlTypeNameCache;
class QQmlComponentAttached;
class QQmlCleanup;
class QQmlDelayedError;
class QQuickWorkerScriptEngine;
class QQmlObjectCreator;
class QDir;
class QQmlIncubator;
class QQmlProfiler;
class QQmlPropertyCapture;

// This needs to be declared here so that the pool for it can live in QQmlEnginePrivate.
// The inline method definitions are in qqmljavascriptexpression_p.h
class QQmlJavaScriptExpressionGuard : public QQmlNotifierEndpoint
{
public:
    inline QQmlJavaScriptExpressionGuard(QQmlJavaScriptExpression *);

    static inline QQmlJavaScriptExpressionGuard *New(QQmlJavaScriptExpression *e,
                                                             QQmlEngine *engine);
    inline void Delete();

    QQmlJavaScriptExpression *expression;
    QQmlJavaScriptExpressionGuard *next;
};

class Q_QML_PRIVATE_EXPORT QQmlEnginePrivate : public QJSEnginePrivate
{
    Q_DECLARE_PUBLIC(QQmlEngine)
public:
    QQmlEnginePrivate(QQmlEngine *);
    ~QQmlEnginePrivate();

    void init();
    // No mutex protecting baseModulesUninitialized, because use outside QQmlEngine
    // is just qmlClearTypeRegistrations (which can't be called while an engine exists)
    static bool baseModulesUninitialized;

    QQmlPropertyCapture *propertyCapture;

    QRecyclePool<QQmlJavaScriptExpressionGuard> jsExpressionGuardPool;

    QQmlContext *rootContext;

#ifdef QT_NO_QML_DEBUGGER
    static const quintptr profiler = 0;
#else
    QQmlProfiler *profiler;
#endif

    bool outputWarningsToMsgLog;

    // Registered cleanup handlers
    QQmlCleanup *cleanup;

    // Bindings that have had errors during startup
    QQmlDelayedError *erroredBindings;
    int inProgressCreations;

    QV8Engine *v8engine() const { return q_func()->handle(); }
    QV4::ExecutionEngine *v4engine() const { return QV8Engine::getV4(q_func()->handle()); }

    QQuickWorkerScriptEngine *getWorkerScriptEngine();
    QQuickWorkerScriptEngine *workerScriptEngine;

    QUrl baseUrl;

    typedef QPair<QPointer<QObject>,int> FinalizeCallback;
    void registerFinalizeCallback(QObject *obj, int index);

    QQmlObjectCreator *activeObjectCreator;
#if QT_CONFIG(qml_network)
    QNetworkAccessManager *createNetworkAccessManager(QObject *parent) const;
    QNetworkAccessManager *getNetworkAccessManager() const;
    mutable QNetworkAccessManager *networkAccessManager;
    mutable QQmlNetworkAccessManagerFactory *networkAccessManagerFactory;
#endif
    QHash<QString,QSharedPointer<QQmlImageProviderBase> > imageProviders;

    QQmlAbstractUrlInterceptor* urlInterceptor;

    int scarceResourcesRefCount;
    void referenceScarceResources();
    void dereferenceScarceResources();

    QQmlImportDatabase importDatabase;
    QQmlTypeLoader typeLoader;

    QString offlineStoragePath;

    mutable quint32 uniqueId;
    inline quint32 getUniqueId() const {
        return uniqueId++;
    }

    // Unfortunate workaround to avoid a circular dependency between
    // qqmlengine_p.h and qqmlincubator_p.h
    struct Incubator : public QSharedData {
        QIntrusiveListNode next;
        // Unfortunate workaround for MSVC
        QIntrusiveListNode nextWaitingFor;
    };
    QIntrusiveList<Incubator, &Incubator::next> incubatorList;
    unsigned int incubatorCount;
    QQmlIncubationController *incubationController;
    void incubate(QQmlIncubator &, QQmlContextData *);

    // These methods may be called from any thread
    inline bool isEngineThread() const;
    inline static bool isEngineThread(const QQmlEngine *);
    template<typename T>
    inline void deleteInEngineThread(T *);
    template<typename T>
    inline static void deleteInEngineThread(QQmlEnginePrivate *, T *);
    QString offlineStorageDatabaseDirectory() const;

    // These methods may be called from the loader thread
    inline QQmlPropertyCache *cache(const QQmlType &, int);
    using QJSEnginePrivate::cache;

    // These methods may be called from the loader thread
    bool isQObject(int);
    QObject *toQObject(const QVariant &, bool *ok = 0) const;
    QQmlMetaType::TypeCategory typeCategory(int) const;
    bool isList(int) const;
    int listType(int) const;
    QQmlMetaObject rawMetaObjectForType(int) const;
    QQmlMetaObject metaObjectForType(int) const;
    QQmlPropertyCache *propertyCacheForType(int);
    QQmlPropertyCache *rawPropertyCacheForType(int);
    void registerInternalCompositeType(QV4::CompiledData::CompilationUnit *compilationUnit);
    void unregisterInternalCompositeType(QV4::CompiledData::CompilationUnit *compilationUnit);

    bool isTypeLoaded(const QUrl &url) const;
    bool isScriptLoaded(const QUrl &url) const;

    void sendQuit();
    void sendExit(int retCode = 0);
    void warning(const QQmlError &);
    void warning(const QList<QQmlError> &);
    void warning(QQmlDelayedError *);
    static void warning(QQmlEngine *, const QQmlError &);
    static void warning(QQmlEngine *, const QList<QQmlError> &);
    static void warning(QQmlEngine *, QQmlDelayedError *);
    static void warning(QQmlEnginePrivate *, const QQmlError &);
    static void warning(QQmlEnginePrivate *, const QList<QQmlError> &);

    inline static QV8Engine *getV8Engine(QQmlEngine *e);
    inline static QV4::ExecutionEngine *getV4Engine(QQmlEngine *e);
    inline static QQmlEnginePrivate *get(QQmlEngine *e);
    inline static const QQmlEnginePrivate *get(const QQmlEngine *e);
    inline static QQmlEnginePrivate *get(QQmlContext *c);
    inline static QQmlEnginePrivate *get(QQmlContextData *c);
    inline static QQmlEngine *get(QQmlEnginePrivate *p);
    inline static QQmlEnginePrivate *get(QV4::ExecutionEngine *e);

    static void registerBaseTypes(const char *uri, int versionMajor, int versionMinor);
    static void registerQtQuick2Types(const char *uri, int versionMajor, int versionMinor);
    static void defineQtQuick2Module();

    static bool designerMode();
    static void activateDesignerMode();

    static bool qml_debugging_enabled;

    mutable QMutex networkAccessManagerMutex;

private:
    // These members must be protected by a QQmlEnginePrivate::Locker as they are required by
    // the threaded loader.  Only access them through their respective accessor methods.
    QHash<int, QV4::CompiledData::CompilationUnit *> m_compositeTypes;
    static bool s_designerMode;

    // These members is protected by the full QQmlEnginePrivate::mutex mutex
    struct Deletable { Deletable():next(0) {} virtual ~Deletable() {} Deletable *next; };
    QFieldList<Deletable, &Deletable::next> toDeleteInEngineThread;
    void doDeleteInEngineThread();

    void cleanupScarceResources();
};

/*
   This function should be called prior to evaluation of any js expression,
   so that scarce resources are not freed prematurely (eg, if there is a
   nested javascript expression).
 */
inline void QQmlEnginePrivate::referenceScarceResources()
{
    scarceResourcesRefCount += 1;
}

/*
   This function should be called after evaluation of the js expression is
   complete, and so the scarce resources may be freed safely.
 */
inline void QQmlEnginePrivate::dereferenceScarceResources()
{
    Q_ASSERT(scarceResourcesRefCount > 0);
    scarceResourcesRefCount -= 1;

    // if the refcount is zero, then evaluation of the "top level"
    // expression must have completed.  We can safely release the
    // scarce resources.
    if (Q_LIKELY(scarceResourcesRefCount == 0)) {
        QV4::ExecutionEngine *engine = QV8Engine::getV4(v8engine());
        if (Q_UNLIKELY(!engine->scarceResources.isEmpty())) {
            cleanupScarceResources();
        }
    }
}

/*!
Returns true if the calling thread is the QQmlEngine thread.
*/
bool QQmlEnginePrivate::isEngineThread() const
{
    Q_Q(const QQmlEngine);
    return QThread::currentThread() == q->thread();
}

/*!
Returns true if the calling thread is the QQmlEngine \a engine thread.
*/
bool QQmlEnginePrivate::isEngineThread(const QQmlEngine *engine)
{
    Q_ASSERT(engine);
    return QQmlEnginePrivate::get(engine)->isEngineThread();
}

/*!
Delete \a value in the engine thread.  If the calling thread is the engine
thread, \a value will be deleted immediately.

This method should be used for *any* type that has resources that need to
be freed in the engine thread.  This is generally types that use V8 handles.
As there is some small overhead in checking the current thread, it is best
practice to check if any V8 handles actually need to be freed and delete
the instance directly if not.
*/
template<typename T>
void QQmlEnginePrivate::deleteInEngineThread(T *value)
{
    Q_Q(QQmlEngine);

    Q_ASSERT(value);
    if (isEngineThread()) {
        delete value;
    } else {
        struct I : public Deletable {
            I(T *value) : value(value) {}
            ~I() { delete value; }
            T *value;
        };
        I *i = new I(value);
        mutex.lock();
        bool wasEmpty = toDeleteInEngineThread.isEmpty();
        toDeleteInEngineThread.append(i);
        mutex.unlock();
        if (wasEmpty)
            QCoreApplication::postEvent(q, new QEvent(QEvent::User));
    }
}

/*!
Delete \a value in the \a engine thread.  If the calling thread is the engine
thread, \a value will be deleted immediately.
*/
template<typename T>
void QQmlEnginePrivate::deleteInEngineThread(QQmlEnginePrivate *engine, T *value)
{
    Q_ASSERT(engine);
    engine->deleteInEngineThread<T>(value);
}

/*!
Returns a QQmlPropertyCache for \a type with \a minorVersion.

The returned cache is not referenced, so if it is to be stored, call addref().
*/
QQmlPropertyCache *QQmlEnginePrivate::cache(const QQmlType &type, int minorVersion)
{
    Q_ASSERT(type.isValid());

    if (minorVersion == -1 || !type.containsRevisionedAttributes())
        return cache(type.metaObject());

    Locker locker(this);
    return QQmlMetaType::propertyCache(type, minorVersion);
}

QV8Engine *QQmlEnginePrivate::getV8Engine(QQmlEngine *e)
{
    Q_ASSERT(e);

    return e->d_func()->v8engine();
}

QV4::ExecutionEngine *QQmlEnginePrivate::getV4Engine(QQmlEngine *e)
{
    Q_ASSERT(e);

    return e->d_func()->v4engine();
}

QQmlEnginePrivate *QQmlEnginePrivate::get(QQmlEngine *e)
{
    Q_ASSERT(e);

    return e->d_func();
}

const QQmlEnginePrivate *QQmlEnginePrivate::get(const QQmlEngine *e)
{
    Q_ASSERT(e);

    return e ? e->d_func() : nullptr;
}

QQmlEnginePrivate *QQmlEnginePrivate::get(QQmlContext *c)
{
    return (c && c->engine()) ? QQmlEnginePrivate::get(c->engine()) : 0;
}

QQmlEnginePrivate *QQmlEnginePrivate::get(QQmlContextData *c)
{
    return (c && c->engine) ? QQmlEnginePrivate::get(c->engine) : 0;
}

QQmlEngine *QQmlEnginePrivate::get(QQmlEnginePrivate *p)
{
    Q_ASSERT(p);

    return p->q_func();
}

QQmlEnginePrivate *QQmlEnginePrivate::get(QV4::ExecutionEngine *e)
{
    if (!e->v8Engine)
        return 0;
    QQmlEngine *qmlEngine = e->v8Engine->engine();
    if (!qmlEngine)
        return 0;
    return get(qmlEngine);
}

QT_END_NAMESPACE

#endif // QQMLENGINE_P_H

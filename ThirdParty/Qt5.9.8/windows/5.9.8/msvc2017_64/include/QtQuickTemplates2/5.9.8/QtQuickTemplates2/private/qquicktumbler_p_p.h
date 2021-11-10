/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Quick Templates 2 module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QQUICKTUMBLER_P_P_H
#define QQUICKTUMBLER_P_P_H

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

#include <QtQuick/private/qquickitemchangelistener_p.h>
#include <QtQuickTemplates2/private/qquickcontrol_p_p.h>

QT_BEGIN_NAMESPACE

class QQuickTumbler;

class Q_QUICKTEMPLATES2_PRIVATE_EXPORT QQuickTumblerPrivate : public QQuickControlPrivate, public QQuickItemChangeListener
{
    Q_DECLARE_PUBLIC(QQuickTumbler)

public:
    QQuickTumblerPrivate();
    ~QQuickTumblerPrivate();

    enum ContentItemType {
        NoContentItem,
        UnsupportedContentItemType,
        PathViewContentItem,
        ListViewContentItem
    };

    QQuickItem *determineViewType(QQuickItem *contentItem);
    void resetViewData();
    QList<QQuickItem *> viewContentItemChildItems() const;

    static QQuickTumblerPrivate *get(QQuickTumbler *tumbler);

    QVariant model;
    QQmlComponent *delegate;
    int visibleItemCount;
    bool wrap;
    bool explicitWrap;
    bool ignoreWrapChanges;
    QQuickItem *view;
    QQuickItem *viewContentItem;
    ContentItemType viewContentItemType;
    int currentIndex;
    int pendingCurrentIndex;
    bool ignoreCurrentIndexChanges;
    int count;
    bool ignoreSignals;

    void _q_updateItemHeights();
    void _q_updateItemWidths();
    void _q_onViewCurrentIndexChanged();
    void _q_onViewCountChanged();
    void _q_calculateAttachedDisplacements();

    void disconnectFromView();
    void setupViewData(QQuickItem *newControlContentItem);
    void syncCurrentIndex();

    void setCount(int newCount);
    void setWrapBasedOnCount();
    void setWrap(bool shouldWrap, bool isExplicit);
    void lockWrap();
    void unlockWrap();

    void itemChildAdded(QQuickItem *, QQuickItem *) override;
    void itemChildRemoved(QQuickItem *, QQuickItem *) override;
};

class Q_QUICKTEMPLATES2_PRIVATE_EXPORT QQuickTumblerAttachedPrivate : public QObjectPrivate, public QQuickItemChangeListener
{
    Q_DECLARE_PUBLIC(QQuickTumblerAttached)

public:
    QQuickTumblerAttachedPrivate();

    void init(QQuickItem *delegateItem);

    void _q_calculateDisplacement();
    void emitIfDisplacementChanged(qreal oldDisplacement, qreal newDisplacement);

    static QQuickTumblerAttachedPrivate *get(QQuickTumblerAttached *attached);

    // The Tumbler that contains the delegate. Required to calculated the displacement.
    QPointer<QQuickTumbler> tumbler;
    // The index of the delegate. Used to calculate the displacement.
    int index;
    // The displacement for our delegate.
    qreal displacement;
};

QT_END_NAMESPACE

#endif // QQUICKTUMBLER_P_P_H

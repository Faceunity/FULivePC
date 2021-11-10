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

#ifndef QQUICKTEXTAREA_P_P_H
#define QQUICKTEXTAREA_P_P_H

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

#include <QtQuick/private/qquicktextedit_p_p.h>
#include <QtQuick/private/qquickitemchangelistener_p.h>
#include <QtQuickTemplates2/private/qquickpresshandler_p_p.h>
#include <QtQuickTemplates2/private/qquickdeferredpointer_p_p.h>

#include "qquicktextarea_p.h"

#if QT_CONFIG(accessibility)
#include <QtGui/qaccessible.h>
#endif

QT_BEGIN_NAMESPACE

class QQuickFlickable;
class QQuickAccessibleAttached;

class QQuickTextAreaPrivate : public QQuickTextEditPrivate, public QQuickItemChangeListener
#if QT_CONFIG(accessibility)
    , public QAccessible::ActivationObserver
#endif
{
    Q_DECLARE_PUBLIC(QQuickTextArea)

public:
    QQuickTextAreaPrivate();
    ~QQuickTextAreaPrivate();

    static QQuickTextAreaPrivate *get(QQuickTextArea *item)
    {
        return static_cast<QQuickTextAreaPrivate *>(QObjectPrivate::get(item));
    }

    void resizeBackground();
    void resolveFont();
    void inheritFont(const QFont &f);

#if QT_CONFIG(quicktemplates2_hover)
    void updateHoverEnabled(bool h, bool e);
#endif

    void attachFlickable(QQuickFlickable *flickable);
    void detachFlickable();
    void ensureCursorVisible();
    void resizeFlickableControl();
    void resizeFlickableContent();

    void itemGeometryChanged(QQuickItem *item, QQuickGeometryChange change, const QRectF &diff) override;

    qreal getImplicitWidth() const override;
    qreal getImplicitHeight() const override;

    void implicitWidthChanged() override;
    void implicitHeightChanged() override;

    void readOnlyChanged(bool isReadOnly);

#if QT_CONFIG(accessibility)
    void accessibilityActiveChanged(bool active) override;
    QAccessible::Role accessibleRole() const override;
#endif

    void cancelBackground();
    void executeBackground(bool complete = false);

#if QT_CONFIG(quicktemplates2_hover)
    bool hovered;
    bool explicitHoverEnabled;
#endif
    QFont font;
    QQuickDeferredPointer<QQuickItem> background;
    QString placeholder;
    Qt::FocusReason focusReason;
    QQuickPressHandler pressHandler;
    QQuickAccessibleAttached *accessibleAttached;
    QQuickFlickable *flickable;
};

QT_END_NAMESPACE

#endif // QQUICKTEXTAREA_P_P_H

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

#ifndef QV4GLOBAL_H
#define QV4GLOBAL_H

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

#include <QtCore/qglobal.h>
#include <QString>

#ifdef QT_NO_DEBUG
#define QML_NEARLY_ALWAYS_INLINE Q_ALWAYS_INLINE
#else
#define QML_NEARLY_ALWAYS_INLINE inline
#endif

#include <qtqmlglobal.h>
#include <private/qtqmlglobal_p.h>

#ifdef QT_QML_BOOTSTRAPPED
#define V4_BOOTSTRAP
#endif

#if defined(Q_CC_MSVC)
#include <float.h>
#include <math.h>

namespace std {

inline bool isinf(double d) { return !_finite(d) && !_isnan(d); }
inline bool isnan(double d) { return !!_isnan(d); }
inline bool isfinite(double d) { return _finite(d); }
#if _MSC_VER < 1800
inline bool signbit(double d) { return _copysign(1.0, d) < 0; }
#endif

} // namespace std

inline double trunc(double d) { return d > 0 ? floor(d) : ceil(d); }
#endif

// Decide whether to enable or disable the JIT

// White list architectures
//
// NOTE: This should match the logic in qv4targetplatform_p.h!

#if defined(Q_PROCESSOR_X86) && (QT_POINTER_SIZE == 4) \
    && (defined(Q_OS_WIN) || defined(Q_OS_LINUX) || defined(Q_OS_QNX) || defined(Q_OS_FREEBSD))
#  define V4_ENABLE_JIT
#elif defined(Q_PROCESSOR_X86_64) && (QT_POINTER_SIZE == 8) \
    && (defined(Q_OS_WIN) || defined(Q_OS_LINUX) || defined(Q_OS_QNX) || defined(Q_OS_MAC) || defined(Q_OS_FREEBSD))
#  define V4_ENABLE_JIT
#elif defined(Q_PROCESSOR_ARM_32) && (QT_POINTER_SIZE == 4)
#  if defined(thumb2) || defined(__thumb2__) || ((defined(__thumb) || defined(__thumb__)) && __TARGET_ARCH_THUMB-0 == 4)
#    define V4_ENABLE_JIT
#  elif defined(__ARM_ARCH_ISA_THUMB) && __ARM_ARCH_ISA_THUMB == 2 // clang 3.5 and later will set this if the core supports the Thumb-2 ISA.
#    define V4_ENABLE_JIT
#  endif
#elif defined(Q_PROCESSOR_ARM_64) && (QT_POINTER_SIZE == 8)
#  if defined(Q_OS_LINUX) || defined(Q_OS_QNX)
#    define V4_ENABLE_JIT
#  endif
#elif defined(Q_PROCESSOR_MIPS_32) && defined(Q_OS_LINUX)
#  define V4_ENABLE_JIT
#endif

// Black list some platforms
#if defined(V4_ENABLE_JIT)
#if defined(Q_OS_IOS) || defined(Q_OS_TVOS)
#    undef V4_ENABLE_JIT
#endif
#endif

// For debug purposes: add CONFIG+=force-compile-jit to qmake's command-line to always compile the JIT.
#if defined(V4_FORCE_COMPILE_JIT) && !defined(V4_ENABLE_JIT)
#  define V4_ENABLE_JIT
#endif

// Do certain things depending on whether the JIT is enabled or disabled

#ifdef V4_ENABLE_JIT
#define ENABLE_YARR_JIT 1
#define ENABLE_JIT 1
#define ENABLE_ASSEMBLER 1
#else
#define ENABLE_YARR_JIT 0
#define ENABLE_ASSEMBLER 0
#define ENABLE_JIT 0
#endif

#if defined(Q_OS_QNX) && defined(_CPPLIB_VER)
#include <math.h>
#undef isnan
#undef isfinite
#undef isinf
#undef signbit
#endif

QT_BEGIN_NAMESPACE

namespace QV4 {

namespace Heap {
    struct Base;
    struct MemberData;
    struct ArrayData;

    struct String;
    struct Object;
    struct ObjectPrototype;

    struct ExecutionContext;
    struct GlobalContext;
    struct CallContext;
    struct ScriptFunction;

    struct BooleanObject;
    struct NumberObject;
    struct StringObject;
    struct ArrayObject;
    struct DateObject;
    struct FunctionObject;
    struct ErrorObject;
    struct ArgumentsObject;
    struct QObjectWrapper;
    struct RegExpObject;
    struct RegExp;
    struct EvalFunction;

    struct ArrayBuffer;
    struct DataView;
    struct TypedArray;

}

class MemoryManager;
struct String;
struct Object;
struct ObjectPrototype;
struct ObjectIterator;
struct ExecutionContext;
struct GlobalContext;
struct CallContext;
struct ScriptFunction;
struct InternalClass;
struct Property;
struct Value;
struct Lookup;
struct ArrayData;
struct VTable;

struct BooleanObject;
struct NumberObject;
struct StringObject;
struct ArrayObject;
struct DateObject;
struct FunctionObject;
struct BuiltinFunction;
struct ErrorObject;
struct ArgumentsObject;
struct Managed;
struct ExecutionEngine;
struct QObjectWrapper;
struct RegExpObject;
struct RegExp;
struct EvalFunction;

struct ArrayBuffer;
struct DataView;
struct TypedArray;

// ReturnedValue is used to return values from runtime methods
// the type has to be a primitive type (no struct or union), so that the compiler
// will return it in a register on all platforms.
// It will be returned in rax on x64, [eax,edx] on x86 and [r0,r1] on arm
typedef quint64 ReturnedValue;
struct CallData;
struct Scope;
struct ScopedValue;
template<typename T> struct Scoped;
typedef Scoped<String> ScopedString;
typedef Scoped<Object> ScopedObject;
typedef Scoped<ArrayObject> ScopedArrayObject;
typedef Scoped<FunctionObject> ScopedFunctionObject;
typedef Scoped<ExecutionContext> ScopedContext;

struct PersistentValueStorage;
class PersistentValue;
class WeakValue;

struct IdentifierTable;
class RegExpCache;
class MultiplyWrappedQObjectMap;

namespace Global {
    enum {
        ReservedArgumentCount = 6
    };
}

enum PropertyFlag {
    Attr_Data = 0,
    Attr_Accessor = 0x1,
    Attr_NotWritable = 0x2,
    Attr_NotEnumerable = 0x4,
    Attr_NotConfigurable = 0x8,
    Attr_ReadOnly = Attr_NotWritable|Attr_NotEnumerable|Attr_NotConfigurable,
    Attr_Invalid = 0xff
};

Q_DECLARE_FLAGS(PropertyFlags, PropertyFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(PropertyFlags)

struct PropertyAttributes
{
    union {
        uchar m_all;
        struct {
            uchar m_flags : 4;
            uchar m_mask : 4;
        };
        struct {
            uchar m_type : 1;
            uchar m_writable : 1;
            uchar m_enumerable : 1;
            uchar m_configurable : 1;
            uchar type_set : 1;
            uchar writable_set : 1;
            uchar enumerable_set : 1;
            uchar configurable_set : 1;
        };
    };

    enum Type {
        Data = 0,
        Accessor = 1,
        Generic = 2
    };

    PropertyAttributes() : m_all(0) {}
    PropertyAttributes(PropertyFlag f) : m_all(0) {
        if (f != Attr_Invalid) {
            setType(f & Attr_Accessor ? Accessor : Data);
            if (!(f & Attr_Accessor))
                setWritable(!(f & Attr_NotWritable));
            setEnumerable(!(f & Attr_NotEnumerable));
            setConfigurable(!(f & Attr_NotConfigurable));
        }
    }
    PropertyAttributes(PropertyFlags f) : m_all(0) {
        if (f != Attr_Invalid) {
            setType(f & Attr_Accessor ? Accessor : Data);
            if (!(f & Attr_Accessor))
                setWritable(!(f & Attr_NotWritable));
            setEnumerable(!(f & Attr_NotEnumerable));
            setConfigurable(!(f & Attr_NotConfigurable));
        }
    }

    void setType(Type t) { m_type = t; type_set = true; }
    Type type() const { return type_set ? (Type)m_type : Generic; }

    bool isData() const { return type() == PropertyAttributes::Data || writable_set; }
    bool isAccessor() const { return type() == PropertyAttributes::Accessor; }
    bool isGeneric() const { return type() == PropertyAttributes::Generic && !writable_set; }

    bool hasType() const { return type_set; }
    bool hasWritable() const { return writable_set; }
    bool hasConfigurable() const { return configurable_set; }
    bool hasEnumerable() const { return enumerable_set; }

    void setWritable(bool b) { m_writable = b; writable_set = true; }
    void setConfigurable(bool b) { m_configurable = b; configurable_set = true; }
    void setEnumerable(bool b) { m_enumerable = b; enumerable_set = true; }

    void resolve() { m_mask = 0xf; if (m_type == Accessor) { m_writable = false; writable_set = false; } }

    bool isWritable() const { return m_type != Data || m_writable; }
    bool isEnumerable() const { return m_enumerable; }
    bool isConfigurable() const { return m_configurable; }

    void clearType() { m_type = Data; type_set = false; }
    void clearWritable() { m_writable = false; writable_set = false; }
    void clearEnumerable() { m_enumerable = false; enumerable_set = false; }
    void clearConfigurable() { m_configurable = false; configurable_set = false; }

    void clear() { m_all = 0; }
    bool isEmpty() const { return !m_all; }

    uint flags() const { return m_flags; }

    bool operator==(PropertyAttributes other) {
        return m_all == other.m_all;
    }
    bool operator!=(PropertyAttributes other) {
        return m_all != other.m_all;
    }
};

struct StackFrame {
    QString source;
    QString function;
    int line;
    int column;
};
typedef QVector<StackFrame> StackTrace;

}

Q_DECLARE_TYPEINFO(QV4::PropertyAttributes, Q_PRIMITIVE_TYPE);

QT_END_NAMESPACE

#endif // QV4GLOBAL_H

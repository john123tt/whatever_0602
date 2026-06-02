/****************************************************************************
** Meta object code from reading C++ file 'realtimeprocessingsettingview.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../realtimeprocessingsettingview.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'realtimeprocessingsettingview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN29RealTimeProcessingSettingViewE_t {};
} // unnamed namespace

template <> constexpr inline auto RealTimeProcessingSettingView::qt_create_metaobjectdata<qt_meta_tag_ZN29RealTimeProcessingSettingViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RealTimeProcessingSettingView",
        "adjust_gain",
        "",
        "eliminate_background",
        "is_enabled",
        "change_color_stops",
        "QList<std::pair<double,QColor>>",
        "colorStops",
        "mine_ground_trend_flatten_changed",
        "enabled",
        "v2_hyperbola_detector_changed",
        "on_adjust_gain_button_clicked",
        "on_eliminate_background_currentIndexChanged",
        "index",
        "on_comboBox_currentIndexChanged",
        "on_time_domain_transform_currentIndexChanged",
        "on_mine_ground_trend_flatten_currentIndexChanged",
        "on_v2_hyperbola_detector_currentIndexChanged"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'adjust_gain'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'eliminate_background'
        QtMocHelpers::SignalData<void(bool)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 4 },
        }}),
        // Signal 'change_color_stops'
        QtMocHelpers::SignalData<void(const QVector<QPair<double,QColor>> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Signal 'mine_ground_trend_flatten_changed'
        QtMocHelpers::SignalData<void(bool)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 9 },
        }}),
        // Signal 'v2_hyperbola_detector_changed'
        QtMocHelpers::SignalData<void(bool)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 9 },
        }}),
        // Slot 'on_adjust_gain_button_clicked'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_eliminate_background_currentIndexChanged'
        QtMocHelpers::SlotData<void(int)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
        // Slot 'on_comboBox_currentIndexChanged'
        QtMocHelpers::SlotData<void(int)>(14, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
        // Slot 'on_time_domain_transform_currentIndexChanged'
        QtMocHelpers::SlotData<void(int)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
        // Slot 'on_mine_ground_trend_flatten_currentIndexChanged'
        QtMocHelpers::SlotData<void(int)>(16, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
        // Slot 'on_v2_hyperbola_detector_currentIndexChanged'
        QtMocHelpers::SlotData<void(int)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<RealTimeProcessingSettingView, qt_meta_tag_ZN29RealTimeProcessingSettingViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject RealTimeProcessingSettingView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN29RealTimeProcessingSettingViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN29RealTimeProcessingSettingViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN29RealTimeProcessingSettingViewE_t>.metaTypes,
    nullptr
} };

void RealTimeProcessingSettingView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RealTimeProcessingSettingView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->adjust_gain(); break;
        case 1: _t->eliminate_background((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->change_color_stops((*reinterpret_cast< std::add_pointer_t<QList<std::pair<double,QColor>>>>(_a[1]))); break;
        case 3: _t->mine_ground_trend_flatten_changed((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 4: _t->v2_hyperbola_detector_changed((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 5: _t->on_adjust_gain_button_clicked(); break;
        case 6: _t->on_eliminate_background_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->on_comboBox_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->on_time_domain_transform_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->on_mine_ground_trend_flatten_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 10: _t->on_v2_hyperbola_detector_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (RealTimeProcessingSettingView::*)()>(_a, &RealTimeProcessingSettingView::adjust_gain, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (RealTimeProcessingSettingView::*)(bool )>(_a, &RealTimeProcessingSettingView::eliminate_background, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (RealTimeProcessingSettingView::*)(const QVector<QPair<double,QColor>> & )>(_a, &RealTimeProcessingSettingView::change_color_stops, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (RealTimeProcessingSettingView::*)(bool )>(_a, &RealTimeProcessingSettingView::mine_ground_trend_flatten_changed, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (RealTimeProcessingSettingView::*)(bool )>(_a, &RealTimeProcessingSettingView::v2_hyperbola_detector_changed, 4))
            return;
    }
}

const QMetaObject *RealTimeProcessingSettingView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RealTimeProcessingSettingView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN29RealTimeProcessingSettingViewE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int RealTimeProcessingSettingView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void RealTimeProcessingSettingView::adjust_gain()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void RealTimeProcessingSettingView::eliminate_background(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void RealTimeProcessingSettingView::change_color_stops(const QVector<QPair<double,QColor>> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void RealTimeProcessingSettingView::mine_ground_trend_flatten_changed(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void RealTimeProcessingSettingView::v2_hyperbola_detector_changed(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP

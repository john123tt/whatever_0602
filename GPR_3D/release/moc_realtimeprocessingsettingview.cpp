/****************************************************************************
** Meta object code from reading C++ file 'realtimeprocessingsettingview.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../realtimeprocessingsettingview.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'realtimeprocessingsettingview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.0. It"
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

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSRealTimeProcessingSettingViewENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSRealTimeProcessingSettingViewENDCLASS = QtMocHelpers::stringData(
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
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSRealTimeProcessingSettingViewENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   80,    2, 0x06,    1 /* Public */,
       3,    1,   81,    2, 0x06,    2 /* Public */,
       5,    1,   84,    2, 0x06,    4 /* Public */,
       8,    1,   87,    2, 0x06,    6 /* Public */,
      10,    1,   90,    2, 0x06,    8 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      11,    0,   93,    2, 0x08,   10 /* Private */,
      12,    1,   94,    2, 0x08,   11 /* Private */,
      14,    1,   97,    2, 0x08,   13 /* Private */,
      15,    1,  100,    2, 0x08,   15 /* Private */,
      16,    1,  103,    2, 0x08,   17 /* Private */,
      17,    1,  106,    2, 0x08,   19 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void, QMetaType::Bool,    9,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, QMetaType::Int,   13,

       0        // eod
};

Q_CONSTINIT const QMetaObject RealTimeProcessingSettingView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSRealTimeProcessingSettingViewENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSRealTimeProcessingSettingViewENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSRealTimeProcessingSettingViewENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<RealTimeProcessingSettingView, std::true_type>,
        // method 'adjust_gain'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'eliminate_background'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'change_color_stops'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVector<QPair<double,QColor>> &, std::false_type>,
        // method 'mine_ground_trend_flatten_changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'v2_hyperbola_detector_changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_adjust_gain_button_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_eliminate_background_currentIndexChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_comboBox_currentIndexChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_time_domain_transform_currentIndexChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_mine_ground_trend_flatten_currentIndexChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_v2_hyperbola_detector_currentIndexChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void RealTimeProcessingSettingView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RealTimeProcessingSettingView *>(_o);
        (void)_t;
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
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RealTimeProcessingSettingView::*)();
            if (_t _q_method = &RealTimeProcessingSettingView::adjust_gain; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (RealTimeProcessingSettingView::*)(bool );
            if (_t _q_method = &RealTimeProcessingSettingView::eliminate_background; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (RealTimeProcessingSettingView::*)(const QVector<QPair<double,QColor>> & );
            if (_t _q_method = &RealTimeProcessingSettingView::change_color_stops; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (RealTimeProcessingSettingView::*)(bool );
            if (_t _q_method = &RealTimeProcessingSettingView::mine_ground_trend_flatten_changed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (RealTimeProcessingSettingView::*)(bool );
            if (_t _q_method = &RealTimeProcessingSettingView::v2_hyperbola_detector_changed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject *RealTimeProcessingSettingView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RealTimeProcessingSettingView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSRealTimeProcessingSettingViewENDCLASS.stringdata0))
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
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
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
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void RealTimeProcessingSettingView::change_color_stops(const QVector<QPair<double,QColor>> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void RealTimeProcessingSettingView::mine_ground_trend_flatten_changed(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void RealTimeProcessingSettingView::v2_hyperbola_detector_changed(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP

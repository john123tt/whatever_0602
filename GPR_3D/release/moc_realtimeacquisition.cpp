/****************************************************************************
** Meta object code from reading C++ file 'realtimeacquisition.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../realtimeacquisition.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'realtimeacquisition.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSrealtimeacquisitionENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSrealtimeacquisitionENDCLASS = QtMocHelpers::stringData(
    "realtimeacquisition",
    "error_occur",
    "",
    "error_code",
    "data_ready",
    "QList<QList<GPR_Complex>>",
    "data",
    "is_last",
    "realtime_play",
    "std::vector<GPR_TxRxChannel>",
    "m_tx_rx_channels",
    "raw_frame_arrived",
    "GPR_DataHeader",
    "header",
    "capture_state_changed",
    "running",
    "on_One_Click_Configuration_button_clicked",
    "on_error",
    "on_Start_Capture_Data_button_toggled",
    "checked",
    "on_save_file_check_box_clicked",
    "on_raw_frame_arrived"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSrealtimeacquisitionENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   74,    2, 0x06,    1 /* Public */,
       4,    2,   77,    2, 0x06,    3 /* Public */,
       8,    1,   82,    2, 0x06,    6 /* Public */,
      11,    2,   85,    2, 0x06,    8 /* Public */,
      14,    1,   90,    2, 0x06,   11 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      16,    0,   93,    2, 0x08,   13 /* Private */,
      17,    1,   94,    2, 0x08,   14 /* Private */,
      18,    1,   97,    2, 0x08,   16 /* Private */,
      20,    1,  100,    2, 0x08,   18 /* Private */,
      21,    2,  103,    2, 0x08,   20 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, 0x80000000 | 5, QMetaType::Bool,    6,    7,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 5,   13,    6,
    QMetaType::Void, QMetaType::Bool,   15,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Bool,   19,
    QMetaType::Void, QMetaType::Bool,   19,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 5,   13,    6,

       0        // eod
};

Q_CONSTINIT const QMetaObject realtimeacquisition::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSrealtimeacquisitionENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSrealtimeacquisitionENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSrealtimeacquisitionENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<realtimeacquisition, std::true_type>,
        // method 'error_occur'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'data_ready'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVector<QVector<GPR_Complex>> &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'realtime_play'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<std::vector<GPR_TxRxChannel>, std::false_type>,
        // method 'raw_frame_arrived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<GPR_DataHeader, std::false_type>,
        QtPrivate::TypeAndForceComplete<QVector<QVector<GPR_Complex>>, std::false_type>,
        // method 'capture_state_changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_One_Click_Configuration_button_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_error'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_Start_Capture_Data_button_toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_save_file_check_box_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_raw_frame_arrived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const GPR_DataHeader &, std::false_type>,
        QtPrivate::TypeAndForceComplete<QVector<QVector<GPR_Complex>>, std::false_type>
    >,
    nullptr
} };

void realtimeacquisition::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<realtimeacquisition *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->error_occur((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->data_ready((*reinterpret_cast< std::add_pointer_t<QList<QList<GPR_Complex>>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 2: _t->realtime_play((*reinterpret_cast< std::add_pointer_t<std::vector<GPR_TxRxChannel>>>(_a[1]))); break;
        case 3: _t->raw_frame_arrived((*reinterpret_cast< std::add_pointer_t<GPR_DataHeader>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QList<QList<GPR_Complex>>>>(_a[2]))); break;
        case 4: _t->capture_state_changed((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 5: _t->on_One_Click_Configuration_button_clicked(); break;
        case 6: _t->on_error((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->on_Start_Capture_Data_button_toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 8: _t->on_save_file_check_box_clicked((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 9: _t->on_raw_frame_arrived((*reinterpret_cast< std::add_pointer_t<GPR_DataHeader>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QList<QList<GPR_Complex>>>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (realtimeacquisition::*)(int );
            if (_t _q_method = &realtimeacquisition::error_occur; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (realtimeacquisition::*)(const QVector<QVector<GPR_Complex>> & , bool );
            if (_t _q_method = &realtimeacquisition::data_ready; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (realtimeacquisition::*)(std::vector<GPR_TxRxChannel> );
            if (_t _q_method = &realtimeacquisition::realtime_play; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (realtimeacquisition::*)(GPR_DataHeader , QVector<QVector<GPR_Complex>> );
            if (_t _q_method = &realtimeacquisition::raw_frame_arrived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (realtimeacquisition::*)(bool );
            if (_t _q_method = &realtimeacquisition::capture_state_changed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject *realtimeacquisition::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *realtimeacquisition::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSrealtimeacquisitionENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int realtimeacquisition::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void realtimeacquisition::error_occur(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void realtimeacquisition::data_ready(const QVector<QVector<GPR_Complex>> & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void realtimeacquisition::realtime_play(std::vector<GPR_TxRxChannel> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void realtimeacquisition::raw_frame_arrived(GPR_DataHeader _t1, QVector<QVector<GPR_Complex>> _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void realtimeacquisition::capture_state_changed(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP

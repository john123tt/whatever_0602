/****************************************************************************
** Meta object code from reading C++ file 'realtimeacquisition.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../realtimeacquisition.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'realtimeacquisition.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN19realtimeacquisitionE_t {};
} // unnamed namespace

template <> constexpr inline auto realtimeacquisition::qt_create_metaobjectdata<qt_meta_tag_ZN19realtimeacquisitionE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
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
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'error_occur'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'data_ready'
        QtMocHelpers::SignalData<void(const QVector<QVector<GPR_Complex>> &, bool)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 }, { QMetaType::Bool, 7 },
        }}),
        // Signal 'realtime_play'
        QtMocHelpers::SignalData<void(std::vector<GPR_TxRxChannel>)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 },
        }}),
        // Signal 'raw_frame_arrived'
        QtMocHelpers::SignalData<void(GPR_DataHeader, QVector<QVector<GPR_Complex>>)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 12, 13 }, { 0x80000000 | 5, 6 },
        }}),
        // Signal 'capture_state_changed'
        QtMocHelpers::SignalData<void(bool)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 15 },
        }}),
        // Slot 'on_One_Click_Configuration_button_clicked'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_error'
        QtMocHelpers::SlotData<void(int)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Slot 'on_Start_Capture_Data_button_toggled'
        QtMocHelpers::SlotData<void(bool)>(18, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 19 },
        }}),
        // Slot 'on_save_file_check_box_clicked'
        QtMocHelpers::SlotData<void(bool)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 19 },
        }}),
        // Slot 'on_raw_frame_arrived'
        QtMocHelpers::SlotData<void(const GPR_DataHeader &, QVector<QVector<GPR_Complex>>)>(21, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 12, 13 }, { 0x80000000 | 5, 6 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<realtimeacquisition, qt_meta_tag_ZN19realtimeacquisitionE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject realtimeacquisition::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19realtimeacquisitionE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19realtimeacquisitionE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN19realtimeacquisitionE_t>.metaTypes,
    nullptr
} };

void realtimeacquisition::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<realtimeacquisition *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
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
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (realtimeacquisition::*)(int )>(_a, &realtimeacquisition::error_occur, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (realtimeacquisition::*)(const QVector<QVector<GPR_Complex>> & , bool )>(_a, &realtimeacquisition::data_ready, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (realtimeacquisition::*)(std::vector<GPR_TxRxChannel> )>(_a, &realtimeacquisition::realtime_play, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (realtimeacquisition::*)(GPR_DataHeader , QVector<QVector<GPR_Complex>> )>(_a, &realtimeacquisition::raw_frame_arrived, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (realtimeacquisition::*)(bool )>(_a, &realtimeacquisition::capture_state_changed, 4))
            return;
    }
}

const QMetaObject *realtimeacquisition::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *realtimeacquisition::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19realtimeacquisitionE_t>.strings))
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
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void realtimeacquisition::error_occur(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void realtimeacquisition::data_ready(const QVector<QVector<GPR_Complex>> & _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void realtimeacquisition::realtime_play(std::vector<GPR_TxRxChannel> _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void realtimeacquisition::raw_frame_arrived(GPR_DataHeader _t1, QVector<QVector<GPR_Complex>> _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void realtimeacquisition::capture_state_changed(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP

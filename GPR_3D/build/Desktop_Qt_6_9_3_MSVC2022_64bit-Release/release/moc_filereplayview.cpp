/****************************************************************************
** Meta object code from reading C++ file 'filereplayview.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../filereplayview.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filereplayview.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN14FileReplayViewE_t {};
} // unnamed namespace

template <> constexpr inline auto FileReplayView::qt_create_metaobjectdata<qt_meta_tag_ZN14FileReplayViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "FileReplayView",
        "data_ready",
        "",
        "QList<QList<GPR_Complex>>",
        "data",
        "is_last",
        "data_batch_ready",
        "QList<QList<QList<GPR_Complex>>>",
        "data_batch",
        "replay",
        "GPR_FileHeader",
        "file_header",
        "channel_number",
        "error_occur",
        "error_code",
        "replay_state_changed",
        "running",
        "display_channel_changed",
        "channel",
        "on_error",
        "on_replay_file_browse_clicked",
        "on_replay_button_toggled",
        "checked",
        "on_replay_file_path_textChanged",
        "path",
        "on_launch_channel_combo_box_currentIndexChanged",
        "index",
        "on_receive_channel_combo_box_currentIndexChanged"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'data_ready'
        QtMocHelpers::SignalData<void(const QVector<QVector<GPR_Complex>> &, bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::Bool, 5 },
        }}),
        // Signal 'data_ready'
        QtMocHelpers::SignalData<void(const QVector<QVector<GPR_Complex>> &)>(1, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'data_batch_ready'
        QtMocHelpers::SignalData<void(const QVector<QVector<QVector<GPR_Complex>> > &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Signal 'replay'
        QtMocHelpers::SignalData<void(const GPR_FileHeader &, int)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 }, { QMetaType::Int, 12 },
        }}),
        // Signal 'error_occur'
        QtMocHelpers::SignalData<void(int)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 14 },
        }}),
        // Signal 'replay_state_changed'
        QtMocHelpers::SignalData<void(bool)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 16 },
        }}),
        // Signal 'display_channel_changed'
        QtMocHelpers::SignalData<void(int)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 18 },
        }}),
        // Slot 'on_error'
        QtMocHelpers::SlotData<void(int)>(19, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 14 },
        }}),
        // Slot 'on_replay_file_browse_clicked'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_replay_button_toggled'
        QtMocHelpers::SlotData<void(bool)>(21, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 22 },
        }}),
        // Slot 'on_replay_file_path_textChanged'
        QtMocHelpers::SlotData<void(const QString &)>(23, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 24 },
        }}),
        // Slot 'on_launch_channel_combo_box_currentIndexChanged'
        QtMocHelpers::SlotData<void(int)>(25, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 26 },
        }}),
        // Slot 'on_receive_channel_combo_box_currentIndexChanged'
        QtMocHelpers::SlotData<void(int)>(27, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 26 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<FileReplayView, qt_meta_tag_ZN14FileReplayViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject FileReplayView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14FileReplayViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14FileReplayViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14FileReplayViewE_t>.metaTypes,
    nullptr
} };

void FileReplayView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<FileReplayView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->data_ready((*reinterpret_cast< std::add_pointer_t<QList<QList<GPR_Complex>>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 1: _t->data_ready((*reinterpret_cast< std::add_pointer_t<QList<QList<GPR_Complex>>>>(_a[1]))); break;
        case 2: _t->data_batch_ready((*reinterpret_cast< std::add_pointer_t<QList<QList<QList<GPR_Complex>>>>>(_a[1]))); break;
        case 3: _t->replay((*reinterpret_cast< std::add_pointer_t<GPR_FileHeader>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 4: _t->error_occur((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->replay_state_changed((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 6: _t->display_channel_changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->on_error((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->on_replay_file_browse_clicked(); break;
        case 9: _t->on_replay_button_toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 10: _t->on_replay_file_path_textChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->on_launch_channel_combo_box_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 12: _t->on_receive_channel_combo_box_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (FileReplayView::*)(const QVector<QVector<GPR_Complex>> & , bool )>(_a, &FileReplayView::data_ready, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (FileReplayView::*)(const QVector<QVector<QVector<GPR_Complex>> > & )>(_a, &FileReplayView::data_batch_ready, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (FileReplayView::*)(const GPR_FileHeader & , int )>(_a, &FileReplayView::replay, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (FileReplayView::*)(int )>(_a, &FileReplayView::error_occur, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (FileReplayView::*)(bool )>(_a, &FileReplayView::replay_state_changed, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (FileReplayView::*)(int )>(_a, &FileReplayView::display_channel_changed, 6))
            return;
    }
}

const QMetaObject *FileReplayView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileReplayView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14FileReplayViewE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int FileReplayView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void FileReplayView::data_ready(const QVector<QVector<GPR_Complex>> & _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 2
void FileReplayView::data_batch_ready(const QVector<QVector<QVector<GPR_Complex>> > & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void FileReplayView::replay(const GPR_FileHeader & _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void FileReplayView::error_occur(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void FileReplayView::replay_state_changed(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void FileReplayView::display_channel_changed(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}
QT_WARNING_POP

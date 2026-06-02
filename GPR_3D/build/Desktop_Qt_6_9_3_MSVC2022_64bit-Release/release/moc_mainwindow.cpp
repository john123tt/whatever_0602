/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../mainwindow.h"
#include <QtGui/qscreen.h>
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "display_data_ready",
        "",
        "QList<double>",
        "data",
        "is_start",
        "display_data_ready_topview",
        "display_data_ready_sideview",
        "processing_data_ready",
        "is_last",
        "processing_data_ready_topview",
        "processing_data_ready_sideview",
        "time_domain_data_ready",
        "x",
        "y",
        "on_display_data",
        "QList<QList<GPR_Complex>>",
        "on_display_data_batch",
        "QList<QList<QList<GPR_Complex>>>",
        "data_batch",
        "on_processing_data_ready",
        "on_processing_data_ready_topview",
        "on_processing_data_ready_sideview",
        "on_replay",
        "GPR_FileHeader",
        "file_header",
        "channel_number",
        "on_replay_display_channel_changed",
        "channel",
        "onPlotDoubleClick",
        "m_plot_id",
        "onPlotDoubleClick_topview",
        "onPlotDoubleClick_sideview",
        "on_eliminate_background",
        "is_open",
        "on_adjust_gain",
        "on_set_volume_data",
        "on_auto_adjust_gain_combo_box_currentIndexChanged",
        "index",
        "on_mean_background_combo_box_currentIndexChanged",
        "on_linear_gain_changed",
        "value",
        "on_topview_avg_radius_changed",
        "on_topview_avg_radius_released",
        "on_topview_range_changed",
        "on_topview_point_range_changed",
        "lower",
        "upper",
        "update_topview_avg_preview",
        "on_main_axis_rect_changed",
        "top",
        "bottom",
        "height",
        "on_map_k_changed",
        "on_mine_ground_trend_flatten_changed",
        "enabled",
        "on_v2_hyperbola_detector_changed",
        "update_range_avg_display",
        "alignAngleSliderWithYAxis"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'display_data_ready'
        QtMocHelpers::SignalData<void(const QVector<double> &, bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::Bool, 5 },
        }}),
        // Signal 'display_data_ready_topview'
        QtMocHelpers::SignalData<void(const QVector<double> &, bool)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::Bool, 5 },
        }}),
        // Signal 'display_data_ready_sideview'
        QtMocHelpers::SignalData<void(const QVector<double> &, bool)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::Bool, 5 },
        }}),
        // Signal 'processing_data_ready'
        QtMocHelpers::SignalData<void(const QVector<double> &, bool)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::Bool, 9 },
        }}),
        // Signal 'processing_data_ready'
        QtMocHelpers::SignalData<void(const QVector<double> &)>(8, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'processing_data_ready_topview'
        QtMocHelpers::SignalData<void(const QVector<double> &, bool)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::Bool, 9 },
        }}),
        // Signal 'processing_data_ready_topview'
        QtMocHelpers::SignalData<void(const QVector<double> &)>(10, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'processing_data_ready_sideview'
        QtMocHelpers::SignalData<void(const QVector<double> &, bool)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::Bool, 9 },
        }}),
        // Signal 'processing_data_ready_sideview'
        QtMocHelpers::SignalData<void(const QVector<double> &)>(11, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'time_domain_data_ready'
        QtMocHelpers::SignalData<void(const QVector<double> &, const QVector<double> &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 13 }, { 0x80000000 | 3, 14 },
        }}),
        // Slot 'on_display_data'
        QtMocHelpers::SlotData<void(const QVector<QVector<GPR_Complex>> &, bool)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 16, 4 }, { QMetaType::Bool, 9 },
        }}),
        // Slot 'on_display_data_batch'
        QtMocHelpers::SlotData<void(const QVector<QVector<QVector<GPR_Complex>> > &)>(17, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 18, 19 },
        }}),
        // Slot 'on_processing_data_ready'
        QtMocHelpers::SlotData<void(const QVector<double> &, bool)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::Bool, 9 },
        }}),
        // Slot 'on_processing_data_ready_topview'
        QtMocHelpers::SlotData<void(const QVector<double> &, bool)>(21, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::Bool, 9 },
        }}),
        // Slot 'on_processing_data_ready_sideview'
        QtMocHelpers::SlotData<void(const QVector<double> &, bool)>(22, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::Bool, 9 },
        }}),
        // Slot 'on_replay'
        QtMocHelpers::SlotData<void(const GPR_FileHeader &, int)>(23, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 24, 25 }, { QMetaType::Int, 26 },
        }}),
        // Slot 'on_replay_display_channel_changed'
        QtMocHelpers::SlotData<void(int)>(27, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 28 },
        }}),
        // Slot 'onPlotDoubleClick'
        QtMocHelpers::SlotData<void(int, int, QString)>(29, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 }, { QMetaType::Int, 14 }, { QMetaType::QString, 30 },
        }}),
        // Slot 'onPlotDoubleClick_topview'
        QtMocHelpers::SlotData<void(int, int, QString)>(31, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 }, { QMetaType::Int, 14 }, { QMetaType::QString, 30 },
        }}),
        // Slot 'onPlotDoubleClick_sideview'
        QtMocHelpers::SlotData<void(int, int, QString)>(32, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 }, { QMetaType::Int, 14 }, { QMetaType::QString, 30 },
        }}),
        // Slot 'on_eliminate_background'
        QtMocHelpers::SlotData<void(bool)>(33, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 34 },
        }}),
        // Slot 'on_adjust_gain'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_set_volume_data'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_auto_adjust_gain_combo_box_currentIndexChanged'
        QtMocHelpers::SlotData<void(int)>(37, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 38 },
        }}),
        // Slot 'on_mean_background_combo_box_currentIndexChanged'
        QtMocHelpers::SlotData<void(int)>(39, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 38 },
        }}),
        // Slot 'on_linear_gain_changed'
        QtMocHelpers::SlotData<void(double)>(40, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Double, 41 },
        }}),
        // Slot 'on_topview_avg_radius_changed'
        QtMocHelpers::SlotData<void(int)>(42, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 41 },
        }}),
        // Slot 'on_topview_avg_radius_released'
        QtMocHelpers::SlotData<void()>(43, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_topview_range_changed'
        QtMocHelpers::SlotData<void()>(44, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_topview_point_range_changed'
        QtMocHelpers::SlotData<void(int, int)>(45, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 46 }, { QMetaType::Int, 47 },
        }}),
        // Slot 'update_topview_avg_preview'
        QtMocHelpers::SlotData<void()>(48, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_main_axis_rect_changed'
        QtMocHelpers::SlotData<void(int, int, int)>(49, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 50 }, { QMetaType::Int, 51 }, { QMetaType::Int, 52 },
        }}),
        // Slot 'on_map_k_changed'
        QtMocHelpers::SlotData<void(double)>(53, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Double, 41 },
        }}),
        // Slot 'on_mine_ground_trend_flatten_changed'
        QtMocHelpers::SlotData<void(bool)>(54, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 55 },
        }}),
        // Slot 'on_v2_hyperbola_detector_changed'
        QtMocHelpers::SlotData<void(bool)>(56, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 55 },
        }}),
        // Slot 'update_range_avg_display'
        QtMocHelpers::SlotData<void()>(57, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'alignAngleSliderWithYAxis'
        QtMocHelpers::SlotData<void()>(58, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'alignAngleSliderWithYAxis'
        QtMocHelpers::SlotData<void(int, int, int)>(58, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 50 }, { QMetaType::Int, 51 }, { QMetaType::Int, 52 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->display_data_ready((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 1: _t->display_data_ready_topview((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 2: _t->display_data_ready_sideview((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 3: _t->processing_data_ready((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 4: _t->processing_data_ready((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1]))); break;
        case 5: _t->processing_data_ready_topview((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 6: _t->processing_data_ready_topview((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1]))); break;
        case 7: _t->processing_data_ready_sideview((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 8: _t->processing_data_ready_sideview((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1]))); break;
        case 9: _t->time_domain_data_ready((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[2]))); break;
        case 10: _t->on_display_data((*reinterpret_cast< std::add_pointer_t<QList<QList<GPR_Complex>>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 11: _t->on_display_data_batch((*reinterpret_cast< std::add_pointer_t<QList<QList<QList<GPR_Complex>>>>>(_a[1]))); break;
        case 12: _t->on_processing_data_ready((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 13: _t->on_processing_data_ready_topview((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 14: _t->on_processing_data_ready_sideview((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 15: _t->on_replay((*reinterpret_cast< std::add_pointer_t<GPR_FileHeader>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 16: _t->on_replay_display_channel_changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 17: _t->onPlotDoubleClick((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 18: _t->onPlotDoubleClick_topview((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 19: _t->onPlotDoubleClick_sideview((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 20: _t->on_eliminate_background((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 21: _t->on_adjust_gain(); break;
        case 22: _t->on_set_volume_data(); break;
        case 23: _t->on_auto_adjust_gain_combo_box_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 24: _t->on_mean_background_combo_box_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 25: _t->on_linear_gain_changed((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 26: _t->on_topview_avg_radius_changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 27: _t->on_topview_avg_radius_released(); break;
        case 28: _t->on_topview_range_changed(); break;
        case 29: _t->on_topview_point_range_changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 30: _t->update_topview_avg_preview(); break;
        case 31: _t->on_main_axis_rect_changed((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 32: _t->on_map_k_changed((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 33: _t->on_mine_ground_trend_flatten_changed((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 34: _t->on_v2_hyperbola_detector_changed((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 35: _t->update_range_avg_display(); break;
        case 36: _t->alignAngleSliderWithYAxis(); break;
        case 37: _t->alignAngleSliderWithYAxis((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            }
            break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            }
            break;
        case 13:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            }
            break;
        case 14:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(const QVector<double> & , bool )>(_a, &MainWindow::display_data_ready, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(const QVector<double> & , bool )>(_a, &MainWindow::display_data_ready_topview, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(const QVector<double> & , bool )>(_a, &MainWindow::display_data_ready_sideview, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(const QVector<double> & , bool )>(_a, &MainWindow::processing_data_ready, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(const QVector<double> & , bool )>(_a, &MainWindow::processing_data_ready_topview, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(const QVector<double> & , bool )>(_a, &MainWindow::processing_data_ready_sideview, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(const QVector<double> & , const QVector<double> & )>(_a, &MainWindow::time_domain_data_ready, 9))
            return;
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 38)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 38;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 38)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 38;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::display_data_ready(const QVector<double> & _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void MainWindow::display_data_ready_topview(const QVector<double> & _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void MainWindow::display_data_ready_sideview(const QVector<double> & _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void MainWindow::processing_data_ready(const QVector<double> & _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 5
void MainWindow::processing_data_ready_topview(const QVector<double> & _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}

// SIGNAL 7
void MainWindow::processing_data_ready_sideview(const QVector<double> & _t1, bool _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2);
}

// SIGNAL 9
void MainWindow::time_domain_data_ready(const QVector<double> & _t1, const QVector<double> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1, _t2);
}
QT_WARNING_POP

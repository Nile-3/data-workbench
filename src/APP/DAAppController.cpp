﻿#include "DAAppController.h"
// Qt
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QStandardPaths>
#include <QFontComboBox>
#include <QComboBox>
#include <QInputDialog>
#include <QMenu>
#include <QCoreApplication>
#include <QActionGroup>
// API
#include "AppMainWindow.h"
#include "DAAppCore.h"
#include "DAAppRibbonArea.h"
#include "DAAppDockingArea.h"
#include "DAAppCommand.h"
#include "DAAppActions.h"
#include "DAAppDataManager.h"
// plugin
// Qt-Advanced-Docking-System
#include "DockManager.h"
#include "DockAreaWidget.h"
// command
// Widget
#include "DAWaitCursorScoped.h"
#include "DADataOperateWidget.h"
#include "DADataOperatePageWidget.h"
#include "DADataManageWidget.h"
#include "DAChartOperateWidget.h"
#include "DAFigureWidget.h"
#include "DAChartWidget.h"
#include "SettingPages/DASettingPageCommon.h"
// Dialog
#include "DAPluginManagerDialog.h"
#include "DAAppSettingDialog.h"
// DACommonWidgets
#include "DAFontEditPannelWidget.h"
#include "DAShapeEditPannelWidget.h"
// Workflow
#include "DAWorkFlowOperateWidget.h"
#include "DAWorkFlowGraphicsView.h"
// project
#include "DAAppProject.h"
// Py
#ifdef DA_ENABLE_PYTHON
#include "DAPyDTypeComboBox.h"
#include "DAPyScripts.h"
#include "pandas/DAPyDataFrame.h"
#include "numpy/DAPyDType.h"
// Widget
#include "DADataOperateOfDataFrameWidget.h"
#endif
//
#include "SettingPages/DAAppConfig.h"

#ifndef DAAPPRIBBONAREA_WINDOW_NAME
#define DAAPPRIBBONAREA_WINDOW_NAME QCoreApplication::translate("DAAppController", "DA", nullptr)
#endif

// 未实现的功能标记
#define DAAPPCONTROLLER_PASS()                                                                                         \
    QMessageBox::                                                                                                      \
            warning(app(),                                                                                             \
                    QCoreApplication::translate("DAAppRibbonArea", "warning", nullptr),                                \
                    QCoreApplication::translate("DAAppRibbonArea",                                                     \
                                                "The current function is not implemented, only the UI is reserved, "   \
                                                "please pay attention: https://gitee.com/czyt1988/data-work-flow",     \
                                                nullptr))

// 快速链接信号槽
#define DAAPPCONTROLLER_ACTION_BIND(actionname, functionname)                                                          \
    connect(actionname, &QAction::triggered, this, &DAAppController::functionname)

namespace DA
{

DAAppController::DAAppController(QObject* par) : QObject(par)
{
}

DAAppController::~DAAppController()
{
}
/**
 * @brief 设置AppMainWindow
 * @param mainWindow
 * @return 返回自身引用,方便链式调用
 */
DAAppController& DAAppController::setAppMainWindow(AppMainWindow* mainWindow)
{
    mMainWindow = mainWindow;
    return (*this);
}

/**
 * @brief 设置core
 * @param core
 * @return
 */
DAAppController& DAAppController::setAppCore(DAAppCore* core)
{
    mCore = core;
    return (*this);
}
/**
 * @brief 设置ribbon
 * @param ribbon
 * @return 返回自身引用,方便链式调用
 */
DAAppController& DAAppController::setAppRibbonArea(DAAppRibbonArea* ribbon)
{
    mRibbon = ribbon;
    return (*this);
}

/**
 * @brief 设置dock
 * @param dock
 * @return 返回自身引用,方便链式调用
 */
DAAppController& DAAppController::setAppDockingArea(DAAppDockingArea* dock)
{
    mDock = dock;
    return (*this);
}

/**
 * @brief 设置AppCommand
 * @param cmd
 * @return 返回自身引用,方便链式调用
 */
DAAppController& DAAppController::setAppCommand(DAAppCommand* cmd)
{
    mCommand = cmd;
    return (*this);
}

/**
 * @brief 设置AppActions
 * @param act
 * @return 返回自身引用,方便链式调用
 */
DAAppController& DAAppController::setAppActions(DAAppActions* act)
{
    mActions = act;
    return (*this);
}

/**
 * @brief 设置AppDataManager
 * @param d
 * @return 返回自身引用,方便链式调用
 */
DAAppController& DAAppController::setAppDataManager(DAAppDataManager* d)
{
    mDatas = d;
    return (*this);
}

/**
 * @brief 获取app
 * @return
 */
AppMainWindow* DAAppController::app() const
{
    return mMainWindow;
}

/**
 * @brief 控制层初始化
 */
void DAAppController::initialize()
{
    initConnection();
#ifdef DA_ENABLE_PYTHON
    initScripts();
#endif
}

/**
 * @brief 获取当前dataframeOperateWidget,如果没有返回nullptr
 *
 * 此函数不返回nullptr的前提是:DataOperateWidget处于焦点，且是DataFrameOperateWidget
 * @param checkDataOperateAreaFocused 是否检测DataOperateWidget是否处于焦点，默认为true
 * @return
 */
DADataOperateOfDataFrameWidget* DAAppController::getCurrentDataFrameOperateWidget(bool checkDataOperateAreaFocused)
{
    if (nullptr == mDock) {
        return nullptr;
    }
    if (checkDataOperateAreaFocused) {
        if (!(mDock->isDockingAreaFocused(DAAppDockingArea::DockingAreaDataOperate))) {
            // 窗口未选中就退出
            return nullptr;
        }
    }
    return mDock->getDataOperateWidget()->getCurrentDataFrameWidget();
}

/**
 * @brief 获取工作流操作窗口
 * @return
 */
DAWorkFlowOperateWidget* DAAppController::getWorkFlowOperateWidget() const
{
    return mDock->getWorkFlowOperateWidget();
}

/**
 * @brief 获取数据操作窗口
 * @return
 */
DADataOperateWidget* DAAppController::getDataOperateWidget() const
{
    return mDock->getDataOperateWidget();
}

/**
 * @brief 获取绘图操作窗口
 * @return
 */
DAChartOperateWidget* DAAppController::getChartOperateWidget() const
{
    return mDock->getChartOperateWidget();
}

/**
 * @brief 获取数据管理窗口
 * @return
 */
DADataManageWidget* DAAppController::getDataManageWidget() const
{
    return mDock->getDataManageWidget();
}
/**
 * @brief 获取当前的绘图
 * @return 如果没有回返回nullptr
 */
DAFigureWidget* DAAppController::getCurrentFigure()
{
    return getChartOperateWidget()->getCurrentFigure();
}

DAFigureWidget* DAAppController::gcf()
{
    return getCurrentFigure();
}

/**
 * @brief 获取当前的图表
 * @return
 */
DAChartWidget* DAAppController::getCurrentChart() const
{
    return getChartOperateWidget()->getCurrentChart();
}

DAChartWidget* DAAppController::gca() const
{
    return getCurrentChart();
}

/**
 * @brief 判断当前是否是在绘图操作模式，就算绘图操作不在焦点，但绘图操作在前端，此函数也返回true
 * @return
 */
bool DAAppController::isLastFocusedOnChartOptWidget() const
{
    return mLastFocusedOpertateWidget.testFlag(LastFocusedOnChartOpt);
}

/**
 * @brief 判断当前是否是在工作流操作模式，就算工作流操作不在焦点，但工作流操作在前端，此函数也返回true
 * @return
 */
bool DAAppController::isLastFocusedOnWorkflowOptWidget() const
{
    return mLastFocusedOpertateWidget.testFlag(LastFocusedOnWorkflowOpt);
}

/**
 * @brief 判断当前是否是在数据操作模式，就算数据操作不在焦点，但工作流操作在前端，此函数也返回true
 * @return
 */
bool DAAppController::isLastFocusedOnDataOptWidget() const
{
    return mLastFocusedOpertateWidget.testFlag(LastFocusedOnDataOpt);
}

/**
 * @brief 基本绑定
 * @note 在setDockAreaInterface函数中还有很多绑定操作
 */
void DAAppController::initConnection()
{
    // Main Category
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionOpen, onActionOpenTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionSave, onActionSaveTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionSaveAs, onActionSaveAsTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionAppendProject, onActionAppendProjectTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionSetting, onActionSettingTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionPluginManager, onActionPluginManagerTriggered);
    // Data Category
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionAddData, onActionAddDataTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionRemoveData, onActionRemoveDataTriggered);
    // Chart Category
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionAddFigure, onActionAddFigureTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionFigureResizeChart, onActionFigureResizeChartTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionFigureNewXYAxis, onActionFigureNewXYAxisTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionChartEnableGrid, onActionChartEnableGridTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionChartEnableGridX, onActionChartEnableGridXTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionChartEnableGridY, onActionChartEnableGridYTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionChartEnableGridXMin, onActionChartEnableGridXMinEnableTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionChartEnableGridYMin, onActionChartEnableGridYMinTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionChartEnableZoom, onActionChartEnableZoomTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionChartZoomIn, onActionChartZoomInTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionChartZoomOut, onActionChartZoomOutTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionChartZoomAll, onActionChartZoomAllTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionChartEnablePan, onActionChartEnablePanTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionChartEnablePickerCross, onActionChartEnablePickerCrossTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionChartEnablePickerY, onActionChartEnablePickerYTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionChartEnablePickerXY, onActionChartEnablePickerXYTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionChartEnableLegend, onActionChartEnableLegendTriggered);
    connect(mActions->actionGroupChartLegendAlignment, &QActionGroup::triggered, this, &DAAppController::onActionGroupChartLegendAlignmentTriggered);
    connect(mRibbon->m_spinboxChartLegendMaxColumns, QOverload< int >::of(&QSpinBox::valueChanged), this, &DAAppController::onChartLegendMaxColumnsValueChanged);
    connect(mRibbon->m_spinboxChartLegendMargin, QOverload< int >::of(&QSpinBox::valueChanged), this, &DAAppController::onChartLegendMarginValueChanged);
    connect(mRibbon->m_spinboxChartLegendSpacing, QOverload< int >::of(&QSpinBox::valueChanged), this, &DAAppController::onChartLegendSpacingValueChanged);
    connect(mRibbon->m_spinboxChartLegendItemMargin, QOverload< int >::of(&QSpinBox::valueChanged), this, &DAAppController::onChartLegendItemMarginValueChanged);
    connect(mRibbon->m_spinboxChartLegendItemSpacing, QOverload< int >::of(&QSpinBox::valueChanged), this, &DAAppController::onChartLegendItemSpacingValueChanged);
    connect(mRibbon->m_spinboxChartLegendBorderRadius,
            QOverload< double >::of(&QDoubleSpinBox::valueChanged),
            this,
            &DAAppController::onChartLegendBorderRadiusValueChanged);
    // 数据操作的上下文标签 Data Operate Context Category
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionInsertRow, onActionInsertRowTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionInsertRowAbove, onActionInsertRowAboveTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionInsertColumnRight, onActionInsertColumnRightTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionInsertColumnLeft, onActionInsertColumnLeftTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionRemoveRow, onActionRemoveRowTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionRemoveColumn, onActionRemoveColumnTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionRemoveCell, onActionRemoveCellTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionRenameColumns, onActionRenameColumnsTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionCreateDataDescribe, onActionCreateDataDescribeTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionCastToNum, onActionCastToNumTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionCastToString, onActionCastToStringTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionCastToDatetime, onActionCastToDatetimeTriggered);
    // 不知为何使用函数指针无法关联信号和槽
    //  connect(m_comboxColumnTypes, &DAPyDTypeComboBox::currentDTypeChanged, this,&DAAppRibbonArea::onComboxColumnTypesCurrentDTypeChanged);
    //  QObject::connect: signal not found in DAPyDTypeComboBox
#ifdef DA_ENABLE_PYTHON
    connect(mRibbon->m_comboxColumnTypes, SIGNAL(currentDTypeChanged(DAPyDType)), this, SLOT(onComboxColumnTypesCurrentDTypeChanged(DAPyDType)));
#endif
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionChangeToIndex, onActionChangeToIndexTriggered);
    // View Category
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionShowWorkFlowArea, onActionShowWorkFlowAreaTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionShowChartArea, onActionShowChartAreaTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionShowDataArea, onActionShowDataAreaTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionShowMessageLogView, onActionShowMessageLogViewTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionShowSettingWidget, onActionSettingWidgetTriggered);

    // workflow edit 工作流编辑
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionWorkflowNew, onActionNewWorkflowTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionWorkflowEnableItemLinkageMove, onActionWorkflowEnableItemLinkageMoveTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionItemGrouping, onActionItemGroupingTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionItemUngroup, onActionItemUngroupTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionWorkflowRun, onActionRunCurrentWorkflowTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionWorkflowTerminate, onActionTerminateCurrentWorkflowTriggered);
    // workflow edit 工作流编辑/data edit 绘图编辑
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionWorkflowStartDrawRect, onActionStartDrawRectTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionWorkflowStartDrawText, onActionStartDrawTextTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionWorkflowAddBackgroundPixmap, onActionAddBackgroundPixmapTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionWorkflowLockBackgroundPixmap, onActionLockBackgroundPixmapTriggered);
    DAAPPCONTROLLER_ACTION_BIND(mActions->actionWorkflowEnableItemMoveWithBackground, onActionEnableItemMoveWithBackgroundTriggered);

    // other
    connect(mActions->actionGroupRibbonTheme, &QActionGroup::triggered, this, &DAAppController::onActionGroupRibbonThemeTriggered);
    //===================================================
    // setDockAreaInterface 有其他的绑定
    //===================================================
    //! 注意！！
    //! 在setDockAreaInterface函数中还有很多绑定操作
    //
    DAAppProject* p = mCore->getAppProject();
    if (p) {
        connect(p, &DAAppProject::projectSaved, this, &DAAppController::onProjectSaved);
        connect(p, &DAAppProject::projectLoaded, this, &DAAppController::onProjectLoaded);
    }
    //===================================================
    // Edit标签字体相关信号槽
    //===================================================
    connect(mRibbon, &DAAppRibbonArea::selectedFont, this, &DAAppController::onEditFontChanged);
    connect(mRibbon, &DAAppRibbonArea::selectedFontColor, this, &DAAppController::onEditFontColorChanged);
    connect(mRibbon, &DAAppRibbonArea::selectedBrush, this, &DAAppController::onEditBrushChanged);
    connect(mRibbon, &DAAppRibbonArea::selectedPen, this, &DAAppController::onEditPenChanged);

    //===================================================
    // workflow窗口字体相关信号槽
    //===================================================

    connect(mRibbon, &DAAppRibbonArea::selectedWorkflowItemFont, this, &DAAppController::onCurrentWorkflowFontChanged);
    connect(mRibbon, &DAAppRibbonArea::selectedWorkflowItemFontColor, this, &DAAppController::onCurrentWorkflowFontColorChanged);
    connect(mRibbon, &DAAppRibbonArea::selectedWorkflowItemBrush, this, &DAAppController::onCurrentWorkflowShapeBackgroundBrushChanged);
    connect(mRibbon, &DAAppRibbonArea::selectedWorkflowItemPen, this, &DAAppController::onCurrentWorkflowShapeBorderPenChanged);

    //===================================================
    // name
    //===================================================
    connect(mDock->dockManager(), &ads::CDockManager::focusedDockWidgetChanged, this, &DAAppController::onFocusedDockWidgetChanged);
    // DADataManageWidget 数据操作
    // DADataOperateWidget
    DADataOperateWidget* dow = mDock->getDataOperateWidget();
    connect(dow, &DADataOperateWidget::pageAdded, this, &DAAppController::onDataOperatePageAdded);
    // DAChartOperateWidget
    DAChartOperateWidget* cow = mDock->getChartOperateWidget();
    connect(cow, &DAChartOperateWidget::figureCreated, this, &DAAppController::onFigureCreated);
    connect(cow, &DAChartOperateWidget::currentFigureChanged, this, &DAAppController::onCurrentFigureChanged);
    connect(cow, &DAChartOperateWidget::chartAdded, this, &DAAppController::onChartAdded);
    connect(cow, &DAChartOperateWidget::currentChartChanged, this, &DAAppController::onCurrentChartChanged);
    // 鼠标动作完成的触发
    connect(mDock->getWorkFlowOperateWidget(), &DAWorkFlowOperateWidget::mouseActionFinished, this, &DAAppController::onWorkFlowGraphicsSceneMouseActionFinished);
    //
    DAWorkFlowOperateWidget* workflowOpt = mDock->getWorkFlowOperateWidget();
    connect(workflowOpt, &DAWorkFlowOperateWidget::selectionItemChanged, this, &DAAppController::onSelectionGraphicsItemChanged);
    connect(workflowOpt, &DAWorkFlowOperateWidget::workflowStartExecute, this, &DAAppController::onWorkflowStartExecute);
    connect(workflowOpt, &DAWorkFlowOperateWidget::workflowFinished, this, &DAAppController::onWorkflowFinished);
    connect(mActions->actionWorkflowShowGrid, &QAction::triggered, workflowOpt, &DAWorkFlowOperateWidget::setCurrentWorkflowShowGrid);
    connect(mActions->actionWorkflowNew, &QAction::triggered, workflowOpt, &DAWorkFlowOperateWidget::appendWorkflowWithDialog);
    connect(mActions->actionWorkflowWholeView, &QAction::triggered, workflowOpt, &DAWorkFlowOperateWidget::setCurrentWorkflowWholeView);
    connect(mActions->actionWorkflowZoomIn, &QAction::triggered, workflowOpt, &DAWorkFlowOperateWidget::setCurrentWorkflowZoomIn);
    connect(mActions->actionWorkflowZoomOut, &QAction::triggered, workflowOpt, &DAWorkFlowOperateWidget::setCurrentWorkflowZoomOut);
}

DAAppConfig* DAAppController::getConfig() const
{
    return mConfig;
}

void DAAppController::setConfig(DAAppConfig* config)
{
    mConfig = config;
}

/**
 * @brief GraphicsScene的鼠标动作执行完成，把action的选中标记清除
 * @param mf
 */
void DAAppController::onWorkFlowGraphicsSceneMouseActionFinished(DAWorkFlowGraphicsScene::MouseActionFlag mf)
{
    switch (mf) {
    case DAWorkFlowGraphicsScene::StartAddRect:
        mActions->actionWorkflowStartDrawRect->setChecked(false);
        break;
    case DAWorkFlowGraphicsScene::StartAddText:
        mActions->actionWorkflowStartDrawText->setChecked(false);
        break;
    default:
        break;
    }
}

/**
 * @brief 插件管理 [config category] - [Plugin Manager]
 * @param on
 */
void DAAppController::onActionPluginManagerTriggered(bool on)
{
    Q_UNUSED(on);
    DAPluginManagerDialog dlg(app());

    dlg.exec();
}

/**
 * @brief 设定界面
 */
void DAAppController::onActionSettingTriggered()
{
    if (mMainWindow) {
        mMainWindow->showSettingDialog();
    }
}

/**
 * @brief DockWidget的焦点变化
 * @param old
 * @param now
 */
void DAAppController::onFocusedDockWidgetChanged(ads::CDockWidget* old, ads::CDockWidget* now)
{
    Q_UNUSED(old);

    if (nullptr == now) {
        mRibbon->hideContextCategory(DAAppRibbonArea::AllContextCategory);
        return;
    }
    // 数据操作窗口激活时，检查是否需要显示m_contextDataFrame
    if (now->widget() == getDataOperateWidget()) {
        // 数据窗口激活
        mLastFocusedOpertateWidget = LastFocusedOnDataOpt;
        mRibbon->showContextCategory(DAAppRibbonArea::ContextCategoryData);
    } else if (now->widget() == getWorkFlowOperateWidget()) {
        // 工作流窗口激活
        mLastFocusedOpertateWidget = LastFocusedOnWorkflowOpt;
        getWorkFlowOperateWidget()->setUndoStackActive();
        mRibbon->showContextCategory(DAAppRibbonArea::ContextCategoryWorkflow);
    } else if (now->widget() == getChartOperateWidget()) {
        // 绘图窗口激活
        mLastFocusedOpertateWidget = LastFocusedOnChartOpt;
        mRibbon->showContextCategory(DAAppRibbonArea::ContextCategoryChart);
    } else if (now->widget() == getDataManageWidget()) {
        if (mCommand) {
            QUndoStack* stack = mCommand->getDataManagerStack();
            if (stack && !(stack->isActive())) {  // Data 相关的窗口 undostack激活
                stack->setActive();
            }
        }
    }
}
/**
 * @brief 打开文件
 */
void DAAppController::onActionOpenTriggered()
{
    // TODO : 这里要加上工程文件的打开支持
    QFileDialog dialog(app());
    QStringList filters;
    filters << tr("project file(*.%1)").arg(DAAppProject::getProjectFileSuffix());
    dialog.setNameFilters(filters);
    if (QDialog::Accepted != dialog.exec()) {
        return;
    }
    QStringList fileNames = dialog.selectedFiles();
    if (fileNames.empty()) {
        return;
    }
    DAAppProject* project = DA_APP_CORE.getAppProject();
    if (!project->getProjectDir().isEmpty()) {
        if (project->isDirty()) {
            // TODO 没有保存。先询问是否保存
            QMessageBox::StandardButton
                    btn = QMessageBox::question(nullptr,
                                                tr("Question"),  // 提示
                                                tr("Another project already exists. Do you want to replace it?")  // 已存在其他工程，是否要替换？
                    );
            if (btn == QMessageBox::Yes) {
                project->clear();
            } else {
                return;
            }
        } else {
            project->clear();
        }
    }
    DA_WAIT_CURSOR_SCOPED();

    if (!project->load(fileNames.first())) {
        qCritical() << tr("failed to load project file:%1").arg(fileNames.first());
        return;
    }
    // 设置工程名称给标题
    app()->setWindowTitle(QString("%1").arg(project->getProjectBaseName()));
}

/**
 * @brief 另存为
 */
void DAAppController::onActionSaveAsTriggered()
{
    QString projectPath = QFileDialog::getSaveFileName(app(),
                                                       tr("Save Project"),  // 保存工程
                                                       QString(),
                                                       tr("project file (*.%1)").arg(DAAppProject::getProjectFileSuffix())  // 工程文件
    );
    if (projectPath.isEmpty()) {
        // 取消退出
        return;
    }
    QFileInfo fi(projectPath);
    if (fi.exists()) {
        // 说明是目录
        QMessageBox::StandardButton btn = QMessageBox::question(nullptr,
                                                                tr("Warning"),
                                                                tr("Whether to overwrite the file:%1").arg(fi.absoluteFilePath()));
        if (btn != QMessageBox::Yes) {
            return;
        }
    }
    // 另存为
    DA_WAIT_CURSOR_SCOPED();
    DAAppProject* project = DA_APP_CORE.getAppProject();
    if (!project->save(projectPath)) {
        qCritical() << tr("Project saved failed!,path is %1").arg(projectPath);  // 工程保存失败！路径位于:%1
        return;
    }
    app()->setWindowTitle(QString("%1").arg(project->getProjectBaseName()));
    qInfo() << tr("Project saved successfully,path is %1").arg(projectPath);  // 工程保存成功，路径位于:%1
}

void DAAppController::onActionAppendProjectTriggered()
{
    QFileDialog dialog(app());
    QStringList filters;
    filters << tr("project file(*.%1)").arg(DAAppProject::getProjectFileSuffix());
    dialog.setNameFilters(filters);
    if (QDialog::Accepted != dialog.exec()) {
        return;
    }
    QStringList fileNames = dialog.selectedFiles();
    if (fileNames.empty()) {
        return;
    }
    DAAppProject* project = DA_APP_CORE.getAppProject();
    DA_WAIT_CURSOR_SCOPED();

    if (!project->appendWorkflowInProject(fileNames.first(), true)) {
        qCritical() << tr("failed to load project file:%1").arg(fileNames.first());
        return;
    }
    // 设置工程名称给标题
    if (project->getProjectBaseName().isEmpty()) {
        app()->setWindowTitle(QString("untitle"));
    } else {
        app()->setWindowTitle(QString("%1").arg(project->getProjectBaseName()));
    }
}

/**
 * @brief 保存工程
 */
void DAAppController::onActionSaveTriggered()
{
    DAAppProject* project   = DA_APP_CORE.getAppProject();
    QString projectFilePath = project->getProjectFilePath();
    qDebug() << "Save Project,Path=" << projectFilePath;
    if (projectFilePath.isEmpty()) {
        QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        projectFilePath = QFileDialog::getSaveFileName(nullptr,
                                                       tr("Save Project"),  // 保存工程
                                                       desktop,
                                                       tr("Project Files (*.%1)").arg(DAAppProject::getProjectFileSuffix())  // 工程文件 (*.%1)
        );
        if (projectFilePath.isEmpty()) {
            // 取消退出
            return;
        }
    }
    bool saveRet = project->save(projectFilePath);
    if (!saveRet) {
        qCritical() << tr("Project saved failed!,path is %1").arg(projectFilePath);  // 工程保存失败！路径位于:%1
    }
}

/**
 * @brief 工程成功保存触发的信号
 * @param path
 */
void DAAppController::onProjectSaved(const QString& path)
{
    DAAppProject* project = DA_APP_CORE.getAppProject();
    app()->setWindowTitle(QString("%1-%2").arg(DAAPPRIBBONAREA_WINDOW_NAME, project->getProjectBaseName()));
    if (mDock) {
        DAWorkFlowOperateWidget* wf = mDock->getWorkFlowOperateWidget();
        if (wf) {
            wf->setCurrentWorkflowName(project->getProjectBaseName());
        }
    }
    qInfo() << tr("Project saved successfully,path is %1").arg(path);  // 工程保存成功，路径位于:%1
}

/**
 * @brief 工程成功加载触发的信号
 * @param path
 */
void DAAppController::onProjectLoaded(const QString& path)
{
    DAAppProject* project = DA_APP_CORE.getAppProject();
    app()->setWindowTitle(QString("%1-%2").arg(DAAPPRIBBONAREA_WINDOW_NAME, project->getProjectBaseName()));
    if (mDock) {
        DAWorkFlowOperateWidget* wf = mDock->getWorkFlowOperateWidget();
        if (wf) {
            wf->setCurrentWorkflowName(project->getProjectBaseName());
        }
    }
    qInfo() << tr("Project load successfully,path is %1").arg(path);  // 工程保存成功，路径位于:%1
}

/**
 * @brief 数据操作窗口添加，需要绑定相关信号槽到ribbon的页面
 * @param page
 */
void DAAppController::onDataOperatePageAdded(DADataOperatePageWidget* page)
{
    switch (page->getDataOperatePageType()) {
    case DADataOperatePageWidget::DataOperateOfDataFrame: {
#ifdef DA_ENABLE_PYTHON
        DADataOperateOfDataFrameWidget* w = static_cast< DADataOperateOfDataFrameWidget* >(page);
        connect(w, &DADataOperateOfDataFrameWidget::selectTypeChanged, this, &DAAppController::onDataOperateDataFrameWidgetSelectTypeChanged);
#endif
    } break;
    default:
        break;
    }
}

#ifdef DA_ENABLE_PYTHON

/**
 * @brief 脚本定义的内容初始化
 */
void DAAppController::initScripts()
{
    if (!mCore->isPythonInterpreterInitialized()) {
        return;
    }
    mFileReadFilters = QStringList(DAPyScripts::getInstance().getIO().getFileReadFilters());
    qDebug() << mFileReadFilters;
}

/**
 * @brief 选择的样式改变信号
 * @param column
 * @param dt
 */
void DAAppController::onDataOperateDataFrameWidgetSelectTypeChanged(const QList< int >& column, DAPyDType dt)
{
    Q_UNUSED(column);
    mRibbon->setDataframeOperateCurrentDType(dt);
}

/**
 * @brief dataframe的列数据类型改变
 * @param index
 */
void DAAppController::onComboxColumnTypesCurrentDTypeChanged(const DA::DAPyDType& dt)
{
    if (DADataOperateOfDataFrameWidget* dfopt = getCurrentDataFrameOperateWidget()) {
        dfopt->changeSelectColumnType(dt);
    }
}
#endif
/**
 * @brief 添加背景图
 */
void DAAppController::onActionAddBackgroundPixmapTriggered()
{
    QStringList filters;
    filters << tr("Image files (*.png *.jpg)")  // 图片文件 (*.png *.jpg)
            << tr("Any files (*)")              // 任意文件 (*)
            ;

    QFileDialog dialog(app());
    dialog.setNameFilters(filters);

    if (QDialog::Accepted != dialog.exec()) {
        return;
    }
    QStringList f = dialog.selectedFiles();
    if (!f.isEmpty()) {
        DAWorkFlowOperateWidget* ow = mDock->getWorkFlowOperateWidget();
        ow->addBackgroundPixmap(f.first());
        mDock->raiseDockingArea(DAAppDockingArea::DockingAreaWorkFlowOperate);
    }
}

void DAAppController::onActionLockBackgroundPixmapTriggered(bool on)
{
    mDock->getWorkFlowOperateWidget()->setBackgroundPixmapLock(on);
}

void DAAppController::onActionEnableItemMoveWithBackgroundTriggered(bool on)
{
    if (DAWorkFlowGraphicsScene* s = mDock->getWorkFlowOperateWidget()->getCurrentWorkFlowScene()) {
        s->enableItemMoveWithBackground(on);
    }
}

/**
   @brief 允许移动图元时，其它和此图元链接起来的图元跟随移动
   @param a
 */
void DAAppController::onActionWorkflowEnableItemLinkageMoveTriggered(bool on)
{
    if (DAWorkFlowGraphicsScene* s = mDock->getWorkFlowOperateWidget()->getCurrentWorkFlowScene()) {
        s->setEnableItemLinkageMove(on);
    }
}

/**
   @brief 分组
 */
void DAAppController::onActionItemGroupingTriggered()
{
    if (DAWorkFlowGraphicsScene* s = mDock->getWorkFlowOperateWidget()->getCurrentWorkFlowScene()) {
        s->groupingSelectItems_();
    }
}

/**
   @brief 取消分组
 */
void DAAppController::onActionItemUngroupTriggered()
{
    if (DAWorkFlowGraphicsScene* s = mDock->getWorkFlowOperateWidget()->getCurrentWorkFlowScene()) {
        s->removeSelectItemGroup_();
    }
}

/**
   @brief 主题切换
   @param a
 */
void DAAppController::onActionGroupRibbonThemeTriggered(QAction* a)
{
    if (mActions->actionRibbonThemeOffice2013 == a) {
        mMainWindow->setRibbonTheme(SARibbonMainWindow::RibbonThemeOffice2013);
    } else if (mActions->actionRibbonThemeOffice2016Blue == a) {
        mMainWindow->setRibbonTheme(SARibbonMainWindow::RibbonThemeOffice2016Blue);
    } else if (mActions->actionRibbonThemeOffice2021Blue == a) {
        mMainWindow->setRibbonTheme(SARibbonMainWindow::RibbonThemeOffice2021Blue);
    } else if (mActions->actionRibbonThemeDark == a) {
        mMainWindow->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark);
    }
}

void DAAppController::onActionRunCurrentWorkflowTriggered()
{
    qDebug() << "onActionRunCurrentWorkflowTriggered";
    // 先检查是否有工程
    DAAppProject* p = DA_APP_CORE.getAppProject();
    if (nullptr == p) {
        qCritical() << tr("get null project");  // cn:空工程，接口异常
        return;
    }
    QString bn = p->getProjectBaseName();
    if (bn.isEmpty()) {
        QMessageBox::warning(app(),
                             tr("warning"),                                                   // cn:警告
                             tr("Before running the workflow, you need to save the project")  // cn：在运行工作流之前，需要先保存工程
        );
        return;
    }
    mDock->getWorkFlowOperateWidget()->runCurrentWorkFlow();
}

/**
 * @brief 终止当前的工作流
 */
void DAAppController::onActionTerminateCurrentWorkflowTriggered()
{
    qDebug() << "onActionTerminateCurrentWorkflowTriggered";
    mDock->getWorkFlowOperateWidget()->terminateCurrentWorkFlow();
}

void DAAppController::onEditFontChanged(const QFont& f)
{
    if (isLastFocusedOnWorkflowOptWidget()) {
        onCurrentWorkflowFontChanged(f);
    } else if (isLastFocusedOnChartOptWidget()) {
    }
}

void DAAppController::onEditFontColorChanged(const QColor& c)
{
    if (isLastFocusedOnWorkflowOptWidget()) {
        onCurrentWorkflowFontColorChanged(c);
    } else if (isLastFocusedOnChartOptWidget()) {
    }
}

void DAAppController::onEditBrushChanged(const QBrush& b)
{
    if (isLastFocusedOnWorkflowOptWidget()) {
        onCurrentWorkflowShapeBackgroundBrushChanged(b);
    } else if (isLastFocusedOnChartOptWidget()) {
    }
}

void DAAppController::onEditPenChanged(const QPen& p)
{
    if (isLastFocusedOnWorkflowOptWidget()) {
        onCurrentWorkflowShapeBorderPenChanged(p);
    } else if (isLastFocusedOnChartOptWidget()) {
    }
}

void DAAppController::onCurrentWorkflowFontChanged(const QFont& f)
{
    DAWorkFlowOperateWidget* wf = mDock->getWorkFlowOperateWidget();
    wf->setDefaultTextFont(f);
    wf->setSelectTextFont(f);
    // 同步
    mRibbon->setEditFont(f);
}

void DAAppController::onCurrentWorkflowFontColorChanged(const QColor& c)
{
    DAWorkFlowOperateWidget* wf = mDock->getWorkFlowOperateWidget();
    wf->setDefaultTextColor(c);
    wf->setSelectTextColor(c);
    // 同步
    mRibbon->setEditFontColor(c);
}

void DAAppController::onCurrentWorkflowShapeBackgroundBrushChanged(const QBrush& b)
{
    DAWorkFlowOperateWidget* wf = mDock->getWorkFlowOperateWidget();
    wf->setSelectShapeBackgroundBrush(b);
    // 同步
    mRibbon->setEditBrush(b);
}

void DAAppController::onCurrentWorkflowShapeBorderPenChanged(const QPen& p)
{
    DAWorkFlowOperateWidget* wf = mDock->getWorkFlowOperateWidget();
    wf->setSelectShapeBorderPen(p);
    // 同步
    mRibbon->setEditPen(p);
}

void DAAppController::onSelectionGraphicsItemChanged(QGraphicsItem* lastSelectItem)
{
    if (lastSelectItem == nullptr) {
        return;
    }
    if (DAGraphicsItem* daitem = dynamic_cast< DAGraphicsItem* >(lastSelectItem)) {
        // 属于DAGraphicsItem系列
        mRibbon->setWorkFlowEditBrush(daitem->getBackgroundBrush());
        mRibbon->setWorkFlowEditPen(daitem->getBorderPen());
        // 通用编辑同步
        mRibbon->setEditBrush(daitem->getBackgroundBrush());
        mRibbon->setEditPen(daitem->getBorderPen());
    } else if (DAStandardGraphicsTextItem* titem = dynamic_cast< DAStandardGraphicsTextItem* >(lastSelectItem)) {

        mRibbon->setWorkFlowEditFont(titem->font());
        mRibbon->setWorkFlowEditFontColor(titem->defaultTextColor());
        // 通用编辑同步
        mRibbon->setEditFont(titem->font());
        mRibbon->setEditFontColor(titem->defaultTextColor());
    }
}

/**
 * @brief 工作流开始执行的关联槽
 * @param wfw
 */
void DAAppController::onWorkflowStartExecute(DAWorkFlowEditWidget* wfw)
{
    Q_UNUSED(wfw);
    mActions->actionWorkflowRun->setEnabled(false);
    mActions->actionWorkflowTerminate->setEnabled(true);
}

/**
 * @brief 工作流执行结束的关联槽
 * @param wfw
 * @param success
 */
void DAAppController::onWorkflowFinished(DAWorkFlowEditWidget* wfw, bool success)
{
    mActions->actionWorkflowRun->setEnabled(true);
    mActions->actionWorkflowTerminate->setEnabled(false);
}

/**
 * @brief 新fig创建
 * @param f
 */
void DAAppController::onFigureCreated(DAFigureWidget* f)
{
    if (nullptr == f) {
        return;
    }
    qDebug() << "DAAppController::onFigureCreate";
    f->getUndoStack()->setActive();
    // updateFigureAboutRibbon(f);//在onActionAddFigureTriggered中调用了
}

/**
 * @brief 当前的fig变化
 * @param f
 * @param index
 */
void DAAppController::onCurrentFigureChanged(DAFigureWidget* f, int index)
{
    Q_UNUSED(index);
    if (nullptr == f) {
        return;
    }
    qDebug() << "DAAppController::onCurrentFigureChanged";
    f->getUndoStack()->setActive();
    mRibbon->updateFigureAboutRibbon(f);
}

/**
 * @brief 新的chart创建
 * @param c
 */
void DAAppController::onChartAdded(DAChartWidget* c)
{
    if (nullptr == c) {
        return;
    }
    // qDebug() << "DAAppController::onChartAdded";
    // updateChartAboutRibbon(c);//在onActionFigureNewXYAxisTriggered中调用了
}

/**
 * @brief 当前的chart改变
 * @param c
 */
void DAAppController::onCurrentChartChanged(DAChartWidget* c)
{
    if (nullptr == c) {
        return;
    }
    qDebug() << "DAAppController::onCurrentChartChanged";
    mRibbon->updateChartAboutRibbon(c);
}

/**
 * @brief 添加数据
 */
void DAAppController::onActionAddDataTriggered()
{
    QFileDialog dialog(app());
    dialog.setNameFilters(mFileReadFilters);
    if (QDialog::Accepted != dialog.exec()) {
        return;
    }
    QStringList fileNames = dialog.selectedFiles();
    if (fileNames.empty()) {
        return;
    }
    DA_WAIT_CURSOR_SCOPED();
    int importdataCount = mDatas->importFromFiles(fileNames);
    if (importdataCount > 0) {
        mDock->raiseDockByWidget((QWidget*)(mDock->getDataManageWidget()));
    }
}

/**
 * @brief 移除数据
 */
void DAAppController::onActionRemoveDataTriggered()
{
    DADataManageWidget* dmw = mDock->getDataManageWidget();
    dmw->removeSelectData();
}

/**
 * @brief 添加一个figure
 */
void DAAppController::onActionAddFigureTriggered()
{
    DAChartOperateWidget* chartopt = getChartOperateWidget();
    DAFigureWidget* fig            = chartopt->createFigure();
    // 把fig的undostack添加
    mCommand->addStack(fig->getUndoStack());
    // 这里不需要回退
    DAChartWidget* chart = fig->createChart();
    chart->setXLabel("x");
    chart->setYLabel("y");
    mRibbon->updateFigureAboutRibbon(fig);
    mDock->raiseDockingArea(DAAppDockingArea::DockingAreaChartOperate);
}

/**
 * @brief 改变绘图的大小
 * @param on
 */
void DAAppController::onActionFigureResizeChartTriggered(bool on)
{
    DAFigureWidget* fig = getCurrentFigure();
    if (nullptr == fig) {
        return;
    }
    mDock->raiseDockingArea(DAAppDockingArea::DockingAreaChartOperate);
    fig->enableSubChartEditor(on);
}

/**
 * @brief 新建一个坐标系
 */
void DAAppController::onActionFigureNewXYAxisTriggered()
{
    DAFigureWidget* fig = gcf();
    if (!fig) {
        qWarning() << tr("Before creating a new coordinate,you need to create a figure");  // cn:在创建一个坐标系之前，需要先创建一个绘图窗口
        return;
    }
    DAChartWidget* w = fig->createChart_(0.1, 0.1, 0.4, 0.4);
    w->enableGrid();
    w->enablePan();
    w->enableXYDataPicker();
    //    w->addCurve({ 1, 2, 3, 4, 5 }, { 3, 5, 8, 0, -3 })->setTitle("curve1");
    //    w->addCurve({ 1, 2, 3, 4, 5 }, { 5, 7, 0, -1, 1 })->setTitle("curve2");
    mRibbon->updateChartAboutRibbon(w);
    mDock->raiseDockingArea(DAAppDockingArea::DockingAreaChartOperate);
}

/**
 * @brief 创建一个曲线
 */
void DAAppController::onActionChartAddCurveTriggered()
{
    DAChartWidget* w = gca();
    if (!w) {
        qWarning() << tr("Before add a curve,you need to create a axis on figure");  // cn:在添加曲线前，需要先在绘图窗口中添加一个坐标系
        return;
    }
}

/**
 * @brief 允许网格
 * @param on
 */
void DAAppController::onActionChartEnableGridTriggered(bool on)
{
    qDebug() << "onActionChartGridEnableTriggered";
    DAChartWidget* w = getCurrentChart();
    if (w) {
        w->enableGrid(on);
        mRibbon->updateChartGridAboutRibbon(w);
    }
}

/**
 * @brief 横向网格
 * @param on
 */
void DAAppController::onActionChartEnableGridXTriggered(bool on)
{
    DAChartWidget* w = getCurrentChart();
    if (w) {
        w->enableGridX(on);
    }
}
/**
 * @brief 纵向网格
 * @param on
 */
void DAAppController::onActionChartEnableGridYTriggered(bool on)
{
    DAChartWidget* w = getCurrentChart();
    if (w) {
        w->enableGridY(on);
    }
}
/**
 * @brief 横向密集网格
 * @param on
 */
void DAAppController::onActionChartEnableGridXMinEnableTriggered(bool on)
{
    DAChartWidget* w = getCurrentChart();
    if (w) {
        w->enableGridXMin(on);
    }
}
/**
 * @brief 纵向密集网格
 * @param on
 */
void DAAppController::onActionChartEnableGridYMinTriggered(bool on)
{
    DAChartWidget* w = getCurrentChart();
    if (w) {
        w->enableGridYMin(on);
    }
}

/**
 * @brief 当前图表允许缩放
 * @param on
 */
void DAAppController::onActionChartEnableZoomTriggered(bool on)
{
    DAChartWidget* w = getCurrentChart();
    if (w) {
        w->enableZoomer(on);
        mRibbon->updateChartZoomPanAboutRibbon(w);
    }
}

/**
 * @brief 当前图表放大
 */
void DAAppController::onActionChartZoomInTriggered()
{
    DAChartWidget* w = getCurrentChart();
    if (w && w->isEnableZoomer()) {
        w->zoomIn();
    }
}

/**
 * @brief 当前图表缩小
 */
void DAAppController::onActionChartZoomOutTriggered()
{
    DAChartWidget* w = getCurrentChart();
    if (w && w->isEnableZoomer()) {
        w->zoomOut();
    }
}

/**
 * @brief 当前图表全部显示
 */
void DAAppController::onActionChartZoomAllTriggered()
{
    DAChartWidget* w = getCurrentChart();
    if (w) {
        w->zoomInCompatible();
    }
}

/**
 * @brief 允许绘图拖动
 * @param on
 */
void DAAppController::onActionChartEnablePanTriggered(bool on)
{
    DAChartWidget* w = getCurrentChart();
    if (w) {
        w->enablePan(on);
        mRibbon->updateChartZoomPanAboutRibbon(w);
    }
}

/**
 * @brief 允许绘图拾取
 * @param on
 */
void DAAppController::onActionChartEnablePickerCrossTriggered(bool on)
{
    DAChartWidget* w = getCurrentChart();
    if (w) {
        w->enableCrossPicker(on);
        if (on) {
            w->enableYDataPicker(false);
            w->enableXYDataPicker(false);
        }
    }
}

/**
 * @brief 允许绘图拾取Y
 * @param on
 */
void DAAppController::onActionChartEnablePickerYTriggered(bool on)
{
    DAChartWidget* w = getCurrentChart();
    if (w) {
        w->enableYDataPicker(on);
        if (on) {
            w->enableCrossPicker(false);
            w->enableXYDataPicker(false);
        }
    }
}

/**
 * @brief 允许绘图拾取XY
 * @param on
 */
void DAAppController::onActionChartEnablePickerXYTriggered(bool on)
{
    DAChartWidget* w = getCurrentChart();
    if (w) {
        w->enableXYDataPicker(on);
        if (on) {
            w->enableCrossPicker(false);
            w->enableYDataPicker(false);
        }
    }
}

/**
 * @brief 允许图例
 * @param on
 */
void DAAppController::onActionChartEnableLegendTriggered(bool on)
{
    DAChartWidget* w = getCurrentChart();
    if (!w) {
        return;
    }
    w->enableLegend(on);
}

/**
 * @brief 绘图图例对齐的actiongroup
 * @param a
 */
void DAAppController::onActionGroupChartLegendAlignmentTriggered(QAction* a)
{
    DAChartWidget* w = getCurrentChart();
    if (!w) {
        return;
    }
    QwtPlotLegendItem* legend = w->getLegend();
    if (!legend) {
        w->enableLegend(true);
        legend = w->getLegend();
        mActions->actionChartEnableLegend->setChecked(true);
    }
    if (!a->isChecked()) {
        // 无法支持uncheck
        a->setChecked(true);
    }
    Qt::Alignment al = static_cast< Qt::Alignment >(a->data().toInt());
    legend->setAlignmentInCanvas(al);
}

/**
 * @brief 绘图图例的最大列数发生改变
 * @param v
 */
void DAAppController::onChartLegendMaxColumnsValueChanged(int v)
{
    DAChartWidget* w = getCurrentChart();
    if (!w) {
        return;
    }
    QwtPlotLegendItem* legend = w->getLegend();
    if (!legend) {
        return;
    }
    legend->setMaxColumns(v);
}

/**
 * @brief legend的margin发生变化
 * @param v
 */
void DAAppController::onChartLegendMarginValueChanged(int v)
{
    DAChartWidget* w = getCurrentChart();
    if (!w) {
        return;
    }
    QwtPlotLegendItem* legend = w->getLegend();
    if (!legend) {
        return;
    }
    legend->setMargin(v);
}

/**
 * @brief legend的Spacing发生变化
 * @param v
 */
void DAAppController::onChartLegendSpacingValueChanged(int v)
{
    DAChartWidget* w = getCurrentChart();
    if (!w) {
        return;
    }
    QwtPlotLegendItem* legend = w->getLegend();
    if (!legend) {
        return;
    }
    legend->setSpacing(v);
}

/**
 * @brief legend的item margin发生变化
 * @param v
 */
void DAAppController::onChartLegendItemMarginValueChanged(int v)
{
    DAChartWidget* w = getCurrentChart();
    if (!w) {
        return;
    }
    QwtPlotLegendItem* legend = w->getLegend();
    if (!legend) {
        return;
    }
    legend->setItemMargin(v);
}

/**
 * @brief legend的item spacing发生变化
 * @param v
 */
void DAAppController::onChartLegendItemSpacingValueChanged(int v)
{
    DAChartWidget* w = getCurrentChart();
    if (!w) {
        return;
    }
    QwtPlotLegendItem* legend = w->getLegend();
    if (!legend) {
        return;
    }
    legend->setItemSpacing(v);
}

/**
 * @brief legend的圆角发生变化
 * @param v
 */
void DAAppController::onChartLegendBorderRadiusValueChanged(double v)
{
    DAChartWidget* w = getCurrentChart();
    if (!w) {
        return;
    }
    QwtPlotLegendItem* legend = w->getLegend();
    if (!legend) {
        return;
    }
    legend->setBorderRadius(v);
}

/**
 * @brief dataframe删除行
 */
void DAAppController::onActionRemoveRowTriggered()
{
#ifdef DA_ENABLE_PYTHON
    if (DADataOperateOfDataFrameWidget* dfopt = getCurrentDataFrameOperateWidget()) {
        dfopt->removeSelectRow();
    }
#endif
}

/**
 * @brief dataframe删除列
 */
void DAAppController::onActionRemoveColumnTriggered()
{
#ifdef DA_ENABLE_PYTHON
    if (DADataOperateOfDataFrameWidget* dfopt = getCurrentDataFrameOperateWidget()) {
        dfopt->removeSelectColumn();
    }
#endif
}

/**
 * @brief 移除单元格
 */
void DAAppController::onActionRemoveCellTriggered()
{
#ifdef DA_ENABLE_PYTHON
    if (DADataOperateOfDataFrameWidget* dfopt = getCurrentDataFrameOperateWidget()) {
        dfopt->removeSelectCell();
    }
#endif
}

/**
 * @brief 插入行
 */
void DAAppController::onActionInsertRowTriggered()
{
#ifdef DA_ENABLE_PYTHON
    if (DADataOperateOfDataFrameWidget* dfopt = getCurrentDataFrameOperateWidget()) {
        dfopt->insertRowBelowBySelect();
    }
#endif
}

/**
 * @brief 在选中位置上面插入一行
 */
void DAAppController::onActionInsertRowAboveTriggered()
{
#ifdef DA_ENABLE_PYTHON
    if (DADataOperateOfDataFrameWidget* dfopt = getCurrentDataFrameOperateWidget()) {
        dfopt->insertRowAboveBySelect();
    }
#endif
}
/**
 * @brief 在选中位置右边插入一列
 */
void DAAppController::onActionInsertColumnRightTriggered()
{
#ifdef DA_ENABLE_PYTHON
    if (DADataOperateOfDataFrameWidget* dfopt = getCurrentDataFrameOperateWidget()) {
        dfopt->insertColumnRightBySelect();
    }
#endif
}
/**
 * @brief 在选中位置左边插入一列
 */
void DAAppController::onActionInsertColumnLeftTriggered()
{
#ifdef DA_ENABLE_PYTHON
    if (DADataOperateOfDataFrameWidget* dfopt = getCurrentDataFrameOperateWidget()) {
        dfopt->insertColumnLeftBySelect();
    }
#endif
}

/**
 * @brief dataframe列重命名
 */
void DAAppController::onActionRenameColumnsTriggered()
{
#ifdef DA_ENABLE_PYTHON
    if (DADataOperateOfDataFrameWidget* dfopt = getCurrentDataFrameOperateWidget()) {
        dfopt->renameColumns();
    }
#endif
}

/**
 * @brief 创建数据描述
 */
void DAAppController::onActionCreateDataDescribeTriggered()
{
    // TODO 此函数应该移动到dataOperateWidget中
#ifdef DA_ENABLE_PYTHON
    if (DADataOperateOfDataFrameWidget* dfopt = getCurrentDataFrameOperateWidget()) {
        DAPyDataFrame df = dfopt->createDataDescribe();
        if (df.isNone()) {
            return;
        }
        DAData data = df;
        data.setName(tr("%1_Describe").arg(dfopt->data().getName()));
        data.setDescribe(tr("Generate descriptive statistics that summarize the central tendency, dispersion and "
                            "shape of a [%1]’s distribution, excluding NaN values")
                                 .arg(dfopt->data().getName()));
        mDatas->addData(data);
        // showDataOperate要在m_dataManagerStack.push之后，因为m_dataManagerStack.push可能会导致data的名字改变
        mDock->showDataOperateWidget(data);
    }
#endif
}

/**
 * @brief 选中列转换为数值
 */
void DAAppController::onActionCastToNumTriggered()
{
#ifdef DA_ENABLE_PYTHON
    if (DADataOperateOfDataFrameWidget* dfopt = getCurrentDataFrameOperateWidget()) {
        dfopt->castSelectToNum();
    }
#endif
}

/**
 * @brief 选中列转换为文字
 */
void DAAppController::onActionCastToStringTriggered()
{
    DAAPPCONTROLLER_PASS();
}

/**
 * @brief 选中列转换为日期
 */
void DAAppController::onActionCastToDatetimeTriggered()
{
#ifdef DA_ENABLE_PYTHON
    if (DADataOperateOfDataFrameWidget* dfopt = getCurrentDataFrameOperateWidget()) {
        dfopt->castSelectToDatetime();
    }
#endif
}

/**
 * @brief 选中列转换为索引
 */
void DAAppController::onActionChangeToIndexTriggered()
{
#ifdef DA_ENABLE_PYTHON
    if (DADataOperateOfDataFrameWidget* dfopt = getCurrentDataFrameOperateWidget()) {
        dfopt->changeSelectColumnToIndex();
    }
#endif
}

/**
 * @brief 显示工作流区域
 */
void DAAppController::onActionShowWorkFlowAreaTriggered()
{
    mDock->raiseDockByWidget((QWidget*)(mDock->getWorkFlowOperateWidget()));
}

/**
 * @brief 显示绘图区域
 */
void DAAppController::onActionShowChartAreaTriggered()
{
    mDock->raiseDockByWidget((QWidget*)(mDock->getChartOperateWidget()));
}

/**
 * @brief 显示数据区域
 */
void DAAppController::onActionShowDataAreaTriggered()
{
    mDock->raiseDockByWidget((QWidget*)(mDock->getDataOperateWidget()));
}

/**
 * @brief 显示信息区域
 */
void DAAppController::onActionShowMessageLogViewTriggered()
{
    mDock->raiseDockByWidget((QWidget*)(mDock->getMessageLogViewWidget()));
}

/**
 * @brief 显示设置区域
 */
void DAAppController::onActionSettingWidgetTriggered()
{
    mDock->raiseDockByWidget((QWidget*)(mDock->getSettingContainerWidget()));
}

/**
 * @brief 新建工作流
 */
void DAAppController::onActionNewWorkflowTriggered()
{
    bool ok      = false;
    QString text = QInputDialog::getText(app(),
                                         tr("new workflow name"),   // cn:新工作流名称
                                         tr("new workflow name:"),  // cn:新工作流名称
                                         QLineEdit::Normal,
                                         QString(),
                                         &ok);
    if (!ok || text.isEmpty()) {
        return;
    }
    DAWorkFlowOperateWidget* wf = mDock->getWorkFlowOperateWidget();
    wf->appendWorkflow(text);
}

/**
 * @brief 绘制矩形
 *
 * * @note 绘制完成后会触发onWorkFlowGraphicsSceneMouseActionFinished，在此函数中把这个状态消除
 * @param on
 */
void DAAppController::onActionStartDrawRectTriggered(bool on)
{
    if (on) {
        mDock->getWorkFlowOperateWidget()->setMouseActionFlag(DAWorkFlowGraphicsScene::StartAddRect, false);
    }
}

/**
 * @brief 绘制文本
 *
 * @note 绘制完成后会触发onWorkFlowGraphicsSceneMouseActionFinished，在此函数中把这个状态消除
 * @param on
 */
void DAAppController::onActionStartDrawTextTriggered(bool on)
{
    if (on) {
        mDock->getWorkFlowOperateWidget()->setMouseActionFlag(DAWorkFlowGraphicsScene::StartAddText, false);
    }
}
}  // end DA

﻿#ifndef DACHARTCURVEITEMSETTINGWIDGET_H
#define DACHARTCURVEITEMSETTINGWIDGET_H
#include "DAGuiAPI.h"
#include <QWidget>
#include <QPointer>
#include "qwt_plot_curve.h"
// qwt
class QwtPlot;
namespace Ui
{
class DAChartCurveItemSettingWidget;
}

namespace DA
{

/**
 * @brief 曲线设置窗口
 *
 * @note 注意此窗口不保存item
 */
class DAGUI_API DAChartCurveItemSettingWidget : public QWidget
{
    Q_OBJECT

public:
	explicit DAChartCurveItemSettingWidget(QWidget* parent = nullptr);
	~DAChartCurveItemSettingWidget();
	// 根据QwtPlotCurve更新ui
	void updateUI(const QwtPlotCurve* item);
    // 根据ui更新plotitem
	void updatePlotItem(QwtPlotCurve* item);
    // 设置plotitem
    void setPlotItem(QwtPlotCurve* item);
    QwtPlotCurve* getPlotItem() const;

	// 标题
	void setTitle(const QString& t);
	QString getTitle() const;
	// Curve Style
	void setCurveStyle(QwtPlotCurve::CurveStyle v);
	QwtPlotCurve::CurveStyle getCurveStyle() const;
	// Curve Attribute
	void setCurveAttribute(QwtPlotCurve::CurveAttribute v);
	QwtPlotCurve::CurveAttribute getCurveAttribute() const;
	// Legend Attribute
	void setLegendAttribute(QwtPlotCurve::LegendAttributes v);
	QwtPlotCurve::LegendAttributes getLegendAttribute() const;
	// maker编辑
	void enableMarkerEdit(bool on = true);
	bool isEnableMarkerEdit() const;
	// fill编辑
	void enableFillEdit(bool on = true);
	bool isEnableFillEdit() const;
	// 画笔
	void setCurvePen(const QPen& v);
	QPen getCurvePen() const;
	// 填充
	void setFillBrush(const QBrush& v);
	QBrush getFillBrush() const;
	// 基线
	void setBaseLine(double v);
	double getBaseLine() const;
	bool isHaveBaseLine() const;
	// 方向
	void setOrientation(Qt::Orientation v);
	Qt::Orientation getOrientation() const;

protected:
	void resetCurveStyleComboBox();
private slots:
	void onCurveStyleCurrentIndexChanged(int index);

private:
	Ui::DAChartCurveItemSettingWidget* ui;
    QwtPlotCurve* mItem { nullptr };
    QPointer< QwtPlot > mPlot { nullptr };
};
}

#endif  // DACHARTCURVEITEMSETTINGWIDGET_H

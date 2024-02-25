﻿#ifndef DAGRAPHICSITEM_H
#define DAGRAPHICSITEM_H
#include <QGraphicsObject>
#include "DAUtils/DAXMLFileInterface.h"
#include "DAGraphicsViewGlobal.h"
class QDomDocument;
class QDomElement;
namespace DA
{
/**
 * @brief DAGraphicsView的基本元件
 *
 * DAGraphicsItem提供了统一的saveToXml接口
 * 加载的过程通过DAGraphicsItemFactory进行加载
 */
class DAGRAPHICSVIEW_API DAGraphicsItem : public QGraphicsObject, public DAXMLFileInterface
{
    Q_OBJECT
    DA_DECLARE_PRIVATE(DAGraphicsItem)
public:
    /**
     * @brief 适用qgraphicsitem_cast
     */
    enum
    {
        Type = DA::ItemType_DAGraphicsItem
    };
    virtual int type() const override
    {
        return (Type);
    }

public:
    DAGraphicsItem(QGraphicsItem* parent = nullptr);
    ~DAGraphicsItem();
    // 保存到xml中
    virtual bool saveToXml(QDomDocument* doc, QDomElement* parentElement) const override;
    virtual bool loadFromXml(const QDomElement* parentElement) override;
    // 设置边框画笔，如果设置一个QPen,则不绘制边框
    void setBorderPen(const QPen& p);
    QPen getBorderPen() const;
    // 设置是否显示边框
    void enableShowBorder(bool on);
    bool isShowBorder() const;
    // 允许选中
    void enableSelect(bool on = true);
    bool isSelectable() const;
    // 是否允许移动
    void enableMove(bool on = true);
    bool isMovable() const;
    // 背景
    void setBackgroundBrush(const QBrush& b);
    QBrush getBackgroundBrush() const;
    // 设置是否显示背景
    void enableShowBackground(bool on);
    bool isShowBackground() const;
    // 分组位置发生改变的事件
    virtual void groupPositionChanged(const QPointF& p);
    // 设置在场景的位置，如果没有分组，和setPos一样，如果分组了，最终也能保证位置在pos位置
    void setScenePos(const QPointF& p);
    void setScenePos(qreal x, qreal y);
    // 获取item的id，id是这个id唯一的标识，id主要为了能单独的找到这个item，在分组加载时使用
    uint64_t getItemID() const;
    void setItemID(uint64_t id);
    // 快速cast，基于type进行快速判断
    static DAGraphicsItem* cast(QGraphicsItem* i);

protected:
    //    virtual bool sceneEvent(QEvent* event) override;
};

}

#endif  // DAGRAPHICSITEM_H

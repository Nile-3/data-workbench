﻿#include "DAGraphicsTextItem.h"
#include <QDebug>
#include <QFont>
#include <QPainter>
#include <QTextItem>
#include <QGraphicsSceneResizeEvent>
#include "DAGraphicsStandardTextItem.h"
// 设置相对位置的锚点，如果设置了锚点，RelativePosition将无效
namespace DA
{

class DAGraphicsTextItem::PrivateData
{
    DA_DECLARE_PUBLIC(DAGraphicsTextItem)
public:
    PrivateData(DAGraphicsTextItem* p);
    // 判断锚点是否固定了
    bool isAnchorFixBoth() const;
    // 只有，没有固定任何方向
    bool isAnchorFree() const;

public:
    DAGraphicsStandardTextItem* mTextItem { nullptr };
    bool mEnableRelativePosition { false };
    qreal mRelativeX { 0.0 };
    qreal mRelativeY { 0.0 };
    ShapeKeyPoint mParentAnchorPoint { ShapeKeyPoint::BottomCenter };
    ShapeKeyPoint mItemAnchorPoint { ShapeKeyPoint::TopCenter };
};

DAGraphicsTextItem::PrivateData::PrivateData(DAGraphicsTextItem* p) : q_ptr(p)
{
    mTextItem = new DAGraphicsStandardTextItem(p);
}

bool DAGraphicsTextItem::PrivateData::isAnchorFixBoth() const
{
    return (mParentAnchorPoint != ShapeKeyPoint::None) && (mItemAnchorPoint != ShapeKeyPoint::None);
}

bool DAGraphicsTextItem::PrivateData::isAnchorFree() const
{
    return (mParentAnchorPoint == ShapeKeyPoint::None) || (mItemAnchorPoint == ShapeKeyPoint::None);
}

//----------------------------------------------------
// DAGraphicsTextItem
//----------------------------------------------------

DAGraphicsTextItem::DAGraphicsTextItem(QGraphicsItem* parent) : DAGraphicsResizeableItem(parent), DA_PIMPL_CONSTRUCT
{
    init();
}

DAGraphicsTextItem::DAGraphicsTextItem(const QFont& f, QGraphicsItem* parent)
    : DAGraphicsResizeableItem(parent), DA_PIMPL_CONSTRUCT
{
    setFont(f);
    init();
}

DAGraphicsTextItem::DAGraphicsTextItem(const QString& str, const QFont& f, QGraphicsItem* parent)
    : DAGraphicsResizeableItem(parent), DA_PIMPL_CONSTRUCT
{
    setFont(f);
    setText(str);
    init();
}

DAGraphicsTextItem::~DAGraphicsTextItem()
{
}

void DAGraphicsTextItem::init()
{
    setAcceptDrops(true);
    setAcceptHoverEvents(true);
    setFocusProxy(d_ptr->mTextItem);
    setEnableResize(false);
    setShowBorder(false);
}

void DAGraphicsTextItem::setEditMode(bool on)
{
    d_ptr->mTextItem->setEditMode(on);
}

bool DAGraphicsTextItem::saveToXml(QDomDocument* doc, QDomElement* parentElement) const
{
    DAGraphicsResizeableItem::saveToXml(doc, parentElement);
    d_ptr->mTextItem->saveToXml(doc, parentElement);
    return true;
}

bool DAGraphicsTextItem::loadFromXml(const QDomElement* itemElement)
{
    DAGraphicsResizeableItem::loadFromXml(itemElement);
    return d_ptr->mTextItem->loadFromXml(itemElement);
}

DAGraphicsStandardTextItem* DAGraphicsTextItem::textItem() const
{
    return d_ptr->mTextItem;
}

void DAGraphicsTextItem::setBodySize(const QSizeF& s)
{
    DAGraphicsResizeableItem::setBodySize(s);
    QRectF br = boundingRect();
    d_ptr->mTextItem->setPos(0, 0);
    d_ptr->mTextItem->setTextWidth(br.width());
    d_ptr->mTextItem->adjustSize();
}

/**
 * @brief 设置文本
 * @param v
 */
void DAGraphicsTextItem::setText(const QString& v)
{
    d_ptr->mTextItem->setPlainText(v);
    d_ptr->mTextItem->adjustSize();
    setBodySize(d_ptr->mTextItem->boundingRect().size());
    updateRelativePosition();
}

/**
 * @brief 文本
 * @return
 */
QString DAGraphicsTextItem::getText() const
{
    return d_ptr->mTextItem->toPlainText();
}

/**
 * @brief 设置字体
 * @param v
 */
void DAGraphicsTextItem::setFont(const QFont& v)
{
    d_ptr->mTextItem->setFont(v);
}

/**
 * @brief 获取字体
 * @return
 */
QFont DAGraphicsTextItem::getFont() const
{
    return d_ptr->mTextItem->font();
}

/**
 * @brief 设置是否开启相对定位
 * @note 相对定位需要有父级item，否则无效
 * @param on
 */
void DAGraphicsTextItem::setEnableRelativePosition(bool on)
{
    d_ptr->mEnableRelativePosition = on;
    if (on) {
        updateRelativePosition();
    }
}

/**
 * @brief 是否开启相对定位
 * @return
 */
bool DAGraphicsTextItem::isEnableRelativePosition() const
{
    return d_ptr->mEnableRelativePosition;
}

/**
 * @brief 设置相对父窗口的相对定位
 * @param xp x位置相对父级item的宽度占比，如parentCorner=Qt::TopLeftCorner,xp=0.2
 * 那么textitem的x定位设置为parentItem.boundingRect().topLeft().x() + parentItem.width() * xp
 * @param yp y位置相对父级item的宽度占比，如parentCorner=Qt::TopLeftCorner,yp=0.1
 * 那么textitem的y定位设置为parentItem.boundingRect().topLeft().y() + parentItem.width() * yp5
 */
void DAGraphicsTextItem::setRelativePosition(qreal xp, qreal yp)
{
    d_ptr->mRelativeX = xp;
    d_ptr->mRelativeY = yp;
    updateRelativePosition();
}

QPointF DAGraphicsTextItem::getRelativePosition() const
{
    return QPointF(d_ptr->mRelativeX, d_ptr->mRelativeY);
}

/**
 * @brief 设置锚点对齐,如果设置了锚点，RelativePosition将无效
 *
 * 父级item的锚点和此item的锚点将贴合
 *
 * 如果需要取消锚点对齐，设置任意一个为ShapeKeyPoint::None即可
 *
 * @param kParentAnchorPoint 父级item的锚点
 * @param thisAnchorPoint 此item的锚点
 */
void DAGraphicsTextItem::setRelativeAnchorPoint(ShapeKeyPoint kParentAnchorPoint, ShapeKeyPoint thisAnchorPoint)
{
    d_ptr->mParentAnchorPoint = kParentAnchorPoint;
    d_ptr->mItemAnchorPoint   = thisAnchorPoint;
    updateRelativePosition();
}

/**
 * @brief 父级的锚点
 */
ShapeKeyPoint DAGraphicsTextItem::getParentRelativeAnchorPoint() const
{
    return d_ptr->mParentAnchorPoint;
}

/**
 * @brief 本条目的锚点
 */
ShapeKeyPoint DAGraphicsTextItem::getItemRelativeAnchorPoint() const
{
    return d_ptr->mItemAnchorPoint;
}

void DAGraphicsTextItem::updateRelativePosition()
{
    if (!isEnableRelativePosition()) {
        return;
    }
    QGraphicsItem* pi = parentItem();
    if (nullptr == pi) {
        return;
    }

    QRectF parentRect = pi->boundingRect();
    if (DAGraphicsResizeableItem* ri = dynamic_cast< DAGraphicsResizeableItem* >(pi)) {
        parentRect = ri->getBodyRect();
    }
    QRectF itemRect = getBodyRect();

    ShapeKeyPoint parKP       = d_ptr->mParentAnchorPoint == ShapeKeyPoint::None ? ShapeKeyPoint::BottomCenter
                                                                                 : d_ptr->mParentAnchorPoint;
    ShapeKeyPoint itemKP      = d_ptr->mItemAnchorPoint == ShapeKeyPoint::None ? ShapeKeyPoint::TopCenter
                                                                               : d_ptr->mItemAnchorPoint;
    QPointF parPosition       = rectShapeKeyPoint(parentRect, parKP);
    QPointF itemPoint         = rectShapeKeyPoint(itemRect, itemKP);
    QPointF itemwillMovePoint = parPosition - itemPoint;
    itemwillMovePoint -= QPointF(parentRect.width() * d_ptr->mRelativeX, parentRect.height() * d_ptr->mRelativeY);
    qDebug() << "updateRelativePosition,parent boundingRect=" << pi->boundingRect() << ",parentRect=" << parentRect
             << ",parPosition=" << parPosition << "item boundingRect = " << boundingRect() << ",itemRect=" << itemRect
             << ",itemPoint=" << itemPoint << ",itemwillMovePoint=" << itemwillMovePoint;
    setPos(itemwillMovePoint);
}

/**
 * @brief 更新相对位置
 */
void DAGraphicsTextItem::updateRelativePosition(const QRectF& parentRect, const QRectF& itemRect)
{
    // QGraphicsItem* pi = parentItem();
    // if (nullptr == pi) {
    //     return;
    // }
    // ShapeKeyPoint parKP       = d_ptr->mParentAnchorPoint == ShapeKeyPoint::None ? ShapeKeyPoint::BottomCenter
    //                                                                              : d_ptr->mParentAnchorPoint;
    // ShapeKeyPoint thisKP      = d_ptr->mItemAnchorPoint == ShapeKeyPoint::None ? ShapeKeyPoint::TopCenter
    //                                                                            : d_ptr->mItemAnchorPoint;
    // QPointF parPosition       = rectShapeKeyPoint(parentRect, parKP);
    // QPointF itemPoint         = rectShapeKeyPoint(itemRect, thisKP);
    // QPointF itemwillMovePoint = parPosition - itemPoint;
    // // 当前计算出来的itemwillMovePoint是基于bodyRect,而实际的boundrect

    // qDebug() << "updateRelativePosition,parent parentRect=" << parentRect << ",parPosition=" << parPosition
    //          << ",itemRect=" << itemRect << ",itemPoint=" << itemPoint << ",itemwillMovePoint=" << itemwillMovePoint;
    // itemwillMovePoint -= QPointF(parentRect.width() * d_ptr->mRelativeX, parentRect.height() * d_ptr->mRelativeY);
    // qDebug() << "updateRelativePosition,origin after offset = " << itemwillMovePoint;
    // setPos(itemwillMovePoint);
}

/**
 * @brief paintBody
 * @param painter
 * @param option
 * @param widget
 * @param bodyRect
 */
void DAGraphicsTextItem::paintBody(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget, const QRectF& bodyRect)
{
    // DAGraphicsResizeableItem::paintBody(painter, option, widget, bodyRect);
    if (isShowBorder()) {
        painter->save();
        painter->setPen(getBorderPen());
        painter->drawRect(bodyRect);
        painter->restore();
    }
}

}  // end da

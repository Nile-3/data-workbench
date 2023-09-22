#ifndef DAGRAPHICSLINKITEM_H
#define DAGRAPHICSLINKITEM_H
#include "DAGraphicsViewGlobal.h"
#include <QGraphicsItem>

class QGraphicsSceneMouseEvent;
namespace DA
{

/**
 * @brief 绘制连接线的item
 */
class DAGRAPHICSVIEW_API DAGraphicsLinkItem : public QGraphicsItem
{
    DA_DECLARE_PRIVATE(DAGraphicsLinkItem)
public:
    enum AnonymousType
    {
        anonymous = DA::ItemType_GraphicsLinkItem
    };
    int type() const override
    {
        return (anonymous);
    }
    /**
     * @brief 连接点的样式
     */
    enum LinkLineStyle
    {
        LinkLineBezier,    ///< 贝塞尔曲线连接
        LinkLineStraight,  ///< 直线连接
        LinkLineKnuckle    ///< 肘形连接（直角）
    };

    /**
     * @brief 标记方向
     */
    enum Orientations
    {
        OrientationStart = 0,  ///< 连接的开始点
        OrientationEnd,        ///< 连接的结束点
        OrientationBoth        ///< 两个都包含
    };

    /**
     * @brief 端点样式
     */
    enum EndPointType
    {
        EndPointNone,       ///< 没有箭头
        EndPointTriangType  ///< 三角形箭头 ▲
    };

public:
    DAGraphicsLinkItem(QGraphicsItem* p = nullptr);
    ~DAGraphicsLinkItem();
    //设置连接点的形式
    void setEndPointType(Orientations o, EndPointType epType);
    EndPointType getEndPointType(Orientations o) const;
    //端点大小
    int getEndPointSize() const;
    void setEndPointSize(int v);
    //连线方式
    void setLinkLineStyle(LinkLineStyle s);
    LinkLineStyle getLinkLineStyle() const;

    //设置线的画笔
    void setLinePen(const QPen& p);
    QPen getLinePen() const;

    //【重要虚函数】更新范围参数
    virtual QRectF updateBoundingRect();

    //设置贝塞尔曲线的控制点的缩放比例，连线时按照控制点的方向延伸出贝塞尔曲线的控制点，延伸的控制点的长度w = length * bezierControlScale
    void setBezierControlScale(qreal rate = 0.25);
    qreal getBezierControlScale() const;

    //开始连接点的位置,位置基于scene
    void setStartScenePosition(const QPointF& p);
    QPointF getStartScenePosition() const;

    //结束连接点的位置
    void setEndScenePosition(const QPointF& p);
    QPointF getEndScenePosition() const;

    //结束节点跟随鼠标，此函数前提是from节点已经确定
    void setEndPositionFollowMouse(bool on = true);
    bool isEndPositionFollowMouse() const;
    //获取链接线
    QPainterPath getLinkLinePainterPath() const;
    //获取末端(箭头)的绘图路径
    QPainterPath getEndPointPainterPath(Orientations epType) const;
    //更新末端
    void updateEndPoint();
    //绘图
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    //绘制连接线
    virtual void paintLinkLine(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget, const QPainterPath& linkPath);
    //绘制箭头
    virtual void paintEndPoint(QPainter* painter,
                               const QStyleOptionGraphicsItem* option,
                               const QPointF& pStart,
                               EndPointType etStart,
                               const QPainterPath& startPainterPath,
                               const QPointF& pEnd,
                               EndPointType etEnd,
                               const QPainterPath& endPainterPath,
                               const QPainterPath& linkPath);

    virtual QRectF boundingRect() const override;
    virtual QPainterPath shape() const override;

    //生成painterpath
    virtual QPainterPath generateLinePainterPath(const QPointF& fromPoint, const QPointF& toPoint, LinkLineStyle linestyle = LinkLineStraight);
    //生成箭头，所有生成的箭头都是尖朝上（↑），绘制的时候需要根据情况进行旋转
    virtual QPainterPath generateEndPointPainterPath(EndPointType epType, int size);

public:
    //计算两个点的距离
    static qreal pointLength(const QPointF& a, const QPointF& b);

    //延长线，以一个方向和距离延伸
    static QPointF elongation(const QPointF& orgPoint, AspectDirection d, qreal externLen);

    //判断两个方向是否相对，也就是东对西，南对北就是相对，相对必定平行
    static bool isDirectionOpposite(AspectDirection d1, AspectDirection d2);

    //判断两个方向是否平行
    static bool isDirectionParallel(AspectDirection d1, AspectDirection d2);

    //顺着点1的方向看，点2是否在前面
    static bool isPointInFront(const QPointF& p1, AspectDirection d1, const QPointF& p2);
    //点是否会相遇
    static bool isPointCanMeet(const QPointF& p1, AspectDirection d1, const QPointF& p2, AspectDirection d2);
    //针对平行点线，沿着方向移动可以接近，此函数只对平行点线有用
    static bool isParallelPointApproachInDirection(const QPointF& p1, AspectDirection d1, const QPointF& p2, AspectDirection d2);
    //翻转方向
    static AspectDirection oppositeDirection(AspectDirection d);
    //返回点1相对点2的方位
    static AspectDirection relativeDirectionOfPoint(const QPointF& p1, const QPointF& p2);
    //获取线段的末端，这个函数可以返回末端但有不是终端的点，这个点离终端的距离不会超过distanceMaxPx
    static QPointF calcPainterPathEndPoint(const QPainterPath& path, bool fromStart = true, qreal distanceMaxPx = 18.0);

protected:
    //开始连接点的位置
    void setStartPosition(const QPointF& p);
    const QPointF& getStartPosition() const;

    //结束连接点的位置
    void setEndPosition(const QPointF& p);
    const QPointF& getEndPosition() const;
    //获取绘图的画笔
    QPen getPainterPen(const QStyleOptionGraphicsItem* option) const;
    //生成painterpath
    virtual QPainterPath generateLinkLineBezierPainterPath(const QPointF& fromPos,
                                                           AspectDirection fromDirect,
                                                           const QPointF& toPos,
                                                           AspectDirection toDirect);
    //生成直线
    virtual QPainterPath generateLinkLineStraightPainterPath(const QPointF& fromPos, const QPointF& toPos);
    //生成直角线
    virtual QPainterPath generateLinkLineKnucklePainterPath(const QPointF& fromPos,
                                                            AspectDirection fromDirect,
                                                            const QPointF& toPos,
                                                            AspectDirection toDirect);
};

// DAAbstractNodeLinkGraphicsItem::EndPointType的枚举转换
DAGRAPHICSVIEW_API QString enumToString(DA::DAGraphicsLinkItem::EndPointType e);
// DAAbstractNodeLinkGraphicsItem::EndPointType的枚举转换
DAGRAPHICSVIEW_API DA::DAGraphicsLinkItem::EndPointType stringToEnum(
    const QString& s,
    DA::DAGraphicsLinkItem::EndPointType defaultEnum = DA::DAGraphicsLinkItem::EndPointNone);
// DAAbstractNodeLinkGraphicsItem::LinkLineStyle的枚举转换
DAGRAPHICSVIEW_API QString enumToString(DA::DAGraphicsLinkItem::LinkLineStyle e);
// DAAbstractNodeLinkGraphicsItem::LinkLineStyle的枚举转换
DAGRAPHICSVIEW_API DA::DAGraphicsLinkItem::LinkLineStyle stringToEnum(
    const QString& s,
    DA::DAGraphicsLinkItem::LinkLineStyle defaultEnum = DA::DAGraphicsLinkItem::LinkLineKnuckle);

}  // end DA

#endif  // DAGRAPHICSLINKITEM_H

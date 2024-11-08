#ifndef DAABSTRACTGRAPHICSVIEWACTION_H
#define DAABSTRACTGRAPHICSVIEWACTION_H
#include "DAGraphicsViewGlobal.h"
class QPaintEvent;
class QKeyEvent;
class QMouseEvent;

namespace DA
{
class DAGraphicsView;

/**
 * @brief ���DAGraphicsView��action.
 *
 */
class DAGRAPHICSVIEW_API DAAbstractGraphicsViewAction
{
	friend class DAGraphicsView;

public:
	DAAbstractGraphicsViewAction(DAGraphicsView* v);
	virtual ~DAAbstractGraphicsViewAction();
	// ��ͼ
	DAGraphicsView* view() const;
	// ����action,��������ɾ��action��end�������κγ�Ա�����Ĳ��������������쳣
	void destroy();

protected:
	// ��ʼ�������ʹ��setAction����õĺ���
	virtual void beginActive();
	// �����������ʹ��ɾ��actionǰ���õĺ���
	virtual void endAction();
	// �����ͼ�¼�,����true������action�ٳ��˴��¼���������scene�м��������¼�,Ĭ�Ϸ���false
	virtual bool paintEvent(QPaintEvent* event);
	virtual bool keyPressEvent(QKeyEvent* event);
	virtual bool keyReleaseEvent(QKeyEvent* event);
	virtual bool mouseDoubleClickEvent(QMouseEvent* event);
	virtual bool mouseMoveEvent(QMouseEvent* event);
	virtual bool mousePressEvent(QMouseEvent* event);
	virtual bool mouseReleaseEvent(QMouseEvent* event);

protected:
	DAGraphicsView* mView { nullptr };
};
}  // end ns DA
#endif

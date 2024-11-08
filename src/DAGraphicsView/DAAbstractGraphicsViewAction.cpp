#include "DAAbstractGraphicsViewAction.h"
#include "DAAbstractGraphicsViewAction.h"
#include "DAAbstractGraphicsViewAction.h"
#include "DAAbstractGraphicsViewAction.h"
#include "DAAbstractGraphicsViewAction.h"
#include "DAAbstractGraphicsViewAction.h"
#include <QPaintEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include "DAGraphicsView.h"

namespace DA{

DAAbstractGraphicsViewAction::DAAbstractGraphicsViewAction(DAGraphicsView * v):mView(v)
{
}

DAAbstractGraphicsViewAction::~DAAbstractGraphicsViewAction()
{
}

DAGraphicsView * DAAbstractGraphicsViewAction::view() const
{
    return mView;
}

void DAAbstractGraphicsViewAction::destroy()
{
    auto v = view();
	if (v) {
		v->clearViewAction();
	}
}

void DAAbstractGraphicsViewAction::beginActive()
{
}

void DAAbstractGraphicsViewAction::endAction()
{
}

/**
 * @brief �����ͼ�¼�.
 *  
 * @param ��ͼ�¼�
 * @return ����true������action�ٳ��˴��¼���������scene�м��������¼�,Ĭ�Ϸ���false
 * @note ��������£��˺�������Ӧ�÷���true
 */
bool DAAbstractGraphicsViewAction::paintEvent(QPaintEvent * event)
{
    return false;
}

bool DAAbstractGraphicsViewAction::keyPressEvent(QKeyEvent * event)
{
    return false;
}

bool DAAbstractGraphicsViewAction::keyReleaseEvent(QKeyEvent * event)
{
    return false;
}

bool DAAbstractGraphicsViewAction::mouseDoubleClickEvent(QMouseEvent * event)
{
    return false;
}

bool DAAbstractGraphicsViewAction::mouseMoveEvent(QMouseEvent * event)
{
    return false;
}

bool DAAbstractGraphicsViewAction::mousePressEvent(QMouseEvent * event)
{
    return false;
}

bool DAAbstractGraphicsViewAction::mouseReleaseEvent(QMouseEvent * event)
{
    return false;
}

}
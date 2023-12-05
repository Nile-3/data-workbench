﻿#include "DAAppCommand.h"
#ifdef DA_ENABLE_PYTHON
#include "Commands/DACommandWithTemplateData.h"
#endif
//===================================================
// using DA namespace -- 禁止在头文件using！！
//===================================================

using namespace DA;

//===================================================
// DAAppCommand
//===================================================
DAAppCommand::DAAppCommand(DAUIInterface* u) : DACommandInterface(u)
{
    //创建临时数据目录
#ifdef DA_ENABLE_PYTHON
    DACommandWithTemplateData::ensureTemplateDirExists();
#endif
}

DAAppCommand::~DAAppCommand()
{
}

void DAAppCommand::setDataManagerStack(QUndoStack* s)
{
    mDataManagerStack = s;
    undoGroup().addStack(s);
}

QUndoStack* DAAppCommand::getDataManagerStack() const
{
    return mDataManagerStack.data();
}

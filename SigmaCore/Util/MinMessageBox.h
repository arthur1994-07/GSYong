#ifndef _MIN_MESSAGE_BOX_H_
#define _MIN_MESSAGE_BOX_H_

#pragma once

namespace sc
{
    //! Tool 일 경우에 Message Box 를 띄우지 않는다. 2011-09-21 jgkim
    void ErrorMessageBox(std::string& Message, bool bToolMode);
}
#endif
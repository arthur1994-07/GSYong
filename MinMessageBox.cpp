#include "stdafx.h"
#include "../Log/LogMan.h"
#include "MinMessageBox.h"

#include "../DebugInclude.h"

namespace sc
{
    /*
    ---------------------------------------------------------------------------
    Message_Box Types
    ---------------------------------------------------------------------------
    MB_ABORTRETRYIGNORE   The message box contains three pushbuttons: Abort, Retry, and Ignore.
    MB_OK   The message box contains one pushbutton: OK.
    MB_OKCANCEL   The message box contains two pushbuttons: OK and Cancel.
    MB_RETRYCANCEL   The message box contains two pushbuttons: Retry and Cancel.
    MB_YESNO   The message box contains two pushbuttons: Yes and No.
    MB_YESNOCANCEL   The message box contains three pushbuttons: Yes, No, and Cancel.

    ---------------------------------------------------------------------------
    Message-Box Modality
    ---------------------------------------------------------------------------
    MB_APPLMODAL   The user must respond to the message box before continuing work in the current window. However, the user can move to the windows of other applications and work in those windows. The default is MB_APPLMODAL if neither MB_SYSTEMMODAL nor MB_TASKMODAL is specified.
    MB_SYSTEMMODAL   All applications are suspended until the user responds to the message box. System-modal message boxes are used to notify the user of serious, potentially damaging errors that require immediate attention and should be used sparingly.
    MB_TASKMODAL   Similar to MB_APPLMODAL, but not useful within a Microsoft Foundation class application. This flag is reserved for a calling application or library that does not have a window handle available.

    ---------------------------------------------------------------------------
    Message-Box Icons
    ---------------------------------------------------------------------------
    MB_ICONEXCLAMATION   An exclamation-point icon appears in the message box.
    MB_ICONINFORMATION   An icon consisting of an "I" in a circle appears in the message box.
    MB_ICONQUESTION   A question-mark icon appears in the message box.
    MB_ICONSTOP   A stop-sign icon appears in the message box.

    ---------------------------------------------------------------------------
    Message-Box Default Buttons
    ---------------------------------------------------------------------------
    MB_DEFBUTTON1   The first button is the default. Note that the first button is always the default unless MB_DEFBUTTON2 or MB_DEFBUTTON3 is specified.
    MB_DEFBUTTON2   The second button is the default.
    MB_DEFBUTTON3   The third button is the default.
    */
    void ErrorMessageBox(std::string& Message, bool bToolMode)
    {
        sc::writeLogError(Message);
        if (!bToolMode)
            AfxMessageBox(Message.c_str(), MB_OK | MB_ICONEXCLAMATION);
    }
}
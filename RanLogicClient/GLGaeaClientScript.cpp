#include "pch.h"
#include "../EngineLib/GUInterface/UITextControl.h"
#include "./Script/ScriptManager.h"
#include "./GLGaeaClient.h"


#include "../SigmaCore/DebugInclude.h"

void GLGaeaClient::ScriptStop()
{
}

void GLGaeaClient::ScriptResume()
{
}

void GLGaeaClient::ScriptDestroy()
{
    if (m_pClientScript)
    {
        m_pClientScript->UnlinkScript();
        PrintMsgTextDlg(NS_UITEXTCOLOR::DEFAULT, "Macro end");
    }
}

void GLGaeaClient::ScriptFrameMove(float fTime, float ElapsedTime)
{
    if (m_pClientScript)
        m_pClientScript->Update(ElapsedTime);
}

void GLGaeaClient::ScriptExec(const std::wstring& FileName)
{
    std::string ScriptName = sc::string::unicodeToAnsi(FileName);
    ScriptExec(ScriptName);
}

void GLGaeaClient::ScriptExec(const std::string& FileName)
{
    if (m_pClientScript)
        m_pClientScript->Run(FileName);
}
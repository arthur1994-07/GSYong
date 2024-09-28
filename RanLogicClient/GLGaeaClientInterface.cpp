#include "pch.h"
#include "./GLGaeaClient.h"

#include "../SigmaCore/DebugInclude.h"

#include "./Widget/GLWidgetScript.h"

void GLGaeaClient::PrintMsgText(D3DCOLOR dwColor, const std::string& strText)
{
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PrintSystemMsg,
		"-s, -dw", strText.c_str(), dwColor );
}

void GLGaeaClient::PrintConsoleText(const std::string& strText)
{
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PrintSystemMsg,
		"-s, -dw", strText.c_str(), NS_UITEXTCOLOR::LIGHTSKYBLUE );
}

void GLGaeaClient::PrintMsgTextDlg(D3DCOLOR dwColor, const std::string& strText )
{
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PrintSystemMsgBox,
		"-s, -dw", strText.c_str(), dwColor );
}

void GLGaeaClient::PrintConsoleTextDlg(const std::string& strText)
{
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PrintSystemMsgBox,
		"-s, -dw", strText.c_str(), NS_UITEXTCOLOR::LIGHTSKYBLUE );
}

void GLGaeaClient::PrintMsgDlg(D3DCOLOR dwColor, const std::string& strText)
{
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PrintSystemBox,
		"-s, -dw", strText.c_str(), dwColor );
}

void GLGaeaClient::PrintAddString(D3DCOLOR dwColor, const std::string& strText)
{
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_AddString,
		"-s, -dw", strText.c_str(), dwColor );
}

void GLGaeaClient::PrintMessageBox(DWORD dwType, const std::string& strText)
{
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_OpenMsgBox,
		"-dw, -s", dwType, strText.c_str() );
}

void GLGaeaClient::DoModalOutInterface(
    const std::string& strText,
    int nModalTitle,
    int nModalType,
    UIGUID CallerID,
    BOOL bHide)
{
    if (m_pGlobalStage)
        m_pGlobalStage->DoModalOutInterface(strText, nModalTitle, nModalType, CallerID, bHide);
}

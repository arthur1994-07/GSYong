#include "../pch.h"
#include "../GLGaeaClient.h"
#include "./GLCharacter.h"

#include "../../SigmaCore/DebugInclude.h"

void GLCharacter::PrintMsgText(D3DCOLOR dwColor, const std::string& strText)
{
    m_pGaeaClient->PrintMsgText(dwColor, strText);
}

void GLCharacter::PrintConsoleText(const std::string& strText)
{
    m_pGaeaClient->PrintConsoleText(strText);
}

void GLCharacter::PrintMsgTextDlg(D3DCOLOR dwColor, const std::string& strText )
{
    m_pGaeaClient->PrintMsgTextDlg(dwColor, strText);
}

void GLCharacter::PrintConsoleTextDlg(const std::string& strText)
{
    m_pGaeaClient->PrintConsoleTextDlg(strText);
}

void GLCharacter::PrintMsgDlg(D3DCOLOR dwColor, const std::string& strText)
{
    m_pGaeaClient->PrintMsgDlg(dwColor, strText);
}
#include "pch.h"
#include "../SigmaCore/MsgPack/MinMsgPack.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Json/MinJson.h"
#include "../SigmaCore/Util/DateTime.h"

#include "../EngineLib/DxTools/DxClubMan.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/GUInterface/UITextControl.h"

#include "../MfcExLib/RanFilter.h"

#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/Msg/ClubMsg.h"
#include "../RanLogic/Msg/JsonMsgDefine.h"
#include "../RanLogic/Club/ClubLogManClient.h"

#include "./Club/GLClubClient.h"
#include "./Land/GLLandManClient.h"
#include "./Stage/DxGlobalStage.h"
#include "./GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


bool GLGaeaClient::SetClubGradeName(size_t Index, const std::string& GradeName)
{
    if (Index >= GetClubGradeSize())
        return false;
    if (!m_pMyClub->IsValid())
        return false;
    if (!m_pMyClub->HaveAuthChangeAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }

    if (m_pMyClub->HaveSameGradeName(GradeName))
    {
        // 이미 같은 이름의 계급 이름이 있음
        return false;
    }

    // Agent 에 계급 이름 변경 요청    
    NET_CLUB_GRADE_NAME_CHANGE_CA SendData;
    SendData.m_Index = Index;
    SendData.m_GradeName = GradeName;
    
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);
    
    NETSENDTOAGENT(NET_MSG_CLUB_GRADE_NAME_CHANGE_CA, SendBuffer);
    return true;
}

void GLGaeaClient::ReqChangeGradeName(int _index, const char* _GradeName)
{
	if (_index >= (int)GetClubGradeSize())
		return;
	if (!m_pMyClub->IsValid())
		return;
	if (!m_pMyClub->HaveAuthChangeAuthority(m_pCharacter->CharDbNum()))
	{
		PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
		return;
	}
	// Agent 에 계급 이름 변경 요청    
	NET_CLUB_GRADE_NAME_CHANGE_CA SendData;
	SendData.m_Index = _index;
	SendData.m_GradeName = _GradeName;

	msgpack::sbuffer SendBuffer;
	msgpack::pack(SendBuffer, SendData);

	NETSENDTOAGENT(NET_MSG_CLUB_GRADE_NAME_CHANGE_CA, SendBuffer);	
}

//! 클럽 계급 이름 변경 요청 결과 Agent->Client
void GLGaeaClient::MsgClubGradeNameChangeAC_Result(NET_MSG_GENERIC* pMsg)
{
    // NET_MSG_CLUB_GRADE_NAME_CHANGE_RESULT_AC
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_CLUB_GRADE_NAME_CHANGE_RESULT_AC RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;  
    
    if (RecvData.m_Result)
    {
        // 계급 이름 변경에 성공하였습니다.        
        PrintMsgTextDlg(NS_UITEXTCOLOR::DEFAULT, ID2GAMEINTEXT("CLUB_GRADE_NAME_CHANGE_SUCCESS"));
    }
    else
    {        
        // 계급 이름 변경에 실패하였습니다.
        PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CLUB_GRADE_NAME_CHANGE_FAIL"));
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_ChangeGradeName);		
    }
}

void GLGaeaClient::MsgClubGradeNameChangeAC(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_CLUB_GRADE_NAME_CHANGE_AC RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    std::string OldGradeName = m_pMyClub->GetGradeName(RecvData.m_Index);
    m_pMyClub->ChangeGradeName(RecvData.m_Index, RecvData.m_GradeName, 0);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_ChangeGradeName);

    // 계급 이름이 %1% 에서 %2% 로 변경되었습니다.
    PrintMsgTextDlg(
        NS_UITEXTCOLOR::DEFAULT,
        sc::string::format(
        ID2GAMEINTEXT("CLUB_GRADE_NAME_CHANGE"),
        OldGradeName,
        RecvData.m_GradeName));
}

size_t GLGaeaClient::GetClubGradeSize() const
{
    return GLogicData::GetInstance().GetClubGradeSize();
}

DWORD GLGaeaClient::ClubGradeFlagGet(size_t Index) const
{
    return m_pMyClub->GetGradeFlag(Index);
}

bool GLGaeaClient::ClubGradeFlagChangeCA(const std::vector<club::GRADE_FLAG>& Data)
{
    if (!m_pMyClub->IsValid())
        return false;

    if (!m_pMyClub->HaveAuthChangeAuthority(m_pCharacter->CharDbNum()))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }  

    NET_CLUB_GRADE_FLAG_CHANGE_CA SendData;
    SendData.m_Data = Data;

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);

    NETSENDTOAGENT(NET_MSG_CLUB_GRADE_FLAG_CHANGE_CA, SendBuffer);

    return true;
}

void GLGaeaClient::MsgClubGradeFlagChangeAC(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_CLUB_GRADE_FLAG_CHANGE_AC RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    for (size_t i=0; i<RecvData.m_Data.size(); ++i)
    {
        club::GRADE_FLAG FlagData = RecvData.m_Data[i];
        m_pMyClub->SetGradeFlag(FlagData.m_Index, FlagData.m_Flag);
    }

    // %1% 님이 클럽 권한을 변경하였습니다.
    PrintMsgTextDlg(
        NS_UITEXTCOLOR::DEFAULT,
        sc::string::format(
            ID2GAMEINTEXT("CLUB_GRADE_FLAG_CHANGE_AC"),
            RecvData.m_ChaName));

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Club_ChangeGradeAuth);
}

size_t GLGaeaClient::GetClubAuthDescSize() const
{
    return GLogicData::GetInstance().GetClubAuthDescSize();
}

std::string GLGaeaClient::GetClubAuthDesc(size_t Index) const
{
    return GLogicData::GetInstance().GetClubAuthDesc(Index);
}

void GLGaeaClient::MsgClubAuthFC(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_CLUB_AUTH_FC* pPacket = (GLMSG::NET_CLUB_AUTH_FC*) pMsg;
    club::AUTH_GRADE Grade(pPacket->m_AuthName, pPacket->m_AuthFlag);
    m_pMyClub->AddAuth(pPacket->m_Index, Grade);
}

std::string GLGaeaClient::GetClubGradeName(size_t Index) const
{
    return m_pMyClub->GetGradeName(Index);
}

DWORD GLGaeaClient::GetClubMasterDBNum(void) const
{
	if (!m_pMyClub || !m_pMyClub->IsValid())
		return false;

	return m_pMyClub->MasterCharDbNum();
}

std::string GLGaeaClient::GetClubMasterName()
{
	if( NULL == m_pMyClub || false == m_pMyClub->IsValid() )
		return "";

	return m_pMyClub->GetMasterName();
}

bool GLGaeaClient::SetClubMemberGrade(DWORD MemberDbNum, size_t Index)
{    
    if (!m_pMyClub->IsValid())
        return false;

    if (!m_pMyClub->IsMember(MemberDbNum))
        return false;

    if (m_pMyClub->IsMaster(MemberDbNum))
        return false;

    DWORD MyChaDbNum = m_pCharacter->CharDbNum();    
    // 권한조사
    if (!m_pMyClub->HaveAuthChangeAuthority(MyChaDbNum))
    {
        PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CLUB_NOT_HAVE_AUTHRITY"));
        return false;
    }

    NET_CLUB_MEMBER_GRADE_CHANGE_CA SendData;
    SendData.m_MemberDbNum = MemberDbNum;
    SendData.m_Index       = Index;

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);

    NETSENDTOAGENT(NET_MSG_CLUB_MEMBER_GRADE_CHANGE_CA, SendBuffer);
    return true;
}

void GLGaeaClient::MsgClubMemberGradeChangeAC2(NET_MSG_GENERIC* pMsg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_CLUB_MEMBER_GRADE_CHANGE_AC2 RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;
    
    if (m_pMyClub->ChangeMemberGrade(RecvData.m_MemberDbNum, RecvData.m_GradeIndex))
    {        
        GLCLUBMEMBER* pMember = m_pMyClub->GetMember(RecvData.m_MemberDbNum);
        if (pMember)
        {
            pMember->GetName();
            m_pMyClub->GetGradeName(RecvData.m_GradeIndex);
            std::string Msg(
                sc::string::format(
                ID2GAMEINTEXT("CLUB_MEMBER_GRADE_CHANGE"),
                pMember->GetName(),
                m_pMyClub->GetGradeName(RecvData.m_GradeIndex)));
            PrintMsgText(NS_UITEXTCOLOR::DEFAULT, Msg);
        }

        // UI Refresh
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Club_UpdateMember );
    }
}
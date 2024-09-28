#include "pch.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Util/SystemInfo.h"

#include "../../EngineLib/G-Logic/GLPeriod.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GlobalParam.h"
#include "../../EngineLib/Common/profile.h"
#include "../../EngineLib/DxTools/Occlusion/NSOcclusionCulling.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
#include "../../RanLogic/Skill/GLSkill.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Event/GLIPEvent.h"
#include "../../RanLogic/Tournament/GLTournamentClientInfo.h"
#include "../../RanLogic/Msg/PVEMsg.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h" 
#include "../../RanLogic/Msg/GLContrlMatching.h"
#include "../../RanLogic/Msg/GLContrlServerMsg.h"
#include "../../RanLogic/Msg/GLContrlPingMsg.h"
#include "../../RanLogic/Msg/GLContrlEventMsg.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../../RanUI/OldUI/Interface/ChatType.h"

#include "../MiniGame/GLMiniGameManager.h"
#include "../Stage/DxGlobalStage.h"
#include "../Friend/GLFriendClient.h"
#include "../Land/GLLandManClient.h"
#include "../Emoticon/Emoticon.h"
#include "../ReferChar/ReferChar.h"
#include "../TexasHoldem/GLTexasHoldemClient.h"
#include "../GLGaeaClient.h"
#include "./dxincommand.h"
#include "../TriggerSystem/ClientTriggerSystem.h"
#include "../PVE/GLPVEClient.h"
#include "../Tournament/GLTournamentClient.h"
#include "../../RanLogic/FlyCamera/FlyCameraDefine.h"

#include "../Post//PostRnClient.h"

// Lotto System
#include "../LottoSystem/GLLottoSystemManClient.h"

#include "../Chat/GLChatMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#include "../GfxInterfaceBase.h"

extern BOOL g_bWIREFRAME_VIEW;

namespace dxincommand
{
#if defined(NDEBUG)
    bool bDISP_FPS = false;
    bool bDISP_CONSOLE = false;
#else
    bool bDISP_FPS = false;
    bool bDISP_CONSOLE = false;
#endif
	bool bDISP_POSITION = false;
    bool bDISP_PINGTRACE = false;
}

bool GameCommand::Dsplay(splitVector& SplitVec)
{
    if (SplitVec.size() != 2)
        return true;
    std::string strParam1 = SplitVec[1];

    if (strParam1=="off")
    {
        dxincommand::bDISP_CONSOLE = false;
        dxincommand::bDISP_FPS = false;
        m_pGaeaClient->PrintConsoleText(
            sc::string::format("%s  : %s", "console, fps", BOOL2STR(false)));
    }
    if (strParam1=="console")
    {
        dxincommand::bDISP_CONSOLE = !dxincommand::bDISP_CONSOLE;
        m_pGaeaClient->PrintConsoleText(
            sc::string::format("%s  : %s", strParam1, BOOL2STR(dxincommand::bDISP_CONSOLE)));
    }
    else if ( strParam1=="fps" )
    {
        dxincommand::bDISP_FPS = !dxincommand::bDISP_FPS;
        m_pGaeaClient->PrintConsoleText(
            sc::string::format("%s  : %s", strParam1, BOOL2STR(dxincommand::bDISP_FPS)));
    }
    else if ( strParam1=="gate" )
    {
        GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
        if (!pLand)
            return false;

        DxLandGateMan* pGateMan = pLand->GetLandGateMan();
        if (!pGateMan)
            return false;

        PDXLANDGATE pLandGate = pGateMan->GetListLandGate();
        while (pLandGate)
        {
            CString strGATE;
            strGATE.Format ( "%d ", pLandGate->GetGateID() );

            strGATE += "[";
            if (pLandGate->GetFlags() & DxLandGate::GATE_OUT)
                strGATE += "out";
            if ((pLandGate->GetFlags()&DxLandGate::GATE_OUT) && (pLandGate->GetFlags()&DxLandGate::GATE_IN))
                strGATE += "/";
            if ( pLandGate->GetFlags() & DxLandGate::GATE_IN )	strGATE += "in";
            strGATE += "]";

            if ( pLandGate->GetFlags() & DxLandGate::GATE_OUT )
            {
                strGATE += "=>";
                const char *szMAPNAME = m_pGaeaClient->GetMapName ( pLandGate->GetToMapID() );
                if ( szMAPNAME )	strGATE += szMAPNAME;
            }

            m_pGaeaClient->PrintConsoleText(strGATE.GetString());

            pLandGate = pLandGate->m_pNext;
        };
    }
    else if ( strParam1=="player" )
    {
        GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
        if (!pLand)
			return false;
        pLand->PrintChar();
    }
    else if ( strParam1=="allplayer" )
    {
        GLMSG::SNET_GM_VIEWALLPLAYER NetMsg;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    }
    else if ( strParam1=="event" )
    {
        GLMSG::SNET_GM_VIEWWORKEVENT NetMsg;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    }
	else if( strParam1 == "pos" )
	{
		dxincommand::bDISP_POSITION = !dxincommand::bDISP_POSITION;
	}
    else if (strParam1 == "chacache")
    {
        m_pGaeaClient->DisplayChaChche();
    }

	return true;
}

bool GameCommand::Render(splitVector& SplitVec)
{
    switch( SplitVec.size() )
	{
	case 1:
		{
			m_pGaeaClient->PrintConsoleText( _T( "/render wireframe" ) );
			m_pGaeaClient->PrintConsoleText( _T( "/render occ" ) );
			m_pGaeaClient->PrintConsoleText( _T( "/render state" ) );
			m_pGaeaClient->PrintConsoleText( _T( "/render on" ) );
			m_pGaeaClient->PrintConsoleText( _T( "/render off" ) );
			m_pGaeaClient->PrintConsoleText( _T( "/render sel n" ) );
			m_pGaeaClient->PrintConsoleText( _T( "/render 1on n" ) );
			m_pGaeaClient->PrintConsoleText( _T( "/render 1off n" ) );
		}
		break;

	case 2:
		{
			std::string strCommend = SplitVec[ 1 ];

			if( strCommend == _T( "state" ) )
			{
                BOOL bState = !GLOBAL_PARAM::IsStateOnOff();
				GLOBAL_PARAM::StateOnOff( bState );
			}
			else if( strCommend == _T( "on" ) )
			{
				GLOBAL_PARAM::Init();
			}
			else if( strCommend == _T( "off" ) )
			{
				GLOBAL_PARAM::Init( 0 );
			}
			else if( strCommend == _T( "wireframe" ) )
			{
				g_bWIREFRAME_VIEW = !g_bWIREFRAME_VIEW;
			}
			else if( strCommend == _T( "occ" ) )
			{
				NSOcclusionCulling::g_bRendering = !NSOcclusionCulling::g_bRendering;
			}
		}
		break;

	case 3:
		{
			std::string strCommend = SplitVec[ 1 ];
			INT iIndex = 0;

			try
			{
				iIndex = boost::lexical_cast< INT >( SplitVec[ 2 ] );

				if( strCommend == _T( "sel" ) )
				{
					if( GLOBAL_PARAM::IsValueIndex( iIndex ) )
						GLOBAL_PARAM::DisableIndex( iIndex );
					else
						GLOBAL_PARAM::EnableIndex( iIndex );
				}
				else if( strCommend == _T( "1on" ) )
				{
					GLOBAL_PARAM::Init( 0 );
					GLOBAL_PARAM::EnableIndex( iIndex );
				}
				else if( strCommend == _T( "1off" ) )
				{
					GLOBAL_PARAM::Init();
					GLOBAL_PARAM::DisableIndex( iIndex );
				}
			}
			catch ( boost::bad_lexical_cast& e )
			{
				m_pGaeaClient->PrintConsoleText( e.what() );
			}
		}
		break;
	}

	return true;
}

bool GameCommand::ShopInfo(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 3 &&  SplitVec.size() != 1)
            return true;

        SNATIVEID mapID = m_pGaeaClient->GetActiveMapID().getGaeaMapID();

        if( mapID.wMainID != 22 || mapID.wSubID != 0 )
        {
            m_pGaeaClient->PrintConsoleText ( "Can't use the command in this map." );
            return true;
        }

        GLMSG::SNET_GM_SHOP_INFO_REQ NetMsg;

        if (SplitVec.size() == 3)
        {
            std::string strPARAM_01 = SplitVec[1];
            std::string strPARAM_02 = SplitVec[2];

            NetMsg.bBasicInfo = FALSE;

            NetMsg.sSearchItemID.wMainID = boost::lexical_cast<WORD> (strPARAM_01);
            NetMsg.sSearchItemID.wSubID  = boost::lexical_cast<WORD> (strPARAM_02);
            m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
        }
        else
        {
            m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
        }
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::Ping(splitVector& SplitVec)
{
	const unsigned int _sizeToken(SplitVec.size());

	switch ( _sizeToken )
	{
	case 1:
		{
			GLMSG::NET_PING_REQUEST_INFORMATION_DETAIL NetMsg;
			m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
		}
		break;
	case 2:
		{
			GLMSG::SNET_GM_PING NetMsg;

			NetMsg.dwGaeaID = m_pGaeaClient->GETMYGAEAID();
			NetMsg.dwSent = GetTickCount();

			std::string strPARAM_01 = SplitVec[1];

			if ( strPARAM_01=="agent" )
			{
				NetMsg.bServerType = 0;
				m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
			}
			else if ( strPARAM_01=="field" )
			{		
				NetMsg.bServerType = 1;
				m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
			}
		}
		break;
	}

    return true;
}

bool GameCommand::PingTrace(splitVector& SplitVec) 
{
	CString strText;

    if ( SplitVec.size() != 2 && SplitVec.size() != 1 )
        return true;

    if ( SplitVec.size() == 1 )
    {
        m_pGaeaClient->PrintConsoleText(
            sc::string::format("Currently PingTrace is turned %s.", dxincommand::bDISP_PINGTRACE ? "ON" : "OFF"));
    }
    else 
    {
        std::string strPARAM_01 = SplitVec[1];

        if ( strPARAM_01=="on" )
        {
            if( dxincommand::bDISP_PINGTRACE == false )
            {
                dxincommand::bDISP_PINGTRACE = true;

                GLMSG::SNET_GM_PINGTRACE_ON NetMsg;

                m_pGaeaClient->NETSENDTOAGENT(&NetMsg);	//To AgentServer
            }
            else
            {
                m_pGaeaClient->PrintConsoleText("PingTrace is already turned ON.");
            }
        }
        else if ( strPARAM_01=="off" )
        {
            if( dxincommand::bDISP_PINGTRACE == true)
            {
                dxincommand::bDISP_PINGTRACE = false;

                GLMSG::SNET_GM_PINGTRACE_OFF NetMsg;

                m_pGaeaClient->NETSENDTOAGENT(&NetMsg);	//To AgentServer
            }
            else
            {
                m_pGaeaClient->PrintConsoleText("PingTrace is already turned OFF.");
            }
        }
        else
            return true;
    }
    return true;    
}

bool GameCommand::AttendanceMaintenance(splitVector& SplitVec) 
{
	CString strText;

    if ( SplitVec.size() != 1 )
        return false;

    if ( SplitVec.size() == 1 )
    {
		GLMSG::SNET_ATTENDANCE_MAINTENANCE_MANUAL_START_CA msg;

        m_pGaeaClient->NETSENDTOAGENT(&msg);	//To AgentS

        m_pGaeaClient->PrintConsoleText(std::string("Requested manually AttendanceMaintenance."));
    }
    
    return true;
}

bool GameCommand::CTF(splitVector& SplitVec) 
{
	CString strText;

    if ( SplitVec.size() != 2 && SplitVec.size() != 1 )
        return true;

    if ( SplitVec.size() == 1 )
    {
		m_pGaeaClient->PrintConsoleText(std::string("Usage: /CTF turn or /CTF resetbuff"));	//set next state of current CTF state

		return true;
    }
    else 
    {
        std::string strPARAM_01 = SplitVec[1];			

        if (strPARAM_01=="turn")
        {
			GLMSG::SNET_CTF_MOVESTATE_BY_GM_CA msg;			
			m_pGaeaClient->NETSENDTOAGENT(&msg);	//To AgentServer

			m_pGaeaClient->PrintConsoleText("Requested changing CTF state.");
        }
		else if ( "resetbuff" == strPARAM_01 )
		{
			// GM이 있는 맵의 모든 캐릭터들의 점령전 버프 삭제
			GLMSG::SNET_GM_RELEASE_CTF_BUFF NetMsg;
			m_pGaeaClient->NETSENDTOFIELD( &NetMsg );
			m_pGaeaClient->PrintConsoleText( "Requested reset CTF buff." );
		}
        else
        {
			{
                m_pGaeaClient->PrintConsoleText("Wrong use of /CTF. Try just /CTF to check the usage.");
            }

			return false;
        }        
    }
    return true;
}

bool GameCommand::MatchTest(splitVector& SplitVec) 
{
	CString strText;

    if ( SplitVec.size() != 2 && SplitVec.size() != 1 )
        return true;

    if ( SplitVec.size() == 1 )
    {
		m_pGaeaClient->PrintConsoleText(std::string("Usage: /match me, /match release or /match party"));	//set next state of current CTF state

		return true;
    }
    else 
    {
        std::string strPARAM_01 = SplitVec[1];

        if (strPARAM_01=="me")
        {
			GLMSG::NET_MATCH_REQUEST_CA msg;
			msg.dwMapID = SNATIVEID(91, 0).dwID;
			msg.bParty = FALSE;
			m_pGaeaClient->NETSENDTOAGENT(&msg);

			m_pGaeaClient->PrintConsoleText("Requested match test for me.");
        }
		else if (strPARAM_01=="cancel")	//매칭 중일 때 cancel 요청
		{
			GLMSG::NET_MATCH_CANCEL_CAM msg;
			m_pGaeaClient->NETSENDTOAGENT(&msg);

			m_pGaeaClient->PrintConsoleText("Requested match cancel.");
		}
		else if (strPARAM_01=="party")
        {
			GLMSG::NET_MATCH_REQUEST_CA msg;
			msg.bParty = TRUE;
			msg.dwMapID = SNATIVEID(91, 0).dwID;
			m_pGaeaClient->NETSENDTOAGENT(&msg);

			m_pGaeaClient->PrintConsoleText("Requested match test for party.");
        }
		else if (strPARAM_01=="release")	// 인던 플레이 중에 퇴장을 요청
        {
			GLMSG::NET_MATCH_REQUEST_CA msg;
			msg.bUnmatch = TRUE;
			msg.bParty = TRUE;
			msg.dwMapID = SNATIVEID(6, 0).dwID;	//테스트용: 돌아올 MapID
			m_pGaeaClient->NETSENDTOAGENT(&msg);

			m_pGaeaClient->PrintConsoleText("Requested match test for release.");
        }
        else
        {
			m_pGaeaClient->PrintConsoleText("Wrong use of /match. Try just /match to check the usage.");

			return false;
        }        
    }
    return true;
}

bool GameCommand::PostTake(splitVector &SplitVec) 
{
	CString strText;

    if ( SplitVec.size() != 2 && SplitVec.size() != 1 )
        return true;

    if ( SplitVec.size() == 1 )
    {
		m_pGaeaClient->PrintConsoleText(std::string("Usage: /posttake PostID"));	//set next state of current CTF state

		return true;
    }
    else 
    {
        std::string strPARAM_01 = SplitVec[1];

		LONGLONG llPostID = boost::lexical_cast<LONGLONG>(strPARAM_01);

		GLMSG::SNET_POST_OPEN_TAKE msg;
		msg.llPostID		= llPostID;
		msg.iRecver			= (int)m_pGaeaClient->GetCharacter()->GetCharID();
		msg.byAttach01Accept= (BYTE)TRUE;
		msg.byMoneyAccept	= (BYTE)TRUE;

		for (int i=0; i<10; i++)
		{
			m_pGaeaClient->NETSENDTOAGENT(&msg);	//To AgentServer
		}
    }

    return true;    
}

bool GameCommand::FpsLimit(splitVector& SplitVec)
{
    /*
    if (SplitVec.size() != 2)
        return true;
    std::string strPARAM_01 = SplitVec[1];

    if ( strPARAM_01=="on" )
    {
        g_bFRAME_LIMIT = TRUE;
        m_pGaeaClient->PrintConsoleText("fps_limit on");
    }
    else if ( strPARAM_01=="off" )
    {
        g_bFRAME_LIMIT = FALSE;
        m_pGaeaClient->PrintConsoleText("fps_limit off");
    }
    */
    return true;
}

bool GameCommand::Profile(splitVector& SplitVec)
{
    if (SplitVec.size() != 2)
        return true;
    std::string strPARAM_01 = SplitVec[1];

    if ( strPARAM_01=="on" )
    {
        g_bProfile = TRUE;                
        m_pGaeaClient->PrintConsoleText(sc::string::format("%s  : %s", "profile", "on"));
    }
    else if ( strPARAM_01=="off" )
    {
        g_bProfile = FALSE;				
        m_pGaeaClient->PrintConsoleText(sc::string::format("%s  : %s", "profile", "off"));
    }
    return true;
}

bool GameCommand::ChatLog(splitVector& SplitVec)
{
    const	int	NORMAL		= 0x0001;	//	일반
    const	int	PRIVATE		= 0x0002;	//	귓속말
    const	int	PARTY		= 0x0004;	//	파티
    const	int	GUILD		= 0x0008;	//	클럽
    const	int	TOALL		= 0x0010;	//	확성기
    const	int ALLIANCE	= 0x0020;	//	동맹
    const	int	SYSTEM		= 0x0100;	//	시스템
    const	int	ALL			= NORMAL | PRIVATE | PARTY | GUILD | TOALL | ALLIANCE | SYSTEM;


    if (SplitVec.size() != 3)
        return true;
    std::string strPARAM_01 = SplitVec[1];
    std::string strPARAM_02 = SplitVec[2];

    int nChatLogType = 0;

    if ( strPARAM_02 == "normal" )			nChatLogType = NORMAL;
    else if ( strPARAM_02 == "private" )	nChatLogType = PRIVATE;
    else if ( strPARAM_02 == "party" )		nChatLogType = PARTY;
    else if ( strPARAM_02 == "guild" )		nChatLogType = GUILD;
    else if ( strPARAM_02 == "toall" )		nChatLogType = TOALL;
    else if ( strPARAM_02 == "alliance" )	nChatLogType = ALLIANCE;
    else if ( strPARAM_02 == "system" )		nChatLogType = SYSTEM;
    else if ( strPARAM_02 == "all" )		nChatLogType = ALL;
    else	return true;

    if ( strPARAM_01=="on" )
    {
		GLChatMan::GetInstance().ChatLog( true, (DWORD) nChatLogType );
        m_pGaeaClient->PrintConsoleText(sc::string::format("%s  : %s : %s", "chatlog", "on", strPARAM_02));
    }
    else if ( strPARAM_01=="off" )
    {
		m_pGaeaClient->PrintConsoleText(sc::string::format("%s  : %s : %s", "chatlog", "off", strPARAM_02));
		GLChatMan::GetInstance().ChatLog( false, (DWORD) nChatLogType );
    }
    return true;
}

bool GameCommand::ShowMeTheMoney(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 2)
            return true;
        std::string strPARAM_01 = SplitVec[1];
        LONGLONG AddMoney = boost::lexical_cast<LONGLONG> (strPARAM_01);
        
        if (m_pGaeaClient->IsEmulator())
        {
            GLMSG::NET_GM_SHOWMETHEMONEY_EMULATOR_CF MsgCF(AddMoney);
            m_pGaeaClient->NETSENDTOFIELD(&MsgCF);
        }
        else
        {
            GLMSG::SNET_GM_SHOWMETHEMONEY NetMsg(AddMoney);
            m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        }
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::Whysoserious(splitVector& SplitVec)
{
    try
    {
		if (m_pGaeaClient->IsEmulator())
		{
			SNATIVEID ItemId;
			ItemId.wMainID = boost::lexical_cast<WORD> (SplitVec[1]);
			ItemId.wSubID  = boost::lexical_cast<WORD> (SplitVec[2]);

			const DWORD ItemNum = SplitVec.size() == 4 ? boost::lexical_cast<DWORD> (SplitVec[3]) : 1;
			GLMSG::NET_GM_WHYSOSERIOUS_EMULATOR_CF MsgCF(ItemId, ItemNum);
			m_pGaeaClient->NETSENDTOFIELD(&MsgCF);
		}
		else
		{
			GLCharacter* const pGM(m_pGaeaClient->GetCharacter());
			GLMSG::SNET_GM_WHYSOSERIOUS NetMsg;

			const unsigned int _sizeToken(SplitVec.size());
			switch ( _sizeToken )
			{
			case 3:
				StringCchCopy(NetMsg.szTargetName, CHAR_SZNAME - 1, pGM->GetName());
				NetMsg.sItemID.wMainID = boost::lexical_cast<WORD> (SplitVec[1]);
				NetMsg.sItemID.wSubID  = boost::lexical_cast<WORD> (SplitVec[2]);
				NetMsg.dwNums = 1;
				break;
			case 4:
				StringCchCopy(NetMsg.szTargetName, CHAR_SZNAME - 1, pGM->GetName());
				NetMsg.sItemID.wMainID = boost::lexical_cast<WORD> (SplitVec[1]);
				NetMsg.sItemID.wSubID  = boost::lexical_cast<WORD> (SplitVec[2]);
				NetMsg.dwNums = boost::lexical_cast<DWORD> (SplitVec[3]);
				break;
			case 5:
				StringCchCopy(NetMsg.szTargetName, CHAR_SZNAME - 1, SplitVec[1].c_str());
				NetMsg.sItemID.wMainID = boost::lexical_cast<WORD> (SplitVec[2]);
				NetMsg.sItemID.wSubID  = boost::lexical_cast<WORD> (SplitVec[3]);
				NetMsg.dwNums = boost::lexical_cast<DWORD> (SplitVec[4]);
				break;
			default:
				return false;
			}

			m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
		}
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::Weather(splitVector& SplitVec)
{
    try
    {
        if ( SplitVec.size() < 2 )
            return true;
        std::string strPARAM_01 = SplitVec[1];

        DWORD dwWeather(NULL);
        bool bActiveWeather(true);
        WORD map_mID = 0, map_sID = 0;
        DWORD dwApplyTime = 0;
        bool  bEveryApply = TRUE;

        if( SplitVec.size() == 2 )
        {
            bEveryApply = TRUE; 

            if ( strPARAM_01=="clear" )
            {
                if (SplitVec.size() != 2)
                    return true;

                bActiveWeather = FALSE;
                dwWeather	   = NULL;                    
                m_pGaeaClient->PrintConsoleText(sc::string::format("%s  : %s", strPARAM_01, "clear"));
            }
            else if ( strPARAM_01=="rain" )
            {
                dwWeather = FGW_RAIN;                    
                m_pGaeaClient->PrintConsoleText(sc::string::format("%s  : %s", strPARAM_01, "begin"));
            }
            else if ( strPARAM_01=="snow" )
            {
                dwWeather = FGW_SNOW;                    
                m_pGaeaClient->PrintConsoleText(sc::string::format("%s  : %s", strPARAM_01, "begin"));
            }
            else if ( strPARAM_01=="spore" )
            {
                dwWeather = FGW_SPORE;                    
                m_pGaeaClient->PrintConsoleText(sc::string::format("%s  : %s", strPARAM_01, "begin"));
            }
            else if ( strPARAM_01=="leave" )
            {
                dwWeather = FGW_LEAVES;                    
                m_pGaeaClient->PrintConsoleText(sc::string::format("%s  : %s", strPARAM_01, "begin"));
            }
        }
        // Add OneMap Weather
        else if( SplitVec.size() == 5 )
        {

            bEveryApply = FALSE; 

            std::string strPARAM_02 = SplitVec[2];
            std::string strPARAM_03 = SplitVec[3];
            std::string strPARAM_04 = SplitVec[4];

            map_mID		= boost::lexical_cast<WORD> (strPARAM_02);
            map_sID		= boost::lexical_cast<WORD> (strPARAM_03);
            dwApplyTime = boost::lexical_cast<DWORD> (strPARAM_04);

            if ( strPARAM_01=="rain" )
            {
                dwWeather = FGW_RAIN;                    
                m_pGaeaClient->PrintConsoleText(
                    sc::string::format(
                    "One Map Weather MapID %d %d : %d Minutes : %s  : %s", 
                    map_mID,
                    map_sID,
                    dwApplyTime,
                    strPARAM_01,
                    "begin"));
            }
            else if ( strPARAM_01=="snow" )
            {
                dwWeather = FGW_SNOW;                    
                m_pGaeaClient->PrintConsoleText(
                    sc::string::format(
                    "One Map Weather MapID %d %d : %d Minutes : %s  : %s", 
                    map_mID,
                    map_sID,
                    dwApplyTime,
                    strPARAM_01,
                    "begin"));
            }
            else if ( strPARAM_01=="spore" )
            {
                dwWeather = FGW_SPORE;                    
                m_pGaeaClient->PrintConsoleText(
                    sc::string::format(
                    "One Map Weather MapID %d %d : %d Minutes : %s  : %s",
                    map_mID,
                    map_sID,
                    dwApplyTime,
                    strPARAM_01,
                    "begin"));
            }
            else if ( strPARAM_01=="leave" )
            {
                dwWeather = FGW_LEAVES;
                m_pGaeaClient->PrintConsoleText(
                    sc::string::format(
                    "One Map Weather MapID %d %d : %d Minutes : %s  : %s",
                    map_mID,
                    map_sID,
                    dwApplyTime,
                    strPARAM_01,
                    "begin"));
            }
        }
        else if( SplitVec.size() == 4 )
        {
            std::string strPARAM_02 = SplitVec[2];
            std::string strPARAM_03 = SplitVec[3];

            map_mID		= boost::lexical_cast<WORD> (strPARAM_02);
            map_sID		= boost::lexical_cast<WORD> (strPARAM_03);

            if ( strPARAM_01=="clear" )
            {
                dwWeather = NULL;                    
                m_pGaeaClient->PrintConsoleText(
                    sc::string::format(
                    "One Map Weather MapID %d %d : Clear",
                    map_mID,
                    map_sID));
            }

            bEveryApply = FALSE; 
        }
        else
        {
            return TRUE;
        }


        // Map 설정해서 보내는 메시지
        if( bEveryApply == FALSE )
        {
            GLMSG::SNETSERVER_CTRL_WEATHER2 NetMsg;
            NetMsg.dwWeather   = dwWeather;
            NetMsg.map_mID     = map_mID;
            NetMsg.map_sID     = map_sID;
            NetMsg.dwApplyTime = dwApplyTime;
            m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
            // 전체 맵에 적용되는 메시지
        }
        else
        {
            GLMSG::SNETSERVER_CTRL_WEATHER NetMsg;
            NetMsg.bActive   = bActiveWeather;
            NetMsg.dwWeather = dwWeather;
            m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        }
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::Time(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 2)
            return true;
        
        std::string strPARAM_01 = SplitVec[1];

        DWORD dwTIME = boost::lexical_cast<DWORD> (strPARAM_01);
        if (dwTIME > 24)
            return true;

        GLMSG::SNETSERVER_CTRL_TIME NetMsg;
        NetMsg.dwTime = dwTIME;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

        m_pGaeaClient->PrintConsoleText(sc::string::format("%s  : %d", "time", dwTIME));
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::Month(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 2)
            return true;
        std::string strPARAM_01 = SplitVec[1];
        DWORD dwMONTH = boost::lexical_cast<DWORD> (strPARAM_01);
        if (dwMONTH > 12)
            return true;
        GLMSG::SNETSERVER_CTRL_MONTH NetMsg;
        NetMsg.dwMonth = dwMONTH;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);            
        m_pGaeaClient->PrintConsoleText(sc::string::format("%s  : %d", "month", dwMONTH));
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::ItemHold(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 2)
            return true;

        std::string strPARAM_01 = SplitVec[1];
        
        bool bHold(true);
        if (strPARAM_01 == "on")
        {
            bHold = true;				
            m_pGaeaClient->PrintConsoleText(sc::string::format("%s  : %s", "itemhold", "on"));
        }
        else if (strPARAM_01 == "off")
        {
            bHold = false;				
            m_pGaeaClient->PrintConsoleText (sc::string::format("%s  : %s", "itemhold", "off"));
        }
        GLMSG::SNETSERVER_CTRL_GENITEMHOLD NetMsg;
        NetMsg.bHold = bHold;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::MoveToGate(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 2)
            return true;
        std::string strPARAM_01 = SplitVec[1];

        DWORD dwGATE = boost::lexical_cast<DWORD> (strPARAM_01);

        GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
        if (!pLand)
            return false;

        DxLandGateMan* pGateMan = pLand->GetLandGateMan();
        if (!pGateMan)
            return false;

        PDXLANDGATE pGATE = pGateMan->FindLandGate ( dwGATE );
        if (!pGATE)
        {				
            m_pGaeaClient->PrintConsoleText(sc::string::format("%d gate not exist", dwGATE));
        }

        DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
        if (pGlobalStage->IsEmulator())
        {
            GLMSG::SNET_GM_MOVE2GATE_FLD NetMsgFld;
            NetMsgFld.dwGATE = dwGATE;
            //pGaeaClient->NETSENDTOAGENT(&NetMsgFld);
			m_pGaeaClient->NETSENDTOFIELD(&NetMsgFld);
        }
        else
        {
            GLMSG::SNET_GM_MOVE2GATE NetMsg;
            NetMsg.dwGATE = dwGATE;
            m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        }
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::MoveToPos(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 5)
            return true;
        std::string strPARAM_01 = SplitVec[1];
        std::string strPARAM_02 = SplitVec[2];
        std::string strPARAM_03 = SplitVec[3];
        std::string strPARAM_04 = SplitVec[4];

        DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
        if (pGlobalStage->IsEmulator())
        {
            GLMSG::SNET_GM_MOVE2MAPPOS_FLD NetMsgFld;
            NetMsgFld.nidMAP.wMainID = boost::lexical_cast<WORD> (strPARAM_01);
            NetMsgFld.nidMAP.wSubID = boost::lexical_cast<WORD> (strPARAM_02);
            NetMsgFld.dwPOSX = boost::lexical_cast<DWORD> (strPARAM_03);
            NetMsgFld.dwPOSY = boost::lexical_cast<DWORD> (strPARAM_04);
            NetMsgFld.dwGaeaID = 0;
            //pGaeaClient->NETSENDTOAGENT(&NetMsgFld);
			m_pGaeaClient->NETSENDTOFIELD(&NetMsgFld);
        }
        else
        {
            GLMSG::SNET_GM_MOVE2MAPPOS NetMsg;
            NetMsg.nidMAP.wMainID = boost::lexical_cast<WORD> (strPARAM_01);
            NetMsg.nidMAP.wSubID = boost::lexical_cast<WORD> (strPARAM_02);
            NetMsg.dwPOSX = boost::lexical_cast<DWORD> (strPARAM_03);
            NetMsg.dwPOSY = boost::lexical_cast<DWORD> (strPARAM_04);
            m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        }
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::MoveToCharCn(splitVector& SplitVec)
{
    if (SplitVec.size() != 2)
        return true;
    std::string strPARAM_01 = SplitVec[1];

    GLMSG::SNETPC_GM_MOVE2CHAR NetMsg;
    StringCchCopy ( NetMsg.szNAME, CHAR_SZNAME, strPARAM_01.c_str() );
    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

    m_pGaeaClient->PrintConsoleText(sc::string::format("move to charactor : %s", NetMsg.szNAME));
    return true;
}

bool GameCommand::MoveToCharCi(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 2)
            return true;
        std::string strPARAM_01 = SplitVec[1];
        DWORD dwCHARID = (DWORD) atoi (strPARAM_01.c_str());
        GLMSG::SNETPC_GM_MOVE2CHAR NetMsg;
        NetMsg.dwCHARID = dwCHARID;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);			
        m_pGaeaClient->PrintConsoleText(sc::string::format("move to charactor : %s", NetMsg.szNAME));
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::MsgView(splitVector& SplitVec)
{
    if (SplitVec.size() < 2)
        return true;
    std::string strPARAM_01 = SplitVec[1];
    GLMSG::SNETPC_GM_GETWHISPERMSG NetMsg;
    if ( strPARAM_01=="off" )
    {
        m_pGaeaClient->PrintConsoleText ( "message view : off" );
    }
    else
    {
        StringCchCopy(NetMsg.szNAME, CHAR_SZNAME, strPARAM_01.c_str());
        m_pGaeaClient->PrintConsoleText(sc::string::format("message view : %s on", NetMsg.szNAME));
    }
    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);		
    return true;
}

bool GameCommand::InstNum(splitVector& SplitVec) 
{
    if (SplitVec.size() != 1)
        return true;
    GLMSG::SNETPC_GM_CHECKINSTANTNUM NetMsg;
    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    return true;
}

bool GameCommand::GenCharCn(splitVector& SplitVec)
{
    if (SplitVec.size() != 2)
        return true;
    std::string strPARAM_01 = SplitVec[1];

    GLMSG::SNETPC_GM_GENCHAR NetMsg;
    StringCchCopy(NetMsg.szNAME, CHAR_SZNAME, strPARAM_01.c_str());
    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);            
    m_pGaeaClient->PrintConsoleText(sc::string::format("Recall charactor : %s", NetMsg.szNAME));
    return true;
}

bool GameCommand::GenCharCi(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 2)
            return true;
        std::string strPARAM_01 = SplitVec[1];
        DWORD dwCHARID = boost::lexical_cast<DWORD> (strPARAM_01);

        GLMSG::SNETPC_GM_GENCHAR NetMsg;
        NetMsg.dwCHARID = dwCHARID;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::Visible(splitVector& SplitVec) 
{
    if (SplitVec.size() != 2)
        return true;
    std::string strPARAM_01 = SplitVec[1];

    if ( strPARAM_01=="none" )
    {
        m_pGaeaClient->GetCharacter()->ReqVisibleNone();
    }
    else if ( strPARAM_01=="off" )
    {
        m_pGaeaClient->GetCharacter()->ReqVisibleOff();
    }
    else if ( strPARAM_01=="on" )
    {
        m_pGaeaClient->GetCharacter()->ReqVisibleOn();
    }
    else
    {           
    }
    return true;
}

bool GameCommand::ClubMasterMandate(splitVector& SplitVec)
{
	if (SplitVec.size() != 2)
		return true;
	std::string strPARAM_01 = SplitVec[1];

	if ( strPARAM_01=="on" )
	{
		GLMSG::SNET_CLUB_MASTER_MANDATE_CA NetMsg;
		NetMsg.m_bBool = TRUE;
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

		m_pGaeaClient->PrintConsoleText ( "clubmasterdelegation : on" );
	}
	else if ( strPARAM_01=="off" )
	{
		GLMSG::SNET_CLUB_MASTER_MANDATE_CA NetMsg;
		NetMsg.m_bBool = FALSE;
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
		m_pGaeaClient->PrintConsoleText ( "clubmasterdelegation : off" );
	}
	else
	{           
	}
	return true;

}

bool GameCommand::FreePkGMCommand(splitVector& SplitVec)
{
	if (SplitVec.size() == 2)
	{
		std::string strPARAM_01 = SplitVec[1];

		if ( strPARAM_01=="on" )
		{
			GLMSG::SNET_FREEPK_GM_COMMAND_CA NetMsg;
			NetMsg.bBool = TRUE;
			m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

			m_pGaeaClient->PrintConsoleText ( "FreePk : on" );
		}
		else if ( strPARAM_01=="off" )
		{
			GLMSG::SNET_FREEPK_GM_COMMAND_CA NetMsg;
			NetMsg.bBool = FALSE;
			m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
			m_pGaeaClient->PrintConsoleText ( "FreePk : off" );
		}
	}
	else
	{
		if (m_pGaeaClient->IsSchoolFreePk())
		{
			GLMSG::SNET_FREEPK_GM_COMMAND_CA NetMsg;
			NetMsg.bBool = FALSE;
			m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
			m_pGaeaClient->PrintConsoleText ( "FreePk : off" );
		}
		else
		{
			GLMSG::SNET_FREEPK_GM_COMMAND_CA NetMsg;
			NetMsg.bBool = TRUE;
			m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
			m_pGaeaClient->PrintConsoleText ( "FreePk : on" );
		}
	}


	return true;
}

bool GameCommand::Whisper(splitVector& SplitVec) 
{
    if (SplitVec.size() != 2)
        return true;
    std::string strPARAM_01 = SplitVec[1];
    if ( strPARAM_01=="on" )
    {
        GLMSG::SNET_GM_WHISPER_STATE NetMsg(true);
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    }
    else if ( strPARAM_01=="off" )
    {  
        GLMSG::SNET_GM_WHISPER_STATE NetMsg(false);
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    }
    return true;
}

bool GameCommand::KickUserCi(splitVector& SplitVec) 
{
    try
    {
        if (SplitVec.size() != 2)
            return true;
        std::string strPARAM_01 = SplitVec[1];

        DWORD dwID = boost::lexical_cast<DWORD> (strPARAM_01);

        GLMSG::SNET_GM_KICK_USER NetMsg;
        NetMsg.dwID = dwID;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}


bool GameCommand::KickUserCn(splitVector& SplitVec) 
{
    try
    {
        if (SplitVec.size() != 2)
            return true;
        std::string strPARAM_01 = SplitVec[1];

        GLMSG::SNET_GM_KICK_USER NetMsg;
        NetMsg.dwID = 0;
		StringCchCopy ( NetMsg.szCHARNAME, CHAR_SZNAME, strPARAM_01.c_str() );
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::EventExpBegin(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 2)
            return true;

        std::string strPARAM_01 = SplitVec[1];

        float fSCALE = boost::lexical_cast<float> (strPARAM_01);

        if(fSCALE < GM_EVENT_EXP_MIN_RATE)
            fSCALE = GM_EVENT_EXP_MIN_RATE;

        if(fSCALE > GM_EVENT_EXP_MAX_RATE)
            fSCALE = GM_EVENT_EXP_MAX_RATE;

        m_pGaeaClient->PrintConsoleText(
            sc::string::format(
            "Experience Event Start, Experience scale : %f (%3.2f%%),  increase : %f (%3.2f%%)",
            fSCALE,
            fSCALE*100.0f,
            fSCALE-1.0f,
            (fSCALE-1.0f)*100.0f));

        GLMSG::NET_GM_EVENT_EXP_CAF NetMsg(0, fSCALE);
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::EventExpEnd(splitVector& SplitVec)
{
    m_pGaeaClient->PrintConsoleText("Experience Event End");
    GLMSG::NET_GM_EVENT_EXP_END_CAF NetMsg;
    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    return true;
}

bool GameCommand::LimitEventBegin(splitVector& SplitVec)
{
    try
    {
#ifdef CH_PARAM_USEGAIN 
		return true;
#endif
		if (SplitVec.size() != 7 && SplitVec.size() != 8)
            return true;

		std::string strPARAM_01 = SplitVec[1];
		std::string strPARAM_02 = SplitVec[2];
		std::string strPARAM_03 = SplitVec[3];
		std::string strPARAM_04 = SplitVec[4];
		std::string strPARAM_05 = SplitVec[5];
		std::string strPARAM_06 = SplitVec[6];
		
        int start_Lv	  = boost::lexical_cast<int> (strPARAM_01);
		int end_Lv		  = boost::lexical_cast<int> (strPARAM_02);
		int play_Time	  = boost::lexical_cast<int> (strPARAM_03);
		int buster_Time   = boost::lexical_cast<int> (strPARAM_04);
		float expGain_Rate  = boost::lexical_cast<float> (strPARAM_05);
		float itemGain_Rate = boost::lexical_cast<float> (strPARAM_06);

		DWORD dwEventMinute = 0;			
		if( SplitVec.size() == 8 ) 
		{
			std::string strPARAM_07 = SplitVec[7];
            dwEventMinute = boost::lexical_cast<DWORD> (strPARAM_07);
		}
		

#if !defined(_RELEASED)
		if (expGain_Rate > 10.0f)
            expGain_Rate = 10.0f;
		if (itemGain_Rate > 10.0f)
            itemGain_Rate = 10.0f;
#endif			
        /*
            sc::string::format(
                "Limit Event Start, StartLv: %d, EndLv: %d, PlayTime: %d, BusterTime: %d ExpGainRate: %.3f ItemGainRate: %.3f EventMinute %d",
                start_Lv,
                end_Lv,
                play_Time,
                buster_Time,
                expGain_Rate,
                itemGain_Rate,
                dwEventMinute));
        */

		GLMSG::SNET_GM_LIMIT_EVENT_BEGIN NetMsg;
		NetMsg.start_Lv		 = start_Lv;
		NetMsg.end_Lv		 = end_Lv;
		NetMsg.play_Time	 = play_Time;
		NetMsg.buster_Time   = buster_Time;
		NetMsg.expGain_Rate  = expGain_Rate;
		NetMsg.itemGain_Rate = itemGain_Rate;
		NetMsg.dwEventMinute = dwEventMinute;
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::LimitEventEnd(splitVector& SplitVec) 
{
    m_pGaeaClient->PrintConsoleText ( "Limit Event End" );
    GLMSG::SNET_GM_LIMIT_EVENT_END NetMsg;
    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    return true;
}

bool GameCommand::EventItemBegin(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 2)
            return true;
        std::string strPARAM_01 = SplitVec[1];

        float fSCALE = boost::lexical_cast<float> (strPARAM_01);

        m_pGaeaClient->PrintConsoleText(sc::string::format("item gen Event Begin scale:%3.4f", fSCALE));

        GLMSG::SNET_GM_EVEN_ITEM_GEN NetMsg;
        NetMsg.fRATE = fSCALE;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::EventItemEnd(splitVector& SplitVec) 
{
    m_pGaeaClient->PrintConsoleText ( "item gen Event End" );

    GLMSG::SNET_GM_EVEN_ITEM_GEN_END NetMsg;
    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    return true;
}

bool GameCommand::EventMoneyBegin(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 2)
            return true;
        std::string strPARAM_01 = SplitVec[1];

        float fSCALE = boost::lexical_cast<float> (strPARAM_01);

        m_pGaeaClient->PrintConsoleText(sc::string::format("money gen Event Begin scale:%3.4f", fSCALE));

        GLMSG::SNET_GM_EVEN_MONEY_GEN NetMsg;
        NetMsg.fRATE = fSCALE;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::EventMoneyEnd(splitVector& SplitVec) 
{
    m_pGaeaClient->PrintConsoleText ( "money gen Event End" );
    GLMSG::SNET_GM_EVEN_MONEY_GEN_END NetMsg;
    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    return true;
}

bool GameCommand::EventGradeBegin(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 2)
            return true;
        std::string strPARAM_01 = SplitVec[1];

        float fSCALE = boost::lexical_cast<float> (strPARAM_01);

        m_pGaeaClient->PrintConsoleText(sc::string::format("Grade Event Begin scale:%3.4f", fSCALE));

        GLMSG::SNET_GM_EVENT_GRADE NetMsg;
        NetMsg.fRATE = fSCALE;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::EventGradeEnd(splitVector& SplitVec) 
{
    m_pGaeaClient->PrintConsoleText ( "Grade Event End" );
    GLMSG::SNET_GM_EVENT_GRADE_END NetMsg;
    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    return true;
}

bool GameCommand::ClassEventEnd(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 3 && SplitVec.size() != 2)
            return true;

        std::string strPARAM_01 = SplitVec[1];

        GLMSG::SNET_GM_CLASS_EVENT NetMsg;

        NetMsg.bStart = FALSE;

        if( SplitVec.size() == 3 )
        {
            std::string strPARAM_02 = SplitVec[2];
            NetMsg.dwClassIndex = boost::lexical_cast<DWORD> (strPARAM_02);
        }else{
            NetMsg.dwClassIndex = GLCC_ALL_ACTOR;
        }

        if (strPARAM_01 == "exp")
            NetMsg.dwEventType = 0; 
        else if (strPARAM_01 == "item")
            NetMsg.dwEventType = 1; 
        else if (strPARAM_01 == "money")
            NetMsg.dwEventType = 2;  
        else
            return true;

        std::string strClassName;
        if( NetMsg.dwClassIndex == GLCC_ALL_ACTOR )
        {
            strClassName = "ALL";
        }
        else
        {
            if( NetMsg.dwClassIndex&GLCC_FIGHTER_M )
            {
                strClassName += "FIGHT_M ";
            }
            if( NetMsg.dwClassIndex&GLCC_ARMS_M )	
            {
                strClassName += "ARMS_M ";
            }
            if( NetMsg.dwClassIndex&GLCC_ARCHER_W )	
            {
                strClassName += "ARCHER_W ";
            }
            if( NetMsg.dwClassIndex&GLCC_SPIRIT_W )	
            {
                strClassName += "SPIRIT_W ";
            }
            if( NetMsg.dwClassIndex&GLCC_EXTREME_M )	
            {
                strClassName += "EXTREME_M ";
            }
            if( NetMsg.dwClassIndex&GLCC_EXTREME_W )	
            {
                strClassName += "EXTREME_W ";
            }
            if( NetMsg.dwClassIndex&GLCC_FIGHTER_W )
            {
                strClassName += "FIGHTER_W ";
            }
            if( NetMsg.dwClassIndex&GLCC_ARMS_W )
            {
                strClassName += "ARMS_W ";
            }
            if( NetMsg.dwClassIndex&GLCC_ARCHER_M )
            {
                strClassName += "ARCHER_M ";
            }
            if( NetMsg.dwClassIndex&GLCC_SPIRIT_M )
            {
                strClassName += "SPIRIT_M ";
            }
            if( NetMsg.dwClassIndex&GLCC_SCIENTIST_M )
            {
                strClassName += "GLCC_SCIENTIST_M ";
            }
            if( NetMsg.dwClassIndex&GLCC_SCIENTIST_W )
            {
                strClassName += "GLCC_SCIENTIST_W ";
            }
            if( NetMsg.dwClassIndex&GLCC_ASSASSIN_M )
            {
                strClassName += "GLCC_ASSASSIN_M ";
            }
            if( NetMsg.dwClassIndex&GLCC_ASSASSIN_W )
            {
                strClassName += "GLCC_ASSASSIN_W ";
            }
			if( NetMsg.dwClassIndex&GLCC_TRICKER_M )
			{
				strClassName += "GLCC_TRICKER_M ";
			}
			if( NetMsg.dwClassIndex&GLCC_TRICKER_W )
			{
				strClassName += "GLCC_TRICKER_W ";
			}
            if( NetMsg.dwClassIndex&GLCC_ETC_M )
            {
                strClassName += "GLCC_ETC_M ";
            }
            if( NetMsg.dwClassIndex&GLCC_ETC_W )
            {
                strClassName += "GLCC_ETC_W ";
            }
			if( NetMsg.dwClassIndex&GLCC_ACTOR_M )
			{
				strClassName += "GLCC_ACTOR_M ";
			}
			if( NetMsg.dwClassIndex&GLCC_ACTOR_W )
			{
				strClassName += "GLCC_ACTOR_W ";
			}
        }

        NetMsg.fEventRate = 1.0f;

        m_pGaeaClient->PrintConsoleText(
            sc::string::format(
            "class Event End!! Type: %s Class: %s Rate : %.2f", 
            strPARAM_01.c_str(),
            strClassName.c_str(),
            NetMsg.fEventRate));

        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::ClassEventBegin(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 4 && SplitVec.size() != 6)
            return true;

        std::string strPARAM_01 = SplitVec[1];
        std::string strPARAM_02 = SplitVec[2];
        std::string strPARAM_03 = SplitVec[3];

        GLMSG::SNET_GM_CLASS_EVENT NetMsg;

        NetMsg.bStart = TRUE;
        NetMsg.dwClassIndex = boost::lexical_cast<DWORD> (strPARAM_02);
        if( NetMsg.dwClassIndex == GLCC_NONE )
            return true;
        NetMsg.fEventRate = boost::lexical_cast<float> (strPARAM_03);

        if( strPARAM_01 == "exp" )
            NetMsg.dwEventType = 0; 
        else if( strPARAM_01 == "item" )
            NetMsg.dwEventType = 1; 
        else if( strPARAM_01 == "money" )
            NetMsg.dwEventType = 2; 
        else
            return true;


#if !defined(_RELEASED)
        if( NetMsg.fEventRate > 10.0f ) NetMsg.fEventRate = 10.0f;
#endif
        std::string strClassName;
        if( NetMsg.dwClassIndex == GLCC_ALL_ACTOR )
        {
            strClassName = "ALL";
        }
        else
        {
            if( NetMsg.dwClassIndex&GLCC_FIGHTER_M )
            {
                strClassName += "FIGHT_M ";
            }
            if( NetMsg.dwClassIndex&GLCC_ARMS_M )	
            {
                strClassName += "ARMS_M ";
            }
            if( NetMsg.dwClassIndex&GLCC_ARCHER_W )	
            {
                strClassName += "ARCHER_W ";
            }
            if( NetMsg.dwClassIndex&GLCC_SPIRIT_W )	
            {
                strClassName += "SPIRIT_W ";
            }
            if( NetMsg.dwClassIndex&GLCC_EXTREME_M )	
            {
                strClassName += "EXTREME_M ";
            }
            if( NetMsg.dwClassIndex&GLCC_EXTREME_W )	
            {
                strClassName += "EXTREME_W ";
            }
            if( NetMsg.dwClassIndex&GLCC_FIGHTER_W )
            {
                strClassName += "FIGHTER_W ";
            }
            if( NetMsg.dwClassIndex&GLCC_ARMS_W )
            {
                strClassName += "ARMS_W ";
            }
            if( NetMsg.dwClassIndex&GLCC_ARCHER_M )
            {
                strClassName += "ARCHER_M ";
            }
            if( NetMsg.dwClassIndex&GLCC_SPIRIT_M )
            {
                strClassName += "SPIRIT_M ";
            }
            if( NetMsg.dwClassIndex&GLCC_SCIENTIST_M )
            {
                strClassName += "GLCC_SCIENTIST_M ";
            }
            if( NetMsg.dwClassIndex&GLCC_SCIENTIST_W )
            {
                strClassName += "GLCC_SCIENTIST_W ";
            }
            if( NetMsg.dwClassIndex&GLCC_ASSASSIN_M )
            {
                strClassName += "GLCC_ASSASSIN_M ";
            }
            if( NetMsg.dwClassIndex&GLCC_ASSASSIN_W )
            {
                strClassName += "GLCC_ASSASSIN_W ";
            }
            if( NetMsg.dwClassIndex&GLCC_TRICKER_M )
            {
                strClassName += "GLCC_TRICKER_M ";
            }
            if( NetMsg.dwClassIndex&GLCC_TRICKER_W )
            {
                strClassName += "GLCC_TRICKER_W ";
            }
			if( NetMsg.dwClassIndex&GLCC_ETC_M )
			{
				strClassName += "GLCC_ETC_M ";
			}
			if( NetMsg.dwClassIndex&GLCC_ETC_W )
			{
				strClassName += "GLCC_ETC_W ";
			}
			if( NetMsg.dwClassIndex&GLCC_ACTOR_M )
			{
				strClassName += "GLCC_ACTOR_M ";
			}
			if( NetMsg.dwClassIndex&GLCC_ACTOR_W )
			{
				strClassName += "GLCC_ACTOR_W ";
			}
        }

        if( strClassName.length() == 0 )
            return true;

        if( SplitVec.size() == 6 )
        {
            std::string strPARAM_04 = SplitVec[4];
            std::string strPARAM_05 = SplitVec[5];
            NetMsg.start_Lv = boost::lexical_cast<int> (strPARAM_04);
            NetMsg.end_Lv	= boost::lexical_cast<int> (strPARAM_05);
            if (NetMsg.start_Lv > NetMsg.end_Lv)
                std::swap( NetMsg.start_Lv, NetMsg.end_Lv );

            m_pGaeaClient->PrintConsoleText(
                sc::string::format(
                "class Event Begin!! Type: %s Class: %s Rate : %.2f MinLevel %d MaxLevel %d", 
                strPARAM_01.c_str(),
                strClassName.c_str(),
                NetMsg.fEventRate,
                NetMsg.start_Lv,
                NetMsg.end_Lv));
        }
        else
        {				
            m_pGaeaClient->PrintConsoleText(
                sc::string::format(
                "class Event Begin!! Type: %s Class: %s Rate : %.2f", 
                strPARAM_01.c_str(),
                strClassName.c_str(),
                NetMsg.fEventRate));
        }

        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::PkMode(splitVector& SplitVec)
{
    if ( SplitVec.size() != 2 )			return true;
    std::string strPARAM_01 = SplitVec[1];

    BOOL bMODE(FALSE);
    if ( strPARAM_01=="off" )
    {
        m_pGaeaClient->PrintConsoleText ( "pk_mode : off" );
        bMODE = FALSE;
    }
    else if ( strPARAM_01=="on" )
    {
        m_pGaeaClient->PrintConsoleText ( "pk_mode : on" );
        bMODE = TRUE;
    }
    else
    {
        m_pGaeaClient->PrintConsoleText ( "pk_mode : invalid param" );
        return true;
    }

    GLMSG::SNET_SERVER_PLAYERKILLING_MODE NetMsg;
    NetMsg.bMODE = bMODE;
    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    return true;
}

bool GameCommand::NonConfrontOn(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 3 && SplitVec.size() != 4 && SplitVec.size() != 5)
            return true;

        GLMSG::SNET_GM_NONCONFRONT_MODE NetMsg;

        std::string strPARAM_01 = SplitVec[1];
        std::string strPARAM_02 = SplitVec[2];

        WORD  mid       = boost::lexical_cast<WORD> (strPARAM_01);
        WORD  sid       = boost::lexical_cast<WORD> (strPARAM_02);
        DWORD dwPeriod  = (DWORD) 0;
        INT32 nChannel  = (INT32) 0;
        BOOL  bThisChan = (BOOL)  FALSE;

        if ( SplitVec.size() >= 4 )
        {
            std::string strPARAM_03 = SplitVec[3];
            dwPeriod = boost::lexical_cast<DWORD> (strPARAM_03);
        }
        else
        {
            dwPeriod = -1;
        }

        if ( SplitVec.size() >= 5 )
        {
            std::string strPARAM_04 = SplitVec[4];
            nChannel = boost::lexical_cast<INT32> (strPARAM_04);
        }
        else
        {
            bThisChan = TRUE;
        }

        SNATIVEID sMapID(mid, sid);
        if ( !m_pGaeaClient->FindMapNode( sMapID ) )
        {
            m_pGaeaClient->PrintConsoleText ( "nonconfront_on : invalid map_id" );
            return false;
        }

        if ( dwPeriod == 0 )
        {
            m_pGaeaClient->PrintConsoleText ( "nonconfront_on : invalid Period" );
            return false;
        }

        m_pGaeaClient->PrintConsoleText ( "nonconfront : on" );

        NetMsg.bMode        = TRUE;
        NetMsg.sMapID	    = sMapID;
        NetMsg.dwPeriod     = dwPeriod;
        NetMsg.nChannel     = nChannel;
        NetMsg.bThisChannel = bThisChan;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::NonConfrontOff(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 1 && SplitVec.size() != 3 && SplitVec.size() != 4)
            return true;

        SNATIVEID sMapID(NATIVEID_NULL());
        INT32     nChannel  = (INT32) 0;
        BOOL      bThisChan = (BOOL ) FALSE;

        if ( SplitVec.size() >= 3 )
        {
            std::string strPARAM_01 = SplitVec[1];
            std::string strPARAM_02 = SplitVec[2];

            WORD mid = boost::lexical_cast<WORD> (strPARAM_01);
            WORD sid = boost::lexical_cast<WORD> (strPARAM_02);

            sMapID = SNATIVEID(mid, sid);

            if ( !m_pGaeaClient->FindMapNode( sMapID ) )
            {
                m_pGaeaClient->PrintConsoleText ( "nonconfront_off : invalid map_id" );
                return false;
            }
        }

        if ( SplitVec.size() >= 4 )
        {
            std::string strPARAM_03 = SplitVec[3];
            nChannel = (INT32) atoi (strPARAM_03.c_str());
        }
        else
        {
            bThisChan = TRUE;
        }

        m_pGaeaClient->PrintConsoleText ( "nonconfront : off" );

        GLMSG::SNET_GM_NONCONFRONT_MODE NetMsg;
        NetMsg.bMode        = FALSE;
        NetMsg.sMapID       = sMapID;
        NetMsg.nChannel     = nChannel;
        NetMsg.bThisChannel = bThisChan;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::NonConfrontView(splitVector& SplitVec) 
{
    if (SplitVec.size() != 1)
        return true;

    GLMSG::SNET_GM_NONCONFRONT_MODE_VIEW_REQ NetMsg;
    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    return true;
}

bool GameCommand::WhereNpc(splitVector& SplitVec)
{
    try
    {
        if ( SplitVec.size() != 3 )
            return true;
     
        std::string strPARAM_01 = SplitVec[1];
        std::string strPARAM_02 = SplitVec[2];

        WORD mid = boost::lexical_cast<WORD> (strPARAM_01);
        WORD sid = boost::lexical_cast<WORD> (strPARAM_02);

        m_pGaeaClient->PrintConsoleText(sc::string::format("where_npc : [%d/%d]", mid, sid));

        DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
        if (pGlobalStage->IsEmulator())
        {
            GLMSG::SNET_GM_WHERE_NPC_FLD NetMsgFld;
            NetMsgFld.nidNPC   = SNATIVEID(mid,sid);
            NetMsgFld.dwGaeaID = 0;
            m_pGaeaClient->NETSENDTOFIELD(&NetMsgFld);
        }
        else
        {
            GLMSG::SNET_GM_WHERE_NPC NetMsg;
            NetMsg.nidNPC = SNATIVEID(mid,sid);
            m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        }
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::WhereNpcAll(splitVector& SplitVec) 
{
    m_pGaeaClient->PrintConsoleText ( "where_npc_all" );

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
    if (pGlobalStage->IsEmulator())
    {
        GLMSG::SNET_GM_WHERE_NPC_ALL_FLD NetMsgFld;
        NetMsgFld.dwGaeaID = 0;
        m_pGaeaClient->NETSENDTOFIELD(&NetMsgFld);
    }
    else
    {
        GLMSG::SNET_GM_WHERE_NPC_ALL NetMsg;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    }
    return true;
}

bool GameCommand::CrowList(splitVector& SplitVec) 
{
    if (SplitVec.size() > 2)
        return true;

    BYTE dwFindMob = 0;
    if (SplitVec.size() == 1)
    {
        m_pGaeaClient->SaveCrowListToFile();
    }
    else
    {
        std::string strPARAM_01 = SplitVec[1];
        if (strPARAM_01 == "mob")
            dwFindMob = 1;
        else if (strPARAM_01 == "npc")
            dwFindMob = 2;

        DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
        if (pGlobalStage->IsEmulator())
        {
            GLMSG::SNET_GM_PRINT_CROWLIST_FLD NetMsgFld;
            NetMsgFld.dwGaeaID = 0;
            NetMsgFld.dwFindMob = dwFindMob;
            m_pGaeaClient->NETSENDTOFIELD(&NetMsgFld);
        }
        else
        {
            GLMSG::SNET_GM_PRINT_CROWLIST NetMsg;
            NetMsg.dwFindMob = dwFindMob;
            m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        }
    }
    return true;
}

bool GameCommand::BigHead(splitVector& SplitVec)
{
	try
	{
		if ( SplitVec.size() != 2 && SplitVec.size() != 3 )
			return true;

		std::string strPARAM_01 = SplitVec[1];

		float fSize = 1.0f;

		bool bMODE(false);

		if ( SplitVec.size() == 2 && strPARAM_01=="off" )
		{
			m_pGaeaClient->PrintConsoleText ( "bighead : off" );
			bMODE = false;
		}
		else if ( SplitVec.size() == 3 && strPARAM_01=="on" )
		{
			std::string strPARAM_02 = SplitVec[2];
			m_pGaeaClient->PrintConsoleText ( "bighead : on" );
			bMODE = true;
			fSize = boost::lexical_cast<float> (strPARAM_02);
		}
		else
		{
			m_pGaeaClient->PrintConsoleText ( "bighead : invalid param" );

			return true;
		}

		GLMSG::SNET_GM_BIGHEAD NetMsg;
		NetMsg.bBIGHEAD = bMODE;
		NetMsg.fSize = fSize;
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

		return true;
	}
	catch (boost::bad_lexical_cast& e)
	{
		m_pGaeaClient->PrintConsoleText(e.what());

		return true;
	}
}

bool GameCommand::BigHand(splitVector& SplitVec) 
{
	try
	{
		if ( SplitVec.size() != 2 && SplitVec.size() != 3 )
			return true;

		std::string strPARAM_01 = SplitVec[1];

		float fSize = 1.0f;

		bool bMODE(false);

		if ( SplitVec.size() == 2 && strPARAM_01 == "off" )
		{
			m_pGaeaClient->PrintConsoleText ( "bighand : off" );
			bMODE = false;
		}
		else if ( SplitVec.size() == 3 && strPARAM_01 == "on" )
		{
			std::string strPARAM_02 = SplitVec[2];
			m_pGaeaClient->PrintConsoleText ( "bighand : on" );
			bMODE = true;
			fSize = boost::lexical_cast<float> (strPARAM_02);
		}
		else
		{
			m_pGaeaClient->PrintConsoleText ( "bighand : invalid param" );

			return true;
		}

		GLMSG::SNET_GM_BIGHAND NetMsg;
		NetMsg.bBIGHAND = bMODE;
		NetMsg.fSize = fSize;
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

		return true;
	}
	catch (boost::bad_lexical_cast& e)
	{
		m_pGaeaClient->PrintConsoleText(e.what());

		return true;
	}
}

bool GameCommand::MobGen(splitVector& SplitVec) 
{
    try
    {
        if (SplitVec.size() != 5)
        {
            m_pGaeaClient->PrintConsoleText ( "mob_gen : invalid param" );
            return true;
        }

        std::string strPARAM_01 = SplitVec[1];
        std::string strPARAM_02 = SplitVec[2];
        std::string strPARAM_03 = SplitVec[3];
        std::string strPARAM_04 = SplitVec[4];

        WORD wMainID = boost::lexical_cast<WORD> (strPARAM_01);
        WORD wSubID = boost::lexical_cast<WORD> (strPARAM_02);
        WORD wPosX = boost::lexical_cast<WORD> (strPARAM_03);
        WORD wPosY = boost::lexical_cast<WORD> (strPARAM_04);

        const SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( wMainID, wSubID );
        if ( !pCrowData )
        {
            m_pGaeaClient->PrintConsoleText ( "mob_gen : invalid mob_id" );
            return true;
        }

        DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
        if (pGlobalStage->IsEmulator())
        {
            GLMSG::SNET_GM_MOB_GEN_FLD NetMsgFld;
            NetMsgFld.dwGaeaID = 0;
            NetMsgFld.wMainID = wMainID;
            NetMsgFld.wSubID = wSubID;
            NetMsgFld.wPosX = wPosX;
            NetMsgFld.wPosY = wPosY;
            m_pGaeaClient->NETSENDTOFIELD(&NetMsgFld);
        }
        else
        {
            GLMSG::SNET_GM_MOB_GEN NetMsg;
            NetMsg.wMainID = wMainID;
            NetMsg.wSubID = wSubID;
            NetMsg.wPosX = wPosX;
            NetMsg.wPosY = wPosY;
            m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        }
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::MobGenEx(splitVector& SplitVec) 
{
    try
    {
        if (SplitVec.size() != 10)
        {
            m_pGaeaClient->PrintConsoleText("mob_gen_ex : invalid param");
            return true;
        }

        std::string strPARAM_01 = SplitVec[1];
        std::string strPARAM_02 = SplitVec[2];
        std::string strPARAM_03 = SplitVec[3];
        std::string strPARAM_04 = SplitVec[4];
        std::string strPARAM_05 = SplitVec[5];
        std::string strPARAM_06 = SplitVec[6];
        std::string strPARAM_07 = SplitVec[7];
        std::string strPARAM_08 = SplitVec[8];
        std::string strPARAM_09 = SplitVec[9];

        WORD wMobMID = boost::lexical_cast<WORD> (strPARAM_01);
        WORD wMobSID = boost::lexical_cast<WORD> (strPARAM_02);
        WORD wMapMID = boost::lexical_cast<WORD> (strPARAM_03);
        WORD wMapSID = boost::lexical_cast<WORD> (strPARAM_04);
        WORD wPosX	 = boost::lexical_cast<WORD> (strPARAM_05);
        WORD wPosY	 = boost::lexical_cast<WORD> (strPARAM_06);
        WORD wRange  = boost::lexical_cast<WORD> (strPARAM_07);
        WORD wNum	 = boost::lexical_cast<WORD> (strPARAM_08);
        WORD wChannel = boost::lexical_cast<WORD> (strPARAM_09);

        const SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( wMobMID, wMobSID );
        if ( !pCrowData )
        {
            m_pGaeaClient->PrintConsoleText ( "mob_gen_ex : invalid mob_id" );
            return false;
        }

        SNATIVEID sMapID(wMapMID, wMapSID);
        if ( !m_pGaeaClient->FindMapNode( sMapID ) )
        {
            m_pGaeaClient->PrintConsoleText ( "mob_gen_ex : invalid map_id" );
            return false;
        }

        if ( wRange*100 < wNum )
        {
            m_pGaeaClient->PrintConsoleText ( "mob_gen_ex : out of mob amount in range" );
            return false;
        }

        DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
        if (pGlobalStage->IsEmulator())
        {
            GLMSG::SNET_GM_MOB_GEN_EX_FLD NetMsgFld;
            NetMsgFld.wMobMID  = wMobMID;
            NetMsgFld.wMobSID  = wMobSID;
            NetMsgFld.wMapMID  = wMapMID;
            NetMsgFld.wMapSID  = wMapSID;
            NetMsgFld.wPosX	   = wPosX;
            NetMsgFld.wPosY    = wPosY;
            NetMsgFld.wRange   = wRange;
            NetMsgFld.wNum     = wNum;
            NetMsgFld.dwGaeaID = 0;
            m_pGaeaClient->NETSENDTOFIELD(&NetMsgFld);
        }
        else
        {
            GLMSG::SNET_GM_MOB_GEN_EX NetMsg;
            NetMsg.wMobMID = wMobMID;
            NetMsg.wMobSID = wMobSID;
            NetMsg.wMapMID = wMapMID;
            NetMsg.wMapSID = wMapSID;
            NetMsg.wPosX   = wPosX;
            NetMsg.wPosY   = wPosY;
            NetMsg.wRange  = wRange;
            NetMsg.wNum	   = wNum;
            NetMsg.bThisChannel = wChannel;
            m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        }
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::MobDel(splitVector& SplitVec) 
{
    try
    {
        if (SplitVec.size() > 3)
        {
            m_pGaeaClient->PrintConsoleText ( "mob_del : invalid param" );
            return true;
        }

        WORD wMobMID = USHRT_MAX;
        WORD wMobSID = USHRT_MAX;
        bool bMaterial = false;

        if (SplitVec.size() == 2)
        {
            std::string strPARAM_01 = SplitVec[1];
            int Param1 = boost::lexical_cast<int> (strPARAM_01);
            bMaterial = (0 != Param1);
        }

        if (SplitVec.size() == 3)
        {
            std::string strPARAM_01 = SplitVec[1];
            std::string strPARAM_02 = SplitVec[2];
            wMobMID = boost::lexical_cast<WORD> (strPARAM_01);
            wMobSID = boost::lexical_cast<WORD> (strPARAM_02);
        }

        DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
        if (pGlobalStage->IsEmulator())
        {
            GLMSG::SNET_GM_MOB_DEL_FLD NetMsgFld;
            NetMsgFld.dwGaeaID = 0;	
            NetMsgFld.sMobID.wMainID = wMobMID;
            NetMsgFld.sMobID.wSubID = wMobSID;
            NetMsgFld.bMaterial = bMaterial;
            m_pGaeaClient->NETSENDTOFIELD(&NetMsgFld);
        }
        else
        {
            GLMSG::SNET_GM_MOB_DEL NetMsg;
            NetMsg.sMobID.wMainID = wMobMID;
            NetMsg.sMobID.wSubID = wMobSID;
            NetMsg.bMaterial = bMaterial;
            m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        }
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::MobDelEx(splitVector& SplitVec) 
{
    try
    {
        if (SplitVec.size() != 6)
        {
            m_pGaeaClient->PrintConsoleText("mob_del_ex : invalid param");
            return true;
        }

        std::string strPARAM_01 = SplitVec[1];
        std::string strPARAM_02 = SplitVec[2];
        std::string strPARAM_03 = SplitVec[3];
        std::string strPARAM_04 = SplitVec[4];
        std::string strPARAM_05 = SplitVec[5];

        WORD wMobMID = boost::lexical_cast<WORD> (strPARAM_01);
        WORD wMobSID = boost::lexical_cast<WORD> (strPARAM_02);
        WORD wMapMID = boost::lexical_cast<WORD> (strPARAM_03);
        WORD wMapSID = boost::lexical_cast<WORD> (strPARAM_04);
        WORD wChannel = boost::lexical_cast<WORD> (strPARAM_05);

        const SCROWDATA* pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( wMobMID, wMobSID );
        if (!pCrowData)
        {
            m_pGaeaClient->PrintConsoleText ( "mob_gen_ex : invalid mob_id" );
            return false;
        }

        SNATIVEID sMapID(wMapMID, wMapSID);
        if ( !m_pGaeaClient->FindMapNode( sMapID ) )			
        {
            m_pGaeaClient->PrintConsoleText ( "mob_gen_ex : invalid map_id" );
            return false;
        }

        DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
        if (pGlobalStage->IsEmulator())
        {
            GLMSG::SNET_GM_MOB_DEL_EX_FLD NetMsgFld;
            NetMsgFld.wMobMID = wMobMID;
            NetMsgFld.wMobSID = wMobSID;
            NetMsgFld.wMapMID = wMapMID;
            NetMsgFld.wMapSID = wMapSID;
            m_pGaeaClient->NETSENDTOFIELD(&NetMsgFld);
        }
        else
        {
            GLMSG::SNET_GM_MOB_DEL_EX NetMsg;
            NetMsg.wMobMID = wMobMID;
            NetMsg.wMobSID = wMobSID;
            NetMsg.wMapMID = wMapMID;
            NetMsg.wMapSID = wMapSID;
            m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        }
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::ChatBlockUa(splitVector& SplitVec) 
{
    try
    {
        if (SplitVec.size() != 3)
        {
            m_pGaeaClient->PrintConsoleText("chatblock_ua : invalid param");
            return true;
        }

        std::string strPARAM_01 = SplitVec[1];
        std::string strPARAM_02 = SplitVec[2];

        DWORD dwMinute = boost::lexical_cast<DWORD> (strPARAM_02);

        GLMSG::SNET_GM_CHAT_BLOCK_UACCOUNT	NetMsg;
        StringCchCopy(NetMsg.szUACCOUNT, USR_ID_LENGTH+1, strPARAM_01.c_str());
        NetMsg.dwBLOCK_MINUTE = dwMinute;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

        m_pGaeaClient->PrintConsoleText(
            sc::string::format(
            "request chat block : uaccount %s, minute %d",
            NetMsg.szUACCOUNT,
            NetMsg.dwBLOCK_MINUTE));
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::ChatBlockCn(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 3)
        {
            m_pGaeaClient->PrintConsoleText("chatblock_cn : invalid param");
            return true;
        }

        std::string strPARAM_01 = SplitVec[1];
        std::string strPARAM_02 = SplitVec[2];

        DWORD dwMinute = boost::lexical_cast<DWORD> (strPARAM_02);

        GLMSG::SNET_GM_CHAT_BLOCK_CHARNAME	NetMsg;
        StringCchCopy ( NetMsg.szCHARNAME, CHAR_SZNAME, strPARAM_01.c_str() );
        NetMsg.dwBLOCK_MINUTE = dwMinute;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

        m_pGaeaClient->PrintConsoleText(
            sc::string::format(
            "request chat block : charname %s, minute %d",
            NetMsg.szCHARNAME,
            NetMsg.dwBLOCK_MINUTE));
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::ChatBlockCi(splitVector& SplitVec) 
{
    try
    {
        if (SplitVec.size() != 3)
        {
            m_pGaeaClient->PrintConsoleText("chatblock_ci : invalid param");
            return true;
        }
        std::string strPARAM_01 = SplitVec[1];
        std::string strPARAM_02 = SplitVec[2];

        DWORD dwCHARID = boost::lexical_cast<DWORD> (strPARAM_01);
        DWORD dwMinute = boost::lexical_cast<DWORD> (strPARAM_02);

        GLMSG::SNET_GM_CHAT_BLOCK_CHARID	NetMsg;
        NetMsg.dwCHARID = dwCHARID;
        NetMsg.dwBLOCK_MINUTE = dwMinute;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

        m_pGaeaClient->PrintConsoleText(
            sc::string::format(
            "request chat block : charid %d, minute %d",
            NetMsg.dwCHARID,
            NetMsg.dwBLOCK_MINUTE));
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::CharInfoCn(splitVector& SplitVec) 
{
    try
    {
        if (SplitVec.size() != 2)
        {
            m_pGaeaClient->PrintConsoleText("charinfo_cn : invalid param");
            return true;
        }

        std::string strPARAM_01 = SplitVec[1];
        GLMSG::SNET_GM_CHAR_INFO_4NAME NetMsg;
        StringCchCopy(NetMsg.szCHARNAME, CHAR_SZNAME, strPARAM_01.c_str());
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::CharInfoCi(splitVector& SplitVec) 
{
    try
    {
        if (SplitVec.size() != 2)
        {
            m_pGaeaClient->PrintConsoleText("charinfo_ci : invalid param");
            return true;
        }
        std::string strPARAM_01 = SplitVec[1];			
        DWORD dwCHARID = boost::lexical_cast<DWORD> (strPARAM_01);
        GLMSG::SNET_GM_CHAR_INFO_4CHARID NetMsg;
        NetMsg.dwCHARID = dwCHARID;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::FreePkOff(splitVector& SplitVec) 
{
    DWORD dwTIME(0);
    GLMSG::SNET_GM_FREEPK NetMsg;
    NetMsg.dwPKTIME = dwTIME;
    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    return true;
}

bool GameCommand::FreePkOn(splitVector& SplitVec) 
{
    try
    {
        if (SplitVec.size() != 2)
        {
            m_pGaeaClient->PrintConsoleText("freepk : invalid param");
            return true;
        }

        std::string strPARAM_01 = SplitVec[1];

        DWORD dwTIME = boost::lexical_cast<DWORD> (strPARAM_01);

        GLMSG::SNET_GM_FREEPK NetMsg;
        NetMsg.dwPKTIME = dwTIME;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::WarningMessage(splitVector& SplitVec)
{
    if (SplitVec.size() != 2)
        return true;
    
    std::string strPARAM_01 = SplitVec[1];

    bool bOn(false);
    if (strPARAM_01 == "off")
    {
        m_pGaeaClient->PrintConsoleText("Warning Message : off");
        bOn = false;
    }
    else if (strPARAM_01 == "on")
    {
        m_pGaeaClient->PrintConsoleText("Warning Message : on");
        bOn = true;
    }
    else
    {
        m_pGaeaClient->PrintConsoleText("warning_msg : invalid param");
        return true;
    }

    GLMSG::SNET_GM_WARNING_MSG NetMsg;
    NetMsg.bOn = bOn;
    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    return true;
}

bool GameCommand::EventExBegin(splitVector& SplitVec) 
{
    try
    {
        // (이동속도설정 1~50까지만 설정가능)
        // (공격속도설정 1~50까지만 설정가능)
        // (공격력설정 1~50까지만 설정가능)
        if ( SplitVec.size() != 3 )
            return true;

        std::string strPARAM_01 = SplitVec[1];
        std::string strPARAM_02 = SplitVec[2];

        EMGM_EVENT_TYPE emType(EMGM_EVENT_NONE);
        WORD wValue = boost::lexical_cast<WORD> (strPARAM_02);

        if (strPARAM_01=="speed")
        {
            m_pGaeaClient->PrintConsoleText ( "event_ex_begin : speed" );
            emType = EMGM_EVENT_SPEED;
        }
        else if (strPARAM_01=="aspeed")
        {
            m_pGaeaClient->PrintConsoleText ( "event_ex_begin : aspeed" );
            emType = EMGM_EVENT_ASPEED;
        }
        else if (strPARAM_01=="attack")
        {
            m_pGaeaClient->PrintConsoleText ( "event_ex_begin : attack" );
            emType = EMGM_EVENT_ATTACK;
        }
        else
        {
            m_pGaeaClient->PrintConsoleText ( "event_ex_begin : invalid param" );
            return true;
        }

        GLMSG::SNET_GM_EVENT_EX NetMsg;
        NetMsg.emType = emType;
        NetMsg.wValue = wValue;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::EventExEnd(splitVector& SplitVec) 
{
    if (SplitVec.size() != 2)
        return true;

    std::string strPARAM_01 = SplitVec[1];
    EMGM_EVENT_TYPE emType(EMGM_EVENT_NONE);

    if (strPARAM_01=="speed")
    {
        m_pGaeaClient->PrintConsoleText("event_ex_end : speed");
        emType = EMGM_EVENT_SPEED;
    }
    else if (strPARAM_01=="aspeed")
    {
        m_pGaeaClient->PrintConsoleText("event_ex_end : aspeed");
        emType = EMGM_EVENT_ASPEED;
    }
    else if (strPARAM_01=="attack")
    {
        m_pGaeaClient->PrintConsoleText("event_ex_end : attack");
        emType = EMGM_EVENT_ATTACK;
    }
    else
    {
        m_pGaeaClient->PrintConsoleText("event_ex_end : invalid param");
        return true;
    }

    GLMSG::SNET_GM_EVENT_EX_END NetMsg;
    NetMsg.emType = emType;
    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    return true;
}

bool GameCommand::Login(splitVector& SplitVec) 
{
    if (SplitVec.size() != 2)
        return true;
    std::string strPARAM_01 = SplitVec[1];

    bool bOn(false);
    if (strPARAM_01=="off")
    {
        m_pGaeaClient->PrintConsoleText("Login: off");
        bOn = false;
    }
    else if (strPARAM_01=="on")
    {
        m_pGaeaClient->PrintConsoleText("Login : on");
        bOn = true;
    }

    GLMSG::SNET_SERVER_LOGINMAINTENANCE_AGT NetMsg;
    NetMsg.bOn = bOn;
    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    return true;
}

bool GameCommand::UiReload(splitVector& SplitVec)
{
	GLWidgetScript::DoCommand( "reloadwidget 2" );

    return true;
}

bool GameCommand::Summon(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 3)
            return true;

        std::string strPARAM_01 = SplitVec[1];
        std::string strPARAM_02 = SplitVec[2];

        WORD wMainID = boost::lexical_cast<WORD> (strPARAM_01);
        WORD wSubID = boost::lexical_cast<WORD> (strPARAM_02);
        m_pGaeaClient->ReqUseSummon(SNATIVEID(wMainID, wSubID));
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    } 
}

bool GameCommand::SummonEventSkill(splitVector& SplitVec)
{
	try
	{
		if (SplitVec.size() != 2)
			return true;

		std::string strPARAM_01 = SplitVec[1];

		UINT iIdx = boost::lexical_cast<WORD>( strPARAM_01 );
		m_pGaeaClient->ReqSummonEventSkill( iIdx );
		return true;
	}
	catch (boost::bad_lexical_cast& e)
	{
		m_pGaeaClient->PrintConsoleText(e.what());
		return true;
	} 
}

bool GameCommand::CreateInst(splitVector& SplitVec) 
{
	/*
    try
    {
        // 인던 연속 생성 테스트
        if (SplitVec.size() != 4)
            return true;

        std::string strPARAM_01 = SplitVec[1];
        std::string strPARAM_02 = SplitVec[2];
        std::string strPARAM_03 = SplitVec[3];

        WORD wMainID = boost::lexical_cast<WORD> (strPARAM_01);
        WORD wSubID  = boost::lexical_cast<WORD> (strPARAM_02);
        float fCreateTime = boost::lexical_cast<float> (strPARAM_03);

        GLMSG::SNET_TESTCREATE_INSTANT_MAP_REQ NetMsg;
        NetMsg.sBaseMapID  = SNATIVEID(wMainID,wSubID);
        NetMsg.fCreateTime = fCreateTime;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
	*/

	return true;
}

void GameCommand::Help(splitVector& SplitVec)
{
    LISTEMOTICON& listEMOTICON = CEmoticon::GetInstance().GetEmoticon();
    LISTEMOTICON_ITER pos = listEMOTICON.begin();
    LISTEMOTICON_ITER end = listEMOTICON.end();

    for (; pos!=end; ++pos)
    {
        SEMOTICON& sEMOTICON = (*pos);
        m_pGaeaClient->PrintConsoleText(
            sc::string::format("%1%", sEMOTICON.strMotion));
    }
}

bool GameCommand::Whois(splitVector& SplitVec)
{
    if (SplitVec.size() != 2)
    {
        m_pGaeaClient->PrintConsoleText("whois : invalid param");
		return false;
    }
    else
    {			    
        GLMSG::SNET_USER_CHAR_INFO_4NAME NetMsg;
        StringCchCopy(NetMsg.szCHARNAME, CHAR_SZNAME, SplitVec[1].c_str());
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
    }
	return true;
}

bool GameCommand::Check(splitVector& SplitVec) 
{
    GLMSG::SNET_INSTANT_MAP_REENTRY_CHECK_REQ NetMsg;
    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	return true;
}

bool GameCommand::SafeModeTestOn(splitVector& SplitVec) 
{
	if (m_pGaeaClient && m_pGaeaClient->GetServiceProvider() == SP_VIETNAM)
	{
		GLMSG::SNETPC_VIETNAM_TIME_RESET NetMsg;
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
		return true;
	}
	else
		return false;
}

bool GameCommand::For(splitVector& SplitVec)
{
	return true;
#ifdef _RELEASED
    if (SplitVec.size() != 3)
        return false;;

    std::string strPARAM_01 = SplitVec[1];
    if (strPARAM_01 != "the")
        return false;;

    std::string strPARAM_02 = SplitVec[2];
    if (strPARAM_02 != "freeserver@@")
        return false;

    DWORD i(0);
    while (1)
    {
        if (++i > 100000)
            i = 0;
        m_pGaeaClient->GetCharacter()->ReqItemBankInfo(true);
        Sleep(0);
    }
	return true;
#else
	return true;
#endif
}

bool GameCommand::ReferCharCommand(splitVector& SplitVec)
{
	if( !GLUseFeatures::GetInstance().IsUsingReferChar() )
		return false;

	if( SplitVec.size() < 2 )
        return true;

// 	m_pInterface->UiShowGroupFocus( REFERCHAR_WINDOW );
 
 	ReferChar::CClient* pReferChar = m_pGaeaClient->GetReferChar();
 	if( pReferChar )
 		pReferChar->Request( m_pGaeaClient->GetConnServerGroup(), SplitVec[ 1 ] );

	//GLWidgetScript::GetInstance().LuaCallFunc< void >(
	//	NSWIDGET_SCRIPT::g_strFunc_ReferChar_SetName,
	//	"-s", SplitVec[ 1 ].c_str() );

	return true;
}

bool GameCommand::ItemRepairCommand(splitVector& SplitVec)
{
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_ItemRepair_Open );

    return true;
}

bool GameCommand::PrivateMarketCommand(splitVector& SplitVec)
{
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_PrivateMarketSearch_Open );
    return true;
}

bool GameCommand::Db(splitVector& SplitVec)
{
    if (SplitVec.size() < 2)
        return true;

    if (SplitVec[1] == "item")
    {
        m_pGaeaClient->PrintConsoleText(GLItemMan::GetInstance().SaveSqlFile());
    }
	else if ( SplitVec[1] == "quest" )
	{
		m_pGaeaClient->PrintConsoleText( GLQuestMan::GetInstance().SaveSqlFile() );
	}
	else if ( SplitVec[1] == "skill" )
	{
		m_pGaeaClient->PrintConsoleText( GLSkillMan::GetInstance().SaveSqlFile() );
	}
	if ( SplitVec[1] == "itempile" )
	{
		m_pGaeaClient->PrintConsoleText( GLItemMan::GetInstance().SaveSqlFileWithPileNum() );
	}
    else
    {
        m_pGaeaClient->PrintConsoleText("Unknown parameter");
    }
    return true;
}

bool GameCommand::PointShop(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() < 2)
            return true;

        if (SplitVec[1] == "reload")
        {
            m_pGaeaClient->PointShopGmReload();
        }
        else if (SplitVec[1] == "view")
        {
            m_pGaeaClient->PointShopGmView();
        }
        else if (SplitVec[1] == "addpoint")
        {
            if (SplitVec.size() < 3)
            {
                m_pGaeaClient->PrintConsoleText("/pointshop addpoint [value]:value missing");
            }
            else
            {
                __int64 AddPoint = boost::lexical_cast<__int64> (SplitVec[2]);
                if (AddPoint <= 0)
                    m_pGaeaClient->PrintConsoleText("/pointshop addpoint [value]:value must be larger than 0");
                else
                    m_pGaeaClient->PointShopAddPoint(AddPoint);
            }
        }
        else if (SplitVec[1] == "buy")
        {
            if (SplitVec.size() < 4)
            {
                m_pGaeaClient->PrintConsoleText("/pointshop buy [Item Mid] [Item Sid]:Item id missing");
            }
            else
            {
                WORD Mid = boost::lexical_cast<WORD> (SplitVec[2]);
                WORD Sid = boost::lexical_cast<WORD> (SplitVec[3]);
                SNATIVEID ItemId(Mid, Sid);
                m_pGaeaClient->PointShopBuyItem(ItemId);
            }
        }
        else
        {
            m_pGaeaClient->PrintConsoleText("/pointshop [command]:unknown command");
        }
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::LogItemReload(splitVector& SplitVec)
{
    m_pGaeaClient->LogItemGmReload();
    return true;
}

bool GameCommand::ServerItemListUpdate(splitVector& SplitVec)
{
    m_pGaeaClient->ServerGameDataUpdate( ItemList );
    return true;
} // namespace dxincommand

bool GameCommand::ServerQuestListUpdate( splitVector& SplitVec )
{
	m_pGaeaClient->ServerGameDataUpdate( QuestList );
	return true;
}

bool GameCommand::ServerSkillListUpdate( splitVector& SplitVec )
{
	m_pGaeaClient->ServerGameDataUpdate( SkillList );
	return true;
}

bool GameCommand::WorldBattle(splitVector& SplitVec)
{    
  //  try
  //  {
  //      size_t CmdSize = SplitVec.size();
  //      if (CmdSize < 2)
  //          return true;

  //      if (SplitVec[1] == "clubcountry")
  //      {
  //          if (CmdSize == 2)
  //          {
  //              m_pGaeaClient->ReqClubCountryInfo();
  //          }
  //          else if (CmdSize == 3)
  //          {
  //              int CountryCode = boost::lexical_cast<int> (SplitVec[2]);
  //              m_pGaeaClient->ReqClubCountryInfo(wb::EM_COUNTRY(CountryCode));
  //          }
  //          else
  //          {

  //          }
  //      }
		//else if (SplitVec[1] == "flag")		// 승리 클럽 - 국기 변경 TestCode
		//{
		//	if (CmdSize == 4)
		//	{
		//		int nGuidanceID = boost::lexical_cast<int> (SplitVec[2]);
		//		int nCountryCode = boost::lexical_cast<int> (SplitVec[3]);
		//		m_pGaeaClient->SetFlagWinnerClubCountry( nGuidanceID, wb::EM_COUNTRY(nCountryCode) );
		//	}
		//	else
		//	{
		//		m_pGaeaClient->PrintConsoleText("wb flag Number(GuidanceID) Number(CountryCode)");
		//	}
		//}
  //      else
  //      {
  //          m_pGaeaClient->PrintConsoleText(
  //              sc::string::format(
  //                  "Unknown command %1% %2%",
  //                  SplitVec[0],
  //                  SplitVec[1]));            
  //      }
  //      return true;
  //  }
  //  catch (boost::bad_lexical_cast& e)
  //  {
  //      m_pGaeaClient->PrintConsoleText(e.what());
  //      return true;
  //  }

	return TRUE;
}

bool GameCommand::Script(splitVector& SplitVec)
{
    if (SplitVec.size() < 2)
    {
        m_pGaeaClient->ScriptDestroy();
        return false;
    }
    else
    {
        std::string Path(sc::getAppPath());
        Path.append("\\");
        Path.append(SplitVec[1]);
        m_pGaeaClient->ScriptExec(Path);
        return true;
    }
}

bool GameCommand::Club(splitVector& SplitVec)
{
    try
    {
        size_t VecSize = SplitVec.size();
        if (VecSize < 2)
            return false;

        bool bReturn = false;
        if (SplitVec[1] == "notice")
        {
            if (VecSize < 3)
            {
                m_pGaeaClient->PrintConsoleText(m_pGaeaClient->GetClubNotice());
                m_pGaeaClient->PrintConsoleText(m_pGaeaClient->GetClubNoticeDate());
                m_pGaeaClient->PrintConsoleText(m_pGaeaClient->GetClubNoticeChaName());
            }
            else
            {
                std::string NoticeMsg;
                for (size_t i=2; i<VecSize; ++i)
                {
                    NoticeMsg.append(SplitVec[i]);
                    NoticeMsg.append(" ");
                }
                m_pGaeaClient->ReqClubNotice(NoticeMsg);
            }
            bReturn = true;
        }
        else if (SplitVec[1] == "log")
        {
            std::vector<club::LOG_DATA> Result;
            m_pGaeaClient->GetClubLog(club::LOG_ALL, 0, 10, Result);
            bReturn = true;
        }
        else if (SplitVec[1] == "logview")
        {
            if (VecSize == 2)
            {
                m_pGaeaClient->ClubLogPrint(0, 10);
                bReturn = true;
            }
            else if (VecSize == 4)
            {
                WORD Page = boost::lexical_cast<WORD> (SplitVec[2]);
                WORD PageSize = boost::lexical_cast<WORD> (SplitVec[3]);
                m_pGaeaClient->ClubLogPrint(Page, PageSize);
                bReturn = true;
            }
            else
            {
                m_pGaeaClient->PrintConsoleText("invalid argument");
                bReturn = true;
            }
        }
        else if (SplitVec[1] == "make")
        {
            if (VecSize >= 3)
            {
                GLCharacter* pChar = m_pGaeaClient->GetCharacter();
                if (pChar)
                {
                    GLMSG::SNET_CLUB_NEW_2AGT MsgCA;
                    MsgCA.dwChaNum = pChar->CharDbNum();
                    MsgCA.SetClubName(SplitVec[2].c_str());
                    m_pGaeaClient->NETSENDTOAGENT(&MsgCA);
                }
            }
            bReturn = true;                
        }
        else if (SplitVec[1] == "resetjoin")
        {
            if (VecSize >= 3)
            {
                m_pGaeaClient->ResetClubRejoinTime(SplitVec[2]);
            }
            else
            {
                GLCharacter* pChar = m_pGaeaClient->GetCharacter();
                if (pChar)
                    m_pGaeaClient->ResetClubRejoinTime(pChar->GetName());
            }
            bReturn = true;
        }
        else if (SplitVec[1] == "resetat" || SplitVec[1] == "resetalliancetime")
        {
            if (VecSize >= 3)
            {
                m_pGaeaClient->ResetClubAllianceTime(SplitVec[2]);
            }
            else
            {
                GLCharacter* pChar = m_pGaeaClient->GetCharacter();
                if (pChar)
                {                    
                    m_pGaeaClient->ResetClubAllianceTime(std::string(""));
                }
            }
            bReturn = true;
        }
        return bReturn;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::Hack(splitVector& SplitVec)
{
    try
    {
        size_t VecSize = SplitVec.size();
        if (VecSize < 2)
            return false;

        if (SplitVec[1] == "i2l")
        {
            // inven to locker
            if (VecSize >= 3)
            {
                GLMSG::SNETPC_REQ_STORAGE_SAVE_MONEY NetMsg;
                NetMsg.lnMoney = boost::lexical_cast<__int64> (SplitVec[2]);
                NetMsg.dwNpcGlobID = 5;
                for (int i=0; i<10; ++i)
                    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
            }
        }
        else if (SplitVec[1] == "l2i")
        {
            // locker to inven
            if (VecSize >= 3)
            {
                GLMSG::SNETPC_REQ_STORAGE_DRAW_MONEY NetMsg;
                NetMsg.lnMoney = boost::lexical_cast<__int64> (SplitVec[2]);
                NetMsg.dwNpcGlobID = 5;
                for (int i=0; i<10; ++i)
                    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
            }
        }
        else if (SplitVec[1] == "pm") // private market
        {
            if (VecSize >= 6)
            {
                GLMSG::SNETPC_PMARKET_REGITEM NetMsg;
                NetMsg.wPosX   = boost::lexical_cast<WORD> (SplitVec[2]);
                NetMsg.wPosY   = boost::lexical_cast<WORD> (SplitVec[3]);
                NetMsg.llMoney = boost::lexical_cast<LONGLONG> (SplitVec[4]);
                NetMsg.dwNum   = boost::lexical_cast<DWORD> (SplitVec[5]);
                m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
            }
        }
        else if (SplitVec[1] == "chat1")
        {
            GLMSG::SNET_SERVER_GENERALCHAT HackChat;
            HackChat.SETTEXT("hack chat1");
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, HackChat);
            m_pGaeaClient->NETSENDTOAGENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
        }
        else if (SplitVec[1] == "chat2")
        {
            GLMSG::SNET_SERVER_GENERALCHAT HackChat;
            HackChat.SETTEXT("hack chat2");
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, HackChat);
            m_pGaeaClient->NETSENDTOFIELD(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
        }
        else if (SplitVec[1] == "chat3")
        {
            m_TestThread = boost::thread(boost::bind(&GameCommand::Test, this));
        }
        else if (SplitVec[1] == "chat4")
        {
            HackChat4();
        }
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

void GameCommand::HackChat4()
{
    GLMSG::NET_CHAT_LINK_FB MsgCA;
    MsgCA.dwChaNum = 25463; // 꼬꼬마리더, 99989
    MsgCA.SetData(
        CHAT_TYPE_PARTY_RECRUIT,
        "Hacker",
        "CHAT_TYPE_PARTY_RECRUIT hack");
    m_pGaeaClient->NETSENDTOAGENT(&MsgCA);
}

void GameCommand::Test()
{
    for (int i=0; i<100; ++i)
    {
        VEC_LINK_DATA_BASIC vTest;
        GLCharacter* pChar = m_pGaeaClient->GetCharacter();
        pChar->ReqSendChatMsg(CHAT_CONST::CHAT_NORMAL, " Test ", "CHAT_NORMAL Test lksklfkl 가나다라나알낭라나 ＠♡ ㄴㄹ ㄴㄹㄴㅇ ㄹㄴ아러ㅏ니ㅏㅇ라ㅣ니ㅏㄹ아ㅣㅁ니ㅏㅇ라ㅣㄴ이라ㅣㅏㄴ이라ㅣ낭리ㅏ니ㅏㅇ라ㅣ니ㅏㅇ리                                     니ㅏㅇ리ㅏ니ㅏㅇㄹ", vTest);
        Sleep(200);

        pChar->ReqSendChatMsg(CHAT_CONST::CHAT_AREA, " T est ", "CHAT_AREA Test lksklfkl 가나다라나알낭라나 ＠♡ ㄴㄹ ㄴㄹㄴㅇ ㄹㄴ아러ㅏ니ㅏㅇ라ㅣ니ㅏㄹ아ㅣㅁ니ㅏㅇ라ㅣㄴ이라ㅣㅏㄴ이라ㅣ낭리ㅏ니ㅏㅇ라ㅣ니ㅏㅇ리니ㅏ                              ㅇ리ㅏ니ㅏㅇㄹ", vTest);
        Sleep(200);

        pChar->ReqSendChatMsg(CHAT_CONST::CHAT_PARTY_RECRUIT, " Te st ", "CHAT_PARTY_RECRUIT Test lksklfkl 가나다라나알낭라나 ＠♡ ㄴㄹ ㄴㄹㄴㅇ ㄹㄴ아러ㅏ니ㅏㅇ라ㅣ니ㅏㄹ아ㅣㅁ니ㅏㅇ라ㅣㄴ이라ㅣㅏㄴ이라ㅣ낭리ㅏ니                         ㅏㅇ라ㅣ니ㅏㅇ리니ㅏㅇ리ㅏ니ㅏㅇㄹ", vTest);
        Sleep(200);

        pChar->ReqSendChatMsg(CHAT_CONST::CHAT_SYSTEM, " Tes t ", "CHAT_SYSTEM Test lksklfkl 가나다라나알낭라나 ＠♡ ㄴㄹ ㄴㄹㄴㅇ ㄹㄴ아러ㅏ니ㅏㅇ라ㅣ니ㅏㄹ아ㅣㅁ니ㅏㅇ라ㅣㄴ이라ㅣㅏㄴ이라ㅣ낭리ㅏ니ㅏㅇ라            ㅣ니ㅏㅇ리니ㅏㅇ리ㅏ니ㅏㅇㄹ", vTest);
        Sleep(200);
    }
}

bool GameCommand::Protocol(splitVector& SplitVec)
{
    size_t VecSize = SplitVec.size();
    if (VecSize < 2)
        return false;

    if (SplitVec[1] == "info")
    {
        m_pGaeaClient->PrintProtocolInfo();
    }
    else
    {

    }
    return true;
}

#define DEFINSTANCE_STRING(_aa) m_pGaeaClient->PrintMsgText(D3DCOLOR_XRGB(239, 228, 176), std::string(_aa));
bool GameCommand::Instance(splitVector& SplitVec)
{
    try
    {
        const unsigned int _nSize(SplitVec.size());
        const unsigned int _colorWarning(D3DCOLOR_XRGB(229, 12 ,35));
        GLMSG::NET_INSTANCE_GM_COMMAND _gmCommandMessage;

		bool bToField(false);
        switch ( _nSize )
        {
        case 1: // "/instance";
            {				
                DEFINSTANCE_STRING("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
                DEFINSTANCE_STRING("~~~~~instance command list~~~~~");
                DEFINSTANCE_STRING("use \"/instance [command] [param0] [param1]\"");
				DEFINSTANCE_STRING("\"showType\" show instance type list;\"");
                DEFINSTANCE_STRING("\"who <instanceMapID>\" show Player's list;");
                DEFINSTANCE_STRING("\"where <player's Name>\" show Player's location;");
                DEFINSTANCE_STRING("\"land <instanceMapID>\" show Instance's land;");				
                DEFINSTANCE_STRING("\"list <page, num>\" show Instance list;");
                DEFINSTANCE_STRING("\"log <keyMapID>\" show Instance log;");
                DEFINSTANCE_STRING("\"log\" show Instance log;");

				DEFINSTANCE_STRING("\"setGlobal <variable, key, value>\" set value of map;");
				DEFINSTANCE_STRING("\"setGlobal <variable, value>\" set value of a variable;");

				DEFINSTANCE_STRING("\"getGlobal <variable>\" show value of variable;");
				DEFINSTANCE_STRING("\"getGlobal <instanceMapID> <variable>\" show value of variable in pointed Instance;");
				DEFINSTANCE_STRING("\"moveToInstance <instanceMapID>\" field to instance;");
				DEFINSTANCE_STRING("\"moveToInstance <player's Name, instanceMapID>\" field to instance Player;");
				DEFINSTANCE_STRING("\"moveToInstance <mID sID x, z>\" instance to instance;");
				DEFINSTANCE_STRING("\"moveToInstance <mID sID nGateID>\" instance to instance Gate;");
				DEFINSTANCE_STRING("\"moveToInstance <player's Name, mID sID x, z>\" instance to instance Player;");				
				DEFINSTANCE_STRING("\"moveToField <mID sID x, z>\" instance to field;");
				DEFINSTANCE_STRING("\"moveTo <player's Name, x, z>\" move to Player;");
				DEFINSTANCE_STRING("\"observe <instanceMapID>;\" observe Instance");

				DEFINSTANCE_STRING("\"doJoin <player's Name>\" join Player;");
				DEFINSTANCE_STRING("\"doJoin <player's Name, instanceMapID>\" join Player at instanceMapID;");
				DEFINSTANCE_STRING("\"doOut <player's Name>\" dismiss Player;");
				DEFINSTANCE_STRING("\"doOut <player's Name, instanceMapID>\" dismiss Player at instanceMapID;");

                DEFINSTANCE_STRING("\"destroy <instanceMapID>\" destroy Instance;");
                DEFINSTANCE_STRING("\"destroy <keyMapID>\" destroy Instance;");
                DEFINSTANCE_STRING("\"destroy\" destroy whole Instance;");
                DEFINSTANCE_STRING("\"create <keyMapID>\" create Instance;");
                DEFINSTANCE_STRING("\"off <keyMapID>\" turn off Instance;");
                DEFINSTANCE_STRING("\"on <keyMapID>\" turn on Instance;");

				DEFINSTANCE_STRING("\"suspendJoin\" suspend to Join current Instance;");
				DEFINSTANCE_STRING("\"suspendJoin <instanceMapID>\" suspend to Join Instance;");
				DEFINSTANCE_STRING("\"unlockJoin\" resume to Join current Instance;");				
				DEFINSTANCE_STRING("\"unlockJoin <instanceMapID>\" resume to Join Instance;");

				DEFINSTANCE_STRING("\"lockJoin\" lock to Join current Instance;");
				DEFINSTANCE_STRING("\"lockJoin <instanceMapID>\" lock to Join Instance;");
				DEFINSTANCE_STRING("\"lockJoin <keyMapID>\" lock to Join type;");
				DEFINSTANCE_STRING("\"unlockJoin\" unlock to Join current Instance;");
				DEFINSTANCE_STRING("\"unlockJoin <instanceMapID>\" unlock to Join Instance;");
				DEFINSTANCE_STRING("\"unlockJoin <keyMapID>\" unlock to Join type;");                
                DEFINSTANCE_STRING("\"reload\" reload Instance Script;");
                DEFINSTANCE_STRING("~~~~~~~~~~~~~[end]~~~~~~~~~~~~~");
                DEFINSTANCE_STRING("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
            }
            return true;
        case 2:
            {
                const std::string _command(SplitVec[1]);
				if ( _command == "debug" )
				{
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_DEBUG;
                    _gmCommandMessage.nParameter0 = 0;
					m_pGaeaClient->NETSENDTOFIELD(&_gmCommandMessage);
					m_pGaeaClient->NETSENDTOAGENT(&_gmCommandMessage);
					
					const SNATIVEID& _gaeaMapID(m_pGaeaClient->GetActiveMapID().getGaeaMapID());
					const SNATIVEID& _baseMapID(m_pGaeaClient->GetActiveMapID().getBaseMapID());					
					DEFINSTANCE_STRING("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
					DEFINSTANCE_STRING(sc::string::format("client : CurrentMap[gaea : %1%/%2%, base : %3%/%4%];", _gaeaMapID.Mid(), _gaeaMapID.Sid(), _baseMapID.Mid(), _baseMapID.Sid()));
					return true;
				}
                else if ( _command == "reload" )
                { // script reload;
                    _gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_RELOAD;
                }				
                else if ( _command == "list" )
                { // nParameter0 = number of Page; // 인던 목록 보기;
                    _gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_LIST;
                    _gmCommandMessage.nParameter0 = 0;
                    _gmCommandMessage.nParameter1 = 20;
                }
                else if ( _command == "log" )
                { // nParameter0 = instanceID; // 인던 기록 보기;
                    _gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_LOG;
                    _gmCommandMessage.nParameter0 = -1;
					_gmCommandMessage.nParameter1 = -1;
                }
				else if ( _command == "suspendJoin" )
				{ // nParameter0 = instanceMapID; // 인던 접속 지연;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_JOIN_SUSPEND;
					_gmCommandMessage.nParameter0 = -1;
					_gmCommandMessage.nParameter1 = TRUE;
				}
				else if ( _command == "resumeJoin" )
				{ // nParameter0 = instanceMapID; // 인던 접속 지연 해제;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_JOIN_SUSPEND;
					_gmCommandMessage.nParameter0 = -1;
					_gmCommandMessage.nParameter1 = FALSE;
				}
				else if ( _command == "lockJoin" )
				{ // nParameter0 = instanceMapID; // 인던 접속 잠금;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_JOIN_LOCK;
					_gmCommandMessage.nParameter0 = -1;
					_gmCommandMessage.nParameter1 = TRUE;
				}
				else if ( _command == "unlockJoin" )
				{ // nParameter0 = instanceMapID; // 인던 접속 잠금 해제;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_JOIN_LOCK;
					_gmCommandMessage.nParameter0 = -1;
					_gmCommandMessage.nParameter1 = FALSE;
				}
                else if ( _command == "destroy" )
                {
                    _gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_DESTROY_TYPE;
                    _gmCommandMessage.nParameter0 = -1;
					_gmCommandMessage.nParameter1 = -1;
                }
				else if ( _command == "showType" )
				{
					m_pGaeaClient->showInstanceType();
					return true;
				}
				else if ( _command == "doOut" )
				{
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_DO_OUT;
					StringCchCopy(_gmCommandMessage.szParameter0, GLMSG::NET_INSTANCE_GM_COMMAND::EMPARAMETER_LENGTH - 1, m_pGaeaClient->GetCharacter()->GetName());
					_gmCommandMessage.nParameter0 = InstanceSystem::InstanceMapID(-1);
					bToField = true;
				}
				else if ( _command == "countPacket" )
				{
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_COUNT_PACKET;
				}
                else
                {
                    m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
                    return true;
                }
            }
            break;
        case 3:
            {
                const std::string _command(SplitVec[1]);
                const std::string _param(SplitVec[2]);
                if ( _command == "who" )
                { // nParameter0 = instanceMapID; 인던 내 플레이어 목록 보기;
                    _gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_WHO;
                    _gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param);
                }
                else if ( _command == "where" )
                { // szParameter0 = player's name; 특정 플레이어의 위치 보기;
                    _gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_WHERE;
					StringCchCopy(_gmCommandMessage.szParameter0, GLMSG::NET_INSTANCE_GM_COMMAND::EMPARAMETER_LENGTH - 1, _param.c_str());
                }
                else if ( _command == "land" )
                { // nParameter0 = instanceMapID; // 인던 목록 보기;
                    _gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_LAND;
                    _gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param);
                }
                else if ( _command == "list" )
                { // nParameter0 = number of Page; // 인던 목록 보기;
                    _gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_LIST;
                    _gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param);
					_gmCommandMessage.nParameter1 = 20;
                }
				else if ( _command == "getGlobal" )
				{ // 스크립트 내부의 변수 값 확인;
					// nParameter0 = instanceMapID;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_GET_GLOBAL;
					_gmCommandMessage.nParameter0 = -1;
					StringCchCopy(_gmCommandMessage.szParameter0, GLMSG::NET_INSTANCE_GM_COMMAND::EMPARAMETER_LENGTH - 1, _param.c_str());
					bToField = true;
				}
				else if ( _command == "moveToInstance" )
				{ // 외부에서 인던의 특정 장소로 이동;
					// nParamater0 = instanceMapID;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_ENTER_TO_INSTANCE;
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param);
					StringCchCopy(_gmCommandMessage.szParameter0, GLMSG::NET_INSTANCE_GM_COMMAND::EMPARAMETER_LENGTH - 1, m_pGaeaClient->GetCharacter()->GetName());					
				}
                else if ( _command == "destroy" )
                {// nParameter0 = instanceMapID; // 특정 인던 삭제;
                    _gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_DESTROY;
                    _gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param);
                }
				else if ( _command == "suspendJoin" )
				{ // nParameter0 = instanceMapID; // 인던 접속 지연;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_JOIN_SUSPEND;
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param);
					_gmCommandMessage.nParameter1 = TRUE;
				}
				else if ( _command == "resumeJoin" )
				{ // nParameter0 = instanceMapID; // 인던 접속 지연 해제;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_JOIN_SUSPEND;
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param);
					_gmCommandMessage.nParameter1 = FALSE;
				}
                else if ( _command == "lockJoin" )
                { // nParameter0 = instanceMapID; // 인던 접속 잠금;
                    _gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_JOIN_LOCK;
                    _gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param);
                    _gmCommandMessage.nParameter1 = TRUE;
                }
                else if ( _command == "unlockJoin" )
                { // nParameter0 = instanceMapID; // 인던 접속 잠금 해제;
                    _gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_JOIN_LOCK;
                    _gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param);
                    _gmCommandMessage.nParameter1 = FALSE;
                }
				else if ( _command == "doJoin" )
				{ // szParameter0 = Player's Name;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_DO_JOIN;
					StringCchCopy(_gmCommandMessage.szParameter0, GLMSG::NET_INSTANCE_GM_COMMAND::EMPARAMETER_LENGTH - 1, _param.c_str());
					_gmCommandMessage.nParameter0 = InstanceSystem::InstanceMapID(-1);
					bToField = true;
				}
				else if ( _command == "doOut" )
				{ // szParameter0 = Player's Name;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_DO_OUT;
					StringCchCopy(_gmCommandMessage.szParameter0, GLMSG::NET_INSTANCE_GM_COMMAND::EMPARAMETER_LENGTH - 1, _param.c_str());
					_gmCommandMessage.nParameter0 = InstanceSystem::InstanceMapID(-1);
					bToField = true;
				}
                else if ( _command == "debug" )
                {
                    _gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_DEBUG;
                    _gmCommandMessage.nParameter0 = -1;
                    StringCchCopy(_gmCommandMessage.szParameter0, GLMSG::NET_INSTANCE_GM_COMMAND::EMPARAMETER_LENGTH - 1, _param.c_str());
                    m_pGaeaClient->NETSENDTOFIELD(&_gmCommandMessage);
                    m_pGaeaClient->NETSENDTOAGENT(&_gmCommandMessage);

					const SNATIVEID& _gaeaMapID(m_pGaeaClient->GetActiveMapID().getGaeaMapID());
					const SNATIVEID& _baseMapID(m_pGaeaClient->GetActiveMapID().getBaseMapID());					
					DEFINSTANCE_STRING("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
					DEFINSTANCE_STRING(sc::string::format("client : CurrentMap[gaea : %1%/%2%, base : %3%/%4%];", _gaeaMapID.Mid(), _gaeaMapID.Sid(), _baseMapID.Mid(), _baseMapID.Sid()));
                    return true;
                }
				else if ( _command == "observe" )
				{
					const DWORD _instanceMapID(boost::lexical_cast<unsigned int>(_param));
					GLMSG::NET_INSTANCE_REQUEST_OBSERVE _requestObserve;
					_requestObserve.instanceMapIDTargetObserve = _instanceMapID;
					m_pGaeaClient->NETSENDTOAGENT(&_requestObserve);
					return true;
				}
                else
                {
                    m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
                    return true;
                }
            }
            break;
        case 4:
            {
                const std::string _command(SplitVec[1]);
                const std::string _param0(SplitVec[2]);
                const std::string _param1(SplitVec[3]);
                if ( _command == "list" )
                { // nParameter0 = number of Page; // 인던 목록 보기;
                    _gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_LIST;
                    _gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
                    _gmCommandMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param1);
                }
				else if ( _command == "log" )
				{ // 인던 로그 보기;
					// nParameter0 = keyMapMainID;
					// nParameter1 = keyMapSubID;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_LOG;
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
					_gmCommandMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param1);
				}
				else if ( _command == "destroy" )
				{ // 해당 type의 인던 모두 삭제;
					// nParameter0 = keyMapMainID;
					// nParameter1 = keyMapSubID;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_DESTROY_TYPE;
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
					_gmCommandMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param1);
				}				
				else if ( _command == "create" )
				{ // 인던 생성 -> 입장;
					// nParameter0 = keyMapMainID; 
					// nParameter1 = keyMapSubID;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_CREATE;
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
					_gmCommandMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param1);
				}
				else if ( _command == "moveToInstance" )
				{ // 외부에서 인던의 특정 장소로 이동;
					// szParameter0 = player's Name;
					// nParamater1 = instanceMapID;					
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_ENTER_TO_INSTANCE;
					StringCchCopy(_gmCommandMessage.szParameter0, GLMSG::NET_INSTANCE_GM_COMMAND::EMPARAMETER_LENGTH - 1, _param0.c_str());
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param1);					
				}
				else if ( _command == "setGlobal" )
				{ // 스크립트 내부 변수 값 변경;					
					// szParameter0 = globalKey Name;
					// nParameter1 = value;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_SET_GLOBAL;
					StringCchCopy(_gmCommandMessage.szParameter0, GLMSG::NET_INSTANCE_GM_COMMAND::EMPARAMETER_LENGTH - 1, _param0.c_str());
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param1);
					bToField = true;
				}
				else if ( _command == "getGlobal" )
				{ // 스크립트 내부의 변수 값 확인;
					// nParameter0 = instanceMapID;
					// szParameter0 = globalKey Name;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_GET_GLOBAL;
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
					StringCchCopy(_gmCommandMessage.szParameter0, GLMSG::NET_INSTANCE_GM_COMMAND::EMPARAMETER_LENGTH - 1, _param1.c_str());
					bToField = true;
				}
				else if ( _command == "lockJoin" )
				{ // 인던 접속 잠금;
					// nParameter0 = keyMapMainID;
					// nParameter1 = keyMapSubID;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_JOIN_LOCK_TYPE;
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
					_gmCommandMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param1);
					_gmCommandMessage.nParameter2 = TRUE;
				}
				else if ( _command == "unlockJoin" )
				{ // 인던 접속 잠금 해제;
					// nParameter0 = keyMapMainID;
					// nParameter1 = keyMapSubID;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_JOIN_LOCK_TYPE;
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
					_gmCommandMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param1);
					_gmCommandMessage.nParameter2 = FALSE;
				}
				else if ( _command == "on" )
				{ // 인던 켬(생성 가능);
					// nParameter0 = keyMapMainID;
					// nParameter1 = keyMapSubID;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_ON;
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
					_gmCommandMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param1);
				}
				else if ( _command == "off" )
				{ // 인던 끔(생성 불가) _ 기존 인던을 지우지는 않음;
					// nParamater0 = keyMapMainID;
					// nParameter1 = keyMapSubID;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_OFF;
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
					_gmCommandMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param1);
				}
				else if ( _command == "doJoin" )
				{ // szParameter0 = Player's Name;
					// nParameter0 = instanceMapID;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_DO_JOIN;
					StringCchCopy(_gmCommandMessage.szParameter0, GLMSG::NET_INSTANCE_GM_COMMAND::EMPARAMETER_LENGTH - 1, _param0.c_str());
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param1);
					bToField = true;
				}
				else if ( _command == "doOut" )
				{ // szParameter0 = Player's Name;
					// nParameter0 = instanceMapID;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_DO_OUT;
					StringCchCopy(_gmCommandMessage.szParameter0, GLMSG::NET_INSTANCE_GM_COMMAND::EMPARAMETER_LENGTH - 1, _param0.c_str());
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param1);
					bToField = true;
				}
				else
				{
					m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
					return true;
				}
            }
            break;
		case 5:			
			{
				const std::string _command(SplitVec[1]);
				const std::string _param0(SplitVec[2]);
				const std::string _param1(SplitVec[3]);
				const std::string _param2(SplitVec[4]);
				if ( _command == "moveTo" )
				{ // 특정 위치로 움직여서 이동;
					// szParameter0 = player's Name;
					// nParameter0 = targetPositionX;
					// nParameter1 = targetPositionZ;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_MOVE_TO;
                    StringCchCopy(_gmCommandMessage.szParameter0, GLMSG::NET_INSTANCE_GM_COMMAND::EMPARAMETER_LENGTH - 1, _param0.c_str());
					_gmCommandMessage.fParameter0 = boost::lexical_cast<float>(_param1);
					_gmCommandMessage.fParameter1 = boost::lexical_cast<float>(_param2);
					bToField = true;
				}
				else if ( _command == "moveToInstance" )
				{ // 특정 게이트 위치로 이동;					
					// nParameter0 = targetMapMainID;
					// nParameter1 = targetMapSubID;
					// nParameter2 = gateID;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_MOVE_TO_GATE;
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
					_gmCommandMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param1);
					_gmCommandMessage.nParameter2 = boost::lexical_cast<unsigned int>(_param2);
					bToField = true;
				}
				else if ( _command == "create" )
				{ // 인던 생성 -> 입장;
					// nParameter0 = keyMapMainID; 
					// nParameter1 = keyMapSubID;
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_CREATE_TEST;
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
					_gmCommandMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param1);
					_gmCommandMessage.nParameter2 = boost::lexical_cast<unsigned int>(_param2);
				}
				else
				{
					m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
					return true;
				}
			}
			break;
		case 6:
			{
				const std::string _command(SplitVec[1]);
				const std::string _param0(SplitVec[2]);
				const std::string _param1(SplitVec[3]);
				const std::string _param2(SplitVec[4]);
				const std::string _param3(SplitVec[5]);
				if ( _command == "moveToInstance" )
				{ // 인던에서 인던의 특정 장소로 이동;
					// szParameter0 = player's Name;
					// nParamater0 = targetMapMainID;
					// nParameter1 = targetMapSubID;
					// fParameter2 = targetPositionX;
					// fParameter3 = targetPositionZ;					
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_MOVE_TO_INSTANCE;
					StringCchCopy(_gmCommandMessage.szParameter0, GLMSG::NET_INSTANCE_GM_COMMAND::EMPARAMETER_LENGTH - 1, m_pGaeaClient->GetCharacter()->GetName());
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
					_gmCommandMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param1);
					_gmCommandMessage.fParameter2 = boost::lexical_cast<float>(_param2);
					_gmCommandMessage.fParameter3 = boost::lexical_cast<float>(_param3);					
					bToField = true;
				}
				else if ( _command == "moveToField" )
				{ // 인던에서 외부의 특정 장소로 이동;
					// szParameter0 = player'sName;
					// nParamater0 = targetMapMainID;
					// nParameter1 = targetMapSubID;
					// fParameter2 = targetPositionX;
					// fParameter3 = targetPositionZ;					
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_MOVE_TO_FIELD;
					StringCchCopy(_gmCommandMessage.szParameter0, GLMSG::NET_INSTANCE_GM_COMMAND::EMPARAMETER_LENGTH - 1, m_pGaeaClient->GetCharacter()->GetName());
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
					_gmCommandMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param1);
					_gmCommandMessage.fParameter2 = boost::lexical_cast<float>(_param2);
					_gmCommandMessage.fParameter3 = boost::lexical_cast<float>(_param3);					
					bToField = true;
				}				
				else
				{
					m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
					return true;
				}
			}
			break;
		case 7:
			{
				const std::string _command(SplitVec[1]);
				const std::string _param0(SplitVec[2]);
				const std::string _param1(SplitVec[3]);
				const std::string _param2(SplitVec[4]);
				const std::string _param3(SplitVec[5]);
				const std::string _param4(SplitVec[6]);
				if ( _command == "moveToInstance" )
				{ // 인던에서 인던의 특정 장소로 이동;
					// szParameter0 = player'sName;
					// nParamater0 = targetMapMainID;
					// nParameter1 = targetMapSubID;
					// fParameter2 = targetPositionX;
					// fParameter3 = targetPositionZ;					
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_MOVE_TO_INSTANCE;
					StringCchCopy(_gmCommandMessage.szParameter0, GLMSG::NET_INSTANCE_GM_COMMAND::EMPARAMETER_LENGTH - 1, _param0.c_str());
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param1);
					_gmCommandMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param2);
					_gmCommandMessage.fParameter2 = boost::lexical_cast<float>(_param3);
					_gmCommandMessage.fParameter3 = boost::lexical_cast<float>(_param4);					
					bToField = true;
				}
				else if ( _command == "moveToField" )
				{ // 인던에서 외부의 특정 장소로 이동;
					// szParameter0 = player'sName;
					// nParamater0 = targetMapMainID;
					// nParameter1 = targetMapSubID;
					// fParameter2 = targetPositionX;
					// fParameter3 = targetPositionZ;					
					_gmCommandMessage.emCommandType = InstanceSystem::EMCOMMAND_MOVE_TO_FIELD;
					StringCchCopy(_gmCommandMessage.szParameter0, GLMSG::NET_INSTANCE_GM_COMMAND::EMPARAMETER_LENGTH - 1, _param0.c_str());
					_gmCommandMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param1);
					_gmCommandMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param2);
					_gmCommandMessage.fParameter2 = boost::lexical_cast<float>(_param3);
					_gmCommandMessage.fParameter3 = boost::lexical_cast<float>(_param4);					
					bToField = true;
				}				
				else
				{
					m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
					return true;
				}
			}
			break;
        default:
            m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
            return true;
        }

		bToField == true ? m_pGaeaClient->NETSENDTOFIELD(&_gmCommandMessage) : m_pGaeaClient->NETSENDTOAGENT(&_gmCommandMessage);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::MiniGame(splitVector& SplitVec)
{
    try
    {
        size_t VecSize = SplitVec.size();
        if (VecSize < 2)
            return false;

        if (SplitVec[1] == "oddeven")
        {
            m_pGaeaClient->GetMiniGameManager()->RequestOpen(MiniGame::EMMINIGAME_ODDEVEN);
        }
        else
        {

        }
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::DPS(splitVector& SplitVec)
{
	try
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_HotKey_DPS );

		return true;
	}
	catch (boost::bad_lexical_cast& e)
	{
		m_pGaeaClient->PrintConsoleText(e.what());
		return true;
	}
}

bool GameCommand::DbUnlock(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() < 2)
        {
            m_pGaeaClient->PrintConsoleText("/dbunlock [user db number]");
            return true;
        }

        DWORD UserDbNum = boost::lexical_cast<DWORD> (SplitVec[1]);
        m_pGaeaClient->DbUnlockUser(UserDbNum);
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::GuidanceInfo(splitVector& SplitVec)
{
	enum
	{
		END = 0,
		START = 1,
		ALL_START = 2,
		ALL_END = 3,
	};

    try
    {
        if (SplitVec.size() < 2)
        {
            m_pGaeaClient->ReqGuidanceInfo();            
        }

		if (SplitVec.size() == 2)
		{
			if (SplitVec[1] == "on")
			{
				m_pGaeaClient->ReqGuidanceOnOff(0, ALL_START);
			}
			else if (SplitVec[1] == "off")
			{
				m_pGaeaClient->ReqGuidanceOnOff(0, ALL_END);
			}
			else
			{
			}           
		}

        if (SplitVec.size() == 3)
        {
			DWORD GuidId    = boost::lexical_cast<DWORD> (SplitVec[1]); // Guidance id
			
			if (SplitVec[2] == "on")
			{
				m_pGaeaClient->ReqGuidanceOnOff(GuidId, START);
			}
			else if (SplitVec[2] == "off")
			{
				m_pGaeaClient->ReqGuidanceOnOff(GuidId, END);
			}
			else
			{
				DWORD ClubDbNum = boost::lexical_cast<DWORD> (SplitVec[2]); // Club db num
				m_pGaeaClient->ReqGuidanceChange(GuidId, ClubDbNum);  
			}          
        }
        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());
        return true;
    }
}

bool GameCommand::ShowContentId(splitVector& SplitVec)
{
    GLOBAL_PARAM::bShowContentID = !GLOBAL_PARAM::bShowContentID;
    if (GLOBAL_PARAM::bShowContentID)
        m_pGaeaClient->PrintConsoleText("Show content id : ON");
    else
        m_pGaeaClient->PrintConsoleText("Show content id : OFF");
    return true;
}

bool GameCommand::NetNsgDelay( splitVector& SplitVec )
{
	try
	{
		size_t VecSize = SplitVec.size();
		if ( VecSize < 2 )
		{
			return false;
		}

		if ( SplitVec[1] == "help" )
		{
			m_pGaeaClient->PrintConsoleText( "/net_msg_delay [type] [second] ex) /net_msg_delay 1 2.0 " );
			m_pGaeaClient->PrintConsoleText( "type 1 is character locker save msg delay" );
			m_pGaeaClient->PrintConsoleText( "type 2 is character locker withdraw msg delay" );
		}
		else if ( SplitVec[1] == "1" )
		{
			if ( VecSize == 3 )
			{
				GLMSG::SNET_GM_MSG_DELAY_MODIFY NetMsgCA;
				NetMsgCA.nMsgType = 1;
				NetMsgCA.fSecond = boost::lexical_cast< float >( SplitVec[2] );
				NetMsgCA.fSecond = abs( NetMsgCA.fSecond );
				m_pGaeaClient->NETSENDTOAGENT( &NetMsgCA );
				m_pGaeaClient->PrintConsoleText(
					sc::string::format("character locker save msg delay set %f second.", NetMsgCA.fSecond ) );
			}
		}
		else if ( SplitVec[1] == "2" )
		{
			if ( VecSize == 3 )
			{
				GLMSG::SNET_GM_MSG_DELAY_MODIFY NetMsgCA;
				NetMsgCA.nMsgType = 2;
				NetMsgCA.fSecond = boost::lexical_cast< float >( SplitVec[2] );
				NetMsgCA.fSecond = abs( NetMsgCA.fSecond );
				m_pGaeaClient->NETSENDTOAGENT( &NetMsgCA );
				m_pGaeaClient->PrintConsoleText(
					sc::string::format("character locker withdraw msg delay set %f second.", NetMsgCA.fSecond ) );
			}
		}
		else if ( SplitVec[1] == "19771129" )
		{
			if ( VecSize == 4 )
			{
				GLMSG::SNET_GM_MSG_DELAY_MODIFY NetMsgCA;
				NetMsgCA.nMsgType = boost::lexical_cast< int >( SplitVec[2] );
				NetMsgCA.fSecond = boost::lexical_cast< float >( SplitVec[3] );
				NetMsgCA.fSecond = abs( NetMsgCA.fSecond );
				m_pGaeaClient->NETSENDTOAGENT( &NetMsgCA );
				m_pGaeaClient->PrintConsoleText(
					sc::string::format("%d msg delay set %f second.", NetMsgCA.nMsgType, NetMsgCA.fSecond ) );
			}
		}
		else
		{
			m_pGaeaClient->PrintConsoleText( "/net_msg_delay [type] [second] ex) /net_msg_delay 1 2.0 " );
			m_pGaeaClient->PrintConsoleText( "type 1 is character locker save msg delay" );
			m_pGaeaClient->PrintConsoleText( "type 2 is character locker withdraw msg delay" );
		}

		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what( ));
		return true;
	}
}

bool GameCommand::PrivateMarket( splitVector& SplitVec )
{
	try
	{
		size_t VecSize = SplitVec.size();
		if ( VecSize < 2 )
		{
			return false;
		}

		if ( SplitVec[1] == "state" )
		{
			GLMSG::SNETPC_PMARKET_STATE_CF NetMsgCF;
			m_pGaeaClient->NETSENDTOFIELD( &NetMsgCF );
		}
		
		if ( VecSize < 3 )
		{
			return false;
		}
		
		if ( SplitVec[1] == "search_delay" )
		{
			GLMSG::SNETPC_PMARKET_OPTION_CA NetMsgCA;
			NetMsgCA.eOptionType = private_market::eCommandOptionTypeSearchDelay;
			NetMsgCA.fValue = boost::lexical_cast< float >( SplitVec[2] );
			m_pGaeaClient->NETSENDTOAGENT( &NetMsgCA );
			m_pGaeaClient->PrintConsoleText( sc::string::format( "private market current search delay = %f", NetMsgCA.fValue ) );
		}
		else if ( SplitVec[1] == "log_delay" )
		{
			GLMSG::SNETPC_PMARKET_OPTION_CA NetMsgCA;
			NetMsgCA.eOptionType = private_market::eCommandOptionTypeLogDelay;
			NetMsgCA.fValue = boost::lexical_cast< float >( SplitVec[2] );
			m_pGaeaClient->NETSENDTOAGENT( &NetMsgCA );
			m_pGaeaClient->PrintConsoleText( sc::string::format( "private market current log delay = %f", NetMsgCA.fValue ) );
		}
		else if ( SplitVec[1] == "channel_check" )
		{
			GLMSG::SNETPC_PMARKET_OPTION_CA NetMsgCA;
			NetMsgCA.eOptionType = private_market::eCommandOptionTypeCheckChannel;

			if ( SplitVec[2] == "on" )
			{
				NetMsgCA.bValue = true;
				m_pGaeaClient->NETSENDTOAGENT( &NetMsgCA );
				m_pGaeaClient->PrintConsoleText( "private market channel check on." );
			}
			else if ( SplitVec[2] == "off" )
			{
				NetMsgCA.bValue = false;
				m_pGaeaClient->NETSENDTOAGENT( &NetMsgCA );
				m_pGaeaClient->PrintConsoleText( "private market channel check off." );
			}
			else
			{
				m_pGaeaClient->PrintConsoleText( "parameter is on or off. ex> /pm channel_check on" );
			}
		}
		else if ( SplitVec[1] == "school_check" )
		{
			GLMSG::SNETPC_PMARKET_OPTION_CA NetMsgCA;
			NetMsgCA.eOptionType = private_market::eCommandOptionTypeCheckSchool;

			if ( SplitVec[2] == "on" )
			{
				NetMsgCA.bValue = true;
				m_pGaeaClient->NETSENDTOAGENT( &NetMsgCA );
				m_pGaeaClient->PrintConsoleText( "private market school check on." );
			}
			else if ( SplitVec[2] == "off" )
			{
				NetMsgCA.bValue = false;
				m_pGaeaClient->NETSENDTOAGENT( &NetMsgCA );
				m_pGaeaClient->PrintConsoleText( "private market school check off." );
			}
			else
			{
				m_pGaeaClient->PrintConsoleText( "parameter is on or off. ex> /pm school_check on" );
			}
		}
		else if ( SplitVec[1] == "commission" )
		{
			GLMSG::SNETPC_PMARKET_OPTION_CA NetMsgCA;
			NetMsgCA.eOptionType = private_market::eCommandOptionTypeCommission;

			if ( SplitVec[2] == "on" )
			{
				NetMsgCA.bValue = true;
				m_pGaeaClient->NETSENDTOAGENT( &NetMsgCA );
				m_pGaeaClient->PrintConsoleText( "private market commission on." );
			}
			else if ( SplitVec[2] == "off" )
			{
				NetMsgCA.bValue = false;
				m_pGaeaClient->NETSENDTOAGENT( &NetMsgCA );
				m_pGaeaClient->PrintConsoleText( "private market commission off." );
			}
			else
			{
				m_pGaeaClient->PrintConsoleText( "parameter is on or off. ex> /pm commission on" );
			}
		}
		else if ( SplitVec[1] == "all_page" )
		{
			GLMSG::SNETPC_PMARKET_OPTION_CA NetMsgCA;
			NetMsgCA.eOptionType = private_market::eCommandOptionTypeAllPage;

			if ( SplitVec[2] == "on" )
			{
				NetMsgCA.bValue = true;
				m_pGaeaClient->NETSENDTOAGENT( &NetMsgCA );
				m_pGaeaClient->PrintConsoleText( "private market all page on." );
			}
			else if ( SplitVec[2] == "off" )
			{
				NetMsgCA.bValue = false;
				m_pGaeaClient->NETSENDTOAGENT( &NetMsgCA );
				m_pGaeaClient->PrintConsoleText( "private market all page off." );
			}
			else
			{
				m_pGaeaClient->PrintConsoleText( "parameter is on or off. ex> /pm all_page on" );
			}
		}
		else if ( SplitVec[1] == "search_option" )
		{
			GLMSG::SNETPC_PMARKET_OPTION_CA NetMsgCA;
			NetMsgCA.eOptionType = private_market::eCommandOptionTypeSearchOption;

			int SearchOption = boost::lexical_cast< int >( SplitVec[2] );
			switch ( SearchOption )
			{
			case 0:
			case 1:
			case 2:
				{
					NetMsgCA.SearchOption = SearchOption;
					m_pGaeaClient->NETSENDTOAGENT( &NetMsgCA );
					m_pGaeaClient->PrintConsoleText( sc::string::format( "private market search option set to %1%.", SearchOption ) );
				}
				break;
			default:
				{
					m_pGaeaClient->PrintConsoleText( sc::string::format( "private market search option setting error. %1%", SearchOption ) );
				}
				break;
			}
		}

		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what( ));
		return true;
	}
}

bool GameCommand::CDMOnOff( splitVector& SplitVec )
{
	enum
	{
		ONE = 0,
		ALL = 1,
	};

	try
	{
		if (SplitVec.size() == 2)
		{
			if (SplitVec[1] == "turn")
			{
				m_pGaeaClient->ReqClubDeathMatchOnOff(0, ALL);
			}
			else
			{
			}           
		}

		if (SplitVec.size() == 3)
		{
			DWORD GuidId    = boost::lexical_cast<DWORD> (SplitVec[1]); // Guidance id

			if (SplitVec[2] == "turn")
			{
				m_pGaeaClient->ReqClubDeathMatchOnOff(GuidId, ONE);
			}
			else
			{
				DWORD ClubDbNum = boost::lexical_cast<DWORD> (SplitVec[2]); // Club db num
				m_pGaeaClient->ReqClubDeathMatchOnOff(GuidId, ClubDbNum);  
			}          
		}
		return true;
	}
	catch (boost::bad_lexical_cast& e)
	{
		m_pGaeaClient->PrintConsoleText(e.what());
		return true;
	}
}

bool GameCommand::ReferenceCount( splitVector& SplitVec )
{
	try
	{
		size_t VecSize = SplitVec.size();
		if ( VecSize != 2 )
		{
			return false;
		}

		GLMSG::SNETPC_GM_REFERENCE_COUNT_CAF NetMsgCA;
		NetMsgCA.nCountMax = boost::lexical_cast< int >( SplitVec[1] );
		m_pGaeaClient->NETSENDTOAGENT( &NetMsgCA );
		m_pGaeaClient->PrintConsoleText( sc::string::format( "server reference count = %d", NetMsgCA.nCountMax ) );
		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what( ));
		return true;
	}
}

bool GameCommand::LoadIPEvent( splitVector& SplitVec )
{
    try
    {
        size_t VecSize = SplitVec.size();
        if ( VecSize != 1 )
        {
            return false;
        }

        // TODO : Request loading IPEvent.lua file.
        GLMSG::SNETPC_GM_LOAD_IPEVENT NetMsgCA;
        m_pGaeaClient->NETSENDTOAGENT( &NetMsgCA );

        m_pGaeaClient->PrintConsoleText( std::string( "request loading ipevent file" ) );

        return true;
    }
    catch ( boost::bad_lexical_cast& e )
    {
        m_pGaeaClient->PrintConsoleText( e.what( ));

        return true;
    }
}

bool GameCommand::SetIPEvent( splitVector& SplitVec )
{
    try
    {
        size_t VecSize = SplitVec.size();
        if ( VecSize != 2 )
        {
            return false;
        }

        bool bEnable = false;
        if ( SplitVec[1] == "on" )
            bEnable = true;

        else if ( SplitVec[1] == "off" )
            bEnable = false;
        else
            return false;

        // TODO : Request setting ipevent.
        GLMSG::SNETPC_GM_SET_IPEVENT NetMsg;
        NetMsg.bEnable = bEnable;
        m_pGaeaClient->NETSENDTOAGENT( &NetMsg );

        std::string strEnable;
        if ( bEnable )
            strEnable = "on";
        else
            strEnable = "off";
        m_pGaeaClient->PrintConsoleText( sc::string::format( "IPEvent %1%", strEnable ) );

        return true;
    }
    catch ( boost::bad_lexical_cast& e )
    {
        m_pGaeaClient->PrintConsoleText( e.what( ));

        return true;
    }
}

bool GameCommand::AcquireEXP( splitVector& SplitVec )
{
    try
    {
        size_t VecSize = SplitVec.size();
        if ( VecSize > 2 || VecSize < 1)
        {
            return false;
        }

        bool bIsToggle = false;
        bool bEnable = false;

        if( VecSize == 1 )
        {
            // 이 경우에는 Toggle 모드로 작동 

            bIsToggle = true;
        }
        else
        {
            if ( SplitVec[1] == "on" )
                bEnable = true;

            else if ( SplitVec[1] == "off" )
                bEnable = false;
            else
                return false;
        }

        // TODO : Request setting ipevent.
        GLMSG::SNETPC_GM_ACQUIRE_EXP NetMsg;
        NetMsg.bIsToggle = bIsToggle;
        NetMsg.bEnable = bEnable;
        m_pGaeaClient->NETSENDTOAGENT( &NetMsg );

        std::string strEnable;
        if ( bEnable )
            strEnable = "on";
        else
            strEnable = "off";

        if(!bIsToggle)
            m_pGaeaClient->PrintConsoleText( sc::string::format( "Acquire EXP %1%", strEnable ) );

        return true;
    }
    catch ( boost::bad_lexical_cast& e )
    {
        m_pGaeaClient->PrintConsoleText( e.what( ));

        return true;
    }
}

bool GameCommand::ReloadTrigger( splitVector& SplitVec )
{
	try
	{
		size_t VecSize = SplitVec.size();

		if (VecSize != 2 || m_pGaeaClient == 0)
		{
			return false;
		}

		bool bCnt = false;
		bool bSvr = false;

		if (SplitVec[1] == _T("server"))
		{
			bSvr = true;
		}
		else if (SplitVec[1] == _T("client"))
		{
			bCnt = true;
		}
		else if (SplitVec[1] == _T("all"))
		{
			bCnt = true;
			bSvr = true;
		}

		if (bCnt)
		{
			GLLandManClient* pLandMan = m_pGaeaClient->GetLandManClient();

			if (pLandMan && pLandMan->GetPVEIndunID().IsVaild())
			{
				m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, _T("인던 안에서는 클라이언트 트리거를 재로딩할 수 없습니다."));
			}
			else
			{
				TriggerSystem::DestroyClientTriggerSystem();				

				if (TriggerSystem::InitClientTriggerSystem())
				{
					m_pGaeaClient->GetPVEClient()->Init();
					m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, _T("클라이언트 트리거 재로딩에 성공 했습니다."));
				}
			}
		}

		if (bSvr)
		{
			GLCharacter* pAvatar = m_pGaeaClient->GetCharacter();

			if (pAvatar)
			{
				GLMSG::NET_PVE_GM_RELOAD_TRIGGER_REQ_CA_OR_AF NetMsg;
				NetMsg.dwCharDbNum = pAvatar->GetCharID();
				msgpack::sbuffer buf;
				msgpack::pack(buf, NetMsg);
				m_pGaeaClient->NETSENDTOAGENT(NET_MSG_PVE_GM_RELOAD_TRIGGER_REQ_CA_OR_AF, buf);
				m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, _T("서버에 트리거 재로딩을 요청 완료 했습니다."));
			}
		}

		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what( ));

		return true;
	}
}

bool GameCommand::GoStage( splitVector& SplitVec )
{
	try
	{
		size_t VecSize = SplitVec.size();

		if (VecSize != 2 || m_pGaeaClient == 0)
		{
			return false;
		}

		int nStage = boost::lexical_cast<int> (SplitVec[1]);

		GLCharacter* pAvatar = m_pGaeaClient->GetCharacter();

		if (pAvatar)
		{
			GLMSG::NET_PVE_GM_GOSTAGE_CF NetMsg;
			NetMsg.dwCharDbNum = pAvatar->GetCharID();
			NetMsg.nStage = nStage;
			msgpack::sbuffer buf;
			msgpack::pack(buf, NetMsg);
			m_pGaeaClient->NETSENDTOFIELD(NET_MSG_PVE_GM_GOSTAGE_REQ_CF, buf);
			m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, _T("서버에 스테이지 이동 요청을 완료 했습니다."));
		}

		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what( ));

		return true;
	}
}

bool GameCommand::Log2DB( splitVector& SplitVec )
{
    try
    {
        size_t VecSize = SplitVec.size();

        if (VecSize != 1)
        {
            return false;
        }

        m_pGaeaClient->PrintConsoleText(
            std::string("Txt2DB Insert OK."));

        GLMSG::SNET_GM_LOG_TO_DB_CAF NetMsg;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

        return true;
    }
    catch ( boost::bad_lexical_cast& e )
    {
        m_pGaeaClient->PrintConsoleText( e.what( ));

        return true;
    }
}

bool GameCommand::TexasHoldem( splitVector& SplitVec )
{
	try
	{
		size_t VecSize = SplitVec.size();

		if ( NULL == m_pGaeaClient )
		{
			return false;
		}
		
		if ( 2 == VecSize )
		{

			if ( _T( "start" ) == SplitVec[1] )
			{
				m_pGaeaClient->GetTexasHoldemManager()->StartGame();
			}
			else if ( _T( "end" ) == SplitVec[1] )
			{
				m_pGaeaClient->GetTexasHoldemManager()->EndGame();
			}
			else if ( _T( "open" ) == SplitVec[1] )
			{
				m_pGaeaClient->GetTexasHoldemManager()->OpenMainMenu();
			}
		}
		else if ( 3 == VecSize )
		{
			if ( _T( "buym" ) == SplitVec[1] )
			{
				LONGLONG chip = boost::lexical_cast< LONGLONG >( SplitVec[2] );
				m_pGaeaClient->GetTexasHoldemManager()->RequestChipPurchaseInGameMoney( chip );
			}
			else if ( _T( "buyp" ) == SplitVec[1] )
			{
				LONGLONG chip = boost::lexical_cast< LONGLONG >( SplitVec[2] );
				m_pGaeaClient->GetTexasHoldemManager()->RequestChipPurchaseInPoint( chip );
			}
		}
		else
		{
			return false;
		}

		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what( ));

		return true;
	}
}

// Lotto System ----------------------------------------------
bool GameCommand::LottoSystemID( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	unsigned int lottoSystemId = 0;

	if( SplitVec.size() != 2 )
	{
		m_pGaeaClient->PrintConsoleText( std::string( "Usage: /lotto_system_id OO" ) );

		return true;
	}
	else 
	{
		lottoSystemId = atoi( SplitVec[1].c_str() );

		if( LottoSystem::GLLottoSystemManClient::Instance()->
			GMCommandSetLottoSystemID( lottoSystemId ) )
			m_pGaeaClient->PrintConsoleText("Saved GM lotto system id.");
	}

	return true;
	
}

bool GameCommand::LottoViewSystemID( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	if( SplitVec.size() != 1 )
	{
		m_pGaeaClient->PrintConsoleText("Usage: /lotto_view_system_id");

		return true;
	}
	else 
	{
		LottoSystem::GLLottoSystemManClient::Instance()->
			GMCommandViewLottoSystemID();
	}

	return true;
}

bool GameCommand::LottoSystemChangeState( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	if( SplitVec.size() != 2 )
	{
		m_pGaeaClient->PrintConsoleText( std::string( "Usage: /lotto turn" ) );

		return true;
	}
	else 
	{
		std::string strPARAM_01 = SplitVec[1];			

		if( strPARAM_01 == "turn" )
		{
			if( LottoSystem::GLLottoSystemManClient::Instance()->GMCommandChangeState() )
				m_pGaeaClient->PrintConsoleText("Requested changing Lotto state.");
		}
		else
		{
			m_pGaeaClient->PrintConsoleText("Wrong use of /lotto. Try just /lotto to check the usage.");
			
			return false;
		}        
	}

	return true;
}

bool GameCommand::LottoSystemNumDSP( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	if( SplitVec.size() != 2 && SplitVec.size() != 1 )
	{
		m_pGaeaClient->PrintConsoleText("Usage: /lotto_num_dsp or /lotto_num_dsp OO");

		return true;
	}

	if( SplitVec.size() == 1 )
	{
		if( LottoSystem::GLLottoSystemManClient::Instance()
			->GMCommandNumDSP( 0 ) )
			m_pGaeaClient->PrintConsoleText("Requested lotto num DSP.");
	}
	else 
	{
		unsigned int num = atoi( SplitVec[1].c_str() );

		if( LottoSystem::GLLottoSystemManClient::Instance()
			->GMCommandNumDSP( num ) )
			m_pGaeaClient->PrintConsoleText("Requested lotto num DSP.");
	}

	return true;
}

bool GameCommand::LottoSystemNum( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	if( SplitVec.size() != 2 && SplitVec.size() != 1 )
	{
		m_pGaeaClient->PrintConsoleText("Usage: /lotto_num or /lotto_num OO");

		return true;
	}

	if( SplitVec.size() == 1 )
	{
		if( LottoSystem::GLLottoSystemManClient::Instance()
			->GMCommandNum( 0 ) )
			m_pGaeaClient->PrintConsoleText("Requested lotto num.");
	}
	else 
	{
		unsigned int num = atoi( SplitVec[1].c_str() );

		if( LottoSystem::GLLottoSystemManClient::Instance()
			->GMCommandNum( num ) )
			m_pGaeaClient->PrintConsoleText("Requested lotto num.");
	}

	return true;
}

bool GameCommand::LottoSystemNumInit( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	if( SplitVec.size() == 1 )
	{
		if( LottoSystem::GLLottoSystemManClient::Instance()
			->GMCommandNumInit() )
			m_pGaeaClient->PrintConsoleText("Complete to initialize lotto num");

		return true;
	}
	else
	{
		m_pGaeaClient->PrintConsoleText("Wrong use of /lotto. Try just /lotto to check the usage.");

		return false;
	}

	return true;
}

bool GameCommand::LottoSystemNumAdd( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	unsigned int num = 0;
	unsigned int count = 0;

	if( SplitVec.size() != 3 )
	{
		m_pGaeaClient->PrintConsoleText( std::string( "Usage: /lotto_system_add OO OO" ) );

		return true;
	}
	else 
	{
		num = atoi( SplitVec[1].c_str() );
		count = atoi( SplitVec[2].c_str() );

		if( LottoSystem::GLLottoSystemManClient::Instance()->
			GMCommandNumAdd( num, count ) )
			m_pGaeaClient->PrintConsoleText("Added lotto num count.");     
	}

	return true;
}

bool GameCommand::LottoSystemNumDel( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	unsigned int num = 0;
	unsigned int count = 0;

	if( SplitVec.size() != 3 )
	{
		m_pGaeaClient->PrintConsoleText( std::string( "Usage: /lotto_system_del OO OO" ) );

		return true;
	}
	else 
	{
		num = atoi( SplitVec[1].c_str() );
		count = atoi( SplitVec[2].c_str() );

		if( LottoSystem::GLLottoSystemManClient::Instance()->
			GMCommandNumDel( num, count ) )
			m_pGaeaClient->PrintConsoleText("Deleted lotto num count.");     
	}

	return true;
}

bool GameCommand::LottoSystemViewDrawingType( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	if( SplitVec.size() == 1 )
	{
		LottoSystem::GLLottoSystemManClient::Instance()->
			GMCommandViewDrawingType();

		return true;
	}
	else 
	{
		m_pGaeaClient->PrintConsoleText(
			std::string( "Usage: /lotto_type" ) );
	}

	return true;
}

bool GameCommand::LottoSystemSetWinNum( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	std::string strWinNum;

	if( SplitVec.size() == 2 )
	{
		strWinNum = SplitVec[1].c_str();

		if( LottoSystem::GLLottoSystemManClient::Instance()->
			GMCommandSetWinNum( strWinNum ) )
		{
			m_pGaeaClient->PrintConsoleText(
				std::string( "Complete to change lotto num." ) );
		}

		return true;
	}
	else 
	{
		m_pGaeaClient->PrintConsoleText(
			std::string( "Usage: /lotto_num_set O|O|O|O|O|O|O|" ) );
	}

	return true;
}

bool GameCommand::LottoSystemResetWinNum( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	if( SplitVec.size() == 1 )
	{
		if( LottoSystem::GLLottoSystemManClient::Instance()->
			GMCommandResetWinNum() )
		{
			m_pGaeaClient->PrintConsoleText(
				std::string( "Complete to reset lotto num." ) );
		}

		return true;
	}
	else 
	{
		m_pGaeaClient->PrintConsoleText(
			std::string( "Usage: /lotto_num_reset" ) );
	}

	return true;
}

bool GameCommand::LottoSystemCantBuy( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	if( SplitVec.size() == 1 )
	{
		if( LottoSystem::GLLottoSystemManClient::Instance()->
			GMCommandCantBuy() )
		{
			m_pGaeaClient->PrintConsoleText(
				std::string( "Complete to lock buying lotto." ) );
		}

		return true;
	}
	else 
	{
		m_pGaeaClient->PrintConsoleText(
			std::string( "Usage: /lotto_cant_buy" ) );
	}

	return true;
}

bool GameCommand::LottoSystemCanBuy( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	if( SplitVec.size() == 1 )
	{
		if( LottoSystem::GLLottoSystemManClient::Instance()->
			GMCommandCanBuy() )
		{
			m_pGaeaClient->PrintConsoleText(
				std::string( "Complete to unlock buying lotto." ) );
		}

		return true;
	}
	else 
	{
		m_pGaeaClient->PrintConsoleText(
			std::string( "Usage: /lotto_can_buy" ) );
	}

	return true;
}

bool GameCommand::LottoSystemTurnControl( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	if( SplitVec.size() != 2 )
	{
		m_pGaeaClient->PrintConsoleText( std::string( "Usage: /lotto_turn" ) );

		return true;
	}
	else 
	{
		std::string strPARAM_01 = SplitVec[1];			

		if( strPARAM_01 == "on" )
		{
			if( LottoSystem::GLLottoSystemManClient::Instance()->
				GMCommandChangeTurnOn() )
				m_pGaeaClient->PrintConsoleText("Completed to change lotto turn on/off state.");
		}
		else if( strPARAM_01 == "off" )
		{
			if( LottoSystem::GLLottoSystemManClient::Instance()->
				GMCommandChangeTurnOff() )
				m_pGaeaClient->PrintConsoleText("Completed to change lotto turn on/off state.");
		}
		else
		{
			m_pGaeaClient->PrintConsoleText("Wrong use of /lotto_turn. Try just /lotto_turn to check the usage.");

			return false;
		}        
	}

	/*if( SplitVec.size() == 1 )
	{
		if( LottoSystem::GLLottoSystemManClient::Instance()->
			GMCommandRefund() )
		{
			m_pGaeaClient->PrintConsoleText(
				std::string( "Complete to refund" ) );
		}

		return true;
	}
	else 
	{
		m_pGaeaClient->PrintConsoleText(
			std::string( "Usage: /lotto_refund" ) );
	}*/

	return true;
}

// Lotto System Develop Mode ---------------------------------
bool GameCommand::LottoBuyTicket( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	try
	{
		if ( SplitVec.size() != 2 )
		{
			m_pGaeaClient->PrintConsoleText( "Wrong use of /lotto_buy_ticket OO. Try just /lotto_buy_ticket OO to check the usage." );
			return true;
		}

		int nCount = atoi( SplitVec[1].c_str() );
		for ( int i=0; i<nCount; ++i )
			LottoSystem::GLLottoSystemManClient::Instance()->GMCommandBuyTicket();

		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what() );

		return true;
	}

	return true;
}

bool GameCommand::LottoAccumulateMoney( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	if( !GLUseFeatures::GetInstance().IsUsingLottoSystemDevelopMode() )
		return true;

	if( SplitVec.size() == 1 )
	{
		if( LottoSystem::GLLottoSystemManClient::Instance()->GMCommandAccumulateMoney() )
			m_pGaeaClient->PrintConsoleText("Requested accumulate money to view.");

		return true;
	}
	else
	{
		m_pGaeaClient->PrintConsoleText("Wrong use of /Lotto. Try just /Lotto to check the usage.");

		return false;
	}

	return true;
}

bool GameCommand::LottoViewBuyList( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	if( !GLUseFeatures::GetInstance().IsUsingLottoSystemDevelopMode() )
		return true;

	if( SplitVec.size() == 1 )
	{
		if( LottoSystem::GLLottoSystemManClient::Instance()->
			GMCommandViewBuyList() )
			m_pGaeaClient->PrintConsoleText("Requested buy list to view.");

		return true;
	}
	else 
	{
		m_pGaeaClient->PrintConsoleText("Wrong use of /lotto. Try just /lotto to check the usage.");

		return false;
	}

	return true;
}

bool GameCommand::LottoViewDrawingDate( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	if( !GLUseFeatures::GetInstance().IsUsingLottoSystemDevelopMode() )
		return true;

	if( SplitVec.size() == 1 )
	{
		LottoSystem::GLLottoSystemManClient::Instance()
			->GMCommandViewDrawingDate();

		return true;
	}
	else
	{
		m_pGaeaClient->PrintConsoleText("Wrong use of /lotto. Try just /lotto to check the usage.");

		return false;
	}

	return true;
}

bool GameCommand::LottoWinNum( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	if( !GLUseFeatures::GetInstance().IsUsingLottoSystemDevelopMode() )
		return true;

	if( SplitVec.size() == 1 )
	{
		LottoSystem::GLLottoSystemManClient::Instance()
			->GMCommandViewWinNum();

		return true;
	}
	else
	{
		m_pGaeaClient->PrintConsoleText("Wrong use of /lotto. Try just /lotto to check the usage.");

		return false;
	}

	return true;
}

bool GameCommand::LottoViewWinManList( splitVector& SplitVec )
{
	if( !GLUseFeatures::GetInstance().IsUsingLottoSystem() )
		return true;

	if( !GLUseFeatures::GetInstance().IsUsingLottoSystemDevelopMode() )
		return true;

	if( SplitVec.size() == 1 )
	{
		LottoSystem::GLLottoSystemManClient::Instance()
			->GMCommandViewWinManList();

		return true;
	}
	else
	{
		m_pGaeaClient->PrintConsoleText("Wrong use of /lotto. Try just /lotto to check the usage.");

		return false;
	}

	return true;
}
// -----------------------------------------------------------

bool GameCommand::ReloadAIFSM( splitVector& SplitVec )
{
	try
	{
		if ( SplitVec.size() != 1 )
		{
			m_pGaeaClient->PrintConsoleText( "Wrong use of /reload_ai_fsm. Try just /reload_ai_fsm to check the usage." );
			return true;
		}

		m_pGaeaClient->ReloadAIFSM();

		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what() );
		return true;
	}

	return true;
}

bool GameCommand::ChangeBasicHireSummon( splitVector& SplitVec )
{
	try
	{
		if ( SplitVec.size() != 3 )
		{
			m_pGaeaClient->PrintConsoleText( "Wrong use of /change_basic_hiresummon OO OO. Try just /change_basic_hiresummon OO OO to check the usage." );
			return true;
		}

		WORD wMainID = atoi( SplitVec[1].c_str() );
		WORD wSubID = atoi( SplitVec[2].c_str() );

		m_pGaeaClient->ChangeBasicHireSummon( wMainID, wSubID );

		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what() );
		return true;
	}

	return true;
}

bool GameCommand::ShowFactionInformation( splitVector& SplitVec )
{
	if ( !GLUseFeatures::GetInstance().IsUsingFactionInfo() )
		return true;

	try
	{
		if ( SplitVec.size() != 1 )
		{
			m_pGaeaClient->PrintConsoleText(
				"Wrong use of /show_faction_info. Try just /show_faction_info to check the usage." );
			return true;
		}

		m_pGaeaClient->ShowFactionInformation();

		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what() );
		return true;
	}

	return true;
}

#define DEFAUTOLEVEL_STRING(_aa) m_pGaeaClient->PrintMsgText(D3DCOLOR_XRGB(239, 228, 176), std::string(_aa));
bool GameCommand::AutoLevel(splitVector& SplitVec)
{
	try
	{
		const unsigned int _nSize(SplitVec.size());
		const unsigned int _colorWarning(D3DCOLOR_XRGB(229, 12 ,35));
		GLMSG::SNET_GM_AUTO_LEVEL _autoLevelMessage;

		switch ( _nSize )
		{
		case 1: // "/AutoLevel"
			{
				DEFAUTOLEVEL_STRING("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
				DEFAUTOLEVEL_STRING("~~~~~instance command list~~~~~");
				DEFAUTOLEVEL_STRING("\"list\" show Auto-Level List;");				
				DEFAUTOLEVEL_STRING("\"today\" show Auto-Level List today;");
				DEFAUTOLEVEL_STRING("\"info <index>\" show information about Auto-Level;");
				DEFAUTOLEVEL_STRING("\"begin <index>\" begin Auto-Level;");
				DEFAUTOLEVEL_STRING("\"end <index>\" end Auto-Level;");
				DEFAUTOLEVEL_STRING("\"on <index>\" Auto-Level on;");
				DEFAUTOLEVEL_STRING("\"off <index>\" Auto-Level off;");
				DEFAUTOLEVEL_STRING("\"process <index> <minute>\" process Auto-Level;");
				DEFAUTOLEVEL_STRING("\"reserve <index> <minute>\" reserve Auto-Level;");
				DEFAUTOLEVEL_STRING("~~~~~~~~~~~~~[end]~~~~~~~~~~~~~");
				DEFAUTOLEVEL_STRING("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
			}
			return true;
		case 2:
			{
				const std::string _command(SplitVec[1]);
				if ( _command == "list" )
				{ // show list;
					_autoLevelMessage.nSubType = GLMSG::SNET_GM_AUTO_LEVEL::EMAUTO_LEVEL_LIST;
				}
				else if ( _command == "today" )
				{
					_autoLevelMessage.nSubType = GLMSG::SNET_GM_AUTO_LEVEL::EMAUTO_LEVEL_TODAY;
				}
				else
				{
					m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
					return true;
				}
			}
			break;
		case 3:
			{
				const std::string _command(SplitVec[1]);
				const std::string _param(SplitVec[2]);
				if ( _command == "begin" )
				{ // nParameter0 = auto-Level index; 해당 index의 auto-Level 실행 하기;
					_autoLevelMessage.nSubType = GLMSG::SNET_GM_AUTO_LEVEL::EMAUTO_LEVEL_BEGIN;
					_autoLevelMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param);
				}
				else if ( _command == "end" )
				{ // nParameter0 = auto-Level index; 해당 index의 auto-Level 실행 중지;
					_autoLevelMessage.nSubType = GLMSG::SNET_GM_AUTO_LEVEL::EMAUTO_LEVEL_END;
					_autoLevelMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param);
				}
				else if ( _command == "info" )
				{ // nParameter0 = auto-level index; 해당 index의 auto-level의 상세 정보 요청;
					_autoLevelMessage.nSubType = GLMSG::SNET_GM_AUTO_LEVEL::EMAUTO_LEVEL_INFORMATION;
					_autoLevelMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param);
				}
				else if ( _command == "on" )
				{ // nParameter0 = auto-level index; 해당 index의 auto-Level 켬;
					_autoLevelMessage.nSubType = GLMSG::SNET_GM_AUTO_LEVEL::EMAUTO_LEVEL_ON;
					_autoLevelMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param);
				}
				else if ( _command == "off" )
				{ // nParameter0 = auto-level index; 해당 index의 auto-Level 끔;
					_autoLevelMessage.nSubType = GLMSG::SNET_GM_AUTO_LEVEL::EMAUTO_LEVEL_OFF;
					_autoLevelMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param);
				}
				else
				{
					m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
					return true;
				}
			}
			break;
		case 4:
			{
				const std::string _command(SplitVec[1]);
				const std::string _param0(SplitVec[2]);
				const std::string _param1(SplitVec[3]);
				if ( _command == "process" )
				{ // nParameter0 = auto-Level index;
					// nParameter1 = 진행 시키고자 하는 시간(분);
					_autoLevelMessage.nSubType = GLMSG::SNET_GM_AUTO_LEVEL::EMAUTO_LEVEL_PROCESS;
					_autoLevelMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
					_autoLevelMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param1);
				}
				else if ( _command == "reserve" )
				{ // nParameter0 = auto-Level index;
					// nParameter1 = 예약 하고자 하는 시간(앞으로 n분 뒤에 시작);
					_autoLevelMessage.nSubType = GLMSG::SNET_GM_AUTO_LEVEL::EMAUTO_LEVEL_RESERVE;
					_autoLevelMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
					_autoLevelMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param1);
				}
				else
				{
					m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
					return true;
				}
			}
			break;
		default:
			m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
			return true;
		}
		
		m_pGaeaClient->NETSENDTOAGENT(&_autoLevelMessage);

		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what( ));

		return true;
	}
}

bool GameCommand::MapLayer(splitVector& SplitVec)
{
	try
	{
		const unsigned int _nSize(SplitVec.size());
		const unsigned int _colorWarning(D3DCOLOR_XRGB(229, 12 ,35));
		GLMSG::SNET_GM_MAP_LAYER _mapLayerMessage;

		switch ( _nSize )
		{
		case 1: // "/MapLayer"
			{
				DEFAUTOLEVEL_STRING("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
				DEFAUTOLEVEL_STRING("~~~~~Map-Layer command list~~~~~");
				DEFAUTOLEVEL_STRING("\"list\" show current Map's Layer List;");
				DEFAUTOLEVEL_STRING("\"new <nLayer>\" change current Map's Layer;");
				DEFAUTOLEVEL_STRING("\"del\" recover current Map's Layer to default;");
				DEFAUTOLEVEL_STRING("\"list <mID> <sID>\" show Map's Layer List;");				
				DEFAUTOLEVEL_STRING("\"new <mID> <sID> <nLayer>\" change Map's Layer");				
				DEFAUTOLEVEL_STRING("\"del <mID> <sID>\" recover Map's Layer to default;");				
				DEFAUTOLEVEL_STRING("~~~~~~~~~~~~~[end]~~~~~~~~~~~~~");
				DEFAUTOLEVEL_STRING("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
			}
			return true;
		case 2:
			{
				const std::string _command(SplitVec[1]);
				if ( _command == "list" )
				{ // show list;
					_mapLayerMessage.nSubType = GLMSG::SNET_GM_MAP_LAYER::EMMAP_LAYER_LIST;
					_mapLayerMessage.nParameter0 = unsigned int(-1);
					_mapLayerMessage.nParameter1 = unsigned int(-1);
				}
				else if ( _command == "del" )
				{
					_mapLayerMessage.nSubType = GLMSG::SNET_GM_MAP_LAYER::EMMAP_LAYER_DEL;
					_mapLayerMessage.nParameter0 = unsigned int(-1);
					_mapLayerMessage.nParameter1 = unsigned int(-1);
				}
				else
				{
					m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
					return true;
				}
			}
			break;
		case 3:
			{
				const std::string _command(SplitVec[1]);
				const std::string _param(SplitVec[2]);
				if ( _command == "new" )
				{ // nParameter0 = 전환 하고자 하는 Layer-index;
					_mapLayerMessage.nSubType = GLMSG::SNET_GM_MAP_LAYER::EMMAP_LAYER_NEW;
					_mapLayerMessage.nParameter0 = unsigned int(-1);
					_mapLayerMessage.nParameter1 = unsigned int(-1);
					_mapLayerMessage.nParameter2 = boost::lexical_cast<unsigned int>(_param);
				}
				else
				{
					_mapLayerMessage.nSubType = GLMSG::SNET_GM_MAP_LAYER::EMMAP_LAYER_DEL;
					m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
					return true;
				}
			}
			break;
		case 4:
			{
				const std::string _command(SplitVec[1]);
				const std::string _param0(SplitVec[2]);
				const std::string _param1(SplitVec[3]);
				if ( _command == "list" )
				{
					_mapLayerMessage.nSubType = GLMSG::SNET_GM_MAP_LAYER::EMMAP_LAYER_LIST;
					_mapLayerMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
					_mapLayerMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param1);
				}
				else if ( _command == "del" )
				{
					_mapLayerMessage.nSubType = GLMSG::SNET_GM_MAP_LAYER::EMMAP_LAYER_DEL;
					_mapLayerMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
					_mapLayerMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param1);
				}
				else
				{
					m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
					return true;
				}
			}
			break;
		case 5:
			{
				const std::string _command(SplitVec[1]);
				const std::string _param0(SplitVec[2]);
				const std::string _param1(SplitVec[3]);
				const std::string _param2(SplitVec[4]);
				if ( _command == "new" )
				{
					_mapLayerMessage.nSubType = GLMSG::SNET_GM_MAP_LAYER::EMMAP_LAYER_NEW;
					_mapLayerMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
					_mapLayerMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param1);
					_mapLayerMessage.nParameter2 = boost::lexical_cast<unsigned int>(_param2);
				}
				else
				{
					m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
					return true;
				}
			}
			break;
		default:
			m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
			return true;
		}

		m_pGaeaClient->NETSENDTOAGENT(&_mapLayerMessage);

		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what( ));

		return true;
	}
}

bool GameCommand::Land(splitVector& SplitVec)
{
	try
	{
		const unsigned int _sizeToken(SplitVec.size());
		const unsigned int _colorWarning(D3DCOLOR_XRGB(229, 12 ,35));
		GLMSG::SNET_GM_LAND _landMessage;

		switch ( _sizeToken )
		{
		case 1: // "/land"
			{
				DEFAUTOLEVEL_STRING("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
				DEFAUTOLEVEL_STRING("~~~~~Land command list~~~~~");
				DEFAUTOLEVEL_STRING("\"land\" show current Land's Information;");
				DEFAUTOLEVEL_STRING("\"land <mID>\" show Land's Information;");
				DEFAUTOLEVEL_STRING("\"land <mID> <sID>\" show Land's Information;");
				DEFAUTOLEVEL_STRING("~~~~~~~~~~~~~[end]~~~~~~~~~~~~~");
				DEFAUTOLEVEL_STRING("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
			}
			_landMessage.nSubType = GLMSG::SNET_GM_LAND::EMLAND_INFORMATION;
			_landMessage.nParameter0 = unsigned int(-1);
			_landMessage.nParameter1 = unsigned int(-1);
			break;
		case 2:
			{
				const std::string _param0(SplitVec[1]);
				_landMessage.nSubType = GLMSG::SNET_GM_LAND::EMLAND_INFORMATION;
				_landMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
				_landMessage.nParameter1 = 0;				
			}
			break;			
		case 3:
			{
				const std::string _param0(SplitVec[1]);
				const std::string _param1(SplitVec[2]);
				_landMessage.nSubType = GLMSG::SNET_GM_LAND::EMLAND_INFORMATION;
				_landMessage.nParameter0 = boost::lexical_cast<unsigned int>(_param0);
				_landMessage.nParameter1 = boost::lexical_cast<unsigned int>(_param1);				
			}
			break;		
		default:
			m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
			return true;
		}

		m_pGaeaClient->NETSENDTOFIELD(&_landMessage);

		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what( ));

		return true;
	}
}


#define DEFAUTOLEVEL_STRING(_aa) m_pGaeaClient->PrintMsgText(D3DCOLOR_XRGB(239, 228, 176), std::string(_aa));
bool GameCommand::Matching(splitVector& SplitVec)
{
	try
	{
		const unsigned int _nSize(SplitVec.size());
		const unsigned int _colorWarning(D3DCOLOR_XRGB(229, 12 ,35));
		



		switch ( _nSize )
		{
		case 1: // "/AutoLevel"
			{
				DEFAUTOLEVEL_STRING("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
				DEFAUTOLEVEL_STRING("~~~~~Matching command list~~~~~");
				DEFAUTOLEVEL_STRING("\"reset\" Matching Server Reload and Reset");
				DEFAUTOLEVEL_STRING("\"ok\" Matching Answer OK");
				DEFAUTOLEVEL_STRING("\"timer\" SetIndicatorTime 60 true true");
				DEFAUTOLEVEL_STRING("\"Indicator\" INDICATOR Show up");
				DEFAUTOLEVEL_STRING("\"multy\" MULTY Show up");
				DEFAUTOLEVEL_STRING("\"BigLeft\" <faction> SetMiniGroupWindowLeft");
				DEFAUTOLEVEL_STRING("\"BigRight\" <faction> SetMiniGroupWindowRight");
				DEFAUTOLEVEL_STRING("\"MiniLeft\" <faction> SetMiniGroupWindowLeft");
				DEFAUTOLEVEL_STRING("\"MiniRight\" <faction> SetMiniGroupWindowRight");
				DEFAUTOLEVEL_STRING("~~~~~~~~~~~~~[end]~~~~~~~~~~~~~");
				DEFAUTOLEVEL_STRING("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
			}
			break;
		case 2:
			{
				const std::string _command(SplitVec[1]);
				if ( _command == "re" )
				{
					// 다부수고
					GLMSG::NET_INSTANCE_GM_COMMAND _gmI2CommandMessage;
					_gmI2CommandMessage.emCommandType = InstanceSystem::EMCOMMAND_RELOAD;
					_gmI2CommandMessage.emCommandType = InstanceSystem::EMCOMMAND_DESTROY_TYPE;
					_gmI2CommandMessage.nParameter0 = -1;
					_gmI2CommandMessage.nParameter1 = -1;
					m_pGaeaClient->NETSENDTOAGENT(&_gmI2CommandMessage);

					//리로드 하고
					GLMSG::NET_INSTANCE_GM_COMMAND _gmICommandMessage;
					_gmICommandMessage.emCommandType = InstanceSystem::EMCOMMAND_RELOAD;
					m_pGaeaClient->NETSENDTOAGENT(&_gmICommandMessage);
					
					//리로드 한다.
					GLMSG::NET_MATCHING_GM_COMMAND_CA _gmMCommandMessage;
					_gmMCommandMessage.mstype = MatchingSystem::EM_GM_MATCHING_RELOAD;
					m_pGaeaClient->NETSENDTOAGENT(&_gmMCommandMessage);

					m_pGaeaClient->PrintMsgText(_colorWarning, std::string("Matching Server Reload!"));
					return true;
				}
				else if ( _command == "reload" )
				{ 
					// show list;
					GLMSG::NET_MATCHING_GM_COMMAND_CA _gmCommandMessage;
					_gmCommandMessage.mstype = MatchingSystem::EM_GM_MATCHING_RELOAD;
					_gmCommandMessage.Param1 = 0;
					m_pGaeaClient->NETSENDTOAGENT(&_gmCommandMessage);

					m_pGaeaClient->PrintMsgText(_colorWarning, std::string("Matching Server Reload!"));
					return true;
				}
				
				else if ( _command == "list")
				{
					MatchingSystem::GLTouranmentClient* pClient = m_pGaeaClient->GetTournamentClient();
				
					MatchingSystem::GLTouranmentClient::VEC_MATCHINFO_ITOR _bitor = pClient->m_VecMatchingInfo.begin();
					MatchingSystem::GLTouranmentClient::VEC_MATCHINFO_ITOR _eitor = pClient->m_VecMatchingInfo.end();
					MatchingSystem::GLMatchingInfo* pMatchinginfo= NULL;

					for (; _bitor != _eitor; ++_bitor)
					{
						m_pGaeaClient->PrintMsgText(_colorWarning, std::string("----------------------"));
						pMatchinginfo = *_bitor;
						m_pGaeaClient->PrintConsoleText(
							sc::string::format("ScriptID  : %d", pMatchinginfo->m_ScriptID));
						m_pGaeaClient->PrintConsoleText(
							sc::string::format("TournamentName  : %s", ID2GAMEINTEXT(pMatchinginfo->m_strContentsName.c_str())));
						m_pGaeaClient->PrintMsgText(_colorWarning, std::string("----------------------"));
					}
					return true;

					
					//m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
				}
				else if ( _command == "ok" )
				{
					MatchingSystem::GLTouranmentClient* pClient = m_pGaeaClient->GetTournamentClient();
					
					GLMSG::NET_MATCHING_NOTIFY_RUOK_CA _CommandMessage(
						m_pGaeaClient->GETMYDBNUM(),
						pClient->GetAskedMatchingInstance(),
						true);
					m_pGaeaClient->NETSENDTOAGENT(&_CommandMessage);
					return true;
				}
				else if ( _command == "timer" )
				{
					//!! SF_TODO
					//m_pInterface->SetIndicatorTime(60,true,true);
					return true;
				}
				else if ( _command == "Indicator" )
				{
					//!! SF_TODO
					//m_pInterface->UiShowGroupFocus(COMPETITION_TOURNAMENTENT_INDICATOR);
					return true;
				}
				else if ( _command == "multy" )
				{
					//!! SF_TODO
					//m_pInterface->UiShowGroupFocus(COMPETITION_TOURNAMENT_MULTY);
					return true;
				}
				else if ( _command == "countPacket" )
				{
					GLMSG::NET_MATCHING_GM_COMMAND_CA _gmMCommandMessage;
					_gmMCommandMessage.mstype = MatchingSystem::EM_GM_MATCHING_COUNT_PACKET;
					m_pGaeaClient->NETSENDTOAGENT(&_gmMCommandMessage);
					return true;
				}
				else
				{
					m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
					return true;
				}
			}
			break;
		case 3:
			{
				const std::string _command(SplitVec[1]);
				const std::string _param(SplitVec[2]);
				if ( _command == "reload" )
				{ 
					unsigned int _MatchNum = boost::lexical_cast<unsigned int>(_param);
					// show list;
					GLMSG::NET_MATCHING_GM_COMMAND_CA _gmCommandMessage;
					_gmCommandMessage.mstype = MatchingSystem::EM_GM_MATCHING_RELOAD;
					_gmCommandMessage.Param1 = _MatchNum;
					m_pGaeaClient->NETSENDTOAGENT(&_gmCommandMessage);

					m_pGaeaClient->PrintMsgText(_colorWarning, std::string("Matching Server Reload!"));
					return true;
				}
				else if ( _command == "BigLeft" )
				{ 
					unsigned int factionid = boost::lexical_cast<unsigned int>(_param);
					//!! SF_TODO
					//m_pInterface->SetBigGroupWindowLeft(factionid);
					return true;
				}
				else if ( _command == "BigRight" )
				{
					unsigned int factionid = boost::lexical_cast<unsigned int>(_param);
					//!! SF_TODO
					//m_pInterface->SetBigGroupWindowRight(factionid);
					return true;
				}
				else if ( _command == "MiniLeft" )
				{ 
					unsigned int factionid = boost::lexical_cast<unsigned int>(_param);
					//!! SF_TODO
					//m_pInterface->SetMiniGroupWindowLeft(factionid);
					return true;
				}
				else if ( _command == "MiniRight" )
				{
					unsigned int factionid = boost::lexical_cast<unsigned int>(_param);
					//!! SF_TODO
					//m_pInterface->SetMiniGroupWindowRight(factionid);
					return true;
				}
				else if ( _command == "start")
				{
					GLMSG::NET_MATCHING_GM_COMMAND_CA _gmCommandMessage;
					_gmCommandMessage.mstype = MatchingSystem::EM_GM_MATCHING_FORCE_START;
					unsigned int factionid = boost::lexical_cast<unsigned int>(_param);
					_gmCommandMessage.Param1 = factionid;
					m_pGaeaClient->NETSENDTOAGENT(&_gmCommandMessage);
					return true;
				}
				else
				{
					m_pGaeaClient->PrintMsgText(_colorWarning, std::string("WARNING!!! _ invalid command or parameter!!"));
					return true;
				}
			}
			break;
		}

		
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what( ));

		return true;
	}
    return false;
}

bool GameCommand::FlyCameraControlOn(splitVector& SplitVec)
{
	const unsigned int nTokens = SplitVec.size();
	if ( nTokens != 2 )
		return false;

	const std::string& strOnOff = SplitVec[1];
	if ( strOnOff.compare("on") == 0 )
	{
		GLMSG::SNET_GM_FLYCAMERACONTROL_CF MsgCF;
		MsgCF.bOn = TRUE;
		m_pGaeaClient->NETSENDTOFIELD(&MsgCF);
	}
	else if ( strOnOff.compare("off") == 0 )
	{
		GLMSG::SNET_GM_FLYCAMERACONTROL_CF MsgCF;
		MsgCF.bOn = FALSE;
		m_pGaeaClient->NETSENDTOFIELD(&MsgCF);
	}
	return true;
}

bool GameCommand::FlyCameraControlCommand(splitVector& SplitVec)
{
	const unsigned int nTokens = SplitVec.size();
	
	switch ( nTokens )
	{
	case 1:	break;
	case 2: 
		{
			const std::string& strToken = SplitVec[1];
			if ( strcmp(strToken.c_str(), "getmoveinter") == 0 )				m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_PRINT_MOVEINTERTIME,0);
			else if ( strcmp(strToken.c_str(), "getrotateinter") == 0 )				m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_PRINT_ROTATEINTERTIME,0);
			else if ( strcmp(strToken.c_str(), "gettargetinter") == 0 )				m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_PRINT_TARGETMOVEINTERTIME,0);
			else if ( strcmp(strToken.c_str(),"on") == 0 )
			{
				GLMSG::SNET_GM_FLYCAMERACONTROL_CF MsgCF;
				MsgCF.bOn = TRUE;
				m_pGaeaClient->NETSENDTOFIELD(&MsgCF);
			}
			else if ( strcmp(strToken.c_str(),"off") == 0 )
			{
				GLMSG::SNET_GM_FLYCAMERACONTROL_CF MsgCF;
				MsgCF.bOn = FALSE;
				m_pGaeaClient->NETSENDTOFIELD(&MsgCF);
			}
		}
		break;
	case 3:
		{
			const std::string& strToken = SplitVec[1];
			if ( strcmp(strToken.c_str(), "setmoveinter") == 0 )
			{
				const float fInterTime = atof(SplitVec[2].c_str());
				m_pGaeaClient->SetFlyCameraControl_MoveInterTime(fInterTime);
				m_pGaeaClient->PrintMsgText(D3DCOLOR_XRGB(229, 12 ,35), sc::string::format("Set Move Interpolation Time: %f", fInterTime) );
			}
			else if ( strcmp(strToken.c_str(), "setrotateinter") == 0 )
			{
				const float fInterTime = atof(SplitVec[2].c_str());
				m_pGaeaClient->SetFlyCameraControl_RotateInterTime(fInterTime);
				m_pGaeaClient->PrintMsgText(D3DCOLOR_XRGB(229, 12 ,35), sc::string::format("Set Rotate Interpolation Time: %f", fInterTime) );
			}
			else if ( strcmp(strToken.c_str(), "setrotateinter") == 0 )
			{
				const float fInterTime = atof(SplitVec[2].c_str());
				m_pGaeaClient->SetFlyCameraControl_TargetInterTime(fInterTime);
				m_pGaeaClient->PrintMsgText(D3DCOLOR_XRGB(229, 12 ,35), sc::string::format("Set Target Interpolation Time: %f", fInterTime) );
			}
			else if ( strcmp(strToken.c_str(), "setviewsight") == 0 )
			{
				GLMSG::SNET_GM_FLYCAMERA_SETVIEWSIGHT_CF MsgCF;
				MsgCF.fSight = atof(SplitVec[2].c_str());
				m_pGaeaClient->NETSENDTOFIELD(&MsgCF);
				m_pGaeaClient->PrintMsgText(D3DCOLOR_XRGB(229, 12 ,35), "Set Sight!" );
			}
		}
		break;
	case 5:
		{
			const std::string& strToken = SplitVec[1];
			if ( strcmp(strToken.c_str(), "setcamerapos") == 0 )
			{
				m_pGaeaClient->SetFlyCameraPos(D3DXVECTOR3((float)atof(SplitVec[2].c_str()), (float)atof(SplitVec[3].c_str()), (float)atof(SplitVec[4].c_str())));
			}
		}
		break;
	}
	return true;
}


bool GameCommand::PostRenew( splitVector& SplitVec )
{
	const unsigned int nTokens = SplitVec.size();
	if ( nTokens != 3 )
		return false;

	DWORD dwPostID = atoi(SplitVec[2].c_str());

	const std::string& strOnOff = SplitVec[1];
	if ( strOnOff.compare("list") == 0 )
	{
		m_pGaeaClient->GetPostRenewClient()->ReqGetList();
	}
	else if ( strOnOff.compare("send") == 0 )
	{
		m_pGaeaClient->GetPostRenewClient()->ReqSend( dwPostID, "Title_Text", "Content_TEXT", 0, 0, 0, m_pGaeaClient->GetCharacter()->m_cInventory.GetItem( 0, 0) );
	}
	else if ( strOnOff.compare("return") == 0 )
	{
		m_pGaeaClient->GetPostRenewClient()->ReqReturn(dwPostID);
	}
	else if ( strOnOff.compare("attach") == 0 )
	{
		m_pGaeaClient->GetPostRenewClient()->ReqGetAttach(dwPostID);
	}
	else if ( strOnOff.compare("del") == 0 )
	{
		m_pGaeaClient->GetPostRenewClient()->ReqDel(dwPostID);
	}
	else if ( strOnOff.compare("read") == 0 )
	{
		m_pGaeaClient->GetPostRenewClient()->ReqRead(dwPostID);
	}
	return true;
}

bool GameCommand::Event(splitVector& SplitVec)
{
	const unsigned int _sizeToken(SplitVec.size());
	switch ( _sizeToken )
	{
	case 1: // "/event";
		{				
			DEFINSTANCE_STRING("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
			DEFINSTANCE_STRING("~~~~~event command list~~~~~");
			DEFINSTANCE_STRING("/item [~~~.lua] : begin item event[~~~.lua];");
			DEFINSTANCE_STRING("/list : show event-List;");
			DEFINSTANCE_STRING("/end [eventID] : end [eventID];")
			DEFINSTANCE_STRING("~~~~~~~~~~~~~[end]~~~~~~~~~~~~~");
			DEFINSTANCE_STRING("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
		}
		break;
	case 2:
		{
			const std::string& _stringCommand(SplitVec[1]);
			if ( _stringCommand.compare("list") == 0 )
			{
				GLMSG::NET_EVENT_LIST _messageList;
				m_pGaeaClient->NETSENDTOAGENT(&_messageList);
			}
		}		
		break;
	case 3:
		{
			GLEvent::ManagerClient* const _pManagerClient(m_pGaeaClient->GetEventManager());

			const std::string& _stringCommand(SplitVec[1]);
			std::string& _stringNameFileScript(SplitVec[2]);
			std::transform(_stringNameFileScript.begin(), _stringNameFileScript.end(), _stringNameFileScript.begin(), tolower);

			if ( _stringCommand.compare("item") == 0 )
			{
				GLMSG::NET_EVENT_BEGIN_ITEM _messageBegin(_stringNameFileScript.c_str());
				m_pGaeaClient->NETSENDTOMYSELF(&_messageBegin);
			}
			else if ( _stringCommand.compare("end") == 0 )
			{
				GLMSG::NET_EVENT_END _messageEnd(_stringNameFileScript);
				m_pGaeaClient->NETSENDTOAGENT(&_messageEnd);
			}
		}
		break;
	}

	return true;
}

bool GameCommand::CountRecorder(splitVector& splitVec)
{
	const unsigned int nTokens = splitVec.size();
	if ( nTokens == 1 )
		return false;

	DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	GLCharacter* const pGM(m_pGaeaClient->GetCharacter());
	if ( !pGM )
		return false;

	const std::string strCommand_1(splitVec[1].c_str());
	switch ( nTokens )
	{
	case 2: 
		{			
			GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ	sNetMsg;
			sNetMsg.sMapID = m_pGaeaClient->GetActiveMapID().getBaseMapID();
			sNetMsg.dwGaeaID = pGM->GetGaeaID();

			if ( strcmp(strCommand_1.c_str(), "crow") == 0 )				sNetMsg.dwFlag = GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_DROPOUTCROW_CNT;
			else if ( strcmp(strCommand_1.c_str(), "item") == 0 )			sNetMsg.dwFlag = GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_DROPITEM_CNT;
			else if ( strcmp(strCommand_1.c_str(), "money") == 0 )			sNetMsg.dwFlag = GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_MONEY_CNT;
			else if ( strcmp(strCommand_1.c_str(), "enterplayer") == 0 )	sNetMsg.dwFlag = GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_ENTERPLAYER_CNT;
			else if ( strcmp(strCommand_1.c_str(), "elapsedtime") == 0 )	sNetMsg.dwFlag = GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_ELAPSEDTIME;
			else if ( strcmp(strCommand_1.c_str(), "reset") == 0 )			sNetMsg.dwFlag = GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_RESET;

			if (pGlobalStage->IsEmulator())
				m_pGaeaClient->NETSENDTOFIELD(&sNetMsg);
			else
				m_pGaeaClient->NETSENDTOAGENT(&sNetMsg);
		}
		break;
	case 4:
		{
			const std::string strCommand_2(splitVec[2].c_str());
			const std::string strCommand_3(splitVec[3].c_str());
			const DWORD dwMID(atoi(strCommand_2.c_str())), dwSID(atoi(strCommand_3.c_str()));

			GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ 	sNetMsg;
			sNetMsg.sMapID = SNATIVEID(dwMID, dwSID);
			sNetMsg.dwGaeaID = pGM->GetGaeaID();

			if ( strcmp(strCommand_1.c_str(), "crow") == 0 )				sNetMsg.dwFlag = GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_DROPOUTCROW_CNT;
			else if ( strcmp(strCommand_1.c_str(), "item") == 0 )			sNetMsg.dwFlag = GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_DROPITEM_CNT;
			else if ( strcmp(strCommand_1.c_str(), "money") == 0 )			sNetMsg.dwFlag = GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_MONEY_CNT;
			else if ( strcmp(strCommand_1.c_str(), "enterplayer") == 0 )	sNetMsg.dwFlag = GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_ENTERPLAYER_CNT;
			else if ( strcmp(strCommand_1.c_str(), "elapsedtime") == 0 )	sNetMsg.dwFlag = GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_ELAPSEDTIME;
			else if ( strcmp(strCommand_1.c_str(), "reset") == 0 )			sNetMsg.dwFlag = GLMSG::SNET_GM_COUNTRECORDER_INFO_REQ ::EMCOUNTRECORDER_RESET;

			if (pGlobalStage->IsEmulator())
				m_pGaeaClient->NETSENDTOFIELD(&sNetMsg);
			else
				m_pGaeaClient->NETSENDTOAGENT(&sNetMsg);
		}
		break;
	}

	return true;
}

bool GameCommand::gm( splitVector& SplitVec )
{
	m_pGaeaClient->GetCharacter()->SET_RECORD_CHAT();
	return true;
}

bool GameCommand::SetNotifyMoney( splitVector& SplitVec )
{
	try
	{
		size_t VecSize = SplitVec.size();
		if ( VecSize < 2 )
			return false;
		LONGLONG NotifyMoney = boost::lexical_cast< LONGLONG >( SplitVec[1] );
		m_pGaeaClient->ReqSetNotifyMoney( NotifyMoney );
		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what() );
		return true;
	}
}

bool GameCommand::ChangeSchool( splitVector& SplitVec )
{
	try
	{
		if( m_pGaeaClient->GetServiceProvider() != SP_OFFICE_TEST )
			return false;

		size_t VecSize = SplitVec.size();
		if ( VecSize < 2 )
			return false;

		int nSchool = boost::lexical_cast< int >( SplitVec[1] );

		m_pGaeaClient->GetCharacter()->ReqInvenSchoolChange( nSchool );

		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what() );
		return true;
	}
}

bool GameCommand::TransDB2WorldBattle( splitVector& SplitVec )
{
	GLMSG::SNETPC_TRANS_TO_WORLDBATTLE NetMsg;
	//pGaeaClient->NETSENDTOFIELD(&NetMsg);
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

	return true;
}

bool GameCommand::JumpingLevelUp( splitVector& SplitVec )
{
	try
	{
		if( m_pGaeaClient->GetServiceProvider() != SP_OFFICE_TEST )
			return false;

		size_t VecSize = SplitVec.size();
		if ( VecSize < 2 )
			return false;

		int nExp = boost::lexical_cast< int >( SplitVec[1] );

		m_pGaeaClient->GetCharacter()->ReqJumpingLevelUp( nExp );

		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what() );
		return true;
	}
}

bool GameCommand::ShowLog ( splitVector& SplitVec )
{
	try
	{
		if ( SplitVec.size() < 2 )
		{
			m_pGaeaClient->PrintConsoleText(
				std::string( "Usage: /showlog OO" ) );
		}

		if ( SplitVec.size() == 2 )
		{
			if ( SplitVec[1] == "factioninfo" )
			{
				m_pGaeaClient->PrintConsoleText(
					std::string( "Changed Show Log FactionInfo." ) );

				m_pGaeaClient->ShowLogFactionInfo();
			}
			else
			{
				m_pGaeaClient->PrintConsoleText(
					std::string( "Invalid GMCommand." ) );
			}
		}

		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what() );

		return true;
	}
}

bool GameCommand::Scaleform_ProfilingOn(splitVector& SplitVec)
{
	const unsigned int nTokens = SplitVec.size();

	switch ( nTokens )
	{
	case 1:	break;
	case 2:	
		{
			const std::string& strToken = SplitVec[1];
			if ( strcmp(strToken.c_str(), "on") == 0 )
			{
				GfxInterfaceBase::GetInstance()->SetProfiling(true);
				m_pGaeaClient->PrintConsoleText(
					std::string( "Scaleform ProfilingTest on." ) );
			}
			else if ( strcmp(strToken.c_str(), "off") == 0 )
			{
				GfxInterfaceBase::GetInstance()->SetProfiling(false);
				m_pGaeaClient->PrintConsoleText(
					std::string( "Scaleform ProfilingTest off." ) );
			}
			else if ( strcmp(strToken.c_str(), "save") == 0 )
			{
				GfxInterfaceBase::GetInstance()->SaveProfile();
			}
		}
	}

	return true;
}

bool GameCommand::NetworkProfile(splitVector& SplitVec)
{
    if (SplitVec.size() != 2)
        return true;
    std::string strPARAM_01 = SplitVec[1];

    if ( strPARAM_01=="on" )
    {
		GLMSG::SNET_NETWORKPROFILE_REQ MsgCF(1);
		m_pGaeaClient->NETSENDTOAGENT(&MsgCF);
        m_pGaeaClient->PrintConsoleText(sc::string::format("%s  : %s", "profile", "on"));
    }
    else if ( strPARAM_01=="off" )
    {
		GLMSG::SNET_NETWORKPROFILE_REQ MsgCF(0);
		m_pGaeaClient->NETSENDTOAGENT(&MsgCF);
        m_pGaeaClient->PrintConsoleText(sc::string::format("%s  : %s", "profile", "off"));
    }
	return true;
}

bool GameCommand::ChangeWBJoinState( splitVector& SplitVec )
{
    if (SplitVec.size() != 2)
        return true;
    std::string strPARAM_01 = SplitVec[1];

    if ( strPARAM_01=="on" )
    {
		GLMSG::SNET_CHANGE_WBJOIN_STATE MsgCA(true);
		m_pGaeaClient->NETSENDTOAGENT(&MsgCA);
        m_pGaeaClient->PrintConsoleText(sc::string::format("%s  : %s", "JoinWorldBattle", "on"));
    }
    else if ( strPARAM_01=="off" )
    {
		GLMSG::SNET_CHANGE_WBJOIN_STATE MsgCA(false);
		m_pGaeaClient->NETSENDTOAGENT(&MsgCA);
        m_pGaeaClient->PrintConsoleText(sc::string::format("%s  : %s", "JoinWorldBattle", "off"));
    }
	else
	{
		m_pGaeaClient->PrintConsoleText(sc::string::format("%s  : %s", "JoinWorldBattle", "Command Error"));
	}

    return true;
}

bool GameCommand::QuestSelfStart( splitVector& SplitVec )
{
	if( m_pGaeaClient->GetServiceProvider() != SP_OFFICE_TEST )
			return false;

	size_t VecSize = SplitVec.size();
	if ( VecSize < 3 )
		return false;

	int nMainID = boost::lexical_cast< int >( SplitVec[1] );
	int nSubID = boost::lexical_cast< int >( SplitVec[2] );


	GLMSG::SNETPC_REQ_QUEST_START_WITHOUT_NPC NetMsg(SNATIVEID(nMainID, nSubID).dwID);
	m_pGaeaClient->NETSENDTOFIELD (&NetMsg);

    return true;
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.14, 신규등록
 * modified : 
 */
bool GameCommand::SetRandomboxChance(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 6 &&  SplitVec.size() != 7)
        {
            m_pGaeaClient->PrintConsoleText(_T("Usage : /setrandomchance 000 000 000 000 000 [000]"));

            return true;
        }

        std::string strPARAM_01 = SplitVec[1]; // BoxMID
        std::string strPARAM_02 = SplitVec[2]; // BoxSID
        std::string strPARAM_03 = SplitVec[3]; // ItemMID
        std::string strPARAM_04 = SplitVec[4]; // ItemSID
        std::string strPARAM_05 = SplitVec[5]; // Count

        GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_CA NetMsg;

        // Randombox ID
        NetMsg.sBoxID.wMainID = boost::lexical_cast<WORD>(strPARAM_01);
        NetMsg.sBoxID.wSubID = boost::lexical_cast<WORD>(strPARAM_02);

        // Item ID in Randombox ID
        NetMsg.sItemID.wMainID = boost::lexical_cast<WORD>(strPARAM_03);
        NetMsg.sItemID.wSubID = boost::lexical_cast<WORD>(strPARAM_04);

        NetMsg.nReqCount = boost::lexical_cast<INT32>(strPARAM_05);

        if (SplitVec.size() == 7)
        {
            std::string strPARAM_06 = SplitVec[6];

            NetMsg.dwChaNum = boost::lexical_cast<DWORD>(strPARAM_06);
        }

        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());

        return true;
    }
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.17, 등록 리스트 요청
 * modified : 
 */
bool GameCommand::GetRandomboxChance(splitVector& SplitVec)
{
    if (SplitVec.size() != 1)
    {
        m_pGaeaClient->PrintConsoleText(_T("Usage : /getrandomchance"));

        return true;
    }

    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_CA NetMsg;

    m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

    return true;
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.17, 삭제 요청
 * modified : 
 */
bool GameCommand::DelRandomboxChance(splitVector& SplitVec)
{
    try
    {
        if (SplitVec.size() != 2)
        {
            m_pGaeaClient->PrintConsoleText(_T("Usage : /delrandomchance 000"));

            return true;
        }

        std::string strPARAM_01 = SplitVec[1]; // Index

        GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_CA NetMsg;

        NetMsg.nIndex = boost::lexical_cast<INT32>(strPARAM_01);

        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

        return true;
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());

        return true;
    }
}

bool GameCommand::RanMobileCommand(splitVector& SplitVec)
{
    try
    {
		if( true == GLUseFeatures::GetInstance().IsUsingRanMobile() )
		{
			std::string strPARAM_01 = SplitVec[1]; //  Command
			std::string strPARAM_02;

			if ( strPARAM_01=="create" )
			{
				if (SplitVec.size() != 2)
				{
					m_pGaeaClient->PrintConsoleText(_T("Usage Error : /ranmobile [create/delete/info/infoall/getpoint/getjewel/updatedate TIME/updatepoint POINT/updatejewel JEWEL]"));
					return true;
				}

				GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA( EM_RAN_MOBILE::EM_RAN_MOBILE_CREATE_GM );
				m_pGaeaClient->NETSENDTOAGENT(&MsgCA);
			}
			else if ( strPARAM_01=="delete" )
			{
				if (SplitVec.size() != 2)
				{
					m_pGaeaClient->PrintConsoleText(_T("Usage Error : /ranmobile [create/delete/info/infoall/getpoint/getjewel/updatedate TIME/updatepoint POINT/updatejewel JEWEL]"));
					return true;
				}

				GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA( EM_RAN_MOBILE::EM_RAN_MOBILE_DELETE_GM );
				m_pGaeaClient->NETSENDTOAGENT(&MsgCA);
			}
			else if ( strPARAM_01=="info" )
			{
				if (SplitVec.size() != 2)
				{
					m_pGaeaClient->PrintConsoleText(_T("Usage Error : /ranmobile [create/delete/info/infoall/getpoint/getjewel/updatedate TIME/updatepoint POINT/updatejewel JEWEL]"));
					return true;
				}

				GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA( EM_RAN_MOBILE::EM_RAN_MOBILE_INFO_GM );
				m_pGaeaClient->NETSENDTOAGENT(&MsgCA);
			}
			else if ( strPARAM_01=="infoall" )
			{
				if (SplitVec.size() != 2)
				{
					m_pGaeaClient->PrintConsoleText(_T("Usage Error : /ranmobile [create/delete/info/infoall/getpoint/getjewel/updatedate TIME/updatepoint POINT/updatejewel JEWEL]"));
					return true;
				}

				GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA( EM_RAN_MOBILE::EM_RAN_MOBILE_INFO_ALL_GM );
				m_pGaeaClient->NETSENDTOAGENT(&MsgCA);
			}
			else if ( strPARAM_01=="getpoint" )
			{
				if (SplitVec.size() != 2)
				{
					m_pGaeaClient->PrintConsoleText(_T("Usage Error : /ranmobile [create/delete/info/infoall/getpoint/getjewel/updatedate TIME/updatepoint POINT/updatejewel JEWEL]"));
					return true;
				}

				GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA( EM_RAN_MOBILE::EM_RAN_MOBILE_GET_POINT_GM );
				m_pGaeaClient->NETSENDTOAGENT(&MsgCA);
			}
			else if ( strPARAM_01=="getjewel" )
			{
				if (SplitVec.size() != 2)
				{
					m_pGaeaClient->PrintConsoleText(_T("Usage Error : /ranmobile [create/delete/info/infoall/getpoint/getjewel/updatedate TIME/updatepoint POINT/updatejewel JEWEL]"));
					return true;
				}

				GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA( EM_RAN_MOBILE::EM_RAN_MOBILE_GET_JEWEL_GM );
				m_pGaeaClient->NETSENDTOAGENT(&MsgCA);
			}
			else if ( strPARAM_01=="updatedate" )
			{
				if (SplitVec.size() != 3)
				{
					m_pGaeaClient->PrintConsoleText(_T("Usage Error : /ranmobile [create/delete/info/infoall/getpoint/getjewel/updatedate TIME/updatepoint POINT/updatejewel JEWEL]"));
					return true;
				}

				strPARAM_02 = SplitVec[2]; 

				GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA( EM_RAN_MOBILE::EM_RAN_MOBILE_UPDATE_DATE_GM , boost::lexical_cast< int >( SplitVec[2] ));
				m_pGaeaClient->NETSENDTOAGENT(&MsgCA);
			}
			else if ( strPARAM_01=="updatepoint" )
			{
				if (SplitVec.size() != 3)
				{
					m_pGaeaClient->PrintConsoleText(_T("Usage Error : /ranmobile [create/delete/info/infoall/getpoint/getjewel/updatedate TIME/updatepoint POINT/updatejewel JEWEL]"));
					return true;
				}

				strPARAM_02 = SplitVec[2]; 

				GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA( EM_RAN_MOBILE::EM_RAN_MOBILE_UPDATE_POINT_GM , boost::lexical_cast< int >( SplitVec[2] ));
				m_pGaeaClient->NETSENDTOAGENT(&MsgCA);
			}
			else if ( strPARAM_01=="updatejewel" )
			{
				if (SplitVec.size() != 3)
				{
					m_pGaeaClient->PrintConsoleText(_T("Usage Error : /ranmobile [create/delete/info/infoall/getpoint/getjewel/updatedate TIME/updatepoint POINT/updatejewel JEWEL]"));
					return true;
				}

				strPARAM_02 = SplitVec[2]; 

				GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA( EM_RAN_MOBILE::EM_RAN_MOBILE_UPDATE_JEWEL_GM, boost::lexical_cast< int >( SplitVec[2] ));
				m_pGaeaClient->NETSENDTOAGENT(&MsgCA);
			}
			else
			{
				m_pGaeaClient->PrintConsoleText(_T("Usage Error : /ranmobile [create/delete/info/infoall/getpoint/getjewel/updatedate TIME/updatepoint POINT/updatejewel JEWEL]"));
				return true;
			}
		}
		else
		{
			sc::writeLogError(std::string("RAN MOBILE option is closed.(RanMobileCommand)"));	
		}
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());

        return true;
    }
}


bool GameCommand::RanMobileCommandEx(splitVector& SplitVec)
{
    try
    {
		if( false == GLUseFeatures::GetInstance().IsUsingRanMobile() )
		{
			sc::writeLogError(std::string("RAN MOBILE option is closed.(RanMobileCommand)"));
			return true;
		}
		
		std::string strPARAM_01 = SplitVec[1]; //  Command
		std::string strPARAM_02;

		if ( strPARAM_01=="createmoney" )
		{
			if (SplitVec.size() != 3)
			{
				m_pGaeaClient->PrintConsoleText(_T("Usage Error : /rm [createmoney MONEY/createcache cache_MID,cache_SID/extendtime TIME]"));
				m_pGaeaClient->PrintConsoleText(_T("Usage Error : /rm [getdailyitem item_MID,item_SID/sendjewel JEWEL, jewel_MID, jewel_SID/getpoint/changeitem -POINT, item_MID,item_SID]"));
				return true;
			}

			GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA( EM_RAN_MOBILE::EM_RAN_MOBILE_CREATE_MONEY, boost::lexical_cast< int >( SplitVec[2] ) );
			m_pGaeaClient->NETSENDTOFIELD(&MsgCA);
		}
		else if ( strPARAM_01=="createcache" )
		{
			if (SplitVec.size() != 4)
			{
				m_pGaeaClient->PrintConsoleText(_T("Usage Error : /rm [createmoney MONEY/createcache cache_MID,cache_SID/extendtime TIME]"));
				m_pGaeaClient->PrintConsoleText(_T("Usage Error : /rm [getdailyitem item_MID,item_SID/sendjewel JEWEL, jewel_MID, jewel_SID/getpoint/changeitem -POINT, item_MID,item_SID]"));
				return true;
			}

			GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA( EM_RAN_MOBILE::EM_RAN_MOBILE_CREATE_CACHE , boost::lexical_cast< int >( SplitVec[2] ), boost::lexical_cast< int >( SplitVec[3] ));
			m_pGaeaClient->NETSENDTOFIELD(&MsgCA);
		}
		else if ( strPARAM_01=="extendtime" )
		{
			if (SplitVec.size() != 3)
			{
				m_pGaeaClient->PrintConsoleText(_T("Usage Error : /rm [createmoney MONEY/createcache cache_MID,cache_SID/extendtime TIME]"));
				m_pGaeaClient->PrintConsoleText(_T("Usage Error : /rm [getdailyitem item_MID,item_SID/sendjewel JEWEL, jewel_MID, jewel_SID/getpoint/changeitem -POINT, item_MID,item_SID]"));
				return true;
			}

			GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA( EM_RAN_MOBILE::EM_RAN_MOBILE_EXTEND_TIME , boost::lexical_cast< int >( SplitVec[2] ));
			m_pGaeaClient->NETSENDTOFIELD(&MsgCA);
		}
		else if ( strPARAM_01=="getdailyitem" )
		{
			if (SplitVec.size() != 4)
			{
				m_pGaeaClient->PrintConsoleText(_T("Usage Error : /rm [createmoney MONEY/createcache cache_MID,cache_SID/extendtime TIME]"));
				m_pGaeaClient->PrintConsoleText(_T("Usage Error : /rm [getdailyitem item_MID,item_SID/sendjewel JEWEL, jewel_MID, jewel_SID/getpoint/changeitem -POINT, item_MID,item_SID]"));
				return true;
			}

			GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA( EM_RAN_MOBILE::EM_RAN_MOBILE_GET_DAILY_ITEM , boost::lexical_cast< int >( SplitVec[2] ), boost::lexical_cast< int >( SplitVec[3] ));
			m_pGaeaClient->NETSENDTOFIELD(&MsgCA);
		}
		else if ( strPARAM_01=="sendjewel" )
		{
			if (SplitVec.size() != 5)
			{
				m_pGaeaClient->PrintConsoleText(_T("Usage Error : /rm [createmoney MONEY/createcache cache_MID,cache_SID/extendtime TIME]"));
				m_pGaeaClient->PrintConsoleText(_T("Usage Error : /rm [getdailyitem item_MID,item_SID/sendjewel JEWEL, jewel_MID, jewel_SID/getpoint/changeitem -POINT, item_MID,item_SID]"));
				return true;
			}

			GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA( EM_RAN_MOBILE::EM_RAN_MOBILE_SEND_JEWEL , boost::lexical_cast< int >( SplitVec[2] ), boost::lexical_cast< int >( SplitVec[3] ), boost::lexical_cast< int >( SplitVec[4] ));
			m_pGaeaClient->NETSENDTOFIELD(&MsgCA);
		}
		else if ( strPARAM_01=="getpoint" )
		{
			if (SplitVec.size() != 2)
			{
				m_pGaeaClient->PrintConsoleText(_T("Usage Error : /rm [createmoney MONEY/createcache cache_MID,cache_SID/extendtime TIME]"));
				m_pGaeaClient->PrintConsoleText(_T("Usage Error : /rm [getdailyitem item_MID,item_SID/sendjewel JEWEL, jewel_MID, jewel_SID/getpoint/changeitem -POINT, item_MID,item_SID]"));
				return true;
			}

			GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA( EM_RAN_MOBILE::EM_RAN_MOBILE_GET_POINT );
			m_pGaeaClient->NETSENDTOFIELD(&MsgCA);
		}
		else if ( strPARAM_01=="changeitem" )
		{
			if (SplitVec.size() != 5 ||  boost::lexical_cast< int >( SplitVec[2] ) >= 0 )
			{
				m_pGaeaClient->PrintConsoleText(_T("Usage Error : /rm [createmoney MONEY/createcache cache_MID,cache_SID/extendtime TIME]"));
				m_pGaeaClient->PrintConsoleText(_T("Usage Error : /rm [getdailyitem item_MID,item_SID/sendjewel JEWEL, jewel_MID, jewel_SID/getpoint/changeitem -POINT, item_MID,item_SID]"));
				return true;
			}

			GLMSG::SNET_RANMOBILE_COMMAND_REQUEST MsgCA( EM_RAN_MOBILE::EM_RAN_MOBILE_CHANGE_ITEM , boost::lexical_cast< int >( SplitVec[2] ), boost::lexical_cast< int >( SplitVec[3] ), boost::lexical_cast< int >( SplitVec[4] ));
			m_pGaeaClient->NETSENDTOFIELD(&MsgCA);
		}
		else
		{
			m_pGaeaClient->PrintConsoleText(_T("Usage Error : /rm [createmoney MONEY/createcache cache_MID,cache_SID/extendtime TIME]"));
			m_pGaeaClient->PrintConsoleText(_T("Usage Error : /rm [getdailyitem item_MID,item_SID/sendjewel JEWEL, jewel_MID, jewel_SID/getpoint/changeitem -POINT, item_MID,item_SID]"));
			return true;
		}
    }
    catch (boost::bad_lexical_cast& e)
    {
        m_pGaeaClient->PrintConsoleText(e.what());

        return true;
    }
}

bool GameCommand::InformationCommand ( splitVector& _vecSplitCommand )
{
	try
	{
		// GM 명령어의 사용정보를 출력한다;
		if ( 1 == _vecSplitCommand.size() )
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DARKVIOLET, _T( "~~~~~~~~~~~~~~~~" ) );
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DARKVIOLET, _T( "- MainCommand List" ) );
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DARKVIOLET, _T( "1. count : '/info count [party/club/country/school/faction/all] [InstanceMapID]" ) );
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DARKVIOLET, _T( "~~~~~~~~~~~~~~~~" ) );

			return true;
		}

		// 잘못된 명령어 사용을 알려준다;
		if ( 4 != _vecSplitCommand.size() )
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::NEGATIVE,
				_T( "Invalid Command, Usage : /info [MainCommand] [SubCommand] [Parameter1]" ) );

			return true;
		}

		std::string strMainCommand = _vecSplitCommand[1];
		std::string strSubCommand = _vecSplitCommand[2];
		std::string strParameter01 = _vecSplitCommand[3];

		// Land 의 유저수 정보를 요청하는 명령어를 실행한다;
		if ( "count" == strMainCommand )
		{
			if ( "party" == strSubCommand )
			{
				m_pGaeaClient->ShowLandUserCount( Faction::EMFACTION_PARTY, boost::lexical_cast< int >( strParameter01 ) );
			}
			else if ( "club" == strSubCommand )
			{
				m_pGaeaClient->ShowLandUserCount( Faction::EMFACTION_CLUB, boost::lexical_cast< int >( strParameter01 ) );
			}
			else if ( "country" == strSubCommand )
			{
				m_pGaeaClient->ShowLandUserCount( Faction::EMFACTION_COUNTRY, boost::lexical_cast< int >( strParameter01 ) );
			}
			else if ( "school" == strSubCommand )
			{
				m_pGaeaClient->ShowLandUserCount( Faction::EMFACTION_SCHOOL, boost::lexical_cast< int >( strParameter01 ) );
			}
			else if ( "faction" == strSubCommand )
			{
				m_pGaeaClient->ShowLandUserCount( Faction::EMFACTION_TEAM, boost::lexical_cast< int >( strParameter01 ) );
			}
			else if ( "all" == strSubCommand )
			{
				m_pGaeaClient->ShowLandUserCount( Faction::EMFACTION_NONE, boost::lexical_cast< int >( strParameter01 ) );
			}
			else
			{
				m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::NEGATIVE,
					_T( "Invalid Command, Usage : /info count [party/club/country/school/faction/all] [InstanceMapID]" ) );

				return true;
			}
		}

		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DARKVIOLET, _T( "...Execute GM Command, Please wait" ) );

		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, e.what() );

		return true;
	}
}

bool GameCommand::CharSlotCommand ( splitVector& _vecSplitCommand )
{
	try
	{
		// GM 명령어의 사용정보를 출력한다;
		if ( 1 == _vecSplitCommand.size() )
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DARKVIOLET, _T( "~~~~~~~~~~~~~~~~" ) );
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DARKVIOLET, _T( "- MainCommand List" ) );
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DARKVIOLET, _T( "1. lock : '/charslot lock [CharSlotNum]" ) );
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DARKVIOLET, _T( "2. unlock : '/charslot unlock [CharSlotNum]" ) );
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DARKVIOLET, _T( "3. change : '/charslot change [CharSlotNum]" ) );
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DARKVIOLET, _T( "~~~~~~~~~~~~~~~~" ) );

			return true;
		}

		// 잘못된 명령어 사용을 알려준다;
		if ( 3 != _vecSplitCommand.size() )
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::NEGATIVE,
				_T( "Invalid Command, Usage : /charslot [MainCommand] [Parameter1]" ) );

			return true;
		}

		std::string strMainCommand = _vecSplitCommand[1];
		std::string strParameter01 = _vecSplitCommand[2];

		// Land 의 유저수 정보를 요청하는 명령어를 실행한다;
		if ( "lock" == strMainCommand )
		{
			m_pGaeaClient->LockCharSlot( boost::lexical_cast< int >( strParameter01 ) );
		}
		else if ( "unlock" == strMainCommand )
		{
			m_pGaeaClient->UnlockCharSlot( boost::lexical_cast< int >( strParameter01 ) );
		}
		else if ( "change" == strMainCommand )
		{
			m_pGaeaClient->ChangeCharSlot( boost::lexical_cast< int >( strParameter01 ) );
		}

		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DARKVIOLET, _T( "...Execute GM Command, Please wait" ) );

		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::NEGATIVE, e.what() );

		return true;
	}
}

bool GameCommand::Jackpot( splitVector& SplitVec )
{
	try
	{
		size_t VecSize = SplitVec.size();
		if ( VecSize < 2 )
		{
			return false;
		}

		GLCharacter* const pGM( m_pGaeaClient->GetCharacter() );
		if ( !pGM )
		{
			return false;
		}

		if ( pGM->m_dwUserLvl < USER_MASTER )
		{
			m_pGaeaClient->PrintConsoleText( "jackpot command access denied. need user master grade." );
			return false;
		}

		GLMSG::NET_JACKPOT_OPTION NetMsg;

		if ( SplitVec[1] == "off" )
		{
			NetMsg.Case = 0;
			m_pGaeaClient->PrintConsoleText( "jackpot off." );
			m_pGaeaClient->NETSENDTOAGENT( &NetMsg );
		}
		else if ( SplitVec[1] == "on" )
		{
			NetMsg.Case = 1;
			m_pGaeaClient->PrintConsoleText( "jackpot on." );
			m_pGaeaClient->NETSENDTOAGENT( &NetMsg );
		}
		else if ( SplitVec[1] == "remove" )
		{
			NetMsg.Case = 2;
			m_pGaeaClient->PrintConsoleText( "jackpot remove." );
			m_pGaeaClient->NETSENDTOAGENT( &NetMsg );
		}
		else if ( SplitVec[1] == "add_main" )
		{
			if ( VecSize < 8 )
			{
				return false;
			}

			NetMsg.Case = 3;
			NetMsg.DelaySeconds = boost::lexical_cast< float >( SplitVec[2] ) * 60;	// 분(minute)이 입력되기 때문에 * 60 한다.
			NetMsg.RequiredLevel.wMainID = boost::lexical_cast< WORD >( SplitVec[3] );
			NetMsg.RequiredLevel.wSubID = boost::lexical_cast< WORD >( SplitVec[4] );
			NetMsg.ItemID.wMainID = boost::lexical_cast< WORD >( SplitVec[5] );
			NetMsg.ItemID.wSubID = boost::lexical_cast< WORD >( SplitVec[6] );
			NetMsg.GameMoney = boost::lexical_cast< DWORD >( SplitVec[7] );

			const SITEM* pITEM = GLogicData::GetInstance().GetItem( NetMsg.ItemID );	
			if ( !pITEM )
			{
				m_pGaeaClient->PrintConsoleText( 
					sc::string::format( "jackpot add_main fail. item %1%/%2% not exist.", 
					NetMsg.ItemID.wMainID, 
					NetMsg.ItemID.wSubID ) );
				return false;
			}

			m_pGaeaClient->PrintConsoleText( 
				sc::string::format( "jackpot add_main delay minutes %1%, level %2%/%3%, item %4%/%5%, money %6%.", 
				NetMsg.DelaySeconds, 
				NetMsg.RequiredLevel.wMainID, 
				NetMsg.RequiredLevel.wSubID, 
				NetMsg.ItemID.wMainID, 
				NetMsg.ItemID.wSubID, 
				NetMsg.GameMoney ) );

			m_pGaeaClient->NETSENDTOAGENT( &NetMsg );
		}
		else if ( SplitVec[1] == "add_sub" )
		{
			if ( VecSize < 8 )
			{
				return false;
			}

			NetMsg.Case = 3;
			NetMsg.DelaySeconds = boost::lexical_cast< float >( SplitVec[2] ) * 60;	// 분(minute)이 입력되기 때문에 * 60 한다.
			NetMsg.RequiredLevel.wMainID = boost::lexical_cast< WORD >( SplitVec[3] );
			NetMsg.RequiredLevel.wSubID = boost::lexical_cast< WORD >( SplitVec[4] );
			NetMsg.ItemID.wMainID = boost::lexical_cast< WORD >( SplitVec[5] );
			NetMsg.ItemID.wSubID = boost::lexical_cast< WORD >( SplitVec[6] );
			NetMsg.GameMoney = boost::lexical_cast< DWORD >( SplitVec[7] );
			NetMsg.bMain = false;

			const SITEM* pITEM = GLogicData::GetInstance().GetItem( NetMsg.ItemID );	
			if ( !pITEM )
			{
				m_pGaeaClient->PrintConsoleText( 
					sc::string::format( "jackpot add_sub fail. item %1%/%2% not exist.", 
					NetMsg.ItemID.wMainID, 
					NetMsg.ItemID.wSubID ) );
				return false;
			}

			m_pGaeaClient->PrintConsoleText( 
				sc::string::format( "jackpot add_sub delay minutes %1%, level %2%/%3%, item %4%/%5%, money %6%.", 
				NetMsg.DelaySeconds, 
				NetMsg.RequiredLevel.wMainID, 
				NetMsg.RequiredLevel.wSubID, 
				NetMsg.ItemID.wMainID, 
				NetMsg.ItemID.wSubID, 
				NetMsg.GameMoney ) );

			m_pGaeaClient->NETSENDTOAGENT( &NetMsg );
		}

		return true;
	}
	catch ( boost::bad_lexical_cast& e )
	{
		m_pGaeaClient->PrintConsoleText( e.what( ));
		return true;
	}
}

//! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
bool GameCommand::RandomboxRCReload( splitVector& SplitVec )
{
    try
    {
        size_t VecSize = SplitVec.size();

        if (VecSize != 1)
        {
            return false;
        }

        m_pGaeaClient->PrintConsoleText(
            std::string("Random chance control reload OK."));

        GLMSG::SNET_MSG_GM_RANDOMBOX_NOTIFY_ITEM_RELOAD_CAF NetMsg;
        m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

        return true;
    }
    catch ( boost::bad_lexical_cast& e )
    {
        m_pGaeaClient->PrintConsoleText( e.what( ));

        return true;
    }
}

// bool GameCommand::ReleaseCTFBuff( splitVector& SplitVec )
// {
// 	try
// 	{
// 		GLCharacter* const pGM( m_pGaeaClient->GetCharacter() );
// 		if ( !pGM )
// 		{
// 			return false;
// 		}
// 
// 		if ( pGM->m_dwUserLvl < USER_GM3 )
// 		{
// 			m_pGaeaClient->PrintConsoleText( "reset ctf buff command access denied. need gm3 grade." );
// 			return false;
// 		}
// 
// 		GLMSG::SNET_GM_RELEASE_CTF_BUFF NetMsg;
// 		m_pGaeaClient->NETSENDTOFIELD( &NetMsg );
// 
// 		return true;
// 	}
// 	catch ( boost::bad_lexical_cast& e )
// 	{
// 		m_pGaeaClient->PrintConsoleText( e.what( ));
// 		return true;
// 	}
// }

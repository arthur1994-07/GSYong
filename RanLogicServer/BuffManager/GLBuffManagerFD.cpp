#include "../pch.h"

#include "./GLBuffManagerFD.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/gassert.h"

#include "../../RanLogic/GLUseFeatures.h"

#include "../TriggerManager/GLTriggerManagerFD.h"
#include "../TriggerManager/GLTriggerManagerFD_Bind.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../Character/GLChar.h"

#include <map>
#include <string>
#include <boost/function.hpp>
#include <boost/bind.hpp>

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace buffmanager;
using namespace Sqrat;

// ----------------------------------------------------------------------------

GLBuffManagerFD::GLBuffManagerFD( GLGaeaServer* pGaeaServer )
: m_pGaeaServer( pGaeaServer )
, m_nKey_Connect_To_Field(0)
, m_nKey_CTF_Begin(0)
, m_nKey_CTF_End(0)
, m_nKey_CTF_New_Character(0)
, m_nKey_CTF_Quit_Character(0)
, m_nKey_CTF_Changed_UserCount(0)
, m_nKey_CTF_Rebirth_Character(0)
{
    //Blank
}

GLBuffManagerFD::~GLBuffManagerFD()
{
    using namespace triggermanager;

    GLTriggerManagerFD::GetInstance()
        .Get_Trigger_Connect_To_Field().Erase( m_nKey_Connect_To_Field );
    GLTriggerManagerFD::GetInstance()
        .Get_Trigger_CTF_Begin().Erase( m_nKey_CTF_Begin );
    GLTriggerManagerFD::GetInstance()
        .Get_Trigger_CTF_End().Erase( m_nKey_CTF_End );
    GLTriggerManagerFD::GetInstance()
        .Get_Trigger_CTF_New_Character().Erase( m_nKey_CTF_New_Character );
    GLTriggerManagerFD::GetInstance()
        .Get_Trigger_CTF_Quit_Character().Erase( m_nKey_CTF_Quit_Character );
    GLTriggerManagerFD::GetInstance()
        .Get_Trigger_CTF_Changed_UserCount().Erase( m_nKey_CTF_Changed_UserCount );
	GLTriggerManagerFD::GetInstance()
		.Get_Trigger_CTF_Rebirth_Character().Erase(m_nKey_CTF_Rebirth_Character);
}

void GLBuffManagerFD::ToDo_BindTrigger()
{
    using namespace triggermanager;

    m_nKey_Connect_To_Field = GLTriggerManagerFD::GetInstance()
        .Get_Trigger_Connect_To_Field().Register( boost::bind(&GLBuffManagerFD::Trigger_Connect_To_Field, this, _1) );
    m_nKey_CTF_Begin = GLTriggerManagerFD::GetInstance()
        .Get_Trigger_CTF_Begin().Register( boost::bind(&GLBuffManagerFD::Trigger_CTF_Begin, this) );
    m_nKey_CTF_End = GLTriggerManagerFD::GetInstance()
        .Get_Trigger_CTF_End().Register( boost::bind(&GLBuffManagerFD::Trigger_CTF_End, this, _1, _2) );
    m_nKey_CTF_New_Character = GLTriggerManagerFD::GetInstance()
        .Get_Trigger_CTF_New_Character().Register( boost::bind(&GLBuffManagerFD::Trigger_CTF_New_Character, this, _1) );
    m_nKey_CTF_Quit_Character = GLTriggerManagerFD::GetInstance()
        .Get_Trigger_CTF_Quit_Character().Register( boost::bind(&GLBuffManagerFD::Trigger_CTF_Quit_Character, this, _1) );
    m_nKey_CTF_Changed_UserCount = GLTriggerManagerFD::GetInstance()
        .Get_Trigger_CTF_Changed_UserCount().Register( boost::bind(&GLBuffManagerFD::Trigger_CTF_Changed_UserCount, this, _1) );
	m_nKey_CTF_Rebirth_Character = GLTriggerManagerFD::GetInstance()
		.Get_Trigger_CTF_Rebirth_Character().Register(boost::bind(&GLBuffManagerFD::Trigger_CTF_Rebirth_Character, this, _1));
}

void GLBuffManagerFD::Trigger_CTF_Rebirth_Character(GLChar* pChar)
{
	if(!pChar)
	{
		sc::writeLogError("GLBuffManagerFD::Trigger_CTF_Rebirth_Character() - pChar is null");
		return;
	}
	if(!pChar->IsCTFPlayerNotCheckMap())
	{
		sc::writeLogError("GLBuffManagerFD::Trigger_CTF_Rebirth_Character() - pChar is not CTFPlayer");
		ResetBuff(pChar, (const DWORD)SKILLFACT_INDEX_CTF_NORMAL_REWARD0);
		ResetBuff(pChar, (const DWORD)SKILLFACT_INDEX_CTF_NORMAL_REWARD1);
		ResetBuff(pChar, (const DWORD)SKILLFACT_INDEX_CTF_NORMAL_REWARD2);
		return;
	}
    ////예외처리
	VECBUFF vecRebirthBuff;
	Sqrat::Array arrRebirthBuff;

	arrRebirthBuff = m_fnSQGetCTFBuffRebirth.Evaluate<Sqrat::Array>(); //스크립트에서 값 가져오기

   
	Sqrat::GetVectorFromArray(arrRebirthBuff, &vecRebirthBuff);//배열에서 벡터로 
	
	__time64_t tExpiredTime = 0;	
	
	VECBUFF_ITER Buffiter = vecRebirthBuff.begin();
	VECBUFF_ITER Buffiter_end = vecRebirthBuff.end();	

	tExpiredTime = CTime::GetCurrentTime().GetTime();
	for(;Buffiter != Buffiter_end; ++Buffiter)
	{
		SQBuff& sqBuff = (*Buffiter);
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData(sqBuff.sID.wMainID, sqBuff.sID.wSubID);	
		__time64_t tTime = (__time64_t)pSkill->m_sAPPLY.sDATA_LVL->fLIFE; //유지시간
		if( ReceiveBuff(pChar,sqBuff.sID, sqBuff.wLEVEL, sqBuff.Make_Time(tExpiredTime + tTime), EMSKILLFACT_TYPE(EMSKILLFACT_TYPE_SYSTEM)) == false )
			sc::writeLogError( std::string("[CTF BUFF ERROR] GLBuffManagerFD::Trigger_CTF_Rebirth_Character()") );
	}
}

void GLBuffManagerFD::Trigger_Connect_To_Field( GLChar* pChar )
{
    if ( GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        //! 현재 CTF 승리학원을 가져온다.
        ENUM_SCHOOL emCTFVictorySchool = m_pGaeaServer->GetCTFAuthedSchool();

        //! 승리학원이 아니면 버프를 리셋
        if ( ENUM_SCHOOL(pChar->m_wSchool) != emCTFVictorySchool )
        {
            ResetBuff( pChar, m_wCTFBuffMID, EMSKILLFACT_TYPE(EMSKILLFACT_TYPE_SYSTEM) );
            goto CTF_END;
        }

        //! 이미 보상 버프가 있다면
        MIN_STATIC_ASSERT(SKILLFACT_INDEX_CTF_NORMAL_REWARD0 < SKILLFACT_SIZE);

        bool bExistBuffTypePlayer =  false;;

        if( pChar->m_sSKILLFACT[SKILLFACT_INDEX_CTF_NORMAL_REWARD0].sNATIVEID != SNATIVEID(false) )
		{
            //! 버프가 변경되었을 수도 있으니 버프를 새로 받는다.
            for( DWORD i = SKILLFACT_INDEX_SYSTEM_START; i < SKILLFACT_INDEX_SYSTEM_END; i++ )
            {
                SNATIVEID sSkillID = pChar->m_sSKILLFACT[i].sNATIVEID;
                WORD wLEVEL = pChar->m_sSKILLFACT[i].wLEVEL;

                EMCTF_REWARD_BUFF_TYPE emType =
                    GetCTFRewardBuffType( SQBuff(sSkillID.dwID, wLEVEL, _SKILLFACT_INFINITY, true) );

                if( emType == EMCTF_REWARD_BUFF_TYPE_PLAYER )
                {
                    bExistBuffTypePlayer = true;
                    break;
                } //if
            } //for
        } //if

        ResetBuff( pChar, m_wCTFBuffMID, EMSKILLFACT_TYPE(EMSKILLFACT_TYPE_SYSTEM) );

        //! 버프 파기 시간을 가져온다
        __time64_t tExpiredTime = CTime::GetCurrentTime().GetTime() + m_pGaeaServer->GetCTFStartCycleS() + m_pGaeaServer->GetRewardTimeM() * 60;

        //! 보상 버프를 지급한다.
        DWORD nSize = static_cast<DWORD>(m_pGaeaServer->GetCTFRewardBuffSize());

        CTF_REWARD_BUFF sRewardBuff;

        for( DWORD i=0; i<nSize; ++i )
        {
            m_pGaeaServer->GetCTFRewardBuff( i, sRewardBuff );

            SQBuff		sqBuff(sRewardBuff.dwID, sRewardBuff.wLevel, _SKILLFACT_INFINITY, true);

            EMCTF_REWARD_BUFF_TYPE emType = GetCTFRewardBuffType( sqBuff );

            switch(emType)
            {
            case EMCTF_REWARD_BUFF_TYPE_PLAYER:
                if ( bExistBuffTypePlayer )
                    if( ReceiveBuff( pChar, sqBuff.sID, sqBuff.wLEVEL, sqBuff.Make_Time(tExpiredTime), EMSKILLFACT_TYPE(EMSKILLFACT_TYPE_SYSTEM) ) == false )
						sc::writeLogError( std::string("[CTF BUFF ERROR] GLBuffManagerFD::Trigger_Connect_To_Field() : EMCTF_REWARD_BUFF_TYPE_PLAYER") );
                break;

            case EMCTF_REWARD_BUFF_TYPE_SCHOOL:
            case EMCTF_REWARD_BUFF_TYPE_DEFAULT:
				if( ReceiveBuff( pChar, sqBuff.sID, sqBuff.wLEVEL, sqBuff.Make_Time(tExpiredTime), EMSKILLFACT_TYPE(EMSKILLFACT_TYPE_SYSTEM) ) == false )
					sc::writeLogError( std::string("[CTF BUFF ERROR] GLBuffManagerFD::Trigger_Connect_To_Field() : EMCTF_REWARD_BUFF_TYPE_DEFAULT") );
                break;
            }; //switch
        } //for
    } //if

CTF_END:
    return;

}

void GLBuffManagerFD::Trigger_CTF_Begin()
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return;
    }

    //! 밸런스 버프를 리셋한다
    for ( UINT i=0; i<SCHOOL_NUM; ++i )
    {
        m_vecCTFBuffBalance[i].clear();
    }

    //! 캐릭터들을 돌면서 점령전 관련 버프를 삭제한다
    GLCHARLIST& cCharList = m_pGaeaServer->GetCharList();

    GLCHARNODE* pNode = cCharList.m_pHead;
    for ( ; pNode; pNode=pNode->pNext )
    {
        GLChar* pChar = pNode->Data;
        if (!pChar)
            continue;

        MIN_STATIC_ASSERT(SKILLFACT_INDEX_CTF_NORMAL_REWARD0 < SKILLFACT_SIZE);

        //! CTF 버프를 받은적이 없다면 삭제하지 않는다.
        if (pChar->m_sSKILLFACT[SKILLFACT_INDEX_CTF_NORMAL_REWARD0].sNATIVEID != SNATIVEID(false))
            ResetBuff( pChar, m_wCTFBuffMID, EMSKILLFACT_TYPE(EMSKILLFACT_TYPE_SYSTEM) );
    }
}

void GLBuffManagerFD::Trigger_CTF_End( ENUM_SCHOOL emVictorySchool, UINT nRewardBuffs )
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return;
    }

    //! 밸런스 버프를 리셋한다
    for ( UINT i=0; i<SCHOOL_NUM; ++i )
    {
        m_vecCTFBuffBalance[i].clear();
    }

    //! CTF에서 지급할 버프 리스트를 가져온다.
    VECBUFF vecCTFBuffReward_Player;
    VECBUFF vecCTFBuffReward_School;
    VECBUFF vecCTFBuffReward_Default;
    {
        CTF_REWARD_BUFF sRewardBuff;

        for ( DWORD i=0; i<nRewardBuffs; ++i  )
        {
            m_pGaeaServer->GetCTFRewardBuff( i, sRewardBuff );

            SQBuff sqBuff(sRewardBuff.dwID, sRewardBuff.wLevel, _SKILLFACT_INFINITY, true);

            EMCTF_REWARD_BUFF_TYPE emType = GetCTFRewardBuffType( sqBuff );

            switch(emType)
            {
            case EMCTF_REWARD_BUFF_TYPE_PLAYER: vecCTFBuffReward_Player.push_back( sqBuff ); break;
            case EMCTF_REWARD_BUFF_TYPE_SCHOOL: vecCTFBuffReward_School.push_back( sqBuff ); break;
            case EMCTF_REWARD_BUFF_TYPE_DEFAULT: vecCTFBuffReward_Default.push_back( sqBuff ); break;
            };
        }
    }

    //! 버프 파기 시간을 계산한다
    __time64_t tExpiredTime = 0;
    {
        tExpiredTime = CTime::GetCurrentTime().GetTime() + m_pGaeaServer->GetCTFStartCycleS() + m_pGaeaServer->GetRewardTimeM()*60;
    }

    //! 모든 승리학원 유저에게 버프 지급
    GLCHARLIST& cCharList = m_pGaeaServer->GetCharList();

    GLCHARNODE* pNode = cCharList.m_pHead;
    for ( ; pNode; pNode=pNode->pNext )
    {
        GLChar* pChar = pNode->Data;

        if ( !pChar )
        {
            continue;
        }

        //! 지급된 모든 점령전 버프를 삭제
        ResetBuff( pChar, m_wCTFBuffMID, EMSKILLFACT_TYPE(EMSKILLFACT_TYPE_SYSTEM) );

        if ( ENUM_SCHOOL(pChar->m_wSchool) != emVictorySchool )
        {
            continue;
        }

        VECBUFF_ITER iter;
        
        iter = vecCTFBuffReward_School.begin();
        for ( ; iter!=vecCTFBuffReward_School.end(); ++iter )
        {
            SQBuff& sqBuff = (*iter);
            if( ReceiveBuff( pChar, sqBuff.sID, sqBuff.wLEVEL, sqBuff.Make_Time(tExpiredTime), EMSKILLFACT_TYPE(EMSKILLFACT_TYPE_SYSTEM) ) == false )
				sc::writeLogError( std::string("[CTF BUFF ERROR] GLBuffManagerFD::Trigger_CTF_End() : school buff") );
        }

        iter = vecCTFBuffReward_Default.begin();
        for ( ; iter!=vecCTFBuffReward_Default.end(); ++iter )
        {
            SQBuff& sqBuff = (*iter);
            if( ReceiveBuff( pChar, sqBuff.sID, sqBuff.wLEVEL, sqBuff.Make_Time(tExpiredTime), EMSKILLFACT_TYPE(EMSKILLFACT_TYPE_SYSTEM) ) == false )
				sc::writeLogError( std::string("[CTF BUFF ERROR] GLBuffManagerFD::Trigger_CTF_End() : reward buff") );
        }
    }

    //! 해당 참여한 승리학원 유저에게 버프 지급
    if ( m_pGaeaServer->IsCTFBattleGround() )
    {
        GLLandMan* pLandMan = m_pGaeaServer->GetLand( m_pGaeaServer->GetCTFPvPMapID() );

        if ( !pLandMan )
        {
            GASSERT(!"GLBuffManagerFD::Trigger_CTF_End() - pLandMan is null");
            return;
        }

        //! 해당 참여한 승리학원 유저에게 버프 지급
        pNode = pLandMan->m_GlobPCList.m_pHead;
        for ( ; pNode; pNode=pNode->pNext )
        {
            GLChar* pChar = pNode->Data;

            if ( !pChar )
            {
                continue;
            }

			// 죽어있다면 강제로 살린다;
			if ( !pChar->IsValidBody() )
			{
				pChar->SetAlive();
			}

            WORD wSchool = pChar->GetSchool();

            if ( ENUM_SCHOOL(pChar->m_wSchool) != emVictorySchool )
            {
                continue;
            }

            VECBUFF_ITER iter = vecCTFBuffReward_Player.begin();
            for ( ; iter!=vecCTFBuffReward_Player.end(); ++iter )
            {
                SQBuff& sqBuff = (*iter);
                if( ReceiveBuff( pChar, sqBuff.sID, sqBuff.wLEVEL, sqBuff.Make_Time(tExpiredTime), EMSKILLFACT_TYPE(EMSKILLFACT_TYPE_SYSTEM) ) == false )
					sc::writeLogError( std::string("[CTF BUFF ERROR] GLBuffManagerFD::Trigger_CTF_End() : winner buff") );
            }
        }
    }
}

void GLBuffManagerFD::Trigger_CTF_New_Character( GLChar* pChar )
{
    if ( !pChar )
    {
        sc::writeLogError(std::string("GLBuffManagerFD::Trigger_CTF_Join_Character() - pChar is Null"));
        return;
    }

    WORD wSchool = pChar->GetSchool();

    if ( m_vecCTFBuffBalance[wSchool].empty() )
    {
        return;
    }

    //! 밸런스 버프를 지급
    //! 버프 파기 시간을 계산한다
    __time64_t tExpiredTime = 0;
    {
        tExpiredTime = CTime::GetCurrentTime().GetTime() + m_pGaeaServer->GetCTFPlayTimeM()*60;
    }

    VECBUFF_ITER iter = m_vecCTFBuffBalance[wSchool].begin();
    VECBUFF_ITER iterEnd = m_vecCTFBuffBalance[wSchool].end();

    for ( ; iter!=iterEnd; ++iter )
    {
        SQBuff& sqBuff = (*iter);
        if( ReceiveBuff( pChar, sqBuff.sID, sqBuff.wLEVEL, sqBuff.GetTime(tExpiredTime), EMSKILLFACT_TYPE(EMSKILLFACT_TYPE_SYSTEM) ) == false )
			sc::writeLogError( std::string("[CTF BUFF ERROR] GLBuffManagerFD::Trigger_CTF_New_Character()") );
    }
}

void GLBuffManagerFD::Trigger_CTF_Quit_Character( GLChar* pChar )
{
    if ( !pChar )
    {
        sc::writeLogError(std::string("GLBuffManagerFD::Trigger_CTF_Exit_Character() - pChar is Null"));
        return;
    }

	if ( m_pGaeaServer->GetCTFAuthedSchool() == ENUM_SCHOOL(pChar->m_wSchool) )
	{
		if ( m_pGaeaServer->GetCTFState() >= CTF_STATE_DONE )
		{
			return;
		}		
	}

    //! 지급된 모든 점령전 버프를 삭제
    ResetBuff( pChar, m_wCTFBuffMID, EMSKILLFACT_TYPE(EMSKILLFACT_TYPE_SYSTEM) );
}

void GLBuffManagerFD::Trigger_CTF_Changed_UserCount( UINT* pSchoolArray )
{
    if ( !pSchoolArray )
    {
        sc::writeLogError( std::string("GLBuffManagerFD::Trigger_CTF_Changed_UserCount() - pSchoolArray is Null") );
        return;
    }

    //! 해당 점령전 맵을 가져온다.
    GLLandMan* pLandMan = m_pGaeaServer->GetLand( m_pGaeaServer->GetCTFPvPMapID() );

    if ( !pLandMan )
    {
        sc::writeLogError( std::string("GLBuffManagerFD::Trigger_CTF_Changed_UserCount() - Landman is Null") );
        return;
    }

    //! 참여 할 수있는 최대 학원생을 구함
    UINT nMaxNumOfPlayers = m_pGaeaServer->GetCTFMaxNumOfPlayers();

    //! 각 학원에 지급해야할 밸런스 버프를 계산
    static VECBUFF vecBalanceBuff[SCHOOL_NUM];
    Sqrat::Array arrBalanceBuff[SCHOOL_NUM];
    static bool bChanged[SCHOOL_NUM] = {};

    {
        arrBalanceBuff[SCHOOL_SM] = m_fnSQGetCTFBuffBalance.Evaluate<Sqrat::Array>(pSchoolArray[SCHOOL_SM], nMaxNumOfPlayers);
        Sqrat::GetVectorFromArray( arrBalanceBuff[SCHOOL_SM], &vecBalanceBuff[SCHOOL_SM] );

        if ( vecBalanceBuff[SCHOOL_SM].size() != m_vecCTFBuffBalance[SCHOOL_SM].size() )
        {
            bChanged[SCHOOL_SM] = true;
        }
        else
        {
            bChanged[SCHOOL_SM] = !std::equal( vecBalanceBuff[SCHOOL_SM].begin(), vecBalanceBuff[SCHOOL_SM].end(), m_vecCTFBuffBalance[SCHOOL_SM].begin() );
        }

        if ( bChanged[SCHOOL_SM] )
        {
            m_vecCTFBuffBalance[SCHOOL_SM] = vecBalanceBuff[SCHOOL_SM];
        }
    }

    {
        arrBalanceBuff[SCHOOL_HA] = m_fnSQGetCTFBuffBalance.Evaluate<Sqrat::Array>(pSchoolArray[SCHOOL_HA], nMaxNumOfPlayers);
        Sqrat::GetVectorFromArray( arrBalanceBuff[SCHOOL_HA], &vecBalanceBuff[SCHOOL_HA] );

        if ( vecBalanceBuff[SCHOOL_HA].size() != m_vecCTFBuffBalance[SCHOOL_HA].size() )
        {
            bChanged[SCHOOL_HA] = true;
        }
        else
        {
            bChanged[SCHOOL_HA] = !std::equal( vecBalanceBuff[SCHOOL_HA].begin(), vecBalanceBuff[SCHOOL_HA].end(), m_vecCTFBuffBalance[SCHOOL_HA].begin() );
        }

        if ( bChanged[SCHOOL_HA] )
        {
            m_vecCTFBuffBalance[SCHOOL_HA] = vecBalanceBuff[SCHOOL_HA];
        }
    }

    {
        arrBalanceBuff[SCHOOL_BH] = m_fnSQGetCTFBuffBalance.Evaluate<Sqrat::Array>(pSchoolArray[SCHOOL_BH], nMaxNumOfPlayers);
        Sqrat::GetVectorFromArray( arrBalanceBuff[SCHOOL_BH], &vecBalanceBuff[SCHOOL_BH] );

        if ( vecBalanceBuff[SCHOOL_BH].size() != m_vecCTFBuffBalance[SCHOOL_BH].size() )
        {
            bChanged[SCHOOL_BH] = true;
        }
        else
        {
            bChanged[SCHOOL_BH] = !std::equal( vecBalanceBuff[SCHOOL_BH].begin(), vecBalanceBuff[SCHOOL_BH].end(), m_vecCTFBuffBalance[SCHOOL_BH].begin() );
        }

        if ( bChanged[SCHOOL_BH] )
        {
            m_vecCTFBuffBalance[SCHOOL_BH] = vecBalanceBuff[SCHOOL_BH];
        }
    }

    //! 전부 바뀌지 않았으면
    if ( !bChanged[SCHOOL_SM] && !bChanged[SCHOOL_HA] && !bChanged[SCHOOL_BH] )
    {
        return;
    }

    //! 각 학원생을 돌면서 밸런스 버프를 지급
    //! 버프 파기 시간을 계산한다
    __time64_t tExpiredTime = 0;
    {
        tExpiredTime = CTime::GetCurrentTime().GetTime() + m_pGaeaServer->GetCTFPlayTimeM()*60;
    }

    GLCHARNODE* pNode = pLandMan->m_GlobPCList.m_pHead;
    for ( ; pNode; pNode=pNode->pNext )
    {
        GLChar* pChar = pNode->Data;

        if ( !pChar )
        {
            continue;
        }

        WORD wSchool = pChar->GetSchool();

        if ( !bChanged[wSchool] )
        {
            continue;
        }

        //! 지급된 모든 점령전 버프를 삭제
        ResetBuff( pChar, m_wCTFBuffMID, EMSKILLFACT_TYPE(EMSKILLFACT_TYPE_SYSTEM) );

        //! 다시 버프를 지급
        VECBUFF_ITER iter = m_vecCTFBuffBalance[wSchool].begin();
        VECBUFF_ITER iterEnd = m_vecCTFBuffBalance[wSchool].end();

        for ( ; iter!=iterEnd; ++iter )
        {
            SQBuff& sqBuff = (*iter);
            if( ReceiveBuff( pChar, sqBuff.sID, sqBuff.wLEVEL, sqBuff.GetTime(tExpiredTime), EMSKILLFACT_TYPE(EMSKILLFACT_TYPE_SYSTEM) ) == false )
				sc::writeLogError( std::string("[CTF BUFF ERROR] GLBuffManagerFD::Trigger_CTF_Changed_UserCount() - ballance buff") );
        }
    }
}

const bool GLBuffManagerFD::ReceiveBuff( GLChar* pChar,
                                      const SNATIVEID& skill_id,
                                      const WORD wlevel,
                                      const SSKILLFACT::STIME& sTime,
                                      EMSKILLFACT_TYPE emSkillFactType,
                                      const DWORD dwCount )
{
    if ( !pChar )
    {
        sc::writeLogError( std::string("GLBuffManagerFD::ReceiveBuff() - pChar is null") );
        return false;
    }

    DWORD dwSlot = 0;
    if ( !pChar->RECEIVE_SKILLFACT( skill_id, wlevel, dwSlot, pChar->GetTargetID(), sTime, dwCount, emSkillFactType ) )
    {
        return false;
    }

    if ( dwSlot == UINT_MAX )
    {
        return false;
    }

    GLMSG::SNETPC_SKILLHOLD_BRD NetMsgBRD;
    NetMsgBRD.emCrow    = CROW_PC;
    NetMsgBRD.dwID		= pChar->GetGaeaID();
    NetMsgBRD.skill_id	= skill_id;
    NetMsgBRD.wLEVEL    = wlevel;;
    NetMsgBRD.wSELSLOT	= static_cast<WORD>(dwSlot);
    NetMsgBRD.sTIME     = sTime;
    NetMsgBRD.sID       = pChar->GetTargetID();

    // 스킬대상과 주변 클라이언트들에게 메세지 전송.
    pChar->SendToClient(&NetMsgBRD );
    pChar->SendMsgViewAround( &NetMsgBRD );

    return true;
}

const bool GLBuffManagerFD::ResetBuff( GLChar* pChar, const DWORD dwIndex )
{
    if (!pChar)
    {
        sc::writeLogError(std::string("GLBuffManagerFD::ResetBuff() - pChar is null"));
        return false;
    }

    if (dwIndex >= SKILLFACT_SIZE)
        return false;

    if ( pChar->m_sSKILLFACT[dwIndex].sNATIVEID != SNATIVEID(false) )
    {
        pChar->m_sSKILLFACT[dwIndex].RESET();

        GLMSG::SNETPC_SKILLHOLD_RS_BRD NetMsgSkillBrd;
        NetMsgSkillBrd.dwID            = pChar->GetGaeaID();
        NetMsgSkillBrd.emCrow          = pChar->GetCrow();
        NetMsgSkillBrd.bRESET[dwIndex] = true;
        pChar->SendToClient(&NetMsgSkillBrd);
        pChar->SendMsgViewAround(&NetMsgSkillBrd);
    }

    return true;
}

const bool GLBuffManagerFD::ResetBuff( GLChar* pChar, const WORD wMid, EMSKILLFACT_TYPE emSkillFactType )
{
    if ( !pChar )
    {
        sc::writeLogError(std::string("GLBuffManagerFD::ResetBuff() - pChar is null"));
        return false;
    }

    DWORD dwBeginIndex = 0;
    DWORD dwEndIndex   = SKILLFACT_SIZE;

    switch ( emSkillFactType )
    {
    case EMSKILLFACT_TYPE_NONE:
        {
            dwBeginIndex = 0;
            dwEndIndex   = SKILLFACT_SIZE;
        }
        break;

    case EMSKILLFACT_TYPE_NORMAL:
        {
            dwBeginIndex = SKILLFACT_INDEX_NORMAL_START;
            dwEndIndex   = SKILLFACT_INDEX_NORMAL_END;
        }
        break;

    case EMSKILLFACT_TYPE_ADDITIONAL:
        {
            dwBeginIndex = SKILLFACT_INDEX_ADDITIONAL_START;
            dwEndIndex   = SKILLFACT_INDEX_ADDITIONAL_END;
        }
        break;

    case EMSKILLFACT_TYPE_LUNCHBOX:
        {
            dwBeginIndex = SKILLFACT_INDEX_LUNCHBOX_START;
            dwEndIndex   = SKILLFACT_INDEX_LUNCHBOX_END;
        }
        break;

    case EMSKILLFACT_TYPE_SYSTEM:
        {
            dwBeginIndex = SKILLFACT_INDEX_SYSTEM_START;
            dwEndIndex   = SKILLFACT_INDEX_SYSTEM_END;
        }
        break;

    };

    GLMSG::SNETPC_SKILLHOLD_RS_BRD NetMsgSkillBrd;
    bool bReset = false;

    for ( DWORD i=dwBeginIndex; i<dwEndIndex; ++i )
    {
        if (i >= SKILLFACT_SIZE || wMid != pChar->m_sSKILLFACT[i].sNATIVEID.Mid())
            continue;

        pChar->m_sSKILLFACT[i].RESET();

        NetMsgSkillBrd.bRESET[i] = true;
        bReset = true;
    }   

    if ( bReset )
    {
        NetMsgSkillBrd.dwID    = pChar->GetGaeaID();
        NetMsgSkillBrd.emCrow  = pChar->GetCrow();
        pChar->SendToClient(&NetMsgSkillBrd);
        pChar->SendMsgViewAround(&NetMsgSkillBrd);
    }

    return true;

}

const bool GLBuffManagerFD::ResetBuff( GLChar* pChar, const SNATIVEID* arrskill_id, const DWORD dwSize, EMSKILLFACT_TYPE emSkillFactType )
{
    if ( !pChar )
    {
        sc::writeLogError(std::string("GLBuffManagerFD::ResetBuff() - pChar is null"));
        return false;
    }

    if ( !arrskill_id )
    {
        sc::writeLogError(std::string("GLBuffManagerFD::ResetBuff() - arrskill_id is null"));
        return false;
    }

    GLMSG::SNETPC_SKILLHOLD_RS_BRD NetMsgSkillBrd;
    bool bReset = false;

    for ( DWORD i=0; i<dwSize; ++i )
    {
        DWORD dwSlot = 0;
        if ( !pChar->RESET_SKILLFACT( arrskill_id[i], dwSlot, emSkillFactType ) )
        {
            continue;
        }

        NetMsgSkillBrd.bRESET[dwSlot] = true;
        bReset = true;
    }   

    if ( bReset )
    {
        NetMsgSkillBrd.dwID    = pChar->GetGaeaID();
        NetMsgSkillBrd.emCrow  = pChar->GetCrow();
        pChar->SendToClient(&NetMsgSkillBrd);
        pChar->SendMsgViewAround(&NetMsgSkillBrd);
    }

    return true;
}
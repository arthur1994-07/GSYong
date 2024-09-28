//
//mjeon.activities
//
#include "../pch.h"
#include "./Activity.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//
//abstraction layer for an item of each activity
//
Activity::Activity(WORD wMID, WORD wSID, UINT nValueBase, float fVersion)
	:m_id(SNATIVEID(wMID, wSID))
	,m_nValueBase(nValueBase)
	,m_pLabel(new ACTIVITY_LABEL)
	,m_pReward(new ACTIVITY_REWARD)
{
	Initialize();		
}


Activity::Activity(DWORD dwID, UINT nValueBase, float fVersion)
	:m_id(SNATIVEID(dwID))
	,m_nValueBase(nValueBase)
	,m_pLabel(new ACTIVITY_LABEL)
	,m_pReward(new ACTIVITY_REWARD)
{
	Initialize();
}

Activity::~Activity()
{	
}

void Activity::Initialize()
{
	m_bUpdated		= FALSE;
	m_nValue		= 0;	
	m_nByOrder		= 0;
	m_status		= ACTIVITY_STATUS_INPROGRESS;
	m_fVersion		= 1.0;
	m_nClass		= ACTIVITY_CLASS_LOW;
	m_nNotifyLevel	= ACTIVITY_NOTIFY_AROUND;
	m_pReward.get()->nRewardPoint	= 0;
	
	m_bValidate		= TRUE;
	m_flagSync		= FALSE;
}

void Activity::ResetData()
{
    m_nValue = 0;
    m_bUpdated = FALSE;
    m_status = ACTIVITY_STATUS_INPROGRESS;
}


ENUM_ACTIVITY_CATEGORY Activity::GetType()
{
    switch ( m_id.wMainID )
    {
    case ACTIVITY_MID_LEVEL:       return ACTIVITY_CATEGORY_LEVEL;
    case ACTIVITY_MID_KILLMOB:     return ACTIVITY_CATEGORY_KILL;
    case ACTIVITY_MID_KILLPC:      return ACTIVITY_CATEGORY_KILL;
    case ACTIVITY_MID_MAP:         return ACTIVITY_CATEGORY_MAP;
    case ACTIVITY_MID_TAKE:        return ACTIVITY_CATEGORY_TAKE;
    case ACTIVITY_MID_USEITEM:     return ACTIVITY_CATEGORY_USEITEM;
    case ACTIVITY_MID_ACTIVITY:    return ACTIVITY_CATEGORY_SIZE;
    case ACTIVITY_MID_QUEST:       return ACTIVITY_CATEGORY_SIZE;
    case ACTIVITY_MID_PONIT:       return ACTIVITY_CATEGORY_SIZE;
    default:                       return ACTIVITY_CATEGORY_SIZE;
    }
}

BOOL Activity::IsCompletedOnMap()
{
	if( m_id.wMainID == ACTIVITY_MID_MAP && m_status == ACTIVITY_STATUS_COMPLETED )
		return TRUE;
	
	return FALSE;
}

BOOL Activity::Update()
{
	if (m_status != ACTIVITY_STATUS_INPROGRESS)
	{
		//
		//NOT to be updated
		//
		return FALSE;
	}

	//
	//to be updated
	//

	if (m_id.wMainID == ACTIVITY_MID_TAKE || m_id.wMainID == ACTIVITY_MID_LEVEL)
	{
		//
		//in case of ACTIVITY_MID_TAKE, activity validation is done by MsgActivityDoneItemTake().
		// therefore, just modify the m_nValue at once.
		//

		//
		//in case of ACTIVITY_MID_LEVEL, level is already maintained by character-info.
		// therefore, just handle the completion of it.
		//

		m_nValue = m_nValueBase;

		if(m_nByOrder > 0)
		{
			SetStatus(ACTIVITY_STATUS_ORDERING);
		}
		else
		{
			SetStatus(ACTIVITY_STATUS_COMPLETED);
		}			
	}
	else
	{
		m_nValue++;

		m_nValue = min((m_nValue), m_nValueBase);

		if (m_nValue >= m_nValueBase)
		{
			if(m_nByOrder > 0)
			{
				SetStatus(ACTIVITY_STATUS_ORDERING);
			}
			else
			{
				SetStatus(ACTIVITY_STATUS_COMPLETED);
			}
		}
	}

	m_bUpdated = TRUE;

	return TRUE;
}


BOOL Activity::IsMatched(DWORD dwTargetID)
{
	if (m_id.Mid() == ACTIVITY_MID_LEVEL)
	{
		return (dwTargetID /*current character's level*/ >= m_nValueBase);
	}
	else
	{
        for ( size_t i = 0; i < m_vecTargetID.size(); i++ )
        {
            if ( dwTargetID == m_vecTargetID[i] )
                return true;
        }
	}
    return false;
}


void Activity::SetCompleteDate()
{
	CTime ctCurrentTime = CTime::GetCurrentTime().GetTime();	
	CString csCurrentTime = ctCurrentTime.Format("%Y-%m-%d %H:%M:%S");

	m_strCompleteDate.assign(csCurrentTime.GetString());
}


//
//Reward & Label accessor-methods
//
UINT& Activity::GetRewardPoint()
{
	return m_pReward->nRewardPoint;
}

std::string& Activity::GetRewardTitle()
{
	return m_pReward->strRewardTitle;
}

std::string& Activity::GetTitle()
{
	return m_pLabel->strTitle;
}

std::string& Activity::GetDesc()
{
	return m_pLabel->strDesc;
}

std::string& Activity::GetRewardTitleID()
{
	return m_pReward->strRewardTitleID;
}

std::string& Activity::GetTitleID()
{
	return m_pLabel->strTitleID;
}

std::string& Activity::GetDescID()
{
	return m_pLabel->strDescID;
}


void Activity::SetTitle( std::string strTitle )
{ 
    m_pLabel->strTitle = strTitle;
}

void Activity::SetDesc( std::string strDesc )
{
    m_pLabel->strDesc = strDesc;
}

void Activity::SetRewardPoint ( UINT nPoint )
{
    m_pReward->nRewardPoint = nPoint;
}

void Activity::SetRewardTitle ( std::string strRewardTitle )
{
    m_pReward->strRewardTitle = strRewardTitle;
}

void Activity::SetTitleID( std::string strTitleID )
{
	m_pLabel->strTitleID = strTitleID;
}

void Activity::SetDescID( std::string strDescID )
{
	m_pLabel->strDescID = strDescID;
}

void Activity::SetRewardTitleID( std::string strRewardTitleID )
{
	 m_pReward->strRewardTitleID = strRewardTitleID;
}




namespace COMMENT
{
	std::string ACTIVITY_CLASS[ACTIVITY_CLASS_SIZE] = 
    {
        "상급 특별활동",
        "중급 특별활동",
        "하급 특별활동",
    };

    std::string ACTIVITY_CATEGORY[ACTIVITY_CATEGORY_SIZE] = 
    {
        "달성",
        "처치",
        "수집",
        "접근",
        "사용",
    };

    std::string ACTIVITY_NOTIFY[ACTIVITY_NOTIFY_SIZE] = 
    {
        "없음",
        "주위",
        "서버전체",
        "클럽",
        "클럽및주위"
    };

    std::string ACTIVITY_MID[ACTIVITY_MID_SIZE] = 
    {
        "레벨 달성",
        "몬스터 처치",
        "유저 처치",
        "맵에 접근",
        "아이템 획득",
        "아이템 사용",
        "활동 달성",
        "퀘스트 완료",
        "활동 포인트",
    };

    std::string ACTIVITY_TEXTID[ENUM_ACTIVITY_TEXTID_SIZE] = 
    {
        "ACTIVITY_TITLE_%1%_%2%",
        "ACTIVITY_DESC_%1%_%2%",
        "ACTIVITY_REWARDTITLE_%1%_%2%",
    };
};

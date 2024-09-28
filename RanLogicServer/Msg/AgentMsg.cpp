#include "pch.h"
#include "../AgentServer/GLAgentServer.h"
#include "../Friend/FriendAgent.h"
#include "../Notify/NotifyManAgent.h"
#include "../Product/GLProductAgent.h"
#include "AgentMsg.h"


#include "../../SigmaCore/DebugInclude.h"


GLCHARAG_DATA::GLCHARAG_DATA() :
    m_UserDbNum(0),
    m_dwUserLvl(0),
    m_tPREMIUM(0),
    m_tCHATBLOCK(0),

    m_dwServerID(0),
    m_CharDbNum(0),

    m_emClass(GLCC_FIGHTER_M),
	m_wLevel(0),
    m_wSchool(0),

    m_ClubDbNum(CLUB_NULL),
    m_tSECEDE(0),

    m_nBright(0),
    m_bUseArmSub(FALSE),

    m_dwStartGate(0),
    m_vStartPos(0,0,0),

    m_sSaveMapID(NATIVEID_NULL()),
    m_vSavePos(0,0,0),

    m_emIPBonus(EMIP_BONUS_NONE),

    m_bTracingUser(FALSE)
{
    memset(m_szName, 0, sizeof(m_szName));
    //memset(m_szPhoneNumber, 0, sizeof(m_szPhoneNumber));
    memset(m_szUserName, 0, sizeof(m_szUserName));
    memset(m_szIp, 0, sizeof(m_szIp));

    m_pFriend = new friendlogic::FriendAgent;
    m_pNotify = new gamelogic::NotifyManAgent;
    m_pProduct = new GLProductAgent;
}

GLCHARAG_DATA::~GLCHARAG_DATA()
{
    if (m_pFriend)
    {
        delete m_pFriend;
        m_pFriend = NULL;
    }

    if (m_pNotify)
    {
        delete m_pNotify;
        m_pNotify = NULL;
    }

    if( m_pProduct )
    {
        delete m_pProduct;
        m_pProduct = NULL;

    }
}

GLCHARAG_DATA& GLCHARAG_DATA::operator= ( const GLCHARAG_DATA &cData )
{
	if( this == &cData )
		return *this;

	m_UserDbNum = cData.m_UserDbNum;
	m_dwUserLvl = cData.m_dwUserLvl;
	m_dwServerID = cData.m_dwServerID;
	m_CharDbNum = cData.m_CharDbNum;
    m_tPREMIUM = cData.m_tPREMIUM;
	m_tCHATBLOCK = cData.m_tCHATBLOCK;

	StringCbCopy( m_szName, CHAR_SZNAME, cData.m_szName );
	//StringCbCopy( m_szPhoneNumber, SMS_RECEIVER, cData.m_szPhoneNumber );
	StringCbCopy( m_szUserName, USR_ID_LENGTH+1, cData.m_szUserName );

	m_emClass = cData.m_emClass;
	m_wLevel  = cData.m_wLevel;
	m_wSchool = cData.m_wSchool;

	m_ClubDbNum = cData.m_ClubDbNum;
	m_tSECEDE = cData.m_tSECEDE;

	m_nBright = cData.m_nBright;
	m_bUseArmSub = cData.m_bUseArmSub;

	m_sStartMapID = cData.m_sStartMapID;
	m_dwStartGate = cData.m_dwStartGate;
	m_vStartPos = cData.m_vStartPos;

	m_sSaveMapID = cData.m_sSaveMapID;
	m_vSavePos = cData.m_vSavePos;
	
	//m_mapFriend = cData.m_mapFriend;
	m_vecAttend = cData.m_vecAttend;

    // Friend
    if (m_pFriend)
        delete m_pFriend;
    m_pFriend = new friendlogic::FriendAgent;
    *m_pFriend = *cData.m_pFriend;

    // Notify
    if (m_pNotify)
        delete m_pNotify;
    m_pNotify = new gamelogic::NotifyManAgent;
    *m_pNotify = *cData.m_pNotify;

	return *this;
}

bool GLCHARAG_DATA::AddFriend(const SFRIEND& Info)
{
    return m_pFriend->Add(Info, NULL, NULL);
}

bool GLCHARAG_DATA::AddFriend(const CHA_FRIEND& Info)
{
    return m_pFriend->Add(Info);
}

bool GLCHARAG_DATA::AddFriendGroup(const FRIEND_GROUP& Info)
{
    return m_pFriend->GroupAdd(Info);
}

void GLCHARAG_DATA::SetFriendLevel(DWORD FriendDbNum, WORD FriendLevel)
{
    m_pFriend->SetFriendLevel(FriendDbNum, FriendLevel);
}

void GLCHARAG_DATA::SetFriendMap(DWORD FriendDbNum, const MapID& FriendMap)
{
    m_pFriend->SetFriendMap(FriendDbNum, FriendMap);
}

bool GLCHARAG_DATA::AddNotify(std::tr1::shared_ptr<gamelogic::NotifyBase> spNotify)
{
    return m_pNotify->Add(spNotify);
}
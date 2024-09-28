#include "../pch.h"
#include "../../SigmaCore/gassert.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "./GLClubDefine.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLCLUBMEMBER::GLCLUBMEMBER()
    : m_ChaDbNum(0)
    , m_Online(false)
    , m_Channel(0)
    , m_CurrentMap(false)
    , m_CurrentPos(0,0,0)
    , m_Grade(0)
    , m_ChaLevel(1)
    , m_ChaClass(GLCC_NONE)
{
    //SecureZeroMemory(m_szName, sizeof(m_szName));
    m_LastPlayTime = sc::time::GetCurrentTime();
}

GLCLUBMEMBER::GLCLUBMEMBER(
    DWORD ChaDbNum,
    const char* szChaName,
    DWORD Flags,
    WORD ChaLevel,
    const __time64_t& LastPlayTime,
    bool bOnline,
    DWORD ChaClass)
    : m_ChaDbNum(ChaDbNum)
    , m_ChaLevel(ChaLevel)
    , m_Grade(Flags)    
    , m_Channel(0)
    , m_CurrentMap(false)
    , m_CurrentPos(0,0,0)
    , m_LastPlayTime(LastPlayTime)
    , m_Online(bOnline)
    , m_ChaClass(ChaClass)
{
    SetName(szChaName);
}

GLCLUBMEMBER::GLCLUBMEMBER(
    DWORD _dwID,
    WORD ChaLevel,
    int	_nCHANNEL,
    const MapID& _nidMAP,
    DWORD dwFlags,
    const D3DXVECTOR3& _vecPOS,
    const char* szName,
    bool _bOnline,
    const __time64_t& LastPlayTime,
    DWORD ChaClass)
    : m_ChaDbNum(_dwID)
    , m_ChaLevel(ChaLevel)
    , m_Channel(_nCHANNEL)
    , m_CurrentMap(_nidMAP)
    , m_Grade(dwFlags)
    , m_CurrentPos(_vecPOS)
    , m_Online(_bOnline)
    , m_LastPlayTime(LastPlayTime)
    , m_ChaClass(ChaClass)
{
    SetName(szName);
}

GLCLUBMEMBER::GLCLUBMEMBER(const GLCLUBMEMBER& value)
{
    operator = (value);
}

GLCLUBMEMBER& GLCLUBMEMBER::operator = (const GLCLUBMEMBER& value)
{
    MIN_STATIC_ASSERT(VERSION <= 0x0004);

    if (this == &value)
        return *this;

    m_ChaDbNum     = value.m_ChaDbNum;
    m_Online       = value.m_Online;
    m_Channel      = value.m_Channel;
    m_CurrentMap   = value.m_CurrentMap;
    m_CurrentPos   = value.m_CurrentPos;
    m_ChaName      = value.m_ChaName;
    m_Grade        = value.m_Grade;
    m_ChaLevel     = value.m_ChaLevel;
    m_LastPlayTime = value.m_LastPlayTime;
    m_ChaClass     = value.m_ChaClass;
    return *this;
}

/*
GLCLUBINFO::GLCLUBINFO()
    : m_DbNum(0)
    , m_Country(wb::UNKNOWN_COUNTRY)
    , m_MasterCharDbNum(0)
    , m_dwCDCertifior(0)
    , m_dwRank(0)
    , m_dwMarkVER(0)
    , m_lnIncomeMoney(0)
    , m_lnStorageMoney(0)
    , m_dwBattleWin(0)
    , m_dwBattleLose(0)
    , m_dwBattleDraw(0)
    , m_dwAllianceBattleWin(0)
    , m_dwAllianceBattleLose(0)
    , m_dwAllianceBattleDraw(0)
    , m_MasterLevel(0)
    , m_PublicType(club::PUBLIC_ALL)
{
    MIN_STATIC_ASSERT(VERSION <= 0x0005);

    SecureZeroMemory(m_szName, sizeof(m_szName));
    SecureZeroMemory(m_szMasterName, sizeof(m_szMasterName));
    SecureZeroMemory(m_aryMark, sizeof(m_aryMark));
    SecureZeroMemory(m_szNotice, sizeof(m_szNotice));
    SecureZeroMemory(m_szNoticeChaName, sizeof(m_szNoticeChaName));

    CTime tDisTime(1970,2,1,1,1,1);

    m_tOrganize = tDisTime.GetTime();
    m_tDissolution = tDisTime.GetTime();
    m_tAuthority = tDisTime.GetTime();
    m_tAllianceSec = tDisTime.GetTime();
    m_tAllianceDis = tDisTime.GetTime();
    m_tLastBattle = tDisTime.GetTime();
    m_NoticeDate = tDisTime.GetTime();
}

GLCLUBINFO& GLCLUBINFO::operator = (const GLCLUBINFO& rhs)
{
    MIN_STATIC_ASSERT(VERSION <= 0x0005);

    if (this == &rhs)
        return *this;

    m_DbNum = rhs.m_DbNum;
    m_Country = rhs.m_Country;
    m_MasterLevel = rhs.m_MasterLevel;
    StringCchCopy(m_szName, CLUB_NAME_LENGTH, rhs.m_szName);

    m_MasterCharDbNum = rhs.m_MasterCharDbNum;
    StringCchCopy(m_szMasterName, CHAR_SZNAME, rhs.m_szMasterName);
    m_dwCDCertifior = rhs.m_dwCDCertifior;

    m_dwRank = rhs.m_dwRank;

    m_dwMarkVER = rhs.m_dwMarkVER;
    memcpy(m_aryMark, rhs.m_aryMark, sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY);

    m_tOrganize = rhs.m_tOrganize;
    m_tDissolution = rhs.m_tDissolution;
    m_tAuthority = rhs.m_tAuthority;

    m_tAllianceSec = rhs.m_tAllianceSec;
    m_tAllianceDis = rhs.m_tAllianceDis;

    m_lnIncomeMoney = rhs.m_lnIncomeMoney;
    m_lnStorageMoney = rhs.m_lnStorageMoney;

    m_dwBattleWin = rhs.m_dwBattleWin;
    m_dwBattleLose = rhs.m_dwBattleLose;
    m_dwBattleDraw = rhs.m_dwBattleDraw;
    m_tLastBattle = rhs.m_tLastBattle;

    m_dwAllianceBattleWin = rhs.m_dwAllianceBattleWin;
    m_dwAllianceBattleLose = rhs.m_dwAllianceBattleLose;
    m_dwAllianceBattleDraw = rhs.m_dwAllianceBattleDraw;

    SetNotice(rhs.m_szNotice);

    SetNoticeChaName(rhs.m_szNoticeChaName);
    m_NoticeDate = rhs.m_NoticeDate;

    m_PublicType = rhs.m_PublicType;

    return *this;
}
*/
GLCLUBBATTLE& GLCLUBBATTLE::operator = ( const GLCLUBBATTLE& value )
{
    if (this == &value)
        return *this;

    m_dwCLUBID        = value.m_dwCLUBID;
    m_tStartTime      = value.m_tStartTime;
    m_tEndTime        = value.m_tEndTime;
    m_wKillPointDB    = value.m_wKillPointDB;
    m_wDeathPointDB   = value.m_wDeathPointDB;
    m_wKillPoint      = value.m_wKillPoint;
    m_wDeathPoint     = value.m_wDeathPoint;
    m_wKillPointTemp  = value.m_wKillPointTemp;
    m_wDeathPointTemp = value.m_wDeathPointTemp;
    m_bKillUpdate     = value.m_bKillUpdate;
    m_nBattleFlag     = value.m_nBattleFlag;
    m_bAlliance       = value.m_bAlliance;

    SetName(value.m_szClubName);

    return *this;
}

GLCLUBBATTLE& GLCLUBBATTLE::operator = ( const GLCLUBBATTLE_LOBY& value )
{
    m_dwCLUBID    = value.m_dwCLUBID;
    m_tStartTime  = value.m_tStartTime;
    m_tEndTime    = value.m_tEndTime;
    m_wKillPoint  = value.m_wKillPoint;
    m_wDeathPoint = value.m_wDeathPoint;
    m_bAlliance   = value.m_bAlliance;

    SetName(value.m_szClubName);

    return *this;
}

GLCLUBBATTLE_LOBY& GLCLUBBATTLE_LOBY::operator = ( const GLCLUBBATTLE_LOBY& value )
{
    m_dwCLUBID    = value.m_dwCLUBID;
    m_tStartTime  = value.m_tStartTime;
    m_tEndTime    = value.m_tEndTime;
    m_wKillPoint  = value.m_wKillPoint;
    m_wDeathPoint = value.m_wDeathPoint;
    m_bAlliance   = value.m_bAlliance;

    SetName(value.m_szClubName);

    return *this;
}

GLCLUBBATTLE_LOBY& GLCLUBBATTLE_LOBY::operator = ( const GLCLUBBATTLE& value )
{
    m_dwCLUBID    = value.m_dwCLUBID;
    m_tStartTime  = value.m_tStartTime;
    m_tEndTime    = value.m_tEndTime;
    m_wKillPoint  = value.m_wKillPoint + value.m_wKillPointTemp;
    m_wDeathPoint = value.m_wDeathPoint + value.m_wDeathPointTemp;
    m_bAlliance   = value.m_bAlliance;

    StringCchCopy( m_szClubName, CLUB_NAME_LENGTH, value.m_szClubName );

    return *this;
}

namespace club
{
    //__time64_t ClubLogDeleteTime = 86400; // 24 hour
    __time64_t ClubLogDeleteTime = 43200; // 12 hour
    //__time64_t ClubLogDeleteTime = 21600; //  6 hour    
} // namespace club
#include "../pch.h"
#include "../../SigmaCore/gassert.h"
#include "../WorldBattle/WorldBattleDefine.h"
#include "./ClubAuthGrade.h"
#include "./GLClub.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLClub::GLClub()
    : m_DbNum(0)
    , m_MasterCharDbNum(0)
    , m_dwRank(0)
    , m_dwCDCertifior(0)
    , m_dwMarkVER(0)
    , m_dwconftCLUB(CLUB_NULL)
    , m_fconftTIMER(0)    
    , m_fTIMER_BATTLE(0)    
    , m_dwAlliance(CLUB_NULL)
    , m_dwReqClubID(CLUB_NULL)
    , m_dwBattleArmisticeReq(0)
    , m_dwBattleWin(0)
    , m_dwBattleLose(0)
    , m_dwBattleDraw(0)
    , m_dwAllianceBattleWin(0)
    , m_dwAllianceBattleLose(0)
    , m_dwAllianceBattleDraw(0)
    , m_Country(wb::UNKNOWN_COUNTRY)
    , m_MasterLevel(0)
    , m_PublicType(club::PUBLIC_ALL)
    , m_lnIncomeMoney(0)
    , m_lnStorageMoney(0)
{
    MIN_STATIC_ASSERT(VERSION <= 0x0007);

    m_pGrade = new club::AuthGrade;

    SecureZeroMemory(m_szName,          sizeof(m_szName));
    SecureZeroMemory(m_szMasterName,    sizeof(m_szMasterName));
    SecureZeroMemory(m_szNotice,        sizeof(m_szNotice));
    SecureZeroMemory(m_aryMark,         sizeof(m_aryMark));
    SecureZeroMemory(m_szNoticeChaName, sizeof(m_szNoticeChaName));
    SecureZeroMemory(m_szNoticeNewbie,  sizeof(m_szNoticeNewbie));

    CTime tDisTime(1970,2,1,1,1,1);

    m_tOrganize    = tDisTime.GetTime();
    m_tDissolution = tDisTime.GetTime();
    m_tAuthority   = tDisTime.GetTime();	
    m_tAllianceSec = tDisTime.GetTime();
    m_tAllianceDis = tDisTime.GetTime();	
    m_tLastBattle  = tDisTime.GetTime();
    m_NoticeDate   = tDisTime.GetTime();
}

GLClub::~GLClub()
{
    delete m_pGrade;
}

const GLClub& GLClub::operator = (const GLClub& rhs)
{
    MIN_STATIC_ASSERT(VERSION <= 0x0007);

    if( this == &rhs )
        return *this;

    m_DbNum = rhs.m_DbNum;
    m_Country = rhs.m_Country;
    SetName(rhs.m_szName);

    m_MasterCharDbNum = rhs.m_MasterCharDbNum;
    SetMasterName(rhs.m_szMasterName);
    m_MasterLevel = rhs.m_MasterLevel;

    m_dwCDCertifior = rhs.m_dwCDCertifior;

    m_dwRank = rhs.m_dwRank;

    m_dwMarkVER = rhs.m_dwMarkVER;
    memcpy(m_aryMark, rhs.m_aryMark, sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY);

    m_tOrganize = rhs.m_tOrganize;
    m_tDissolution = rhs.m_tDissolution;
    m_tAuthority = rhs.m_tAuthority;

    m_tAllianceSec = rhs.m_tAllianceSec;
    m_tAllianceDis = rhs.m_tAllianceDis;

    SetNotice(rhs.m_szNotice);
    SetNoticeNewbie(rhs.m_szNoticeNewbie);

    m_mapMembers = rhs.m_mapMembers;
    m_listCDMMembers = rhs.m_listCDMMembers;

    m_dwAlliance = rhs.m_dwAlliance;
    m_setAlliance = rhs.m_setAlliance;

    m_dwconftCLUB = rhs.m_dwconftCLUB;
    m_fconftTIMER = rhs.m_fconftTIMER;
    m_setConftSTART = rhs.m_setConftSTART;
    m_setConftCURRENT = rhs.m_setConftCURRENT;
    m_conftOPT = rhs.m_conftOPT;

    //m_fTIMER_MONEY = rhs.m_fTIMER_MONEY;
    //m_fTIMER_POS = rhs.m_fTIMER_POS;
    m_fTIMER_BATTLE = rhs.m_fTIMER_BATTLE;

    m_mapBattle = rhs.m_mapBattle;
    //m_mapBattle = rhs.m_mapBattle;								//	배틀 정보
    m_dwReqClubID = rhs.m_dwReqClubID;
    m_dwBattleWin = rhs.m_dwBattleWin;
    m_dwBattleLose = rhs.m_dwBattleLose;
    m_dwBattleDraw = rhs.m_dwBattleDraw;
    m_tLastBattle = rhs.m_tLastBattle;

    m_dwAllianceBattleWin = rhs.m_dwAllianceBattleWin;
    m_dwAllianceBattleLose = rhs.m_dwAllianceBattleLose;
    m_dwAllianceBattleDraw = rhs.m_dwAllianceBattleDraw;

    *m_pGrade = *rhs.m_pGrade;
    
    m_NoticeDate = rhs.m_NoticeDate;
    SetNoticeChaName(rhs.m_szNoticeChaName);

    m_PublicType = rhs.m_PublicType;

    m_lnIncomeMoney  = rhs.m_lnIncomeMoney;
    m_lnStorageMoney = rhs.m_lnStorageMoney;

    m_AutoKickOut = rhs.m_AutoKickOut;

    return *this;
}
/*
void GLClub::SetInfo(const GLCLUBINFO& value)
{
    MIN_STATIC_ASSERT(GLCLUBINFO::VERSION <= 0x0005);

    m_DbNum = value.m_DbNum;
    m_Country = value.m_Country;
    SetName(value.m_szName);

    m_MasterCharDbNum = value.m_MasterCharDbNum;
    SetMasterName(value.m_szMasterName);
    m_MasterLevel = value.m_MasterLevel;

    m_dwCDCertifior = value.m_dwCDCertifior;

    m_dwRank = value.m_dwRank;

    m_dwMarkVER = value.m_dwMarkVER;
    memcpy ( m_aryMark, value.m_aryMark, sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY );

    m_tOrganize = value.m_tOrganize;
    m_tDissolution = value.m_tDissolution;
    m_tAuthority = value.m_tAuthority;

    m_tAllianceSec = value.m_tAllianceSec;
    m_tAllianceDis = value.m_tAllianceDis;

    SetNotice(value.m_szNotice);

    //m_lnIncomeMoney = value.m_lnIncomeMoney;
    //m_lnStorageMoney = value.m_lnStorageMoney;

    m_dwBattleWin = value.m_dwBattleWin;
    m_dwBattleLose = value.m_dwBattleLose;
    m_dwBattleDraw = value.m_dwBattleDraw;

    m_dwAllianceBattleWin = value.m_dwAllianceBattleWin;
    m_dwAllianceBattleLose = value.m_dwAllianceBattleLose;
    m_dwAllianceBattleDraw = value.m_dwAllianceBattleDraw;

    m_NoticeDate = value.m_NoticeDate;
    SetNoticeChaName(value.m_szNoticeChaName);

    m_PublicType = value.m_PublicType;
}
*/
bool GLClub::DoDissolution(__time64_t tDISSOLUTION)
{
    m_tDissolution = tDISSOLUTION;
    return true;
}

bool GLClub::DoDissolutionCancel ()
{
    CTime tDisTime(1970,2,1,1,1,1);
    m_tDissolution = tDisTime.GetTime();
    return true;
}

bool GLClub::IsRegDissolution()
{
    CTime tDisTime(m_tDissolution);
    return tDisTime.GetYear() != 1970;
}

bool GLClub::CheckDissolution()
{
    if (!IsRegDissolution())
        return false;

    CTime tCurTime = CTime::GetCurrentTime();
    return tCurTime>=m_tDissolution;
}

bool GLClub::GetDissolution( CTime &tDissolution )
{
	tDissolution = m_tDissolution;
	CTime tCurTime = CTime::GetCurrentTime();
	if( tCurTime < m_tDissolution )
		return true;
	else
		return false;
}

void GLClub::RESET()
{
    MIN_STATIC_ASSERT(VERSION <= 0x0007);

    m_DbNum = 0;
    m_Country = wb::UNKNOWN_COUNTRY;
    m_MasterCharDbNum = 0;
    m_MasterLevel = 0;
    m_dwRank = 0;
    m_dwCDCertifior = 0;

    m_dwMarkVER = 0;
    m_tOrganize = 0;
    m_tDissolution = 0;

    memset(m_szName,          0, sizeof(char) * CHAR_SZNAME);
    memset(m_szMasterName,    0, sizeof(char) * CHAR_SZNAME);
    memset(m_szNotice,        0, sizeof(char) * EMCLUB_NOTICE_LEN);
    memset(m_szNoticeNewbie,  0, sizeof(char) * EMCLUB_NOTICE_LEN);
    memset(m_szNoticeChaName, 0, sizeof(char) * CHAR_SZNAME);

    CTime tDisTime(1970,2,1,1,1,1);
    m_tDissolution = tDisTime.GetTime();
    m_NoticeDate = tDisTime.GetTime();

    m_mapMembers.clear();
    m_listCDMMembers.clear();

    m_dwAlliance = CLUB_NULL;
    m_setAlliance.clear();	

    m_setConftSTART.clear();
    m_setConftCURRENT.clear();
    m_conftOPT.RESET ();

    //m_fTIMER_POS = 0;
    //m_fTIMER_MONEY = 0;
    m_fTIMER_BATTLE = 0;

    //m_bStorageLoadFromDb = false;
    //m_lnIncomeMoney = 0;
    
    m_dwReqClubID = CLUB_NULL;
    m_dwBattleArmisticeReq = 0;
    m_mapBattle.clear();
    m_mapBattleDel.clear();

    m_dwBattleWin = 0;
    m_dwBattleLose = 0;
    m_dwBattleDraw = 0;
    m_tLastBattle = tDisTime.GetTime();

    m_dwAllianceBattleWin = 0;
    m_dwAllianceBattleLose = 0;
    m_dwAllianceBattleDraw = 0;

    m_PublicType = club::PUBLIC_ALL;

    m_lnIncomeMoney = 0;
    m_lnStorageMoney = 0;

    m_pGrade->Reset();

    m_AutoKickOut.Reset();
}

bool GLClub::AddMember(
    DWORD CharDbNum,
    const char* szCharName,
    DWORD dwFlags,
    WORD ChaLevel,
    const __time64_t& LastPlayTime,
    bool bOnline,
    EMCHARCLASS ChaClass)
{
    MIN_STATIC_ASSERT(GLCLUBMEMBER::VERSION <= 0x0004);

    if (!szCharName)
        return false;

    CLUBMEMBERS_ITER iter = m_mapMembers.find(CharDbNum);
    if (iter != m_mapMembers.end())
        m_mapMembers.erase(iter);

    GLCLUBMEMBER Member(CharDbNum, szCharName, dwFlags, ChaLevel, LastPlayTime, bOnline, ChaClass);
    m_mapMembers.insert(CLUBMEMBERS_VALUE(CharDbNum, Member));
    return true;
}

bool GLClub::AddMemberEx(DWORD ChaDbNum, const GLCLUBMEMBER& Member)
{
    CLUBMEMBERS_ITER iter = m_mapMembers.find(ChaDbNum);
    if (iter != m_mapMembers.end())
        m_mapMembers.erase(iter);
    m_mapMembers.insert(CLUBMEMBERS_VALUE(ChaDbNum, Member));
    return true;
}

bool GLClub::DelMember(DWORD dwCharID)
{
    CLUBMEMBERS_ITER pos = m_mapMembers.find(dwCharID);
    if (pos == m_mapMembers.end())
        return false;

    if (m_dwCDCertifior == dwCharID)
        m_dwCDCertifior = 0;

    m_mapMembers.erase(pos);
    return true;
}

bool GLClub::IsMember(DWORD dwCharID) const
{
    CLUBMEMBERS_CITER pos = m_mapMembers.find(dwCharID);
    if (pos != m_mapMembers.end())
        return true;
    else
        return false;
}

GLCLUBMEMBER* GLClub::GetMember(DWORD dwCharID)
{
    CLUBMEMBERS_ITER pos = m_mapMembers.find(dwCharID);
    if (pos != m_mapMembers.end())
        return &pos->second;
    else
        return NULL;
}

const GLCLUBMEMBER* GLClub::GetMember(DWORD dwCharID) const
{
    CLUBMEMBERS_CITER pos = m_mapMembers.find(dwCharID);
    if (pos != m_mapMembers.end())
        return &pos->second;
    else
        return NULL;
}

std::string GLClub::GetMemberName(DWORD ChaDbNum) const
{
    CLUBMEMBERS_CITER pos = m_mapMembers.find(ChaDbNum);
    if (pos != m_mapMembers.end())
    {
        const GLCLUBMEMBER& Member = pos->second;
        return std::string(Member.GetName());
    }
    else
    {
        return std::string("");
    }
}

void GLClub::SetMember(const GLCLUBMEMBER& Member)
{
    CLUBMEMBERS_ITER iter = m_mapMembers.find(Member.m_ChaDbNum);
    if (iter != m_mapMembers.end())
        m_mapMembers.erase(iter);
    m_mapMembers.insert(CLUBMEMBERS_VALUE(Member.m_ChaDbNum, Member));
}

void GLClub::SetMemberName(DWORD ChaDbNum, std::string& chaName)
{
	GLCLUBMEMBER* pMember = GetMember(ChaDbNum);
	if( pMember )
		pMember->m_ChaName = chaName;
}

void GLClub::SetMemberState(DWORD dwCharID, bool bONLINE, int nCHANNEL, const MapID& mapID)
{
    GLCLUBMEMBER* pMember = GetMember(dwCharID);
    if (pMember)
    {
        pMember->m_Online = bONLINE;
        pMember->m_Channel = nCHANNEL;
        pMember->m_CurrentMap = mapID;
    }
}

void GLClub::SetMemberState(DWORD ChaDbNum, const MapID& mapID)
{
    GLCLUBMEMBER* pMember = GetMember(ChaDbNum);
    if (pMember)
        pMember->m_CurrentMap = mapID;
}

void GLClub::SetMemberState(DWORD ChaDbNum, WORD ChaLevel)
{
    GLCLUBMEMBER* pMember = GetMember(ChaDbNum);
    if (pMember)
        pMember->m_ChaLevel = ChaLevel;

    if (m_MasterCharDbNum == ChaDbNum)
        m_MasterLevel = ChaLevel;
}

void GLClub::SetMemberPos(DWORD dwCharID, const D3DXVECTOR3& vecPOS)
{
    GLCLUBMEMBER* pMember = GetMember(dwCharID);
    if (pMember)
        pMember->m_CurrentPos = vecPOS;
}

bool GLClub::SetMemberOffline(DWORD ChaDbNum)
{
    GLCLUBMEMBER* pMember = GetMember(ChaDbNum);
    if (pMember)
    {
        pMember->m_Online = false;
        return true;
    }
    else
    {
        return false;
    }
}

/*
bool GLClub::SetMemberFlag ( DWORD dwCharID, DWORD dwFlags )
{
    //	Note : 실제로 지원되는 flag만 설정.
    dwFlags = (dwFlags & club::AUTHORITY_SUBMATER);

    GLCLUBMEMBER* pMember = GetMember(dwCharID);
    if (!pMember)
        return false;

    //	CDM 셋팅
    if (!SetMemberCDMFlag(dwCharID, (dwFlags & club::AUTHORITY_DEATH_MATCH)))
        return false;

    //	Note : 선도클럽 인증 권한 유무.
    if (dwFlags & club::AUTHORITY_CD)
    {
        // 이미 다른 부마스터가 인증권한을 갖고 있을경우
        if ( m_dwCDCertifior != 0 && m_dwCDCertifior != dwCharID )
        {
            // 인증권한 플래그를 제거한다.
            GLCLUBMEMBER* pOldMember = GetMember ( m_dwCDCertifior );
            if ( pOldMember )
            {
                pOldMember->m_Grade &= ~club::AUTHORITY_CD;
            }
        }

        //	선도클럽 인증 권한 부여.
        m_dwCDCertifior = dwCharID;
    }
    else if ( m_dwCDCertifior == dwCharID )
    {
        //	선도클럽 인증 권한 제거.
        m_dwCDCertifior = 0;
        pMember->m_Grade &= ~club::AUTHORITY_CD;
    }

    pMember->m_Grade = dwFlags;

    return true;
}
*/
DWORD GLClub::GetMemberFlag(DWORD dwCharID)
{
    GLCLUBMEMBER* pMember = GetMember(dwCharID);
    if (!pMember)
        return 0;
    else    
        return m_pGrade->GetFlag(pMember->m_Grade);
}

bool GLClub::SetMemberCDMFlag ( DWORD dwCharID, BOOL bSet )
{
    if ( bSet )
    {
        if ( !EnableCDMFlag( dwCharID )	) return false;

        if ( !IsMemberFlgCDM( dwCharID ) )
        {
            m_listCDMMembers.push_back( dwCharID );
        }
    }
    else
    {
        //	CDM 권한 있을경우 제거
        if ( IsMemberFlgCDM( dwCharID ) )
        {
            CLUB_CDM_MEM_ITER pos = std::find(m_listCDMMembers.begin(),m_listCDMMembers.end(),dwCharID );
            if ( pos != m_listCDMMembers.end() )
                m_listCDMMembers.erase( pos );
        }
    }

    return true;
}

bool GLClub::AddAlliance(DWORD dwAlliance, const char* szClubName, const char* szMasterName, DWORD MarkVersion)
{
    CLUB_ALLIANCE_CITER iter = m_setAlliance.find(dwAlliance);
    if (iter != m_setAlliance.end())
        return false;

    m_setAlliance.insert(CLUB_ALLIANCE_VALUE(dwAlliance, GLCLUBALLIANCE(dwAlliance, szClubName, szMasterName, MarkVersion)));
    return true;
}

bool GLClub::DelAlliance(DWORD dwAlliance)
{
    CLUB_ALLIANCE_ITER pos = m_setAlliance.find(dwAlliance);
    if (pos != m_setAlliance.end())
    {
        m_setAlliance.erase(pos);
        return true;
    }
    else
    {
        return false;
    }
}

bool GLClub::ClearAlliance ()
{
    m_dwAlliance = CLUB_NULL;
    m_setAlliance.clear();

    ReSetAllianceBattle();

    return true;
}

bool GLClub::UpdateAllianceClubName( DWORD dwAlliance, const char* szClubName )
{
	if ( !szClubName )
	{
		return false;
	}

	CLUB_ALLIANCE_ITER iter = m_setAlliance.find( dwAlliance );
	if ( iter == m_setAlliance.end() )
		return false;

	GLCLUBALLIANCE& Alliance = iter->second;
	StringCchCopy( Alliance.m_szName, CHAR_SZNAME, szClubName );
	
	return true;
}

void GLClub::SetAllianceDis ()
{
    //	해체시 제한시간 설정.
    CTime tCurTime = CTime::GetCurrentTime();
    CTimeSpan tSpan ( 0, GLCONST_CHAR::dwCLUB_ALLIANCE_DIS, 0, 0 );
    tCurTime += tSpan;

    m_tAllianceDis = tCurTime.GetTime();
}

void GLClub::SetAllianceSec ()
{
    //	제명시 제한시간 설정.
    CTime tCurTime = CTime::GetCurrentTime();
    CTimeSpan tSpan ( 0, GLCONST_CHAR::dwCLUB_ALLIANCE_SEC, 0, 0 );
    tCurTime += tSpan;

    m_tAllianceSec = tCurTime.GetTime();
}

void GLClub::SetAuthority ()
{
    //	제명시 제한시간 설정.
    CTime tCurTime = CTime::GetCurrentTime();
    CTimeSpan tSpan ( 0, GLCONST_CHAR::dwCLUB_AUTHORITY, 0, 0 );
    tCurTime += tSpan;

    m_tAuthority = tCurTime.GetTime();
}


bool GLClub::IsMaster(DWORD dwCharID) const
{
    return (m_MasterCharDbNum==dwCharID);
}

bool GLClub::IsSubMaster(DWORD dwCharID) const
{
    const GLCLUBMEMBER* pMember = GetMember(dwCharID);
    if (pMember)
        return pMember->GetChaDbNum() == m_dwCDCertifior ? true : false;
    else
        return false;
}

bool GLClub::IsAlliance() const
{
    return (m_dwAlliance!=CLUB_NULL);
}

//	같은 클럽이거나 동맹인지 체크하는 함수
//	클라이언트는 두개의 함수를 사용가능
//	서버의 경우는 동맹리스트를 마스터 클럽만 가지고 있으므로 
//	IsAllianceGuild_CHIEF() 해당 함수는 마스터 클럽만 사용가능하다.
//	Common함수는 동맹ID( AllianceID )를 가지고 체크하므로 아무데서나 사용가능하다.
bool GLClub::IsAllianceGuild_CHIEF ( DWORD dwGuildID )
{
    if (m_DbNum == 0 || dwGuildID == 0)
        return false;

    if (m_DbNum == dwGuildID)
        return true;

    CLUB_ALLIANCE_CITER pos = m_setAlliance.find(dwGuildID);
    return pos != m_setAlliance.end();
}

bool GLClub::IsAllianceGuild_COMMON ( DWORD dwClubID, DWORD dwAllianceID )
{
    if ( m_DbNum == 0 || dwClubID == 0 )
        return false;
    if ( m_DbNum == dwClubID )
        return true;

    if ( m_dwAlliance == 0 || dwAllianceID == 0 )
        return false;
    if ( m_dwAlliance == dwAllianceID )
        return true;

    return false;
}

bool GLClub::IsChief() const
{
    return (m_dwAlliance==m_DbNum);
}

bool GLClub::IsAllianceDis ()
{
    CTime tDis(m_tAllianceDis);
    CTime tCur = CTime::GetCurrentTime();

    // CTime 의 멤버를 호출할때는 한번에 값을 받아와야 한다.
    // 내부적으로 static 으로 정의되어 있기 때문에
    // 다음번 동작이 유효하다고 보장되지 않는다.	
    // This function calls GetLocalTm, which uses an internal statically allocated buffer.
    // The data in this buffer is overwritten because of calls to other CTime member functions.
    int nYear = tDis.GetYear();
    if ( nYear <= 1970 || nYear > 2038 ) 
        return false;

    // 시간이 지나면 초기화
    if ( tDis <= tCur )
    {
        CTime tTime(1970,2,1,1,1,1);
        m_tAllianceDis = tTime.GetTime();
        return false;
    }

    return true;
}

bool GLClub::IsAllianceSec ()
{
    CTime tSec(m_tAllianceSec);
    CTime tCur = CTime::GetCurrentTime();

    int nYear = tSec.GetYear();
    if ( nYear <= 1970 || nYear > 2038 ) 
        return false;

    // 시간이 지나면 초기화
    if ( tSec <= tCur )
    {
        CTime tTime(1970,2,1,1,1,1);
        m_tAllianceSec = tTime.GetTime();
        return false;
    }

    return true;
}

bool GLClub::IsAuthority ()
{
    CTime tSec(m_tAuthority);
    CTime tCur = CTime::GetCurrentTime();

    int nYear = tSec.GetYear();
    if ( nYear <= 1970 || nYear > 2038 ) 
        return false;

    // 시간이 지나면 초기화
    if ( tSec <= tCur )
    {
        CTime tTime(1970,2,1,1,1,1);
        m_tAuthority = tTime.GetTime();
        return false;
    }

    return true;
}
/*
bool GLClub::IsMemberFlgJoin(DWORD dwCharID)
{    
    //if (IsMaster(dwCharID))
    //    return true;
    //GLCLUBMEMBER* pMember = GetMember(dwCharID);
    //if (pMember)
    //    return pMember->m_Grade & club::AUTHORITY_MEMBER_JOIN ? true : false;
    //else
    //    return false;
    return HaveMemberJoinAuthority(dwCharID);
}
*/
/*
bool GLClub::IsMemberFlgKick(DWORD dwCharID)
{    
    //if (IsMaster(dwCharID))
    //    return true;
    //GLCLUBMEMBER* pMember = GetMember(dwCharID);
    //if (pMember)        
    //    return pMember->m_Grade & club::AUTHORITY_MEMBER_KICK ? true : false;
    //else
    //    return false;
    return HaveMemberKickAuthority(dwCharID);
}
*/
bool GLClub::IsMemberFlgNotice(DWORD dwCharID)
{
    /*
    if (IsMaster(dwCharID))
        return true;

    GLCLUBMEMBER* pMember = GetMember(dwCharID);
    if (pMember)
        return pMember->m_Grade & club::AUTHORITY_NOTICE_CHANGE ? true : false;
    else
        return false;
    */
    return HaveNoticeChangeAuthority(dwCharID);
}

bool GLClub::IsMemberFlgCDCertify(DWORD dwCharID)
{
    // 클럽의 맴버가 아니면
    GLCLUBMEMBER* pMember = GetMember(dwCharID);
    if (!pMember)
        return false;

    //	동맹에 가입되어 있을 경우
    if (IsAlliance())
    {
        // 동맹의 마스터 클럽이면
        if (IsChief())
        {
            if (m_dwCDCertifior==0 && IsMaster(dwCharID))
                return true;
            else
                return m_dwCDCertifior==dwCharID;
        }
    }
    else
    {
        //	인증 권한 대행자가 없을경우는 마스터에게 권한이 있음.
        if (m_dwCDCertifior==0 && IsMaster(dwCharID))
            return true;
        else            
            return m_dwCDCertifior==dwCharID; // 인증 권한 대행자가 있을 경우 점검
    }
    return false;
}

bool GLClub::IsMemberFlgMarkChange(DWORD dwCharID)
{
    /*
    if (IsMaster(dwCharID))
        return true;

    GLCLUBMEMBER* pMember = GetMember(dwCharID);
    if (pMember)
        return pMember->m_Grade & club::AUTHORITY_MARK_CHANGE ? true : false;
    else
        return false;
    */
    return HaveMarkChangeAuthority(dwCharID);
}

bool GLClub::IsMemberFlgCDM(DWORD dwCharID)
{
    /*
    GLCLUBMEMBER* pMember = GetMember(dwCharID);
    if (pMember)
        return pMember->m_Grade & club::AUTHORITY_DEATH_MATCH ? true : false;
    else
        return false;
    */
    return HaveDeathMatchAuthority(dwCharID);
}

bool GLClub::EnableCDMFlag(DWORD dwCharID)
{
    if (IsMemberFlgCDM(dwCharID))
        return true;

    if (m_listCDMMembers.size() < GLCONST_CHAR::dwCLUB_DEATHMATCH_MEM)
        return true;
    else
        return false;
}

bool GLClub::IsCDMEnter(DWORD dwCharID)
{
    if (m_dwRank < GLCONST_CHAR::dwCLUB_DEATHMATCH_RANK)
        return false;

    if (IsMaster(dwCharID))
        return true;
    else
        return IsMemberFlgCDM(dwCharID);
}

void GLClub::CLEARCONFT ()
{
    m_dwconftCLUB = CLUB_NULL;
    m_fconftTIMER = 0;
    m_conftOPT.RESET();

    m_setConftSTART.clear ();
    m_setConftCURRENT.clear ();
}

void GLClub::ADDCONFT_MEMBER ( DWORD dwID )
{
    m_setConftCURRENT.insert ( dwID );
}

void GLClub::DELCONFT_MEMBER(DWORD dwID)
{
    CLUBCONFT_ITER pos = m_setConftCURRENT.find(dwID);
    if (pos != m_setConftCURRENT.end())
        m_setConftCURRENT.erase(pos);
}

DWORD GLClub::GETCONFT_NUM()
{
    return (DWORD) m_setConftCURRENT.size();
}

void GLClub::SETCONFT_MEMBER()
{
    m_setConftSTART = m_setConftCURRENT;
}

bool GLClub::ISCONFT_MEMBER(DWORD dwID)
{
    CLUBCONFT_ITER pos = m_setConftCURRENT.find(dwID);
    return (pos != m_setConftCURRENT.end());
}

bool GLClub::ISCONFTING()
{
    return m_dwconftCLUB!=CLUB_NULL;
}

bool GLClub::ISCONFT_LOSS()
{
    return m_setConftCURRENT.empty()==true;
}

void GLClub::ADDBATTLECLUB(const GLCLUBBATTLE& sBattleClub)
{
    m_mapBattle[sBattleClub.m_dwCLUBID] = sBattleClub;
}

void GLClub::DELBATTLECLUB(DWORD dwClubID)
{
    CLUB_BATTLE_ITER pos = m_mapBattle.find(dwClubID);
    if (pos != m_mapBattle.end())
        m_mapBattle.erase(pos);
}

void GLClub::ADDBATTLE_READY_CLUB( const GLCLUBBATTLE& sBattleClub )
{
    m_mapBattleReady[sBattleClub.m_dwCLUBID] = sBattleClub;
}

void GLClub::DELBATTLE_READY_CLUB( DWORD dwClubID )
{
    CLUB_BATTLE_ITER pos = m_mapBattleReady.find(dwClubID);
    if (pos != m_mapBattleReady.end())
        m_mapBattleReady.erase(pos);
}

GLCLUBBATTLE* GLClub::GetClubBattle( DWORD dwClubID )
{
    CLUB_BATTLE_ITER pos = m_mapBattle.find(dwClubID);
    if (pos != m_mapBattle.end())
        return &(*pos).second;
    else
        return NULL;    
}

GLCLUBBATTLE* GLClub::GetClubBattleReady( DWORD dwClubID )
{
    CLUB_BATTLE_ITER pos = m_mapBattleReady.find(dwClubID);
    if (pos != m_mapBattleReady.end())
        return &(*pos).second;
    else
        return NULL;
}

GLCLUBBATTLE_DEL* GLClub::GetClubBattleDel(DWORD dwClubID)
{
    CLUB_BATTLE_DEL_ITER pos = m_mapBattleDel.find(dwClubID);
    if (pos != m_mapBattleDel.end())
        return &(*pos).second;
    else
        return NULL;    
}

bool GLClub::IsBattleStop( DWORD dwClubID )
{
    GLCLUBBATTLE* pClubBattle = GetClubBattle( dwClubID );
    if (!pClubBattle)
        return false;

    CTime tStopTime = CTime::GetCurrentTime();
    CTimeSpan tDisTimeSpan(0,0,GLCONST_CHAR::dwCLUB_BATTLE_DIS_TIME,0);
    CTime tDisTime ( pClubBattle->m_tStartTime);
    tDisTime += tDisTimeSpan;

    if (tDisTime < tStopTime)
        return true;
    else
        return false;
}

bool GLClub::IsGuidBattle()
{
    CTime tCurTime = CTime::GetCurrentTime();
    CTimeSpan tDisTimeSpan(0,0,GLCONST_CHAR::dwCLUB_BATTLE_GUID_TIME,0);
    CTime tDisTime ( m_tLastBattle );
    tDisTime += tDisTimeSpan;

    if (tDisTime < tCurTime)
        return true;
    else
        return false;
}

bool GLClub::IsBattle( DWORD dwClubID )
{
    if (dwClubID == 0)
        return false;

    GLCLUBBATTLE* pClubBattle = GetClubBattle( dwClubID );
    if ( !pClubBattle || pClubBattle->m_bAlliance )
        return false;
    else
        return true;
}

bool GLClub::IsBattleAlliance( DWORD dwClubID )
{
    if (dwClubID == 0)
        return false;

    GLCLUBBATTLE* pClubBattle = GetClubBattle(dwClubID);
    if (!pClubBattle || !pClubBattle->m_bAlliance)
        return false;
    else
        return true;
}

bool GLClub::IsBattleReady( DWORD dwClubID )
{
    if (dwClubID == 0)
        return false;

    CLUB_BATTLE_ITER pos = m_mapBattleReady.find(dwClubID);
    return pos!=m_mapBattleReady.end();
}

void GLClub::KillBattleClubMem( DWORD dwClubID, WORD nKill )
{
    GLCLUBBATTLE* pClubBattle = GetClubBattle( dwClubID );
    if (!pClubBattle)
        return;
    pClubBattle->m_wKillPoint += nKill;
    pClubBattle->m_bKillUpdate = true;
}
void GLClub::DeathBattleClubMem( DWORD dwClubID, WORD nDeath )
{
    GLCLUBBATTLE* pClubBattle = GetClubBattle( dwClubID );
    if (!pClubBattle)
        return;
    pClubBattle->m_wDeathPoint += nDeath;
    pClubBattle->m_bKillUpdate = true;
}

//	동맹배틀 전적 초기화
void GLClub::ReSetAllianceBattle()
{
    m_dwAllianceBattleWin = 0;
    m_dwAllianceBattleLose = 0;
    m_dwAllianceBattleDraw = 0;
}

bool GLClub::CheckAuth(DWORD ChaDbNum, club::AUTHORITY Auth) const
{
    CLUBMEMBERS_CITER iter = m_mapMembers.find(ChaDbNum);
    if (iter != m_mapMembers.end())
    {
        const GLCLUBMEMBER& Member = iter->second;        
        return m_pGrade->CheckAuth(Member.m_Grade, Auth);
    }
    else
    {
        return false;
    }
}

bool GLClub::AddAuth(size_t Index, const club::AUTH_GRADE& Grade)
{
    if (!m_pGrade->Add(Index, Grade))
        return false;
    else
        return true;
}

club::AUTH_GRADE GLClub::GetAuth(size_t Index) const
{
    return m_pGrade->GetData(Index);
}

bool GLClub::ChangeAuth(size_t Index, club::AUTH_GRADE& Grade)
{
    if (!m_pGrade->Change(Index, Grade))
        return false;
    else
        return true;
}

bool GLClub::ChangeAuth(size_t IndexA, size_t IndexB)
{
    if (!m_pGrade->Change(IndexA, IndexB))
        return false;
    else
        return true;
}


bool GLClub::HaveMemberJoinAuthority(DWORD ChaDbNum) const
{
    if (m_MasterCharDbNum == ChaDbNum)
        return true;
    else
        return CheckAuth(ChaDbNum, club::AUTHORITY_MEMBER_JOIN);
}

bool GLClub::HaveMemberKickAuthority(DWORD ChaDbNum) const
{
    if (m_MasterCharDbNum == ChaDbNum)
        return true;
    else
        return CheckAuth(ChaDbNum, club::AUTHORITY_MEMBER_KICK);
}

bool GLClub::HaveNoticeChangeAuthority(DWORD ChaDbNum) const
{
    if (m_MasterCharDbNum == ChaDbNum)
        return true;
    else
        return CheckAuth(ChaDbNum, club::AUTHORITY_NOTICE_CHANGE);
}

bool GLClub::HaveMarkChangeAuthority(DWORD ChaDbNum) const
{
    if (m_MasterCharDbNum == ChaDbNum)
        return true;
    else
        return CheckAuth(ChaDbNum, club::AUTHORITY_MARK_CHANGE);
}

bool GLClub::HaveDeathMatchAuthority(DWORD ChaDbNum) const
{
    if (m_MasterCharDbNum == ChaDbNum)
        return true;
    else
        return CheckAuth(ChaDbNum, club::AUTHORITY_DEATH_MATCH);
}

bool GLClub::HaveAuthChangeAuthority(DWORD ChaDbNum) const
{
    if (m_MasterCharDbNum == ChaDbNum)
        return true;
    else
        return CheckAuth(ChaDbNum, club::AUTHORITY_CHANGE);
}

bool GLClub::HaveRankUpAuthority(DWORD ChaDbNum) const
{
    if (m_MasterCharDbNum == ChaDbNum)
        return true;
    else
        return CheckAuth(ChaDbNum, club::AUTHORITY_RANK_UP);
}

bool GLClub::HaveAllianceAuthority(DWORD ChaDbNum) const
{
    if (m_MasterCharDbNum == ChaDbNum)
        return true;
    else
        return CheckAuth(ChaDbNum, club::AUTHORITY_ALLIANCE);
}

bool GLClub::HaveAllianceDisAuthority(DWORD ChaDbNum) const
{
    if (m_MasterCharDbNum == ChaDbNum)
        return true;
    else
        return CheckAuth(ChaDbNum, club::AUTHORITY_ALLIANCE_DIS);
}

bool GLClub::HaveClubBattleAuthority(DWORD ChaDbNum) const
{
    if (m_MasterCharDbNum == ChaDbNum)
        return true;
    else
        return CheckAuth(ChaDbNum, club::AUTHORITY_CLUB_BATTLE);
}

bool GLClub::HaveMoneyAuthority(DWORD ChaDbNum) const
{
    if (m_MasterCharDbNum == ChaDbNum)
        return true;
    else
        return CheckAuth(ChaDbNum, club::AUTHORITY_MONEY);
}

bool GLClub::HaveStorageAuthority(DWORD ChaDbNum) const
{
    if (m_MasterCharDbNum == ChaDbNum)
        return true;
    else
        return CheckAuth(ChaDbNum, club::AUTHORITY_ITEM);
}

WORD GLClub::GetGradeSize() const
{
    return m_pGrade->Size();
}

std::string GLClub::GetGradeName(size_t Index) const
{
    return m_pGrade->GetName(Index);
}

bool GLClub::HaveSameGradeName(const std::string& GradeName) const
{
    return m_pGrade->HaveSameName(GradeName);
}

//! Club member 계급 변경
bool GLClub::ChangeMemberGrade(DWORD MemberDbNum, size_t Grade)
{
    GLCLUBMEMBER* pMember = GetMember(MemberDbNum);
    if (!pMember)
        return false;

    if (Grade >= m_pGrade->Size())
        return false;

    if (m_MasterCharDbNum == MemberDbNum)
        return false;

    pMember->SetGrade(static_cast<DWORD> (Grade));
    return true;
}

void GLClub::GetAllianceData(std::vector<GLCLUBALLIANCE>& vData) const
{    
    for (CLUB_ALLIANCE_CITER iter=m_setAlliance.begin(); iter!=m_setAlliance.end(); ++iter)
        vData.push_back(iter->second);
}

//! 클럽 배틀/club battle
void GLClub::GetClubBattleData(std::vector<GLCLUBBATTLE>& vData) const
{
    for (CLUB_BATTLE_CITER iter=m_mapBattle.begin(); iter!=m_mapBattle.end(); ++iter)
        vData.push_back(iter->second);
}

bool GLClub::ChangeGradeName(size_t GradeIndex, const std::string& GradeName, DWORD ReqChaDbNum)
{
    if (ReqChaDbNum != 0)
    {
        if (!HaveAuthChangeAuthority(ReqChaDbNum))
            return false;
    }
    return m_pGrade->ChangeName(GradeIndex, GradeName);
}

DWORD GLClub::GetGradeFlag(size_t Index) const
{
    return m_pGrade->GetFlag(Index);
}

bool GLClub::SetGradeFlag(const std::vector<club::GRADE_FLAG>& Data)
{
    for (size_t i=0; i<Data.size(); ++i)
        m_pGrade->SetFlag(Data[i].m_Index, Data[i].m_Flag);
    return true;
}

bool GLClub::SetGradeFlag(size_t Index, DWORD Flag)
{
    return m_pGrade->SetFlag(Index, Flag);
}

//! Login time
void GLClub::SetMemberLastPlayTime(DWORD ChaDbNum, const __time64_t& LoginTime)
{
    GLCLUBMEMBER* pMember = GetMember(ChaDbNum);
    if (pMember)
        pMember->SetLastPlayTime(LoginTime);
}

void GLClub::GetMemberDbNumList(std::set<DWORD>& Member) const
{
    for (CLUBMEMBERS_CITER iter=m_mapMembers.begin(); iter!=m_mapMembers.end(); ++iter)
    {
        Member.insert(iter->first);
    }
}

void GLClub::SetStorageMoney( LONGLONG llMoney )
{
	if ( llMoney >= 0 )
	{
		m_lnStorageMoney = llMoney;
	}

	if ( m_lnStorageMoney < 0 )
	{
		m_lnStorageMoney = 0;
	}
}

void GLClub::SetIncomeMoney( LONGLONG llMoney )
{
	if ( llMoney >= 0 )
	{
		m_lnIncomeMoney = llMoney;
	}

	if ( m_lnIncomeMoney < 0 )
	{
		m_lnIncomeMoney = 0;
	}
}

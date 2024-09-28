#include "pch.h"

#include "../../SigmaCore/Util/Lock.h"
#include "../../SigmaCore/gassert.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../RanLogic/Character/GLCharDefine.h"
#include "../Character/CharNameCache.h"
#include "CharManField.h"

#include "../../SigmaCore/DebugInclude.h"

float CharManField::MAX_DELAY_TIME = 300000;

// framemove 5초에 한번식 돌도록
float CharManField::FRAME_MOVE_DELAY = 5.0f;

CharManField::CharManField(net::EMSERVERTYPE servertype)
: m_fFrameMoveTimer( 0.0f )
{
    InitializeCriticalSectionAndSpinCount(&m_SaveDbUserIdLock, sc::CRITICAL_SECTION_SPIN_COUNT);
    InitializeCriticalSectionAndSpinCount(&m_CreatingUserIdLock, sc::CRITICAL_SECTION_SPIN_COUNT);

	m_ServerType = servertype;

    m_pNameCache = new CharNameCache;
}

CharManField::~CharManField()
{
    delete m_pNameCache;
    m_pNameCache = NULL;

    m_reqSaveDBUserID.clear();

    DeleteCriticalSection(&m_CreatingUserIdLock);
    DeleteCriticalSection(&m_SaveDbUserIdLock);
}

void CharManField::ReqSaveDbUserAdd(DWORD UserDbNum)
{
	if (m_ServerType == net::SERVER_INSTANCE)
		return;

    sc::CriticalSectionOwnerLock OwnerLock(m_SaveDbUserIdLock);
    
    PCID_CITER iter = m_reqSaveDBUserID.find(UserDbNum);

	// make_pair 보다 value_type 을 사용
    if (iter == m_reqSaveDBUserID.end())
        m_reqSaveDBUserID.insert( PCID_VALUE( UserDbNum, GetTickCount() ) );
}

void CharManField::ReqSaveDbUserDel(DWORD UserDbNum)
{
	if (m_ServerType == net::SERVER_INSTANCE)
		return;

	// 직접 호출에서 메세지 방식으로 전환 되어 락 걸 필요 없음;
	// 다른 함수 들은 아직 검증되지 않았으므로 일단 이쪽만 삭제;
//  sc::CriticalSectionOwnerLock OwnerLock(m_SaveDbUserIdLock);

    PCID_ITER iter = m_reqSaveDBUserID.find(UserDbNum);
    if (iter != m_reqSaveDBUserID.end())
        m_reqSaveDBUserID.erase(iter);
}

BOOL CharManField::ReqSaveDbUserFind(DWORD UserDbNum)
{
	if (m_ServerType == net::SERVER_INSTANCE)
		return FALSE;

    sc::CriticalSectionOwnerLock OwnerLock(m_SaveDbUserIdLock);

    PCID_CITER iter = m_reqSaveDBUserID.find(UserDbNum);
    if (iter != m_reqSaveDBUserID.end())
        return TRUE;
    else
        return FALSE;
}

void CharManField::CreatingCharAdd(DWORD UserDbNum)
{
	if (m_ServerType == net::SERVER_INSTANCE)
		return;

    sc::CriticalSectionOwnerLock OwnerLock(m_CreatingUserIdLock);

    PCID_CITER iter = m_setCreatingChar.find(UserDbNum);

	// make_pair 보다 value_type 을 사용
    if (iter == m_setCreatingChar.end())
        m_setCreatingChar.insert( PCID_VALUE( UserDbNum, GetTickCount() ) );
}

void CharManField::CreatingCharDel(DWORD UserDbNum)
{
	if (m_ServerType == net::SERVER_INSTANCE)
		return;

    sc::CriticalSectionOwnerLock OwnerLock(m_CreatingUserIdLock);

    PCID_ITER iter = m_setCreatingChar.find(UserDbNum);
    if (iter != m_setCreatingChar.end())
        m_setCreatingChar.erase(iter);
}

BOOL CharManField::CreatingCharFind(DWORD UserDbNum)
{
	if (m_ServerType == net::SERVER_INSTANCE)
		return FALSE;

    sc::CriticalSectionOwnerLock OwnerLock(m_CreatingUserIdLock);

    PCID_CITER iter = m_setCreatingChar.find(UserDbNum);
    if (iter != m_setCreatingChar.end())
        return TRUE;
    else
        return FALSE;
}

//! 캐릭터이름/Pointer
GLChar* CharManField::GetChaByName(const std::string& ChaName)
{
    GLCHAR_MAP_ITER iter = m_PCNameMap.find(ChaName);
    if (iter != m_PCNameMap.end())
        return iter->second;
    else
        return NULL;
}

bool CharManField::ChaNameExist(const std::string& ChaName)
{
    GLCHAR_MAP_ITER iter = m_PCNameMap.find(ChaName);
    if (iter != m_PCNameMap.end())
        return true;
    else
        return false;
}

void CharManField::ChaNameDel(const std::string& ChaName)
{
    GLCHAR_MAP_ITER iter = m_PCNameMap.find(ChaName);
    if (iter != m_PCNameMap.end())
        m_PCNameMap.erase(iter);
}

void CharManField::ChaNameAdd(const std::string& ChaName, GLChar* pChar)
{
    GLCHAR_MAP_ITER iter = m_PCNameMap.find(ChaName);
    if (iter != m_PCNameMap.end())
        m_PCNameMap.erase(iter);
    if (pChar)
        m_PCNameMap.insert(GLCHAR_MAP_VALUE(ChaName, pChar));
}

//! Character DB 번호/Gaea ID
DWORD CharManField::GetGaeaId(DWORD ChaDbNum)
{
	GASSERT(m_ServerType != net::SERVER_INSTANCE);
	if (m_ServerType == net::SERVER_INSTANCE)
		return MAXDWORD;

    CLIENTMAP_ITER iter = m_mapCHARID.find(ChaDbNum);
    if (iter != m_mapCHARID.end())
        return iter->second;
    else
        return GAEAID_NULL;
}

void CharManField::ChaDbNumAdd(DWORD ChaDbNum, DWORD GaeaId)
{
	GASSERT(m_ServerType != net::SERVER_INSTANCE);
	if(m_ServerType == net::SERVER_INSTANCE)
		return;
	
    CLIENTMAP_ITER iter = m_mapCHARID.find(ChaDbNum);
    if (iter != m_mapCHARID.end())
        m_mapCHARID.erase(iter);
    m_mapCHARID.insert(CLIENTMAP_VALUE(ChaDbNum, GaeaId));
}

void CharManField::ChaDbNumDel(DWORD ChaDbNum)
{
	GASSERT(m_ServerType != net::SERVER_INSTANCE);
	if(m_ServerType == net::SERVER_INSTANCE)
		return;

    CLIENTMAP_ITER iter = m_mapCHARID.find(ChaDbNum);
    if (iter != m_mapCHARID.end())
        m_mapCHARID.erase(iter);
}


//! Character DB 번호/Gaea ID
DWORD CharManField::GetGaeaId(DWORD64& UniqueID)
{
	GASSERT(m_ServerType == net::SERVER_INSTANCE);
	if(m_ServerType != net::SERVER_INSTANCE)
		return GAEAID_NULL;

    CLIENTMAP64_ITER iter = m_mapCHARID64.find(UniqueID);
    if (iter != m_mapCHARID64.end())
        return iter->second;
    else
        return GAEAID_NULL;
}

DWORD CharManField::GetGaeaId(DWORD dwServerGroupNum, DWORD dwChaDbNum)
{
	GASSERT(m_ServerType == net::SERVER_INSTANCE);
	if(m_ServerType != net::SERVER_INSTANCE)
		return GAEAID_NULL;

    CLIENTMAP64_ITER iter = m_mapCHARID64.find(UNIQUEID(dwServerGroupNum, dwChaDbNum).Id());
    if (iter != m_mapCHARID64.end())
        return iter->second;
    else
        return GAEAID_NULL;
}

void CharManField::ChaUniqueIDAdd(const DWORD64& UniqueID, DWORD GaeaId)
{
	GASSERT(m_ServerType == net::SERVER_INSTANCE);
	if(m_ServerType != net::SERVER_INSTANCE)
		return;
	
    CLIENTMAP64_ITER iter = m_mapCHARID64.find(UniqueID);
    if (iter != m_mapCHARID64.end())
        m_mapCHARID64.erase(iter);
    m_mapCHARID64.insert(CLIENTMAP64_VALUE(UniqueID, GaeaId));
}

void CharManField::ChaUniqueIDDel(const DWORD64& UniqueID)
{
	GASSERT(m_ServerType == net::SERVER_INSTANCE);
	if(m_ServerType != net::SERVER_INSTANCE)
		return;

    CLIENTMAP64_ITER iter = m_mapCHARID64.find(UniqueID);
    if (iter != m_mapCHARID64.end())
        m_mapCHARID64.erase(iter);
}

void CharManField::ChaNameDbNumAdd(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType)
{
    m_pNameCache->Add(ChaName, ChaDbNum, UserDbNum, UserType);
}

void CharManField::ChaNameDbNumAdd(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType, int iClass, int iLv)
{
	CHAR_BASE_CACHE_DATA sData = CHAR_BASE_CACHE_DATA( ChaDbNum, UserDbNum, static_cast<BYTE>(UserType), ChaName, iClass, iLv);

	m_pNameCache->Add(sData);
}

DWORD CharManField::GetChaDbNumByChaName(const std::string& ChaName)
{
    return m_pNameCache->FindChaDbNum(ChaName);
}

std::string CharManField::GetChaNameByDbNum(DWORD ChaDbNum)
{
    return m_pNameCache->FindChaName(ChaDbNum);
}

std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> CharManField::GetCharCacheData(const std::string& ChaName)
{
    return m_pNameCache->Find(ChaName);
}

std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> CharManField::GetCharCacheData(DWORD ChaDbNum)
{
    return m_pNameCache->Find(ChaDbNum);
}

void CharManField::FrameMove( float fElapsedTime )
{
	if ( net::SERVER_INSTANCE == m_ServerType )
		return;

	m_fFrameMoveTimer += fElapsedTime;
	if ( m_fFrameMoveTimer > FRAME_MOVE_DELAY )
	{
		m_fFrameMoveTimer = 0.0f;
		FrameMoveSaveCharacter();
		FrameMoveCreateCharacter();	
	}
}

void CharManField::FrameMoveSaveCharacter()
{
	sc::CriticalSectionOwnerLock OwnerLock( m_SaveDbUserIdLock );
	DWORD CurTime = GetTickCount();

	// m_reqSaveDBUserID 정리
	PCID_ITER iter = m_reqSaveDBUserID.begin();
	PCID_ITER iterEnd = m_reqSaveDBUserID.end();

	for ( ; iter != iterEnd; )
	{
		DWORD FlowTime = CurTime - iter->second;

		if ( FlowTime > MAX_DELAY_TIME )
		{
			sc::writeLogInfo(
				stringformat(
				"CharManField::FrameMove : Delete save character ID because of time out. UserDbNum %1%", iter->first ) );

			iter = m_reqSaveDBUserID.erase( iter );
		}
		else
			iter++;
	}
}

void CharManField::FrameMoveCreateCharacter()
{
	sc::CriticalSectionOwnerLock OwnerLock( m_CreatingUserIdLock );
	DWORD CurTime = GetTickCount();

	// m_setCreatingChar 정리
	PCID_ITER iter = m_setCreatingChar.begin();
	PCID_ITER iterEnd = m_setCreatingChar.end();

	for ( ; iter != iterEnd; )
	{
		DWORD FlowTime = CurTime - iter->second;

		if ( FlowTime > MAX_DELAY_TIME )
		{
			sc::writeLogInfo(
				stringformat(
				"CharManField::FrameMove : Delete careate character ID because of time out. UserDbNum %1%", iter->first ) );

			iter = m_setCreatingChar.erase( iter );
		}
		else
			iter++;
	}
}

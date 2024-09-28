#include "pch.h"
#include "../Character/CharNameCache.h"
#include "../Character/GLCharAG.h"
#include "./GLAgentServer.h"
#include "./AgentClientMan.h"

#include "../../SigmaCore/DebugInclude.h"

AgentCharMan::AgentCharMan(GLAgentServer* pServer)
    : m_pServer(pServer)
{
    m_pNameCache = new CharNameCache; 
}

AgentCharMan::~AgentCharMan()
{
    delete m_pNameCache;
    m_pNameCache = NULL;    
}

GLCharAG* AgentCharMan::GetCharByUserAccount(const std::string& UserId)
{
    CHAR_MAP_ITER iter = m_UAccountMap.find(UserId);
    if (iter != m_UAccountMap.end())
        return iter->second;
    else
        return NULL;
}

void AgentCharMan::UserAccountAdd(const std::string& UserId, GLCharAG* pChar)
{
    if (!pChar || UserId.empty())
        return;

    CHAR_MAP_ITER iter = m_UAccountMap.find(UserId);
    if (iter != m_UAccountMap.end())
        m_UAccountMap.erase(iter);
    m_UAccountMap.insert(CHAR_MAP_VALUE(UserId, pChar));
}

void AgentCharMan::UserAccountDel(const std::string& UserId)
{
    CHAR_MAP_ITER iter = m_UAccountMap.find(UserId);
    if (iter != m_UAccountMap.end())
        m_UAccountMap.erase(iter);
}

DWORD AgentCharMan::GetGaeaIdByClientSlot(DWORD ClientSlot)
{
    CLIENTMAP_ITER iter = m_ClientSlotGaeaIdMap.find(ClientSlot);
    if (iter != m_ClientSlotGaeaIdMap.end())
        return iter->second;
    else
        return GAEAID_NULL;
}

void AgentCharMan::NetworkSlotAdd(DWORD ClientSlot, DWORD GaeaId)
{
    CLIENTMAP_ITER iter = m_ClientSlotGaeaIdMap.find(ClientSlot);
    if (iter != m_ClientSlotGaeaIdMap.end())
        m_ClientSlotGaeaIdMap.erase(iter);
    m_ClientSlotGaeaIdMap.insert(CLIENTMAP_VALUE(ClientSlot, GaeaId));
}

void AgentCharMan::NetworkSlotDel(DWORD ClientSlot)
{
    CLIENTMAP_ITER iter = m_ClientSlotGaeaIdMap.find(ClientSlot);
    if (iter != m_ClientSlotGaeaIdMap.end())
        m_ClientSlotGaeaIdMap.erase(iter);
}

void AgentCharMan::ChaNameDbNumAdd(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType)
{
    m_pNameCache->Add(ChaName, ChaDbNum, UserDbNum, UserType);
}

void AgentCharMan::ChaNameDbNumAdd(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType, int iClass, int iLv)
{
	CHAR_BASE_CACHE_DATA data = CHAR_BASE_CACHE_DATA( ChaDbNum, UserDbNum, static_cast<BYTE>(UserType), ChaName, iClass, iLv );

	m_pNameCache->Add(data);
}

DWORD AgentCharMan::GetChaDbNumByChaName(const std::string& ChaName)
{
    return m_pNameCache->FindChaDbNum(ChaName);
}

std::string AgentCharMan::GetChaNameByDbNum(DWORD ChaDbNum)
{
    return m_pNameCache->FindChaName(ChaDbNum);
}

std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> AgentCharMan::GetCharCacheData(const std::string& ChaName)
{
    return m_pNameCache->Find(ChaName);
}

std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> AgentCharMan::GetCharCacheData(DWORD ChaDbNum)
{
    return m_pNameCache->Find(ChaDbNum);
}

void AgentCharMan::ChaDbNumGaeaIdAdd(DWORD ChaDbNum, DWORD GaeaId)
{
    CLIENTMAP_ITER iter = m_CharDbNumGaeaIdMap.find(ChaDbNum);
    if (iter != m_CharDbNumGaeaIdMap.end())
        m_CharDbNumGaeaIdMap.erase(iter);
    m_CharDbNumGaeaIdMap.insert(CLIENTMAP_VALUE(ChaDbNum, GaeaId));
}

void AgentCharMan::ChaDbNumGaeaIdDel(DWORD ChaDbNum)
{
    CLIENTMAP_ITER iter = m_CharDbNumGaeaIdMap.find(ChaDbNum);
    if (iter != m_CharDbNumGaeaIdMap.end())
        m_CharDbNumGaeaIdMap.erase(iter);
}

DWORD AgentCharMan::GetGaeaIdByChaDbNum(DWORD ChaDbNum)
{
    CLIENTMAP_ITER iter = m_CharDbNumGaeaIdMap.find(ChaDbNum);
    if (iter != m_CharDbNumGaeaIdMap.end())
        return iter->second;
    else
        return GAEAID_NULL;
}

void AgentCharMan::UserDbNumGaeaIdAdd(DWORD UserDbNum, DWORD GaeaId)
{
    CLIENTMAP_ITER iter = m_UserNumberMap.find(UserDbNum);
    if (iter != m_UserNumberMap.end())
        m_UserNumberMap.erase(iter);
    m_UserNumberMap.insert(CLIENTMAP_VALUE(UserDbNum, GaeaId));
}

void AgentCharMan::UserDbNumGaeaIdDel(DWORD UserDbNum)
{
    CLIENTMAP_ITER iter = m_UserNumberMap.find(UserDbNum);
    if (iter != m_UserNumberMap.end())
        m_UserNumberMap.erase(iter);
}

DWORD AgentCharMan::GetGaeaIdByUserDbNum(DWORD UserDbNum)
{
    CLIENTMAP_ITER iter = m_UserNumberMap.find(UserDbNum);
    if (iter != m_UserNumberMap.end())
        return iter->second;
    else
        return GAEAID_NULL;
}

//! 캐릭터이름/캐릭터 Pointer
void AgentCharMan::ChaNameAdd(const std::string& ChaName, GLCharAG* pChar)
{
    if (ChaName.empty() || !pChar)
        return;

    CHAR_MAP_ITER iter = m_CharNameMap.find(ChaName);
    if (iter != m_CharNameMap.end())
        m_CharNameMap.erase(iter);
    m_CharNameMap.insert(CHAR_MAP_VALUE(ChaName, pChar));

    //ChaNameDbNumAdd(ChaName, pChar->ChaDbNum(), pChar->UserDbNum(), pChar->UserType());
	ChaNameDbNumAdd(ChaName, pChar->ChaDbNum(), pChar->UserDbNum(), pChar->UserType(), static_cast<int>(pChar->GetClass()), pChar->GetChaLevel());
}

void AgentCharMan::ChaNameDel(const std::string& ChaName)
{
    CHAR_MAP_ITER iter = m_CharNameMap.find(ChaName);
    if (iter != m_CharNameMap.end())
        m_CharNameMap.erase(iter);
}

GLCharAG* AgentCharMan::GetCharByChaName(const std::string& ChaName)
{
    CHAR_MAP_ITER iter = m_CharNameMap.find(ChaName);
    if (iter != m_CharNameMap.end())
        return iter->second;
    else
        return NULL;
}

void AgentCharMan::ChaNameSearch(
    GLCharAG* pChar,
    DWORD MapId,
    const std::string& findName,
    std::vector<std::string>& charNameVec,
    bool bSearchGM)
{
    for (CHAR_MAP_ITER it = m_CharNameMap.begin(); it != m_CharNameMap.end(); ++it)
    {
        const std::string& name = it->first;
        GLCharAG* pCharAg = it->second;

        if (GLCONST_CHAR::bSCHOOL_FREE_PK_ALWAYS)
        {            
            if (pCharAg->m_wSchool != pChar->m_wSchool)
                continue;
        }

        if (!bSearchGM && pCharAg->UserType() >= USER_GM4)
            continue;

        if( pChar->IsBlock(pCharAg->m_szName) || pCharAg->IsBlock(pChar->m_szName) )
            continue;

        if (MapId == SNATIVEID(false).Id() || MapId == pCharAg->GetCurrentMap().getGaeaMapID().Id() )
        {
            if (std::string::npos != name.find(findName))
                charNameVec.push_back(name);
        }
    }
}

void AgentCharMan::ChaNameSearch(
    GLCharAG* pChar,
    DWORD MapId,
    const std::string& findName,
    std::vector<std::pair<int, std::string> >& charNameVec,
    bool bSearchGM)
{
    for (CHAR_MAP_ITER it = m_CharNameMap.begin(); it != m_CharNameMap.end(); ++it)
    {
        const std::string& name = it->first;
        GLCharAG* pCharAg = it->second;

        if (GLCONST_CHAR::bSCHOOL_FREE_PK_ALWAYS)
        {            
            if (pCharAg->m_wSchool != pChar->m_wSchool)
                continue;
        }

        if (!bSearchGM && pCharAg->UserType() >= USER_GM4)
            continue;

        if( pChar->IsBlock(pCharAg->m_szName) || pCharAg->IsBlock(pChar->m_szName) )
            continue;

        if (MapId == SNATIVEID(false).Id() || MapId == pCharAg->GetCurrentMap().getGaeaMapID().Id())
        {
            if (std::string::npos != name.find(findName))
            {
                std::pair<int, std::string> FIND_DATA(pCharAg->GetChaLevel(), name);
                charNameVec.push_back(FIND_DATA);
            }
        }
    }
}

void AgentCharMan::ChaSaveUserNumAdd( DWORD dwUserNum )
{
	m_SaveUserNum.insert( CLIENTMAP_VALUE( dwUserNum, GetTickCount() ) );
}

void AgentCharMan::ChaSaveUserNumDel( DWORD dwUserNum )
{
	CLIENTMAP_ITER iter = m_SaveUserNum.find( dwUserNum );
	if ( iter != m_SaveUserNum.end() )
		m_SaveUserNum.erase( iter );
}

bool AgentCharMan::ChaSaveUserNumExist( DWORD dwUserNum )
{
	CLIENTMAP_ITER iter = m_SaveUserNum.find( dwUserNum );
	if ( iter != m_SaveUserNum.end() )
	{
		DWORD dwElapsedTime = GetTickCount() - iter->second;

		if ( dwElapsedTime > 600000 )
		{
			m_SaveUserNum.erase( iter );
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}
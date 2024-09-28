#include "../pch.h"
#include "./GLAGLandMan.h"
#include "../AgentServer/GLAgentServer.h"
#include "../../EngineLib/DxLand/DxLandMan.h"

#include "../../SigmaCore/DebugInclude.h"


GLAGLandMan::GLAGLandMan(GLAgentServer* pServer)
    : m_pServer(pServer)
    , m_dwClubMapID(UINT_MAX)
    , m_bGuidBattleMap(false)
    , m_bClubDeathMatchMap(false)
{
}

GLAGLandMan::~GLAGLandMan(void)
{
	CleanUp ();
}

BOOL GLAGLandMan::LoadWldFile ( const char* szWldFile, bool bLandGateLoad )
{
//	HRESULT hr;

	m_sLevelHead.m_strWldFile = szWldFile;

	char szPathName[MAX_PATH] = {0};
	StringCchCopy ( szPathName, MAX_PATH, DxLandMan::GetPath() );
	StringCchCat ( szPathName, MAX_PATH, m_sLevelHead.m_strWldFile.c_str() );

	char szFileType[FILETYPESIZE];
	DWORD dwVersion;

	sc::SerialFile SFile;
	if (SFile.OpenFile(FOT_READ, szPathName))
	{
		//	Note : 파일 형식 버전확인.
		//
		SFile.GetFileType( szFileType, FILETYPESIZE, dwVersion );

		if ( strcmp(DxLandMan::FILEMARK,szFileType) )
		{
			CString Str;
			Str.Format ( "[%s]  Map File Head Error.", m_sLevelHead.m_strWldFile.c_str() );
			MessageBox ( NULL, Str.GetString(), "ERROR", MB_OK );

			return FALSE;
		}

		if ( dwVersion > DxLandMan::VERSION )
		{
			CString Str;
			Str.Format ( "[%s]  Map File unknown version.", m_sLevelHead.m_strWldFile.c_str() );
			MessageBox ( NULL, Str.GetString(), "ERROR", MB_OK );

			return FALSE;
		}

		SLAND_FILEMARK sLandMark;

		//	Note : .wld 파일의 맵ID는 사용하지 않음.
		SNATIVEID sMapID;
		SFile >> sMapID.dwID;

		char szMapName[MAXLANDNAME];
		SFile.ReadBuffer ( szMapName, sizeof(char)*MAXLANDNAME );

		sLandMark.LoadSet ( SFile );

		//	Note : 출입구 설정 읽기.
		//
		if ( sLandMark.dwGATE_MARK && bLandGateLoad )
		{
			SFile.SetOffSet ( sLandMark.dwGATE_MARK );
			m_pLandGateMan->Load ( SFile );
		}
	}

	return TRUE;
}

BOOL GLAGLandMan::LoadFile(const char* szFile, MapAxisInfoMan* pAxisMan)
{
	GASSERT(szFile);
	BOOL bOk(FALSE);
	bOk = GLLevelFileServer::LoadFile(szFile, NULL, pAxisMan);
	if (!bOk)
        return FALSE;

	bOk = LoadWldFile(m_sLevelHead.m_strWldFile.c_str(), GLLevelFileServer::GetFileVer()<=VERSION_BEFORE_GATE);
	if (!bOk)
        return FALSE;
	return TRUE;
}

void GLAGLandMan::CleanUp ()
{
	m_setPC.clear();
	m_dwClubMapID = UINT_MAX;
	m_bGuidBattleMap = false;
	m_bClubDeathMatchMap = false;

	clearGateMan();
}

bool GLAGLandMan::DropPC(DWORD dwID)
{
	m_setPC.insert(dwID);
	return true;
}

bool GLAGLandMan::DropOutPC ( DWORD dwID )
{
	SETPC_ITER pos = m_setPC.find(dwID);
	if (pos == m_setPC.end())
        return false;
	m_setPC.erase(pos);
	return true;
}

void GLAGLandMan::SENDTOALLCLIENT(NET_MSG_GENERIC* nmg)
{
	PGLCHARAG pCHAR = NULL;
	SETPC_ITER pos = m_setPC.begin();
	SETPC_ITER end = m_setPC.end();
	for ( ; pos!=end; ++pos)
	{
		pCHAR = m_pServer->GetCharByDbNum((*pos));
		if (pCHAR)
			m_pServer->SENDTOCLIENT(pCHAR->ClientSlot(), nmg);
	}
}

bool GLAGLandMan::CheckCDMMembersLimited( DWORD ClubDbNum )
{
    unsigned int TotalCDMMembers = 0;

    PGLCHARAG pCHAR = NULL;
    SETPC_ITER pos = m_setPC.begin();
    SETPC_ITER end = m_setPC.end();
    for ( ; pos!=end; ++pos)
    {
        pCHAR = m_pServer->GetCharByDbNum((*pos));
        if ( pCHAR )
        {
            if ( pCHAR->GetClubDbNum() == ClubDbNum )
                TotalCDMMembers++;
        }
    }

    if ( TotalCDMMembers < GLCONST_CHAR::dwCLUB_DEATHMATCH_MEM )
        return true;
    else
        return false;
}

GLAGLandManager::GLAGLandManager(GLAgentServer* pAgentServer, const DWORD dwInstanceMapID)
: m_pAgentServer(pAgentServer)
, m_dwInstanceMapID(dwInstanceMapID)
, m_pScriptInstance(NULL)
{
    for ( unsigned int _index(MAX_LAND_SID); _index--; )
        m_pLand[_index] = NULL;
}
GLAGLandManager::GLAGLandManager(const GLAGLandManager& _rhs, const DWORD dwInstanceMapID, const DWORD dwFieldChannel, const DWORD dwFieldID)
: m_pAgentServer(_rhs.m_pAgentServer)
, m_dwInstanceMapID(dwInstanceMapID)
, m_dwFieldChannel(dwFieldChannel)
, m_dwFieldID(dwFieldID)
, m_pScriptInstance(NULL)
{
    for ( WORD _index(MAX_LAND_SID); _index--; )
    {
        GLAGLandMan* const pLandSource(_rhs.m_pLand[_index]);
        if ( pLandSource == NULL )
        {
            m_pLand[_index] = NULL;
            continue;
        }

        m_pLand[_index] = new GLAGLandMan(m_pAgentServer);
        *m_pLand[_index] = *pLandSource;
		m_pLand[_index]->SetGaeaMapID(SNATIVEID((WORD)dwInstanceMapID, _index));

        if ( pLandSource->IsPartyInstantMap() == true )
            m_pLand[_index]->SetInstantMapData(GAEAID_NULL, pLandSource->GetInstantMapHostID());
        else
            m_pLand[_index]->SetInstantMapData(pLandSource->GetInstantMapHostID(), GLPartyID());		
    }
}
GLAGLandManager::GLAGLandManager(const GLAGLandManager& _rhs)
: m_pAgentServer(_rhs.m_pAgentServer)
, m_dwInstanceMapID(_rhs.m_dwInstanceMapID)
, m_pScriptInstance(NULL)
{
    for ( unsigned int _i(MAX_LAND_SID); _i--; )
    {
        GLAGLandMan* const pLand(_rhs.m_pLand[_i]);
        if ( pLand == NULL )
        {
            m_pLand[_i] = NULL;
            continue;
        }

        m_pLand[_i] = new GLAGLandMan(m_pAgentServer);
        *m_pLand[_i] = *pLand;
		m_pLand[_i]->SetGaeaMapID(SNATIVEID(WORD(m_dwInstanceMapID), WORD(_i)));

        if ( pLand->IsPartyInstantMap() == true )
            m_pLand[_i]->SetInstantMapData(GAEAID_NULL, pLand->GetInstantMapHostID());
        else
            m_pLand[_i]->SetInstantMapData(pLand->GetInstantMapHostID(), GLPartyID());		
    }
}
GLAGLandManager::~GLAGLandManager(void)
{
    for ( unsigned int _i(MAX_LAND_SID); _i--; )
    {
        SAFE_DELETE(m_pLand[_i]);
    }
}

const bool GLAGLandManager::loadLevelFile(const SNATIVEID& baseMapID, const char* szFile, MapAxisInfoMan* pAxisMan)
{
	const WORD wMainMapID(baseMapID.Mid());
	const WORD wSubMapID(baseMapID.Sid());
	if ( wMainMapID >= MAX_LAND_MID )
		return false;

    if ( wSubMapID >= MAX_LAND_SID )
        return false;

    if ( m_pLand[wSubMapID] == 0 )
    {
        m_pLand[wSubMapID] = new GLAGLandMan(m_pAgentServer);
        if ( m_pLand[wSubMapID] == 0 )
            return false;
    }

    if ( m_pLand[wSubMapID]->LoadFile(szFile, pAxisMan) == FALSE )
    {
        SAFE_DELETE(m_pLand[wSubMapID]);
        return false;
    }
    return true;
}

const DWORD GLAGLandManager::getInstanceMapID(void) const
{
    return m_dwInstanceMapID;
}
const DWORD GLAGLandManager::getFieldChannel(void) const
{
	return m_dwFieldChannel;
}
const DWORD GLAGLandManager::getFieldID(void) const
{
	return m_dwFieldID;
}
const SNATIVEID GLAGLandManager::getBaseMapID(const unsigned int _indexStage) const
{
	if ( _indexStage >= MAX_LAND_SID )
		return SNATIVEID(false);

	if ( m_pLand[_indexStage] == NULL )
		return SNATIVEID(false);

    return m_pLand[_indexStage]->GetBaseMapID();
}
GLAGLandMan* const GLAGLandManager::getLand(const DWORD dwSubMapID) const
{
    if ( dwSubMapID >= MAX_LAND_SID )
        return NULL;

	// 현재 agent에서는 child-map을 사용할 일이 없으므로 기본 맵을 반환한다;
	// 사용할 일이 생길 경우 필드와 동일하게 생성 해주도록 한다;
    return m_pLand[0];
}

void GLAGLandManager::setScriptInstance(InstanceSystem::InstanceAgent* pScriptInstance)
{
	m_pScriptInstance = pScriptInstance;
}
void GLAGLandManager::setMapID(const SNATIVEID& baseMapID, const SNATIVEID& gaeaMapID, const bool bPeaceZone, const bool bPKZone)
{
	if ( baseMapID.Mid() >= MAX_LAND_MID )
		return;

	if ( baseMapID.Sid() >= MAX_LAND_SID )
		return;

	const WORD indexStage(gaeaMapID.Sid());
	if ( indexStage >= MAX_LAND_SID )
		return;	

	GLAGLandMan* const pLand(m_pLand[indexStage]);
    if ( pLand == 0 )
        return;
	
	pLand->SetGaeaMapID(gaeaMapID);
	pLand->SetBaseMapID(baseMapID);
	pLand->SetPeaceZone(bPeaceZone);
	pLand->SetPKZone(bPKZone);

}

InstanceSystem::InstanceAgent* const GLAGLandManager::getScriptInstance(void)
{
    return m_pScriptInstance;
}

void GLAGLandManager::setInstantMapData(const DWORD dwHostID, const GLPartyID& PartyID)
{
	if ( m_pLand[0] == NULL )
		return;

	m_pLand[0]->SetInstantMapData(dwHostID, PartyID);
}
#include "../pch.h"

#include "../../SigmaCore/Math/Random.h"
#include "../../SigmaCore/File/SerialFile.h"

#include "../../EngineLib/DxTools/Collision.h"
#include "../../EngineLib/DxTools/EditMeshs.h"
#include "../../EngineLib/Common/profile.h"

#include "../../RanLogic/Crow/GLCrowData.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"

#include "../Land/GLLandMan.h"

#include "GLMobScheduleManServer.h"

#include "../../SigmaCore/DebugInclude.h"



//-------------------------------------------------------------------------[GLMobScheduleManServer]
//
GLMobScheduleManServer::GLMobScheduleManServer() 
	: m_pLandMan(NULL)
    , m_dwMobMaxCount(0)
    , m_dwMobAliveCount(0)
	, m_dwLayerIndex(0)
{	
}

GLMobScheduleManServer::~GLMobScheduleManServer()
{
	CleanUp ();
}

const GLMobScheduleManServer& GLMobScheduleManServer::operator =(const GLMobScheduleManServer& _rhs)
{
	if ( this == &_rhs )
		return *this;

	GLMobScheduleMan::operator =(_rhs);
	m_pLandMan = _rhs.m_pLandMan;
	m_dwMobMaxCount = _rhs.m_dwMobMaxCount;
	m_dwMobAliveCount = _rhs.m_dwMobAliveCount;
	m_dwLayerIndex = _rhs.m_dwLayerIndex;
	return *this;
}

HRESULT GLMobScheduleManServer::InitMobScheduleState ()
{
	HRESULT hr = S_OK;
	if ( !m_pLandMan )	return E_FAIL;

	PROFILE_BEGIN("m_GLMobSchList");
	sc::SGLNODE<GLMobSchedule*>* pMySelf = m_GLMobSchList.m_pHead;
	for ( ; pMySelf; pMySelf = pMySelf->pNext )
	{
		GLMobSchedule* pMobSch = pMySelf->Data;

		if ( !pMobSch )
			continue;

		if ( pMobSch->m_dwVolatFlags & GLMobSchedule::EM_ONCE )
		{	
			pMobSch->m_dwVolatFlags ^= GLMobSchedule::EM_ONCE;
		}
	}
	PROFILE_END("m_GLMobSchList");

	
	
	PROFILE_BEGIN("m_mapMobGroub");
	
	MAPMOBGROUB_ITER pos = m_mapMobGroub.begin ();
	MAPMOBGROUB_ITER end = m_mapMobGroub.end ();
	
	for ( ; pos!=end; ++pos )
	{
		GLMOBGROUB &sGROUB = (*pos).second;
		
		if ( sGROUB.m_vecSchedule.empty() )
			continue;

		DWORD dwSCH_SIZE = (DWORD) sGROUB.m_vecSchedule.size();

		GLMobSchedule* pMobSch(NULL);

		if ( sGROUB.m_dwRendGenIndex!=UINT_MAX && sGROUB.m_dwRendGenIndex < dwSCH_SIZE )
		{
			pMobSch = sGROUB.m_vecSchedule[sGROUB.m_dwRendGenIndex];
		}
		else
		{
			//	새로운 몹을 선택.
			sGROUB.m_dwRendGenIndex = DWORD ( sc::Random::getInstance().GetValue() % dwSCH_SIZE );

			if ( sGROUB.m_dwRendGenIndex >= dwSCH_SIZE )
				sGROUB.m_dwRendGenIndex = 0;

			pMobSch = sGROUB.m_vecSchedule[sGROUB.m_dwRendGenIndex];
		}

		if ( pMobSch->m_dwVolatFlags & GLMobSchedule::EM_EXMODE )
		{
			pMobSch->m_dwVolatFlags ^= GLMobSchedule::EM_ONCE;  // by 경대
		}
	}

	PROFILE_END("m_mapMobGroub");

	return S_OK;
}

const DWORD GLMobScheduleManServer::DropCrow ( GLMobSchedule* pMobSchedule, float fElapsedTime, bool bFirstCreateForInstMap )
{
	if ( NULL == pMobSchedule )
		return UINT_MAX;

	if ( pMobSchedule->m_bALive )
	{
		m_dwMobAliveCount++;
		return UINT_MAX;
	}

	if ( pMobSchedule->m_bGroub )
		return UINT_MAX;

	// Regen Timer 를 이용할 경우 Timer 를 줄인다;
	if( pMobSchedule->GetUseRegenTimer() )
		pMobSchedule->m_fTimer -= fElapsedTime;

	// 리젠하지 않는 경우에는 Drop하지 않는다;
	if( !pMobSchedule->IsRegen() )
		return UINT_MAX;

	//	시간 지정 젠이면 현재 젠 시간을 지정해준다;
	if( pMobSchedule->GetUseRegenTimeStamp() )
		pMobSchedule->SetGenTime();

	//	Note : 시간이 되면 몹을 뿌려준다;
	//
	DWORD dwGlobID = UINT_MAX;
	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( pMobSchedule->m_CrowID );

	if ( !pCrowData )
		return UINT_MAX;

	if ( pCrowData->m_emCrow == CROW_MATERIAL ) 
		dwGlobID = m_pLandMan->DropMaterial( pMobSchedule, bFirstCreateForInstMap );
	else										 
		dwGlobID = m_pLandMan->DropCrow( pMobSchedule, bFirstCreateForInstMap );

	if ( dwGlobID != UINT_MAX )
	{
		pMobSchedule->m_bALive = TRUE;
		pMobSchedule->m_dwGlobID = dwGlobID;

		// 새로운 생성시간 부여;
		pMobSchedule->m_fTimer = static_cast<float>(
			sc::Random::getInstance().RandomNumber(
			pMobSchedule->m_fReGenTimeMin,
			pMobSchedule->m_fReGenTimeMax ) );

		if ( pMobSchedule->m_dwVolatFlags & GLMobSchedule::EM_EXMODE || 
			( m_pLandMan->GetUseLevelEtcFunc( EMETCFUNC_REGENONCE ) ) )
		{
			pMobSchedule->m_dwVolatFlags |= GLMobSchedule::EM_ONCE;  // by 경대
		}

		m_dwMobAliveCount++;
	}

	return dwGlobID;
}

HRESULT GLMobScheduleManServer::FrameMove( float fTime, float fElapsedTime, bool bFirstCreateForInstMap /* = FALSE */ )
{
	HRESULT hr = S_OK;
	
	if ( !m_pLandMan )
		return E_FAIL;

	//	if( m_pLandMan->IsInstantMap() ) return S_OK;

    m_dwMobMaxCount = m_GLMobSchList.m_dwAmount + m_mapMobGroub.size();
    m_dwMobAliveCount = 0;
    
    // 일반 Monster
	PROFILE_BEGIN("m_GLMobSchList");
	sc::SGLNODE<GLMobSchedule*>* pCur = m_GLMobSchList.m_pHead;


	for ( ; pCur; pCur = pCur->pNext )
	{
		GLMobSchedule* pMobSch = pCur->Data;
        if( pMobSch == NULL  )
            continue;
		
		DropCrow( pMobSch, fElapsedTime, bFirstCreateForInstMap );
	}
	PROFILE_END("m_GLMobSchList");


    // Boss Monster
	PROFILE_BEGIN("m_mapMobGroub");
	MAPMOBGROUB_ITER pos = m_mapMobGroub.begin ();
	MAPMOBGROUB_ITER end = m_mapMobGroub.end ();

	for ( ; pos!=end; ++pos )
	{
		GLMOBGROUB &sGROUB = (*pos).second;
		if ( sGROUB.m_vecSchedule.empty() )
			continue;

		DWORD dwSCH_SIZE = (DWORD) sGROUB.m_vecSchedule.size();
		
		GLMobSchedule* pMobSch(NULL);
		if ( sGROUB.m_dwRendGenIndex!=UINT_MAX && sGROUB.m_dwRendGenIndex < dwSCH_SIZE )
		{
			pMobSch = sGROUB.m_vecSchedule[sGROUB.m_dwRendGenIndex];
			
			//	Note : 생존시에는 무시한다.
			if ( pMobSch->m_bALive )
            {
                m_dwMobAliveCount++;
				continue;
            }

			//	Note : 이미 생성됬던 몹이 죽었을 때는 다른 몹을 지정해준다.
			//if ( pMobSch->m_fTimer < 0.0f )
			if ( pMobSch->IsRegen() )			
			{
				//	기존 몹에는 새로운 생성 지연 시간 지정.
				pMobSch->m_fTimer = static_cast<float>(
					sc::Random::getInstance().RandomNumber(pMobSch->m_fReGenTimeMin, pMobSch->m_fReGenTimeMax));

				//	새로운 몹을 선택.
				sGROUB.m_dwRendGenIndex = DWORD ( sc::Random::getInstance().GetValue() % dwSCH_SIZE );
				if ( sGROUB.m_dwRendGenIndex >= dwSCH_SIZE )		sGROUB.m_dwRendGenIndex = 0;

				pMobSch = sGROUB.m_vecSchedule[sGROUB.m_dwRendGenIndex];
				pMobSch->m_fTimer = static_cast<float>(
					sc::Random::getInstance().RandomNumber(pMobSch->m_fReGenTimeMin, pMobSch->m_fReGenTimeMax));
			}
		}
		else
		{
			//	새로운 몹을 선택.
			sGROUB.m_dwRendGenIndex = DWORD ( rand() % dwSCH_SIZE );
			if ( sGROUB.m_dwRendGenIndex >= dwSCH_SIZE )		sGROUB.m_dwRendGenIndex = 0;

			pMobSch = sGROUB.m_vecSchedule[sGROUB.m_dwRendGenIndex];
			pMobSch->m_fTimer = static_cast<float>(
				sc::Random::getInstance().RandomNumber(pMobSch->m_fReGenTimeMin, pMobSch->m_fReGenTimeMax));
		}

		// Regen Timer 를 이용할 경우 Timer 를 줄인다;
		if( pMobSch->GetUseRegenTimer() )
			pMobSch->m_fTimer -= fElapsedTime;

		// 리젠하지 않는 경우에는 Drop하지 않는다;
		if( !pMobSch->IsRegen() )
			continue;

		//	시간 지정 젠이면 현재 젠 시간을 지정해준다;
		if( pMobSch->GetUseRegenTimeStamp() )
			pMobSch->SetGenTime();

		//	Note : 시간이 되면 몹을 뿌려준다.
		//
		DWORD dwGlobID(UINT_MAX);
		const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( pMobSch->m_CrowID );
		if ( !pCrowData )
			continue;
		
		if ( pCrowData->m_emCrow == CROW_MATERIAL )	
			dwGlobID = m_pLandMan->DropMaterial(pMobSch,bFirstCreateForInstMap);
		else											
			dwGlobID = m_pLandMan->DropCrow(pMobSch,bFirstCreateForInstMap);

		if ( dwGlobID != UINT_MAX )
		{
			pMobSch->m_bALive = TRUE;
			pMobSch->m_dwGlobID = dwGlobID;

			if ( pMobSch->m_dwVolatFlags & GLMobSchedule::EM_EXMODE )
			{
				pMobSch->m_dwVolatFlags |= GLMobSchedule::EM_ONCE;  // by 경대
			}

			m_dwMobAliveCount++;
		}
	}
	PROFILE_END("m_mapMobGroub");

	return S_OK;
}

void GLMobScheduleManServer::SetLandMan ( GLLandMan* pLandMan )
{
	m_pLandMan = pLandMan;
}

void GLMobScheduleManServer::CleanUp ()
{
    GLMobScheduleMan::CleanUp();

	m_pLandMan = NULL;
}

DWORD GLMobScheduleManServer::GetAliveCount()
{
    return m_dwMobAliveCount;
}



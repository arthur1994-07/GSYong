#include "../pch.h"

#include "../../SigmaCore/File/SerialFile.h"

#include "../../EngineLib/DxTools/Collision.h"
#include "../../EngineLib/DxTools/EditMeshs.h"

#include "./GLCrowData.h"
#include "./GLCrowDataMan.h"
#include "GLMobScheduleMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//-------------------------------------------------------------------------[GLMobScheduleMan]
//
const DWORD GLMobScheduleMan::VERSION = 0x0100;

GLMobScheduleMan::GLMobScheduleMan()
{	
	memset(m_szFileName, 0, sizeof(char) * (MAX_PATH));
}

GLMobScheduleMan::~GLMobScheduleMan()
{
	CleanUp ();
}

const GLMobScheduleMan& GLMobScheduleMan::operator =(const GLMobScheduleMan& rhs)
{
	if ( this == &rhs )
		return *this;

	StringCchCopy ( m_szFileName, MAX_PATH, rhs.m_szFileName );

	m_setMobDesc = rhs.m_setMobDesc;
	m_mapMobGroub = rhs.m_mapMobGroub;

	m_GLMobSchList.DELALL ();
	sc::SGLNODE<GLMobSchedule*>* pCur = rhs.GetMobSchList()->m_pHead;
	while ( pCur )
	{
		GLMobSchedule* pOldMObSch = pCur->Data;

		GLMobSchedule* pMobSch = new GLMobSchedule;
		*pMobSch = *pOldMObSch;

		m_GLMobSchList.ADDTAIL ( pMobSch );

		pCur = pCur->pNext;
	};

	return *this;
}

void GLMobScheduleMan::AddMobSchGroub ( GLMobSchedule *pMobSch )
{
	if (!pMobSch)
        return;

	const PCROWDATA pDATA = GLCrowDataMan::GetInstance().GetCrowData(pMobSch->m_CrowID);
	if (!pDATA)
        return;
	if ((pDATA->m_sAction.m_dwActFlag&EMCROWACT_RANDOMREGEN) == NULL)
        return;

	pMobSch->m_bGroub = TRUE;

	MAPMOBGROUB_ITER pos = m_mapMobGroub.find ( pMobSch->m_CrowID.dwID );
	if (pos != m_mapMobGroub.end())
	{
		GLMOBGROUB& GROUB = (*pos).second;
		GROUB.ADD(pMobSch);
	}
	else
	{
		GLMOBGROUB sNEW;
		sNEW.ADD(pMobSch);
		m_mapMobGroub[pMobSch->m_CrowID.dwID] = sNEW;
	}
}

void GLMobScheduleMan::DelMobSchGroub ( GLMobSchedule *pMobSch )
{
	if (!pMobSch)
        return;

	const PCROWDATA pDATA = GLCrowDataMan::GetInstance().GetCrowData ( pMobSch->m_CrowID );
	if (!pDATA)
        return;
	if ((pDATA->m_sAction.m_dwActFlag&EMCROWACT_BOSS)==NULL)
        return;

	MAPMOBGROUB_ITER pos = m_mapMobGroub.find ( pMobSch->m_CrowID.dwID );
	if (pos != m_mapMobGroub.end())
	{
		GLMOBGROUB& GROUB = (*pos).second;
		GROUB.DEL(pMobSch);
	}
}

sc::SGLNODE<GLMobSchedule*>* GLMobScheduleMan::FindMobSch ( const char *szName )
{
	sc::SGLNODE<GLMobSchedule*>* pCur = m_GLMobSchList.m_pHead;

	for ( ; pCur; pCur = pCur->pNext )
	{
		if ( !strcmp ( pCur->Data->m_szName, szName ) )
		{
			return pCur;
		}
	}

	return NULL;
}

BOOL GLMobScheduleMan::DelMobSch ( const char *szName )
{
	sc::SGLNODE<GLMobSchedule*>* pCur = m_GLMobSchList.m_pHead;

	for ( ; pCur; pCur = pCur->pNext )
	{
		if ( !strcmp ( pCur->Data->m_szName, szName ) )
		{
			DelMobSchGroub ( pCur->Data );

			SAFE_DELETE(pCur->Data);
			m_GLMobSchList.DELNODE ( pCur );
			return TRUE;
		}
	}

	return FALSE;
}

GLMobSchedule* GLMobScheduleMan::GetCollisionMobSchedule ( D3DXVECTOR3 vPoint1, D3DXVECTOR3 vPoint2 )
{
	sc::SGLNODE<GLMobSchedule*>* pCur = m_GLMobSchList.m_pHead;

	for ( ; pCur; pCur = pCur->pNext )
	{
		GLMobSchedule* pMobSch = pCur->Data;

		BOOL bCol = pMobSch->IsCollision ( vPoint1, vPoint2 );
		if ( bCol )		return pMobSch;
	}

	return NULL;
}

void GLMobScheduleMan::SetObjRotate90()
{
	sc::SGLNODE<GLMobSchedule*>* pCur = m_GLMobSchList.m_pHead;

	for ( ; pCur; pCur = pCur->pNext )
	{
		pCur->Data->SetObjRotate90();
	}
}

HRESULT GLMobScheduleMan::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv )
{
	HRESULT hr = S_OK;
	
	if ( GLMobSchedule::m_bRENDACT )
	{
		sc::SGLNODE<GLMobSchedule*>* pCur = m_GLMobSchList.m_pHead;

		while ( pCur )
		{
			GLMobSchedule* pMobSch = pCur->Data;
			pMobSch->Render ( pd3dDevice, cv );

			pCur = pCur->pNext;
		}
	}

	return S_OK;
}

BOOL GLMobScheduleMan::LoadFile ( sc::BaseStream &SFile )
{
	m_setMobDesc.clear ();

	BOOL bExist;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	dwSize = SFile.ReadBlockSize ();

	SFile >> bExist;
	while ( bExist )
	{
		GLMobSchedule* pMobSch = new GLMobSchedule;

		pMobSch->Load ( SFile );
		m_GLMobSchList.ADDTAIL ( pMobSch );

		AddMobSchGroub ( pMobSch );
		m_setMobDesc.insert ( pMobSch->m_CrowID.dwID );

		SFile >> bExist;
	}

	return TRUE;
}

BOOL GLMobScheduleMan::SaveFile ( sc::SerialFile &SFile )
{
	SFile << VERSION;
	SFile.BeginBlock();

	sc::SGLNODE<GLMobSchedule*>* pCur = m_GLMobSchList.m_pHead;
	SFile << BOOL ( pCur!=NULL );

	while ( pCur )
	{
		GLMobSchedule* pMobSch = pCur->Data;

		pMobSch->Save ( SFile );

		pCur = pCur->pNext;
		SFile << BOOL ( pCur!=NULL );
	}

	SFile.EndBlock();

	return TRUE;
}

void GLMobScheduleMan::CleanUp ()
{
	memset(m_szFileName, 0, sizeof(char) * (MAX_PATH));

	sc::SGLNODE<GLMobSchedule*>* pCur = m_GLMobSchList.m_pHead;
	while ( pCur )
	{
		SAFE_DELETE(pCur->Data);
		pCur = pCur->pNext;
	}

	m_GLMobSchList.DELALL ();

	m_mapMobGroub.clear();
}



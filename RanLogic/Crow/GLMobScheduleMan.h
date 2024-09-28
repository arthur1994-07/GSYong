#pragma once

#include <hash_map>
#include <hash_set>
#include <set>

#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/Container/GLList.h"

#include "../../EngineLib/DxTools/DxMethods.h"
#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../EngineLib/NaviMesh/NavigationMesh.h"

#include "../../RanLogic/Crow/GLMobSchedule.h"

namespace sc {
    class SerialFile;
}

class GLMobScheduleMan
{
public:
	typedef std::set<DWORD>				MOBDESC;
	typedef std::set<DWORD>::iterator	MOBDESC_ITER;

protected:
	const static DWORD		VERSION;

	char					m_szFileName[MAX_PATH];
	MOBSCHEDULELIST			m_GLMobSchList;			//	스케쥴 리스트.
	MOBDESC					m_setMobDesc;			//	설정된 몹 종류. ( 형상정보 사전 로딩을 위해서 )
	MAPMOBGROUB				m_mapMobGroub;			//	특정 종류의 몹(보스)을 랜덤 위치에 출현하기 위해서 여러개의 스케쥴을 쓸수 있게 만듬.

public:
	sc::SGLNODE<GLMobSchedule*>* FindMobSch ( const char *szName );
	
	void AddMobSchClone ( GLMobSchedule MobSch )
	{
		GLMobSchedule* pNewMobSch = new GLMobSchedule;
		*pNewMobSch = MobSch;

		m_GLMobSchList.ADDTAIL ( pNewMobSch );
	}

	void AddMobSch ( GLMobSchedule *pMobSch )
	{
		m_GLMobSchList.ADDTAIL ( pMobSch );
	}

	void AddMobSch ( const GLMobSchedule &MobSch )
	{
		GLMobSchedule *pMobSch = new GLMobSchedule;
		*pMobSch = MobSch;
		m_GLMobSchList.ADDTAIL ( pMobSch );
	}


	void AddMobSchGroub ( GLMobSchedule *pMobSch );
	void DelMobSchGroub ( GLMobSchedule *pMobSch );

	BOOL DelMobSch ( const char *szName );
	const sc::CGLLIST<GLMobSchedule*>* GetMobSchList () const	{ return &m_GLMobSchList; }
	MOBDESC&				 GetMobDesc ()		{ return m_setMobDesc; }
	MAPMOBGROUB&			 GetMobGroubMap ()  { return m_mapMobGroub; }


public:
	GLMobSchedule* GetCollisionMobSchedule ( D3DXVECTOR3 vPoint1, D3DXVECTOR3 vPoint2 );

public:
	void SetObjRotate90();

public:
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );

public:
	void CleanUp ();

public:
	BOOL LoadFile ( sc::BaseStream &SFile );
	BOOL SaveFile ( sc::SerialFile &SFile );

public:
	virtual const GLMobScheduleMan& operator =(const GLMobScheduleMan& _rhs);
	GLMobScheduleMan();
	virtual ~GLMobScheduleMan();

public:
	void Reset ()
	{
		sc::SGLNODE<GLMobSchedule*>* pCur = m_GLMobSchList.m_pHead;
		while ( pCur )
		{
			GLMobSchedule* pMObSch = pCur->Data;
			pMObSch->Reset();

			pCur = pCur->pNext;
		};
	}
};

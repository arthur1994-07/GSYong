#include "pch.h"

#include "../../EngineLib/NaviMesh/Collision/LayerCollision.h"
#include "../../EngineLib/DxLand/DxLandEff.h"
#include "../../EngineLib/G-Logic/GLOGIC.h"
#include "../../EngineLib/Common/SUBPATH.h"

#include "../../RanLogic/Land/GLLandMark.h"
#include "../../RanLogic/Land/MapAxisInfoMan.h"
#include "../../RanLogic/Quest/GLQuest.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "GLLevelFileServer.h"


#include "../../SigmaCore/DebugInclude.h"


GLLevelFileServer::GLLevelFileServer()
{
}

GLLevelFileServer::~GLLevelFileServer()
{
}

void GLLevelFileServer::ResetData()
{
    GLLevelFile::ResetData();

    if( m_MobSchMan )
        m_MobSchMan->CleanUp();
}

void GLLevelFileServer::SetObjRotate90()
{
    GLLevelFile::SetObjRotate90();

	// Note : 몹
	m_MobSchMan->SetObjRotate90();
}

BOOL GLLevelFileServer::CheckMobSchMan( const MOBSCHEDULELIST* pMobScheduleList )
{	
	/// 저장하기전에 몹 스케쥴에 리더 그룹이 제대로 설정되어 있는지 체크한다.
	MOBSCHEDULENODE *pHead = pMobScheduleList->m_pHead;

	std::map<std::string,BOOL> mapMobGroupInfo;
	while(pHead)
	{		
		GLMobSchedule *pSchdule = pHead->Data;
		if( pSchdule->m_strGroupName != ""  )
		{
			if( mapMobGroupInfo.find( pSchdule->m_strGroupName ) == mapMobGroupInfo.end() )
			{
				mapMobGroupInfo[pSchdule->m_strGroupName] = FALSE;
			}
			if( pSchdule->m_bLeaderMonster )
			{
				mapMobGroupInfo[pSchdule->m_strGroupName] = TRUE;
			}
		}
		pHead = pHead->pNext;
	}

	std::map<std::string,BOOL>::iterator mapMobGroup_Iter = mapMobGroupInfo.begin();
	for( ; mapMobGroup_Iter != mapMobGroupInfo.end(); ++mapMobGroup_Iter )
	{
		if( !mapMobGroup_Iter->second )
		{
			char szTempChar[256] = {0,};
			sprintf( szTempChar, "You must set up leader of the \"%s\" group", mapMobGroup_Iter->first.c_str() );
			MessageBox( NULL, szTempChar, "Warning", MB_OK );
			return FALSE;

		}
	}

	return TRUE;
}

BOOL GLLevelFileServer::SaveFile ( const char *szFile )
{

	for (MobSchManContainerIterator it = m_MobSchManContainer.begin(); it != m_MobSchManContainer.end(); ++it)
	{
		if (CheckMobSchMan((*it)->GetMobSchList()) == FALSE)
		{
			return FALSE;
		}
	}

	GASSERT(szFile);

	std::string strFullPath = sc::getAppPath();
	strFullPath.append(SubPath::GLOGIC_LEVEL_FILE);
	strFullPath.append(szFile);

	sc::SerialFile SFile;
	SFile.SetFileType ( FILE_EXT, VERSION );
	if ( !SFile.OpenFile ( FOT_WRITE, strFullPath.c_str() ) )	return FALSE;
	SFile.SetEncode ( true );

	m_strFileName = szFile;

	m_sLevelHead.SAVE ( SFile );
	m_sLevelRequire.SAVE ( SFile );

	SFile.BeginBlock();
	{
		SFile << (int)m_MobSchManContainer.size();

		for (MobSchManContainerIterator it = m_MobSchManContainer.begin(); it != m_MobSchManContainer.end(); ++it)
		{
			(*it)->SaveFile ( SFile );
		}
	}
	SFile.EndBlock();

	SFile.BeginBlock();
	{
		for (LandGateManContainerIterator it = m_LandGateManContainer.begin(); it != m_LandGateManContainer.end(); ++it)
		{
			DxLandGateMan* pLandGateMan = *it;
			pLandGateMan->Save ( SFile );
		}
	}
	SFile.EndBlock();


	SFile << static_cast<DWORD>( m_vecLayerEffect.size() );
	SFile.BeginBlock();
	{
		for ( DWORD i=0; i<m_vecLayerEffect.size(); ++i )
		{
			m_vecLayerEffect[i]->Save( SFile );
		}
	}
	SFile.EndBlock();


	if ( m_vecLandMark.size()==0 )
	{
		SFile << static_cast<BOOL>(FALSE);
	}
	else
	{
		SFile << static_cast<BOOL>(TRUE);
	}
	SFile.BeginBlock();
	{
		for ( DWORD i=0; i<m_vecLandMark.size(); ++i )
		{
			m_vecLandMark[i]->SAVE ( SFile );

			if ( i >= (m_vecLandMark.size()-1) )
			{
				SFile << static_cast<BOOL>(FALSE);
			}
			else
			{
				SFile << static_cast<BOOL>(TRUE);
			}
		}
	}
	SFile.EndBlock();

	m_sLevelEtcFunc.SAVE ( SFile );

	SFile << static_cast<DWORD>( m_vecLayerCollision.size() );
	SFile.BeginBlock();
	{
		for ( DWORD i=0; i<m_vecLayerCollision.size(); ++i )
		{
			m_vecLayerCollision[i]->Save( SFile );
		}
	}
	SFile.EndBlock();

	return TRUE;
}

BOOL GLLevelFileServer::LOAD_000 ( sc::BaseStream &SFile )
{
	//	Note : 파일 구조 변경전
	//
	DWORD dwVer;
	SFile >> dwVer;
	if ( dwVer!=0x0100 )
	{
		MessageBox ( NULL, "LevelFile unknown data version.", "ERROR", MB_OK );
		return FALSE;
	}

	SLEVEL_HEAD_100 sOldHead;
	SFile.ReadBuffer ( &sOldHead, sizeof(SLEVEL_HEAD_100) );
	m_sLevelHead.Assign ( sOldHead );

	
	{
		SpMobScheduleManServer pMobSchedule(new GLMobScheduleManServer);
		if ( pMobSchedule->LoadFile(SFile) == FALSE )
			return FALSE;
		m_MobSchManContainer.push_back(pMobSchedule);
	}

	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );

	return TRUE;
}

BOOL GLLevelFileServer::LOAD_101 ( sc::BaseStream &SFile )
{
	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	
	{
		SpMobScheduleManServer pMobSchedule(new GLMobScheduleManServer);
		if ( pMobSchedule->LoadFile(SFile) == FALSE )
			return FALSE;
		m_MobSchManContainer.push_back(pMobSchedule);
	}

	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );

	return TRUE;
}

BOOL GLLevelFileServer::LOAD_102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptBaseEffect )
{
	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
	m_vecLayerEffect[0]->LoadOLD( SFile, bExceptBaseEffect );

	BOOL bLOAD(FALSE);
	SFile >> bLOAD;
	if ( bLOAD )
	{
		while ( bLOAD )
		{
			PLANDMARK pLandMark = new GLLANDMARK;
			pLandMark->LOAD ( SFile );
			AddLandMark ( pLandMark );
			
			SFile >> bLOAD;
		}
	
		//BuildLandMarkTree ();
	}

	{
		DxLandGateMan* pLandGateMan(new DxLandGateMan);
		if ( FAILED(pLandGateMan->Load(SFile)) )
			return FALSE;
		m_LandGateManContainer.push_back(pLandGateMan);
	}
	
	{
		SpMobScheduleManServer pMobSchedule(new GLMobScheduleManServer);
		if ( pMobSchedule->LoadFile(SFile) == FALSE )
			return FALSE;
		m_MobSchManContainer.push_back(pMobSchedule);
	}

	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );

	return TRUE;
}

BOOL GLLevelFileServer::LOAD_103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptBaseEffect )
{
	DWORD dwBLOCKSIZE(0);

	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
	m_vecLayerEffect[0]->LoadOLD_2( SFile, pd3dDevice ? (!bExceptBaseEffect ? FALSE : TRUE) : TRUE );

	BOOL bLOAD(FALSE);
	SFile >> bLOAD;
	dwBLOCKSIZE = SFile.ReadBlockSize();
	if ( pd3dDevice )
	{
		if ( bLOAD )
		{
			while ( bLOAD )
			{
				PLANDMARK pLandMark = new GLLANDMARK;
				pLandMark->LOAD ( SFile );
				AddLandMark ( pLandMark );
				
				SFile >> bLOAD;
			}
		
			//BuildLandMarkTree ();
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
	}

	dwBLOCKSIZE = SFile.ReadBlockSize();
	{
		DxLandGateMan* pLandGateMan(new DxLandGateMan);
		if ( FAILED(pLandGateMan->Load(SFile)) )
			return FALSE;
		m_LandGateManContainer.push_back(pLandGateMan);
	}

	dwBLOCKSIZE = SFile.ReadBlockSize();	
	{
		SpMobScheduleManServer pMobSchedule(new GLMobScheduleManServer());
		if ( pMobSchedule->LoadFile(SFile) == FALSE )
			return FALSE;
		m_MobSchManContainer.push_back(pMobSchedule);
	}
	//else
	//{
	//	SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
	//}

	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );

	return TRUE;
}

BOOL GLLevelFileServer::LOAD_104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptBaseEffect )
{
	DWORD dwBLOCKSIZE(0);

	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );
	m_sLevelEtcFunc.LOAD ( SFile );

	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
	m_vecLayerEffect[0]->LoadOLD_2( SFile, pd3dDevice ? (!bExceptBaseEffect ? FALSE : TRUE) : TRUE );

	BOOL bLOAD(FALSE);
	SFile >> bLOAD;
	dwBLOCKSIZE = SFile.ReadBlockSize();
	if ( pd3dDevice )
	{
		if ( bLOAD )
		{
			while ( bLOAD )
			{
				PLANDMARK pLandMark = new GLLANDMARK;
				pLandMark->LOAD ( SFile );
				AddLandMark ( pLandMark );

				SFile >> bLOAD;
			}

			//BuildLandMarkTree ();
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
	}

	dwBLOCKSIZE = SFile.ReadBlockSize();
	{
		DxLandGateMan* pLandGateMan(new DxLandGateMan);
		if ( FAILED(pLandGateMan->Load(SFile)) )
			return FALSE;
		m_LandGateManContainer.push_back(pLandGateMan);
	}

	dwBLOCKSIZE = SFile.ReadBlockSize();	
	{
		SpMobScheduleManServer pMobSchedule(new GLMobScheduleManServer());
		if ( pMobSchedule->LoadFile(SFile) == FALSE )
			return FALSE;


		m_MobSchManContainer.push_back(pMobSchedule);
	}
	//else
	//{
	//	SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
	//}

	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );

	return TRUE;
}

BOOL GLLevelFileServer::LOAD_105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptBaseEffect )
{
	DWORD dwBLOCKSIZE(0);

	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	dwBLOCKSIZE = SFile.ReadBlockSize();
	
	{
		SpMobScheduleManServer pMobSchedule(new GLMobScheduleManServer);
		if ( pMobSchedule->LoadFile(SFile) == FALSE )
			return FALSE;


		m_MobSchManContainer.push_back(pMobSchedule);
	}
	//else
	//{
	//	SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
	//}

	dwBLOCKSIZE = SFile.ReadBlockSize();
	{
		DxLandGateMan* pLandGateMan(new DxLandGateMan);
		if ( FAILED(pLandGateMan->Load(SFile)) )
			return FALSE;
		m_LandGateManContainer.push_back(pLandGateMan);
	}

	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
	m_vecLayerEffect[0]->LoadOLD_2( SFile, pd3dDevice ? (!bExceptBaseEffect ? FALSE : TRUE) : TRUE );

	BOOL bLOAD(FALSE);
	SFile >> bLOAD;
	dwBLOCKSIZE = SFile.ReadBlockSize();
	if ( pd3dDevice )
	{
		if ( bLOAD )
		{
			while ( bLOAD )
			{
				PLANDMARK pLandMark = new GLLANDMARK;
				pLandMark->LOAD ( SFile );
				AddLandMark ( pLandMark );

				SFile >> bLOAD;
			}

			//BuildLandMarkTree ();
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
	}

	
	m_sLevelEtcFunc.LOAD ( SFile );

	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );

	return TRUE;
}

BOOL GLLevelFileServer::LOAD_106 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptBaseEffect )
{
	DWORD dwBLOCKSIZE(0);

	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	dwBLOCKSIZE = SFile.ReadBlockSize();

	int count = 0;
	SFile >> count;
	
	for (int i = 0; i < count; ++i)
	{
		SpMobScheduleManServer spMobSchMan(new GLMobScheduleManServer);

		if (spMobSchMan)
		{
			if (spMobSchMan->LoadFile(SFile))
			{

				m_MobSchManContainer.push_back(spMobSchMan);
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}	

	dwBLOCKSIZE = SFile.ReadBlockSize();
	{
		DxLandGateMan* pRootLandGateMan(new DxLandGateMan);
		if ( FAILED(pRootLandGateMan->Load(SFile)) )
			return FALSE;

		m_LandGateManContainer.push_back(pRootLandGateMan);
		for (int i = 1; i < count; ++i)
		{
			DxLandGateMan* pLandGateMan(new DxLandGateMan);
			*pLandGateMan = *pRootLandGateMan;

			m_LandGateManContainer.push_back(pLandGateMan);
		}
	}


	// Efffect
	{
		if ( count <= 0 )
		{
			m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
			SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
		}
		else
		{
			m_vecLayerEffect.reserve( m_vecLayerEffect.capacity() + count );

			for (int i = 0; i < count; ++i)
			{
				m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
			}

			m_vecLayerEffect[0]->LoadOLD_2( SFile, pd3dDevice ? (!bExceptBaseEffect ? FALSE : TRUE) : TRUE );
		}
	}


	BOOL bLOAD(FALSE);
	SFile >> bLOAD;
	dwBLOCKSIZE = SFile.ReadBlockSize();
	if ( pd3dDevice )
	{
		if ( bLOAD )
		{
			while ( bLOAD )
			{
				PLANDMARK pLandMark = new GLLANDMARK;
				pLandMark->LOAD ( SFile );
				AddLandMark ( pLandMark );

				SFile >> bLOAD;
			}

			//BuildLandMarkTree ();
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
	}


	m_sLevelEtcFunc.LOAD ( SFile );

	// Collision
	{
		if ( count <= 0 )
		{
			m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );
		}
		else
		{
			m_vecLayerCollision.reserve( m_vecLayerCollision.capacity() + count );

			for (int i = 0; i < count; ++i)
			{
				m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );
			}
		}
	}

	return TRUE;
}

BOOL GLLevelFileServer::LOAD_107 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptBaseEffect )
{
	DWORD dwBLOCKSIZE(0);

	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	dwBLOCKSIZE = SFile.ReadBlockSize();

	int count = 0;
	SFile >> count;

	for (int i = 0; i < count; ++i)
	{
		SpMobScheduleManServer spMobSchMan(new GLMobScheduleManServer);

		if (spMobSchMan)
		{
			if (spMobSchMan->LoadFile(SFile))
			{

				m_MobSchManContainer.push_back(spMobSchMan);
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}	

	dwBLOCKSIZE = SFile.ReadBlockSize();
	{		
		for (int i = 0; i < count; ++i)
		{
			DxLandGateMan* pLandGateMan(new DxLandGateMan);
			if ( FAILED(pLandGateMan->Load(SFile)) )
				return FALSE;

			m_LandGateManContainer.push_back(pLandGateMan);
		}
	}


	// Efffect
	{
		if ( count <= 0 )
		{
			m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
			SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
		}
		else
		{
			m_vecLayerEffect.reserve( m_vecLayerEffect.capacity() + count );

			for (int i = 0; i < count; ++i)
			{
				m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
			}

			m_vecLayerEffect[0]->LoadOLD_2( SFile, pd3dDevice ? (!bExceptBaseEffect ? FALSE : TRUE) : TRUE );
		}
	}


	BOOL bLOAD(FALSE);
	SFile >> bLOAD;
	dwBLOCKSIZE = SFile.ReadBlockSize();
	if ( pd3dDevice )
	{
		if ( bLOAD )
		{
			while ( bLOAD )
			{
				PLANDMARK pLandMark = new GLLANDMARK;
				pLandMark->LOAD ( SFile );
				AddLandMark ( pLandMark );

				SFile >> bLOAD;
			}

			//BuildLandMarkTree ();
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
	}


	m_sLevelEtcFunc.LOAD ( SFile );

	// Collision
	{
		if ( count <= 0 )
		{
			m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );
		}
		else
		{
			m_vecLayerCollision.reserve( m_vecLayerCollision.capacity() + count );

			for (int i = 0; i < count; ++i)
			{
				m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );
			}
		}
	}

	return TRUE;
}

BOOL GLLevelFileServer::LOAD_108 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptBaseEffect )
{
	DWORD dwBLOCKSIZE(0);

	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	dwBLOCKSIZE = SFile.ReadBlockSize();

	int count = 0;
	SFile >> count;

	for (int i = 0; i < count; ++i)
	{
		SpMobScheduleManServer spMobSchMan(new GLMobScheduleManServer);

		if (spMobSchMan)
		{
			if (spMobSchMan->LoadFile(SFile))
			{

				m_MobSchManContainer.push_back(spMobSchMan);
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}	

	dwBLOCKSIZE = SFile.ReadBlockSize();
	{		
		for (int i = 0; i < count; ++i)
		{
			DxLandGateMan* pLandGateMan(new DxLandGateMan);
			if ( FAILED(pLandGateMan->Load(SFile)) )
				return FALSE;

			m_LandGateManContainer.push_back(pLandGateMan);
		}
	}



	// Efffect
	{
		int nEffectSize = 0;
		SFile >> nEffectSize;

		dwBLOCKSIZE = SFile.ReadBlockSize();
		{
			if ( nEffectSize <= 0 )
			{
				m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
				SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
			}
			else
			{
				m_vecLayerEffect.reserve( m_vecLayerEffect.capacity() + nEffectSize - 1 );

				m_vecLayerEffect[0]->Load( SFile, pd3dDevice ? (!bExceptBaseEffect ? FALSE : TRUE) : TRUE );

				for (int i = 0; i < nEffectSize-1; ++i)
				{
					boost::shared_ptr<LayerEffect> spLayerEffect = boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE));
					spLayerEffect->Load( SFile, pd3dDevice ? FALSE : TRUE );
					m_vecLayerEffect.push_back( spLayerEffect );
				}
			}
		}
	}

	

	BOOL bLOAD(FALSE);
	SFile >> bLOAD;
	dwBLOCKSIZE = SFile.ReadBlockSize();
	if ( pd3dDevice )
	{
		if ( bLOAD )
		{
			while ( bLOAD )
			{
				PLANDMARK pLandMark = new GLLANDMARK;
				pLandMark->LOAD ( SFile );
				AddLandMark ( pLandMark );

				SFile >> bLOAD;
			}

			//BuildLandMarkTree ();
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
	}


	m_sLevelEtcFunc.LOAD ( SFile );

	// Collision
	{
		if ( count <= 0 )
		{
			m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );
		}
		else
		{
			m_vecLayerCollision.reserve( m_vecLayerCollision.capacity() + count );

			for (int i = 0; i < count; ++i)
			{
				m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );
			}
		}
	}

	return TRUE;
}

BOOL GLLevelFileServer::LOAD_109 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptBaseEffect )
{
	DWORD dwBLOCKSIZE(0);

	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	dwBLOCKSIZE = SFile.ReadBlockSize();

	int count = 0;
	SFile >> count;

	for (int i = 0; i < count; ++i)
	{
		SpMobScheduleManServer spMobSchMan(new GLMobScheduleManServer);

		if (spMobSchMan)
		{
			if (spMobSchMan->LoadFile(SFile))
			{

				m_MobSchManContainer.push_back(spMobSchMan);
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}	

	dwBLOCKSIZE = SFile.ReadBlockSize();
	{		
		for (int i = 0; i < count; ++i)
		{
			DxLandGateMan* pLandGateMan(new DxLandGateMan);
			if ( FAILED(pLandGateMan->Load(SFile)) )
				return FALSE;

			m_LandGateManContainer.push_back(pLandGateMan);
		}
	}



	// Efffect
	{
		int nEffectSize = 0;
		SFile >> nEffectSize;

		dwBLOCKSIZE = SFile.ReadBlockSize();
		{
			if ( nEffectSize <= 0 )
			{
				m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
				SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
			}
			else
			{
				m_vecLayerEffect.reserve( m_vecLayerEffect.capacity() + nEffectSize - 1 );

				m_vecLayerEffect[0]->Load( SFile, pd3dDevice ? (!bExceptBaseEffect ? FALSE : TRUE) : TRUE );

				for (int i = 0; i < nEffectSize-1; ++i)
				{
					boost::shared_ptr<LayerEffect> spLayerEffect = boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE));
					spLayerEffect->Load( SFile, pd3dDevice ? FALSE : TRUE );
					m_vecLayerEffect.push_back( spLayerEffect );
				}
			}
		}
	}



	BOOL bLOAD(FALSE);
	SFile >> bLOAD;
	dwBLOCKSIZE = SFile.ReadBlockSize();
	if ( pd3dDevice )
	{
		if ( bLOAD )
		{
			while ( bLOAD )
			{
				PLANDMARK pLandMark = new GLLANDMARK;
				pLandMark->LOAD ( SFile );
				AddLandMark ( pLandMark );

				SFile >> bLOAD;
			}

			//BuildLandMarkTree ();
		}
	}
	else
	{
		SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
	}


	m_sLevelEtcFunc.LOAD ( SFile );


	// Collision
	{
		int nCollisionSize = 0;
		SFile >> nCollisionSize;

		dwBLOCKSIZE = SFile.ReadBlockSize();
		{
			if ( nCollisionSize <= 0 )
			{
				m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );
				SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
			}
			else
			{
				m_vecLayerCollision.reserve( m_vecLayerCollision.capacity() + nCollisionSize - 1 );

				m_vecLayerCollision[0]->Load( SFile );

				for (int i = 0; i < nCollisionSize-1; ++i)
				{
					boost::shared_ptr<LayerCollision> spLayerCollision = boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE));
					spLayerCollision->Load( SFile );
					m_vecLayerCollision.push_back( spLayerCollision );
				}
			}
		}
	}

	return TRUE;
}

// 파일 구조가 변경시에는 GLAGLandMan의 LoadFile() 함수의 파일 읽기 구조 또한 검토 하여야 함.
// GLLevelFileClient 와 동시에 진행되야 합니다.
BOOL GLLevelFileServer::LoadFile(const std::string& FileName, LPDIRECT3DDEVICEQ pd3dDevice, MapAxisInfoMan* pAxisMan, const bool bImport, BOOL bExceptBaseEffect)
{
	if (FileName.empty())
        return FALSE;

    //std::string PathName(GLOGIC::GetPath());
	//PathName.append(FileName);

	std::string FullPath = sc::getAppPath();
	FullPath.append(SubPath::GLOGIC_LEVEL_FILE);
	FullPath.append(FileName);

	std::auto_ptr<sc::BaseStream> pBStream(
		GLOGIC::openfile_basestream(
		FALSE, // GLOGIC::UseLogicZipFile(), 
		"", // GLogicData::GetInstance().GetLevelZipFile(),
		FullPath.c_str(), //PathName.c_str(),
		FileName,
		false,
		FALSE )); //GLOGIC::UseLogicPackFile()));

	if (!pBStream.get())
		return FALSE;

	sc::BaseStream& SFile = *pBStream;
	
	if ( bImport )
	{
		// Import 시는 m_vecLayerEffect 초기화를 하지 않는다.

		// 최소 하나도 없으면 안되므로 보완작업.
		if ( m_vecLayerEffect.empty() )
		{
			m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
		}
	}
	else
	{
		m_strFileName = FileName;
		bExceptBaseEffect = FALSE;

		// 사이즈 한개로 초기화
		m_vecLayerEffect.clear();
		m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
	}

	// 사이즈 한개로 초기화
	m_vecLayerCollision.clear();
	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );

	char szFileType[FILETYPESIZE];
	DWORD dwFileVER;
	SFile.GetFileType( szFileType, FILETYPESIZE, dwFileVER );
	SFile.SetEncode ( dwFileVER>=VERSION_BEFORE_ENCODE );
	
	m_dwFileVer = dwFileVER;

    // CAUTION : GLLevelFileClient 와 동시에 진행되야 합니다.
	BOOL bLOAD(FALSE);
	switch ( dwFileVER )
	{
	case 0x000:		bLOAD = LOAD_000 ( SFile );									break;
	case 0x101:		bLOAD = LOAD_101 ( SFile );									break;
	case 0x102:		bLOAD = LOAD_102 ( SFile, pd3dDevice, bExceptBaseEffect );	break;
	case 0x103:		bLOAD = LOAD_103 ( SFile, pd3dDevice, bExceptBaseEffect );	break;
	case 0x104:		bLOAD = LOAD_104 ( SFile, pd3dDevice, bExceptBaseEffect );	break;
	case 0x105:		bLOAD = LOAD_105 ( SFile, pd3dDevice, bExceptBaseEffect );	break;
	case 0x106:		bLOAD = LOAD_106 ( SFile, pd3dDevice, bExceptBaseEffect );	break;
	case 0x107:		bLOAD = LOAD_107 ( SFile, pd3dDevice, bExceptBaseEffect );	break;
	case 0x108:		bLOAD = LOAD_108 ( SFile, pd3dDevice, bExceptBaseEffect );	break;
	case VERSION:	bLOAD = LOAD_109 ( SFile, pd3dDevice, bExceptBaseEffect );	break;
	default:
		MessageBox(NULL, "LevelFile unknown data version.", FileName.c_str(), MB_OK);
		return FALSE;
	};		

	if (!bLOAD)
	{
		MessageBox(NULL, "unknown data version.", FileName.c_str(), MB_OK);
		return FALSE;
	}

	SetDeActiveAllLayer();
	SetActiveLayer( 0 );

	SetMobSchMan(0);

	// 현제 래벨에 맞는 미니멥 설정 로드.
    bool bExist = false;
    if (pAxisMan)
    {
        std::tr1::shared_ptr<GLMapAxisInfo> spAxisInfo = pAxisMan->GetInfo(FileName);
        if (spAxisInfo)
        {
            m_sLevelAxisInfo = *spAxisInfo.get();
            bExist = true;
        }
    }

    if (!bExist)
	    m_sLevelAxisInfo.LoadFile(FileName.c_str());

	return TRUE;
}

BOOL GLLevelFileServer::SaveCsvFile( std::fstream &SFile, bool bAllSave )
{
    // Save Csv Head
    SFile << "Level File Name" << ",";
    SFile << "Quad Count" << ",";
    SLEVEL_REQUIRE::SaveCsvHead( SFile );
    SFile << std::endl;

    // Save Csv Basic info
    char szTempName[256];
    if( !bAllSave )
        strncpy( szTempName, m_strFileName.c_str(), m_strFileName.size()-4 );
    else 
        strcpy( szTempName, m_strFileName.c_str() );
    SFile << szTempName << ",";
    SFile << (INT)m_sLevelHead.m_eDivision << ",";
    m_sLevelRequire.SaveCsv( SFile );
    SFile << std::endl;

    // Save Csv Gate Head
    DxLandGateMan::SaveCsvHead( SFile );
    SFile << std::endl;

    // Save Csv Gate Data
    m_pLandGateMan->SaveCsv( SFile );
    //	SFile << std::endl;

    return TRUE;
}

BOOL GLLevelFileServer::LoadCsvFile( const char *szFile )
{
    sc::CStringFile StrFile( 10240 );
    if( !StrFile.Open ( szFile ) )
        return FALSE;

    STRUTIL::ClearSeparator();
    STRUTIL::RegisterSeparator( "," );

    CString strLine;
    CStringArray StrArray;
    int iCsvCur = 0;

    // Skip Csv Basic Head
    StrFile.GetNextLine( strLine );

    // Load Csv Basic Info
    StrFile.GetNextLine( strLine );
    STRUTIL::StringSeparate( strLine, StrArray );

    STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], m_strFileName );
    m_sLevelHead.m_eDivision = (EMDIVISION)atoi( StrArray[ iCsvCur++ ] );
    m_sLevelRequire.LoadCsv( StrArray );

    // Skip Csv Gate Head
    StrFile.GetNextLine( strLine );

    // Load Csv Gate Data
    clearGateMan();
    while( StrFile.GetNextLine( strLine ) )
    {
        STRUTIL::StringSeparate( strLine, StrArray );
        m_pLandGateMan->LoadCsv( StrArray );
    }
    m_pLandGateMan->BuildTree();

    return TRUE;
}

BOOL GLLevelFileServer::LoadAllCsvFile(LPDIRECT3DDEVICEQ pd3dDevice, MapAxisInfoMan* pAxisMan)
{
    char szPathName[MAX_PATH] = {0};
    StringCchCopy(szPathName,MAX_PATH,GLOGIC::GetPath ());
    StringCchCat(szPathName,MAX_PATH,"level.csv");

    sc::CStringFile StrFile( 10240 );
    if( !StrFile.Open ( szPathName ) )	return FALSE;

    STRUTIL::ClearSeparator();
    STRUTIL::RegisterSeparator( "," );

    CString strLine;
    CStringArray StrArray;
    std::string strTemp;

    int iCsvCur;

    // First Skip Csv Basic Head
    StrFile.GetNextLine( strLine );

    // Load Csv Basic Head
    while( StrFile.GetNextLine( strLine ) )
    {

        InvalidateDeviceObjects();
        DeleteDeviceObjects();

        clearGateMan();
        m_MobSchMan->Reset ();
        m_MobSchMan->CleanUp ();

        iCsvCur = 0;

        STRUTIL::StringSeparate( strLine, StrArray );
        STRUTIL::InputStrCsv( StrArray[iCsvCur++], strTemp );

        if( !LoadFile(strTemp.c_str(), pd3dDevice, pAxisMan))
        {
            strTemp			    += " Load Failed!";
            MessageBox( NULL, strTemp.c_str(), "error", MB_OK );
            continue;
        }

        m_strFileName = strTemp;
        m_sLevelHead.m_eDivision = (EMDIVISION)atoi( StrArray[ iCsvCur++ ] );
        m_sLevelRequire.LoadCsv( StrArray );

        // Skip Csv Gate Head
        StrFile.GetNextLine( strLine );

        // Load Csv Gate Data
		clearGateMan();
        while( StrFile.GetNextLine( strLine ) )
        {
            STRUTIL::StringSeparate( strLine, StrArray );
            STRUTIL::InputStrCsv( StrArray[0], strTemp );	
            if( strTemp == "Level File Name" ) break;

            m_pLandGateMan->LoadCsv( StrArray );
        }

        m_pLandGateMan->BuildTree();

        SaveFile( m_strFileName.c_str() );
    }

    return TRUE;
}

void GLLevelFileServer::AddMobSchMan(UINT index)
{	
	SpMobScheduleManServer spNewMobSchMan(new GLMobScheduleManServer);
	DxLandGateMan* pNewLandGateMan(new DxLandGateMan);
	boost::shared_ptr<LayerEffect> spLayerEffect(new LayerEffect(FALSE) );
	

	if (spNewMobSchMan)
	{
		spNewMobSchMan->SetLandMan(m_MobSchMan->GetLandMan());
		

		if ( index >= m_MobSchManContainer.size() )
		{
			m_MobSchManContainer.push_back(spNewMobSchMan);
			m_LandGateManContainer.push_back(pNewLandGateMan);
			m_vecLayerEffect.push_back(spLayerEffect);
		}
		else
		{
			m_MobSchManContainer.insert(m_MobSchManContainer.begin() + index, spNewMobSchMan);
			m_LandGateManContainer.insert(m_LandGateManContainer.begin() + index, pNewLandGateMan);
			m_vecLayerEffect[index + 1] = spLayerEffect;
		}
		m_MobSchMan = spNewMobSchMan;
		m_pLandGateMan = pNewLandGateMan;
	}
}

void GLLevelFileServer::DelMobSchMan(UINT index)
{
	if ( index >= m_MobSchManContainer.size() )
		return;

	m_MobSchManContainer.erase(m_MobSchManContainer.begin() + index);
	m_LandGateManContainer.erase(m_LandGateManContainer.begin() + index);
	m_vecLayerEffect.erase(m_vecLayerEffect.begin() + index + 1);
}

void GLLevelFileServer::SetMobSchMan(UINT index)
{
	if ( index >= m_MobSchManContainer.size() )
		return;

	m_MobSchMan = m_MobSchManContainer[index];	
	m_pLandGateMan = m_LandGateManContainer[index];
}

UINT GLLevelFileServer::GetSizeMobSchMan()
{

    return m_MobSchManContainer.size();
}

void GLLevelFileServer::ResetMobSchMan(GLLandMan* pLandMan)
{

	MobSchManContainerIterator it = m_MobSchManContainer.begin();

	for ( ; it != m_MobSchManContainer.end(); ++it)
	{
		SpMobScheduleManServer pMobSchMan = *it;
		pMobSchMan->SetLandMan(pLandMan);
	}
}

void GLLevelFileServer::ClearMobSchMan()
{
	GLLevelFile::clearGateMan();

	if (m_MobSchMan)
	{
		m_MobSchMan->CleanUp();
	}


	m_MobSchManContainer.clear();
}

void GLLevelFileServer::operator=(const GLLevelFileServer &rhs)
{
	if (this == &rhs)
		return;

	for ( MobSchManContainerConstIterator it(rhs.m_MobSchManContainer.begin()); it != rhs.m_MobSchManContainer.end(); ++it )
	{
		SpMobScheduleManServer spNewMobSchMan(new GLMobScheduleManServer);

		if (spNewMobSchMan)
		{
			*spNewMobSchMan = *(*it);


			m_MobSchManContainer.push_back(spNewMobSchMan);

			if (rhs.m_MobSchMan == *it)
			{
				m_MobSchMan = spNewMobSchMan;
			}
		}
	}

	GLLevelFile::operator=(rhs);
}

std::tr1::shared_ptr<GLMobScheduleManServer> GLLevelFileServer::GetMobSchMan( UINT nLayer )
{

	if ( nLayer >= m_MobSchManContainer.size() )
		return std::tr1::shared_ptr<GLMobScheduleManServer>();

	return m_MobSchManContainer[nLayer];
}

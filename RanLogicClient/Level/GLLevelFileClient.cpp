#include "pch.h"
#include "../../SigmaCore/Util/MinMessageBox.h"

#include "../../EngineLib/NaviMesh/Collision/LayerCollision.h"
#include "../../EngineLib/DxLand/DxLandEff.h"
#include "../../EngineLib/G-Logic/GLOGIC.h"
#include "../../EngineLib/Common/SUBPATH.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Land/GLLandMark.h"
#include "../../RanLogic/Land/GLMapAxisInfo.h"
#include "../../RanLogic/Quest/GLQuest.h"

#include "../GLGaeaClient.h"
#include "GLLevelFileClient.h"

#include "../../SigmaCore/DebugInclude.h"

GLLevelFileClient::GLLevelFileClient(GLGaeaClient* pGaeaClient)
    : m_pGaeaClient(pGaeaClient)
{
}

GLLevelFileClient::~GLLevelFileClient(void)
{
}

BOOL GLLevelFileClient::LOAD_000 ( sc::BaseStream &SFile )
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
		SpMobScheduleMan pMobSchedule(new GLMobScheduleMan);
		if ( pMobSchedule->LoadFile(SFile) == FALSE )
			return FALSE;
		m_MobSchManContainer.push_back(pMobSchedule);
	}	

	// 사이즈 한개로 초기화
	m_vecLayerEffect.clear();
	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(FALSE)) );

	m_vecLayerCollision.clear();
	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );
	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(FALSE)) );

	return TRUE;
}

BOOL GLLevelFileClient::LOAD_101 ( sc::BaseStream &SFile )
{
	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	{
		SpMobScheduleMan pMobSchedule(new GLMobScheduleMan);
		if ( pMobSchedule->LoadFile(SFile) == FALSE )
			return FALSE;
		m_MobSchManContainer.push_back(pMobSchedule);
	}

	// 사이즈 한개로 초기화
	m_vecLayerEffect.clear();
	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(FALSE)) );

	m_vecLayerCollision.clear();
	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );
	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(FALSE)) );

	return TRUE;
}

BOOL GLLevelFileClient::LOAD_102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	// 사이즈 한개로 초기화
	m_vecLayerEffect.clear();
	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(FALSE)) );
	m_vecLayerEffect[0]->LoadOLD( SFile, FALSE );

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
		SpMobScheduleMan pMobSchedule(new GLMobScheduleMan);
		if ( pMobSchedule->LoadFile(SFile) == FALSE )
			return FALSE;
		m_MobSchManContainer.push_back(pMobSchedule);
	}

	m_vecLayerCollision.clear();
	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );
	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(FALSE)) );

	return TRUE;
}

BOOL GLLevelFileClient::LOAD_103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBLOCKSIZE(0);

	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	// 사이즈 한개로 초기화
	m_vecLayerEffect.clear();
	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(FALSE)) );
	m_vecLayerEffect[0]->LoadOLD_2( SFile, pd3dDevice ? FALSE : TRUE );

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
		SpMobScheduleMan pMobSchedule(new GLMobScheduleMan());
		if ( pMobSchedule->LoadFile(SFile) == FALSE )
			return FALSE;
		m_MobSchManContainer.push_back(pMobSchedule);
	}

	m_vecLayerCollision.clear();
	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );
	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(FALSE)) );

	return TRUE;
}

BOOL GLLevelFileClient::LOAD_104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBLOCKSIZE(0);

	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );
	m_sLevelEtcFunc.LOAD ( SFile );

	// 사이즈 한개로 초기화
	m_vecLayerEffect.clear();
	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(FALSE)) );
	m_vecLayerEffect[0]->LoadOLD_2( SFile, pd3dDevice ? FALSE : TRUE );

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
		SpMobScheduleMan pMobSchedule(new GLMobScheduleMan());
		if ( pMobSchedule->LoadFile(SFile) == FALSE )
			return FALSE;
		m_MobSchManContainer.push_back(pMobSchedule);
	}

	m_vecLayerCollision.clear();
	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );
	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(FALSE)) );

	return TRUE;
}

BOOL GLLevelFileClient::LOAD_105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBLOCKSIZE(0);

	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	dwBLOCKSIZE = SFile.ReadBlockSize();
	{
		SpMobScheduleMan pMobSchedule(new GLMobScheduleMan());
		if ( pMobSchedule->LoadFile(SFile) == FALSE )
			return FALSE;
		m_MobSchManContainer.push_back(pMobSchedule);
	}

	dwBLOCKSIZE = SFile.ReadBlockSize();
	{
		DxLandGateMan* pLandGateMan(new DxLandGateMan);
		if ( FAILED(pLandGateMan->Load(SFile)) )
			return FALSE;
		m_LandGateManContainer.push_back(pLandGateMan);
	}

	// 사이즈 한개로 초기화
	m_vecLayerEffect.clear();
	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
	m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(FALSE)) );
	m_vecLayerEffect[0]->LoadOLD_2( SFile, pd3dDevice ? FALSE : TRUE );

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

	m_vecLayerCollision.clear();
	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );
	m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(FALSE)) );

	return TRUE;
}

BOOL GLLevelFileClient::LOAD_106( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBLOCKSIZE(0);

	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	dwBLOCKSIZE = SFile.ReadBlockSize();

	int count = 0;
	SFile >> count;

	for (int i = 0; i < count; ++i)
	{
		SpMobScheduleMan spMobSchMan(new GLMobScheduleMan);

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
		m_vecLayerEffect.clear();

		if ( count <= 0 )
		{
			// m_vecLayerEffect 무조건 1개는 가지고 있어야 한다.
			m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
			m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(FALSE)) );
			SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
		}
		else
		{
			m_vecLayerEffect.reserve( count+1 );

			m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
			m_vecLayerEffect[0]->LoadOLD_2( SFile, pd3dDevice ? FALSE : TRUE );

			for (int i = 0; i < count; ++i)
			{
				m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(FALSE)) );
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
		m_vecLayerCollision.clear();

		if ( count <= 0 )
		{
			// m_vecLayerEffect 무조건 1개는 가지고 있어야 한다.
			m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );
			m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(FALSE)) );
		}
		else
		{
			m_vecLayerCollision.reserve( count+1 );
			m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );

			for (int i = 0; i < count; ++i)
			{
				m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(FALSE)) );
			}
		}
	}

	return TRUE;
}

BOOL GLLevelFileClient::LOAD_107 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBLOCKSIZE(0);

	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	dwBLOCKSIZE = SFile.ReadBlockSize();

	int count = 0;
	SFile >> count;

	for (int i = 0; i < count; ++i)
	{
		SpMobScheduleMan spMobSchMan(new GLMobScheduleMan);

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
		m_vecLayerEffect.clear();

		if ( count <= 0 )
		{
			// m_vecLayerEffect 무조건 1개는 가지고 있어야 한다.
			m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
			m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(FALSE)) );
			SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
		}
		else
		{
			m_vecLayerEffect.reserve( count+1 );

			m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
			m_vecLayerEffect[0]->LoadOLD_2( SFile, pd3dDevice ? FALSE : TRUE );

			for (int i = 0; i < count; ++i)
			{
				m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(FALSE)) );
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
		m_vecLayerCollision.clear();

		if ( count <= 0 )
		{
			// m_vecLayerEffect 무조건 1개는 가지고 있어야 한다.
			m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );
			m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(FALSE)) );
		}
		else
		{
			m_vecLayerCollision.reserve( count+1 );
			m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );

			for (int i = 0; i < count; ++i)
			{
				m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(FALSE)) );
			}
		}
	}

	return TRUE;
}

BOOL GLLevelFileClient::LOAD_108 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBLOCKSIZE(0);

	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	dwBLOCKSIZE = SFile.ReadBlockSize();

	int count = 0;
	SFile >> count;

	for (int i = 0; i < count; ++i)
	{
		SpMobScheduleMan spMobSchMan(new GLMobScheduleMan);

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
		m_vecLayerEffect.clear();

		int nEffectSize = 0;
		SFile >> nEffectSize;

		dwBLOCKSIZE = SFile.ReadBlockSize();
		{
			if ( nEffectSize <= 0 )
			{
				// m_vecLayerEffect 무조건 1개는 가지고 있어야 한다.
				m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
				m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(FALSE)) );
				SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
			}
			else
			{
				m_vecLayerEffect.reserve( nEffectSize );

				for (int i = 0; i < nEffectSize; ++i)
				{
					m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect( (i == 0) ? TRUE : FALSE )) );
					m_vecLayerEffect[i]->Load( SFile, pd3dDevice ? FALSE : TRUE );
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
		m_vecLayerCollision.clear();

		if ( count <= 0 )
		{
			// m_vecLayerEffect 무조건 1개는 가지고 있어야 한다.
			m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );
			m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(FALSE)) );
		}
		else
		{
			m_vecLayerCollision.reserve( count+1 );
			m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );

			for (int i = 0; i < count; ++i)
			{
				m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(FALSE)) );
			}
		}
	}

	return TRUE;
}

BOOL GLLevelFileClient::LOAD_109 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBLOCKSIZE(0);

	m_sLevelHead.LOAD ( SFile );
	m_sLevelRequire.LOAD ( SFile );

	dwBLOCKSIZE = SFile.ReadBlockSize();

	int count = 0;
	SFile >> count;

	for (int i = 0; i < count; ++i)
	{
		SpMobScheduleMan spMobSchMan(new GLMobScheduleMan);

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
		m_vecLayerEffect.clear();

		int nEffectSize = 0;
		SFile >> nEffectSize;

		dwBLOCKSIZE = SFile.ReadBlockSize();
		{
			if ( nEffectSize <= 0 )
			{
				// m_vecLayerEffect 무조건 1개는 가지고 있어야 한다.
				m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(TRUE)) );
				m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect(FALSE)) );
				SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
			}
			else
			{
				m_vecLayerEffect.reserve( nEffectSize );

				for (int i = 0; i < nEffectSize; ++i)
				{
					m_vecLayerEffect.push_back( boost::shared_ptr<LayerEffect>(new LayerEffect( (i == 0) ? TRUE : FALSE )) );
					m_vecLayerEffect[i]->Load( SFile, pd3dDevice ? FALSE : TRUE );
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
		m_vecLayerCollision.clear();

		int nCollisionSize = 0;
		SFile >> nCollisionSize;

		dwBLOCKSIZE = SFile.ReadBlockSize();
		{
			if ( nCollisionSize <= 0 )
			{
				m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(TRUE)) );
				m_vecLayerCollision.push_back( boost::shared_ptr<LayerCollision>(new LayerCollision(FALSE)) );
				SFile.SetOffSet ( SFile.GetfTell() + dwBLOCKSIZE );
			}
			else
			{
				m_vecLayerCollision.reserve( nCollisionSize );

				for (int i = 0; i < nCollisionSize; ++i)
				{
					boost::shared_ptr<LayerCollision> spLayerCollision = boost::shared_ptr<LayerCollision>(new LayerCollision( (i == 0) ? TRUE : FALSE ));
					spLayerCollision->Load( SFile );
					m_vecLayerCollision.push_back( spLayerCollision );
				}
			}
		}
	}

	return TRUE;
}

// 파일 구조가 변경시에는 GLAGLandMan의 LoadFile() 함수의 파일 읽기 구조 또한 검토 하여야 함.
// GLLevelFileServer 와 동시에 진행되야 합니다.
BOOL GLLevelFileClient::LoadFile(const std::string& FileName, LPDIRECT3DDEVICEQ pd3dDevice)
{
	//
	if (FileName.empty())
    {
        std::string ErrorMsg("GLLevelFileClient::LoadFile FileName is empty.");
        sc::ErrorMessageBox(ErrorMsg, false);
        return FALSE;
    }

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
    {
        std::string ErrorMsg(
            sc::string::format(
                "GLLevelFileClient::LoadFile not exist file. %1%",
                FileName));
        sc::ErrorMessageBox(ErrorMsg, false);
		return FALSE;
    }

	sc::BaseStream& SFile = *pBStream;
	
	m_strFileName = FileName;

	char szFileType[FILETYPESIZE];
	DWORD dwFileVER;
	SFile.GetFileType( szFileType, FILETYPESIZE, dwFileVER );
	SFile.SetEncode ( dwFileVER>=VERSION_BEFORE_ENCODE );
	
	m_dwFileVer = dwFileVER;

    // CAUTION : GLLevelFileServer 와 동시에 진행되야 합니다.
	BOOL bLOAD(FALSE);
	switch ( dwFileVER )
	{
	case 0x000:
		bLOAD = LOAD_000 ( SFile );
		break;
	case 0x101:
		bLOAD = LOAD_101 ( SFile );
		break;
	case 0x102:
		bLOAD = LOAD_102 ( SFile, pd3dDevice );	
		break;
	case 0x103:
		bLOAD = LOAD_103 ( SFile, pd3dDevice );	
		break;
	case 0x104:
		bLOAD = LOAD_104 ( SFile, pd3dDevice );
		break;
	case 0x105:
		bLOAD = LOAD_105 ( SFile, pd3dDevice );
		break;
	case 0x106:
		bLOAD = LOAD_106 ( SFile, pd3dDevice );	
		break;
	case 0x107:
		bLOAD = LOAD_107 ( SFile, pd3dDevice );
		break;
	case 0x108:
		bLOAD = LOAD_108 ( SFile, pd3dDevice );
		break;
	case VERSION:
		bLOAD = LOAD_109 ( SFile, pd3dDevice );
		break;
	default:
		MessageBox(NULL, "LevelFile unknown data version.", FileName.c_str(), MB_OK);
		return FALSE;
	};

	if ( !bLOAD )
	{
		MessageBox(NULL, "unknown data version.", FileName.c_str(), MB_OK);
		return FALSE;
	}

	SetMobSchMan(0);

	// 현재 레벨에 맞는 미니멥 설정 로드.
    bool bExist = false;
    if (m_pGaeaClient)
    {
        std::tr1::shared_ptr<GLMapAxisInfo> spAxisInfo = m_pGaeaClient->GetAxisInfo(FileName);
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

void GLLevelFileClient::SetMobSchMan(UINT index)
{
	if ( index >= m_MobSchManContainer.size() )
		return;

	m_MobSchMan = m_MobSchManContainer[index];			// 클라이언트에서 미니맵에서 몹 위치나 NPC 정보에 필요할듯 함.
	m_pLandGateMan = m_LandGateManContainer[index];		// 클라이언트에서 gate 위치가 필요함.
}
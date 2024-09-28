#include "../pch.h"

#include "../../EngineLib/NaviMesh/navigationmesh.h"

#include "./GLLandMan.h"
#include "../../RanLogic/Item/GLItem.h"
#include "../FieldServer/GLGaeaServer.h"

#include "../../SigmaCore/DebugInclude.h"


BOOL GLLandMan::LoadWldFile ( const char* szWldFile, bool bLandGateLoad )
{
	//HRESULT hr;

	m_sLevelHead.m_strWldFile = szWldFile;

	char szPathName[MAX_PATH] = {0};
	StringCchCopy ( szPathName, MAX_PATH, DxLandMan::GetPath() );
	StringCchCat ( szPathName, MAX_PATH, m_sLevelHead.m_strWldFile.c_str() );

	char szFileType[FILETYPESIZE];
	DWORD dwVersion;

	BOOL bExist;
	sc::SerialFile SFile;
	if (SFile.OpenFile(FOT_READ, szPathName))
	{
		//	Note : ���� ���� ����Ȯ��.
		//
		SFile.GetFileType( szFileType, FILETYPESIZE, dwVersion );

		if ( strcmp(DxLandMan::FILEMARK,szFileType) )
		{
			CString Str;
			Str.Format ( "[%s]  Wld file Head Error.", m_sLevelHead.m_strWldFile.c_str() );
			MessageBox ( NULL, Str.GetString(), "ERROR", MB_OK );

			return FALSE;
		}

		if ( dwVersion > DxLandMan::VERSION )
		{
			CString Str;
			Str.Format ( "[%s] Wld file unknown version.", m_sLevelHead.m_strWldFile.c_str() );
			MessageBox ( NULL, Str.GetString(), "ERROR", MB_OK );

			return FALSE;
		}

		SLAND_FILEMARK sLandMark;

		//	Note : .wld ������ ��ID�� ������� ����.
		SNATIVEID sMapID;
		SFile >> sMapID.dwID;

		//	Note : .wld ������ ���̸��� ������� ����.
		char szMapName[MAXLANDNAME];
		SFile.ReadBuffer ( szMapName, sizeof(char)*MAXLANDNAME );

		sLandMark.LoadSet ( SFile );

		//	Note : �����̼� �޽� �б�.
		//
		SFile.SetOffSet ( sLandMark.dwNAVI_MARK );
		SFile >> bExist;
		if ( bExist )
		{
			m_spNaviMeshInWld = boost::shared_ptr<NavigationMesh>(new NavigationMesh);
			m_spNaviMeshInWld->LoadFile ( SFile );

			// �⺻���� �����ϵ��� �Ѵ�.
			m_spNaviMesh = boost::shared_ptr<NavigationMesh>( new NavigationMesh(*m_spNaviMeshInWld.get()) );
		}

		//	Note : ���Ա� ���� �б�.
		//
		if ( sLandMark.dwGATE_MARK && bLandGateLoad )
		{
			SFile.SetOffSet ( sLandMark.dwGATE_MARK );



			//////////////////////////////////////////////////////////////////////////
			// �����ڵ�. ���� Ȯ�� �ʿ�.
			if ( !m_pLandGateMan )
				m_pLandGateMan = new DxLandGateMan;



			m_pLandGateMan->Load ( SFile );
		}

		//	Note : ���� ���� �б�.
		//
		if ( sLandMark.dwWEATHER_MARK )
		{
			SFile.SetOffSet ( sLandMark.dwWEATHER_MARK );

			DxWeatherMan::LoadSetProp ( SFile, m_sWeatherProp );
		}
	}

	return TRUE;
}

BOOL GLLandMan::LoadNaviBypassFile( const char* szNaviFile )
{
	m_sLevelHead.m_strNaviBypassFile = szNaviFile;

	m_spNaviMeshBypass = boost::shared_ptr<NavigationMesh>(new NavigationMesh);
	if ( m_spNaviMeshBypass->LoadFile( szNaviFile ) )
	{
		m_spNaviMesh = boost::shared_ptr<NavigationMesh>( new NavigationMesh(*m_spNaviMeshBypass.get()) );

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void GLLandMan::ClearNaviBypassFile()
{
	m_sLevelHead.m_strNaviBypassFile = _T("");
	m_spNaviMeshBypass.reset();

	if ( m_spNaviMeshInWld.get() )
	{
		m_spNaviMesh = boost::shared_ptr<NavigationMesh>( new NavigationMesh(*m_spNaviMeshInWld.get()) );
	}
}

BOOL GLLandMan::ImportLayer(const char* szFile, MapAxisInfoMan* pAxisMan, BOOL bExceptEffect)
{
	const BOOL bOk = GLLevelFileServer::LoadFile(szFile, m_pd3dDevice, pAxisMan, true, bExceptEffect);	
	
	return bOk;
}
//	Note : ���� ������ ����ÿ��� GLAGLandMan�� LoadFile() �Լ��� ���� �б� ���� ���� ���� �Ͽ��� ��.
//
BOOL GLLandMan::LoadFile(const char* szFile, MapAxisInfoMan* pAxisMan)
{
	GASSERT(szFile);
	HRESULT	hr;

	//	Note : Map ������ ����.
	ReSetMap ();

	BOOL bOk(FALSE);
	
	//
	//mjeon
	//Load .lev file.
	//
	bOk = GLLevelFileServer::LoadFile(szFile, m_pd3dDevice, pAxisMan);	
	if (!bOk)
		return FALSE;

	//
	//mjeon
	//Load .wld file.
	//
	bOk = LoadWldFile ( m_sLevelHead.m_strWldFile.c_str(), GLLevelFileServer::GetFileVer()<=VERSION_BEFORE_GATE );
	if ( !bOk )
	{
		CString strTemp = m_sLevelHead.m_strWldFile.c_str();
		strTemp += " : GLLandMan::LoadWldFile() Failure.";
		MessageBox ( NULL, strTemp, "ERROR", MB_OK );
		
		return FALSE;
	}

	//
	//Load .navi file.
	//
	if ( !m_sLevelHead.m_strNaviBypassFile.empty() )
	{
		bOk = LoadNaviBypassFile ( m_sLevelHead.m_strNaviBypassFile.c_str() );
		if ( !bOk )
		{
			CString strTemp = m_sLevelHead.m_strNaviBypassFile.c_str();
			strTemp += " : GLLandMan::LoadNaviBypassFile() Failure.";
			MessageBox ( NULL, strTemp, "ERROR", MB_OK );

			return FALSE;
		}
	}

	//mjeon
	//SetMap() creates QuadTree for the LandMan.
	//
	//	Note : Map ������ �ʱ�ȭ.
	//
	hr = SetMap ();
	if ( FAILED(hr) )
		return FALSE;

	if ( m_pd3dDevice )
	{
		hr = GLLevelFileServer::InitDeviceObjects ( m_pd3dDevice );
		if ( FAILED(hr) )
			return hr;

		hr = GLLevelFileServer::RestoreDeviceObjects ( m_pd3dDevice );
		if ( FAILED(hr) )
			return hr;
	}

	SMAPNODE* pMAP_NODE = m_pGLGaeaServer->FindMapNode ( GetBaseMapID() );
	if ( pMAP_NODE )
	{
		m_bPKZone				= pMAP_NODE->IsPkZone();
		m_bFreePK				= pMAP_NODE->IsFreePk();
		m_bItemDrop			= pMAP_NODE->IsItemDrop();
		m_bPetActivity		= pMAP_NODE->IsPetActivity();
		m_bDECEXP				= pMAP_NODE->IsDecExp();
		m_bVehicleActivity	= pMAP_NODE->IsVehicleActivity();
		m_bClubBattleZone	= pMAP_NODE->IsClubBattleZone();
		m_bLunchBoxForbid	= pMAP_NODE->IsLunchBoxForbid();
		m_bCPReset			= pMAP_NODE->IsCombatPointReset();		
		m_bPKMap				= pMAP_NODE->IsPkMap();
		m_bUIMapInfo			= pMAP_NODE->IsUIMapInfo();
		m_bUIMapSelect		= pMAP_NODE->IsUIMapSelect();
		m_bPartyMap			= pMAP_NODE->IsPartyMap();
		m_bExpeditionMap	= pMAP_NODE->IsExpeditionMap();
        m_bClubPkRecord		= pMAP_NODE->IsClubPkRecord();		
		m_bCullByObjectMap	= pMAP_NODE->IsCullByObjectMap();
		m_bChangeSchool	= pMAP_NODE->IsChangeSchool();
	}

	return TRUE;
}


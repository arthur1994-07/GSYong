// Terrain0.cpp: implementation of the DxEffectTiling class.
//
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../../Common/StlFunctions.h"

#include "../../DxTools/TextureManager.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxLand/DxPiece/DxSingleTexMesh.h"

#include "./DxEffectTiling.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void DxEffectTiling::SEPARATEOBJ::Save( sc::SerialFile &SFile )
{
    SFile.WriteBuffer ( &m_sVertex[0], sizeof(VERTEXNORCOLORTEX) );
    SFile.WriteBuffer ( &m_sVertex[1], sizeof(VERTEXNORCOLORTEX) );
    SFile.WriteBuffer ( &m_sVertex[2], sizeof(VERTEXNORCOLORTEX) );
}

void DxEffectTiling::SEPARATEOBJ::Load( sc::SerialFile &SFile )
{
    SFile.ReadBuffer ( &m_sVertex[0], sizeof(VERTEXNORCOLORTEX) );
    SFile.ReadBuffer ( &m_sVertex[1], sizeof(VERTEXNORCOLORTEX) );
    SFile.ReadBuffer ( &m_sVertex[2], sizeof(VERTEXNORCOLORTEX) );
}

void DxEffectTiling::SaveBuffer ( sc::SerialFile &SFile )
{
	SFile.BeginBlock();
	{
		char strTexture[255];

		{
			DWORD	dwMaterialsNUM;
			dwMaterialsNUM = (DWORD)m_mapMaterials.size();
			SFile << dwMaterialsNUM;											//	SAVE

			DWORD				dwFirst;
			MATERIALMAP_ITER	iter		= m_mapMaterials.begin();
			MATERIALMAP_ITER	iter_end	= m_mapMaterials.end();
			for ( ; iter!=iter_end; ++iter )
			{
				dwFirst		= (*iter).first;
				StringCchCopy( strTexture, 255, (*iter).second->strTexture.c_str() );
				SFile << dwFirst;											//	SAVE
				SFile.WriteBuffer ( strTexture, sizeof(char)*255 );			//	SAVE

				// Octree ������ ������� �ʴ´�.
				SFile.BeginBlock();
				{
					// PointEX
					{
						POINTEX sPointEX;
						DWORD	dwCountEX = 0;
						for ( DWORD i=0; i<m_dwPointEX; ++i )
						{
							if ( (*iter).second->m_pPointUseColor[i].bUse )	
								++dwCountEX;
						}

						SFile << dwCountEX;													//	SAVE
						for ( DWORD i=0; i<m_dwPointEX; ++i )
						{
							if ( (*iter).second->m_pPointUseColor[i].bUse )
							{
								SFile << m_pPointEX[i].vPos;						// �� ��ġ
								SFile << (*iter).second->m_pPointUseColor[i].dwColor;	// �÷���
							}
						}
					}

					// listTriangle Save
					DWORD dwSize = static_cast<DWORD>( (*iter).second->listTriangle.size() );
					SFile << dwSize;											//	SAVE
					TRIANGLELIST_ITER iterTrianle = (*iter).second->listTriangle.begin();
					for ( ; iterTrianle != (*iter).second->listTriangle.end(); ++iterTrianle )
					{
						SFile << (*iterTrianle);
					}
				}
				SFile.EndBlock();
			}
		}

		// FrameMesh ������ ������� �ʴ´�.
		SFile.BeginBlock();
		{
			MATERIALMAP_ITER iter = m_mapMaterials.begin();
			for ( ; iter!=m_mapMaterials.end(); ++iter )
			{
				SFile << (*iter).first;

				BOOL bUse = (*iter).second->m_pMeshList ? TRUE : FALSE;
				SFile << bUse;
				if ( bUse )
				{
					(*iter).second->m_pMeshList->Save( SFile, FALSE );
				}
			}
		}
		SFile.EndBlock();
	}
	SFile.EndBlock();
}

void DxEffectTiling::LoadBufferSet ( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	// ���⼭ �ε� �ϴ� ���� ���� �ؾ� �Ѵ�.
	// �ֳĸ� X ������ ������ ���� �ֱ� ������ �̰����� ���� �����ؼ��� �ȵȴ�.
	// < X ���Ͽ� �����ִ� ȿ��, X ������ ���ϸ� �ڽŵ� ���ؾ� �ϱ� ������ >

	//DWORD	dwBuffSize_Base;
	DWORD	dwBuffSize_Blend;
	BOOL	bExt;
	DWORD	dwMaterialsNUM;

	DWORD	dwFirst;
	char	strTexture[255];
	MATERIALMAP_ITER iter;

	//std::for_each ( m_mapMaterials.begin(), m_mapMaterials.end(), std_afunc::DeleteMapObject() );
	m_mapMaterials.clear();

	DWORD dwUsePoint_Temp(0L);
	POINTEX_PREV sPointTexPrev;
	std::vector<POINTEX_PREV> vecPOINTTEX_PREV;

	DWORD dwBuffSize(0L);
	SFile >> dwBuffSize; //	���� �ʰ� ��ŵ������ ���ǹǷ� ���� �ε忡���� ���ǹ�.

	if ( VERSION==dwVer )
	{
		SFile >> dwMaterialsNUM;
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwFirst;
			SFile.ReadBuffer ( strTexture, sizeof(char)*255 );

			SFile >> dwBuffSize_Blend;

			std::tr1::shared_ptr<MATERIALEX> spMaterials( new MATERIALEX(m_dwPointEX) );

			// PointEX
			{
				SFile >> dwUsePoint_Temp;
				for ( DWORD i=0; i<dwUsePoint_Temp; ++i )
				{
					SFile >> sPointTexPrev.vPos;	// �� ��ġ
					SFile >> sPointTexPrev.dwColor;	// �÷���

					sPointTexPrev.dwMaterial = dwFirst;
					vecPOINTTEX_PREV.push_back( sPointTexPrev );
				}
			}

			// listTriangle Load
			{
				DWORD dwSize(0L);
				DWORD dwTriangle(0L);
				SFile >> dwSize;											//	LOAD
				for ( DWORD i=0; i<dwSize; ++i )
				{
					SFile >> dwTriangle;

					if ( dwTriangle >= m_dwFaces )
					{
						// �� ������ �Ѿ�� ������ �ִ� ��
						// ���� x ���� �����Ϳ� ���� x ���� ����Ÿ�� �ٸ���.
						TSTRING strName;
						strName += GetName();
						strName += _T(" - ");
						strName += m_szAdaptFrame;
						strName += _T(" - ���� x���� Frame�� ���� x���� Frame�� �������� �ʽ��ϴ�. ������ ���� �� �ֽ��ϴ�.");
						AfxMessageBox( strName.c_str() );
						continue;
					}

					spMaterials->listTriangle.push_back( dwTriangle );
				}
			}

			spMaterials->strTexture = strTexture;

            spMaterials->m_textureRes = TextureManager::GetInstance().LoadTexture(
                strTexture,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_EFFECT,
				false,
				FALSE );

            if( spMaterials->m_textureRes.GetTexture() )
			    spMaterials->m_bAlpha = (BOOL)spMaterials->m_textureRes.GetAlphaType();	// ���� ������ �Ⱦ����� üũ

			m_mapMaterials.insert ( std::make_pair(dwFirst,spMaterials) );		// ����
		}

		SFile >> dwBuffSize_Blend;
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize_Blend );

		// vecPOINTTEX_PREV �����͸� m_pPointEX_Load ���� �ű��.
		SAFE_DELETE_ARRAY( m_pPointEX_Load );
		m_dwPointEX_Load = static_cast<DWORD>( vecPOINTTEX_PREV.size() );
		m_pPointEX_Load = new POINTEX_PREV[m_dwPointEX_Load];
		for ( DWORD i=0; i<m_dwPointEX_Load; ++i )
		{
			m_pPointEX_Load[i] = vecPOINTTEX_PREV[i];
		}

		// 
		ClonePointEX( pd3dDevice, FALSE );

		return;
	}
	else if ( 0x00000105==dwVer )
	{
		//DWORD			dwPointEX_Load(0L);
		//POINTEX*		pPointEX_Load(NULL);

		SFile >> m_dwPointEX_Load;
		SAFE_DELETE_ARRAY ( m_pPointEX_Load );

		if ( m_dwPointEX_Load )
		{
			m_pPointEX_Load = new POINTEX_PREV[m_dwPointEX_Load];
			for ( DWORD i=0; i<m_dwPointEX_Load; ++i )
			{
				SFile.ReadBuffer ( &m_pPointEX_Load[i], sizeof(POINTEX_PREV) );
			}
		}

		SFile >> dwBuffSize_Blend; //	���� �ʰ� ��ŵ������ ���ǹǷ� ���� �ε忡���� ���ǹ�.

		SFile >> dwMaterialsNUM;
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwFirst;
			SFile.ReadBuffer ( strTexture, sizeof(char)*255 );

			//if ( pPointEX_Load )
			//{
			//}

			std::tr1::shared_ptr<MATERIALEX> spMaterials( new MATERIALEX(m_dwPointEX) );

			spMaterials->strTexture = strTexture;

			spMaterials->m_textureRes = TextureManager::GetInstance().LoadTexture(
				strTexture,
				false,
				TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
				0,
				D3DFMT_UNKNOWN,
				TEXTURE_RESIZE_NONE,
				TEXTURE_EFFECT,
				false,
				FALSE );

			if( spMaterials->m_textureRes.GetTexture() )
				spMaterials->m_bAlpha = (BOOL)spMaterials->m_textureRes.GetAlphaType();	// ���� ������ �Ⱦ����� üũ

			m_mapMaterials.insert ( std::make_pair(dwFirst,spMaterials) );		// ����
		}

		SFile >> dwBuffSize_Blend;
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize_Blend );

		// 
		ClonePointEX( pd3dDevice, TRUE );

		return;
	}
    else if ( 0x00000104==dwVer )
	{
		SFile >> m_dwPointEX_Load;
		SAFE_DELETE_ARRAY ( m_pPointEX_Load );

		if ( m_dwPointEX_Load )
		{
			m_pPointEX_Load = new POINTEX_PREV[m_dwPointEX_Load];
			for ( DWORD i=0; i<m_dwPointEX_Load; ++i )
			{
				SFile.ReadBuffer ( &m_pPointEX_Load[i], sizeof(POINTEX_PREV) );
			}
		}

		SFile >> dwBuffSize_Blend; //	���� �ʰ� ��ŵ������ ���ǹǷ� ���� �ε忡���� ���ǹ�.

		SFile >> dwMaterialsNUM;
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwFirst;
			SFile.ReadBuffer ( strTexture, sizeof(char)*255 );

			std::tr1::shared_ptr<MATERIALEX> spMaterials( new MATERIALEX(m_dwPointEX) );

			spMaterials->strTexture = strTexture;
			
            spMaterials->m_textureRes = TextureManager::GetInstance().LoadTexture(
                strTexture,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_EFFECT,
				false,
				FALSE );

			if( spMaterials->m_textureRes.GetTexture() )
			    spMaterials->m_bAlpha = (BOOL)spMaterials->m_textureRes.GetAlphaType();	// ���� ������ �Ⱦ����� üũ

			m_mapMaterials.insert ( std::make_pair(dwFirst,spMaterials) );		// ����
			iter = m_mapMaterials.find ( dwFirst );
			if ( iter!=m_mapMaterials.end() )
			{
				(*iter).second->listFace.clear();

				DWORD dwSize;
				SFile >> dwSize;
				for ( DWORD i=0; i<dwSize; ++i )
				{
					SEPARATEOBJ sOBJ;
					sOBJ.Load( SFile );
				}
			}
		}

		SFile >> dwBuffSize_Blend;
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize_Blend );

		// 
		ClonePointEX( pd3dDevice, TRUE );

		return;
	}
    else if ( (0x00000103==dwVer) || (dwVer==0x00000102) || (dwVer==0x00000101) )
	{
		SFile >> m_dwPointEX_Load;
		SAFE_DELETE_ARRAY ( m_pPointEX_Load );

		if ( m_dwPointEX_Load )
		{
			m_pPointEX_Load = new POINTEX_PREV[m_dwPointEX_Load];
			for ( DWORD i=0; i<m_dwPointEX_Load; ++i )
			{
				SFile.ReadBuffer ( &m_pPointEX_Load[i], sizeof(POINTEX_PREV) );
			}
		}

		SFile >> dwBuffSize_Blend; //	���� �ʰ� ��ŵ������ ���ǹǷ� ���� �ε忡���� ���ǹ�.

		SFile >> dwMaterialsNUM;
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwFirst;
			SFile.ReadBuffer ( strTexture, sizeof(char)*255 );

			std::tr1::shared_ptr<MATERIALEX> spMaterials( new MATERIALEX(m_dwPointEX) );

			spMaterials->strTexture = strTexture;

            spMaterials->m_textureRes = TextureManager::GetInstance().LoadTexture(
                strTexture,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_EFFECT,
				false,
				FALSE );

			if( spMaterials->m_textureRes.GetTexture() )
			    spMaterials->m_bAlpha = (BOOL)spMaterials->m_textureRes.GetAlphaType();	// ���� ������ �Ⱦ����� üũ

			m_mapMaterials.insert ( std::make_pair(dwFirst,spMaterials) );		// ����
			iter = m_mapMaterials.find ( dwFirst );
			if ( iter!=m_mapMaterials.end() )
			{
				(*iter).second->listFace.clear();

                struct SEPARATEOBJ_PREV
                {
	                VERTEXNORCOLORTEX m_sVertex[3];
                };

				DWORD dwSize;
				SFile >> dwSize;
				for ( DWORD i=0; i<dwSize; ++i )
				{
                    SEPARATEOBJ_PREV sOBJ_PREV;
					SFile.ReadBuffer ( &sOBJ_PREV, sizeof(SEPARATEOBJ_PREV) );
				}
			}
		}

		SFile >> dwBuffSize_Blend;
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize_Blend );

		// 
		ClonePointEX( pd3dDevice, TRUE );

		return;
	}
	else if ( dwVer == 0x00000100 )
	{
		SFile >> dwBuffSize_Blend; //	���� �ʰ� ��ŵ������ ���ǹǷ� ���� �ε忡���� ���ǹ�.

		SFile >> m_dwPointEX_Load;
		SAFE_DELETE_ARRAY ( m_pPointEX_Load );

		if ( m_dwPointEX_Load )
		{
			m_pPointEX_Load = new POINTEX_PREV[m_dwPointEX_Load];
			for ( DWORD i=0; i<m_dwPointEX_Load; ++i )
			{
				SFile.ReadBuffer ( &m_pPointEX_Load[i], sizeof(POINTEX_PREV) );
			}
		}

		SFile >> dwMaterialsNUM;
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwFirst;
			SFile.ReadBuffer ( strTexture, sizeof(char)*255 );

			std::tr1::shared_ptr<MATERIALEX> spMaterials( new MATERIALEX(m_dwPointEX) );

			spMaterials->strTexture = strTexture;

            spMaterials->m_textureRes = TextureManager::GetInstance().LoadTexture(
                strTexture,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_EFFECT,
				false,
				FALSE );

			if( spMaterials->m_textureRes.GetTexture() )
			    spMaterials->m_bAlpha = (BOOL)spMaterials->m_textureRes.GetAlphaType();	// ���� ������ �Ⱦ����� üũ

			m_mapMaterials.insert ( std::make_pair(dwFirst,spMaterials) );		// ����
			iter = m_mapMaterials.find ( dwFirst );
			if ( iter!=m_mapMaterials.end() )
			{
				struct TEMP
				{
					D3DXVECTOR3			vMax;
					D3DXVECTOR3			vMin;
					VERTEXNORCOLORTEX			vVertex[3];
				};
				TEMP sTemp;

				SFile >> bExt;
				while ( bExt )
				{
					SFile >> sTemp.vMax;
					SFile >> sTemp.vMin;

					SFile.ReadBuffer ( &(sTemp.vVertex[0]), sizeof(VERTEXNORCOLORTEX) );
					SFile.ReadBuffer ( &(sTemp.vVertex[1]), sizeof(VERTEXNORCOLORTEX) );
					SFile.ReadBuffer ( &(sTemp.vVertex[2]), sizeof(VERTEXNORCOLORTEX) );
					SFile >> bExt;
				};
			}
		}

		
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );

		// 
		ClonePointEX( pd3dDevice, TRUE );

		return;
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );

		return;
	}
}

void DxEffectTiling::LoadBuffer ( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	LoadBuffer_NEW ( SFile, dwVer, pd3dDevice, FALSE );
}

void DxEffectTiling::LoadBuffer_NEW( sc::SerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bDummy )
{
	DWORD	dwBuffSize_Blend;
	DWORD	dwMaterialsNUM;

	DWORD	dwFirst;
	char	strTexture[255];
	MATERIALMAP_ITER iter;

	//std::for_each ( m_mapMaterials.begin(), m_mapMaterials.end(), std_afunc::DeleteMapObject() );
	m_mapMaterials.clear();

	DWORD dwBuffSize;
	SFile >> dwBuffSize; //	���� �ʰ� ��ŵ������ ���ǹǷ� ���� �ε忡���� ���ǹ�.

	if ( VERSION==dwVer )
	{
		SFile >> dwMaterialsNUM;
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwFirst;
			SFile.ReadBuffer ( strTexture, sizeof(char)*255 );

			std::tr1::shared_ptr<MATERIALEX> spMaterials( new MATERIALEX(m_dwPointEX) );

			// Skip
			{
				SFile >> dwBuffSize_Blend;
				DWORD dwCur = SFile.GetfTell();
				SFile.SetOffSet ( dwCur+dwBuffSize_Blend );
			}

			spMaterials->strTexture = strTexture;

            spMaterials->m_textureRes = TextureManager::GetInstance().LoadTexture(
                strTexture,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_EFFECT,
				false,
				FALSE );

			if( spMaterials->m_textureRes.GetTexture() )
			    spMaterials->m_bAlpha = (BOOL)spMaterials->m_textureRes.GetAlphaType();	// ���� ������ �Ⱦ����� üũ

			m_mapMaterials.insert ( std::make_pair(dwFirst,spMaterials) );		// ����
		}

		DWORD dwNUM;
		SFile >> dwBuffSize_Blend; //	���� �ʰ� ��ŵ������ ���ǹǷ� ���� �ε忡���� ���ǹ�.
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwNUM;

			MATERIALMAP_ITER iter = m_mapMaterials.find(dwNUM);
			if ( iter != m_mapMaterials.end() )
			{
				SAFE_DELETE ( (*iter).second->m_pMeshList );

				BOOL bUse;
				SFile >> bUse;
				if ( bUse )
				{
					(*iter).second->m_pMeshList = new DxSingleTexMesh;
					(*iter).second->m_pMeshList->Load( pd3dDevice, SFile, TRUE, FALSE, bDummy );
				}
			}
		}

		return;
	}
	else if ( 0x00000105==dwVer )
	{
		SFile >> m_dwPointEX_Load;
		SAFE_DELETE_ARRAY ( m_pPointEX_Load );

		if ( m_dwPointEX_Load )
		{
			m_pPointEX_Load = new POINTEX_PREV[m_dwPointEX_Load];
			for ( DWORD i=0; i<m_dwPointEX_Load; ++i )
			{
				SFile.ReadBuffer ( &m_pPointEX_Load[i], sizeof(POINTEX_PREV) );
			}
		}

		SFile >> dwBuffSize_Blend; //	���� �ʰ� ��ŵ������ ���ǹǷ� ���� �ε忡���� ���ǹ�.

		SFile >> dwMaterialsNUM;
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwFirst;
			SFile.ReadBuffer ( strTexture, sizeof(char)*255 );

			std::tr1::shared_ptr<MATERIALEX> spMaterials( new MATERIALEX(m_dwPointEX) );

			spMaterials->strTexture = strTexture;

			spMaterials->m_textureRes = TextureManager::GetInstance().LoadTexture(
				strTexture,
				false,
				TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
				0,
				D3DFMT_UNKNOWN,
				TEXTURE_RESIZE_NONE,
				TEXTURE_EFFECT,
				false,
				FALSE );

			if( spMaterials->m_textureRes.GetTexture() )
				spMaterials->m_bAlpha = (BOOL)spMaterials->m_textureRes.GetAlphaType();	// ���� ������ �Ⱦ����� üũ

			m_mapMaterials.insert ( std::make_pair(dwFirst,spMaterials) );		// ����
		}

		DWORD dwNUM;
		SFile >> dwBuffSize_Blend; //	���� �ʰ� ��ŵ������ ���ǹǷ� ���� �ε忡���� ���ǹ�.
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwNUM;

			MATERIALMAP_ITER iter = m_mapMaterials.find(dwNUM);
			if ( iter != m_mapMaterials.end() )
			{
				SAFE_DELETE ( (*iter).second->m_pMeshList );

				BOOL bUse;
				SFile >> bUse;
				if ( bUse )
				{
					(*iter).second->m_pMeshList = new DxSingleTexMesh;
					(*iter).second->m_pMeshList->Load( pd3dDevice, SFile, TRUE, FALSE, bDummy );
				}
			}
		}

		return;
	}
    else if ( 0x00000104==dwVer )
	{
		SFile >> m_dwPointEX_Load;
		SAFE_DELETE_ARRAY ( m_pPointEX_Load );

		if ( m_dwPointEX_Load )
		{
			m_pPointEX_Load = new POINTEX_PREV[m_dwPointEX_Load];
			for ( DWORD i=0; i<m_dwPointEX_Load; ++i )
			{
				SFile.ReadBuffer ( &m_pPointEX_Load[i], sizeof(POINTEX_PREV) );
			}
		}

		SFile >> dwBuffSize_Blend; //	���� �ʰ� ��ŵ������ ���ǹǷ� ���� �ε忡���� ���ǹ�.

		SFile >> dwMaterialsNUM;
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwFirst;
			SFile.ReadBuffer ( strTexture, sizeof(char)*255 );

			std::tr1::shared_ptr<MATERIALEX> spMaterials( new MATERIALEX(m_dwPointEX) );

			spMaterials->strTexture = strTexture;

            spMaterials->m_textureRes = TextureManager::GetInstance().LoadTexture(
                strTexture,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_EFFECT,
				false,
				FALSE );

			if( spMaterials->m_textureRes.GetTexture() )
			    spMaterials->m_bAlpha = (BOOL)spMaterials->m_textureRes.GetAlphaType();	// ���� ������ �Ⱦ����� üũ

			m_mapMaterials.insert ( std::make_pair(dwFirst,spMaterials) );		// ����
			iter = m_mapMaterials.find ( dwFirst );
			if ( iter!=m_mapMaterials.end() )
			{
				(*iter).second->listFace.clear();

				DWORD dwSize;
				SFile >> dwSize;
				for ( DWORD i=0; i<dwSize; ++i )
				{
					SEPARATEOBJ sOBJ;
					sOBJ.Load( SFile );
				}
			}
		}

		DWORD dwNUM;
		SFile >> dwBuffSize_Blend; //	���� �ʰ� ��ŵ������ ���ǹǷ� ���� �ε忡���� ���ǹ�.
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwNUM;

			MATERIALMAP_ITER iter = m_mapMaterials.find(dwNUM);
			if ( iter != m_mapMaterials.end() )
			{
				SAFE_DELETE ( (*iter).second->m_pMeshList );

				BOOL bUse;
				SFile >> bUse;
				if ( bUse )
				{
					(*iter).second->m_pMeshList = new DxSingleTexMesh;
					(*iter).second->m_pMeshList->Load( pd3dDevice, SFile, TRUE, FALSE, bDummy );
				}
			}
		}

		return;
	}
    else if ( 0x00000103==dwVer || 0x00000102==dwVer )
	{
		SFile >> m_dwPointEX_Load;
		SAFE_DELETE_ARRAY ( m_pPointEX_Load );

		if ( m_dwPointEX_Load )
		{
			m_pPointEX_Load = new POINTEX_PREV[m_dwPointEX_Load];
			for ( DWORD i=0; i<m_dwPointEX_Load; ++i )
			{
				SFile.ReadBuffer ( &m_pPointEX_Load[i], sizeof(POINTEX_PREV) );
			}
		}

		SFile >> dwBuffSize_Blend; //	���� �ʰ� ��ŵ������ ���ǹǷ� ���� �ε忡���� ���ǹ�.

		SFile >> dwMaterialsNUM;
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwFirst;
			SFile.ReadBuffer ( strTexture, sizeof(char)*255 );

			std::tr1::shared_ptr<MATERIALEX> spMaterials( new MATERIALEX(m_dwPointEX) );

			spMaterials->strTexture = strTexture;

            spMaterials->m_textureRes = TextureManager::GetInstance().LoadTexture(
                strTexture,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_EFFECT,
				false,
				FALSE );

			if( spMaterials->m_textureRes.GetTexture() )
			    spMaterials->m_bAlpha = (BOOL)spMaterials->m_textureRes.GetAlphaType();	// ���� ������ �Ⱦ����� üũ

			m_mapMaterials.insert ( std::make_pair(dwFirst,spMaterials) );		// ����
			iter = m_mapMaterials.find ( dwFirst );
			if ( iter!=m_mapMaterials.end() )
			{
				(*iter).second->listFace.clear();

                struct SEPARATEOBJ_PREV
                {
	                VERTEXNORCOLORTEX m_sVertex[3];
                };

				DWORD dwSize;
				SFile >> dwSize;
				for ( DWORD i=0; i<dwSize; ++i )
				{
					SEPARATEOBJ sOBJ;
                    SEPARATEOBJ_PREV sOBJ_PREV;
					SFile.ReadBuffer ( &sOBJ_PREV, sizeof(SEPARATEOBJ_PREV) );
				}
			}
		}

		DWORD dwNUM;
		SFile >> dwBuffSize_Blend; //	���� �ʰ� ��ŵ������ ���ǹǷ� ���� �ε忡���� ���ǹ�.
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwNUM;

			MATERIALMAP_ITER iter = m_mapMaterials.find(dwNUM);
			if ( iter != m_mapMaterials.end() )
			{
				SAFE_DELETE ( (*iter).second->m_pMeshList );

				BOOL bUse;
				SFile >> bUse;
				if ( bUse )
				{
					(*iter).second->m_pMeshList = new DxSingleTexMesh;
					(*iter).second->m_pMeshList->Load( pd3dDevice, SFile, TRUE, FALSE, bDummy );
				}
			}
		}

		return;
	}
	else if ( dwVer == 0x00000101 )
	{
		SFile >> m_dwPointEX_Load;
		SAFE_DELETE_ARRAY ( m_pPointEX_Load );

		if ( m_dwPointEX_Load )
		{
			m_pPointEX_Load = new POINTEX_PREV[m_dwPointEX_Load];
			for ( DWORD i=0; i<m_dwPointEX_Load; ++i )
			{
				SFile.ReadBuffer ( &m_pPointEX_Load[i], sizeof(POINTEX_PREV) );
			}
		}

		SFile >> dwBuffSize_Blend; //	���� �ʰ� ��ŵ������ ���ǹǷ� ���� �ε忡���� ���ǹ�.

		SFile >> dwMaterialsNUM;
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwFirst;
			SFile.ReadBuffer ( strTexture, sizeof(char)*255 );

			std::tr1::shared_ptr<MATERIALEX> spMaterials( new MATERIALEX(m_dwPointEX) );

			spMaterials->strTexture = strTexture;

            spMaterials->m_textureRes = TextureManager::GetInstance().LoadTexture(
                strTexture,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_EFFECT,
				false,
				FALSE );

			if( spMaterials->m_textureRes.GetTexture() )
			    spMaterials->m_bAlpha = (BOOL)spMaterials->m_textureRes.GetAlphaType();	// ���� ������ �Ⱦ����� üũ

			m_mapMaterials.insert ( std::make_pair(dwFirst,spMaterials) );		// ����
			iter = m_mapMaterials.find ( dwFirst );
			if ( iter!=m_mapMaterials.end() )
			{
				(*iter).second->listFace.clear();

                struct SEPARATEOBJ_PREV
                {
	                VERTEXNORCOLORTEX m_sVertex[3];
                };

				DWORD dwSize;
				SFile >> dwSize;
				for ( DWORD i=0; i<dwSize; ++i )
				{
					SEPARATEOBJ sOBJ;
                    SEPARATEOBJ_PREV sOBJ_PREV;
					SFile.ReadBuffer ( &sOBJ_PREV, sizeof(SEPARATEOBJ_PREV) );
				}
			}
		}

		DWORD dwNUM;
		SFile >> dwBuffSize_Blend; //	���� �ʰ� ��ŵ������ ���ǹǷ� ���� �ε忡���� ���ǹ�.
		for ( DWORD i=0; i<dwMaterialsNUM; ++i )
		{
			SFile >> dwNUM;

			MATERIALMAP_ITER iter = m_mapMaterials.find(dwNUM);
			if ( iter != m_mapMaterials.end() )
			{
				SAFE_DELETE ( (*iter).second->m_pMeshList );

				BOOL bUse;
				SFile >> bUse;
				if ( bUse )
				{
					(*iter).second->m_pMeshList = new DxSingleTexMesh;
					(*iter).second->m_pMeshList->Load_VER100 ( pd3dDevice, SFile, bDummy );
				}
			}
		}

		return;
	}
	else if ( dwVer == 0x00000100 )
	{
		SFile >> dwBuffSize_Blend;

		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize+dwBuffSize_Blend );

		return;
	}
	else
	{
	    DWORD dwCur = SFile.GetfTell();
	    SFile.SetOffSet ( dwCur+dwBuffSize );

		return;
	}
}

void DxEffectTiling::ClonePointEX( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMakeTriangle )
{
	if ( !m_pPointEX_Load )	
		return;

	//// m_pPointUseColor ����
	//{
	//	MATERIALMAP_ITER iter = m_mapMaterials.begin();
	//	for( ; iter != m_mapMaterials.end(); ++iter )
	//	{
	//		SAFE_DELETE_ARRAY( (*iter).second->m_pPointUseColor );
	//		(*iter).second->m_pPointUseColor = new POINT_USE_COLOR[ m_dwPointEX ];
	//	}
	//}

	D3DXVECTOR3 vLoadPos, vPos;
	for ( DWORD i=0; i<m_dwPointEX_Load; ++i )
	{
		for ( DWORD j=0; j<m_dwPointEX; ++j )
		{
			vLoadPos	= m_pPointEX_Load[i].vPos;
			vPos		= m_pPointEX[j].vPos;
			if ( ((vLoadPos.x+0.01f)>vPos.x) && (vLoadPos.x<(vPos.x+0.01f)) && 
				((vLoadPos.y+0.01f)>vPos.y) && (vLoadPos.y<(vPos.y+0.01f)) && 
				((vLoadPos.z+0.01f)>vPos.z) && (vLoadPos.z<(vPos.z+0.01f)) )
			{
				MATERIALMAP_ITER iter = m_mapMaterials.find( m_pPointEX_Load[i].dwMaterial );
				if( iter != m_mapMaterials.end() )
				{
					(*iter).second->m_pPointUseColor[j].bUse = TRUE;
					(*iter).second->m_pPointUseColor[j].dwColor = m_pPointEX_Load[i].dwColor;
				}

				//m_pPointEX[j].dwMaterial	= m_pPointEX_Load[i].dwMaterial;		// Load �� �����͸� �д´�.
				//m_pPointEX[j].dwColor		= m_pPointEX_Load[i].dwColor;			// Load �� �����͸� �д´�.

				m_pPointEX_Load[i].pPoint	= m_pPointEX[j].pPoint;			// �ӽ÷� �־��� ���̴�.

				j = m_dwPointEX;		// ���� ������ ������.
			}
		}
	}

	// �ﰢ�� ����Ʈ�� �����.
	if ( bMakeTriangle )
	{
		DWORD				dwMaterial;
		DWORD				dwFaceNUM;
		POINTA*				pPoint;
		MATERIALMAP_ITER	iter;
		TRIANGLELIST_ITER	_iter;
		for ( DWORD i=0; i<m_dwPointEX_Load; ++i )
		{
			dwMaterial = m_pPointEX_Load[i].dwMaterial;
			iter = m_mapMaterials.find(dwMaterial);

			if ( iter != m_mapMaterials.end() )
			{
				pPoint = m_pPointEX_Load[i].pPoint;
				while (pPoint)
				{
					BOOL bUse = FALSE;
					dwFaceNUM = m_pPoint[pPoint->dwIndex].dwFace_NUM;

					_iter = (*iter).second->listTriangle.begin();
					for ( ; _iter!=(*iter).second->listTriangle.end(); ++_iter )
					{
						if ( (*_iter) == dwFaceNUM )
						{
							bUse = TRUE;
							//break;
						}
					}

					if ( !bUse )		// ���� ���� ���� �����Ѵ�.
					{
						(*iter).second->listTriangle.push_back ( dwFaceNUM );
					}

					pPoint = pPoint->pNext;
				}
			}
			else	// ��Ʈ������ ���� �� ����.. ������... �ƴϴ� ���� üũ �� ����.
			{
			}

			m_pPointEX_Load[i].pPoint = NULL;		// �̰� ������ �����ϴ�. �ӽ� ���̴�. �ı� �� �� ���� ������ �ȵȴ�.
		}
	}
	else
	{
		for ( DWORD i=0; i<m_dwPointEX_Load; ++i )
		{
			m_pPointEX_Load[i].pPoint = NULL;		// �̰� ������ �����ϴ�. �ӽ� ���̴�. �ı� �� �� ���� ������ �ȵȴ�.
		}
	}


	SAFE_DELETE_ARRAY ( m_pPointEX_Load );
}
#include "pch.h"

#include <algorithm>
#include "../../Common/StlFunctions.h"

#include "DxFrameMesh.h"
#include "../../DxLand/DxLandMan.h"
#include "../../../SigmaCore/File/SerialFile.h"
#include "../../DxTools/TextureManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL	DxMeshes::SaveFile ( sc::SerialFile &SFile )
{
	SFile << VERSION_B;

	SFile << m_cMaterials;

	if ( m_cMaterials )
	{		
		SFile.WriteBuffer ( m_rgMaterials, sizeof ( D3DMATERIALQ ) * m_cMaterials );
		SFile.WriteBuffer ( m_exMaterials, sizeof ( D3DEXMATERIAL ) * m_cMaterials );
	}

	if ( m_strTextureFiles )
	{
		SFile << BOOL ( TRUE );
		for ( DWORD i = 0; i < m_cMaterials; i++ )
		{
			int StrLength;
			StrLength = m_strTextureFiles[i].GetLength();
			SFile << StrLength;

			if ( StrLength )
			{
				SFile.WriteBuffer ( (void *)m_strTextureFiles[i].GetString(), sizeof ( char ) * StrLength );
			}
		}
	}
	else
	{
		SFile << BOOL ( FALSE );
	}


	if ( m_szName )
	{
		SFile << BOOL ( TRUE );
		int	StrLength;
		StrLength = static_cast<int>( strlen ( m_szName ) );
		SFile << StrLength;
		if ( StrLength )
		{
			SFile.WriteBuffer ( m_szName, sizeof ( char ) * StrLength );
		}
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	for ( DWORD i=0; i<m_pDxOctreeMesh->m_dwAttribTableSize; i++ )
	{
		if ( m_cMaterials <= m_pDxOctreeMesh->m_pAttribTable[i].AttribId )
		{
			MessageBox ( NULL, "cMaterials <= m_pDxOctreeMesh->m_pAttribTable[i].AttribId", "ERROR", MB_OK );
		}
	}

	m_pDxOctreeMesh->SaveFile ( SFile );
	
	if ( m_pMeshNext )
	{
		SFile << BOOL ( TRUE );
		m_pMeshNext->SaveFile ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	return TRUE;
}

BOOL	DxMeshes::LoadFile_NEW ( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwVer;
	SFile >> dwVer;

	BOOL	bExist = FALSE;

	SFile >> m_cMaterials;	

	if ( m_cMaterials == 0 )
    {
        m_rgMaterials = new D3DMATERIALQ[1];
		m_exMaterials = new D3DEXMATERIAL[1];		
        m_pTextureRes = new TextureResource[1];
		m_pMaskTexRes = new TextureResource[1];
		m_strTextureFiles = new CString[1];

        if ( m_rgMaterials == NULL || m_pTextureRes == NULL || m_strTextureFiles == NULL || m_exMaterials == NULL )
        {
            return FALSE;
        }
       
        memset(m_rgMaterials, 0, sizeof(D3DMATERIALQ));
        m_rgMaterials[0].Diffuse.r = 0.5f;
        m_rgMaterials[0].Diffuse.g = 0.5f;
        m_rgMaterials[0].Diffuse.b = 0.5f;
        m_rgMaterials[0].Specular = m_rgMaterials[0].Diffuse;

		m_strTextureFiles[0] = "";
    }
	//	Note : 메터리얼 값이 있을 경우.
	//
    else
    {
        m_rgMaterials = new D3DMATERIALQ[m_cMaterials];
		m_exMaterials = new D3DEXMATERIAL[m_cMaterials];	
        m_pTextureRes = new TextureResource[m_cMaterials];
		m_pMaskTexRes = new TextureResource[m_cMaterials];
		m_strTextureFiles = new CString[m_cMaterials];		
        if (m_rgMaterials == NULL || m_pTextureRes == NULL || m_strTextureFiles == NULL || m_exMaterials == NULL )
        {
            return	FALSE;
        }
        
		SFile.ReadBuffer ( m_rgMaterials, sizeof ( D3DMATERIALQ ) * m_cMaterials );

		///////////////////////////////////////
		// Note : 버전 체크, 107 : D3DEXMATERIAL 사이즈가 변했음

		TCHAR szFileType[FILETYPESIZE] = "";
		DWORD dwVersion;

		SFile.GetFileType(NULL, 0, dwVersion); 

		if ( 0x117 <= dwVersion )
		{
			SFile.ReadBuffer ( m_exMaterials, sizeof ( D3DEXMATERIAL ) * m_cMaterials );
		}
		else
		{
			D3DEXMATERIAL_100 exMaterial_100;

			for ( DWORD i=0; i<m_cMaterials; ++i )
			{
				SFile.ReadBuffer ( &exMaterial_100, sizeof ( D3DEXMATERIAL_100 ) );

				m_exMaterials[i].MtrlType       = exMaterial_100.MtrlType;
				m_exMaterials[i].bSubSetTriRend = exMaterial_100.bSubSetTriRend;
				m_exMaterials[i].bCamColl       = FALSE;
			}
		}

		SFile >> bExist;
		if ( bExist )
		{
			//	Note	:	strTextureFiles로드 및 텍스쳐 로드
			//
			for ( DWORD i = 0; i < m_cMaterials; i++ )
			{	
				int StrLength;			
				SFile >> StrLength;

				if ( StrLength )
				{
					char	*TmpStr = new char [StrLength + 1];
					SFile.ReadBuffer ( TmpStr, sizeof ( char ) * StrLength );
					TmpStr[StrLength] = '\0';

					m_strTextureFiles[i] = TmpStr;

					delete	[] TmpStr;


					//	Note : 텍스쳐 로드.
					//
					if (!m_strTextureFiles[i].IsEmpty())
					{
                        m_pTextureRes[i] = TextureManager::GetInstance().LoadTexture(
                            m_strTextureFiles[i].GetString(),
                            false,
                            TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                            0,
                            D3DFMT_UNKNOWN,
                            TEXTURE_RESIZE_1024,
                            TEXTURE_LAND,
							false,
							FALSE );

						// DXMTRLTYPE 재설정
						if( (m_exMaterials[i].MtrlType==MATERIAL_ALPHA_NOZWRI) || 
							(m_exMaterials[i].MtrlType==MATERIAL_ALPHA_NOTEST) )	
						{
						}
						else if ( m_pTextureRes[i].GetTexture() )
						{
							TEXTURE_ALPHA emTexType = m_pTextureRes[i].GetAlphaType();
							if( emTexType==EMTT_NORMAL )			m_exMaterials[i].MtrlType = MATERIAL_NORMAL;
							else if( emTexType==EMTT_ALPHA_HARD )	m_exMaterials[i].MtrlType = MATERIAL_ALPHA_HARD;
							else if( emTexType==EMTT_ALPHA_SOFT )	m_exMaterials[i].MtrlType = MATERIAL_ALPHA_SOFT;
							else if( emTexType==EMTT_ALPHA_SOFT01 )	m_exMaterials[i].MtrlType = MATERIAL_ALPHA_SOFT;
							else if( emTexType==EMTT_ALPHA_SOFT02 )	m_exMaterials[i].MtrlType = MATERIAL_ALPHA_SOFT;
						}
					}				
				}
			}
		}
    }

	SFile >> bExist;
	if ( bExist )
	{
		int	StrLength;		
		SFile >> StrLength;
		
		if ( StrLength )
		{
			m_szName = new char[StrLength];
			SFile.ReadBuffer ( m_szName, sizeof ( char ) * StrLength );
		}
	}

	m_pDxOctreeMesh = new DxOctreeMesh;
	m_pDxOctreeMesh->LoadFile_NEW( SFile, pd3dDevice );
	

	for ( DWORD i=0; i<m_pDxOctreeMesh->m_dwAttribTableSize; i++ )
	{
		if ( m_cMaterials <= m_pDxOctreeMesh->m_pAttribTable[i].AttribId )
		{
			//MessageBox ( NULL, "cMaterials <= m_pDxOctreeMesh->m_pAttribTable[i].AttribId", "ERROR", MB_OK );
		}
	}

	SFile >> bExist;
	if ( bExist )
	{
		m_pMeshNext = new DxMeshes;
		m_pMeshNext->LoadFile_NEW ( SFile, pd3dDevice );
	}

	return TRUE;
}

BOOL	DxMeshes::LoadFile_OLD ( sc::SerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	BOOL	bExist = FALSE;

	SFile >> m_cMaterials;	

	if ( m_cMaterials == 0 )
	{
		m_rgMaterials = new D3DMATERIALQ[1];
		m_exMaterials = new D3DEXMATERIAL[1];		
		m_pTextureRes = new TextureResource[1];
		m_pMaskTexRes = new TextureResource[1];
		m_strTextureFiles = new CString[1];

		if ( m_rgMaterials == NULL || m_pTextureRes == NULL || m_strTextureFiles == NULL || m_exMaterials == NULL )
		{
			return FALSE;
		}

		memset(m_rgMaterials, 0, sizeof(D3DMATERIALQ));
		m_rgMaterials[0].Diffuse.r = 0.5f;
		m_rgMaterials[0].Diffuse.g = 0.5f;
		m_rgMaterials[0].Diffuse.b = 0.5f;
		m_rgMaterials[0].Specular = m_rgMaterials[0].Diffuse;

		m_strTextureFiles[0] = "";
	}
	//	Note : 메터리얼 값이 있을 경우.
	//
	else
	{
		m_rgMaterials = new D3DMATERIALQ[m_cMaterials];
		m_exMaterials = new D3DEXMATERIAL[m_cMaterials];	
		m_pTextureRes = new TextureResource[m_cMaterials];
		m_pMaskTexRes = new TextureResource[m_cMaterials];
		m_strTextureFiles = new CString[m_cMaterials];		
		if (m_rgMaterials == NULL || m_pTextureRes == NULL || m_strTextureFiles == NULL || m_exMaterials == NULL )
		{
			return	FALSE;
		}

		SFile.ReadBuffer ( m_rgMaterials, sizeof ( D3DMATERIALQ ) * m_cMaterials );

		///////////////////////////////////////
		// Note : 버전 체크, 107 : D3DEXMATERIAL 사이즈가 변했음

		TCHAR szFileType[FILETYPESIZE] = "";
		DWORD dwVersion;

		SFile.GetFileType(NULL, 0, dwVersion); 

		if ( 0x117 <= dwVersion )
		{
			SFile.ReadBuffer ( m_exMaterials, sizeof ( D3DEXMATERIAL ) * m_cMaterials );
		}
		else
		{
			D3DEXMATERIAL_100 exMaterial_100;

			for ( DWORD i=0; i<m_cMaterials; ++i )
			{
				SFile.ReadBuffer ( &exMaterial_100, sizeof ( D3DEXMATERIAL_100 ) );

				m_exMaterials[i].MtrlType       = exMaterial_100.MtrlType;
				m_exMaterials[i].bSubSetTriRend = exMaterial_100.bSubSetTriRend;
				m_exMaterials[i].bCamColl       = FALSE;
			}
		}

		SFile >> bExist;
		if ( bExist )
		{
			//	Note	:	strTextureFiles로드 및 텍스쳐 로드
			//
			for ( DWORD i = 0; i < m_cMaterials; i++ )
			{	
				int StrLength;			
				SFile >> StrLength;

				// 잘못된 값이 들어왔다면 이후 모든것 취소한다.
				if ( StrLength > MAX_PATH )
					return FALSE;

				if ( StrLength )
				{
					char	*TmpStr = new char [StrLength + 1];
					SFile.ReadBuffer ( TmpStr, sizeof ( char ) * StrLength );
					TmpStr[StrLength] = '\0';

					m_strTextureFiles[i] = TmpStr;

					delete	[] TmpStr;


					//	Note : 텍스쳐 로드.
					//
					if (!m_strTextureFiles[i].IsEmpty())
					{
						m_pTextureRes[i] = TextureManager::GetInstance().LoadTexture(
							m_strTextureFiles[i].GetString(),
							false,
							TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
							0,
							D3DFMT_UNKNOWN,
							TEXTURE_RESIZE_1024,
							TEXTURE_LAND,
							false,
							FALSE );

						// DXMTRLTYPE 재설정
						if( (m_exMaterials[i].MtrlType==MATERIAL_ALPHA_NOZWRI) || 
							(m_exMaterials[i].MtrlType==MATERIAL_ALPHA_NOTEST) )	
						{
						}
						else if ( m_pTextureRes[i].GetTexture() )
						{
							TEXTURE_ALPHA emTexType = m_pTextureRes[i].GetAlphaType();
							if( emTexType==EMTT_NORMAL )			m_exMaterials[i].MtrlType = MATERIAL_NORMAL;
							else if( emTexType==EMTT_ALPHA_HARD )	m_exMaterials[i].MtrlType = MATERIAL_ALPHA_HARD;
							else if( emTexType==EMTT_ALPHA_SOFT )	m_exMaterials[i].MtrlType = MATERIAL_ALPHA_SOFT;
							else if( emTexType==EMTT_ALPHA_SOFT01 )	m_exMaterials[i].MtrlType = MATERIAL_ALPHA_SOFT;
							else if( emTexType==EMTT_ALPHA_SOFT02 )	m_exMaterials[i].MtrlType = MATERIAL_ALPHA_SOFT;
						}
					}				
				}
			}
		}
	}

	SFile >> bExist;
	if ( bExist )
	{
		int	StrLength;		
		SFile >> StrLength;

		if ( StrLength )
		{
			m_szName = new char[StrLength];
			SFile.ReadBuffer ( m_szName, sizeof ( char ) * StrLength );
		}
	}

	m_pDxOctreeMesh = new DxOctreeMesh;
	m_pDxOctreeMesh->LoadFile_OLD( SFile, pd3dDevice );


	for ( DWORD i=0; i<m_pDxOctreeMesh->m_dwAttribTableSize; i++ )
	{
		if ( m_cMaterials <= m_pDxOctreeMesh->m_pAttribTable[i].AttribId )
		{
			//MessageBox ( NULL, "cMaterials <= m_pDxOctreeMesh->m_pAttribTable[i].AttribId", "ERROR", MB_OK );
		}
	}

	SFile >> bExist;
	if ( bExist )
	{
		m_pMeshNext = new DxMeshes;
		m_pMeshNext->LoadFile_OLD ( SFile, pd3dDevice );
	}

	return TRUE;
}


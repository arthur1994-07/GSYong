#include "pch.h"

#include <algorithm>
#include "../../Common/StlFunctions.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../DxTools/TextureManager.h"
#include "./DxTexEffMan.h"

#include "DxTextureEffMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// Note : DxTextureEffMan
DxTextureEffMan::DxTextureEffMan()
    : m_pd3dDevice( NULL )
{
}

DxTextureEffMan::~DxTextureEffMan()
{
	// Note : 이거 Texture 때문에 지우면 뻑 ~!!
	//m_mapTexEff.clear();

    std::for_each( m_mapTexEff.begin(), m_mapTexEff.end(), std_afunc::DeleteMapObject() );
	m_mapTexEff.clear();
}

void DxTextureEffMan::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
    m_pd3dDevice = pd3dDevice;

    MAPTEXEFF_ITER iter = m_mapTexEff.begin();
    for ( ; iter!=m_mapTexEff.end(); ++iter )
    {
        (*iter).second->OnResetDevice( pd3dDevice );
    }
}

void DxTextureEffMan::OnLostDevice()
{
    MAPTEXEFF_ITER iter = m_mapTexEff.begin();
    for ( ; iter!=m_mapTexEff.end(); ++iter )
    {
        (*iter).second->OnLostDevice( m_pd3dDevice );
    }
}

void DxTextureEffMan::OnDestroyDevice()
{
	m_mapTexList.clear();

	std::for_each( m_mapTexEff.begin(), m_mapTexEff.end(), std_afunc::DeleteMapObject() );
	m_mapTexEff.clear();
}

void DxTextureEffMan::InsertTexList( const char* szName )
{
    TextureData sData(TEXEFF_DEFAULT,FALSE);
	TSTRING strName(szName);
	std::transform ( strName.begin(), strName.end(), strName.begin(), tolower );
	m_mapTexList.insert( std::make_pair(strName,sData) );
}

void DxTextureEffMan::InsertTextureEff( MAPTEXEFF& mapData, const char* szName, DxTexEffBase* pBase )
{
    if( !pBase )
        return;

	MAPTEXEFF_ITER iter = mapData.find( szName );
	if( iter==mapData.end() )
	{
		// Note : 새로운 것 생성
		mapData.insert( std::make_pair(szName,pBase) );
	}
	else
	{
		// Note : 데이터의 추가
		pBase->m_pNext = (*iter).second;
		(*iter).second = pBase;
	}
}

DxTexEffBase* DxTextureEffMan::GetEffRoot( const char* szName )
{
	MAPTEXEFF_ITER iter = m_mapTexEff.find( szName );
	if( iter!=m_mapTexEff.end() )	
        return (*iter).second;

	return NULL;
}

void DxTextureEffMan::ReloadVMFX( LPDIRECT3DDEVICEQ pd3dDevice )
{
    MAPTEXEFF_ITER iter = m_mapTexEff.begin();
    for ( ; iter!=m_mapTexEff.end(); ++iter )
    {
        (*iter).second->ReloadVMFX( pd3dDevice );
    }
}

BOOL DxTextureEffMan::SetTextureData( const TSTRING& strName, const TextureData& sTextureData )
{
    MAPTEXLIST_ITER iter = m_mapTexList.find( strName );
	if( iter==m_mapTexList.end() )	
        return FALSE;	// ERROR
	
    (*iter).second = sTextureData;

	return TRUE;
}

const TextureData* DxTextureEffMan::GetTextureData( const TSTRING& strName ) const
{
	MAPTEXLIST_CITER iter = m_mapTexList.find( strName );
	if( iter==m_mapTexList.end() )	
        return NULL;

    return &(*iter).second;
}

DWORD DxTextureEffMan::GetTexType( const TSTRING& strName ) const
{
	MAPTEXLIST_CITER iter = m_mapTexList.find( strName );
	if( iter==m_mapTexList.end() )	
		return TEXEFF_DEFAULT;

	return (*iter).second.m_dwTexType;
}

BOOL DxTextureEffMan::IsGetTextureEFFDXT( DWORD i, LPCSTR& szName, DWORD& dwTexType, DWORD& dwFlagEff, TSTRING& strVM ) const
{
	if( i >= m_mapTexList.size() )	
        return FALSE;	// ERROR

    // 텍스쳐 이름과 어떤타입의 Tex 인지 알아온다.
    {
	    MAPTEXLIST_CITER iter = m_mapTexList.begin();
	    for( DWORD loop=0; loop<i; ++loop )	++iter;
    	
	    szName = iter->first.c_str();
	    dwTexType = (DWORD)iter->second.m_dwTexType;
    }

    // 무슨 Effect가 쓰이고 있는지 확인한다.
    {
        strVM = _T("");

        MAPTEXEFF_CITER iter = m_mapTexEff.find( szName );
        if ( iter != m_mapTexEff.end() ) 
        {            
            DxTexEffBase* pCur = (*iter).second;
		    while( pCur )
		    {
                pCur->GetFileNameVM( strVM );
                return TRUE;
		    }
        }
    }

	return TRUE;
}

void DxTextureEffMan::DeleteTexEff( const char* pName, DxTexEffBase* pBase )
{
	DxTexEffBase* pPrev = NULL;
	DxTexEffBase* pCur;
	MAPTEXEFF_ITER iter = m_mapTexEff.find( pName );
	if( iter!=m_mapTexEff.end() )
	{
		pCur = (*iter).second;
		while( pCur )
		{
			if( pCur==pBase )
			{
				if( !pPrev )	// 처음 걸리면
				{
					(*iter).second = pCur->m_pNext;

					pCur->m_pNext = NULL;
					SAFE_DELETE( pCur );
				}
				else			// 중간에 있을때
				{
					pPrev->m_pNext = pCur->m_pNext;

					pCur->m_pNext = NULL;
					SAFE_DELETE( pCur );
				}

				// Note : 데이터가 없다면 삭제를 해준다.
				if( !(*iter).second )
				{
					m_mapTexEff.erase( iter );
				}

				return;
			}

			pPrev = pCur;
			pCur = pCur->m_pNext;
		}
	}
}

void DxTextureEffMan::SetDXTFlag( const TSTRING& strName, DWORD dwFlag )
{
	MAPTEXLIST_ITER iter = m_mapTexList.find( strName );
	if ( iter != m_mapTexList.end() )
	{
		iter->second.m_dwTexType = dwFlag;
	}
}

// VM 을 사용하면서 2Side Rendering을 사용했는가~?
BOOL DxTextureEffMan::IsVM2Side( const TSTRING& strName ) const
{
    MAPTEXEFF_CITER iter = m_mapTexEff.find( strName );
	if( iter==m_mapTexEff.end() )	
        return FALSE;

    DxTexEffBase* pCur = (*iter).second;
	while( pCur )
	{
        if ( pCur->IsVM2Side() )
            return TRUE;

		pCur = pCur->m_pNext;
	}

    return FALSE;
}

void DxTextureEffMan::CloneMapTexEff( LPDIRECT3DDEVICEQ pd3dDevice, MAPTEXEFF& mapSrc, MAPTEXEFF& mapDest )
{
	PBYTE			pProp;
	std::string		strName;
	DxTexEffBase*	pCur;
	DxTexEffBase*	pNew;

	MAPTEXEFF_ITER iter = mapSrc.begin();
	for( ; iter!=mapSrc.end(); ++iter )
	{
		strName = (*iter).first.c_str();

		pCur = (*iter).second;
		while( pCur )
		{
			// Note : 만들어야지.
			pCur->GetProperty( pProp );
			pNew = DxTexEffMan::GetInstance().AdaptToDxFrame( pCur->GetTypeID(), pd3dDevice, pProp );

			// Note : TextureEff 삽입
			InsertTextureEff( mapDest, strName.c_str(), pNew );

			pCur = pCur->m_pNext;
		}
	}
}

void DxTextureEffMan::Clone( LPDIRECT3DDEVICEQ pd3dDevice, const DxTextureEffMan* pSrc )
{
    OnLostDevice();
    OnDestroyDevice();

    m_mapTexList = pSrc->m_mapTexList;

    PBYTE			pProp;
    MAPTEXEFF_CITER iter = pSrc->m_mapTexEff.begin();
	for ( ; iter!=pSrc->m_mapTexEff.end(); ++iter )
	{
		DxTexEffBase* pCur = (*iter).second;
		while( pCur )
		{
            // Note : 만들어야지.
			pCur->GetProperty( pProp );
			DxTexEffBase* pNew = DxTexEffMan::GetInstance().AdaptToDxFrame( pCur->GetTypeID(), pd3dDevice, pProp );

			// Note : TextureEff 삽입
			InsertTextureEff( m_mapTexEff, (*iter).first.c_str(), pNew );

			pCur = pCur->m_pNext;
		}
	}
}

void DxTextureEffMan::SavePSF( sc::SerialFile& SFile )
{
    SFile << VERSION;
    SFile.BeginBlock();
    {
	    // Note : TexList
	    {
		    DWORD dwSize = (DWORD)m_mapTexList.size();
		    SFile << dwSize;

		    MAPTEXLIST_ITER iter = m_mapTexList.begin();
		    for( ; iter!=m_mapTexList.end(); ++iter )
		    {
			    SFile << (*iter).first;
			    SFile << (*iter).second.m_dwTexType;
                SFile << (*iter).second.m_dwPieceLM_Option;
		    }
	    }


	    // Note : Texture Effect List
	    {
		    DWORD dwSize = (DWORD)m_mapTexEff.size();
		    SFile << dwSize;

		    DxTexEffBase* pCur;
		    MAPTEXEFF_ITER iter = m_mapTexEff.begin();
		    for( ; iter!=m_mapTexEff.end(); ++iter )
		    {
			    SFile << (*iter).first;

			    dwSize = 0;
			    pCur = (*iter).second;
			    while( pCur )
			    {
				    ++dwSize;
				    pCur = pCur->m_pNext;
			    }

			    SFile << dwSize;

			    pCur = (*iter).second;
			    while( pCur )
			    {
				    SFile << pCur->GetTypeID();
				    pCur->SavePSF( SFile );

				    pCur = pCur->m_pNext;
			    }
		    }
	    }
    }
    SFile.EndBlock();
}

void DxTextureEffMan::LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, BOOL bPiece )
{
    DWORD dwVer, dwBufferSize;
    SFile >> dwVer;
    SFile >> dwBufferSize;

	if ( dwVer==0x102 )
	{
		// Note : TexList
		{
			DWORD dwSize;
			SFile >> dwSize;

			DWORD           dwPieceLM_Option;
			DWORD			dwFlag;
			std::string		strName;
			MAPTEXLIST_ITER iter;
			for( DWORD i=0; i<dwSize; ++i )
			{
				SFile >> strName;
				SFile >> dwFlag;
				SFile >> dwPieceLM_Option;

				if ( dwFlag == 0 )
				{
					dwFlag = TEXEFF_DEFAULT;
				}

				iter = m_mapTexList.find( strName.c_str() );
				if( iter!=m_mapTexList.end() )	
				{
					(*iter).second.m_dwTexType = dwFlag;
					(*iter).second.m_dwPieceLM_Option = dwPieceLM_Option;
				}
				else if ( bPiece )
				{
					TextureData sTextureData(dwFlag,dwPieceLM_Option);
					std::transform ( strName.begin(), strName.end(), strName.begin(), tolower );
					m_mapTexList.insert( std::make_pair( strName, sTextureData ) );
				}
			}
		}


		// Note : Texture Effect List
		{
			DWORD dwSize, dwEffSize;
			SFile >> dwSize;

			DxTexEffBase*	pTexEffBase;
			DWORD			dwTypeID;
			std::string		strName;
			for( DWORD i=0; i<dwSize; ++i )
			{
				SFile >> strName;

				SFile >> dwEffSize;
				for( DWORD j=0; j<dwEffSize; ++j )
				{
					SFile >> dwTypeID;

					// Note : 만들어야지.
					pTexEffBase = DxTexEffMan::GetInstance().AdaptToDxFrame( dwTypeID, pd3dDevice, NULL );

					// Note : Load
					if ( pTexEffBase->LoadPSF( pd3dDevice, SFile ) )
					{
						// Note : TextureEff 삽입
						InsertTextureEff( m_mapTexEff, strName.c_str(), pTexEffBase );
					}
					else
					{
						SAFE_DELETE ( pTexEffBase );
					}
				}
			}
		}
	}
	else if( dwVer==0x101 )
	{
		// Note : TexList
		{
			DWORD dwSize;
			SFile >> dwSize;

			BOOL            bTemp;
			DWORD			dwFlag;
			std::string		strName;
			MAPTEXLIST_ITER iter;
			for( DWORD i=0; i<dwSize; ++i )
			{
				SFile >> strName;
				SFile >> dwFlag;
				SFile >> bTemp;

				if ( dwFlag == 0 )
				{
					dwFlag = TEXEFF_DEFAULT;
				}

				DWORD dwPieceLM_Option(0L);
				if ( bTemp )
				{
					dwPieceLM_Option |= EMPLO_DONT_LM_SHADOW_RECEIVE;
				}

				iter = m_mapTexList.find( strName.c_str() );
				if( iter!=m_mapTexList.end() )	
				{
					(*iter).second.m_dwTexType = dwFlag;
					(*iter).second.m_dwPieceLM_Option = dwPieceLM_Option;
				}
				else if ( bPiece )
				{
					TextureData sTextureData(dwFlag,dwPieceLM_Option);
					std::transform ( strName.begin(), strName.end(), strName.begin(), tolower );
					m_mapTexList.insert( std::make_pair( strName, sTextureData ) );
				}
			}
		}


		// Note : Texture Effect List
		{
			DWORD dwSize, dwEffSize;
			SFile >> dwSize;

			DxTexEffBase*	pTexEffBase;
			DWORD			dwTypeID;
			std::string		strName;
			for( DWORD i=0; i<dwSize; ++i )
			{
				SFile >> strName;

				SFile >> dwEffSize;
				for( DWORD j=0; j<dwEffSize; ++j )
				{
					SFile >> dwTypeID;

					// Note : 만들어야지.
					pTexEffBase = DxTexEffMan::GetInstance().AdaptToDxFrame( dwTypeID, pd3dDevice, NULL );

					// Note : Load
					if ( pTexEffBase->LoadPSF( pd3dDevice, SFile ) )
					{
						// Note : TextureEff 삽입
						InsertTextureEff( m_mapTexEff, strName.c_str(), pTexEffBase );
					}
					else
					{
						SAFE_DELETE ( pTexEffBase );
					}
				}
			}
		}
	}
	else
	{
		SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
	}
}

void DxTextureEffMan::LoadPSF_VER100( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
	// Note : TexList
	{
		DWORD dwSize;
		SFile >> dwSize;

		DWORD			dwFlag;
		std::string		strName;
		MAPTEXLIST_ITER iter;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;
			SFile >> dwFlag;

			if ( dwFlag == 0 )
			{
				dwFlag = TEXEFF_DEFAULT;
			}

			iter = m_mapTexList.find( strName.c_str() );
			if( iter!=m_mapTexList.end() )	
                (*iter).second.m_dwTexType = dwFlag;
		}
	}


	// Note : Texture Effect List
	{
		DWORD dwSize, dwEffSize;
		SFile >> dwSize;

		DxTexEffBase*	pTexEffBase;
		DWORD			dwTypeID;
		std::string		strName;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;

			SFile >> dwEffSize;
			for( DWORD j=0; j<dwEffSize; ++j )
			{
				SFile >> dwTypeID;

				// Note : 만들어야지.
				pTexEffBase = DxTexEffMan::GetInstance().AdaptToDxFrame( dwTypeID, pd3dDevice, NULL );

				// Note : Load
			    if ( pTexEffBase->LoadPSF( pd3dDevice, SFile ) )
				{
					// Note : TextureEff 삽입
					InsertTextureEff( m_mapTexEff, strName.c_str(), pTexEffBase );
				}
				else
				{
					SAFE_DELETE ( pTexEffBase );
				}
			}
		}
	}
}

void DxTextureEffMan::LoadPSF_PREV_PIECE( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
    // Note : Texture Effect List
	{
		DWORD dwSize, dwEffSize;
		SFile >> dwSize;

		DxTexEffBase*	pTexEffBase;
		DWORD			dwTypeID;
		std::string		strName;
		for( DWORD i=0; i<dwSize; ++i )
		{
			SFile >> strName;

			SFile >> dwEffSize;
			for( DWORD j=0; j<dwEffSize; ++j )
			{
				SFile >> dwTypeID;

				// Note : 만들어야지.
				pTexEffBase = DxTexEffMan::GetInstance().AdaptToDxFrame( dwTypeID, pd3dDevice, NULL );

				// Note : Load
			    if ( pTexEffBase->LoadPSF( pd3dDevice, SFile ) )
				{
					// Note : TextureEff 삽입
					InsertTextureEff( m_mapTexEff, strName.c_str(), pTexEffBase );

					// Note : 데이터가 있다는 것을 셋팅함.
					TextureData sTextureData(TEXEFF_DEFAULT,FALSE);
					std::transform ( strName.begin(), strName.end(), strName.begin(), tolower );
					m_mapTexList.insert( std::make_pair( strName, sTextureData ) );
				}
				else
				{
					SAFE_DELETE ( pTexEffBase );
				}
			}
		}
	}
}

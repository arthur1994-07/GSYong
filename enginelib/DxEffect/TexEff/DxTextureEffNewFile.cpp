#include "pch.h"

#include "./DxTexEffMan.h"
#include "./DxTexEff.h"
#include "../../Archive/Archive.h"

#include "DxTextureEffNewFile.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxTextureEffNewFile::DxTextureEffNewFile()
{
}

DxTextureEffNewFile::~DxTextureEffNewFile()
{
    CleanUp();
}

void DxTextureEffNewFile::CleanUp()
{
    m_mapTexEff.clear();
}

void DxTextureEffNewFile::Insert( const TSTRING& strName, const VEC_TEXEFF& vecTexEff )
{
    m_mapTexEff.insert( std::make_pair( strName, vecTexEff ) );
}

const MAP_VECTEXEFF& DxTextureEffNewFile::GetTexEff() const
{
    return m_mapTexEff;
}

void DxTextureEffNewFile::Save( const TSTRING& strName, boost::function<void (const TCHAR*)> pArchive )
{
    std::string strNameNew = strName;

	std::string::size_type idx;
	idx = strNameNew.find( ".wld" );
	if( idx!=std::string::npos )	strNameNew += '1';		// wld1 로 만듬.

    SPSerialFile spFile;

    if (Archive::CreateSerialFile(pArchive, spFile) == false)
    {
        return ;
    }

    sc::SerialFile& SFile = *spFile;

	if ( SFile.OpenFile ( FOT_WRITE, strNameNew.c_str() ) )	
        Save( SFile );

	SFile.CloseFile();
}

BOOL DxTextureEffNewFile::Load( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strName )
{
    std::string strNameNew = strName;

	std::string::size_type idx;
	idx = strNameNew.find( ".wld" );
	if( idx!=std::string::npos )	
        strNameNew += '1';		// wld1 로 만듬.

    sc::SerialFile SFile;
	if ( SFile.OpenFile ( FOT_READ, strNameNew.c_str() ) )	
    {
        Load( pd3dDevice, SFile );
        SFile.CloseFile();
        return TRUE;
    }

	SFile.CloseFile();
    return FALSE;
}

void DxTextureEffNewFile::Save( sc::SerialFile& SFile )
{
    SFile << VERSION;
    SFile.BeginBlock();
    {
        DWORD dwTexSize = static_cast<DWORD> (m_mapTexEff.size());
        SFile << dwTexSize;

        MAP_VECTEXEFF_ITER iter = m_mapTexEff.begin();
        for ( ; iter!=m_mapTexEff.end(); ++iter )
        {
            // 텍스쳐 이름
            SFile << (*iter).first;

            // Effect 갯수
            const VEC_TEXEFF& vecTexEff = (*iter).second;
            DWORD dwSize = static_cast<DWORD> (vecTexEff.size());
            SFile << dwSize;

            // Effect 저장
            for ( DWORD i=0; i<dwSize; ++i )
            {
                SFile << vecTexEff[i]->GetTypeID();
				vecTexEff[i]->SavePSF( SFile );
            }
        }
    }
    SFile.EndBlock();
}

void DxTextureEffNewFile::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile )
{
    CleanUp();

    DWORD dwVer, dwBufferSize;
    SFile >> dwVer;
    SFile >> dwBufferSize;

    DWORD dwTexSize(0);
    SFile >> dwTexSize;

    TSTRING strName;
    DWORD dwSize(0);
    DWORD dwTypeID(0);
    DxTexEffBase* pTexEffBase(NULL);
    VEC_TEXEFF vecTexEff;
    for ( DWORD i=0; i<dwTexSize; ++i )
    {
        // 삭제
        vecTexEff.clear();

        // 텍스쳐 이름
        SFile >> strName;

        // 텍스쳐 Effect 갯수
        SFile >> dwSize;
        for ( DWORD i=0; i<dwSize; ++i )
        {
            SFile >> dwTypeID;

			// Note : 만들어야지.
			pTexEffBase = DxTexEffMan::GetInstance().AdaptToDxFrame( dwTypeID, pd3dDevice, NULL );

			// Note : Load
			if ( pTexEffBase->LoadPSF( pd3dDevice, SFile ) )
			{
				// 삽입
				std::tr1::shared_ptr<DxTexEffBase> spNew = std::tr1::shared_ptr<DxTexEffBase>( pTexEffBase );
				vecTexEff.push_back( spNew );
			}
			else
			{
				SAFE_DELETE ( pTexEffBase );
			}
        }

        // Note : TextureEff 삽입
        Insert( strName, vecTexEff );
    }
}
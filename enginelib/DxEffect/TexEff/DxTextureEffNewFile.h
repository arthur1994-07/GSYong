#pragma once

#include "../../DxLand/DxLandDef.h"

class DxTextureEffNewFile
{
private:
    enum
    {
        VERSION = 0x100,
    };

private:
    MAP_VECTEXEFF m_mapTexEff;

public:
    void Insert( const TSTRING& strName, const VEC_TEXEFF& vecTexEff );
    const MAP_VECTEXEFF& GetTexEff() const;

    void Save( const TSTRING& strName, boost::function<void (const TCHAR*)> pArchive = 0 );
    BOOL Load( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strName );

private:
    void Save( sc::SerialFile& SFile );
    void Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile );

    void CleanUp();

public:
    DxTextureEffNewFile();
    ~DxTextureEffNewFile();
};
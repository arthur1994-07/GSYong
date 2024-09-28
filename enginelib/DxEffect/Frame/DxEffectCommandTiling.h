#pragma once

#include <boost/tr1/memory.hpp>

#include "./DxEffectTiling.h"

struct DXMATERIAL;
class DxEffectTiling;


//-------------------------------------------------------------------------
// 부모클래스
class CommandTiling
{
public:
    // Undo
    virtual void Undo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling ) = 0;

    // Redo
    virtual void Redo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling ) = 0;

public:
    CommandTiling();
    virtual ~CommandTiling();
};

//-------------------------------------------------------------------------
// Composite Command - 다중으로 가질 수 있도록 함.
class CommandTiling_Composite : public CommandTiling
{
private:
    std::vector<std::tr1::shared_ptr<CommandTiling>>   m_vecspCommand;

// 작업된다.
public:
    void Insert( std::tr1::shared_ptr<CommandTiling> spCommand );

public:
    // Undo
    virtual void Undo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling );

    // Redo
    virtual void Redo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling );

public:
    CommandTiling_Composite();
    virtual ~CommandTiling_Composite();
};

//-------------------------------------------------------------------------
// 매터리얼을 변경한다.
class CommandTiling_MaterialChange : public CommandTiling
{
private:
	DWORD			m_dwPointEX;
    DxEffectTiling::MATERIALMAP	m_mapMaterials_Prev;    // 이전
	DxEffectTiling::MATERIALMAP m_mapMaterials;         // 현재

public:
    // New
    void SetMaterial( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling::MATERIALMAP* pmapMaterial );

private:
    // New
    void SetMaterial( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling::MATERIALMAP* pmapMaterialDest, DxEffectTiling::MATERIALMAP* pmapMaterialSrc );

public:
    // Undo
    virtual void Undo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling );

    // Redo
    virtual void Redo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling );

public:
    CommandTiling_MaterialChange( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling::MATERIALMAP* pmapMaterial, DWORD dwPointEX );
    virtual ~CommandTiling_MaterialChange();
};

//-------------------------------------------------------------------------
// 타일에 그림을 변경한다.
class CommandTiling_TileChange : public CommandTiling
{
private:
    DWORD m_dwSize;
	DxEffectTiling::MATERIALMAP m_mapMaterials_Prev;	// 이전
	DxEffectTiling::MATERIALMAP m_mapMaterials_Now;		// 현재

public:
    // Undo
    virtual void Undo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling );

    // Redo
    virtual void Redo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling );

public:
    CommandTiling_TileChange( DWORD dwSize, DxEffectTiling::MATERIALMAP* pmapMaterials_Prev, DxEffectTiling::MATERIALMAP* pmapMaterials_Now );
    virtual ~CommandTiling_TileChange();
};

//-------------------------------------------------------------------------
// 브러쉬 타일을 변경한다.
class CommandTiling_SelectMaterial : public CommandTiling
{
private:
    int m_nSelectMaterial_Prev; // 이전
    int m_nSelectMaterial;      // 현재

public:
    // New
    void SelectMaterial( int nSelectMaterial );

public:
    // Undo
    virtual void Undo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling );

    // Redo
    virtual void Redo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling );

public:
    CommandTiling_SelectMaterial( int nSelectMaterial_Prev );
    virtual ~CommandTiling_SelectMaterial();
};


//-------------------------------------------------------------------------
// Command 관리 클래스
class CommandTilingManager
{
private:
    int m_nIndex;
    std::vector<std::tr1::shared_ptr<CommandTiling>>   m_vecspCommand;

public:
    // 작업된다.
    void Insert( std::tr1::shared_ptr<CommandTiling> spCommand );

    // Undo
    void Undo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling );

    // Redo
    void Redo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling );

    // 정리
    void CleanUp();

public:
    CommandTilingManager();
    ~CommandTilingManager();
};
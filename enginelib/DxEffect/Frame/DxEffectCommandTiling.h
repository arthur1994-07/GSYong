#pragma once

#include <boost/tr1/memory.hpp>

#include "./DxEffectTiling.h"

struct DXMATERIAL;
class DxEffectTiling;


//-------------------------------------------------------------------------
// �θ�Ŭ����
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
// Composite Command - �������� ���� �� �ֵ��� ��.
class CommandTiling_Composite : public CommandTiling
{
private:
    std::vector<std::tr1::shared_ptr<CommandTiling>>   m_vecspCommand;

// �۾��ȴ�.
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
// ���͸����� �����Ѵ�.
class CommandTiling_MaterialChange : public CommandTiling
{
private:
	DWORD			m_dwPointEX;
    DxEffectTiling::MATERIALMAP	m_mapMaterials_Prev;    // ����
	DxEffectTiling::MATERIALMAP m_mapMaterials;         // ����

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
// Ÿ�Ͽ� �׸��� �����Ѵ�.
class CommandTiling_TileChange : public CommandTiling
{
private:
    DWORD m_dwSize;
	DxEffectTiling::MATERIALMAP m_mapMaterials_Prev;	// ����
	DxEffectTiling::MATERIALMAP m_mapMaterials_Now;		// ����

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
// �귯�� Ÿ���� �����Ѵ�.
class CommandTiling_SelectMaterial : public CommandTiling
{
private:
    int m_nSelectMaterial_Prev; // ����
    int m_nSelectMaterial;      // ����

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
// Command ���� Ŭ����
class CommandTilingManager
{
private:
    int m_nIndex;
    std::vector<std::tr1::shared_ptr<CommandTiling>>   m_vecspCommand;

public:
    // �۾��ȴ�.
    void Insert( std::tr1::shared_ptr<CommandTiling> spCommand );

    // Undo
    void Undo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling );

    // Redo
    void Redo( LPDIRECT3DDEVICEQ pd3dDevice, DxEffectTiling* pEffectTiling );

    // ����
    void CleanUp();

public:
    CommandTilingManager();
    ~CommandTilingManager();
};
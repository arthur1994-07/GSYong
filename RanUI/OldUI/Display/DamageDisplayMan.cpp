#include "StdAfx.h"
#include "DamageDisplayMan.h"
#include "DamageDisplay.h"
#include "../../../EngineLib/DxTools/TextureManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CDamageDisplayMan::CDamageDisplayMan (EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_nStartIndex(0)
{
}

CDamageDisplayMan::~CDamageDisplayMan ()
{
}

HRESULT CDamageDisplayMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_strNumberTEX = "number.dds";

    m_textureRes = TextureManager::GetInstance().LoadTexture(
        m_strNumberTEX,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        1,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_UI,
		false,
		FALSE );

	return CUIGroup::InitDeviceObjects ( pd3dDevice );
}

HRESULT CDamageDisplayMan::DeleteDeviceObjects ()
{
	m_textureRes.Clear();

	return CUIGroup::DeleteDeviceObjects ();
}

void CDamageDisplayMan::CreateSubControl ()
{
	for ( int i = 0; i < nMAX_DAMAGE_DISPLAY; i++ )
	{
		CDamageDisplay* pDamageDisplay = new CDamageDisplay(m_pEngineDevice);
		pDamageDisplay->CreateSub ( this, "DAMAGE_DISPLAY" );
		pDamageDisplay->CreateSubControl ();
		pDamageDisplay->SetVisibleSingle ( FALSE );
		RegisterControl ( pDamageDisplay );
		m_pDamageDisplay[i] = pDamageDisplay;
	}
}

void CDamageDisplayMan::SetDamage ( int nPosX, int nPosY, int nDamage, DWORD dwDamageFlag, BOOL bAttack )
{
	CDamageDisplay* pDamageDisplay = m_pDamageDisplay[m_nStartIndex];
	pDamageDisplay->SetVisibleSingle ( TRUE );
	pDamageDisplay->SetDamage ( nDamage, dwDamageFlag, bAttack );
	pDamageDisplay->SetGlobalPos ( D3DXVECTOR2 ( float(nPosX), float(nPosY) ) );

	m_nStartIndex++;
	if ( nMAX_DAMAGE_DISPLAY <= m_nStartIndex )
	{
		m_nStartIndex = 0;
	}
}
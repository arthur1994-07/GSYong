#include "pch.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../enginelib/DxMeshs/SkinMesh/DxSkinChar.h"
#include "../enginelib/DxEffect/Char/DxEffCharData.h"
#include "../enginelib/DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../enginelib/DxTools/RENDERPARAM.h"


#include "SkinCharMulti.h"

SkinCharMulti::SkinCharMulti()
: m_bIsUseSkinCharMulti( FALSE )
{
}

SkinCharMulti::SkinCharMulti( DxSkinCharData* pCharData, LPDIRECT3DDEVICEQ pd3dDevice, bool bAmbientUP, DWORD dwFlags )
: m_pCharData( pCharData )
, m_pd3dDevice( pd3dDevice )
, m_bAmbientUP( bAmbientUP )
, m_dwFlags( dwFlags )
, m_nSkinCount( 0 )
{
	m_vecSkinCharMulti.clear();
}

SkinCharMulti::~SkinCharMulti()
{
	ResetSkinCharMulti();
}

HRESULT SkinCharMulti::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_vecSkinCharMulti.size() )
	{
		BOOST_FOREACH(DxSkinChar* pSkinCharTest, m_vecSkinCharMulti)
		{
			pSkinCharTest->InitDeviceObjects( pd3dDevice );
		}
		return S_OK;
	}
	return S_FALSE;
}

HRESULT SkinCharMulti::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_vecSkinCharMulti.size() )
	{
		BOOST_FOREACH(DxSkinChar* pSkinCharTest, m_vecSkinCharMulti)
		{
			pSkinCharTest->RestoreDeviceObjects( pd3dDevice );
		}
		return S_OK;
	}
	return S_FALSE;

}

HRESULT SkinCharMulti::InvalidateDeviceObjects()
{
	if ( m_vecSkinCharMulti.size() )
	{
		BOOST_FOREACH(DxSkinChar* pSkinCharTest, m_vecSkinCharMulti)
		{
			pSkinCharTest->InvalidateDeviceObjects();
		}
		return S_OK;
	}
	return S_FALSE;
}

HRESULT SkinCharMulti::DeleteDeviceObjects()
{
	if ( m_vecSkinCharMulti.size() )
	{
		BOOST_FOREACH(DxSkinChar* pSkinCharTest, m_vecSkinCharMulti)
		{
			pSkinCharTest->DeleteDeviceObjects();
		}
		return S_OK;
	}
	return S_FALSE;
}

BOOL SkinCharMulti::PutPassiveEffect( const char* szEffFile, const D3DXVECTOR3 *pDir, float fTarDir/*=0.0f */ )
{
	BOOST_FOREACH(DxSkinChar* pSkinCharTest, m_vecSkinCharMulti)
	{
		DxEffcharDataMan::GetInstance().PutPassiveEffect ( pSkinCharTest, szEffFile, pDir, fTarDir );
	}
	return TRUE;
}

void SkinCharMulti::RenderMyChar( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bOpaqueState, BOOL bHALF_VISIBLE, const D3DXVECTOR3 &vDiffuse, const D3DXVECTOR3 &vPointLight, BOOL bMy )
{
	if ( !m_bIsUseSkinCharMulti )
		return;

	if ( !IsUpdateSuit() )
		return;

	BOOST_FOREACH(DxSkinChar* pRenderChar, m_vecSkinCharMulti)
	{
		if ( !pRenderChar )
			return;

		NSMaterialSkinManager::SetMob( FALSE );
		pRenderChar->SetWorldLight( vDiffuse, vPointLight );

		if ( bHALF_VISIBLE )	// 캐릭터 반투명 렌더
		{
			if ( bOpaqueState )
			{
				// 아무것도 하지 않는다.
			}
			else
			{
				if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
				{
					pRenderChar->RenderFixed( pd3dDevice, TRUE, 0.5f );
				}
				else
				{
					pRenderChar->SetPhysX_Cloth( RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_ALL ? TRUE : RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_MY ? bMy ? TRUE : FALSE : FALSE );
					pRenderChar->RenderMaterialOpaque ( pd3dDevice, 0.5f  );
					pRenderChar->RenderMaterialHardAlpha ( pd3dDevice, 0.5f );
					pRenderChar->RenderMaterialSoftAlpha ( pd3dDevice, 0.5f );
				}
			}		
		}
		else		// 정상렌더
		{
			if ( bOpaqueState )
			{
				if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
				{
					pRenderChar->RenderFixed( pd3dDevice, TRUE, 1.f  );
				}
				else
				{
					pRenderChar->SetPhysX_Cloth( RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_ALL ? TRUE : RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_MY ? bMy ? TRUE : FALSE : FALSE );
					pRenderChar->RenderMaterialOpaque ( pd3dDevice, 1.f  );
					pRenderChar->RenderMaterialHardAlpha ( pd3dDevice, 1.f );
				}
			}
			else
			{
				if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
				{
				}
				else
				{
					pRenderChar->RenderMaterialSoftAlpha ( pd3dDevice, 1.f );
				}
			}
		}
	}
}

BOOL SkinCharMulti::OutEffect( std::string strEffFile )
{
	if ( m_vecSkinCharMulti.size() )
	{
		BOOST_FOREACH(DxSkinChar* pRenderChar, m_vecSkinCharMulti)
		{
			DxEffcharDataMan::GetInstance().OutEffect ( pRenderChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str() );
		}
		return TRUE;
	}

	return FALSE;
}

//void SkinCharMulti::CalculateAmbientColor( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan, const D3DXVECTOR3& vPos )
//{
//	//BOOST_FOREACH(DxSkinChar* pRenderChar, m_vecSkinCharMulti)
//	//{
//	//	pRenderChar->CalculateAmbientColor( pd3dDevice, pLandMan, vPos );
//	//}
//}

void SkinCharMulti::SELECTSKILLANI( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType/*=AN_SUB_NONE */ )
{
	if ( !m_bIsUseSkinCharMulti )
		return;

	BOOST_FOREACH(DxSkinChar* pRenderChar, m_vecSkinCharMulti)
	{
		pRenderChar->SELECTSKILLANI ( MType, SType );
	}
}

BOOL SkinCharMulti::IsANI_SELECTANI( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType/*=AN_SUB_NONE*/, DWORD dwFlags/*=NULL*/, DWORD dwSelect/*=0 */ )
{
	BOOST_FOREACH(DxSkinChar* pRenderChar, m_vecSkinCharMulti)
	{
		pRenderChar->IsANI_SELECTANI ( MType, SType, dwFlags, dwSelect );
	}
	return TRUE;
}

BOOL SkinCharMulti::IsANI_SELECTANI( const TCHAR* szAnim, DWORD dwFlags/*=NULL */ )
{

	BOOST_FOREACH(DxSkinChar* pRenderChar, m_vecSkinCharMulti)
	{
		pRenderChar->IsANI_SELECTANI ( szAnim, dwFlags );
	}
	return TRUE;
}

void SkinCharMulti::SetPosition( const D3DXVECTOR3& vPos )
{
	BOOST_FOREACH(DxSkinChar* pRenderChar, m_vecSkinCharMulti)
	{
		pRenderChar->SetPosition ( vPos );
	}
}

HRESULT SkinCharMulti::FrameMove( const float fTime, const float fElapsedTime, BOOL bNoneOverFrame, const D3DXMATRIX &matRot, const BOOL bContinue/*=TRUE*/, BOOL const bFreeze/*=FALSE*/, const BOOL bAttackMode/*=TRUE*/, const BOOL bBoardRender /*= FALSE */ )
{
	if ( !m_bIsUseSkinCharMulti )
		return TRUE;

	if ( !IsUpdateSuit() )
		return TRUE;

	std::vector<D3DXVECTOR3> vecPosition = SkinCharMultiData::GetInstance().GetVecPosition();

	for (unsigned i = 0; i < m_vecSkinCharMulti.size(); i++ )
	{
		D3DXMATRIX matAddPos;
		ZeroMemory(&matAddPos, sizeof(D3DXMATRIX));
		if ( i < vecPosition.size() )
		{
			matAddPos = D3DXmatrix(vecPosition[i]);
		}
		m_vecSkinCharMulti[i]->FrameMove ( fTime,
										   fElapsedTime, 
										   bNoneOverFrame, 
										   matRot + matAddPos, 
										   bContinue, 
										   bFreeze, 
										   bAttackMode, 
										   bBoardRender );
	}
	return S_OK;
}

void SkinCharMulti::ClearReserveAction()
{
	for (unsigned i = 0; i < m_vecSkinCharMulti.size(); i++ )
	{
		m_vecSkinCharMulti[i]->ClearReserveAction();
	}
}

void SkinCharMulti::ResetSkinCharMulti()
{
	BOOST_FOREACH( DxSkinChar* pRenderChar, m_vecSkinCharMulti)
	{
		SAFE_DELETE(pRenderChar);
	}
	m_vecSkinCharMulti.clear();
}

void SkinCharMulti::SetCharData( DxSkinCharData* pCharData, LPDIRECT3DDEVICEQ pd3dDevice, bool bAmbientUP, DWORD dwFlags )
{
	for ( unsigned i = 0; i < 7; i++ )
	{
		DxSkinChar* pRenderChar = new DxSkinChar;
		pRenderChar->SetCharData(pCharData, pd3dDevice, bAmbientUP, dwFlags );
		m_vecSkinCharMulti.push_back(pRenderChar);
	}
}

void SkinCharMulti::SetHairColor( WORD wColor )
{
	BOOST_FOREACH(DxSkinChar* pRenderChar, m_vecSkinCharMulti)
	{
		pRenderChar->SetHairColor( wColor );
	}
}

void SkinCharMulti::ResetPiece( int i )
{
	if ( m_vecSkinCharMulti.size() )
	{
		BOOST_FOREACH(DxSkinChar* pRenderChar, m_vecSkinCharMulti)
		{
			pRenderChar->ResetPiece( EMPIECECHAR( i ) );
		}
	}
}

HRESULT SkinCharMulti::SetPiece( const TCHAR* szFile, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFlag, int nLevel, EMSCD_FLAGS emFlags, BOOL bAttachSub/*=FALSE*/, EMPIECECHAR* pOutPiecetype/*=NULL*/, const char* pSelfBodyEffect/*=NULL */ )
{
	HRESULT hr = S_OK;

	BOOST_FOREACH(DxSkinChar* pRenderChar, m_vecSkinCharMulti)
	{
		hr = pRenderChar->SetPiece( szFile, pd3dDevice, dwFlag, nLevel, emFlags, bAttachSub, pOutPiecetype, pSelfBodyEffect );
		if ( E_FAIL == hr )
			return hr;
	}

	return hr;
}

HRESULT SkinCharMulti::SetPiece(EMPIECECHAR emPieceChar,
								const std::string& strFileChange,
								LPDIRECT3DDEVICEQ pd3dDevice,
								DWORD dwFlag,
								int nLevel,
								EMSCD_FLAGS emFlags,
								BOOL bAttachSub/*=FALSE*/,
								EMPIECECHAR* pOutPiecetype/*=NULL*/,
								const char* pSelfBodyEffect/*=NULL */ )
{
	HRESULT hr = S_OK;

	BOOST_FOREACH(DxSkinChar* pRenderChar, m_vecSkinCharMulti)
	{
		PDXCHARPART pCharPart = pRenderChar->GetPiece(emPieceChar);
		if (pCharPart && strcmp( pCharPart->GetFileName(), strFileChange.c_str() ))
		hr = pRenderChar->SetPiece( strFileChange, pd3dDevice, dwFlag, nLevel, emFlags, bAttachSub, pOutPiecetype, pSelfBodyEffect );
	}

	return hr;
}

void SkinCharMulti::SELECTANI( const TCHAR* szAnim, DWORD dwFlags/*=NULL*/ )
{
	if ( !m_bIsUseSkinCharMulti )
		return;

	BOOST_FOREACH(DxSkinChar* pRenderChar, m_vecSkinCharMulti)
	{
		pRenderChar->SELECTANI ( szAnim, dwFlags );
	}
}

void SkinCharMulti::SELECTANI( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType/*=AN_SUB_NONE*/, DWORD dwFlags/*=NULL*/, DWORD dwSelect/*=0*/ )
{
	if ( !m_bIsUseSkinCharMulti )
		return;

	BOOST_FOREACH(DxSkinChar* pRenderChar, m_vecSkinCharMulti)
	{
		pRenderChar->SELECTANI ( MType, SType, dwFlags, dwSelect );
	}
}

void SkinCharMulti::DEFAULTANI()
{
	BOOST_FOREACH(DxSkinChar* pRenderChar, m_vecSkinCharMulti)
	{
		pRenderChar->DEFAULTANI();
	}
}

void SkinCharMulti::TOSTARTTIME()
{
	if ( m_vecSkinCharMulti. size() )
	{
		BOOST_FOREACH(DxSkinChar* pRenderChar, m_vecSkinCharMulti)
		{
			pRenderChar->TOSTARTTIME();
		}
	}
}

void SkinCharMulti::TOENDTIME()
{
	BOOST_FOREACH(DxSkinChar* pRenderChar, m_vecSkinCharMulti)
	{
		pRenderChar->TOENDTIME();
	}
}

void SkinCharMulti::SetPartRend( EMPIECECHAR emPIECE, BOOL bRender )
{
	if ( !m_bIsUseSkinCharMulti )
		return;

	BOOST_FOREACH(DxSkinChar* pRenderChar, m_vecSkinCharMulti)
	{
		pRenderChar->SetPartRend( emPIECE, bRender );
	}
}

void SkinCharMulti::DOSHOCKMIX()
{
	BOOST_FOREACH(DxSkinChar* pRenderChar, m_vecSkinCharMulti)
	{
		pRenderChar->DOSHOCKMIX();
	}
}

BOOL SkinCharMulti::SetUseSkinCharMulti( BOOL bUse, int nNum )
{
	m_bIsUseSkinCharMulti = bUse;
	SetSkinCount(nNum);
	if ( m_bIsUseSkinCharMulti && m_vecSkinCharMulti.size() == 0 )
	{
		//SkinCharMultiData::GetInstance().LoadFile("ClonSkillPos.lua");
	}

	if ( m_bIsUseSkinCharMulti && m_vecSkinCharMulti.size() != nNum )
	{
		DxSkinChar* pRenderChar = new DxSkinChar;
		pRenderChar->InvalidateDeviceObjects();
		pRenderChar->SetCharData(m_pCharData, m_pd3dDevice, m_bAmbientUP, m_dwFlags );
		m_vecSkinCharMulti.push_back(pRenderChar);
		return TRUE;
	}
	
	if ( !m_bIsUseSkinCharMulti )
	{
		ResetSkinCharMulti();
		DeleteDeviceObjects();
		ResetSkinCount();
	}
	
	return FALSE;
}

D3DXMATRIX SkinCharMulti::D3DXmatrix( D3DXVECTOR3 vecPos )
{
	return D3DXMATRIX(0,0,0,0,
					  0,0,0,0,
					  0,0,0,0,
					  vecPos.x, vecPos.y, vecPos.z,		0);
}

void SkinCharMulti::SetSkinCount( int nCount )
{
	m_nSkinCount = nCount;
}

void SkinCharMulti::ResetSkinCount()
{
	m_nSkinCount = 0;
}

BOOL SkinCharMulti::IsUpdateSuit()
{
	return m_nSkinCount == static_cast<int>(m_vecSkinCharMulti.size());
}
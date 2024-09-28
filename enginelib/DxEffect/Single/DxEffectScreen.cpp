#include "pch.h"

#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/EditMeshs.h"

#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxEffect/Single/DxEffSingleMan.h"
#include "../../DxEffect/EffectLua/NSEffectLuaPlay.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "./DxEffectScreen.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//	Note	:	디버그용,
extern BOOL			g_bEFFtoTEX;
extern BOOL			g_bEffectAABBBox;

//---------------------------------------------------------------------[SCREEN_PROPERTY]
const DWORD	SCREEN_PROPERTY::VERSION	= 0x0100;
const char	SCREEN_PROPERTY::NAME[]	= "EffectLua Play";

DxEffSingle* SCREEN_PROPERTY::NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup )
{
	DxEffectScreen *pEffect = new DxEffectScreen;

	//	Note : TransPorm Set.		[공통설정]3
	pEffect->m_matLocal = m_matLocal;

	//	Note : 시간 설정.			[공통설정]
	pEffect->m_fGBeginTime = m_fGBeginTime;
	pEffect->m_fGLifeTime = m_fGLifeTime;

	//	Note : 날아가는 오브젝트	[공통설정]
	pEffect->m_bMoveObj = m_bMoveObj;

	pEffect->m_strEffectName = m_szEffectName;

	// DxEffSingle 에 있는 m_textureRes 이 로딩되었다 사라졌고,
	// EFF_PROPERTY 에 있는 m_textureRes 는 로딩되어 있지 않을 경우.
	if ( !m_textureRes.Update() )
	{
		m_textureRes = TextureManager::GetInstance().LoadTexture(
			m_szEffectName,
			false,
			TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
			0,
			D3DFMT_UNKNOWN,
			TEXTURE_RESIZE_NONE,
			TEXTURE_EFFECT,
			true,
			m_bBackUpCache );
	}

	pEffect->m_textureRes = m_textureRes;


	return pEffect;
}

HRESULT SCREEN_PROPERTY::SaveFile ( sc::SerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	//
	SFile << (DWORD) ( GetSizeBase() + sizeof(m_Property) );

	//	Note : 부모 클레스의 정보.
	SFile.WriteBuffer( m_matLocal, sizeof(D3DXMATRIX) );

	SFile << m_bMoveObj;
	SFile << m_fGBeginTime;
	SFile << m_fGLifeTime;

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(PROPERTY) );

	//	Note : 형제, 자식 저장.
	//
	EFF_PROPERTY::SaveFile ( SFile );

	return S_OK;
}

HRESULT SCREEN_PROPERTY::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == VERSION )
	{
		//	Note : 부모 클레스의 정보.
		SFile.ReadBuffer ( m_matLocal, sizeof(D3DXMATRIX) );

		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );

		m_textureRes.Clear();
	}
	else
	{
		//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
		//
		DWORD dwCur = SFile.GetfTell ();
		SFile.SetOffSet ( dwCur+dwSize );
	}

	//	Note : Device 자원을 생성한다.
	//
	hr = Create ( pd3dDevice, bBackUpCache );
	if ( FAILED(hr) )	return hr;

	//	Note : 형제, 자식 읽기.
	//
	EFF_PROPERTY::LoadFile ( SFile, pd3dDevice, bBackUpCache, bOldDecalUSE );

	return S_OK;
}

HRESULT SCREEN_PROPERTY::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache )
{
	m_bBackUpCache = bBackUpCache;
	m_textureRes = TextureManager::GetInstance().LoadTexture(
		m_szEffectName,
		false,
		TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
		0,
		D3DFMT_UNKNOWN,
		TEXTURE_RESIZE_NONE,
		TEXTURE_EFFECT,
		true,
		bBackUpCache );

	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice, bBackUpCache );

	return S_OK;
}

HRESULT SCREEN_PROPERTY::DeleteDeviceObjects ()
{
	m_textureRes.Clear();

	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::DeleteDeviceObjects ();

	return S_OK;
}

const DWORD	DxEffectScreen::TYPEID		= EFFSINGLE_SCREENEFFECT;
const DWORD	DxEffectScreen::FLAG		= NULL;
const char	DxEffectScreen::NAME[]		= "EffectLua Play";

//	Note :
//
DxEffectScreen::DxEffectScreen(void) :
	m_pd3dDevice(NULL)
{
}

DxEffectScreen::~DxEffectScreen(void)
{

}

void DxEffectScreen::CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	if( m_pChild )		m_pChild->CheckAABBBox( vMax, vMin );
	if( m_pSibling )	m_pSibling->CheckAABBBox( vMax, vMin );
}

HRESULT DxEffectScreen::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXMATRIX matTrans;
	D3DXMatrixIdentity ( &matTrans );

	STARGETID	TargetID;
	TargetID.GaeaId	= 2;
	TargetID.emCrow	= CROW_MOB;
	TargetID.vPos	= D3DXVECTOR3( 0.f, 0.f, 0.f );

	NSEffectPlay::StartPlay( pd3dDevice, m_strEffectName.c_str(), matTrans, TargetID );

	//	Note : 형제, 자식 호출
	//
	DxEffSingle::InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectScreen::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	//	Note : 형제, 자식 호출
	//
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectScreen::InvalidateDeviceObjects ()
{
	//	Note : 형제, 자식 호출
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

HRESULT DxEffectScreen::DeleteDeviceObjects ()
{
	NSEffectPlay::Delete( m_strEffectName.c_str() );

	//	Note : 형제, 자식 호출
	//
	DxEffSingle::DeleteDeviceObjects ();

	return S_OK;
}

void DxEffectScreen::ReStartEff ()
{
	m_fGAge			= 0.f;
	m_dwRunFlag		&= ~EFF_PLY_PLAY;
	m_dwRunFlag		&= ~EFF_PLY_DONE;
	m_dwRunFlag		&= ~EFF_PLY_END;

	if ( m_pChild )		m_pChild->ReStartEff ();
	if ( m_pSibling )	m_pSibling->ReStartEff ();
}

HRESULT DxEffectScreen::FrameMove ( float fTime, float fElapsedTime )
{
	//	Note : Eff의 나이를 계산.
	//
	m_fGAge += fElapsedTime;

	//	Note : 반복할 때 시간 및 여러 값을 초기화 한다.
	//
	if ( m_rThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_rThisGroup->m_fGBeginTime+m_rThisGroup->m_fGLifeTime) )
		{
			m_fGAge			= 0.f;
			m_dwRunFlag		&= ~EFF_PLY_PLAY;
			m_dwRunFlag		&= ~EFF_PLY_DONE;
			m_dwRunFlag		&= ~EFF_PLY_END;
		}
	}

	if ( m_dwRunFlag & EFF_PLY_END )		goto _RETURN;

	//	Note : 시작되지 않았는지 검사, 시작 시간이면 활성화 시킴.
	//
	if ( !( m_dwRunFlag & EFF_PLY_PLAY ) )
	{
		if ( m_fGAge >= m_fGBeginTime )		m_dwRunFlag |= EFF_PLY_PLAY;
		else								goto _RETURN;
	}

	//	Note : 종료 시점이 되었는지 검사.
	//
	if ( IsReadyToDie() )					m_dwRunFlag |= EFF_PLY_END;

_RETURN:
	//	Note : 형제, 자식 노드.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );

	return S_OK;
}

HRESULT	DxEffectScreen::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha )
{
	HRESULT hr(S_OK);

	if ( !(m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_RENDPASS )		goto _RETURN;

	// Note : Get Texture
	if( !m_textureRes.GetTexture() )
		goto _RETURN;


	m_textureRes.GetTexture();

_RETURN:
	if ( FAILED(hr) )	return hr;

	//	Note : 형제, 자식 노드.
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matComb, fAlpha );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb, fAlpha );

	return S_OK;
}

HRESULT DxEffectScreen::FinalCleanup()
{
	//	Note : 형제 자식 노드.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

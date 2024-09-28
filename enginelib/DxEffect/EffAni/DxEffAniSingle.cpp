#include "pch.h"

#include "../../DxTools/editmeshs.h"

#include "../../DxEffect/Frame/DxEffectMan.h"

#include "../../DxEffect/Single/DxEffSingle.h"
#include "../Single/DxEffSinglePropGMan.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../DxMeshs/SkinMesh/DxSkeletonMaintain.h"
#include "../../DxMeshs/SkinMesh/DxSkinChar.h"
#include "../../DxMeshs/SkinMesh/DxAttBone.h"
#include "../../DxMeshs/SkinMesh/DxVehicle.h"

#include "../../DxMeshs/SkinMesh/DxSkeletonManager.h"
#include "../../DxTools/DxWeatherMan.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../G-Logic/GLPeriod.h"

#include "./DxEffAniSingle.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL g_bCHAR_EDIT_RUN;
extern BOOL g_bEFFECT_SPHERE_VISIBLE;

DWORD		DxEffAniData_Single::TYPEID			= EMEFFANI_SINGLE;
DWORD		DxEffAniData_Single::VERSION		= 0x0101;
char		DxEffAniData_Single::NAME[64]		= "Single EFFECT";

DxEffAni* DxEffAniData_Single::NEWOBJ( DxSkinAnimation* pAnimContainer )
{
	DxEffAniSingle*	pEff = new DxEffAniSingle;

	pEff->m_dwFlags		= m_dwFlags;
	pEff->m_dwFrame		= m_dwFrame;
	pEff->m_fFact		= m_fFact;
	pEff->m_fScale		= m_fScale;
	pEff->m_matBase		= m_matBase;

	pEff->m_pPropGroupRes = &m_sPropGroupRes;

	StringCchCopy( pEff->m_szEff,		STRING_NUM_128, m_szEff );
	StringCchCopy( pEff->m_szBipFirst,	STRING_NUM_128, m_szBipFirst );
	StringCchCopy( pEff->m_szBipSecond,	STRING_NUM_128, m_szBipSecond );

	pEff->Create ( m_pd3dDevice, pAnimContainer );

	return	pEff;
}

HRESULT	DxEffAniData_Single::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize, dwTypeID;

	SFile >> dwTypeID;
	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == VERSION )
	{
		//	Note : 이팩트의 Property 를 로드
		//
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );
	}
	else if ( dwVer == 0x0100 )
	{
		//	Note : 이팩트의 Property 를 로드.
		EFFANI_PROPERTY_SINGLE_100 sProp;
		SFile.ReadBuffer( &sProp, sizeof(EFFANI_PROPERTY_SINGLE_100) );
		m_dwFlags	= sProp.m_dwFlags;
		m_dwFrame	= sProp.m_dwFrame;
		m_fFact		= sProp.m_fFact;
		m_fScale	= sProp.m_fScale;

		StringCchCopy( m_szBipFirst, STRING_NUM_128, sProp.m_szBipFirst );
		StringCchCopy( m_szBipSecond, STRING_NUM_128, sProp.m_szBipSecond );
		StringCchCopy( m_szEff, STRING_NUM_128, sProp.m_szEff );

		D3DXMatrixIdentity( &m_matBase );
	}
	else
	{
		//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
		SFile.SetOffSet(SFile.GetfTell() + dwSize);
        std::string ErrorMsg(
            sc::string::format(
                "DxEffAniData_Single::LoadFile() - CURRENT_VERSION_%1%, SAVE_VERSION_%2%",
                VERSION,
                dwVer));
		CDebugSet::ToLogFile(ErrorMsg);
		return E_FAIL;
	}

	// Device 자원을 생성한다
	if (pd3dDevice)
	{
		hr = Create(pd3dDevice);
		if (FAILED(hr))
            return hr;
	}

	return S_OK;
}

HRESULT	DxEffAniData_Single::SaveFile ( sc::SerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}

BOOL DxEffAniData_Single::CheckEff ( DWORD dwCurKeyTime, int nPrevFrame )
{
	if( nPrevFrame == (int)dwCurKeyTime )	return FALSE;		// 같은 프레임이라면 새로 생성 안한다.

	if( nPrevFrame > (int)dwCurKeyTime )	// 시간이 초기화 됨.
	{
		// 0프레임일 경우
		if ( m_dwFrame <= dwCurKeyTime )
		{
			return TRUE;
		}

		return FALSE;
	}

	if( ((int)m_dwFrame > nPrevFrame) && (m_dwFrame <= dwCurKeyTime) )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL DxEffAniData_Single::IsSingleEffLoop()
{
	return m_dwFlags&USEEFFLOOP;
}

void DxEffAniData_Single::SetEffAniData ( DxEffAniData*	pData )
{
	DxEffAniData::SetEffAniData ( pData );

	//m_sPropGroupRes = ((DxEffAniData_Single*)pData)->m_sPropGroupRes;
	m_sPropGroupRes = DxEffSinglePropGMan::GetInstance().LoadEffectGProp( m_szEff, TRUE, FALSE );
}

HRESULT DxEffAniData_Single::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffAniData::InitDeviceObjects ( pd3dDevice );

	m_sPropGroupRes = DxEffSinglePropGMan::GetInstance().LoadEffectGProp( m_szEff, TRUE, FALSE );
	//if ( !m_pPropGroupRes->IsValid() )
	//	return E_FAIL;

	return S_OK;
}

HRESULT DxEffAniData_Single::DeleteDeviceObjects ()
{
	DxEffAniData::DeleteDeviceObjects ();

	return S_OK;
}


//	-----------------------------------------------	--------------------------------------------------

DxEffAniSingle::DxEffAniSingle(void)
	: DxEffAni()
	, m_pd3dDevice(NULL)
	, m_bDataSetting(FALSE)
	, m_vPos_1(0.f,0.f,0.f)
	, m_vPos_2(0.f,0.f,0.f)
	, m_fRandomFact(0.f)
	, m_pPropGroupRes(NULL)
{
	D3DXMatrixIdentity ( &m_matBase );
	D3DXMatrixIdentity ( &m_matWorld );
}

DxEffAniSingle::~DxEffAniSingle(void)
{
	CleanUp ();
}

// DxEffSinglePropGMan를 ThreadLoading 하도록 바꾸었기 때문에 DxSingleGroup가 로딩안되었으면 로딩을 시도한다.
void DxEffAniSingle::CreateDxEffSingleGroup( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_spSingleGroup.get() )
		return;

	m_pd3dDevice = pd3dDevice;

	BOOL	bUse = TRUE;
	bUse = SetWeather();		// 날씨에 대한 적용
	if ( bUse )
	{
		bUse = SetTime();		// 낮 밤에 대한 적용
	}

	if( g_bCHAR_EDIT_RUN )	
		bUse = TRUE;		// 캐릭터 에디터에서는 계속 나오게 함.

	if ( m_pPropGroupRes && m_pPropGroupRes->Update() && bUse )	
	{	
		m_spSingleGroup.reset();						// 이전 데이터는 지워준다.
		m_spSingleGroup = m_pPropGroupRes->get()->NEWEFFGROUP ();
		if ( m_spSingleGroup.get() )
		{
			//	Note : Eff Group Create Dev.
			//
			m_spSingleGroup->Create ( m_pd3dDevice );

			if ( m_dwFlags & USEEFFLOOP )
			{
				m_spSingleGroup->m_dwFlag |= EFF_CFG_NEVERDIE;		// Loop 셋팅이 되어 있다.
			}
			else
			{
				m_spSingleGroup->m_dwFlag &= ~EFF_CFG_NEVERDIE;		// 효과는 무조건 사라져야 한다.
			}
		}
	}
}

// DxEffSingleGroup 을 정리한다.
void DxEffAniSingle::DeleteDxEffSingleGroup()
{
	if ( !m_spSingleGroup.get() )
		return;

	m_spSingleGroup->CleanUp();
	m_spSingleGroup.reset();
}

HRESULT DxEffAniSingle::OneTimeSceneInit ()
{
	if ( m_spSingleGroup.get() )
        m_spSingleGroup->OneTimeSceneInit ();

	return S_OK;
}

HRESULT DxEffAniSingle::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	m_pd3dDevice = pd3dDevice;

	m_fRandomFact = ((float)(rand()%101)/100);			// 0.f~1.f

	// DxEffSingleGroup 을 정리한다.
	DeleteDxEffSingleGroup();

	// DxEffSinglePropGMan를 ThreadLoading 하도록 바꾸었기 때문에 DxSingleGroup가 로딩안되었으면 로딩을 시도한다.
	CreateDxEffSingleGroup( pd3dDevice );

	//if ( m_pPropGroup && bUse )
	//{	
	//	m_spSingleGroup.reset();						// 이전 데이터는 지워준다.
	//	m_spSingleGroup = m_pPropGroup->NEWEFFGROUP ();
	//	if ( m_spSingleGroup.get() == NULL )
 //           return E_FAIL;

	//	//	Note : Eff Group Create Dev.
	//	//
	//	m_spSingleGroup->Create ( pd3dDevice );

	//	m_fRandomFact = ((float)(rand()%101)/100);			// 0.f~1.f
	//	m_spSingleGroup->m_dwFlag &= ~EFF_CFG_NEVERDIE;		// 효과는 무조건 사라져야 한다.
	//}

	//if ( m_spSingleGroup.get() )
 //       m_spSingleGroup->InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffAniSingle::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_spSingleGroup.get() )
        m_spSingleGroup->RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffAniSingle::InvalidateDeviceObjects ()
{
	if ( m_spSingleGroup.get() )
        m_spSingleGroup->InvalidateDeviceObjects ();

	return S_OK;
}

HRESULT DxEffAniSingle::DeleteDeviceObjects ()
{
	if ( m_spSingleGroup.get() )
        m_spSingleGroup->DeleteDeviceObjects ();

	return S_OK;
}

HRESULT DxEffAniSingle::FinalCleanup ()
{
	if ( m_spSingleGroup.get() )
        m_spSingleGroup->FinalCleanup();

    m_spSingleGroup.reset();

	return S_OK;
}

HRESULT DxEffAniSingle::FrameMove ( DxSkinCharEffAniBase* pSkinCharBase, float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	// DxEffSinglePropGMan를 ThreadLoading 하도록 바꾸었기 때문에 DxSingleGroup가 로딩안되었으면 로딩을 시도한다.
	CreateDxEffSingleGroup( m_pd3dDevice );

	if ( m_spSingleGroup.get() == NULL )
	{
		return S_OK;
	}

	m_spSingleGroup->FrameMove ( fTime, fElapsedTime );

	if ( !m_spSingleGroup->m_dwAliveCount )		// 지워야 할 경우.
	{
		SetEnd ();
		return S_OK;
	}

	return S_OK;
}

HRESULT	DxEffAniSingle::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharEffAniBase* pSkinChar, const D3DXMATRIX& matCurPos, float fAlpha )
{
	if ( m_spSingleGroup.get() == NULL )
        return S_OK;

	if ( m_dwFlags&USECHARMOVE || m_dwFlags&USEEFFLOOP )		// 이동하는 물체 , Loop 물체도 계속 위치를 얻어온다.
	{
		const D3DXMATRIXA16* pBoneMatrix(NULL);
		if ( !SetPosition ( pSkinChar->GetSkeletonMaintain(), &pBoneMatrix ) )
			return S_FALSE;

		SetMatrix ( matCurPos, *pBoneMatrix );
	}
	else							// 처음 사용 할 경우
	{
		if ( !m_bDataSetting )
		{
			const D3DXMATRIXA16* pBoneMatrix(NULL);
			if ( !SetPosition ( pSkinChar->GetSkeletonMaintain(), &pBoneMatrix ) )	
				return S_FALSE;

			SetMatrix ( matCurPos, *pBoneMatrix );

			m_bDataSetting = TRUE;			// 데이터 셋팅 완료
		}

		m_spSingleGroup->m_matWorld = m_matWorld;
	}

	BOOL	bUse = TRUE;
	bUse = SetWeather();		// 날씨에 대한 적용
	if ( bUse )
	{
		bUse = SetTime();		// 낮 밤에 대한 적용
	}

	if( g_bCHAR_EDIT_RUN )	
	{
		bUse = TRUE;		// 캐릭터 에디터에서는 계속 나오게 함.
	}

	if ( g_bEFFECT_SPHERE_VISIBLE ) // VISIBLE이 켜져있을때만 구가 출력
	{
		EDITMESHS::RENDERSPHERE ( pd3dDevice, m_vPos_1, 1.0f );
		EDITMESHS::RENDERSPHERE ( pd3dDevice, m_vPos_2, 1.0f );	
	}

	m_spSingleGroup->m_bOut_Particle = bUse;
	m_spSingleGroup->m_fOut_Scale	= m_fScale;				// 스케일

	m_spSingleGroup->Render ( pd3dDevice, D3DXVECTOR3(1.f,1.f,1.f), fAlpha );

	return S_OK;
}
//
//HRESULT	DxEffAniSingle::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxAttBone* pAttBone, const D3DXMATRIX& matCurPos, float fAlpha )
//{
//	if ( m_spSingleGroup.get() == NULL )
//        return S_OK;
//
//	if ( m_dwFlags&USECHARMOVE || m_dwFlags&USEEFFLOOP )		// 이동하는 물체 , Loop 물체도 계속 위치를 얻어온다.
//	{
//		const D3DXMATRIXA16* pBoneMatrix(NULL);
//		if ( !SetPosition ( pAttBone->GetSkeletonMaintain(), &pBoneMatrix ) )	
//			return S_OK;
//
//		SetMatrix ( matCurPos, *pBoneMatrix );
//	}
//	else							// 처음 사용 할 경우
//	{
//		if ( !m_bDataSetting )
//		{
//			const D3DXMATRIXA16* pBoneMatrix(NULL);
//			if ( !SetPosition ( pAttBone->GetSkeletonMaintain(), &pBoneMatrix ) )	
//				return S_OK;
//
//			SetMatrix ( matCurPos, *pBoneMatrix );
//
//			m_bDataSetting = TRUE;			// 데이터 셋팅 완료
//		}
//
//		m_spSingleGroup->m_matWorld = m_matWorld;
//	}
//
//	BOOL	bUse = TRUE;
//	bUse = SetWeather();		// 날씨에 대한 적용
//	if ( bUse )
//	{
//		bUse = SetTime();		// 낮 밤에 대한 적용
//	}
//
//	if( g_bCHAR_EDIT_RUN )	
//	{
//		bUse = TRUE;		// 캐릭터 에디터에서는 계속 나오게 함.
//	}
//
//	if ( g_bEFFECT_SPHERE_VISIBLE ) // VISIBLE이 켜져있을때만 구가 출력
//	{
//		EDITMESHS::RENDERSPHERE ( pd3dDevice, m_vPos_1, 1.0f );
//		EDITMESHS::RENDERSPHERE ( pd3dDevice, m_vPos_2, 1.0f );	
//	}
//
//	m_spSingleGroup->m_bOut_Particle = bUse;
//	m_spSingleGroup->m_fOut_Scale	= m_fScale;				// 스케일
//
//	m_spSingleGroup->Render ( pd3dDevice, D3DXVECTOR3(1.f,1.f,1.f), fAlpha );
//
//	return S_OK;
//}
//
//HRESULT	DxEffAniSingle::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxVehicle* pVehicle, const D3DXMATRIX& matCurPos, float fAlpha )
//{
//	if ( m_spSingleGroup.get() == NULL )
//        return S_OK;
//
//	if ( m_dwFlags&USECHARMOVE || m_dwFlags&USEEFFLOOP )		// 이동하는 물체 , Loop 물체도 계속 위치를 얻어온다.
//	{
//		const D3DXMATRIXA16* pBoneMatrix(NULL);
//		if ( !SetPosition ( pVehicle->GetSkeletonMaintain(), &pBoneMatrix ) )	return S_OK;
//
//		SetMatrix ( matCurPos, *pBoneMatrix );
//	}
//	else							// 처음 사용 할 경우
//	{
//		if ( !m_bDataSetting )
//		{
//			const D3DXMATRIXA16* pBoneMatrix(NULL);
//			if ( !SetPosition ( pVehicle->GetSkeletonMaintain(), &pBoneMatrix ) )	return S_OK;
//
//			SetMatrix ( matCurPos, *pBoneMatrix );
//
//			m_bDataSetting = TRUE;			// 데이터 셋팅 완료
//		}
//
//		m_spSingleGroup->m_matWorld = m_matWorld;
//	}
//
//	BOOL	bUse = TRUE;
//	bUse = SetWeather();		// 날씨에 대한 적용
//	if ( bUse )
//	{
//		bUse = SetTime();		// 낮 밤에 대한 적용
//	}
//
//	if( g_bCHAR_EDIT_RUN )	
//	{
//		bUse = TRUE;		// 캐릭터 에디터에서는 계속 나오게 함.
//	}
//
//	if ( g_bEFFECT_SPHERE_VISIBLE ) // VISIBLE이 켜져있을때만 구가 출력
//	{
//		EDITMESHS::RENDERSPHERE ( pd3dDevice, m_vPos_1, 1.0f );
//		EDITMESHS::RENDERSPHERE ( pd3dDevice, m_vPos_2, 1.0f );	
//	}
//
//	m_spSingleGroup->m_bOut_Particle = bUse;
//	m_spSingleGroup->m_fOut_Scale	= m_fScale;				// 스케일
//
//	m_spSingleGroup->Render ( pd3dDevice, D3DXVECTOR3(1.f,1.f,1.f), fAlpha );
//
//	return S_OK;
//}

BOOL	DxEffAniSingle::SetPosition ( const DxSkeletonMaintain* pSkeletonMaintain,  const D3DXMATRIXA16** ppMatrix )
{
	TSTRING strname( m_szBipFirst );
	const DxBoneTransMaintain* pBoneTrans = pSkeletonMaintain->FindBone( strname );
	if ( NULL == pBoneTrans ) 
	{
		CDebugSet::ToLogFile( sc::string::format( _T(" Bone Does Not Exist : %1%"), strname ) );
		GASSERT ( 0 && "DxEffAniSingle::SetPosition() : Bone Does Not Exist" );
		return FALSE;
	}

	(*ppMatrix) = &pBoneTrans->m_matCombined;			// 주소를 얻는다.
	m_vPos_1.x = pBoneTrans->m_matCombined._41;
	m_vPos_1.y = pBoneTrans->m_matCombined._42;
	m_vPos_1.z = pBoneTrans->m_matCombined._43;

	if( (m_dwFlags&USEDRAWRANGE) || (m_dwFlags&USEDRAWDIRECT) )
	{
		strname = m_szBipSecond;
		pBoneTrans = pSkeletonMaintain->FindBone( strname );
		if ( NULL == pBoneTrans ) 
		{
			CDebugSet::ToLogFile( sc::string::format( _T(" Bone Does Not Exist : %1%"), strname ) );
			GASSERT ( 0 && "DxEffAniSingle::SetPosition() : Bone Does Not Exist" );
			return FALSE;
		}

		m_vPos_2.x = pBoneTrans->m_matCombined._41;
		m_vPos_2.y = pBoneTrans->m_matCombined._42;
		m_vPos_2.z = pBoneTrans->m_matCombined._43;
	}

	return TRUE;
}

void	DxEffAniSingle::SetMatrix ( const D3DXMATRIX& matCurPos, const D3DXMATRIX& matBone )
{
    if( m_spSingleGroup.get() == NULL )
        return;

	D3DXMATRIX	matWorld;
	D3DXMatrixIdentity ( &matWorld );

	if ( m_dwFlags&USEDRAWPOINT )
	{
		matWorld._41 = m_vPos_1.x;
		matWorld._42 = m_vPos_1.y;
		matWorld._43 = m_vPos_1.z;

		D3DXMATRIX	matRotate, matRotateAni;
		D3DXMatrixRotationY ( &matRotate, (D3DX_PI*0.5f) );
		matRotateAni = matCurPos;
		matRotateAni._41 = 0.f;
		matRotateAni._42 = 0.f;
		matRotateAni._43 = 0.f;

		D3DXMatrixMultiply ( &matRotate, &matRotate, &matRotateAni );
		D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );

		m_spSingleGroup->m_matWorld = matWorld;
	}
	else if ( m_dwFlags&USEDRAWRANGE )
	{
		float fTemp = 1.f - m_fRandomFact;
		matWorld._41 = m_vPos_1.x*m_fRandomFact + m_vPos_2.x*fTemp;
		matWorld._42 = m_vPos_1.y*m_fRandomFact + m_vPos_2.y*fTemp;
		matWorld._43 = m_vPos_1.z*m_fRandomFact + m_vPos_2.z*fTemp;

		D3DXMATRIX	matRotate, matRotateAni;
		D3DXMatrixRotationY ( &matRotate, (D3DX_PI*0.5f) );
		matRotateAni = matCurPos;
		matRotateAni._41 = 0.f;
		matRotateAni._42 = 0.f;
		matRotateAni._43 = 0.f;

		D3DXMatrixMultiply ( &matRotate, &matRotate, &matRotateAni );
		D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );

		m_spSingleGroup->m_matWorld = matWorld;
	}
	else if ( m_dwFlags&USEDRAWDIRECT )
	{
		matWorld._41 = m_vPos_1.x;
		matWorld._42 = m_vPos_1.y;
		matWorld._43 = m_vPos_1.z;

		m_spSingleGroup->m_vTargetPos	= m_vPos_2;	// 목표 위치
		m_spSingleGroup->m_vStartPos		= m_vPos_1;	// 시작 위치
		m_spSingleGroup->m_vDirect		= m_spSingleGroup->m_vTargetPos - m_spSingleGroup->m_vStartPos;	// 방향

		m_spSingleGroup->m_bOut_Direct	= TRUE;					// 방향까지 넣어 준다.

		DxBillboardLookAtDir ( m_spSingleGroup->m_matOutRotate, &m_spSingleGroup->m_vDirect );

		m_spSingleGroup->m_matWorld = matWorld;
	}
	else if( m_dwFlags&USEDRAWMATRIX )
	{
		D3DXMatrixMultiply( &matWorld, &m_matBase, &matBone );
		m_spSingleGroup->m_matWorld = matWorld;
	}

	m_matWorld = matWorld;
}

BOOL	DxEffAniSingle::SetTime ()
{
	if ( GLPeriod::GetInstance().IsOffLight() || DxLightMan::GetInstance()->IsNightAndDay() == FALSE )	// 낮
	{
		if ( m_dwFlags & USETIMEDAY )	return TRUE;
		else							return FALSE;
	}
	else									// 밤
	{
		if ( m_dwFlags & USETIMENIGHT )	return TRUE;
		else							return FALSE;
	}

	return TRUE;
}

BOOL	DxEffAniSingle::SetWeather ()
{
	if ( DxWeatherMan::GetInstance()->GetRain()->UseRain() )			// 비 오는 날
	{
		if ( m_dwFlags & USESKYRAIN )	return TRUE;
		else							return FALSE;
	}
	else if ( DxWeatherMan::GetInstance()->GetSnow()->UseSnow() )		// 눈 오는 날
	{
		if ( m_dwFlags & USESKYSNOW )	return TRUE;
		else							return FALSE;
	}
	else if ( DxWeatherMan::GetInstance()->GetLeaves()->UseLeaves() )	// 낙엽 내리는 날
	{
		if ( m_dwFlags & USESKYLEAVES )	return TRUE;
		else							return FALSE;
	}
	else																// 맑은 날씨
	{
		if ( m_dwFlags & USESKYFINE )	return TRUE;
		else							return FALSE;
	}

	return TRUE;
}

void DxEffAniSingle::SetNeverDieErase()
{
	if ( !m_spSingleGroup.get() )
		return;

	m_spSingleGroup->m_dwFlag &= ~EFF_CFG_NEVERDIE;		// 사라져야 된다고 셋팅한다.
}
#include "pch.h"

#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxEffect/ShaderConstant.h"

#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxEffect/Single/DxEffSingleMan.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "./dxeffectcamera.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//---------------------------------------------------------------------[CAMERA_PROPERTY]
const DWORD	CAMERA_PROPERTY::VERSION	= 0x0101;
const char	CAMERA_PROPERTY::NAME[]	= "ī�޶�";

DxEffSingle* CAMERA_PROPERTY::NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup )
{
	DxEffectCamera *pEffect = new DxEffectCamera;

	//	Note : TransPorm Set.		[���뼳��]
	pEffect->m_matLocal = m_matLocal;
	
	//	Note : �ð� ����.			[���뼳��]
	pEffect->m_fGBeginTime = m_fGBeginTime;
	pEffect->m_fGLifeTime = m_fGLifeTime;

	//	Note : ���ư��� ������Ʈ	[���뼳��]
	pEffect->m_bMoveObj		= m_bMoveObj;

	pEffect->m_iCameraSet	= m_iCameraSet;
	pEffect->m_fValue		= m_fValue;
	pEffect->m_fPlayTime	= 1.f/m_fPlayNum;

	return pEffect;
}

HRESULT CAMERA_PROPERTY::SaveFile ( sc::SerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA�� ����� ����Ѵ�. Load �ÿ� ������ Ʋ�� ��� ����.
	//
	SFile << (DWORD) ( GetSizeBase() + sizeof(m_Property) );

	//	Note : �θ� Ŭ������ ����.
	SFile.WriteBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	
	SFile << m_bMoveObj;
	SFile << m_fGBeginTime;
	SFile << m_fGLifeTime;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(PROPERTY) );

	//	Note : ����, �ڽ� ����.
	//
	EFF_PROPERTY::SaveFile ( SFile );

	return S_OK;
}

HRESULT CAMERA_PROPERTY::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : ������ ��ġ�� ���. 
	//
	if( dwVer == VERSION )
	{
		//	Note : �θ� Ŭ������ ����.
		SFile.ReadBuffer ( m_matLocal, sizeof(D3DXMATRIX) );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else if( dwVer == 0x0100 )
	{
		//	Note : �θ� Ŭ������ ����.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else
	{
		//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
		//
		DWORD dwCur = SFile.GetfTell ();
		SFile.SetOffSet ( dwCur+dwSize );
	}

	//	Note : Device �ڿ��� �����Ѵ�.
	//
	hr = Create ( pd3dDevice, bBackUpCache );
	if ( FAILED(hr) )	return hr;

	//	Note : ����, �ڽ� �б�.
	//
	EFF_PROPERTY::LoadFile ( SFile, pd3dDevice, bBackUpCache, bOldDecalUSE );

	return S_OK;
}

HRESULT CAMERA_PROPERTY::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache )
{
	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice, bBackUpCache );

	return S_OK;
}

HRESULT CAMERA_PROPERTY::DeleteDeviceObjects ()
{
	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::DeleteDeviceObjects ();

	return S_OK;
}

//-------------------------------------------- [���� �ɹ� ���� �� �ʱ�ȭ] ----------------------------------------------
//
//
const DWORD	DxEffectCamera::TYPEID	= EFFSINGLE_CAMERA;
const DWORD	DxEffectCamera::FLAG		= NULL;
const char	DxEffectCamera::NAME[]	= "ī�޶�";

//	Note : 
//
BOOL DxEffectCamera:: VAILEDDEVICE				= FALSE;

HRESULT DxEffectCamera::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	VAILEDDEVICE = TRUE;
	return S_OK;
}

HRESULT DxEffectCamera::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	VAILEDDEVICE = FALSE;
	return S_OK;
}

void DxEffectCamera::OnInitDevice_STATIC()
{
}

void DxEffectCamera::OnDeleteDevice_STATIC()
{
}


HRESULT DxEffectCamera::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectCamera::InvalidateDeviceObjects ()
{
	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

//-------------------------------------------- [Ŭ���� ��ü] ------------------------------------------------------------
//
//
DxEffectCamera::DxEffectCamera () :
	m_pd3dDevice(NULL)
{
	m_fGLifeTime	= 0.0f;
	m_fTimeSum		= 0.f;

	m_vCameraMoveSum	= D3DXVECTOR3 (0.f,0.f,0.f);
}

DxEffectCamera::~DxEffectCamera ()
{
	CleanUp ();

	D3DXVECTOR3	*pFromPt	= &DxViewPort::GetInstance().GetFromPt();
	//D3DXVECTOR3	*pLookatPt	= &DxViewPort::GetInstance().GetLookatPt();
	pFromPt->y		-= m_vCameraMoveSum.y;
	//pLookatPt->y	-= m_vCameraMoveSum.y;
	m_vCameraMoveSum.y = 0.f;
}

HRESULT DxEffectCamera::FinalCleanup()
{
	//	Note : ���� �ڽ� ���.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

HRESULT DxEffectCamera::FrameMove ( float fTime, float fElapsedTime )
{
	D3DXVECTOR3		vUp( 0.f, 1.f, 0.f );
	D3DXVECTOR3		vDeltaGVel;
	D3DXVECTOR3		vLocal;
	D3DXVECTOR3&	vFromPt		= DxViewPort::GetInstance().GetFromPt();
	D3DXMATRIX		matLocal;

	//	Note : Eff�� ���̸� ���.
	//
	m_fGAge += fElapsedTime;
	m_fTimeSum	+= fElapsedTime;	// ���� �ð�

	//	Note : �ݺ��� �� �ð� �� ���� ���� �ʱ�ȭ �Ѵ�.
	//
	if ( m_rThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_rThisGroup->m_fGBeginTime+m_rThisGroup->m_fGLifeTime) )
		{
			m_fGAge		= 0.f;
			m_dwRunFlag &= ~EFF_PLY_PLAY;
			m_dwRunFlag &= ~EFF_PLY_DONE;
			m_dwRunFlag &= ~EFF_PLY_END;
		}
	}

	if ( m_dwRunFlag & EFF_PLY_END )
	{
		goto _RETURN;
	}

	//	Note : ���۵��� �ʾҴ��� �˻�, ���� �ð��̸� Ȱ��ȭ ��Ŵ.
	//
	if ( !( m_dwRunFlag & EFF_PLY_PLAY ) )
	{
		if ( m_fGAge >= m_fGBeginTime )	
		{
			m_dwRunFlag |= EFF_PLY_PLAY;
		}
		else
		{
			// ��������.
			goto _RETURN;
		}
	}

	//	Note : ���� ������ �Ǿ����� �˻�.
	//
	if ( IsReadyToDie() )			
	{
		m_dwRunFlag |= EFF_PLY_END;
	}

	if ( m_dwRunFlag&EFF_PLY_RENDPASS )	
	{
		D3DXVECTOR3	*pFromPt = &DxViewPort::GetInstance().GetFromPt();
		//D3DXVECTOR3	*pLookatPt = &DxViewPort::GetInstance().GetLookatPt();
		pFromPt->y		-= m_vCameraMoveSum.y;
		//pLookatPt->y	-= m_vCameraMoveSum.y;
		m_vCameraMoveSum.y = 0.f;

		goto _RETURN;	// ��鸮�� �ʵ���
	}

	float		AddY;
	D3DXVECTOR3	*pFromPt = &DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3	*pLookatPt = &DxViewPort::GetInstance().GetLookatPt();

	switch ( m_iCameraSet )
	{
	//	ī�޶� �� �Ʒ��� ����
	case CAMERA_MOVE_UP_DOWN:
		if ( m_fTimeSum > m_fPlayTime )
		{
			m_fTimeSum = 0.f;
			AddY = 1.f*m_fValue*((rand()%10)/10.f);		// m_fValue - ��������

			if ( m_vCameraMoveSum.y > 0.0f )
			{
				m_vCameraMoveSum.y	-= AddY;
				pFromPt->y			-= AddY;
			//	pLookatPt->y		-= AddY;
			}
			else
			{
				m_vCameraMoveSum.y	+= AddY;
				pFromPt->y			+= AddY;
			//	pLookatPt->y		+= AddY;
			}
		}
		break;

		// ī�޶� ȸ��.
	case CAMERA_ROTATION_Y:
		// 0.0173f �̶�� ���� ������ �ƴ� Test �� ������ ����. // m_fValue(360) ��ġ�� ������ 1�ʴ� 360 ���� �������� ��ġ��.
		DxViewPort::GetInstance().CameraRotation( D3DXVECTOR3( 0.f, (m_fValue * fElapsedTime) * 0.0173f, 0.f ), FALSE );		// m_fValue - ȸ���ӵ�
		break;
	}

	if( m_dwRunFlag & EFF_PLY_END )
	{
		pFromPt->y		-= m_vCameraMoveSum.y;
	//	pLookatPt->y	-= m_vCameraMoveSum.y;
		m_vCameraMoveSum.y = 0.f;
	}

	DxViewPort::GetInstance().SetViewTrans( *pFromPt, *pLookatPt, vUp );
	DxViewPort::GetInstance().ComputeClipVolume();

_RETURN:
	//	Note : ����, �ڽ� ���.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );

    return S_OK;
}

HRESULT DxEffectCamera::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha )
{
	HRESULT hr = S_OK;

	if ( !(m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_RENDPASS )		goto _RETURN;

_RETURN:
	//	Note : ����, �ڽ� ���.
	//
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matComb, fAlpha );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb, fAlpha );

	return hr;
}
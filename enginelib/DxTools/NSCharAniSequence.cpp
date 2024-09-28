// NSCharAniSequence.cpp:
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "../../SigmaCore/File/FileUtil.h"
#include "../../SigmaCore/Util/Util.h"

#include "../DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../DxMeshs/SkinMesh/DxSkinChar.h"
#include "../DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../DxEffect/ShaderConstant.h"
#include "../DxLand/DxLandMan.h"
#include "../Water/NSWaterMain.h"
#include "../DxEffect/NSEffectThread.h"
#include "../DxTools/DxShadowMap.h"
#include "../DxTools/DxEnvironment.h"
#include "../DxTools/Material/NSMaterialManager.h"
#include "../DxTools/CubeMap/NSCubeMapUtil.h"
#include "../DxTools/CubeMap/NSCubeMapCulling.h"
#include "../DxTools/CubeMap/NSCubeMap.h"
#include "../DxTools/Light/NSShadowLight.h"
#include "../DxTools/PostProcessing/NSBloomHDR.h"
#include "../DxTools/Deffered/NSDeffered.h"
#include "../DxEffect/Frame/DxEffectMan.h"
#include "../PhysX/NSPhysX.h"
#include "./PostProcessing/NSGlow.h"
#include "./Texture/NSTexCompress.h"
#include "./Light/DxLightMan.h"
//#include "../DxEffect/DxEffectMan.h"
#include "../DxResponseMan.h"
#include "./DxBackUpRendTarget.h"
#include "./DxEnvironment.h"
#include "./DxDynamicVB.h"
#include "./DxViewPort.h"
#include "./DxGlowMan.h"
#include "./DxCubeMap.h"
#include "./DxMethods.h"
#include "./TextureManager.h"
#include "./DxSurfaceTex.h"
#include "./RENDERPARAM.h"

#include "NSCharAniSequence.h"

#include "../../RanLogic/FlyCamera/GLFlyCameraKeyFrameControl.h"



// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


extern BOOL			g_bCHAR_EDIT_RUN;
extern BOOL			g_bEFFECT_SPHERE_VISIBLE;

namespace NSCharAniSequence
{
	enum { AF_CUSTOM_SCREENSIZE_IDX = 4 };

	float g_fSkinTimePrev(-1.f);
	float g_fTimeADD(0.f);
	BOOL g_bFrameMoveADD(FALSE);	// 추가로 렌더링되는 시간.
	TSTRING	g_strPath;

	// 640*480
	// 720*480
	// 800*600
	// 1280*720
	float g_afScreenSizeX[] =
	{
		640.f,
		720.f,
		800.f,
		1280.f,
		0.0f
	};

	float g_afScreenSizeY[] = 
	{
		480.f,
		480.f,
		600.f,
		720.f,
		0.0f,
	};

	TSTRING g_strFPS[] = 
	{
		_T("30fps"),
		_T("60fps")
	};

	INT							m_nAnimIndex;
	std::vector<std::string>	m_vecAnimname;
	GLFlyCameraKeyFrameControl* m_pFlyCam;

	D3DXMATRIX m_matProj;

	BOOL ColorPower( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTextureSRC, LPDIRECT3DTEXTUREQ& pTextureDEST, float fPower )
	{
		D3DSURFACE_DESC pDesc;
		pTextureSRC->GetLevelDesc( 0, &pDesc );
		DWORD dwTexelWidth = pDesc.Width;
		DWORD dwTexelHeight = pDesc.Height;

		LPDIRECT3DBASETEXTURE9 pBaseTexture(NULL);
		HRESULT hr = NSTexCompress::Compress
		(
			pd3dDevice,
			dwTexelWidth,
			dwTexelHeight,
			0,
			1,
			D3DFMT_A8R8G8B8,
			pTextureSRC,
			pBaseTexture
		);

		if( FAILED(hr) )
		{
			return FALSE;
		}

		pTextureDEST = static_cast< LPDIRECT3DTEXTUREQ >( pBaseTexture );

		D3DLOCKED_RECT	stLocked;
		hr = pTextureDEST->LockRect( 0, &stLocked, NULL, 0 );
		if( FAILED(hr) )
		{
			GASSERT( 0 && _T("hr = pTextureDEST->LockRect( 0, &stLocked, NULL, 0 );") );
			return FALSE;
		}
		DWORD* pData = (DWORD*)stLocked.pBits;
		int iPitch = stLocked.Pitch / 4;

		float fAlpha(0.f);
		D3DXVECTOR3 vColor(0.f,0.f,0.f);
		D3DXVECTOR3 vColor_2(0.f,0.f,0.f);
		for( DWORD y=0; y<dwTexelHeight; ++y )
		{
			for( DWORD x=0; x<dwTexelWidth; ++x )
			{
				vColor.x = (float) ((pData[y*iPitch+x]&0xff0000)>>16);
				vColor.y = (float) ((pData[y*iPitch+x]&0xff00)>>8);
				vColor.z = (float) (pData[y*iPitch+x]&0xff);
				vColor.x /= 255.f;
				vColor.y /= 255.f;
				vColor.z /= 255.f;

				vColor_2.x = vColor.x*vColor.x;
				vColor_2.y = vColor.y*vColor.y;
				vColor_2.z = vColor.z*vColor.z;
				fAlpha = vColor_2.x;
				if( fAlpha < vColor_2.y )		fAlpha = vColor_2.y;
				if( fAlpha < vColor_2.z )		fAlpha = vColor_2.z;
				//fAlpha = vColor.x + vColor.y + vColor.z;
				//fAlpha /= 3.f;
				//fAlpha = fAlpha * fAlpha;
				//fAlpha = vColor.x;
				//if( fAlpha > vColor.y )		fAlpha = vColor.y;
				//if( fAlpha > vColor.z )		fAlpha = vColor.z;

				vColor.x *= fPower;
				vColor.y *= fPower;
				vColor.z *= fPower;

				if( vColor.x < 0.f )	vColor.x = 0.f;
				if( vColor.y < 0.f )	vColor.y = 0.f;
				if( vColor.z < 0.f )	vColor.z = 0.f;
				if( vColor.x > 1.f )	vColor.x = 1.f;
				if( vColor.y > 1.f )	vColor.y = 1.f;
				if( vColor.z > 1.f )	vColor.z = 1.f;

				pData[y*iPitch+x] = (static_cast<DWORD>(fAlpha*255.f) << 24) +
									(static_cast<DWORD>(vColor.x*255.f) << 16) +
									(static_cast<DWORD>(vColor.y*255.f) << 8) +
									(static_cast<DWORD>(vColor.z*255.f));
			}
		}

		pTextureDEST->UnlockRect( 0 );

		return TRUE;
	}

	BOOL ColorAdd( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTexture, LPDIRECT3DTEXTUREQ pTextureAlpha, LPDIRECT3DTEXTUREQ& pTextureOUT )
	{
		D3DSURFACE_DESC pDesc;
		pTexture->GetLevelDesc( 0, &pDesc );
		DWORD dwTexelWidth = pDesc.Width;
		DWORD dwTexelHeight = pDesc.Height;

		LPDIRECT3DBASETEXTURE9 pBaseTexture(NULL);
		HRESULT hr = NSTexCompress::Compress
		(
			pd3dDevice,
			dwTexelWidth,
			dwTexelHeight,
			0,
			1,
			D3DFMT_A8R8G8B8,
			pTexture,
			pBaseTexture
		);

		if( FAILED(hr) )
		{
			return FALSE;
		}

		pTextureOUT = static_cast< LPDIRECT3DTEXTUREQ >( pBaseTexture );

		D3DLOCKED_RECT	stLocked;
		hr = pTexture->LockRect( 0, &stLocked, NULL, 0 );
		if( FAILED(hr) )
		{
			GASSERT( 0 && _T("hr = pTexture->LockRect( 0, &stLocked, NULL, 0 );") );
			return FALSE;
		}
		DWORD* pDataSRC = (DWORD*)stLocked.pBits;

		hr = pTextureAlpha->LockRect( 0, &stLocked, NULL, 0 );
		if( FAILED(hr) )
		{
			GASSERT( 0 && _T("hr = pTextureAlpha->LockRect( 0, &stLocked, NULL, 0 );") );
			pTexture->UnlockRect( 0 );
			return FALSE;
		}
		DWORD* pDataALPHA = (DWORD*)stLocked.pBits;

		hr = pTextureOUT->LockRect( 0, &stLocked, NULL, 0 );
		if( FAILED(hr) )
		{
			GASSERT( 0 && _T("hr = pTextureOUT->LockRect( 0, &stLocked, NULL, 0 );") );
			pTextureAlpha->UnlockRect( 0 );
			pTexture->UnlockRect( 0 );
			return FALSE;
		}
		DWORD* pDataAdd = (DWORD*)stLocked.pBits;
		int iPitch = stLocked.Pitch / 4;

		float fAlpha(0.f);
		D3DXVECTOR4 vColorSRC(0.f,0.f,0.f,0.f);
		D3DXVECTOR4 vColorAlpha(0.f,0.f,0.f,0.f);
		D3DXVECTOR4 vColorAdd(0.f,0.f,0.f,0.f);
		for( DWORD y=0; y<dwTexelHeight; ++y )
		{
			for( DWORD x=0; x<dwTexelWidth; ++x )
			{
				vColorSRC.w = (float) ((pDataSRC[y*iPitch+x]&0xff000000)>>24);
				vColorSRC.x = (float) ((pDataSRC[y*iPitch+x]&0xff0000)>>16);
				vColorSRC.y = (float) ((pDataSRC[y*iPitch+x]&0xff00)>>8);
				vColorSRC.z = (float) (pDataSRC[y*iPitch+x]&0xff);
				vColorSRC.w /= 255.f;
				vColorSRC.x /= 255.f;
				vColorSRC.y /= 255.f;
				vColorSRC.z /= 255.f;

				vColorAlpha.w = (float) ((pDataALPHA[y*iPitch+x]&0xff000000)>>24);
				vColorAlpha.x = (float) ((pDataALPHA[y*iPitch+x]&0xff0000)>>16);
				vColorAlpha.y = (float) ((pDataALPHA[y*iPitch+x]&0xff00)>>8);
				vColorAlpha.z = (float) (pDataALPHA[y*iPitch+x]&0xff);
				vColorAlpha.w /= 255.f;
				vColorAlpha.x /= 255.f;
				vColorAlpha.y /= 255.f;
				vColorAlpha.z /= 255.f;

				vColorAdd.x = vColorSRC.x + vColorAlpha.x;
				vColorAdd.y = vColorSRC.y + vColorAlpha.y;
				vColorAdd.z = vColorSRC.z + vColorAlpha.z;
				vColorAdd.w = vColorSRC.w + vColorAlpha.w;

				if( vColorAdd.x < 0.f )	vColorAdd.x = 0.f;
				if( vColorAdd.y < 0.f )	vColorAdd.y = 0.f;
				if( vColorAdd.z < 0.f )	vColorAdd.z = 0.f;
				if( vColorAdd.w < 0.f )	vColorAdd.w = 0.f;
				if( vColorAdd.x > 1.f )	vColorAdd.x = 1.f;
				if( vColorAdd.y > 1.f )	vColorAdd.y = 1.f;
				if( vColorAdd.z > 1.f )	vColorAdd.z = 1.f;
				if( vColorAdd.w > 1.f )	vColorAdd.w = 1.f;

				pDataAdd[y*iPitch+x] = (static_cast<DWORD>(vColorAdd.w*255.f) << 24) +
										(static_cast<DWORD>(vColorAdd.x*255.f) << 16) +
										(static_cast<DWORD>(vColorAdd.y*255.f) << 8) +
										(static_cast<DWORD>(vColorAdd.z*255.f));
			}
		}

		pTextureOUT->UnlockRect( 0 );
		pTextureAlpha->UnlockRect( 0 );
		pTexture->UnlockRect( 0 );

		return TRUE;
	}

	BOOL ColorMultiply( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTexture, LPDIRECT3DTEXTUREQ& pTextureOUT )
	{
		D3DSURFACE_DESC pDesc;
		pTexture->GetLevelDesc( 0, &pDesc );
		DWORD dwTexelWidth = pDesc.Width;
		DWORD dwTexelHeight = pDesc.Height;

		LPDIRECT3DBASETEXTURE9 pBaseTexture(NULL);
		HRESULT hr = NSTexCompress::Compress
		(
			pd3dDevice,
			dwTexelWidth,
			dwTexelHeight,
			0,
			1,
			D3DFMT_A8R8G8B8,
			pTexture,
			pBaseTexture
		);

		if( FAILED(hr) )
		{
			return FALSE;
		}

		pTextureOUT = static_cast< LPDIRECT3DTEXTUREQ >( pBaseTexture );

		D3DLOCKED_RECT	stLocked;
		hr = pTexture->LockRect( 0, &stLocked, NULL, 0 );
		if( FAILED(hr) )
		{
			GASSERT( 0 && _T("hr = pTexture->LockRect( 0, &stLocked, NULL, 0 );") );
			return FALSE;
		}
		DWORD* pDataSRC = (DWORD*)stLocked.pBits;

		hr = pTextureOUT->LockRect( 0, &stLocked, NULL, 0 );
		if( FAILED(hr) )
		{
			GASSERT( 0 && _T("HRESULT hr = pTexture->LockRect( 0, &stLocked, NULL, 0 );") );
			pTexture->UnlockRect( 0 );
			return FALSE;
		}
		DWORD* pDataDEST = (DWORD*)stLocked.pBits;
		int iPitch = stLocked.Pitch / 4;

		float fAlpha(0.f);
		D3DXVECTOR4 vColorSRC(0.f,0.f,0.f,0.f);
		D3DXVECTOR4 vColorDEST(0.f,0.f,0.f,0.f);
		for( DWORD y=0; y<dwTexelHeight; ++y )
		{
			for( DWORD x=0; x<dwTexelWidth; ++x )
			{
				vColorSRC.w = (float) ((pDataSRC[y*iPitch+x]&0xff000000)>>24);
				vColorSRC.x = (float) ((pDataSRC[y*iPitch+x]&0xff0000)>>16);
				vColorSRC.y = (float) ((pDataSRC[y*iPitch+x]&0xff00)>>8);
				vColorSRC.z = (float) (pDataSRC[y*iPitch+x]&0xff);
				vColorSRC.w /= 255.f;
				vColorSRC.x /= 255.f;
				vColorSRC.y /= 255.f;
				vColorSRC.z /= 255.f;

				if ( vColorSRC.w > 0.f )
				{
					vColorDEST.x = vColorSRC.x / vColorSRC.w;
					vColorDEST.y = vColorSRC.y / vColorSRC.w;
					vColorDEST.z = vColorSRC.z / vColorSRC.w;
				}
				else
				{
					vColorDEST.x = 0.f;
					vColorDEST.y = 0.f;
					vColorDEST.z = 0.f;
				}
				vColorDEST.w = vColorSRC.w;

				if( vColorDEST.x < 0.f )	vColorDEST.x = 0.f;
				if( vColorDEST.y < 0.f )	vColorDEST.y = 0.f;
				if( vColorDEST.z < 0.f )	vColorDEST.z = 0.f;
				if( vColorDEST.w < 0.f )	vColorDEST.w = 0.f;
				if( vColorDEST.x > 1.f )	vColorDEST.x = 1.f;
				if( vColorDEST.y > 1.f )	vColorDEST.y = 1.f;
				if( vColorDEST.z > 1.f )	vColorDEST.z = 1.f;
				if( vColorDEST.w > 1.f )	vColorDEST.w = 1.f;

				pDataDEST[y*iPitch+x] = (static_cast<DWORD>(vColorDEST.w*255.f) << 24) +
					(static_cast<DWORD>(vColorDEST.x*255.f) << 16) +
					(static_cast<DWORD>(vColorDEST.y*255.f) << 8) +
					(static_cast<DWORD>(vColorDEST.z*255.f));
			}
		}

		pTextureOUT->UnlockRect( 0 );
		pTexture->UnlockRect( 0 );

		return TRUE;
	}

	BOOL OnFrameMove( LPDIRECT3DDEVICEQ pd3dDevice, EngineDeviceMan* pEngineDevice, DxSkinChar* pSkinChar, boost::shared_ptr<DxLandMan>	spLandMan, float fElapsedTime, const D3DXMATRIX& matWorld )
	{
		if ( m_pFlyCam != NULL )
		{
			m_pFlyCam->Update(0.0f, fElapsedTime);
			DxViewPort::GetInstance().SetViewTrans(
				DxViewPort::GetInstance().GetFromPt() + m_pFlyCam->GetReserveMove() 
				, DxViewPort::GetInstance().GetLookatPt() + m_pFlyCam->GetReserveLook()
				, D3DXVECTOR3(0.0f, 1.0f, 0.0f));
		}
		// Note : 각종 장치들.
		pEngineDevice->FrameMove( 0.f, fElapsedTime, TRUE );

		// Note : FrameMove
		spLandMan->FrameMove( g_fTimeADD, fElapsedTime, DxViewPort::GetInstance().GetClipVolume(), FALSE );
		pSkinChar->FrameMove( g_fTimeADD, fElapsedTime, FALSE, matWorld );

		float fSkinTime = pSkinChar->GETCURTIME ();

		// Note : 계속 작업.
		if ( pSkinChar->ISENDANIM() )
		{
			++m_nAnimIndex;
			// std::vector<std::string>::iterator itr = std::find(m_vecAnimname.begin(), m_vecAnimname.end(), pSkinChar->GETCURANIM()->m_strCfgFile);
			// itr++;
			//if ( itr == m_vecAnimname.end() )
			if ( static_cast<INT>(m_vecAnimname.size()) <= m_nAnimIndex )
			{
				pSkinChar->SELECTANI( AN_PLACID, AN_SUB_NONE );
				return FALSE;
			}
			pSkinChar->SELECTANI_EDIT(m_vecAnimname[m_nAnimIndex].c_str(), TRUE, EMANI_IGNORELOOP | EMANI_ONLYZEROFRAMESTART );
		}
	}

	BOOL OnFrameMoveADD( LPDIRECT3DDEVICEQ pd3dDevice, EngineDeviceMan* pEngineDevice, DxSkinChar* pSkinChar, boost::shared_ptr<DxLandMan>	spLandMan, float fElapsedTime, float fAddTime, const D3DXMATRIX& matWorld )
	{
		g_fTimeADD += fElapsedTime;
		if ( m_pFlyCam != NULL )
		{
			m_pFlyCam->Update(0.0f, fElapsedTime);
			DxViewPort::GetInstance().SetViewTrans(
				DxViewPort::GetInstance().GetFromPt() + m_pFlyCam->GetReserveMove() 
				, DxViewPort::GetInstance().GetLookatPt() + m_pFlyCam->GetReserveLook()
				, D3DXVECTOR3(0.0f, 1.0f, 0.0f));
		}

		// Note : 각종 장치들.
		pEngineDevice->FrameMove( 0.f, fElapsedTime, TRUE );

		// Note : FrameMove
		spLandMan->FrameMove( g_fTimeADD, fElapsedTime, DxViewPort::GetInstance().GetClipVolume(), FALSE );
		pSkinChar->FrameMove ( g_fTimeADD, fElapsedTime, FALSE, matWorld );

		// Note : 계속 작업.
		if( g_fTimeADD < fAddTime )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	void OnRender( LPDIRECT3DDEVICEQ pd3dDevice
		, DxSkinChar* pSkinChar
		, boost::shared_ptr<DxLandMan>	spLandMan
		, float fElapsedTime
		, BOOL bEff
		, const D3DXMATRIX& matWorld
		, INT nWidth, INT nHeight
		, LPDIRECT3DSURFACEQ pSurfaceq)
	{
		// Begin the scene
		if( SUCCEEDED( pd3dDevice->BeginScene() ) )
		{
			// Note : 월드에딧에서는 정보를 받아와야 하기 때문에 이 Code 가 추가됨;
			NSMaterialManager::SetFogParam( DxFogMan::GetInstance().GetFogStart_RealTime(), DxFogMan::GetInstance().GetFogEnd_RealTime() );

			//	Note : Glow 
			DxGlowMan::GetInstance().RenderTex ( pd3dDevice );

			//	Note : Light 설정;
			DxLightMan::GetInstance()->Render ( pd3dDevice, FALSE, FALSE );

			//	Note : 버텍스 쉐이더 콘스탄트 설정;
			DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
			pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIRECT, (float*)&Light.m_Light.Direction, 1 );
			pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIFFUSE, (float*)&Light.m_Light.Diffuse, 1 );
			pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTAMBIENT, (float*)&Light.m_Light.Ambient, 1 );

			D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
			pd3dDevice->SetVertexShaderConstantF ( VSC_CAMERAPOSITION, (float*)&vFromPt, 1 );

			//	Note : 포인트 라이트 VertexShaderConstant;
			D3DLIGHTQ	pLight;
			D3DXVECTOR4	vPointPos;
			D3DXVECTOR3	vPointDiff;
			for ( int i=0; i<3; ++i )
			{
				if ( DxLightMan::GetInstance()->GetClosedLight(i+1) )
				{
					pLight = DxLightMan::GetInstance()->GetClosedLight(i+1)->m_Light;
					vPointDiff = D3DXVECTOR3 ( pLight.Diffuse.r, pLight.Diffuse.g, pLight.Diffuse.b );
					vPointPos.x = pLight.Position.x;
					vPointPos.y = pLight.Position.y;
					vPointPos.z = pLight.Position.z;
					vPointPos.w = pLight.Range;
				}
				else
				{
					vPointPos = D3DXVECTOR4 ( 0.f, 0.f, 0.f, 0.1f );
					vPointDiff = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
				}
				pd3dDevice->SetVertexShaderConstantF (i*2+VSC_PLIGHTPOS01, (float*)&vPointPos, 1);
				pd3dDevice->SetVertexShaderConstantF (i*2+VSC_PLIGHTDIFF01, (float*)&vPointDiff, 1);
			}

			D3DXMATRIX matView = DxViewPort::GetInstance().GetMatView();
			D3DXMATRIX matProj = DxViewPort::GetInstance().GetMatProj();

			pd3dDevice->SetVertexShaderConstantF ( VSC_SKIN_DEFAULT, (float*)&D3DXVECTOR4 (1.f, 0.5f, 0.f, 765.01f), 1 );

			D3DXVECTOR3	vLightVector = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
			D3DXVec3TransformNormal ( &vLightVector, &vLightVector, &matView );
			D3DXVec3Normalize ( &vLightVector, &vLightVector);
			vLightVector = -vLightVector;
			pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIRECT_VIEW, (float*)&vLightVector, 1 );

			D3DXMatrixTranspose( &matView, &matView );
			D3DXMatrixTranspose( &matProj, &matProj );

			pd3dDevice->SetVertexShaderConstantF ( VSC_MATWVP_01, (float*)&matView, 4 );
			pd3dDevice->SetVertexShaderConstantF ( VSC_MATWVP_02, (float*)&matProj, 4 );

			D3DXVECTOR3	vDirect;
			vDirect.x	=  -Light.m_Light.Direction.x;
			vDirect.y	=  -Light.m_Light.Direction.y;
			vDirect.z	=  -Light.m_Light.Direction.z;
			D3DXVec3Normalize ( &vDirect, &vDirect );
			pd3dDevice->SetPixelShaderConstantF ( 0, (float*)&vDirect, 1 );				// 빛 방향
			pd3dDevice->SetPixelShaderConstantF ( 1, (float*)&Light.m_Light.Diffuse, 1 );
			pd3dDevice->SetPixelShaderConstantF ( 2, (float*)&Light.m_Light.Ambient, 1 );

			// PhysX 물리엔진 적용;
			NSPhysX::Render( fElapsedTime );

			
 			// 지형 그림자를 생성하도록 한다;
 			{
 				// SpotLight 셋팅.
 				//NSShadowLight::FrameMove( fElapsedTime );
 
 				DxShadowMap::GetInstance().ClearShadow ( pd3dDevice, spLandMan->GetUsedMaterialSystem() );
 
 				//m_pCharacter->RenderShadow( m_pd3dDevice, FALSE );
 				DxShadowMap::GetInstance().RenderShadowCharMob ( pSkinChar, matWorld, pd3dDevice, spLandMan->GetUsedMaterialSystem() ? FALSE : TRUE );
 
 				spLandMan->RenderShadow( pd3dDevice );
 
 				// PS_3_0_HIGH 이상일 경우만 동작한다;
 				//if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
 				{
 					spLandMan->RenderShadow_SpotLight1( pd3dDevice );
 					//m_pCharacter->RenderShadow_SpotLight( m_pd3dDevice, m_scpLandMan->GetUsedMaterialSystem(), 0 );
 					DxShadowMap::GetInstance().RenderShadowCharMob_SpotLight( pSkinChar, matWorld, pd3dDevice, spLandMan->GetUsedMaterialSystem(), 0 );
 
 					spLandMan->RenderShadow_SpotLight2( pd3dDevice );
 					//m_pCharacter->RenderShadow_SpotLight( m_pd3dDevice, m_scpLandMan->GetUsedMaterialSystem(), 1 );
 					DxShadowMap::GetInstance().RenderShadowCharMob_SpotLight( pSkinChar, matWorld, pd3dDevice, spLandMan->GetUsedMaterialSystem(), 1 );
 				}
 			}
 
 			// 환경맵;
 			{
 				DxEnvironment::GetInstance().ResetReflectionCV();
 				DxEnvironment::GetInstance().RenderReflectionMaterial( pd3dDevice, spLandMan.get() );
 			}
 
 			//////////////////////////////////////////////////////////////////////////
 			{
				D3DXMATRIX matIdentity;
				D3DXMatrixIdentity( &matIdentity );

 				NSDeffered::RenderDefferedForSM30NM
 					(
 					pd3dDevice,
 					D3DCOLOR_XRGB(0,0,0),
 					spLandMan.get(),
 					nWidth, nHeight,
 					pSurfaceq,
 					DxSurfaceTex::GetInstance().m_pColorSuf_1st,
 					DxSurfaceTex::GetInstance().m_pColorSuf_4th,
 					DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
 					DxSurfaceTex::GetInstance().m_pColorSuf_3rd,	// Color는 unsign64 를 쓰겠다.
 					DxSurfaceTex::GetInstance().m_pColorSuf_LBuffer,
 					DxSurfaceTex::GetInstance().m_pSufFourCC_INTZ,
 					DxSurfaceTex::GetInstance().m_pColorTex_1st,
 					DxSurfaceTex::GetInstance().m_pColorTex_4th,
 					DxSurfaceTex::GetInstance().m_pColorTex_2nd,
 					DxSurfaceTex::GetInstance().m_pColorTex_3rd, 	// Color는 unsign64 를 쓰겠다.
 					DxSurfaceTex::GetInstance().m_pColorTex_LBuffer,
 					DxSurfaceTex::GetInstance().m_pTexFourCC_INTZ, 
 					matIdentity,
 					DxViewPort::GetInstance().GetMatView(),
 					DxViewPort::GetInstance().GetMatProj(),
 					DxViewPort::GetInstance().GetClipVolume(),
 					FALSE
 					);
 				// 지형 물 렌더링
 				NSWaterMain::Render( pd3dDevice );
 			}
 
 			D3DLIGHTQ sLightBackUp;
 			if ( pSkinChar->IsUseWorldColor() )
 			{
 				pd3dDevice->GetLight( 0, &sLightBackUp );
 
 				D3DLIGHTQ sLight;
 				sLight = sLightBackUp;
 				sLight.Diffuse.r = 0.f;
 				sLight.Diffuse.g = 0.f;
 				sLight.Diffuse.b = 0.f;
 				pd3dDevice->SetLight( 0, &sLight );
 			}

			if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
			{
				pSkinChar->RenderFixed( pd3dDevice, TRUE, 1.f );
			}
			else
			{
				pSkinChar->SetPhysX_Cloth( TRUE );
				pSkinChar->RenderMaterialOpaque ( pd3dDevice, 1.f );
				pSkinChar->RenderMaterialHardAlpha ( pd3dDevice, 1.f );
				pSkinChar->RenderMaterialSoftAlpha ( pd3dDevice, 1.f );
				pSkinChar->RenderMaterialEffect ( pd3dDevice, 1.f );
			}

			// Land_EFF
 			{
 				//	RenderDefferedOpaqueForPSF
				D3DXMATRIX matIdentity;
				D3DXMatrixIdentity( &matIdentity );
 				D3DCOLOR colorClear = D3DCOLOR_XRGB(0,0,0);
 
 				{
 					NSDeffered::RenderDefferedForSM30NM
 						(
 						pd3dDevice,
 						D3DCOLOR_XRGB(0,0,0),
 						spLandMan.get(),
 						nWidth, nHeight,
 						pSurfaceq,						
 						DxSurfaceTex::GetInstance().m_pColorSuf_1st,
 						DxSurfaceTex::GetInstance().m_pColorSuf_4th,
 						DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
 						DxSurfaceTex::GetInstance().m_pColorSuf_3rd,	// Color는 unsign64 를 쓰겠다.
 						DxSurfaceTex::GetInstance().m_pColorSuf_LBuffer,
 						DxSurfaceTex::GetInstance().m_pSufFourCC_INTZ,
 						DxSurfaceTex::GetInstance().m_pColorTex_1st,
 						DxSurfaceTex::GetInstance().m_pColorTex_4th,
 						DxSurfaceTex::GetInstance().m_pColorTex_2nd,
 						DxSurfaceTex::GetInstance().m_pColorTex_3rd, 	// Color는 unsign64 를 쓰겠다.
 						DxSurfaceTex::GetInstance().m_pColorTex_LBuffer,
 						DxSurfaceTex::GetInstance().m_pTexFourCC_INTZ, 
 						matIdentity,
 						DxViewPort::GetInstance().GetMatView(),
 						DxViewPort::GetInstance().GetMatProj(),
 						DxViewPort::GetInstance().GetClipVolume(),
 						TRUE
 						);
 				}
 
 				spLandMan->RenderEff_Deffered( pd3dDevice, DxViewPort::GetInstance().GetClipVolume() );
 			}

			// Note : Optimize
			OPTMManager::GetInstance().Render_HardAlpha();
			OPTMManager::GetInstance().Render();

			//	Note : 글로우 처리를 하기위해 사용 한다.
 			DxGlowMan::GetInstance().RenderTex ( pd3dDevice );
 			DxGlowMan::GetInstance().Render( pd3dDevice );
 			DxGlowMan::GetInstance().RenderBURN( pd3dDevice );

			// HDR - Bloom 적용.
			//m_pd3dDevice->StretchRect( DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR, NULL, DxSurfaceTex::GetInstance().m_pColorBuffer, NULL, D3DTEXF_POINT );
			pd3dDevice->SetRenderTarget( 0, pSurfaceq );
/*
//			if ( RENDERPARAM::g_bEnable_HDR )
//			{
//				NSBloomHDR::OnRender( pd3dDevice );
//			}
			// 지금까지 그린것을 굴절을 위해 복사해놓는다.
//			DxEnvironment::GetInstance().UserToRefraction ( pd3dDevice );
// 
// 			//	Note : Wave 되야 하는것을 저장후 마지막으로 뿌린다.
// 			DxEnvironment::GetInstance().RenderWave ( pd3dDevice, DxEffectMan::GetInstance().GetBumpTexture() );
*/
			// End the scene.
			pd3dDevice->EndScene();
		}
	}

	void Create( LPDIRECT3DDEVICEQ pd3dDevice, 
				EngineDeviceMan* pEngineDevice,
				DxSkinCharData* pSkinCharDataSRC,
				const TCHAR* pCharName, 
				//const TCHAR* pAniName,
				std::vector<std::string>& vecAnimName,
				//const TCHAR* pLandName,
				boost::shared_ptr<DxLandMan> spLandMan,
				DWORD dwScreenSizeType,
				DWORD dwAA,
				DWORD dwFPS,
				float fAddTime,
				float fColorPower, 
				const D3DXMATRIX& matWorld,
				DWORD dwAnimSeqFlag,
				DWORD dwCurrentFrame,
				GLFlyCameraKeyFrameControl* pFlyCam)
	{
		LPDIRECT3DSURFACEQ pd3dBackBuffer;
		HRESULT hr = pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pd3dBackBuffer );
		D3DSURFACE_DESC		d3dsdBackBuffer;
		pd3dBackBuffer->GetDesc( &d3dsdBackBuffer );
		pd3dBackBuffer->Release();

		// FlyCamera set;
		m_pFlyCam = pFlyCam;
		if ( m_pFlyCam != NULL )
		{
			FlyCameraControl::SMsg msg(FlyCameraControl::EMFLYCAMERA_PLAY_ANIMATION, -1);
			m_pFlyCam->AddMessage(&msg);
			const GLFlyCameraKeyFrameControl::KEYFRAME_VEC& vecKeylist = m_pFlyCam->GetKeyFrameList();
			if ( vecKeylist.empty() == false )
			{
				// FlyCamera는 기본적으로 움직일시 상대적으로 움직인다;
				// 이 구간이 빠지면 시작위치부터 상대적으로 움직인다;
				DxViewPort::GetInstance().SetViewTrans(
					vecKeylist[0].sMoveToTargetID.vPos
					, vecKeylist[0].sLookToTargetID.vPos
					, D3DXVECTOR3(0.0f, 1.0f, 0.0f));
			}
		}
		
		m_nAnimIndex = 0;
		m_vecAnimname = vecAnimName;
		// Note :
		D3DCAPSQ d3dCaps;
		pd3dDevice->GetDeviceCaps ( &d3dCaps );

		// Note : 텍스쳐를 최대크기로 만들수 있도록 한다.
		int nWidth(1);
		int nHeight(1);
		while( d3dCaps.MaxTextureWidth > g_afScreenSizeX[dwScreenSizeType]*nWidth*2 )
		{
			nWidth *= 2;
		}
		while( d3dCaps.MaxTextureHeight > g_afScreenSizeY[dwScreenSizeType]*nHeight*2 )
		{
			nHeight *= 2;
		}
		int nMultiply(1);
		if( nWidth >= nHeight )		// 제일 작은값으로 사용한다.~!!
		{
			nMultiply = nHeight;
		}
		else
		{
			nMultiply = nWidth;
		}
		if( nMultiply > (int) dwAA )			// dwAA 배보다 클 필요는 없다.
		{
			nMultiply = dwAA;
		}
		DWORD dwFullSizeX = (DWORD) (g_afScreenSizeX[dwScreenSizeType]*nMultiply);
		DWORD dwFullSizeY = (DWORD) (g_afScreenSizeY[dwScreenSizeType]*nMultiply);

		{
			DxSurfaceTex::GetInstance().m_dwSetWidth = dwFullSizeX;
			DxSurfaceTex::GetInstance().m_dwSetHeight = dwFullSizeY;

			DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)dwFullSizeX, 
				(float)dwFullSizeY, 1.f, 2000.0f );
			hr = DxViewPort::GetInstance().SetViewPort ();
			DxViewPort::GetInstance().FrameMove(0.0f, 0.0f);

			pEngineDevice->InvalidateDeviceObjects();
			pEngineDevice->RestoreDeviceObjects();
		}

		g_fSkinTimePrev = -1.f;
		g_fTimeADD = 0.f;
		g_bFrameMoveADD = FALSE;	// 추가로 렌더링되는 시간.
		float fElapsedTime = 1.f/ (float)dwFPS;

		// Note : 폴더생성
		TSTRING strFolder = g_strPath + m_vecAnimname[0];//pAniName;
							
		strFolder = sc::util::GetName_ExtErase( strFolder );	// .x 를 없앰.
		char szWidth[255], szHeight[255];
		_itoa_s((INT)g_afScreenSizeX[dwScreenSizeType], szWidth, 255, 10);
		_itoa_s((INT)g_afScreenSizeY[dwScreenSizeType], szHeight, 255, 10);

		CString str;
		str.Format("%d", dwFPS);
		std::string strWidth(szWidth), strHeight(szHeight);
		strFolder += _T('_') + strWidth + _T('_') + strHeight + _T('_') + _T(str.GetString());

		if ( true == sc::file::isExist( strFolder.c_str() ) )
		{
			// _PatchUpload 폴더가 이미 존재
			//CORE_COMMON::UTIL::autoRename( strUploadFolder );
		}
		sc::file::createDirectory( strFolder.c_str() );

		// Note : DxSkinChar
		DxSkinChar* pSkinChar = NULL;
		{
			DxSkinCharData* pSkinCharData = DxSkinCharDataContainer::GetInstance().LoadData
			(
			pCharName,
			FALSE
			);

			// 조각 파일 삽입;
			for ( int i=0; i<PIECE_SIZE; ++i )
			{
				std::string strPiece;
#ifndef DEF_SKIN_SYNC
				DxSkinPieceBase* pSkinPieceBase = pSkinCharDataSRC->GetPiece(pd3dDevice, i, EMSCD_ZERO);
				if ( pSkinPieceBase == NULL )
				{
					pSkinCharData->ResetPieceEDIT(pd3dDevice, i);
					continue;
				}

				strPiece = pSkinPieceBase->GetFileName();
#else
				strPiece = pSkinCharDataSRC->GetPiece(EMPIECECHAR(i));
				if ( strPiece.empty() )
				{
					pSkinCharData->ResetPieceEDIT(pd3dDevice, i);
					continue;
				}
#endif
				HRESULT hrPieceEdit = pSkinCharData->SetPieceEDIT(strPiece.c_str(), pd3dDevice, EMSCD_ZERO, FG_BONEFILE_CHECK|FG_MUSTNEWLOAD);
				if ( FAILED(hrPieceEdit) )
				{
					if ( hr == E_DIFFERENT_BONEFILE )
						MessageBox ( NULL, _T("삽입하고자 하는 '조각'의 본파일과 현재 지정된 본파일이 틀립니다."), _T( "ERROR" ), MB_OK );

					return;
				}
			}

			if ( !pSkinCharData )
				return;

			pSkinChar = new DxSkinChar;
			pSkinChar->SetCharData( pSkinCharData, pd3dDevice, FALSE, EMSCD_SET_BASE_CPS | EMSCD_MESH_THREAD );

			pSkinChar->SELECTANI_EDIT( m_vecAnimname[0].c_str(), TRUE, EMANI_IGNORELOOP | EMANI_ONLYZEROFRAMESTART );
		}

		// Note : World
// 		boost::shared_ptr<DxLandMan>	spLandMan(new DxLandMan);
// 		{
// 			if ( spLandMan->LoadFile(pLandName, pd3dDevice) != TRUE )
// 				return;
// 
// 			spLandMan->ActiveMap(spLandMan);
// 		}

		// Note : 에딧 구 없앰.
		BOOL bCHAR_EDIT_RUN_BACKUP = g_bCHAR_EDIT_RUN;
		g_bCHAR_EDIT_RUN = FALSE;

		BOOL bEFFECT_SPHERE_VISIBLE_BACKUP = g_bEFFECT_SPHERE_VISIBLE;
		g_bEFFECT_SPHERE_VISIBLE = FALSE;


		// Projection 값을 구함.
		D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, g_afScreenSizeX[dwScreenSizeType]/g_afScreenSizeY[dwScreenSizeType], 5.f, 30000.f );

		UINT uiWidth( dwFullSizeX );
		UINT uiHeight( dwFullSizeY );
		D3DFORMAT emFormat( D3DFMT_A8R8G8B8 );

		// Note : 텍스쳐를 생성한다.
		LPDIRECT3DTEXTUREQ	pTexture;
		LPDIRECT3DSURFACEQ	pSurface;
		LPDIRECT3DSURFACEQ	pZBuffer;
		hr = TextureManager::GetInstance().CreateTexture(
			pd3dDevice,
			uiWidth, 
			uiHeight, 
			1, 
			D3DUSAGE_RENDERTARGET, 
			emFormat, 
			D3DPOOL_DEFAULT, 
			&pTexture,
            TEXTURE_RESIZE_NONE,
            TEXTURE_CATEGORY_SIZE );
		if( FAILED(hr) )
		{
			MessageBox( NULL, _T("[CreateTexture] 텍스쳐 생성에 실패하였습니다.."), _T( "ERROR" ), MB_OK );
			return;
		}

		hr = pTexture->GetSurfaceLevel( 0, &pSurface );
		if( FAILED(hr) )
		{
			return;
		}

		hr = pd3dDevice->CreateDepthStencilSurface(
            dwFullSizeX, 
			dwFullSizeY, 
			DxSurfaceTex::GetInstance().GetZFormat(),
			D3DMULTISAMPLE_NONE, 
			0, 
			TRUE, 
			&pZBuffer, 
			NULL );

		DxBackupTarget sBackupTarget;
        sBackupTarget.Backup( pd3dDevice, DxBackupTarget::ALL );

		{
			pd3dDevice->SetRenderTarget( 0, pSurface );
			pd3dDevice->SetDepthStencilSurface(DxSurfaceTex::GetInstance().m_pSufFourCC_INTZ);

			DWORD dwSeparateAlphaBlendEnable;
			pd3dDevice->GetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, &dwSeparateAlphaBlendEnable );

			pd3dDevice->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_ONE );
			pd3dDevice->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, dwSeparateAlphaBlendEnable );

			// 
			D3DXMATRIX matProjPREV;
			pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProjPREV );
			pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );
			
			{
				int n1000(0);
				int n100(0);
				int n10(0);
				int n1(0);
				TCHAR c100;
				TCHAR c10;
				TCHAR c1;
				TSTRING strTexture(_T(""));

				int nTexCount(0);
				BOOL bFrameRender(TRUE);
				while( bFrameRender )
				{
					// Note : 캐릭터를 렌더링한다.
					if( g_bFrameMoveADD )
					{
						bFrameRender = OnFrameMoveADD( pd3dDevice, pEngineDevice, pSkinChar, spLandMan, fElapsedTime, fAddTime, matWorld );
					}
					else
					{
						BOOL bFrame = OnFrameMove( pd3dDevice, pEngineDevice, pSkinChar, spLandMan, fElapsedTime, matWorld );
						if( !bFrame )
						{
							if( fAddTime > 0.f )
							{
								g_bFrameMoveADD = TRUE;
							}
							else
							{
								bFrameRender = FALSE;
							}
						}
					}

					if ( dwAnimSeqFlag & EMANISEQ_ONLYCURRENTFRAME )
					{
						bFrameRender = FALSE;

						const DxSkinAnimation* pAnimCont = pSkinChar->GETCURANIM ();
						
						pSkinChar->SETCURTIME_ALL_FOR_EDIT( (float) pAnimCont->m_UNITTIME * dwCurrentFrame );
						pSkinChar->SET_ATTBONE_CURTIME_ALL_FOR_EDIT( (float) pAnimCont->m_UNITTIME * dwCurrentFrame );
					}

					//////////////////////////////////////////////////////////////////////////
					// NSSkinAniThread
					NSSkinAniThread::CalcluateSkinAniControlForCharEditer( fElapsedTime );
					// NSEffectThread 대기

					NSEffectThread::Wait_FrameMove( pd3dDevice, fElapsedTime );

					// Note : 처음 렌더링
					pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L );
					OnRender( pd3dDevice, pSkinChar, spLandMan, fElapsedTime, FALSE, matWorld
						, uiWidth, uiHeight//g_afScreenSizeX[dwScreenSizeType], g_afScreenSizeY[dwScreenSizeType]
						, pSurface);

					// Note : 캐릭터만 BackUp
					LPDIRECT3DTEXTUREQ pTextureChar(NULL);
					LPDIRECT3DBASETEXTURE9 pBaseTexture(NULL);
					HRESULT hr = NSTexCompress::Compress
					(
						pd3dDevice,
						dwFullSizeX,
						dwFullSizeY,
						0,
						1,
						D3DFMT_A8R8G8B8,
						pTexture,
						pBaseTexture
					);

					if( FAILED(hr) )
					{
						MessageBox( NULL, _T("[NSTexCompress::Compress 1] 텍스쳐 생성에 실패하였습니다.."), _T( "ERROR" ), MB_OK );
						goto _RETURN;
					}

					pTextureChar = static_cast< LPDIRECT3DTEXTUREQ >( pBaseTexture );

					// Note : Color 를 1/Alpha값으로 곱하기한다.
					LPDIRECT3DTEXTUREQ pTextureOUT(NULL);
					if( !ColorMultiply( pd3dDevice, pTextureChar, pTextureOUT ) )
					{
						MessageBox( NULL, _T("[ColorAdd] ERROR"), _T( "ERROR" ), MB_OK );
						goto _RETURN;
					}
					SAFE_RELEASE( pTextureChar );

					// Note : 원하는 사이즈로 줄여줌.
					pBaseTexture = NULL;
					hr = NSTexCompress::Compress(
						pd3dDevice,
						(DWORD) g_afScreenSizeX[dwScreenSizeType],
						(DWORD) g_afScreenSizeY[dwScreenSizeType],
						0,
						1,
						D3DFMT_A8R8G8B8,
						pTextureOUT,
						pBaseTexture );
					SAFE_RELEASE( pTextureOUT );

					if( FAILED(hr) )
					{
						MessageBox( NULL, _T("[NSTexCompress::Compress 2] 텍스쳐 생성에 실패하였습니다.."), _T( "ERROR" ), MB_OK );
						goto _RETURN;
					}

					pTextureOUT = static_cast< LPDIRECT3DTEXTUREQ >( pBaseTexture );

					TCHAR tmpName[16];
					_itot_s(nTexCount, tmpName, 10);
					strTexture = strFolder + _T("\\") + tmpName + _T(".tga");

					// Note : 텍스쳐를 만든다.
					//D3DXSaveTextureToFile( strTexture.c_str(), D3DXIFF_TGA, pTextureAdd, NULL );
					D3DXSaveTextureToFile( strTexture.c_str(), D3DXIFF_TGA, pTextureOUT, NULL );
					SAFE_RELEASE( pTextureOUT );

					// Note : 텍스쳐 카운트 높임.
					++nTexCount;					
				}
			}
			pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProjPREV );
		}

_RETURN:
		sBackupTarget.Restore();

		// Note : 텍스쳐를 삭제한다.
 		SAFE_RELEASE( pTexture );
 		SAFE_RELEASE( pSurface );
		SAFE_RELEASE( pZBuffer );

		// Note : DxSkinChar
		SAFE_DELETE( pSkinChar );

		// Note : 에딧 구
		g_bCHAR_EDIT_RUN = bCHAR_EDIT_RUN_BACKUP;
		g_bEFFECT_SPHERE_VISIBLE = bEFFECT_SPHERE_VISIBLE_BACKUP;

		{
			DxSurfaceTex::GetInstance().m_dwSetWidth = 0;
			DxSurfaceTex::GetInstance().m_dwSetHeight = 0;

			DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)d3dsdBackBuffer.Width, 
				(float)d3dsdBackBuffer.Height, 1.f, 2000.0f );

			hr = DxViewPort::GetInstance().SetViewPort ();
			DxViewPort::GetInstance().FrameMove(0.0f, 0.0f);

			pEngineDevice->InvalidateDeviceObjects();
			pEngineDevice->RestoreDeviceObjects();
		}
	}

	void SetCustomScnWidth(INT nWidth)	{	g_afScreenSizeX[AF_CUSTOM_SCREENSIZE_IDX] = nWidth;	}
	void SetCustomScnHeight(INT nHeight) {	g_afScreenSizeY[AF_CUSTOM_SCREENSIZE_IDX] = nHeight; }
	INT GetCustomScnWidth(void) { return g_afScreenSizeX[AF_CUSTOM_SCREENSIZE_IDX]; }
	INT GetCustomScnHeight(void) { return g_afScreenSizeY[AF_CUSTOM_SCREENSIZE_IDX]; }
	INT GetScnWidth(INT idx) { return g_afScreenSizeX[idx]; };
	INT GetScnHeight(INT idx){ return g_afScreenSizeY[idx]; };
}
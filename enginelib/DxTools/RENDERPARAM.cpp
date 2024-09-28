#include "pch.h"
#include "../../SigmaCore/Encrypt/CompByte.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/File/StringFile.h"

#include "DebugSet.h"
#include "../Common/SUBPATH.h"
#include "DxFontMan.h"
#include "./DxSurfaceTex.h"
#include "./DxShadowMap.h"
#include "DxPostProcess.h"

#include <map>
#include <string>
#include <algorithm>
#include "../DxEffect/Frame/DxEffectMan.h"
#include "../Common/StlFunctions.h"
#include "../Common/gltexfile.h"

#include "RENDERPARAM.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace RENDERPARAM
{
	std::string			strRootPath			= "";
	std::string			strParamFile		= "";

	LPDIRECT3DDEVICEQ	g_pd3dDevice(NULL);

	EMCHARRENDER_TYPE	emCharRenderTYPE	= EMCRT_NORMAL;

	// Note : 감마
	float	fGamma			= 1.f;
	DWORD	dwOverBright	= 1;
	float	fContrast		= 1.f;

	// Note : Device 성능
	BOOL		m_bUseMIXED = FALSE;
	BOOL		g_bPixelShader_1_1 = FALSE;
	BOOL		g_bPixelShader_1_4 = FALSE;
	BOOL		g_bPixelShader_2 = FALSE;
	BOOL		g_bPixelShader_3 = FALSE;
	BOOL		g_bDefferedRendering = FALSE;		// 이것이 TRUE 여야만 기본Deffered가 활성화될수 있다.
													// D3DFMT_A16B16G16R16F 되면 왠만한건 됨.
													// 	예외 if ( d3dCaps.PrimitiveMiscCaps & D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS &&
													//			d3dCaps.PrimitiveMiscCaps & D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING )

	BOOL		g_bDefferedRendering_HIGH = FALSE;	// 이것이 TRUE 여야만 HIGH, VeryHigh 가 활성화될수 있다.
													// D3DFMT_A16B16G16R16 되면 왠만한건 됨.

	BOOL		g_bDefferedRendering_INTZ = FALSE;	// Direct DepthBuffer Access, Deffered 용
	BOOL		g_bDefferedRendering_DF16 = FALSE;	// Direct DepthBuffer Access, Deffered 용
	//BOOL		g_bDefferedRendering_RAWZ = FALSE;	// 이것 사용하지 않도록 한다.

	BOOL		g_bSurfaceG32R32F = FALSE;			// 이 Surface가 가능한가~?
													// 이게 되야 지형의 VeryHigh 가 활성화 될 수 있다.

	BOOL		g_bEnableHW_HDR = FALSE;			// HDR	// 하드웨어상 가능한가~?
	BOOL		g_bEnable_HDR = FALSE;				// HDR 
	BOOL		g_bSeparateAlphaBlend = FALSE;
	TnL_MODE	g_emTnLMode = TnL_FIXED;

	BOOL		g_bForceLowHardwareMode = FALSE;	// 강제 저사양모드 활성화 ON / OFF

	TEXTURE_FILTERING	g_emTextureFiltering = TEXTURE_FILTER_ANISOTROPIC_16X;

	// 옵션이 바뀐다고 바로 변경이 되지 않고 맵 이동시 옵션이 변경되도록 한다.
	BOOL				g_bChangeTexQuality = FALSE;						// 옵션이 변경되었는지 확인하는 코드
	TEXTURE_QUALITY		g_emTextureQuality_NEW = TEXTURE_QUALITY_HIGH;		// 변경된 옵션을 저장해 놓는다. ( 맵이동시 이 값이 g_emTextureQuality 로 저장된다. )
	TEXTURE_QUALITY		g_emTextureQuality = TEXTURE_QUALITY_HIGH;			// 현재 사용되는 옵션.

	PHYSX_CLOTH_LEVEL	g_emPhysXCloth = PHYSX_CLOTH_LEVEL_ALL;

	// 지형 Qulity
	// 지형제작시의 렌더링기준과 유저가 설정한 렌더링기준을 참조하여 지형렌더링 옵션을 정한다.
	TnL_MODE		g_emGroundQulity = TnL_NONE;
	TnL_MODE		g_emGroundQulityTHREAD = TnL_FIXED;

	// 캐릭터 Qulity
	// 지형제작시의 렌더링기준과 유저가 설정한 렌더링기준을 참조하여 캐릭터렌더링 옵션을 정한다.
	TnL_CHAR_MODE	g_emCharacterQulity = TnL_CHAR_FIXED;
	//TnL_CHAR_MODE	g_emCharacterQulityHW = TnL_CHAR_FIXED;

	BOOL		g_bFORCE_TnL_PS_3_0_DF = FALSE;

	BOOL	bEnableEEX = FALSE;
	BOOL	bEnableSSE = FALSE;

	// Note : 테스트
	BOOL	bCALCULATE_BONE = TRUE;
	BOOL	bCALCULATE_SSE = TRUE;
	BOOL	bRENDER_CHAR = TRUE;
	BOOL	bRENDER_WORLD = TRUE;
	BOOL	bRENDER_PIECE = TRUE;


	BOOL	bUseNormalMap = FALSE;
	BOOL	bMineEffect = FALSE;
	BOOL	bTargetEffect = FALSE;
	BOOL	bClickEffect = FALSE;
    BOOL    bVolumeLight = TRUE;

	DWORD	m_dwResolutionX = 800;
	DWORD	m_dwResolutionY = 600;
	DWORD	m_dwGpuVendor = 0;		// 0 - Etc, 1 - Intel, 2 - Nvidia, 3 - AMD
	DWORD	m_dwGpuModel = 0;
	DWORD	m_dwShaderModel = 0L;

	EMSERVICE_TYPE emSERVICE_TYPE = EMSERVICE_DEFAULT;
//	BOOL		   bXML_USE = TRUE;
	language::LANGFLAG emLangSet = language::DEFAULT;




	// MMX 제공하는지 체크
	bool isMMXSupported() 
	{ 
		int fSupported; 

		__asm 
		{ 
		mov eax,1 // CPUID level 1 
		cpuid  // EDX = feature flag 
		and edx,0x800000 // test bit 23 of feature flag 
		mov fSupported,edx // != 0 if MMX is supported 
		} 

		if(fSupported != 0)
		{
			return true; 
		}
		else 
		{
			return false; 
		}
	} 

	// SSE 제공하는지 체크
	bool isISSESupported() 
	{ 
		int processor; 
		int features; 
		int ext_features = 0; 

		__asm 
		{ 
			pusha 
			mov eax,1 
			cpuid 
			mov processor,eax     // Store processor family/model/step 
			mov features,edx      // Store features bits 

			mov eax,080000000h 
			cpuid 
			// Check which extended functions can be called 
			cmp eax,080000001h      // Extended Feature Bits 
			jb  no_features         // jump if not supported 

			mov eax,080000001h      // Select function 0x80000001 
			cpuid 
			mov ext_features,edx  // Store extended features bits 

		no_features: 
			popa 
		} 

		if (((features >> 25) & 1) != 0) 
		{
			return true; 
		}
		else 
		{
			if (((ext_features >> 22) & 1) != 0) 
			{
				return true; 
			}
			else 
			{
				return false; 
			}
		}
	}

	BOOL IsCheckDeviceFormat_A16B16G16R16F( LPDIRECT3DQ pD3D, const D3DCAPSQ* pd3dCaps, D3DFORMAT sFormat )
	{
		// 이 정수형으로 인해 Geforce 6000/7000 시리즈는 FALSE가 된다.
		if( SUCCEEDED( pD3D->CheckDeviceFormat(
			pd3dCaps->AdapterOrdinal,
			pd3dCaps->DeviceType,
			sFormat,
			D3DUSAGE_RENDERTARGET,
			D3DRTYPE_TEXTURE,
			D3DFMT_A16B16G16R16 ) ) )
		{
			if( SUCCEEDED( pD3D->CheckDeviceFormat(
				pd3dCaps->AdapterOrdinal,
				pd3dCaps->DeviceType,
				sFormat,
				D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
				D3DRTYPE_TEXTURE,
				D3DFMT_A16B16G16R16F ) ) )
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	void CheckSystemInfo( LPDIRECT3DQ pD3D, LPDIRECT3DDEVICEQ pd3dDevice )
	{
		// 해상도 Update
		{
			D3DSURFACE_DESC		d3dsdBackBuffer;
			LPDIRECT3DSURFACEQ pBackBuffer;
			pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
			pBackBuffer->GetDesc( &d3dsdBackBuffer );
			pBackBuffer->Release();

			RENDERPARAM::m_dwResolutionX = d3dsdBackBuffer.Width;
			RENDERPARAM::m_dwResolutionY = d3dsdBackBuffer.Height;
		}

		// GPU 정보 Update
		{
			D3DADAPTER_IDENTIFIER9 sAdapter;
			pD3D->GetAdapterIdentifier( D3DADAPTER_DEFAULT, 0, &sAdapter );

			std::string	strName = sAdapter.Description;
			std::transform( strName.begin(), strName.end(), strName.begin(), tolower );

			TSTRING::size_type idx;
			idx = strName.find( _T("intel") );
			if( idx!=TSTRING::npos )
			{
				RENDERPARAM::m_dwGpuVendor = 1;
			}

			idx = strName.find( _T("nvidia") );
			if( idx!=TSTRING::npos )
			{
				RENDERPARAM::m_dwGpuVendor = 2;
			}

			idx = strName.find( _T("geforce") );
			if( idx!=TSTRING::npos )
			{
				RENDERPARAM::m_dwGpuVendor = 2;
			}

			idx = strName.find( _T("gf") );
			if( idx!=TSTRING::npos )
			{
				RENDERPARAM::m_dwGpuVendor = 2;
			}

			idx = strName.find( _T("amd") );
			if( idx!=TSTRING::npos )
			{
				RENDERPARAM::m_dwGpuVendor = 3;
			}

			idx = strName.find( _T("ati") );
			if( idx!=TSTRING::npos )
			{
				RENDERPARAM::m_dwGpuVendor = 3;
			}

			idx = strName.find( _T("radeon") );
			if( idx!=TSTRING::npos )
			{
				RENDERPARAM::m_dwGpuVendor = 3;
			}

			idx = strName.find( _T("igp") );
			if( idx!=TSTRING::npos )
			{
				RENDERPARAM::m_dwGpuVendor = 4;
			}

			BOOL bFirst(TRUE);
			BOOL bLink(TRUE);
			TSTRING strGpuModel(_T(""));
			for ( DWORD i=0; i<strName.size(); ++i )
			{
				if ( strName[i] >= '0' && strName[i] <= '9' )
				{
					// 숫자 사이에 문자가 있을 경우 같은문자 3개를 추가로 넣어준다.
					if ( !bLink )
					{
						strGpuModel += strName[i];
						strGpuModel += strName[i];
						strGpuModel += strName[i];
					}

					strGpuModel += strName[i];

					bFirst = FALSE;
					bLink = TRUE;
				}
				else
				{
					if ( !bFirst )
					{
						bLink = FALSE;
					}
				}
			}

			RENDERPARAM::m_dwGpuModel = atoi(strGpuModel.c_str());
		}
	}

	void DeviceCheck( LPDIRECT3DQ pD3D, LPDIRECT3DDEVICEQ pd3dDevice )
	{
		D3DCAPSQ d3dCaps;
		pd3dDevice->GetDeviceCaps ( &d3dCaps );

		D3DSURFACE_DESC		d3dsdBackBuffer;
		LPDIRECT3DSURFACEQ pBackBuffer;
		pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
		pBackBuffer->GetDesc( &d3dsdBackBuffer );
		pBackBuffer->Release();

		//	Note : MIXED
		D3DDEVICE_CREATION_PARAMETERS d3dCreateParam;
		pd3dDevice->GetCreationParameters ( &d3dCreateParam );
		if( d3dCreateParam.BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING )
		{
			m_bUseMIXED = TRUE;
		}

		//	Note : Vertex Shader
		if ( m_bUseMIXED &&	d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1) )
		{
			if ( d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1, 1) )
			{
				m_dwShaderModel = 1;
				g_bPixelShader_1_1 = TRUE;
			}
			if ( d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1, 4) )
			{
				g_bPixelShader_1_4 = TRUE;
			}

			//// 이 수치가 -1 로 셋팅되어 있으면 안된다.
			//// Intel(R) Q45/Q43 Express Chipset 은 shadermodel 은 3.0 이 지원된다고 나오지만 이렇게 셋팅되어있다.
			//// 렌더링 결과를 보면 sm.2.0 에서 렌더링이 정상으로 되질 않는다.
			//// 시간이 충분하다면 찾아볼만 하지만, 아직은 그럴 필요성을 못 느껴서 sm.1.4 버젼만 동작되도록 한다.
			//if ( d3dCaps.MaxVShaderInstructionsExecuted == -1 && 
			//	d3dCaps.MaxPShaderInstructionsExecuted == -1 && 
			//	DxSurfaceTex::GetInstance().IsLowLowGPU() )
			//{
			//	// 문제가 있는 그래픽카드
			//	// Intel 문 저사양.. 
			//	// 그런데 최신 ATI 그래픽 카드도 d3dCaps.MaxVShaderInstructionsExecuted = -1 이다 ㅜㅜ
			//}
			//else
			{
				if ( d3dCaps.PixelShaderVersion >= D3DPS_VERSION(2, 0) )
				{
					m_dwShaderModel = 2;
					g_bPixelShader_2 = TRUE;
				}
				if ( d3dCaps.PixelShaderVersion >= D3DPS_VERSION(3, 0) )
				{
					m_dwShaderModel = 3;
					g_bPixelShader_3 = TRUE;
				}
			}
		}

		if( SUCCEEDED( pD3D->CheckDeviceFormat( d3dCaps.AdapterOrdinal,
											d3dCaps.DeviceType,
											d3dsdBackBuffer.Format,
											D3DUSAGE_RENDERTARGET,
											D3DRTYPE_TEXTURE,
											D3DFMT_A16B16G16R16F ) ) )
		{
			if ( g_bPixelShader_2 )
			{
				g_bEnableHW_HDR = TRUE;
			}
		}

		if ( d3dCaps.NumSimultaneousRTs >= 3 )
		{
			

			// Geforce 7600GS - D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS : FALSE
			//					D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING : TRUE
			//
			if ( d3dCaps.PrimitiveMiscCaps & D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS &&
				d3dCaps.PrimitiveMiscCaps & D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING )
			{
				if( SUCCEEDED( pD3D->CheckDeviceFormat(
					d3dCaps.AdapterOrdinal,
					d3dCaps.DeviceType,
					d3dsdBackBuffer.Format,
					D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
					D3DRTYPE_TEXTURE,
					D3DFMT_A8R8G8B8 ) ) )
				{
					g_bDefferedRendering = TRUE;
				}

				if( FAILED( pD3D->CheckDeviceFormat(
					d3dCaps.AdapterOrdinal,
					d3dCaps.DeviceType,
					d3dsdBackBuffer.Format,
					D3DUSAGE_RENDERTARGET,
					D3DRTYPE_TEXTURE,
					D3DFMT_A16B16G16R16F ) ) )
				{
					g_bDefferedRendering = FALSE;
				}
			}
		}

		if ( d3dCaps.NumSimultaneousRTs >= 4 && g_bDefferedRendering )
		{
			D3DSURFACE_DESC		d3dsdBackBuffer;
			LPDIRECT3DSURFACEQ pBackBuffer;
			pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
			pBackBuffer->GetDesc( &d3dsdBackBuffer );
			pBackBuffer->Release();

			D3DFORMAT m_FormatINTZ = (D3DFORMAT)MAKEFOURCC('I','N','T','Z');
			if( SUCCEEDED( pD3D->CheckDeviceFormat(
				d3dCaps.AdapterOrdinal,
				d3dCaps.DeviceType,
				d3dsdBackBuffer.Format,
				D3DUSAGE_DEPTHSTENCIL,
				D3DRTYPE_TEXTURE,
				m_FormatINTZ ) ) )
			{
				if ( IsCheckDeviceFormat_A16B16G16R16F( pD3D, &d3dCaps, d3dsdBackBuffer.Format ) )
				{
					g_bDefferedRendering_HIGH = TRUE;
					g_bDefferedRendering_INTZ = TRUE;
				}
			}
			else
			{
				// R.A.W.Z 는 쓰지 않도록 하자.
				// Nvidia 6000, 7000 시리즈에서 사용가능하지만, 사용시 특별한 작업이 들어가기도 한다.
				// 이것으로 인해 HIGH, VeryHigh 는 사용 불가가 된다.
				//D3DFORMAT sFormatRAWZ = (D3DFORMAT)MAKEFOURCC('R','A','W','Z');
				//if( SUCCEEDED( pD3D->CheckDeviceFormat(
				//	d3dCaps.AdapterOrdinal,
				//	d3dCaps.DeviceType,
				//	d3dsdBackBuffer.Format,
				//	D3DUSAGE_DEPTHSTENCIL,
				//	D3DRTYPE_TEXTURE,
				//	sFormatRAWZ ) ) )
				//{
				//	if ( IsCheckDeviceFormat_A16B16G16R16F( pD3D, &d3dCaps, d3dsdBackBuffer.Format ) )
				//	{
				//		g_bDefferedRendering_HIGH = TRUE;
				//		g_bDefferedRendering_RAWZ = TRUE;
				//	}
				//}
				//else
				{
					D3DFORMAT sFormatDF16 = (D3DFORMAT)MAKEFOURCC('D','F','1','6');
					if( SUCCEEDED( pD3D->CheckDeviceFormat(
						d3dCaps.AdapterOrdinal,
						d3dCaps.DeviceType,
						d3dsdBackBuffer.Format,
						D3DUSAGE_DEPTHSTENCIL,
						D3DRTYPE_TEXTURE,
						sFormatDF16 ) ) )
					{
						if ( IsCheckDeviceFormat_A16B16G16R16F( pD3D, &d3dCaps, d3dsdBackBuffer.Format ) )
						{
							g_bDefferedRendering_HIGH = TRUE;
							g_bDefferedRendering_DF16 = TRUE;
						}
					}
				}
			}

			// 아무리 못해도 기본 Deffered 는 되야 D3DFMT_G32R32F 를 사용할 수 있다.
			// Deffered 도 안되면서 D3DFMT_G32R32F 쓰는건 무리다.
			if ( g_bDefferedRendering_HIGH )
			{
				if( SUCCEEDED( pD3D->CheckDeviceFormat(
					d3dCaps.AdapterOrdinal,
					d3dCaps.DeviceType,
					d3dsdBackBuffer.Format,
					D3DUSAGE_RENDERTARGET,
					D3DRTYPE_TEXTURE,
					D3DFMT_G32R32F ) ) )
				{
					g_bSurfaceG32R32F = TRUE;
				}
			}
		}

		if ( d3dCaps.PrimitiveMiscCaps & D3DPMISCCAPS_SEPARATEALPHABLEND )
		{
			g_bSeparateAlphaBlend = TRUE;
		}
		
		// Note : MMX 와 SSE의 지원 여부 체크.
		if( isMMXSupported() )
		{
			bEnableEEX = TRUE;
		}
		if( isISSESupported() )
		{
			bEnableSSE = TRUE;
		}
	}

	BOOL LOAD( LPDIRECT3DQ pD3D, LPDIRECT3DDEVICEQ pd3dDevice, const char *szRootPath )
	{
        // Note : Device를 체크한다.
		DeviceCheck( pD3D, pd3dDevice );

		// 사용되는 TnL Mode 를 선택한다.
		CheckTnLMode();

		//strRootPath = szRootPath;
		//strParamFile = strRootPath + "\\RenderParam.ini";

		//gltexfile cFILE;
		//cFILE.reg_sep("\t ");

		//if (!cFILE.open(strParamFile, false))
  //          return FALSE;

		//int nTEMP(0);

		////	디스플레이 설정.
		//cFILE.getflag ( "emCharRenderTYPE", 1, 1, emCharRenderTYPE );

		//// Note : 감마
		//cFILE.getflag ( "fGamma", 1, 1, fGamma );
		//cFILE.getflag ( "dwOverBright", 1, 1, dwOverBright );
		//cFILE.getflag ( "fContrast", 1, 1, fContrast );


		//// Note : 테스트
		//cFILE.getflag ( "bCALCULATE_BONE", 1, 1, bCALCULATE_BONE );
		//cFILE.getflag ( "bCALCULATE_SSE", 1, 1, bCALCULATE_SSE );
		//cFILE.getflag ( "bRENDER_CHAR", 1, 1, bRENDER_CHAR );
		//cFILE.getflag ( "bRENDER_WORLD", 1, 1, bRENDER_WORLD );
		//cFILE.getflag ( "bRENDER_PIECE", 1, 1, bRENDER_PIECE );
	

  //      // Note : SSE 가 지원하면 EMCRT_SOFTWARE, 안하면 EMCRT_NORMAL;
		////		혹시 EMCRT_SOFTWARE 나 EMCRT_NORMAL 이 여기서 뭐 해야 할것이 있다면,
		////		EMCRT_NON_SOFT 작업을 위쪽으로 빼 내야 한다.
		////		
		//switch( emCharRenderTYPE )
		//{
		//case EMCRT_NON_SOFT:
		//	if( bEnableSSE )
		//	{
		//		emCharRenderTYPE = EMCRT_SOFTWARE;
		//	}
		//	else
		//	{
		//		emCharRenderTYPE = EMCRT_NORMAL;
		//	}
		//	break;
		//case EMCRT_VERTEX:
		//	if( !m_bUseMIXED )	emCharRenderTYPE = EMCRT_NORMAL;
		//	break;
		//case EMCRT_PIXEL:
		//	if( !g_bPixelShader_2 )	emCharRenderTYPE = EMCRT_VERTEX;
		//	if( !m_bUseMIXED )		emCharRenderTYPE = EMCRT_NORMAL;
		//	break;
		//};

		return TRUE;
	}

	void CheckTnLMode()
	{
		//////////////////////////////////////////////////////////////////////////
		// 강제 최저사양 시작.
		if ( g_bForceLowHardwareMode )
		{
			if ( g_bPixelShader_1_1 )
			{
				g_emTnLMode = TnL_PS_1_1;
			}
			else
			{
				g_emTnLMode = TnL_FIXED;
			}

			if ( g_bPixelShader_2 )
			{
				// 이전에는 g_bPixelShader_1_1 조건 체크였지만, s_m_cos_airport_body.cps, s_m_cos_airport_leg.cps 가
				// 특정 컴퓨터(홍근씨컴퓨터)에서 버그가 발생하여 2.0 으로 변경하니 정상작동하였다.
				// 원인 파악은 안되서 g_bPixelShader_2 이상만 되도록 한다.
				g_emCharacterQulity = TnL_CHAR_PS_1_1;
			}
			else
			{
				// 옵션으로는 이곳에 들어올 수 없다.
				g_emCharacterQulity = TnL_CHAR_FIXED;
			}

			g_bEnable_HDR = FALSE;

			return;
		}

		//////////////////////////////////////////////////////////////////////////
		// 지형
		if ( g_bPixelShader_3 && 
			g_bDefferedRendering_HIGH &&
			g_bSurfaceG32R32F &&
			DxShadowMap::GetInstance().GetShadowDetail() >= LAND_QT_HIGH_HIGH && 
			DxSurfaceTex::GetInstance().m_pColorTex_1st &&
			DxSurfaceTex::GetInstance().m_pColorTex_2nd &&
			DxSurfaceTex::GetInstance().m_pColorTex_3rd &&
			DxSurfaceTex::GetInstance().m_pColorTex_4th &&
			DxSurfaceTex::GetInstance().m_p512Tex_4th &&
			DxSurfaceTex::GetInstance().m_pShadowTexG32R32F_VSM_CSM2 && 
			DxSurfaceTex::GetInstance().m_pShadowTexR32F_1024 && 
			DxSurfaceTex::GetInstance().m_pShadowTexR32F_CSM &&
			DxSurfaceTex::GetInstance().m_pShadowTexR32F_PL1 && 
			DxSurfaceTex::GetInstance().m_pShadowTexR32F_PL2 )
		{
			g_emTnLMode = TnL_PS_3_0_CSM;
		}
		else if ( g_bPixelShader_3 && 
			g_bDefferedRendering_HIGH &&
			DxShadowMap::GetInstance().GetShadowDetail() >= LAND_QT_HIGH && 
			DxSurfaceTex::GetInstance().m_pShadowTexG32R32F_VSM_CSM2 && 
			DxSurfaceTex::GetInstance().m_pColorTex_1st &&
			DxSurfaceTex::GetInstance().m_pColorTex_2nd &&
			DxSurfaceTex::GetInstance().m_pColorTex_3rd &&
			DxSurfaceTex::GetInstance().m_pColorTex_4th &&
			DxSurfaceTex::GetInstance().m_p512Tex_4th &&
			DxSurfaceTex::GetInstance().m_pShadowTexR32F )
		{
			g_emTnLMode = TnL_PS_3_0_NM;
		}
		else if ( g_bPixelShader_3 && 
			DxShadowMap::GetInstance().GetShadowDetail() >= LAND_QT_MIDDLE && 
			DxSurfaceTex::GetInstance().m_pColorTex_1st &&
			DxSurfaceTex::GetInstance().m_pColorTex_2nd &&
			DxSurfaceTex::GetInstance().m_pColorTex_4th &&
			DxSurfaceTex::GetInstance().m_p512Tex_4th &&
			DxSurfaceTex::GetInstance().m_pShadowTexR32F )
		{
			g_emTnLMode = TnL_PS_3_0_DF;
		}
		else if ( g_bPixelShader_1_1 )
		{
			g_emTnLMode = TnL_PS_1_1;
		}
		else
		{
			// [shhan][2014.09.12] 정책을 바꿔서 g_bPixelShader_1_1 지원이 안되는 것만 TnL_FIXED 로 동작되도록 한다.
			//						옵션으로는 이곳에 들어올 수 없다.
			g_emTnLMode = TnL_FIXED;
		}


		////////////////////////////////////////////////////////////////////////
		// 캐릭터 
		if ( g_bPixelShader_3 && 
			DxEffectMan::GetInstance().GetSkinDetail() >= SKD_BEST &&
			DxSurfaceTex::GetInstance().m_pShadowTexR32F )
		{
			g_emCharacterQulity = TnL_CHAR_PS_3_0;
		}
		else if ( g_bPixelShader_2 && 
			DxEffectMan::GetInstance().GetSkinDetail() >= SKD_SPEC )
		{
			g_emCharacterQulity = TnL_CHAR_PS_2_0;
		}
		else if ( g_bPixelShader_2 )
		{
			// 이전에는 g_bPixelShader_1_1 조건 체크였지만, s_m_cos_airport_body.cps, s_m_cos_airport_leg.cps 가
			// 특정 컴퓨터(홍근씨컴퓨터)에서 버그가 발생하여 2.0 으로 변경하니 정상작동하였다.
			// 원인 파악은 안되서 g_bPixelShader_2 이상만 되도록 한다.
			g_emCharacterQulity = TnL_CHAR_PS_1_1;
		}
		else
		{
			// 옵션으로는 이곳에 들어올 수 없다.
			g_emCharacterQulity = TnL_CHAR_FIXED;
		}


		//////////////////////////////////////////////////////////////////////////
		// HDR
		//if ( g_emCharacterQulity >= TnL_CHAR_PS_3_0 && g_bEnableHW_HDR )
		if ( g_bEnableHW_HDR && 
			DxPostProcess::GetInstance().IsOptionEnable() && 
			g_emCharacterQulity >= TnL_CHAR_PS_2_0 &&
			DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR &&
			DxSurfaceTex::GetInstance().m_pBloomBrightSuf &&
			DxSurfaceTex::GetInstance().m_pBloomDownSampleSuf &&
			DxSurfaceTex::GetInstance().m_pBloomDownSampleSufH )
		{
			g_bEnable_HDR = TRUE;
		}
		else
		{
			g_bEnable_HDR = FALSE;
		}


		// 강제 고정파이프라인 렌더링.
		//g_emCharacterQulity = TnL_CHAR_FIXED;
	}

	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		g_pd3dDevice = pd3dDevice;
	}

	void SetTextureFiltering( TEXTURE_FILTERING emTexFilter )
	{
		g_emTextureFiltering = emTexFilter;

		DWORD dwMaxAnisotropy(1);
		DWORD dwMinFilter(D3DTEXF_LINEAR);
		DWORD dwMagFilter(D3DTEXF_LINEAR);
		DWORD dwMipFilter(D3DTEXF_LINEAR);
		switch ( RENDERPARAM::g_emTextureFiltering )
		{
		//case TEXTURE_FILTER_BILINEAR:
		//	dwMipFilter = D3DTEXF_POINT;	// intel 내장에서도 속도차이가 없다.
		//	break;

		case TEXTURE_FILTER_TRILINEAR:
			break;

		case TEXTURE_FILTER_ANISOTROPIC_2X:
			dwMaxAnisotropy = 2;
			break;

		case TEXTURE_FILTER_ANISOTROPIC_4X:
			dwMaxAnisotropy = 4;
			break;

		case TEXTURE_FILTER_ANISOTROPIC_8X:
			dwMaxAnisotropy = 8;
			break;

		case TEXTURE_FILTER_ANISOTROPIC_16X:
			dwMaxAnisotropy = 16;
			break;

		default:
			break;
		};


		D3DCAPSQ d3dCaps;
		g_pd3dDevice->GetDeviceCaps ( &d3dCaps );
		if ( dwMaxAnisotropy >= 2 && d3dCaps.MaxAnisotropy >= 2 )
		{
			dwMinFilter = D3DTEXF_ANISOTROPIC;
			dwMaxAnisotropy = dwMaxAnisotropy > d3dCaps.MaxAnisotropy ? d3dCaps.MaxAnisotropy : dwMaxAnisotropy;

			for ( DWORD i=0; i<8; ++i )
			{
				g_pd3dDevice->SetSamplerState( i, D3DSAMP_MAXANISOTROPY, dwMaxAnisotropy );
			}
		}


		for ( DWORD i=0; i<8; ++i )
		{
			g_pd3dDevice->SetSamplerState( i, D3DSAMP_MINFILTER, dwMinFilter );
			g_pd3dDevice->SetSamplerState( i, D3DSAMP_MAGFILTER, dwMagFilter );
			g_pd3dDevice->SetSamplerState( i, D3DSAMP_MIPFILTER, dwMipFilter );
		}
	}

	void SetTextureQuality( TEXTURE_QUALITY emTexQuality )
	{
		// Option 파일에서 처음 로딩 할 경우를 대비해 NEW 에도 값을 넣는다.
		g_emTextureQuality_NEW = emTexQuality;

		if ( g_emTextureQuality != emTexQuality )
		{
			g_bChangeTexQuality = TRUE;
		}
		else
		{
			g_bChangeTexQuality = FALSE;
		}
	}

	// 강제로 옵션을 넣어버린다.
	// Lobby 에서는 Texture Quality 를 최고로 보여줘야되어서 이 함수가 생겨났다.
	void SetTextureQuality_FORCE( TEXTURE_QUALITY emTexQuality )
	{
		g_bChangeTexQuality = TRUE;
		g_emTextureQuality = emTexQuality;
	}

	// 맵이동 할 때나 적용되도록 한다.
	void ApplyTextureQuality()
	{
		if ( g_bChangeTexQuality )
		{
			g_emTextureQuality = g_emTextureQuality_NEW;
		}
		g_bChangeTexQuality = FALSE;
	}
}


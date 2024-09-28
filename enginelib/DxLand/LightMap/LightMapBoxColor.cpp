#include "pch.h"

#include "../../DxTools/RENDERPARAM.h"
#include "../../G-Logic/GLPeriod.h"
#include "../DxPiece/NSPieceQuickSort.h"
#include "../NSLandThread.h"
#include "../DxLandMan.h"
#include "./NSLightMapFX.h"
#include "./NSLightMapTools.h"

#include "LightMapBoxColor.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


LightMapBoxColor::LightMapBoxColor( const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax, const D3DXMATRIX& matWorld )
{
	D3DXVECTOR3 vMaxNew = vMax;
	D3DXVECTOR3 vMinNew = vMin;
	D3DXMatrixAABBTransform( vMaxNew, vMinNew, matWorld );

	float fRate[SAMPLING_LINE] = { 0.125f, 0.375f, 0.625f, 0.875f };

	// +X
	for ( DWORD y=0; y<SAMPLING_LINE; ++y )
	{
		for ( DWORD x=0; x<SAMPLING_LINE; ++x )
		{
			m_sDayNightBox.m_avPos[0][y*SAMPLING_LINE+x] = D3DXVECTOR3( vMaxNew.x, vMinNew.y+((vMaxNew.y-vMinNew.y)*fRate[x]), vMinNew.z+((vMaxNew.z-vMinNew.z)*fRate[y]) );
		}
	}
	m_sDayNightBox.m_avNor[0] = D3DXVECTOR3( 1.f, 0.f, 0.f );
	m_sDayNightBox.m_avCombine[0] = D3DXVECTOR3( 1.f, 1.f, 1.f );

	// -X
	for ( DWORD y=0; y<SAMPLING_LINE; ++y )
	{
		for ( DWORD x=0; x<SAMPLING_LINE; ++x )
		{
			m_sDayNightBox.m_avPos[1][y*SAMPLING_LINE+x] = D3DXVECTOR3( vMinNew.x, vMinNew.y+((vMaxNew.y-vMinNew.y)*fRate[x]), vMinNew.z+((vMaxNew.z-vMinNew.z)*fRate[y]) );
		}
	}
	m_sDayNightBox.m_avNor[1] = D3DXVECTOR3( -1.f, 0.f, 0.f );
	m_sDayNightBox.m_avCombine[1] = D3DXVECTOR3( 1.f, 1.f, 1.f );

	// +Y
	for ( DWORD y=0; y<SAMPLING_LINE; ++y )
	{
		for ( DWORD x=0; x<SAMPLING_LINE; ++x )
		{
			m_sDayNightBox.m_avPos[2][y*SAMPLING_LINE+x] = D3DXVECTOR3( vMinNew.x+((vMaxNew.x-vMinNew.x)*fRate[x]), vMaxNew.y, vMinNew.z+((vMaxNew.z-vMinNew.z)*fRate[y]) );
		}
	}
	m_sDayNightBox.m_avNor[2] = D3DXVECTOR3( 0.f, 1.f, 0.f );
	m_sDayNightBox.m_avCombine[2] = D3DXVECTOR3( 1.f, 1.f, 1.f );

	// -Y
	for ( DWORD y=0; y<SAMPLING_LINE; ++y )
	{
		for ( DWORD x=0; x<SAMPLING_LINE; ++x )
		{
			m_sDayNightBox.m_avPos[3][y*SAMPLING_LINE+x] = D3DXVECTOR3( vMinNew.x+((vMaxNew.x-vMinNew.x)*fRate[x]), vMinNew.y, vMinNew.z+((vMaxNew.z-vMinNew.z)*fRate[y]) );
		}
	}
	m_sDayNightBox.m_avNor[3] = D3DXVECTOR3( 0.f, -1.f, 0.f );
	m_sDayNightBox.m_avCombine[3] = D3DXVECTOR3( 1.f, 1.f, 1.f );	

	// +Z
	for ( DWORD y=0; y<SAMPLING_LINE; ++y )
	{
		for ( DWORD x=0; x<SAMPLING_LINE; ++x )
		{
			m_sDayNightBox.m_avPos[4][y*SAMPLING_LINE+x] = D3DXVECTOR3( vMinNew.x+((vMaxNew.x-vMinNew.x)*fRate[x]), vMinNew.y+((vMaxNew.y-vMinNew.y)*fRate[y]), vMaxNew.z );
		}
	}
	m_sDayNightBox.m_avNor[4] = D3DXVECTOR3( 0.f, 0.f, 1.f );
	m_sDayNightBox.m_avCombine[4] = D3DXVECTOR3( 1.f, 1.f, 1.f );

	// -Z
	for ( DWORD y=0; y<SAMPLING_LINE; ++y )
	{
		for ( DWORD x=0; x<SAMPLING_LINE; ++x )
		{
			m_sDayNightBox.m_avPos[5][y*SAMPLING_LINE+x] = D3DXVECTOR3( vMinNew.x+((vMaxNew.x-vMinNew.x)*fRate[x]), vMinNew.y+((vMaxNew.y-vMinNew.y)*fRate[y]), vMinNew.z );
		}
	}
	m_sDayNightBox.m_avNor[5] = D3DXVECTOR3( 0.f, 0.f, -1.f );
	m_sDayNightBox.m_avCombine[5] = D3DXVECTOR3( 1.f, 1.f, 1.f );

	for ( DWORD i=0; i<TEST_COUNT; ++i )
	{
		m_sDayNightBox.m_avDirectColor[i] = D3DXVECTOR3( 1.f, 1.f, 1.f );
		m_sDayNightBox.m_avIndirectColor[i] = D3DXVECTOR3( 0.f, 0.f, 0.f );
		m_sDayNightBox.m_avColorNoDynamicLight[i] = D3DXVECTOR3( 0.f, 0.f, 0.f );
		m_sDayNightBox.m_avColorDirectionDirect[i] = D3DXVECTOR3( 0.f, 0.f, 0.f );
		D3DXVec3Normalize( &m_sDayNightBox.m_avNor[i], &m_sDayNightBox.m_avNor[i] );
	}

	m_sDayNightBox.m_vIndirectColorVC_HIGH = D3DXVECTOR3(0.f,0.f,0.f);
	m_sDayNightBox.m_vIndirectColorVC_HIGH_HIGH = D3DXVECTOR3(0.f,0.f,0.f);
	m_sDayNightBox.m_fSpot1ColorVC = 0.f;			// PS_3_0 에서 spotlight 를 위해 쓰임.
	m_sDayNightBox.m_fSpot2ColorVC = 0.f;			// PS_3_0 에서 spotlight 를 위해 쓰임.
}

LightMapBoxColor::LightMapBoxColor( LightMapBoxColor* pSrc )
{
	m_sDayNightBox = pSrc->m_sDayNightBox;
}

LightMapBoxColor::~LightMapBoxColor()
{
}

void LightMapBoxColor::LightMapCalculationBoxColor( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMeshCollBase* pStaticMesh, DxPieceManager* pPieceManager, const WORDWORD& wwLight )
{
	for ( DWORD i=0; i<TEST_COUNT; ++i )
	{
		m_sDayNightBox.m_avCombine[i] = D3DXVECTOR3(0.f,0.f,0.f);
		m_sDayNightBox.m_avIndirectColor[i] = D3DXVECTOR3(0.f,0.f,0.f);
		m_sDayNightBox.m_afSpot1Color[i] = 0.f;
		m_sDayNightBox.m_afSpot2Color[i] = 0.f;
		m_sDayNightBox.m_avColorNoDynamicLight[i] = D3DXVECTOR3(0.f,0.f,0.f);
		m_sDayNightBox.m_avColorDirectionDirect[i] = D3DXVECTOR3(0.f,0.f,0.f);

		for ( DWORD x=0; x<SAMPLING_LINE*SAMPLING_LINE; ++x )
		{
			D3DXVECTOR3 vColorALL(0.f,0.f,0.f);
			D3DXVECTOR3 vColorIndirect(0.f,0.f,0.f);
			float fSpot1Color(0.f);
			float fSpot2Color(0.f);
			D3DXVECTOR3 vColorNoDynamicLight(0.f,0.f,0.f);
			D3DXVECTOR3 vColorDirectionDirect(0.f,0.f,0.f);

			NSLightMapTools::GetColorForObject
			( 
				pd3dDevice, 
				pStaticMesh, 
				pPieceManager, 
				m_sDayNightBox.m_avPos[i][x], 
				m_sDayNightBox.m_avNor[i], 
				FALSE, 
				vColorALL, 
				vColorIndirect,
				fSpot1Color, 
				fSpot2Color,
				vColorNoDynamicLight,
				vColorDirectionDirect,
				wwLight 
			);

			m_sDayNightBox.m_avCombine[i] += vColorALL;
			m_sDayNightBox.m_avIndirectColor[i] += vColorIndirect;
			m_sDayNightBox.m_afSpot1Color[i] += fSpot1Color;
			m_sDayNightBox.m_afSpot2Color[i] += fSpot2Color;
			m_sDayNightBox.m_avColorNoDynamicLight[i] += vColorNoDynamicLight;
			m_sDayNightBox.m_avColorDirectionDirect[i] += vColorDirectionDirect;
		}

		m_sDayNightBox.m_avCombine[i] /= (SAMPLING_LINE*SAMPLING_LINE);
		m_sDayNightBox.m_avIndirectColor[i] /= (SAMPLING_LINE*SAMPLING_LINE);
		m_sDayNightBox.m_afSpot1Color[i] /= (SAMPLING_LINE*SAMPLING_LINE);
		m_sDayNightBox.m_afSpot2Color[i] /= (SAMPLING_LINE*SAMPLING_LINE);
		m_sDayNightBox.m_avColorNoDynamicLight[i] /= (SAMPLING_LINE*SAMPLING_LINE);
		m_sDayNightBox.m_avColorDirectionDirect[i] /= (SAMPLING_LINE*SAMPLING_LINE);

		// AmbientCube 일 경우 노멀에 의해 또 계산 되기 때문에 생각보다 색이 어둡게 뽑힌다.
		// LightMap(3.f) 보다는 OverLight 제한 수치를 높게해준다. 
		// 얻어온 값에서 제한이 많이되서 넘어온다.
		// 할 필요성은 많이 없다.
		{
			float fOverLightLimit(5.f);
			if ( m_sDayNightBox.m_avCombine[i].x > fOverLightLimit )
				m_sDayNightBox.m_avCombine[i].x = fOverLightLimit;
			if ( m_sDayNightBox.m_avCombine[i].y > fOverLightLimit )
				m_sDayNightBox.m_avCombine[i].y = fOverLightLimit;
			if ( m_sDayNightBox.m_avCombine[i].z > fOverLightLimit )
				m_sDayNightBox.m_avCombine[i].z = fOverLightLimit;

			if ( m_sDayNightBox.m_avIndirectColor[i].x > fOverLightLimit )
				m_sDayNightBox.m_avIndirectColor[i].x = fOverLightLimit;
			if ( m_sDayNightBox.m_avIndirectColor[i].y > fOverLightLimit )
				m_sDayNightBox.m_avIndirectColor[i].y = fOverLightLimit;
			if ( m_sDayNightBox.m_avIndirectColor[i].z > fOverLightLimit )
				m_sDayNightBox.m_avIndirectColor[i].z = fOverLightLimit;

			if ( m_sDayNightBox.m_avColorNoDynamicLight[i].x > fOverLightLimit )
				m_sDayNightBox.m_avColorNoDynamicLight[i].x = fOverLightLimit;
			if ( m_sDayNightBox.m_avColorNoDynamicLight[i].y > fOverLightLimit )
				m_sDayNightBox.m_avColorNoDynamicLight[i].y = fOverLightLimit;
			if ( m_sDayNightBox.m_avColorNoDynamicLight[i].z > fOverLightLimit )
				m_sDayNightBox.m_avColorNoDynamicLight[i].z = fOverLightLimit;
		}

		m_sDayNightBox.m_avDirectColor[i] = m_sDayNightBox.m_avCombine[i];
	}

	SetVoxelColor_LightMap( m_sDayNightBox );
}

void LightMapBoxColor::BakeRadiosity( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan, LPDIRECT3DTEXTUREQ pTextureSystem, float fReflectance, float fFogBlend, DWORD dwFogLength )
{
	BOOL bPieceInstancing = NSPieceQuickSort::g_bPieceInstancing;
	NSPieceQuickSort::g_bPieceInstancing = FALSE;

	//// CV Thread 작업이 시작되겠음. CV 계산은 0 부터 시작. 렌더는 현재 안함.
	//{
	//	CLIPVOLUME sCV;
	//	D3DXMATRIX matView;
	//	D3DXMATRIX matProj;
	//	pLandMan->GetClipVolumeRAD( sCV, matView, matProj, m_sDayNightBox.m_avPos[0][0], m_sDayNightBox.m_avNor[0] );

	//	NSLandThread::Wait_FrameMoveRAD( pLandMan->GetArrayEntityRenderListSwap(), sCV );
	//}

	DWORD dwColor(0L);
	D3DXVECTOR3 vLookatPtOUT;
	for ( DWORD i=0; i<TEST_COUNT; ++i )
	{
		D3DXVECTOR3	avColorTemp;
		avColorTemp.x = 0.f;
		avColorTemp.y = 0.f;
		avColorTemp.z = 0.f;
		for ( DWORD x=0; x<SAMPLING_LINE*SAMPLING_LINE; ++x )
		{
			// CV 계산은 1 이 될경우 렌더는 0 이 동작된다.
			{
				int ni = i;
				int nx = x;
				++nx;
				if ( nx >= SAMPLING_LINE*SAMPLING_LINE )
				{
					++ni;
					nx = 0;
				}
				if ( ni < TEST_COUNT )
				{
					CLIPVOLUME sCV;
					D3DXMATRIX matView;
					D3DXMATRIX matProj;
					pLandMan->GetClipVolumeRAD( sCV, matView, matProj, vLookatPtOUT, m_sDayNightBox.m_avPos[ni][nx], m_sDayNightBox.m_avNor[ni] );

					NSLandThread::FrameMoveRAD( pLandMan->GetArrayEntityRenderListSwap(), sCV, m_sDayNightBox.m_avPos[ni][nx], vLookatPtOUT );
				}
				else
				{
					// 완료될 때까지 대기한다.
					while ( !NSLandThread::IsCompleteData() )
					{
					}

					// swap 을 한다.
					NSLandThread::Swap();
				}
			}

			dwColor = pLandMan->Render_Radiosity( pd3dDevice,
													m_sDayNightBox.m_avPos[i][x], 
													m_sDayNightBox.m_avNor[i], 
													pTextureSystem, 
													fReflectance, 
													fFogBlend, 
													dwFogLength, 
													TRUE );
			m_sDayNightBox.m_avIndirectColor[i].x = static_cast<float>((dwColor&0xff0000)>>16) / 255;
			m_sDayNightBox.m_avIndirectColor[i].y = static_cast<float>((dwColor&0xff00)>>8) / 255;
			m_sDayNightBox.m_avIndirectColor[i].z = static_cast<float>((dwColor&0xff)) / 255;

			avColorTemp += m_sDayNightBox.m_avIndirectColor[i];
		}

		m_sDayNightBox.m_avIndirectColor[i] = avColorTemp / (SAMPLING_LINE*SAMPLING_LINE);

		// Radiosity 작업시 1/3 으로 작업했기 때문에 값을 되돌려준다.
		m_sDayNightBox.m_avIndirectColor[i] *= 3.f;
	}

	AddRadiosityColorForVoxel();

	NSPieceQuickSort::g_bPieceInstancing = bPieceInstancing;
}

void LightMapBoxColor::AddRadiosityColorForVoxel()
{
	for ( DWORD i=0; i<TEST_COUNT; ++i )
	{
		m_sDayNightBox.m_avCombine[i] = m_sDayNightBox.m_avDirectColor[i] + m_sDayNightBox.m_avIndirectColor[i];
		m_sDayNightBox.m_avIndirectColor[i] = m_sDayNightBox.m_avColorNoDynamicLight[i] + m_sDayNightBox.m_avIndirectColor[i];
	}

	SetVoxelColor_LightMap( m_sDayNightBox );
}

void LightMapBoxColor::SetVoxelColor_LightMap( DAYNIGHT_BOX& sDataBox )
{
	struct COLOR
	{
		float m_fBright;
		D3DXVECTOR3 m_vColor;

		bool operator<(const COLOR& rhs)    const
		{
			return m_fBright < rhs.m_fBright;
		}

		bool operator>(const COLOR& rhs)    const
		{
			return m_fBright > rhs.m_fBright;
		}
	};

	sDataBox.m_vVexelColor = D3DXVECTOR3(0.f,0.f,0.f);
	sDataBox.m_vIndirectColorVC_HIGH = D3DXVECTOR3(0.f,0.f,0.f);
	sDataBox.m_vIndirectColorVC_HIGH_HIGH = D3DXVECTOR3(0.f,0.f,0.f);

	{
		std::vector<COLOR>	vecColor;
		for ( DWORD i=0; i<TEST_COUNT; ++i )
		{
			COLOR sColor;
			sColor.m_fBright = sDataBox.m_avCombine[i].x+sDataBox.m_avCombine[i].y+sDataBox.m_avCombine[i].z;
			sColor.m_vColor = sDataBox.m_avCombine[i];
			vecColor.push_back( sColor );
		}
		std::sort( vecColor.begin(), vecColor.end(), std::greater<COLOR>() );

		if ( vecColor.size() != TEST_COUNT )
			return;

		int nCount(0);
		//for ( int i=static_cast<int>(vecColor.size())-1; i>=0 && nCount<CALCULATE_COUNT; --i, ++nCount )
		for ( ; nCount<static_cast<int>(vecColor.size()) && nCount<CALCULATE_COUNT; ++nCount )
		{
			sDataBox.m_vVexelColor += vecColor[nCount].m_vColor;
		}
		if ( nCount == 0 )
			return;

		sDataBox.m_vVexelColor /= static_cast<float>( nCount );
	}

	// IndirectColor 계산 ( TnL_PS_3_0_NM )
	{
		std::vector<COLOR>	vecColor;
		for ( DWORD i=0; i<TEST_COUNT; ++i )
		{
			COLOR sColor;
			sColor.m_vColor = sDataBox.m_avIndirectColor[i] + sDataBox.m_avColorDirectionDirect[i];
			sColor.m_fBright = sColor.m_vColor.x+sColor.m_vColor.y+sColor.m_vColor.z;
			vecColor.push_back( sColor );
		}
		std::sort( vecColor.begin(), vecColor.end(), std::greater<COLOR>() );

		if ( vecColor.size() != TEST_COUNT )
			return;

		int nCount(0);
		//for ( int i=static_cast<int>(vecColor.size())-1; i>=0 && nCount<CALCULATE_COUNT; --i, ++nCount )
		for ( ; nCount<static_cast<int>(vecColor.size()) && nCount<CALCULATE_COUNT; ++nCount )
		{
			sDataBox.m_vIndirectColorVC_HIGH += vecColor[nCount].m_vColor;
		}
		if ( nCount == 0 )
			return;

		sDataBox.m_vIndirectColorVC_HIGH /= static_cast<float>( nCount );
	}

	// IndirectColor 계산 ( TnL_PS_3_0_CSM )
	{
		for ( DWORD i=0; i<TEST_COUNT; ++i )
		{
			sDataBox.m_vIndirectColorVC_HIGH_HIGH += sDataBox.m_avIndirectColor[i];
		}

		sDataBox.m_vIndirectColorVC_HIGH_HIGH /= TEST_COUNT;
	}

	// m_fSpot1ColorVC 계산
	{
		std::vector<float>	vecfColor;
		for ( DWORD i=0; i<TEST_COUNT; ++i )
		{
			vecfColor.push_back( sDataBox.m_afSpot1Color[i] );
		}
		std::sort( vecfColor.begin(), vecfColor.end(), std::greater<float>() );

		int nCount = 0;
		for ( ; nCount<static_cast<int>(vecfColor.size()) && nCount<CALCULATE_COUNT; ++nCount )
		{
			sDataBox.m_fSpot1ColorVC += vecfColor[nCount];
		}
		if ( nCount == 0 )
			return;

		sDataBox.m_fSpot1ColorVC /= nCount;
	}

	// m_fSpot2ColorVC 계산
	{
		std::vector<float>	vecfColor;
		for ( DWORD i=0; i<TEST_COUNT; ++i )
		{
			vecfColor.push_back( sDataBox.m_afSpot2Color[i] );
		}
		std::sort( vecfColor.begin(), vecfColor.end(), std::greater<float>() );

		int nCount = 0;
		for ( ; nCount<static_cast<int>(vecfColor.size()) && nCount<CALCULATE_COUNT; ++nCount )
		{
			sDataBox.m_fSpot2ColorVC += vecfColor[nCount];
		}
		if ( nCount == 0 )
			return;

		sDataBox.m_fSpot2ColorVC /= nCount;
	}
}

//-----------------------------------------------------------------------------------------------------------------------------------
//												LightMapBoxColorFullData
LightMapBoxColorFullData::LightMapBoxColorFullData()
{
	m_vVoxelColor = D3DXVECTOR4(1.f,1.f,1.f,1.f);
	m_vIndirectVC_HIGH = D3DXVECTOR4(1.f,1.f,1.f,1.f);
	m_vIndirectVC_HIGH_HIGH = D3DXVECTOR4(1.f,1.f,1.f,1.f);
	m_vVC_Spot1_Spot2 = D3DXVECTOR2(0.f,0.f);

	for ( DWORD i=0; i<LightMapBoxColor::TEST_COUNT; ++i )
	{
		m_avCombineColor[i] = D3DXVECTOR3(0.f,0.f,0.f);
		m_avIndirectColor[i] = D3DXVECTOR3(0.f,0.f,0.f);
		m_avDirectionDirectColor[i] = D3DXVECTOR3(0.f,0.f,0.f);
		m_afSpot1Color[i] = 0.f;
		m_afSpot2Color[i] = 0.f;
	}
}

LightMapBoxColorFullData::~LightMapBoxColorFullData()
{

}

// RAD 렌더시 호출된다.
void LightMapBoxColorFullData::FrameMoveRAD( D3DXVECTOR4& vVoxelColorUsed, D3DXVECTOR4 avAmbientCube[] )
{
	vVoxelColorUsed = m_vVoxelColor;

	for ( DWORD i=0; i<LightMapBoxColor::TEST_COUNT; ++i )
	{
		avAmbientCube[i] = m_avAmbientCube[i];
	}
}

// Piece 갯수는 많지만 1fps 에 1개씩만 호출되도록 되어있다.
void LightMapBoxColorFullData::FrameMoveStep( D3DXVECTOR4** ppAmbientCube, 
											 float** ppSpot1Color, 
											 float** ppSpot2Color,
											 D3DXVECTOR4** ppVoxelColor,
											 D3DXVECTOR2** ppSpot1_Spot2 )
{
	(*ppSpot1Color) = &m_afSpot1Color[0];
	(*ppSpot2Color) = &m_afSpot2Color[0];

	if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_CSM )
	{
		(*ppAmbientCube) = &m_avIndirectAC_HIGH_HIGH[0];
		(*ppVoxelColor) = &m_vIndirectVC_HIGH_HIGH;
		//(*ppVoxelColor) = &m_vVoxelColor;
	}
	else if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_NM )
	{
		(*ppAmbientCube) = &m_avIndirectAC_HIGH_HIGH[0];
		(*ppVoxelColor) = &m_vIndirectVC_HIGH_HIGH;
		//(*ppVoxelColor) = &m_vVoxelColor;
	}
	else
	{
		(*ppAmbientCube) = &m_avAmbientCube[0];
		(*ppVoxelColor) = &m_vVoxelColor;
	}

	(*ppSpot1_Spot2) = &m_vVC_Spot1_Spot2;
}

void LightMapBoxColorFullData::LightMapCalculationBoxColor( LightMapBoxColor* pLightMapBoxColor )
{
	m_vVoxelColor = D3DXVECTOR4( pLightMapBoxColor->GetColorVoxel(), 1.f );
	m_vIndirectVC_HIGH = D3DXVECTOR4( pLightMapBoxColor->GetIndirectVC_HIGH(), 1.f );
	m_vIndirectVC_HIGH_HIGH = D3DXVECTOR4( pLightMapBoxColor->GetIndirectVC_HIGH_HIGH(), 1.f );
	m_vVC_Spot1_Spot2 = D3DXVECTOR2( pLightMapBoxColor->GetSpot1ColorVC(), pLightMapBoxColor->GetSpot2ColorVC() );

	for ( DWORD i=0; i<LightMapBoxColor::TEST_COUNT; ++i )
	{
		const D3DXVECTOR3* pSrc = pLightMapBoxColor->GetCombineColor();
		m_avCombineColor[i] = pSrc[i];

		pSrc = pLightMapBoxColor->GetIndirectColor();
		m_avIndirectColor[i] = pSrc[i];

		pSrc = pLightMapBoxColor->GetDirectionDirectColor();
		m_avDirectionDirectColor[i] = pSrc[i];

		const float* fColor = pLightMapBoxColor->GetSpot1Color();
		m_afSpot1Color[i] = fColor[i];

		fColor = pLightMapBoxColor->GetSpot2Color();
		m_afSpot2Color[i] = fColor[i];
	}

	SetIndirectVCAC();
}

void LightMapBoxColorFullData::AddRadiosityColorForVoxel( LightMapBoxColor* pLightMapBoxColor )
{
	m_vVoxelColor = D3DXVECTOR4( pLightMapBoxColor->GetColorVoxel(), 1.f );
	m_vIndirectVC_HIGH = D3DXVECTOR4( pLightMapBoxColor->GetIndirectVC_HIGH(), 1.f );
	m_vIndirectVC_HIGH_HIGH = D3DXVECTOR4( pLightMapBoxColor->GetIndirectVC_HIGH_HIGH(), 1.f );
	m_vVC_Spot1_Spot2 = D3DXVECTOR2( pLightMapBoxColor->GetSpot1ColorVC(), pLightMapBoxColor->GetSpot2ColorVC() );

	for ( DWORD i=0; i<LightMapBoxColor::TEST_COUNT; ++i )
	{
		const D3DXVECTOR3* pSrc = pLightMapBoxColor->GetCombineColor();
		m_avCombineColor[i] = pSrc[i];
	}

	for ( DWORD i=0; i<LightMapBoxColor::TEST_COUNT; ++i )
	{
		const D3DXVECTOR3* pSrc = pLightMapBoxColor->GetIndirectColor();
		m_avIndirectColor[i] = pSrc[i];
	}

	SetIndirectVCAC();
}

void LightMapBoxColorFullData::SetIndirectVCAC()
{
	D3DXVECTOR3 vDirectionDirectColorMax(0.f,0.f,0.f);
	for ( DWORD i=0; i<LightMapBoxColor::TEST_COUNT; ++i )
	{
		m_avAmbientCube[i].x = m_avCombineColor[i].x;
		m_avAmbientCube[i].y = m_avCombineColor[i].y;
		m_avAmbientCube[i].z = m_avCombineColor[i].z;
		m_avAmbientCube[i].w = 1.f;

		m_avIndirectAC_HIGH[i].x = m_avIndirectColor[i].x + m_avDirectionDirectColor[i].x;
		m_avIndirectAC_HIGH[i].y = m_avIndirectColor[i].y + m_avDirectionDirectColor[i].y;
		m_avIndirectAC_HIGH[i].z = m_avIndirectColor[i].z + m_avDirectionDirectColor[i].z;
		m_avIndirectAC_HIGH[i].w = 1.f;

		m_avIndirectAC_HIGH_HIGH[i].x = m_avIndirectColor[i].x;
		m_avIndirectAC_HIGH_HIGH[i].y = m_avIndirectColor[i].y;
		m_avIndirectAC_HIGH_HIGH[i].z = m_avIndirectColor[i].z;
		m_avIndirectAC_HIGH_HIGH[i].w = 1.f;

		if ( vDirectionDirectColorMax.x < m_avDirectionDirectColor[i].x )
		{
			vDirectionDirectColorMax.x = m_avDirectionDirectColor[i].x;
		}
		if ( vDirectionDirectColorMax.y < m_avDirectionDirectColor[i].y )
		{
			vDirectionDirectColorMax.y = m_avDirectionDirectColor[i].y;
		}
		if ( vDirectionDirectColorMax.z < m_avDirectionDirectColor[i].z )
		{
			vDirectionDirectColorMax.z = m_avDirectionDirectColor[i].z;
		}
	}

	// 이렇게 하는 이유는.
	// 최고옵이라 하여도 멀리 있을 때는 자체그림자가 아닌 m_avIndirectAC_HIGH_HIGH[6] 수치로만 계산하기 때문에.
	// LBuffer 계산 시, 어두운 곳에 있는 물체가 너무 밝게 보일 수 있다. ( 중옵도 마찬가지 이유. )
	// 그래서 이값을 Shader 에서 계산하도록 한다.
	if ( vDirectionDirectColorMax.x > vDirectionDirectColorMax.y && vDirectionDirectColorMax.x > vDirectionDirectColorMax.z )
	{
		m_avIndirectAC_HIGH_HIGH[0].w = vDirectionDirectColorMax.x * 1.0f;
		//m_vIndirectVC_HIGH_HIGH.w = vDirectionDirectColorMax.x * 1.0f;
	}
	else if ( vDirectionDirectColorMax.y > vDirectionDirectColorMax.z )
	{
		m_avIndirectAC_HIGH_HIGH[0].w = vDirectionDirectColorMax.y * 1.0f;
		//m_vIndirectVC_HIGH_HIGH.w = vDirectionDirectColorMax.y * 1.0f;
	}
	else
	{
		m_avIndirectAC_HIGH_HIGH[0].w = vDirectionDirectColorMax.z * 1.0f;
		//m_vIndirectVC_HIGH_HIGH.w = vDirectionDirectColorMax.z * 1.0f;
	}

	//if ( m_avIndirectAC_HIGH_HIGH[0].w > 1.f )
	//	m_avIndirectAC_HIGH_HIGH[0].w = 1.f;

	//if ( m_vIndirectVC_HIGH_HIGH.w > 1.f )
	//	m_vIndirectVC_HIGH_HIGH.w = 1.f;

	// Voxel 은 기본색을 조금 높여준다. ( 옵션 고,최고 에서만. )
	// 또한 fx 에서는 
	// opaque 나 hardalpha 라면 디퍼드 합산시 shadow(m_vVoxelColor.w)에 영향 받는다.
	// softalpha라면			"		vAddColor += g_vLightDiffuse.xyz * g_vVoxelColor.w * 0.5f; \r\n"		// 알파라면 그냥 색을 넣는다.
	// 작업이 된다.
	//m_vIndirectVC_HIGH_HIGH.x += (m_vVoxelColor.x-m_vIndirectVC_HIGH_HIGH.x) * 0.666f;
	//m_vIndirectVC_HIGH_HIGH.y += (m_vVoxelColor.y-m_vIndirectVC_HIGH_HIGH.y) * 0.666f;
	//m_vIndirectVC_HIGH_HIGH.z += (m_vVoxelColor.z-m_vIndirectVC_HIGH_HIGH.z) * 0.666f;

	//m_vIndirectVC_HIGH_HIGH.x = m_vVoxelColor.x;
	//m_vIndirectVC_HIGH_HIGH.y = m_vVoxelColor.y;
	//m_vIndirectVC_HIGH_HIGH.z = m_vVoxelColor.z;
}

void LightMapBoxColorFullData::Save( sc::SerialFile& SFile )
{
	SFile << VERSION;
	SFile.BeginBlock();
	{
		SFile.WriteBuffer( &m_vVoxelColor, sizeof(D3DXVECTOR3) );
		SFile.WriteBuffer( &m_vIndirectVC_HIGH, sizeof(D3DXVECTOR3) );
		SFile.WriteBuffer( &m_vIndirectVC_HIGH_HIGH, sizeof(D3DXVECTOR3) );
		SFile.WriteBuffer( &m_vVC_Spot1_Spot2, sizeof(D3DXVECTOR2) );

		SFile.WriteBuffer( &m_avCombineColor[0], sizeof(D3DXVECTOR3)*LightMapBoxColor::TEST_COUNT );
		SFile.WriteBuffer( &m_avIndirectColor[0], sizeof(D3DXVECTOR3)*LightMapBoxColor::TEST_COUNT );
		SFile.WriteBuffer( &m_avDirectionDirectColor[0], sizeof(D3DXVECTOR3)*LightMapBoxColor::TEST_COUNT );
		
		SFile.WriteBuffer( &m_afSpot1Color[0], sizeof(float)*LightMapBoxColor::TEST_COUNT );
		SFile.WriteBuffer( &m_afSpot2Color[0], sizeof(float)*LightMapBoxColor::TEST_COUNT );
	}
	SFile.EndBlock();
}

void LightMapBoxColorFullData::Load( sc::SerialFile& SFile )
{
	DWORD dwVer, dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	if ( dwVer == 0x104 )
	{
		SFile.ReadBuffer( &m_vVoxelColor, sizeof(D3DXVECTOR3) );
		SFile.ReadBuffer( &m_vIndirectVC_HIGH, sizeof(D3DXVECTOR3) );
		SFile.ReadBuffer( &m_vIndirectVC_HIGH_HIGH, sizeof(D3DXVECTOR3) );
		SFile.ReadBuffer( &m_vVC_Spot1_Spot2, sizeof(D3DXVECTOR2) );

		SFile.ReadBuffer( &m_avCombineColor[0], sizeof(D3DXVECTOR3)*LightMapBoxColor::TEST_COUNT );
		SFile.ReadBuffer( &m_avIndirectColor[0], sizeof(D3DXVECTOR3)*LightMapBoxColor::TEST_COUNT );
		SFile.ReadBuffer( &m_avDirectionDirectColor[0], sizeof(D3DXVECTOR3)*LightMapBoxColor::TEST_COUNT );

		SFile.ReadBuffer( &m_afSpot1Color[0], sizeof(float)*LightMapBoxColor::TEST_COUNT );
		SFile.ReadBuffer( &m_afSpot2Color[0], sizeof(float)*LightMapBoxColor::TEST_COUNT );

		SetIndirectVCAC();
	}
	else if ( dwVer == 0x103 )
	{
		SFile.ReadBuffer( &m_vVoxelColor, sizeof(D3DXVECTOR3) );
		SFile.ReadBuffer( &m_vIndirectVC_HIGH_HIGH, sizeof(D3DXVECTOR3) );
		SFile.ReadBuffer( &m_vVC_Spot1_Spot2, sizeof(D3DXVECTOR2) );

		SFile.ReadBuffer( &m_avCombineColor[0], sizeof(D3DXVECTOR3)*LightMapBoxColor::TEST_COUNT );
		SFile.ReadBuffer( &m_avIndirectColor[0], sizeof(D3DXVECTOR3)*LightMapBoxColor::TEST_COUNT );

		SFile.ReadBuffer( &m_afSpot1Color[0], sizeof(float)*LightMapBoxColor::TEST_COUNT );
		SFile.ReadBuffer( &m_afSpot2Color[0], sizeof(float)*LightMapBoxColor::TEST_COUNT );

		for ( DWORD i=0; i<LightMapBoxColor::TEST_COUNT; ++i )
		{
			m_avAmbientCube[i].x = m_avCombineColor[i].x;
			m_avAmbientCube[i].y = m_avCombineColor[i].y;
			m_avAmbientCube[i].z = m_avCombineColor[i].z;
			m_avAmbientCube[i].w = 1.f;

			m_avIndirectAC_HIGH_HIGH[i].x = m_avIndirectColor[i].x;
			m_avIndirectAC_HIGH_HIGH[i].y = m_avIndirectColor[i].y;
			m_avIndirectAC_HIGH_HIGH[i].z = m_avIndirectColor[i].z;
			m_avIndirectAC_HIGH_HIGH[i].w = 1.f;
		}
	}
	else if ( dwVer == 0x102 )
	{
		D3DXVECTOR3 vTemp;
		SFile >> vTemp;
		m_vVoxelColor = D3DXVECTOR4 ( vTemp, 1.f );

		SFile.ReadBuffer( &m_avCombineColor[0], sizeof(D3DXVECTOR3)*LightMapBoxColor::TEST_COUNT );
		SFile.ReadBuffer( &m_avIndirectColor[0], sizeof(D3DXVECTOR3)*LightMapBoxColor::TEST_COUNT );

		SFile.ReadBuffer( &m_afSpot1Color[0], sizeof(float)*LightMapBoxColor::TEST_COUNT );
		SFile.ReadBuffer( &m_afSpot2Color[0], sizeof(float)*LightMapBoxColor::TEST_COUNT );

		for ( DWORD i=0; i<LightMapBoxColor::TEST_COUNT; ++i )
		{
			m_avAmbientCube[i].x = m_avCombineColor[i].x;
			m_avAmbientCube[i].y = m_avCombineColor[i].y;
			m_avAmbientCube[i].z = m_avCombineColor[i].z;
			m_avAmbientCube[i].w = 1.f;

			m_avIndirectAC_HIGH_HIGH[i].x = m_avIndirectColor[i].x;
			m_avIndirectAC_HIGH_HIGH[i].y = m_avIndirectColor[i].y;
			m_avIndirectAC_HIGH_HIGH[i].z = m_avIndirectColor[i].z;
			m_avIndirectAC_HIGH_HIGH[i].w = 1.f;
		}
	}
	else if ( dwVer == 0x101 )
	{
		D3DXVECTOR3 vTemp;
		SFile >> vTemp;
		m_vVoxelColor = D3DXVECTOR4 ( vTemp, 1.f );

		SFile.ReadBuffer( &m_avCombineColor[0], sizeof(D3DXVECTOR3)*LightMapBoxColor::TEST_COUNT );
		SFile.ReadBuffer( &m_avIndirectColor[0], sizeof(D3DXVECTOR3)*LightMapBoxColor::TEST_COUNT );

		for ( DWORD i=0; i<LightMapBoxColor::TEST_COUNT; ++i )
		{
			m_avAmbientCube[i].x = m_avCombineColor[i].x + m_avIndirectColor[i].x;
			m_avAmbientCube[i].y = m_avCombineColor[i].y + m_avIndirectColor[i].y;
			m_avAmbientCube[i].z = m_avCombineColor[i].z + m_avIndirectColor[i].z;
			m_avAmbientCube[i].w = 1.f;

			m_avIndirectAC_HIGH_HIGH[i].x = m_avIndirectColor[i].x;
			m_avIndirectAC_HIGH_HIGH[i].y = m_avIndirectColor[i].y;
			m_avIndirectAC_HIGH_HIGH[i].z = m_avIndirectColor[i].z;
			m_avIndirectAC_HIGH_HIGH[i].w = 1.f;
		}
	}
	else if ( dwVer == 0x100 )
	{
		LoadPrev( SFile );
	}
	else
	{
		SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
	}
}

void LightMapBoxColorFullData::LoadPrev( sc::SerialFile& SFile )
{
	D3DXVECTOR3 vTemp;
	SFile >> vTemp;
	m_vVoxelColor = D3DXVECTOR4 ( vTemp, 1.f );

	D3DXVECTOR3 vVoxelColorTemp;
	SFile >> vVoxelColorTemp;

	SFile.ReadBuffer( &m_avCombineColor[0], sizeof(D3DXVECTOR3)*LightMapBoxColor::TEST_COUNT );
	SFile.ReadBuffer( &m_avIndirectColor[0], sizeof(D3DXVECTOR3)*LightMapBoxColor::TEST_COUNT );

	D3DXVECTOR3	avDirectColor[LightMapBoxColor::TEST_COUNT];
	D3DXVECTOR3	avIndirectColor[LightMapBoxColor::TEST_COUNT];
	SFile.ReadBuffer( &avDirectColor[0], sizeof(D3DXVECTOR3)*LightMapBoxColor::TEST_COUNT );
	SFile.ReadBuffer( &avIndirectColor[0], sizeof(D3DXVECTOR3)*LightMapBoxColor::TEST_COUNT );

	for ( DWORD i=0; i<LightMapBoxColor::TEST_COUNT; ++i )
	{
		m_avAmbientCube[i].x = m_avCombineColor[i].x + m_avIndirectColor[i].x;
		m_avAmbientCube[i].y = m_avCombineColor[i].y + m_avIndirectColor[i].y;
		m_avAmbientCube[i].z = m_avCombineColor[i].z + m_avIndirectColor[i].z;
		m_avAmbientCube[i].w = 1.f;

		m_avIndirectAC_HIGH_HIGH[i].x = m_avIndirectColor[i].x;
		m_avIndirectAC_HIGH_HIGH[i].y = m_avIndirectColor[i].y;
		m_avIndirectAC_HIGH_HIGH[i].z = m_avIndirectColor[i].z;
		m_avIndirectAC_HIGH_HIGH[i].w = 1.f;
	}
}

void LightMapBoxColorFullData::LoadPrev2( sc::SerialFile& SFile )
{
	D3DXVECTOR3 vTemp;
	SFile >> vTemp;
	m_vVoxelColor = D3DXVECTOR4 ( vTemp, 1.f );

	D3DXVECTOR3 vVoxelColorTemp;
	SFile >> vVoxelColorTemp;
}

//////////////////////////////////////////////////////////////////////////
const D3DXVECTOR4* LightMapBoxColorFullData::GetAmbientCube() const
{
	return m_avAmbientCube;
}
#include "pch.h"

//#include <algorithm>

//#include "../../EngineBaseLib/Common/StlFunctions.h"

#include "../SkyBox/DxSkyBoxMan.h"
#include "../Light/DxLightMan.h"
#include "../DxWeatherMan.h"
#include "../EditMeshs.h"

#include "DxFogLocalMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------- DxFogLocal
DxFogLocal::DxFogLocal()
	: m_fRange1(400.f)
	, m_fRange2(250.f)
	, m_fFogStart(0.f)
	, m_fFogEnd(1000.f)
	, m_cFogColor(D3DCOLOR_XRGB(0,0,0))
	, m_bActiveLight(FALSE)
	, m_cDiffuse(D3DCOLOR_XRGB(0,0,0))
	, m_cAmbient(D3DCOLOR_XRGB(0,0,0))
	, m_dwWeatherFlags(0L)
{
	D3DXMatrixIdentity( &m_matWorld );

	m_pSkyBoxMan = new DxSkyBoxMan;
}

DxFogLocal::~DxFogLocal()
{
	SAFE_DELETE( m_pSkyBoxMan );
}

BOOL DxFogLocal::IsWeatherRain()
{
	return m_dwWeatherFlags & ISRAIN;
}

BOOL DxFogLocal::IsWeatherLeaves()
{
	return m_dwWeatherFlags & ISLEAVES;
}

void DxFogLocal::FrameMove( float fElapsedTime )
{
	m_pSkyBoxMan->FrameMove( fElapsedTime );
}

void DxFogLocal::Render( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX* pmatReflect )
{
	m_pSkyBoxMan->Render( pd3dDevice, pmatReflect );
}

void DxFogLocal::Render_LightShafts( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSkyBoxMan->Render_LightShafts_Local( pd3dDevice );
}

void DxFogLocal::RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwR = (DWORD) m_cFogColor.r;
	DWORD dwG = (DWORD) m_cFogColor.g;
	DWORD dwB = (DWORD) m_cFogColor.b;
	dwR += 127;
	dwG += 127;
	dwB += 127;
	if( dwR > 255 )		dwR -= 255;
	if( dwG > 255 )		dwG -= 255;
	if( dwB > 255 )		dwB -= 255;

	DWORD dwColor;
	dwColor = 0xff000000 + (dwR<<24) + (dwG <<8) + dwB;

	D3DXVECTOR3 vPos;
	vPos.x = m_matWorld._41;
	vPos.y = m_matWorld._42;
	vPos.z = m_matWorld._43;
	EDITMESHS::RENDERSPHERE( pd3dDevice, vPos, m_fRange1, NULL, dwColor );
	EDITMESHS::RENDERSPHERE( pd3dDevice, vPos, m_fRange2, NULL, dwColor );
}

void DxFogLocal::Save( sc::SerialFile &SFile )
{
	SFile <<(DWORD)VERSION;
	SFile.BeginBlock();
	{
		SFile.WriteBuffer( &m_matWorld, sizeof(D3DXMATRIX) );
		SFile << m_fRange1;
		SFile << m_fRange2;
		SFile << m_fFogStart;
		SFile << m_fFogEnd;
		SFile << m_cFogColor.r;
		SFile << m_cFogColor.g;
		SFile << m_cFogColor.b;

		SFile << m_bActiveLight;
		SFile << m_cDiffuse.r;
		SFile << m_cDiffuse.g;
		SFile << m_cDiffuse.b;
		SFile << m_cAmbient.r;
		SFile << m_cAmbient.g;
		SFile << m_cAmbient.b;

		SFile << m_dwWeatherFlags;

		m_pSkyBoxMan->Save( SFile );
	}
	SFile.EndBlock();
}

void DxFogLocal::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile )
{
	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	if( dwVer==VERSION )
	{
		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );
		SFile >> m_fRange1;
		SFile >> m_fRange2;
		SFile >> m_fFogStart;
		SFile >> m_fFogEnd;
		SFile >> m_cFogColor.r;
		SFile >> m_cFogColor.g;
		SFile >> m_cFogColor.b;

		SFile >> m_bActiveLight;
		SFile >> m_cDiffuse.r;
		SFile >> m_cDiffuse.g;
		SFile >> m_cDiffuse.b;
		SFile >> m_cAmbient.r;
		SFile >> m_cAmbient.g;
		SFile >> m_cAmbient.b;

		SFile >> m_dwWeatherFlags;

		SAFE_DELETE( m_pSkyBoxMan );
		m_pSkyBoxMan = new DxSkyBoxMan;
		m_pSkyBoxMan->Load( pd3dDevice, SFile, TRUE );
		m_pSkyBoxMan->SetAlpha( 0.f );
	}
	else if( dwVer==0x0104 )
	{
		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );
		SFile >> m_fRange1;
		SFile >> m_fRange2;
		SFile >> m_fFogStart;
		SFile >> m_fFogEnd;
		SFile >> m_cFogColor.r;
		SFile >> m_cFogColor.g;
		SFile >> m_cFogColor.b;

		SFile >> m_bActiveLight;
		SFile >> m_cDiffuse.r;
		SFile >> m_cDiffuse.g;
		SFile >> m_cDiffuse.b;
		SFile >> m_cAmbient.r;
		SFile >> m_cAmbient.g;
		SFile >> m_cAmbient.b;

		SAFE_DELETE( m_pSkyBoxMan );
		m_pSkyBoxMan = new DxSkyBoxMan;
		m_pSkyBoxMan->Load( pd3dDevice, SFile, TRUE );
		m_pSkyBoxMan->SetAlpha( 0.f );
	}
	else if( dwVer==0x0103 )
	{
		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );
		SFile >> m_fRange1;
		SFile >> m_fRange2;
		SFile >> m_fFogStart;
		SFile >> m_fFogEnd;
		SFile >> m_cFogColor.r;
		SFile >> m_cFogColor.g;
		SFile >> m_cFogColor.b;

		SAFE_DELETE( m_pSkyBoxMan );
		m_pSkyBoxMan = new DxSkyBoxMan;
		m_pSkyBoxMan->Load( pd3dDevice, SFile, TRUE );
		m_pSkyBoxMan->SetAlpha( 0.f );
	}
	else if( dwVer==0x0101 || dwVer==0x0102 )
	{
		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );
		SFile >> m_fRange1;
		SFile >> m_fRange2;
		SFile >> m_fFogStart;
		SFile >> m_fFogEnd;
		SFile >> m_cFogColor.r;
		SFile >> m_cFogColor.g;
		SFile >> m_cFogColor.b;
	}
	else if( dwVer==0x0100 )
	{
		D3DXVECTOR3 vPos;
		SFile >> vPos;
		SFile >> m_fRange1;
		SFile >> m_fRange2;
		SFile >> m_fFogStart;
		SFile >> m_fFogEnd;
		SFile >> m_cFogColor.r;
		SFile >> m_cFogColor.g;
		SFile >> m_cFogColor.b;

		m_matWorld._41 = vPos.x;
		m_matWorld._42 = vPos.y;
		m_matWorld._43 = vPos.z;
	}
}

void DxFogLocal::Clone( LPDIRECT3DDEVICEQ pd3dDevice, const DxFogLocal* pSrc )
{
	m_matWorld			= pSrc->m_matWorld;
	m_fRange1			= pSrc->m_fRange1;
	m_fRange2			= pSrc->m_fRange2;
	m_fFogStart			= pSrc->m_fFogStart;
	m_fFogEnd			= pSrc->m_fFogEnd;
	m_cFogColor			= pSrc->m_cFogColor;

	m_bActiveLight		= pSrc->m_bActiveLight;
	m_cDiffuse			= pSrc->m_cDiffuse;
	m_cAmbient			= pSrc->m_cAmbient;
	m_dwWeatherFlags	= pSrc->m_dwWeatherFlags;

	SAFE_DELETE( m_pSkyBoxMan );
	m_pSkyBoxMan = new DxSkyBoxMan;
	m_pSkyBoxMan->Clone( pd3dDevice, pSrc->m_pSkyBoxMan );
}

//DxFogLocal::DxFogLocal( const DxFogLocal& rhs )
//{
//	m_matWorld	= rhs.m_matWorld;
//	m_fRange1	= rhs.m_fRange1;
//	m_fRange2	= rhs.m_fRange2;
//	m_fFogStart	= rhs.m_fFogStart;
//	m_fFogEnd	= rhs.m_fFogEnd;
//	m_cFogColor	= rhs.m_cFogColor;
//
//	SAFE_DELETE( m_pSkyBoxMan );
//	m_pSkyBoxMan = new DxSkyBoxMan;
//}
//
//DxFogLocal& DxFogLocal::operator= ( const DxFogLocal& sSrc )
//{
//	m_matWorld	= sSrc.m_matWorld;
//	m_fRange1	= sSrc.m_fRange1;
//	m_fRange2	= sSrc.m_fRange2;
//	m_fFogStart	= sSrc.m_fFogStart;
//	m_fFogEnd	= sSrc.m_fFogEnd;
//	m_cFogColor	= sSrc.m_cFogColor;
//
//	SAFE_DELETE( m_pSkyBoxMan );
//	m_pSkyBoxMan = new DxSkyBoxMan;
//
//	return *this;
//}

// ----------------------------------------------------------------- DxFogLocalMan
DxFogLocalMan::DxFogLocalMan()
	: m_fRainRate(0.f)
{
}

DxFogLocalMan::~DxFogLocalMan()
{
	CleanUp();
}

void DxFogLocalMan::CleanUp()
{
	std::for_each( m_vecFogLocal.begin(), m_vecFogLocal.end(), std_afunc::DeleteObject() );
	m_vecFogLocal.clear();
}

void DxFogLocalMan::FrameMove( float fElapsedTime,
							  const D3DXVECTOR3& vPos, 
							  float fFogStart, 
							  float fFogEnd, 
							  const D3DXCOLOR& cFogColor, 
							  float& fFogStartOUT, 
							  float& fFogEndOUT, 
							  D3DXCOLOR& cFogColorOUT,
							  BOOL ChangeDirectionLight )	// ChangeDirectionLight = Light셋팅이나 Fog의 Local 셋팅 때만 TRUE이다.
{
	//struct COMPUTE_FOG
	//{
	//	float fRate;
	//	DWORD dwIndex;
	//};
	//typedef std::vector<COMPUTE_FOG>	VEC_COMPUTE_FOG;
	//typedef VEC_COMPUTE_FOG				VEC_COMPUTE_FOG_ITER;
	//VEC_COMPUTE_FOG	m_vecComputeFog;

	//float fRate(0.f);
	//float fLength(0.f);
	//D3DXVECTOR3 vDir(0.f,0.f,0.f);
	//for( DWORD i=0; i<m_vecFogLocal.size(); ++i )
	//{
	//	const DxFogLocal* pFogLocal = m_vecFogLocal[i];
	//	D3DXVECTOR3 vPosSRC;
	//	vPosSRC.x = pFogLocal->m_matWorld._41;
	//	vPosSRC.y = pFogLocal->m_matWorld._42;
	//	vPosSRC.z = pFogLocal->m_matWorld._43;
	//	vDir = vPosSRC - vPos;
	//	fLength = D3DXVec3Length( &vDir );
	//	if( fLength < pFogLocal->m_fRange1 )		// Fog 범위안에 들어있다.
	//	{
	//		// Note : 비중을 구한다.
	//		if( fLength < pFogLocal->m_fRange2 )
	//		{
	//			fRate = 1.f;
	//		}
	//		else
	//		{
	//			fRate = (fLength-pFogLocal->m_fRange2) / (pFogLocal->m_fRange1-pFogLocal->m_fRange2);
	//			fRate = 1.f - fRate;
	//		}

	//		if( fRate > 1.f )
	//		{
	//			fRate = 1.f;
	//		}
	//		else if( fRate < 0.f )
	//		{
	//			fRate = 0.f;
	//		}

	//		// Note : 작업된 수치를 대입한다.
	//		COMPUTE_FOG sData;
	//		sData.fRate		= fRate;
	//		sData.dwIndex	= i;
	//		m_vecComputeFog.push_back( sData );

	//		pFogLocal->m_pSkyBoxMan->SetAlpha( fRate );
	//	}
	//	else
	//	{
	//		pFogLocal->m_pSkyBoxMan->SetAlpha( 0.f );
	//	}
	//}

	//// 비렌더링을 위한 영향 받는 비율
	//m_fRainRate = 0.f;

	//// 낙엽을 위한 영향 받는 비율
	//float fLeavesRate = 0.f;

	//// Note : 직접 Fog값을 구한다.
	//fFogStartOUT = fFogStart;
	//fFogEndOUT = fFogEnd;
	//cFogColorOUT = cFogColor;
	//float fWeight(0.f);
	//for( DWORD i=0; i<m_vecComputeFog.size(); ++i )
	//{
	//	//fWeight = 1.f/(2+i);
	//	//fWeight *= m_vecComputeFog[i].fRate;
	//	fWeight = m_vecComputeFog[i].fRate;
	//	fFogStartOUT = (m_vecFogLocal[m_vecComputeFog[i].dwIndex]->m_fFogStart*fWeight) + (fFogStartOUT*(1.f-fWeight));
	//	fFogEndOUT = (m_vecFogLocal[m_vecComputeFog[i].dwIndex]->m_fFogEnd*fWeight) + (fFogEndOUT*(1.f-fWeight));
	//	cFogColorOUT.r = (m_vecFogLocal[m_vecComputeFog[i].dwIndex]->m_cFogColor.r*fWeight) + (cFogColorOUT.r*(1.f-fWeight));
	//	cFogColorOUT.g = (m_vecFogLocal[m_vecComputeFog[i].dwIndex]->m_cFogColor.g*fWeight) + (cFogColorOUT.g*(1.f-fWeight));
	//	cFogColorOUT.b = (m_vecFogLocal[m_vecComputeFog[i].dwIndex]->m_cFogColor.b*fWeight) + (cFogColorOUT.b*(1.f-fWeight));

	//	// 비 - Rain
	//	if( m_vecFogLocal[m_vecComputeFog[i].dwIndex]->IsWeatherRain() )
	//	{
	//		m_fRainRate += fWeight;
	//		if( m_fRainRate > 1.f )
	//			m_fRainRate = 1.f;
	//	}

	//	// 낙엽 - Leaves
	//	if( m_vecFogLocal[m_vecComputeFog[i].dwIndex]->IsWeatherLeaves() )
	//	{
	//		fLeavesRate += fWeight;
	//		if( fLeavesRate > 1.f )
	//			fLeavesRate = 1.f;
	//	}
	//}

 //   // Weather에 어느정도 비율의 비가 내려야 하는지 셋팅해 준다.
 //   DxWeatherMan::GetInstance()->SetRainRate_POSITION( m_fRainRate );

 //   // Weather에 어느정도 비율의 낙엽이 내려야 하는지 셋팅해 준다.
 //   DxWeatherMan::GetInstance()->SetLeavesRate_POSITION( fLeavesRate );


	//// ChangeDirectionLight = Light셋팅이나 Fog의 Local 셋팅 때만 TRUE이다.
	//if( ChangeDirectionLight )
	//{
	//	// 기본 Light를 가져온다.
	//	D3DXCOLOR cLightDiffuse;
	//	D3DXCOLOR cLightAmbient;
	//	D3DLIGHTQ sLight = DxLightMan::GetInstance()->GetDirectLight()->m_Light;
	//	cLightDiffuse.r = sLight.Diffuse.r;
	//	cLightDiffuse.g = sLight.Diffuse.g;
	//	cLightDiffuse.b = sLight.Diffuse.b;
	//	cLightAmbient.r = sLight.Ambient.r;
	//	cLightAmbient.g = sLight.Ambient.g;
	//	cLightAmbient.b = sLight.Ambient.b;

	//	// Light를 계산한다.
	//	CalculateLight( vPos, cLightDiffuse, cLightAmbient );

	//	// Light 계산된 값을 넣는다.
	//	sLight.Diffuse.r = cLightDiffuse.r;
	//	sLight.Diffuse.g = cLightDiffuse.g;
	//	sLight.Diffuse.b = cLightDiffuse.b;
	//	sLight.Ambient.r = cLightAmbient.r;
	//	sLight.Ambient.g = cLightAmbient.g;
	//	sLight.Ambient.b = cLightAmbient.b;
	//	DxLightMan::GetInstance()->SetDirectionLight( sLight );
	//}
	//else
	//{
	//	D3DLIGHTQ sLight = DxLightMan::GetInstance()->GetDirectLight()->m_Light;
	//	DxLightMan::GetInstance()->SetDirectionLight( sLight );
	//}

	//// 
	//for( DWORD i=0; i<m_vecFogLocal.size(); ++i )
	//{
	//	m_vecFogLocal[i]->FrameMove( fElapsedTime );
	//}
}

void DxFogLocalMan::CalculateLight( const D3DXVECTOR3& vPos, D3DXVECTOR3& vDiffuseINOUT, D3DXVECTOR3& vAmbientINOUT )
{
	D3DXCOLOR cDiffuse;
	D3DXCOLOR cAmbient;
	cDiffuse.r = vDiffuseINOUT.x;
	cDiffuse.g = vDiffuseINOUT.y;
	cDiffuse.b = vDiffuseINOUT.z;
	cAmbient.r = vAmbientINOUT.x;
	cAmbient.g = vAmbientINOUT.y;
	cAmbient.b = vAmbientINOUT.z;

	CalculateLight( vPos, cDiffuse, cAmbient );

	vDiffuseINOUT.x = cDiffuse.r;
	vDiffuseINOUT.y = cDiffuse.g;
	vDiffuseINOUT.z = cDiffuse.b;
	vAmbientINOUT.x = cAmbient.r;
	vAmbientINOUT.y = cAmbient.g;
	vAmbientINOUT.z = cAmbient.b;
}

void DxFogLocalMan::CalculateLight( const D3DXVECTOR3& vPos, D3DXCOLOR& cDiffuseINOUT, D3DXCOLOR& cAmbientINOUT )
{
	struct COMPUTE_FOG
	{
		float fRate;
		DWORD dwIndex;
	};
	typedef std::vector<COMPUTE_FOG>	VEC_COMPUTE_FOG;
	typedef VEC_COMPUTE_FOG				VEC_COMPUTE_FOG_ITER;
	VEC_COMPUTE_FOG	m_vecComputeFog;

	float fRate(0.f);
	float fLength(0.f);
	D3DXVECTOR3 vDir(0.f,0.f,0.f);
	for( DWORD i=0; i<m_vecFogLocal.size(); ++i )
	{
		const DxFogLocal* pFogLocal = m_vecFogLocal[i];
		D3DXVECTOR3 vPosSRC;
		vPosSRC.x = pFogLocal->m_matWorld._41;
		vPosSRC.y = pFogLocal->m_matWorld._42;
		vPosSRC.z = pFogLocal->m_matWorld._43;
		vDir = vPosSRC - vPos;
		fLength = D3DXVec3Length( &vDir );
		if( fLength < pFogLocal->m_fRange1 )		// Fog 범위안에 들어있다.
		{
			// Note : 비중을 구한다.
			if( fLength < pFogLocal->m_fRange2 )
			{
				fRate = 1.f;
			}
			else
			{
				fRate = (fLength-pFogLocal->m_fRange2) / (pFogLocal->m_fRange1-pFogLocal->m_fRange2);
				fRate = 1.f - fRate;
			}

			if( fRate > 1.f )
			{
				fRate = 1.f;
			}
			else if( fRate < 0.f )
			{
				fRate = 0.f;
			}

			// Note : 작업된 수치를 대입한다.
			COMPUTE_FOG sData;
			sData.fRate		= fRate;
			sData.dwIndex	= i;
			m_vecComputeFog.push_back( sData );
		}
	}

	// Note : 직접 Fog값을 구한다.
	float fWeight(0.f);
	for( DWORD i=0; i<m_vecComputeFog.size(); ++i )
	{
		fWeight = m_vecComputeFog[i].fRate;

		if( m_vecFogLocal[m_vecComputeFog[i].dwIndex]->m_bActiveLight )
		{
			cDiffuseINOUT.r = (m_vecFogLocal[m_vecComputeFog[i].dwIndex]->m_cDiffuse.r*fWeight) + (cDiffuseINOUT.r*(1.f-fWeight));
			cDiffuseINOUT.g = (m_vecFogLocal[m_vecComputeFog[i].dwIndex]->m_cDiffuse.g*fWeight) + (cDiffuseINOUT.g*(1.f-fWeight));
			cDiffuseINOUT.b = (m_vecFogLocal[m_vecComputeFog[i].dwIndex]->m_cDiffuse.b*fWeight) + (cDiffuseINOUT.b*(1.f-fWeight));
			cAmbientINOUT.r = (m_vecFogLocal[m_vecComputeFog[i].dwIndex]->m_cAmbient.r*fWeight) + (cAmbientINOUT.r*(1.f-fWeight));
			cAmbientINOUT.g = (m_vecFogLocal[m_vecComputeFog[i].dwIndex]->m_cAmbient.g*fWeight) + (cAmbientINOUT.g*(1.f-fWeight));
			cAmbientINOUT.b = (m_vecFogLocal[m_vecComputeFog[i].dwIndex]->m_cAmbient.b*fWeight) + (cAmbientINOUT.b*(1.f-fWeight));
		}
	}
}

void DxFogLocalMan::CalculateLightRate( const D3DXVECTOR3& vPos, FLOAT& fRateMax )
{
	D3DXVECTOR3 vPosSRC;
	D3DXVECTOR3 vDir;
	float fLength;
	float fRate;

	for( size_t i=0; i<m_vecFogLocal.size(); ++i )
	{
		const DxFogLocal* pFogLocal = m_vecFogLocal[ i ];

		vPosSRC.x = pFogLocal->m_matWorld._41;
		vPosSRC.y = pFogLocal->m_matWorld._42;
		vPosSRC.z = pFogLocal->m_matWorld._43;

		vDir = vPosSRC - vPos;

		fLength = D3DXVec3Length( &vDir );
		if( fLength >= pFogLocal->m_fRange1 )
			continue;

		if( fLength < pFogLocal->m_fRange2 )
		{
			fRate = 1.0f;
		}
		else
		{
			fRate = ( fLength - pFogLocal->m_fRange2 ) / ( pFogLocal->m_fRange1 - pFogLocal->m_fRange2 );
			fRate = 1.0f - fRate;
		}

		fRateMax = new_max( fRateMax, fRate );
		if( fRateMax >= 1.0f )
			break;
	}

	fRateMax = new_min( fRateMax, 1.0f );
	fRateMax = new_max( fRateMax, 0.0f );
}

void DxFogLocalMan::Render( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX* pmatReflect )
{
	DWORD dwAlphaOP(0);
	DWORD dwAlphaARG1(0);
	DWORD dwAlphaARG2(0);
	DWORD dwFogEnable(0);
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,   &dwAlphaOP );
	pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG1, &dwAlphaARG1 );
	pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG2, &dwAlphaARG2 );

	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,	D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );

	for( DWORD i=0; i<m_vecFogLocal.size(); ++i )
	{
		m_vecFogLocal[i]->Render( pd3dDevice, pmatReflect );
	}

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, dwAlphaOP );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, dwAlphaARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, dwAlphaARG2 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE , TRUE );
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	TRUE );
}

void DxFogLocalMan::Render_LightShafts( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( DWORD i=0; i<m_vecFogLocal.size(); ++i )
	{
		m_vecFogLocal[i]->Render_LightShafts( pd3dDevice );
	}
}

void DxFogLocalMan::Insert( const D3DXVECTOR3& vPos )
{
	DxFogLocal* pFogLocal = new DxFogLocal;
	pFogLocal->m_matWorld._41 = vPos.x;
	pFogLocal->m_matWorld._42 = vPos.y;
	pFogLocal->m_matWorld._43 = vPos.z;
	m_vecFogLocal.push_back( pFogLocal );
}

void DxFogLocalMan::Modify( int nIndex, const D3DXVECTOR3& vPos, float fRange1, float fRange2, float fStart, float fEnd, const D3DXCOLOR& cColor )
{
	if( nIndex >= (int) m_vecFogLocal.size() )
	{
		return;
	}

	DxFogLocal* pFogLocal	= m_vecFogLocal[nIndex];
	pFogLocal->m_matWorld._41	= vPos.x;
	pFogLocal->m_matWorld._42	= vPos.y;
	pFogLocal->m_matWorld._43	= vPos.z;
	pFogLocal->m_fRange1		= fRange1;
	pFogLocal->m_fRange2		= fRange2;
	pFogLocal->m_fFogStart		= fStart;
	pFogLocal->m_fFogEnd		= fEnd;
	pFogLocal->m_cFogColor		= cColor;
}

void DxFogLocalMan::Delete( int nIndex )
{
	int nCount(0);
	VEC_FOG_LOCAL_ITER iter = m_vecFogLocal.begin();
	for( ;iter!=m_vecFogLocal.end(); ++iter, ++nCount )
	{
		if( nIndex==nCount )
		{
			m_vecFogLocal.erase( iter );
			return;
		}
	}
}

void DxFogLocalMan::SetObjRotate90( const D3DXMATRIX& matRotate )
{
	for( DWORD i=0; i<m_vecFogLocal.size(); ++i )
	{
		D3DXMatrixMultiply(
			&m_vecFogLocal[i]->m_matWorld,
			&m_vecFogLocal[i]->m_matWorld,
			&matRotate );
	}
}

void DxFogLocalMan::RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwFogEnable;
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	for( DWORD i=0; i<m_vecFogLocal.size(); ++i )
	{
		m_vecFogLocal[i]->RenderEdit( pd3dDevice );
	}

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );
}

void DxFogLocalMan::Save( sc::SerialFile &SFile )
{
	SFile << (DWORD)VERSION;
	SFile.BeginBlock();
	{
		DWORD dwSize = (DWORD) m_vecFogLocal.size();
		SFile << dwSize;
		for( DWORD i=0; i<m_vecFogLocal.size(); ++i )
		{
			m_vecFogLocal[i]->Save( SFile );
		}
	}
	SFile.EndBlock();
}

void DxFogLocalMan::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile &SFile )
{
	CleanUp();

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	DWORD dwSize(0);
	SFile >> dwSize;
	for( DWORD i=0; i<dwSize; ++i )
	{
		DxFogLocal* pFogLocal = new DxFogLocal;
		pFogLocal->Load( pd3dDevice, SFile );
		m_vecFogLocal.push_back( pFogLocal );
	}
}

void DxFogLocalMan::Clone( LPDIRECT3DDEVICEQ pd3dDevice, const DxFogLocalMan* pSrc )
{
	CleanUp();

	for( DWORD i=0; i<pSrc->m_vecFogLocal.size(); ++i )
	{
		DxFogLocal* pFogLocal = new DxFogLocal;
		pFogLocal->Clone( pd3dDevice, pSrc->m_vecFogLocal[i] );
		m_vecFogLocal.push_back( pFogLocal );
	}
}

namespace NSFogLocalMan
{
	DxFogLocalMan* m_rFogLocalMan(NULL);

	void CleanUp()
	{
		m_rFogLocalMan = NULL;
	}

	void SetFogLocalMan( DxFogLocalMan* pFogLocalMan )
	{
		m_rFogLocalMan = pFogLocalMan;
	}

	void CalculateLight( const D3DXVECTOR3& vPos, D3DXVECTOR3& vDiffuseINOUT, D3DXVECTOR3& vAmbientINOUT )
	{
		if( !m_rFogLocalMan )
			return;

		m_rFogLocalMan->CalculateLight( vPos, vDiffuseINOUT, vAmbientINOUT );
	}
}
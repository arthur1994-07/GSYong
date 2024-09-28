#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"

#include "../DxViewPort.h"
#include "./NSShadowLight.h"

#include "DxShadowLight.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------------
DxShadowSpotLight::DxShadowSpotLight()
	: m_dwID(-1)
	, m_fAlpha(0.f)
	, m_fIndexCoolTime(0.f)
	, m_dwIndexCUR(0)
	, m_fSinTime(0.f)
	, m_fSinCycleTime(1.f)
	, m_fSinPower(0.f)
	, m_fSinAdd(1.f)
	, m_fAttEndLineValue(1.f)
	, m_fOverLighting(2.f)
{
	SetDefault( D3DXVECTOR3(0.f,0.f,0.f) );
}

void DxShadowSpotLight::SetDefault( const D3DXVECTOR3& vPos )
{
	SecureZeroMemory( &m_sLight, sizeof(D3DLIGHTQ) );
	m_sLight.Type        = D3DLIGHT_SPOT;
	m_sLight.Diffuse.r   = 1.f;
	m_sLight.Diffuse.g   = 1.f;
	m_sLight.Diffuse.b   = 1.f;

	m_sLight.Position.x = vPos.x;
	m_sLight.Position.y = vPos.y;
	m_sLight.Position.z = vPos.z;

	D3DXVECTOR3 vDirection = D3DXVECTOR3(0.f, -1.f, 0.01f);
	D3DXVec3Normalize ( &vDirection, &vDirection );
	m_sLight.Direction = vDirection;

	m_sLight.Range		= 80.0f;

	m_sLight.Attenuation0 = 1.0f;
	m_sLight.Attenuation1 = 0.0f;
	m_sLight.Attenuation2 = 0.0f;

	m_sLight.Falloff	= 1.f;
	m_sLight.Theta		= 45.f;          // Inner angle of spotlight cone - 우리가 사용할때는 D3DX_PI로 변환해서 사용한다.
	m_sLight.Phi		= 90.f;          // Outer angle of spotlight cone - 우리가 사용할때는 D3DX_PI로 변환해서 사용한다.

	D3DXMatrixIdentity( &m_matWorld );
	m_matWorld._41 = vPos.x;
	m_matWorld._42 = vPos.y;
	m_matWorld._43 = vPos.z;

	m_strName = _T("UNNAMED");
	m_bEditCheck = FALSE;
	m_nAttenuation0 = 100;
	m_nAttenuation1 = 0;
	m_nAttenuation2 = 0;
}

// Game의 Mesh들 렌더링 전에 호출하도록 한다.
void DxShadowSpotLight::PreRender( const D3DXMATRIX& matView )
{
	D3DXVECTOR3 vPosition(m_matWorld._41,m_matWorld._42,m_matWorld._43);
	D3DXVec3TransformCoord( &m_vPositionViewSpace, &vPosition, &matView );

	D3DXVECTOR3 vDirection(-m_sLight.Direction.x,-m_sLight.Direction.y,-m_sLight.Direction.z);
	D3DXVec3TransformNormal( &m_vDirectionViewSpace, &vDirection, &matView );
}

void DxShadowSpotLight::FrameMove( float fElapsedTime )
{
	m_fSinTime += fElapsedTime * (2.f * D3DX_PI / m_fSinCycleTime);
	float fSinPower(m_fSinAdd);
	if ( m_fSinPower != 0.f )
	{
		fSinPower = sinf(m_fSinTime) * m_fSinPower + m_fSinAdd;
		if ( fSinPower < 0.f )
			fSinPower = 0.f;
	}

	float fColor(0.f);
	float fRate(0.f);
	float fInvRate(0.f);
	DWORD dwR(0);
	DWORD dwG(0);
	DWORD dwB(0);
	DWORD dwSize = (DWORD)m_vecColor.size();
	switch( dwSize )
	{
	case 0:
		m_sLight.Diffuse.r = 0.f;
		m_sLight.Diffuse.g = 0.f;
		m_sLight.Diffuse.b = 0.f;
		break;

	case 1:
		fColor = static_cast<float>(m_vecColor[0].m_dwR) * fSinPower / 255;
		if ( fColor > 1.f )
			fColor = 1.f;
		m_sLight.Diffuse.r = fColor;

		fColor = static_cast<float>(m_vecColor[0].m_dwG) * fSinPower / 255;
		if ( fColor > 1.f )
			fColor = 1.f;
		m_sLight.Diffuse.g = fColor;

		fColor = static_cast<float>(m_vecColor[0].m_dwB) * fSinPower / 255;
		if ( fColor > 1.f )
			fColor = 1.f;
		m_sLight.Diffuse.b = fColor;
		break;

	default:
		m_fIndexCoolTime += fElapsedTime;
		if( m_fIndexCoolTime > m_vecColor[m_dwIndexCUR].m_fTime )
		{
			m_fIndexCoolTime = 0.f;
			++m_dwIndexCUR;
		}

		if( m_dwIndexCUR >= dwSize )	
			m_dwIndexCUR = 0;

		if ( m_vecColor[m_dwIndexCUR].m_bFlash == TRUE )
		{
			fRate = 1.f;
			fInvRate = 0.f;
		}
		else
		{
			fRate = m_fIndexCoolTime/m_vecColor[m_dwIndexCUR].m_fTime;
			if ( fRate > 1.f )
				fRate = 1.f;
			fInvRate = 1.f - fRate;
			if ( fInvRate > 1.f )
				fInvRate = 1.f;
		}

		DWORD dwR, dwG, dwB;
		if( m_dwIndexCUR == 0 )
		{
			dwR = (DWORD)((m_vecColor[0].m_dwR * fRate) + (m_vecColor[dwSize-1].m_dwR * fInvRate));
			dwG = (DWORD)((m_vecColor[0].m_dwG * fRate) + (m_vecColor[dwSize-1].m_dwG * fInvRate));
			dwB = (DWORD)((m_vecColor[0].m_dwB * fRate) + (m_vecColor[dwSize-1].m_dwB * fInvRate));
		}
		else
		{
			dwR = (DWORD)((m_vecColor[m_dwIndexCUR].m_dwR * fRate) +  (m_vecColor[m_dwIndexCUR-1].m_dwR * fInvRate));
			dwG = (DWORD)((m_vecColor[m_dwIndexCUR].m_dwG * fRate) +  (m_vecColor[m_dwIndexCUR-1].m_dwG * fInvRate));
			dwB = (DWORD)((m_vecColor[m_dwIndexCUR].m_dwB * fRate) +  (m_vecColor[m_dwIndexCUR-1].m_dwB * fInvRate));
		}

		fColor = static_cast<float>(dwR) * fSinPower / 255;
		if ( fColor > 1.f )
			fColor = 1.f;
		m_sLight.Diffuse.r = fColor;

		fColor = static_cast<float>(dwG) * fSinPower / 255;
		if ( fColor > 1.f )
			fColor = 1.f;
		m_sLight.Diffuse.g = fColor;

		fColor = static_cast<float>(dwB) * fSinPower / 255;
		if ( fColor > 1.f )
			fColor = 1.f;
		m_sLight.Diffuse.b = fColor;
		break;
	}
}

void DxShadowSpotLight::InsertColor( DWORD dwR, DWORD dwG, DWORD dwB )
{
	SPOT_COLOR sColor;
	sColor.m_dwR = dwR;
	sColor.m_dwG = dwG;
	sColor.m_dwB = dwB;
	sColor.m_fTime = 1.f;
	sColor.m_bFlash = FALSE;
	m_vecColor.push_back( sColor );
}

void DxShadowSpotLight::DeleteColor( int nIndex )
{
	// 렌더쪽에 문제가 생길 수 있는 부분을 없앤다.
	m_dwIndexCUR = 0;

	int nCount(0);
	std::vector<SPOT_COLOR>::iterator iter = m_vecColor.begin();
	for( ; iter!=m_vecColor.end(); ++iter, ++nCount )
	{
		if( nIndex==nCount )
		{
			m_vecColor.erase( iter );
			return;
		}
	}
}

void DxShadowSpotLight::ChageColor( int nIndex, DWORD dwR, DWORD dwG, DWORD dwB )
{
	int nCount(0);
	std::vector<SPOT_COLOR>::iterator iter = m_vecColor.begin();
	for( ; iter!=m_vecColor.end(); ++iter, ++nCount )
	{
		if( nIndex==nCount )
		{
			SPOT_COLOR sColor = m_vecColor[nIndex];
			sColor.m_dwR = dwR;
			sColor.m_dwG = dwG;
			sColor.m_dwB = dwB;
			m_vecColor[nIndex] = sColor;
		}
	}
}

void DxShadowSpotLight::SetColorTime( int nIndex, float fTime )
{
	int nCount(0);
	std::vector<SPOT_COLOR>::iterator iter = m_vecColor.begin();
	for( ; iter!=m_vecColor.end(); ++iter, ++nCount )
	{
		if( nIndex==nCount )
		{
			SPOT_COLOR sColor = m_vecColor[nIndex];
			sColor.m_fTime = fTime;
			m_vecColor[nIndex] = sColor;
		}
	}
}

void DxShadowSpotLight::SetFlash( int nIndex, BOOL bFlash )
{
	int nCount(0);
	std::vector<SPOT_COLOR>::iterator iter = m_vecColor.begin();
	for( ; iter!=m_vecColor.end(); ++iter, ++nCount )
	{
		if( nIndex==nCount )
		{
			SPOT_COLOR sColor = m_vecColor[nIndex];
			sColor.m_bFlash = bFlash;
			m_vecColor[nIndex] = sColor;
		}
	}
}

// Save & Load
void DxShadowSpotLight::Save ( sc::SerialFile &SFile )
{
	SFile << VERSION;
	SFile.BeginBlock();
	{
		SFile << m_fAlpha;
		SFile.WriteBuffer( &m_sLight, sizeof(D3DLIGHTQ) );
		SFile.WriteBuffer( &m_matWorld, sizeof(D3DXMATRIX) );
		SFile.WriteBuffer( &m_matCamera, sizeof(D3DXMATRIX) );

		SFile << m_fSinCycleTime;	// Save, Load
		SFile << m_fSinPower;		// Save, Load
		SFile << m_fSinAdd;			// Save, Load

		DWORD dwSize = m_vecColor.size();
		SFile << dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			SFile << m_vecColor[i].m_dwR;
			SFile << m_vecColor[i].m_dwG;
			SFile << m_vecColor[i].m_dwB;
			SFile << m_vecColor[i].m_fTime;
			SFile << m_vecColor[i].m_bFlash;
		}

		SFile << m_fAttEndLineValue;
		SFile << m_fOverLighting;

		// Edit
		SFile << m_strName;
		SFile << m_nAttenuation0;
		SFile << m_nAttenuation1;
		SFile << m_nAttenuation2;
	}
	SFile.EndBlock();
}

void DxShadowSpotLight::Load ( sc::SerialFile &SFile, DWORD dwID )
{
	m_vecColor.clear();

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	if ( dwVer==0x105 )
	{
		m_dwID = dwID;

		SFile >> m_fAlpha;
		SFile.ReadBuffer( &m_sLight, sizeof(D3DLIGHTQ) );
		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &m_matCamera, sizeof(D3DXMATRIX) );

		SFile >> m_fSinCycleTime;	// Save, Load
		SFile >> m_fSinPower;		// Save, Load
		SFile >> m_fSinAdd;			// Save, Load

		DWORD dwSize;
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			SPOT_COLOR sColor;
			SFile >> sColor.m_dwR;
			SFile >> sColor.m_dwG;
			SFile >> sColor.m_dwB;
			SFile >> sColor.m_fTime;
			SFile >> sColor.m_bFlash;
			m_vecColor.push_back( sColor );
		}

		SFile >> m_fAttEndLineValue;
		SFile >> m_fOverLighting;

		// Edit
		SFile >> m_strName;
		SFile >> m_nAttenuation0;
		SFile >> m_nAttenuation1;
		SFile >> m_nAttenuation2;
	}
	else if ( dwVer==0x104 )
	{
		m_dwID = dwID;

		SFile >> m_fAlpha;
		SFile.ReadBuffer( &m_sLight, sizeof(D3DLIGHTQ) );
		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &m_matCamera, sizeof(D3DXMATRIX) );

		SFile >> m_fSinCycleTime;	// Save, Load
		SFile >> m_fSinPower;		// Save, Load
		SFile >> m_fSinAdd;			// Save, Load

		DWORD dwSize;
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			SPOT_COLOR sColor;
			SFile >> sColor.m_dwR;
			SFile >> sColor.m_dwG;
			SFile >> sColor.m_dwB;
			SFile >> sColor.m_fTime;
			SFile >> sColor.m_bFlash;
			m_vecColor.push_back( sColor );
		}

		SFile >> m_fAttEndLineValue;

		// Edit
		SFile >> m_strName;
		SFile >> m_nAttenuation0;
		SFile >> m_nAttenuation1;
		SFile >> m_nAttenuation2;
	}
	else if ( dwVer==0x103 )
	{
		m_dwID = dwID;

		SFile >> m_fAlpha;
		SFile.ReadBuffer( &m_sLight, sizeof(D3DLIGHTQ) );
		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &m_matCamera, sizeof(D3DXMATRIX) );

		SFile >> m_fSinCycleTime;	// Save, Load
		SFile >> m_fSinPower;		// Save, Load
		SFile >> m_fSinAdd;			// Save, Load

		DWORD dwSize;
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			SPOT_COLOR sColor;
			SFile >> sColor.m_dwR;
			SFile >> sColor.m_dwG;
			SFile >> sColor.m_dwB;
			SFile >> sColor.m_fTime;
			SFile >> sColor.m_bFlash;
			m_vecColor.push_back( sColor );
		}

		// Edit
		SFile >> m_strName;
		SFile >> m_nAttenuation0;
		SFile >> m_nAttenuation1;
		SFile >> m_nAttenuation2;
	}
	else if ( dwVer==0x102 )
	{
		m_dwID = dwID;

		SFile >> m_fAlpha;
		SFile.ReadBuffer( &m_sLight, sizeof(D3DLIGHTQ) );
		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &m_matCamera, sizeof(D3DXMATRIX) );

		{
			SPOT_COLOR sColor;
			sColor.m_dwR = static_cast<DWORD>( m_sLight.Diffuse.r * 255 );
			sColor.m_dwG = static_cast<DWORD>( m_sLight.Diffuse.g * 255 );
			sColor.m_dwB = static_cast<DWORD>( m_sLight.Diffuse.b * 255 );
			m_vecColor.push_back( sColor );
		}

		// Edit
		SFile >> m_strName;
		SFile >> m_nAttenuation0;
		SFile >> m_nAttenuation1;
		SFile >> m_nAttenuation2;
	}
	else if ( dwVer==0x101 || dwVer==0x100 )
	{
		SFile >> m_dwID;
		SFile >> m_fAlpha;
		SFile.ReadBuffer( &m_sLight, sizeof(D3DLIGHTQ) );
		SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &m_matCamera, sizeof(D3DXMATRIX) );

		// Edit
		SFile >> m_strName;
		SFile >> m_nAttenuation0;
		SFile >> m_nAttenuation1;
		SFile >> m_nAttenuation2;

		m_dwID = dwID;
	}
}

//-----------------------------------------------------------------------------------------------
DxShadowLightNode::DxShadowLightNode()
	: m_rShadowSpotLight(NULL)
	, m_pNext(NULL)
{
}

DxShadowLightNode::~DxShadowLightNode()
{
	m_rShadowSpotLight = NULL;
	SAFE_DELETE( m_pNext );
}

// NSShadowLight 의 g_vecShadowSpotLight 와 g_pTreeHead 의 데이터를 연결해준다.
void DxShadowLightNode::SetData( DxShadowSpotLight* pShadowSpotLight, DxShadowLightNode** ppTreeHead )
{
	m_rShadowSpotLight = pShadowSpotLight;
	m_pNext = (*ppTreeHead);
	(*ppTreeHead) = this;
}

void DxShadowLightNode::FrameMove( float fElapsedTime )
{ 
	if ( !m_rShadowSpotLight )
		return;

	const D3DLIGHT9& sLight = m_rShadowSpotLight->m_sLight;
	{
		// 조금더 중점쪽으로 계산할 수 있는 수치를 구한다.
		D3DXVECTOR3 vPosOffset;
		vPosOffset.x = sLight.Direction.x * sLight.Range * 0.5f;
		vPosOffset.y = sLight.Direction.y * sLight.Range * 0.5f;
		vPosOffset.z = sLight.Direction.z * sLight.Range * 0.5f;

		const D3DXVECTOR3& vLookAt = DxViewPort::GetInstance().GetLookatPt ();
		D3DXVECTOR3 vDistance;
		vDistance.x = vLookAt.x - m_rShadowSpotLight->m_matWorld._41 - vPosOffset.x;
		vDistance.y = vLookAt.y - m_rShadowSpotLight->m_matWorld._42 - vPosOffset.y;
		vDistance.z = vLookAt.z - m_rShadowSpotLight->m_matWorld._43 - vPosOffset.z;

		// 거리를 구한다.
		float fDistance = D3DXVec3Length( &vDistance );

		// 범위도 계산에 포함되도록 한다.
		fDistance -= sLight.Range;

		// 거리가 가까울 경우만 ShadowSpotLight 를 동작하도록 한다.
		if ( fDistance < 250.f )
		{
			// insert ~!!
			NSShadowLight::Insert_mapDistanceLightID( fDistance, m_rShadowSpotLight->m_dwID );
		}
	}
}

void DxShadowLightNode::GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	vMax = m_vMax;
	vMin = m_vMin;
}

BOOL DxShadowLightNode::IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	D3DXVECTOR3 vCenter = ( m_vMax + m_vMin ) * 0.5f;
	return COLLISION::IsWithInPoint( vDivMax, vDivMin, vCenter );
}

OBJAABB* DxShadowLightNode::GetNext()
{
	return m_pNext;
}

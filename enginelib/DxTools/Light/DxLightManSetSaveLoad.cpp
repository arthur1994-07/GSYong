#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"

#include "../DxMethods.h"
#include "DxLightMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

static void ColorUp( D3DCOLORVALUE& d3dColor, float fValue )
{
	float fDiffR = d3dColor.r;
	float fDiffG = d3dColor.g;
	float fDiffB = d3dColor.b;

	fDiffR *= fValue;
	fDiffG *= fValue;
	fDiffB *= fValue;

	d3dColor.r = d3dColor.r + fDiffR;
	d3dColor.g = d3dColor.g + fDiffG;
	d3dColor.b = d3dColor.b + fDiffB;

	if( d3dColor.r > 1.f )		d3dColor.r = 1.f;
	if( d3dColor.g > 1.f )		d3dColor.g = 1.f;
	if( d3dColor.b > 1.f )		d3dColor.b = 1.f;
}

void	DXLIGHT::SaveSet ( sc::SerialFile &SFile )
{
    SFile << static_cast<DWORD>( VERSION );
    SFile.BeginBlock();
    {
	    BOOL	bResult = FALSE;

	    SFile.WriteBuffer( m_matWorld, sizeof( D3DXMATRIX ) );
       	
	    SFile.WriteBuffer ( m_szLightName, sizeof ( char ) * 256 );
	    SFile << m_bEnablePL_Day;
        SFile << m_bEnablePL_Night;
	    SFile << m_BaseDiffuse;

        SFile << m_nAttenuation0;
        SFile << m_nAttenuation1;
        SFile << m_nAttenuation2;

		SFile << m_fOverLighting;

	    m_Light.Diffuse = m_BaseDiffuse;	// 기본값 저장

	    if ( m_Light.Type == D3DLIGHT_POINT )	// 엠뷰언트 값 저장
	    {
		    m_Light.Ambient.r = 0.f;
		    m_Light.Ambient.g = 0.f;
		    m_Light.Ambient.b = 0.f;
	    }

	    SFile.WriteBuffer ( &m_Light, sizeof ( D3DLIGHTQ ) );
    }
    SFile.EndBlock();
}
void	DXLIGHTNODE::SaveSet ( sc::SerialFile &SFile )
{
	BOOL	bResult = FALSE;

	SFile.WriteBuffer ( &vMax, sizeof ( D3DXVECTOR3 ) );
	SFile.WriteBuffer ( &vMin, sizeof ( D3DXVECTOR3 ) );

	if ( pLight )
	{
		SFile << BOOL ( TRUE );
		SFile.WriteBuffer ( pLight->m_szLightName, sizeof ( char ) * 256 );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( pLeftChild )
	{
		SFile << BOOL ( TRUE );
		pLeftChild->SaveSet ( SFile );		
	}
	else
	{
		SFile << BOOL ( FALSE );
	}

	if ( pRightChild )
	{
		SFile << BOOL ( TRUE );
		pRightChild->SaveSet ( SFile );
	}
	else
	{
		SFile << BOOL ( FALSE );
	}
}

void	DXLIGHT::LoadSet( sc::SerialFile &SFile )
{
    DWORD dwVer, dwBufferSize;
    SFile >> dwVer;
    SFile >> dwBufferSize;

	if ( dwVer == 0x102 )
	{
		SFile.ReadBuffer( &m_matWorld, sizeof( D3DXMATRIX ) );

		SFile.ReadBuffer ( m_szLightName, sizeof ( char ) * 256 );
		SFile >> m_bEnablePL_Day;
		SFile >> m_bEnablePL_Night;
		SFile >> m_BaseDiffuse;

		SFile >> m_nAttenuation0;
		SFile >> m_nAttenuation1;
		SFile >> m_nAttenuation2;

		SFile >> m_fOverLighting;

		SFile.ReadBuffer ( &m_Light, sizeof ( D3DLIGHTQ ) );

		// Attenuation를 알맞은 수치로 바꾼다.
		{
			m_Light.Attenuation0 = static_cast<float>(m_nAttenuation0) * 0.01f;
			m_Light.Attenuation1 = static_cast<float>(m_nAttenuation1) * 0.01f / ( m_Light.Range * 0.5f );
			m_Light.Attenuation2 = static_cast<float>(m_nAttenuation2) * 0.01f / ( m_Light.Range * m_Light.Range * 0.5f );
		}

		if ( (m_BaseDiffuse.a != m_Light.Diffuse.a) || (m_BaseDiffuse.r != m_Light.Diffuse.r) ||
			(m_BaseDiffuse.g != m_Light.Diffuse.g) || (m_BaseDiffuse.b != m_Light.Diffuse.b))
		{
			m_Light.Ambient	= m_BaseDiffuse;
			m_BaseDiffuse	= m_Light.Diffuse;
		}

		float	fSpecular;
		fSpecular = m_Light.Diffuse.r;
		if ( fSpecular >= 1.f )		m_Light.Specular.r = 1.f;
		else						m_Light.Specular.r = fSpecular;

		fSpecular = m_Light.Diffuse.g;
		if ( fSpecular >= 1.f )		m_Light.Specular.g = 1.f;
		else						m_Light.Specular.g = fSpecular;

		fSpecular = m_Light.Diffuse.b;
		if ( fSpecular >= 1.f )		m_Light.Specular.b = 1.f;
		else						m_Light.Specular.b = fSpecular;
	}
	else if ( dwVer == 0x101 )
	{
		SFile.ReadBuffer( &m_matWorld, sizeof( D3DXMATRIX ) );

		SFile.ReadBuffer ( m_szLightName, sizeof ( char ) * 256 );
		SFile >> m_bEnablePL_Day;
		SFile >> m_bEnablePL_Night;
		SFile >> m_BaseDiffuse;

		SFile >> m_nAttenuation0;
		SFile >> m_nAttenuation1;
		SFile >> m_nAttenuation2;

		SFile.ReadBuffer ( &m_Light, sizeof ( D3DLIGHTQ ) );

		// Attenuation를 알맞은 수치로 바꾼다.
		{
			m_Light.Attenuation0 = static_cast<float>(m_nAttenuation0) * 0.01f;
			m_Light.Attenuation1 = static_cast<float>(m_nAttenuation1) * 0.01f / ( m_Light.Range * 0.5f );
			m_Light.Attenuation2 = static_cast<float>(m_nAttenuation2) * 0.01f / ( m_Light.Range * m_Light.Range * 0.5f );
		}

		if ( (m_BaseDiffuse.a != m_Light.Diffuse.a) || (m_BaseDiffuse.r != m_Light.Diffuse.r) ||
			(m_BaseDiffuse.g != m_Light.Diffuse.g) || (m_BaseDiffuse.b != m_Light.Diffuse.b))
		{
			m_Light.Ambient	= m_BaseDiffuse;
			m_BaseDiffuse	= m_Light.Diffuse;
		}

		float	fSpecular;
		fSpecular = m_Light.Diffuse.r;
		if ( fSpecular >= 1.f )		m_Light.Specular.r = 1.f;
		else						m_Light.Specular.r = fSpecular;

		fSpecular = m_Light.Diffuse.g;
		if ( fSpecular >= 1.f )		m_Light.Specular.g = 1.f;
		else						m_Light.Specular.g = fSpecular;

		fSpecular = m_Light.Diffuse.b;
		if ( fSpecular >= 1.f )		m_Light.Specular.b = 1.f;
		else						m_Light.Specular.b = fSpecular;
	}
	else if ( dwVer == 0x100 )
	{
		BOOL	bResult = FALSE;

		SFile.ReadBuffer( &m_matWorld, sizeof( D3DXMATRIX ) );

		SFile.ReadBuffer ( m_szLightName, sizeof ( char ) * 256 );
		SFile >> m_bEnablePL_Day;
		SFile >> m_bEnablePL_Night;
		SFile >> m_BaseDiffuse;

		SFile >> m_nAttenuation0;
		SFile >> m_nAttenuation1;
		SFile >> m_nAttenuation2;

		SFile.ReadBuffer ( &m_Light, sizeof ( D3DLIGHTQ ) );

		// Attenuation를 알맞은 수치로 바꾼다.
		{
			m_Light.Attenuation0 = static_cast<float>(m_nAttenuation0) * 0.01f;
			m_Light.Attenuation1 = static_cast<float>(m_nAttenuation1) * 0.01f / ( m_Light.Range * 0.5f );
			m_Light.Attenuation2 = static_cast<float>(m_nAttenuation2) * 0.01f / ( m_Light.Range * m_Light.Range * 0.5f );
		}

		if ( (m_BaseDiffuse.a != m_Light.Diffuse.a) || (m_BaseDiffuse.r != m_Light.Diffuse.r) ||
			(m_BaseDiffuse.g != m_Light.Diffuse.g) || (m_BaseDiffuse.b != m_Light.Diffuse.b))
		{
			m_Light.Ambient	= m_BaseDiffuse;
			m_BaseDiffuse	= m_Light.Diffuse;
		}

		float	fSpecular;
		fSpecular = m_Light.Diffuse.r;
		if ( fSpecular >= 1.f )		m_Light.Specular.r = 1.f;
		else						m_Light.Specular.r = fSpecular;

		fSpecular = m_Light.Diffuse.g;
		if ( fSpecular >= 1.f )		m_Light.Specular.g = 1.f;
		else						m_Light.Specular.g = fSpecular;

		fSpecular = m_Light.Diffuse.b;
		if ( fSpecular >= 1.f )		m_Light.Specular.b = 1.f;
		else						m_Light.Specular.b = fSpecular;

		SFile >> bResult;
		if ( bResult )
		{		
			pNext = new DXLIGHT();
			pNext->LoadSet ( SFile );	
		}
	}
	else
	{
		SFile.SetOffSet( SFile.GetfTell() + dwBufferSize );
	}
}

void	DXLIGHT::LoadSet_Ver106_8( sc::SerialFile &SFile )
{
	BOOL	bResult = FALSE;

	SFile.ReadBuffer( &m_matWorld, sizeof( D3DXMATRIX ) );

	SFile.ReadBuffer ( m_szLightName, sizeof ( char ) * 256 );
	SFile >> m_bEnablePL_Night;
    m_bEnablePL_Day = m_bEnablePL_Night;
    SFile >> m_BaseDiffuse;

    SFile >> m_nAttenuation0;
    SFile >> m_nAttenuation1;
    SFile >> m_nAttenuation2;

	SFile.ReadBuffer ( &m_Light, sizeof ( D3DLIGHTQ ) );

    // Attenuation를 알맞은 수치로 바꾼다.
    {
        m_Light.Attenuation0 = static_cast<float>(m_nAttenuation0) * 0.01f;
	    m_Light.Attenuation1 = static_cast<float>(m_nAttenuation1) * 0.01f / ( m_Light.Range * 0.5f );
	    m_Light.Attenuation2 = static_cast<float>(m_nAttenuation2) * 0.01f / ( m_Light.Range * m_Light.Range * 0.5f );
    }

	if ( (m_BaseDiffuse.a != m_Light.Diffuse.a) || (m_BaseDiffuse.r != m_Light.Diffuse.r) ||
		(m_BaseDiffuse.g != m_Light.Diffuse.g) || (m_BaseDiffuse.b != m_Light.Diffuse.b))
	{
		m_Light.Ambient	= m_BaseDiffuse;
		m_BaseDiffuse	= m_Light.Diffuse;
	}

	float	fSpecular;
	fSpecular = m_Light.Diffuse.r;
	if ( fSpecular >= 1.f )		m_Light.Specular.r = 1.f;
	else						m_Light.Specular.r = fSpecular;

	fSpecular = m_Light.Diffuse.g;
	if ( fSpecular >= 1.f )		m_Light.Specular.g = 1.f;
	else						m_Light.Specular.g = fSpecular;

	fSpecular = m_Light.Diffuse.b;
	if ( fSpecular >= 1.f )		m_Light.Specular.b = 1.f;
	else						m_Light.Specular.b = fSpecular;

	SFile >> bResult;
	if ( bResult )
	{		
        pNext = new DXLIGHT();
        pNext->LoadSet_Ver106_8 ( SFile );	
	}
}

void	DXLIGHT::LoadSet_Ver103_5( sc::SerialFile &SFile )
{
	BOOL	bResult = FALSE;

	SFile.ReadBuffer( &m_matWorld, sizeof( D3DXMATRIX ) );

	SFile.ReadBuffer ( m_szLightName, sizeof ( char ) * 256 );
	SFile >> m_bEnablePL_Night;
    m_bEnablePL_Day = m_bEnablePL_Night;
	SFile >> m_BaseDiffuse;

	SFile.ReadBuffer ( &m_Light, sizeof ( D3DLIGHTQ ) );

	if ( (m_BaseDiffuse.a != m_Light.Diffuse.a) || (m_BaseDiffuse.r != m_Light.Diffuse.r) ||
		(m_BaseDiffuse.g != m_Light.Diffuse.g) || (m_BaseDiffuse.b != m_Light.Diffuse.b))
	{
		m_Light.Ambient	= m_BaseDiffuse;
		m_BaseDiffuse	= m_Light.Diffuse;
	}

    m_Light.Attenuation0 = 1.0f;
	m_Light.Attenuation1 = 1.0f/m_Light.Range;		// 최대거리일 경우, (값 = 색 / 1.5f)
	m_Light.Attenuation2 = 0.0f;

    m_nAttenuation0 = 100;
    m_nAttenuation1 = 10;
    m_nAttenuation2 = 0;

	float	fSpecular;
	fSpecular = m_Light.Diffuse.r;
	if ( fSpecular >= 1.f )		m_Light.Specular.r = 1.f;
	else						m_Light.Specular.r = fSpecular;

	fSpecular = m_Light.Diffuse.g;
	if ( fSpecular >= 1.f )		m_Light.Specular.g = 1.f;
	else						m_Light.Specular.g = fSpecular;

	fSpecular = m_Light.Diffuse.b;
	if ( fSpecular >= 1.f )		m_Light.Specular.b = 1.f;
	else						m_Light.Specular.b = fSpecular;

	SFile >> bResult;
	if ( bResult )
	{		
        pNext = new DXLIGHT();
        pNext->LoadSet_Ver103_5 ( SFile );	
	}
}

void	DXLIGHT::LoadSet_Ver101_2( sc::SerialFile &SFile )
{
	BOOL	bResult = FALSE;

	SFile.ReadBuffer( &m_matWorld, sizeof( D3DXMATRIX ) );

	SFile.ReadBuffer ( m_szLightName, sizeof ( char ) * 256 );
	SFile >> m_bEnablePL_Night;
    m_bEnablePL_Day = m_bEnablePL_Night;
	SFile >> m_BaseDiffuse;

	SFile.ReadBuffer ( &m_Light, sizeof ( D3DLIGHTQ ) );

	if ( (m_BaseDiffuse.a != m_Light.Diffuse.a) || (m_BaseDiffuse.r != m_Light.Diffuse.r) ||
		(m_BaseDiffuse.g != m_Light.Diffuse.g) || (m_BaseDiffuse.b != m_Light.Diffuse.b))
	{
		m_Light.Ambient	= m_BaseDiffuse;
		m_BaseDiffuse	= m_Light.Diffuse;
	}

	if ( m_Light.Type == D3DLIGHT_POINT )
	{
		ColorUp( m_Light.Diffuse, 0.6f );			// MODULATE2X -> MODULATE 로 바뀌면서 보정작업
		m_Light.Ambient.r = 0.f;
		m_Light.Ambient.g = 0.f;
		m_Light.Ambient.b = 0.f;
	}

	m_Light.Attenuation0 = 1.0f;
	m_Light.Attenuation1 = 1.0f/m_Light.Range;		// 최대거리일 경우, (값 = 색 / 1.5f)
	m_Light.Attenuation2 = 0.0f;

    m_nAttenuation0 = 100;
    m_nAttenuation1 = 10;
    m_nAttenuation2 = 0;

	float	fSpecular;
	fSpecular = m_Light.Diffuse.r;
	if ( fSpecular >= 1.f )		m_Light.Specular.r = 1.f;
	else						m_Light.Specular.r = fSpecular;

	fSpecular = m_Light.Diffuse.g;
	if ( fSpecular >= 1.f )		m_Light.Specular.g = 1.f;
	else						m_Light.Specular.g = fSpecular;

	fSpecular = m_Light.Diffuse.b;
	if ( fSpecular >= 1.f )		m_Light.Specular.b = 1.f;
	else						m_Light.Specular.b = fSpecular;

	SFile >> bResult;
	if ( bResult )
	{		
        pNext = new DXLIGHT();
        pNext->LoadSet_Ver101_2( SFile );	
	}
}

void	DXLIGHT::LoadSet_Ver100( sc::SerialFile &SFile )
{
	BOOL	bResult = FALSE;

	DXAFFINEPARTS m_sAffineParts;

	SFile >> bResult;
	if ( bResult )
	{
		SFile.ReadBuffer( &m_sAffineParts, sizeof ( DXAFFINEPARTS ) );
	}

	D3DXMatrixCompX( m_matWorld, m_sAffineParts );	// m_matWorld 만 사용하게 되어서 변환

	SFile.ReadBuffer ( m_szLightName, sizeof ( char ) * 256 );
	SFile >> m_bEnablePL_Night;
    m_bEnablePL_Day = m_bEnablePL_Night;
	SFile >> m_BaseDiffuse;

	SFile.ReadBuffer ( &m_Light, sizeof ( D3DLIGHTQ ) );

	if ( (m_BaseDiffuse.a != m_Light.Diffuse.a) || (m_BaseDiffuse.r != m_Light.Diffuse.r) ||
		(m_BaseDiffuse.g != m_Light.Diffuse.g) || (m_BaseDiffuse.b != m_Light.Diffuse.b))
	{
		m_Light.Ambient	= m_BaseDiffuse;
		m_BaseDiffuse	= m_Light.Diffuse;
	}

	if ( m_Light.Type == D3DLIGHT_POINT )
	{
		ColorUp( m_Light.Diffuse, 0.6f );			// MODULATE2X -> MODULATE 로 바뀌면서 보정작업
		m_Light.Ambient.r = 0.f;
		m_Light.Ambient.g = 0.f;
		m_Light.Ambient.b = 0.f;
	}

	m_Light.Attenuation0 = 1.0f;
	m_Light.Attenuation1 = 1.0f/m_Light.Range;		// 최대거리일 경우, (값 = 색 / 1.5f)
	m_Light.Attenuation2 = 0.0f;

    m_nAttenuation0 = 100;
    m_nAttenuation1 = 10;
    m_nAttenuation2 = 0;

	float	fSpecular;
	fSpecular = m_Light.Diffuse.r;
	if ( fSpecular >= 1.f )		m_Light.Specular.r = 1.f;
	else						m_Light.Specular.r = fSpecular;

	fSpecular = m_Light.Diffuse.g;
	if ( fSpecular >= 1.f )		m_Light.Specular.g = 1.f;
	else						m_Light.Specular.g = fSpecular;

	fSpecular = m_Light.Diffuse.b;
	if ( fSpecular >= 1.f )		m_Light.Specular.b = 1.f;
	else						m_Light.Specular.b = fSpecular;

	SFile >> bResult;
	if ( bResult )
	{		
        pNext = new DXLIGHT();
        pNext->LoadSet_Ver100( SFile );	
	}
}

void	DXLIGHTNODE::LoadSet ( sc::SerialFile &SFile, const LIST_DXLIGHT& listPontLight )
{
	BOOL	bResult = FALSE;

	SFile.ReadBuffer ( &vMax, sizeof ( D3DXVECTOR3 ) );
	SFile.ReadBuffer ( &vMin, sizeof ( D3DXVECTOR3 ) );

	SFile >> bResult;
	if ( bResult )
	{
		TCHAR	szLightName[STRING_LENGTH_256] = _T("");		
		SFile.ReadBuffer ( szLightName, sizeof ( TCHAR ) * STRING_LENGTH_256 );

		//	<--	이름으로 노드를 찾아 연결함
		LIST_DXLIGHT_CITER iter = listPontLight.begin();
		for( ; iter!=listPontLight.end(); ++iter )
		{
			if ( !_tcscmp ( szLightName, (*iter)->m_szLightName ) )
			{
				pLight = (*iter);
				break;
			}
		}
	}

	SFile >> bResult;
	if ( bResult )
	{
		pLeftChild = new DXLIGHTNODE();
		pLeftChild->LoadSet ( SFile, listPontLight );
	}

	SFile >> bResult;
	if ( bResult )
	{
		pRightChild = new DXLIGHTNODE();
		pRightChild->LoadSet ( SFile, listPontLight );
	}
}

void DxLightMan::Save ( sc::SerialFile &SFile )
{
	BOOL bResult = FALSE;

	SFile << VERSION;

	SFile << m_bLighting;
	SFile << m_bWhenDayPointOff;
	SFile << m_bNightAndDay;
	SFile << m_dwForceStopTime;

	m_LtDirectNoon.SaveSet ( SFile );
	m_LtDirectNight.SaveSet ( SFile );

    SFile << m_fLM_DDPower;
    SFile.WriteBuffer( &m_vShadowColor[0], sizeof( D3DXVECTOR3 ) );
    SFile.WriteBuffer( &m_vShadowColor[1], sizeof( D3DXVECTOR3 ) );

	SFile << static_cast<DWORD>( m_listPontLight.size() );
	LIST_DXLIGHT_ITER iter = m_listPontLight.begin();
	for( ; iter!=m_listPontLight.end(); ++iter )
	{
		(*iter)->SaveSet ( SFile );
	}

	SFile << BOOL ( m_pTreeHead ? TRUE : FALSE );
    if ( m_pTreeHead )
	{
		m_pTreeHead->SaveSet ( SFile );		
	}
}

void DxLightMan::Load_100 ( sc::SerialFile &SFile )
{
	BOOL bResult = FALSE;

	SFile >> m_bLighting;
	SFile >> m_bNightAndDay;
	SFile >> m_bWhenDayPointOff;

	m_LtDirectNight.LoadSet_Ver100( SFile );
	m_LtDirectNoon.LoadSet_Ver100( SFile );

	ColorUp( m_LtDirectNoon.m_Light.Diffuse, 0.6f );
	ColorUp( m_LtDirectNight.m_Light.Ambient, 0.2f );
	ColorUp( m_LtDirectNoon.m_Light.Ambient, 0.2f );

	SFile >> bResult;
	if ( bResult )
	{
		DXLIGHT* pCur(NULL);
		pCur = new DXLIGHT();
		pCur->LoadSet_Ver100( SFile );

        while( pCur )
		{
			m_listPontLight.push_back( pCur );
			pCur = pCur->pNext;
		}
	}

	SFile >> bResult;
	if ( bResult )
	{	
		m_pTreeHead = new DXLIGHTNODE();
		m_pTreeHead->LoadSet ( SFile, m_listPontLight );		
	}
}

void DxLightMan::Load_101 ( sc::SerialFile &SFile )
{
	BOOL bResult = FALSE;

	SFile >> m_bLighting;
	SFile >> m_bNightAndDay;
	SFile >> m_bWhenDayPointOff;

	m_LtDirectNight.LoadSet_Ver101_2( SFile );
	m_LtDirectNoon.LoadSet_Ver101_2( SFile );

	ColorUp( m_LtDirectNoon.m_Light.Diffuse, 0.6f );
	ColorUp( m_LtDirectNight.m_Light.Ambient, 0.2f );
	ColorUp( m_LtDirectNoon.m_Light.Ambient, 0.2f );

	SFile >> bResult;
	if ( bResult )
	{
		DXLIGHT* pCur(NULL);
		pCur = new DXLIGHT();
		pCur->LoadSet_Ver101_2 ( SFile );

        while( pCur )
		{
			m_listPontLight.push_back( pCur );
			pCur = pCur->pNext;
		}
	}

	SFile >> bResult;
	if ( bResult )
	{	
		m_pTreeHead = new DXLIGHTNODE();
		m_pTreeHead->LoadSet ( SFile, m_listPontLight );		
	}
}

void DxLightMan::Load_102 ( sc::SerialFile &SFile )
{
	BOOL bResult = FALSE;

	SFile >> m_bLighting;
	SFile >> m_bNightAndDay;
	SFile >> m_bWhenDayPointOff;

	m_LtDirectNight.LoadSet_Ver101_2( SFile );
	m_LtDirectNoon.LoadSet_Ver101_2( SFile );

	SFile >> bResult;
	if ( bResult )
	{
		DXLIGHT* pCur(NULL);
		pCur = new DXLIGHT();
		pCur->LoadSet_Ver101_2 ( SFile );

        while( pCur )
		{
			m_listPontLight.push_back( pCur );
			pCur = pCur->pNext;
		}
	}

	SFile >> bResult;
	if ( bResult )
	{	
		m_pTreeHead = new DXLIGHTNODE();
		m_pTreeHead->LoadSet ( SFile, m_listPontLight );		
	}
}

void DxLightMan::Load_103 ( sc::SerialFile &SFile )
{
	BOOL bResult = FALSE;

	SFile >> m_bLighting;
	SFile >> m_bNightAndDay;
	SFile >> m_bWhenDayPointOff;

	m_LtDirectNight.LoadSet_Ver103_5 ( SFile );
	m_LtDirectNoon.LoadSet_Ver103_5 ( SFile );

	SFile >> bResult;
	if ( bResult )
	{
		DXLIGHT* pCur(NULL);
		pCur = new DXLIGHT();
		pCur->LoadSet_Ver103_5 ( SFile );

        while( pCur )
		{
			m_listPontLight.push_back( pCur );
			pCur = pCur->pNext;
		}
	}

	SFile >> bResult;
	if ( bResult )
	{	
		m_pTreeHead = new DXLIGHTNODE();
		m_pTreeHead->LoadSet ( SFile, m_listPontLight );		
	}
}

void DxLightMan::Load_104 ( sc::SerialFile &SFile )
{
	BOOL bResult = FALSE;

	SFile >> m_bLighting;
	SFile >> m_bWhenDayPointOff;
	SFile >> m_bNightAndDay;

	m_LtDirectNoon.LoadSet_Ver103_5 ( SFile );
	m_LtDirectNight.LoadSet_Ver103_5 ( SFile );

	SFile >> bResult;
	if ( bResult )
	{
		DXLIGHT* pCur(NULL);
		pCur = new DXLIGHT();
		pCur->LoadSet_Ver103_5 ( SFile );

        while( pCur )
		{
			m_listPontLight.push_back( pCur );
			pCur = pCur->pNext;
		}
	}

	SFile >> bResult;
	if ( bResult )
	{	
		m_pTreeHead = new DXLIGHTNODE();
		m_pTreeHead->LoadSet ( SFile, m_listPontLight );		
	}
}

void DxLightMan::Load_105 ( sc::SerialFile &SFile )
{
	BOOL bResult = FALSE;

	SFile >> m_bLighting;
	SFile >> m_bWhenDayPointOff;
	SFile >> m_bNightAndDay;

	m_LtDirectNoon.LoadSet_Ver103_5 ( SFile );
	m_LtDirectNight.LoadSet_Ver103_5 ( SFile );

    DWORD dwSize(0);
	SFile >> dwSize;
	for( DWORD i=0; i<dwSize; ++i )
	{
		DXLIGHT* pLight = new DXLIGHT();
		pLight->LoadSet_Ver103_5( SFile );
		m_listPontLight.push_back( pLight );
	}


	SFile >> bResult;
	if ( bResult )
	{	
		m_pTreeHead = new DXLIGHTNODE();
		m_pTreeHead->LoadSet ( SFile, m_listPontLight );		
	}
}

void DxLightMan::Load_106 ( sc::SerialFile &SFile )
{
	BOOL bResult = FALSE;

	SFile >> m_bLighting;
	SFile >> m_bWhenDayPointOff;
	SFile >> m_bNightAndDay;

	m_LtDirectNoon.LoadSet_Ver106_8 ( SFile );
	m_LtDirectNight.LoadSet_Ver106_8 ( SFile );

    DWORD dwSize(0);
	SFile >> dwSize;
	for( DWORD i=0; i<dwSize; ++i )
	{
		DXLIGHT* pLight = new DXLIGHT();
		pLight->LoadSet_Ver106_8( SFile );
		m_listPontLight.push_back( pLight );
	}


	SFile >> bResult;
	if ( bResult )
	{	
		m_pTreeHead = new DXLIGHTNODE();
		m_pTreeHead->LoadSet ( SFile, m_listPontLight );		
	}
}

void DxLightMan::Load_107 ( sc::SerialFile &SFile )
{
	BOOL bResult = FALSE;

	SFile >> m_bLighting;
	SFile >> m_bWhenDayPointOff;
	SFile >> m_bNightAndDay;

	m_LtDirectNoon.LoadSet_Ver106_8 ( SFile );
	m_LtDirectNight.LoadSet_Ver106_8 ( SFile );

    SFile.ReadBuffer( m_vShadowColor[0], sizeof( D3DXVECTOR3 ) );
    SFile.ReadBuffer( m_vShadowColor[1], sizeof( D3DXVECTOR3 ) );

    DWORD dwSize(0);
	SFile >> dwSize;
	for( DWORD i=0; i<dwSize; ++i )
	{
		DXLIGHT* pLight = new DXLIGHT();
		pLight->LoadSet_Ver106_8( SFile );
		m_listPontLight.push_back( pLight );
	}


	SFile >> bResult;
	if ( bResult )
	{	
		m_pTreeHead = new DXLIGHTNODE();
		m_pTreeHead->LoadSet ( SFile, m_listPontLight );		
	}
}

void DxLightMan::Load_108 ( sc::SerialFile &SFile )
{
	BOOL bResult = FALSE;

	SFile >> m_bLighting;
	SFile >> m_bWhenDayPointOff;
	SFile >> m_bNightAndDay;

	m_LtDirectNoon.LoadSet_Ver106_8 ( SFile );
	m_LtDirectNight.LoadSet_Ver106_8 ( SFile );

    SFile >> m_fLM_DDPower;
    SFile.ReadBuffer( m_vShadowColor[0], sizeof( D3DXVECTOR3 ) );
    SFile.ReadBuffer( m_vShadowColor[1], sizeof( D3DXVECTOR3 ) );

    DWORD dwSize(0);
	SFile >> dwSize;
	for( DWORD i=0; i<dwSize; ++i )
	{
		DXLIGHT* pLight = new DXLIGHT();
		pLight->LoadSet_Ver106_8( SFile );
		m_listPontLight.push_back( pLight );
	}


	SFile >> bResult;
	if ( bResult )
	{	
		m_pTreeHead = new DXLIGHTNODE();
		m_pTreeHead->LoadSet ( SFile, m_listPontLight );		
	}
}

void DxLightMan::Load_109 ( sc::SerialFile &SFile )
{
	BOOL bResult = FALSE;

	SFile >> m_bLighting;
	SFile >> m_bWhenDayPointOff;
	SFile >> m_bNightAndDay;

	m_LtDirectNoon.LoadSet( SFile );
	m_LtDirectNight.LoadSet( SFile );

    SFile >> m_fLM_DDPower;
    SFile.ReadBuffer( m_vShadowColor[0], sizeof( D3DXVECTOR3 ) );
    SFile.ReadBuffer( m_vShadowColor[1], sizeof( D3DXVECTOR3 ) );

    DWORD dwSize(0);
	SFile >> dwSize;
	for( DWORD i=0; i<dwSize; ++i )
	{
		DXLIGHT* pLight = new DXLIGHT();
		pLight->LoadSet( SFile );
		m_listPontLight.push_back( pLight );
	}


	SFile >> bResult;
	if ( bResult )
	{	
		m_pTreeHead = new DXLIGHTNODE();
		m_pTreeHead->LoadSet ( SFile, m_listPontLight );		
	}
}

void DxLightMan::Load_110 ( sc::SerialFile &SFile )
{
	BOOL bResult = FALSE;

	SFile >> m_bLighting;
	SFile >> m_bWhenDayPointOff;
	SFile >> m_bNightAndDay;
	SFile >> m_dwForceStopTime;

	m_LtDirectNoon.LoadSet( SFile );
	m_LtDirectNight.LoadSet( SFile );

	SFile >> m_fLM_DDPower;
	SFile.ReadBuffer( m_vShadowColor[0], sizeof( D3DXVECTOR3 ) );
	SFile.ReadBuffer( m_vShadowColor[1], sizeof( D3DXVECTOR3 ) );

	DWORD dwSize(0);
	SFile >> dwSize;
	for( DWORD i=0; i<dwSize; ++i )
	{
		DXLIGHT* pLight = new DXLIGHT();
		pLight->LoadSet( SFile );
		m_listPontLight.push_back( pLight );
	}


	SFile >> bResult;
	if ( bResult )
	{	
		m_pTreeHead = new DXLIGHTNODE();
		m_pTreeHead->LoadSet ( SFile, m_listPontLight );		
	}
}

void DxLightMan::Load ( sc::SerialFile &SFile, DWORD dwLandVer, BOOL bSETLAND, BOOL bMaterialSystem )
{
	//	Note : 모든 이전 데이터를 초기화.
	//
	CleanUp ();

	BOOL bResult = FALSE;

	//	Note : 이전 버전.

	DWORD dwOldVer;
	if ( bSETLAND )	dwOldVer = 0x0025;	//	DxSetLandMan::VERSION old version,
	else			dwOldVer = 0x0104;	//	DxLandMan::VERSION old version

	if ( dwLandVer < dwOldVer )
	{
		SFile >> m_bLighting;
		m_LtDirectNight.LoadSet_Ver100( SFile );	

		//	낮 Direct Light 설정.
		m_LtDirectNoon.m_BaseDiffuse = m_LtDirectNight.m_BaseDiffuse;
		m_LtDirectNoon.m_bEnablePL_Day = m_LtDirectNight.m_bEnablePL_Day;
        m_LtDirectNoon.m_bEnablePL_Night = m_LtDirectNight.m_bEnablePL_Night;
		m_LtDirectNoon.m_Light = m_LtDirectNight.m_Light;
		StringCchCopy( m_LtDirectNoon.m_szLightName, 256, m_LtDirectNight.m_szLightName );
		StringCchCat( m_LtDirectNoon.m_szLightName, 256, "[NOON]" );

		ColorUp( m_LtDirectNoon.m_Light.Diffuse, 0.6f );
		ColorUp( m_LtDirectNight.m_Light.Ambient, 0.2f );
		ColorUp( m_LtDirectNoon.m_Light.Ambient, 0.2f );

		SFile >> bResult;
		if ( bResult )
		{
			DXLIGHT* pCur(NULL);
		    pCur = new DXLIGHT();
		    pCur->LoadSet_Ver100( SFile );

            while( pCur )
		    {
			    m_listPontLight.push_back( pCur );
			    pCur = pCur->pNext;
		    }
		}

		SFile >> bResult;
		if ( bResult )
		{	
			m_pTreeHead = new DXLIGHTNODE();
			m_pTreeHead->LoadSet ( SFile, m_listPontLight );		
		}
	}
	//	버전 관리후 버전.
	else
	{
		DWORD dwVersion;
		SFile >> dwVersion;

		switch ( dwVersion )
		{
		case 0x0100:	
            Load_100(SFile);	
            break;

		case 0x0101:	
            Load_101(SFile);	
            break;

		case 0x0102:	
            Load_102(SFile);	
            break;

		case 0x0103:	
            Load_103(SFile);	
            break;

		case 0x0104:	
            Load_104(SFile);	
            break;

        case 0x0105:	
            Load_105(SFile);	
            break;

        case 0x0106:	
            Load_106(SFile);	
            break;

        case 0x0107:	
            Load_107(SFile);	
            break;

        case 0x0108:	
            Load_108(SFile);	
            break;

		case 0x0109:	
			Load_109(SFile);	
			break;

        case VERSION:	
            Load_110(SFile);	
            break;

		default:
			{
			}
			break;
		};			
	}

	// Note : 로드 한 빛을 벡터에 넣는다.
	ListToVectorLIGHT();

	// [shhan][2014.09.12] MaterialSystem 이라면 낮밤을 사용할 수 없다.
	//						이전 맵이라면 셋팅을 따른다.
	if ( bMaterialSystem )
	{
		m_bNightAndDay = FALSE;
	}
}



#include "pch.h"

#include "../../../SigmaCore/Util/Lock.h"
#include "../../../SigmaCore/gassert.h"

#include "../../Common/StlFunctions.h"
#include "./NSTexCompress.h"

#include "NSTexture16Bit.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace NSTexture16Bit
{
	typedef std::map<TSTRING,std::tr1::shared_ptr<DATA>>	MAP_TEXTURE16BIT;
	typedef MAP_TEXTURE16BIT::iterator						MAP_TEXTURE16BIT_ITER;
	MAP_TEXTURE16BIT	g_mapTexture16Bit;
	sc::DefaultLock		g_sDefaultLock;

	void CleanUp()
	{
		g_mapTexture16Bit.clear();
	}

	DATA* Find ( const TSTRING& strName )
	{
		MAP_TEXTURE16BIT_ITER iter = g_mapTexture16Bit.find( strName );
		if ( iter != g_mapTexture16Bit.end() )
		{
			return (*iter).second.get();
		}
		return NULL;
	}

	DATA* Insert( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strName, LPDIRECT3DTEXTURE9 pTexture )
	{
		if ( !pTexture )
			return NULL;

		DATA* pTextureData = Find ( strName );
		if ( pTextureData )
		{
			return pTextureData;
		}

		// lock 을 걸고 재시도.
		g_sDefaultLock.lockOn();

		pTextureData = Find ( strName );
		if ( pTextureData )
		{
			g_sDefaultLock.lockOff();
			return pTextureData;
		}

		std::tr1::shared_ptr<DATA> spTextureData = std::tr1::shared_ptr<DATA>(new DATA);

		D3DSURFACE_DESC sDesc;
		pTexture->GetLevelDesc( 0, &sDesc );

		LPDIRECT3DBASETEXTURE9 pTexTemp(NULL);
		if ( sDesc.Format == D3DFMT_A8R8G8B8 )
		{
			pTexTemp = pTexture;
		}
		else
		{
			NSTexCompress::Compress
			(
				pd3dDevice,
				sDesc.Width,
				sDesc.Height,
				0,
				1,
				D3DFMT_A8R8G8B8,
				pTexture,
				pTexTemp
			);
		}
		LPDIRECT3DTEXTURE9 pTexture16BIT = static_cast<LPDIRECT3DTEXTURE9>( pTexTemp );
		if ( !pTexture16BIT )
		{
			g_sDefaultLock.lockOff();
			return NULL;
		}

		spTextureData->m_wwColorSize = WORDWORD(sDesc.Width,sDesc.Height);
		spTextureData->m_pwColorARRAY = new WORD[ sDesc.Width * sDesc.Height ];

		D3DLOCKED_RECT stLocked;
		HRESULT hr = pTexture16BIT->LockRect( 0, &stLocked, NULL, 0 );
		if( FAILED(hr) )
		{
			GASSERT( 0 && _T("HRESULT hr = pTexture16BIT->LockRect( 0, &stLocked, NULL, 0 );") );
			return NULL;
		}

		DWORD* pData = (DWORD*)stLocked.pBits;
		INT iPitch = stLocked.Pitch / 4;

		BYTE cColor[4];
		DWORD dwColor;
		for ( DWORD y=0; y<sDesc.Height; ++y )
		{
			for ( DWORD x=0; x<sDesc.Width; ++x )
			{
				dwColor = pData[ x + y * iPitch ];

				cColor[0] = static_cast<BYTE>((dwColor & 0xff000000) >> 24);
				cColor[1] = static_cast<BYTE>((dwColor & 0xff0000) >> 16);
				cColor[2] = static_cast<BYTE>((dwColor & 0xff00) >> 8);
				cColor[3] = static_cast<BYTE>((dwColor & 0xff));

				spTextureData->m_pwColorARRAY[x+y*sDesc.Width] = NSTexCompress::ColorTo1555( &cColor[0] );
			}
		}

		pTexture16BIT->UnlockRect( 0 );

		if ( sDesc.Format != D3DFMT_A8R8G8B8 )
		{
			SAFE_RELEASE ( pTexture16BIT );
		}

		g_mapTexture16Bit.insert( std::make_pair(strName,spTextureData) );

		g_sDefaultLock.lockOff();

		return spTextureData.get();
	}

	D3DXVECTOR4 GetColor( const NSTexture16Bit::DATA* pData, const D3DXVECTOR2& vUV )
	{
		float fPixel[2];
		int nPixel[2];
		fPixel[0] = (pData->m_wwColorSize.wA * vUV.x);
		fPixel[1] = (pData->m_wwColorSize.wB * vUV.y);
		nPixel[0] = static_cast<int>(fPixel[0]);
		nPixel[1] = static_cast<int>(fPixel[1]);
		fPixel[0] -= nPixel[0];
		fPixel[1] -= nPixel[1];

		while( nPixel[0] < 0 )
		{
			nPixel[0] += pData->m_wwColorSize.wA;
		}
		while( nPixel[0] >= static_cast<int>(pData->m_wwColorSize.wA) )
		{
			nPixel[0] -= pData->m_wwColorSize.wA;
		}

		while( nPixel[1] < 0 )
		{
			nPixel[1] += pData->m_wwColorSize.wB;
		}
		while( nPixel[1] >= static_cast<int>(pData->m_wwColorSize.wB) )
		{
			nPixel[1] -= pData->m_wwColorSize.wB;
		}

		int nType(0);
		if ( fPixel[0] < 0.5f && fPixel[1] < 0.5f )
			nType = 0;
		if ( fPixel[0] >= 0.5f && fPixel[1] < 0.5f )
			nType = 1;
		if ( fPixel[0] < 0.5f && fPixel[1] >= 0.5f )
			nType = 2;
		if ( fPixel[0] >= 0.5f && fPixel[1] >= 0.5f )
			nType = 3;

		// 4 점을 가져온다.
		// x, y 비율을 셋팅한다.
		int nPixelQuad[4][2];
		float fRate[2];
		switch ( nType )
		{
		case 0:
			fRate[0] = fPixel[0] + 0.5f;
			fRate[1] = fPixel[1] + 0.5f;

			nPixelQuad[0][0] = nPixel[0] - 1;
			nPixelQuad[0][1] = nPixel[1] - 1;

			nPixelQuad[1][0] = nPixel[0];
			nPixelQuad[1][1] = nPixel[1] - 1;

			nPixelQuad[2][0] = nPixel[0] - 1;
			nPixelQuad[2][1] = nPixel[1];

			nPixelQuad[3][0] = nPixel[0];
			nPixelQuad[3][1] = nPixel[1];
			break;

		case 1:
			fRate[0] = fPixel[0] - 0.5f;
			fRate[1] = fPixel[1] + 0.5f;

			nPixelQuad[0][0] = nPixel[0];
			nPixelQuad[0][1] = nPixel[1] - 1;

			nPixelQuad[1][0] = nPixel[0] + 1;
			nPixelQuad[1][1] = nPixel[1] - 1;

			nPixelQuad[2][0] = nPixel[0];
			nPixelQuad[2][1] = nPixel[1];

			nPixelQuad[3][0] = nPixel[0] + 1;
			nPixelQuad[3][1] = nPixel[1];
			break;

		case 2:
			fRate[0] = fPixel[0] + 0.5f;
			fRate[1] = fPixel[1] - 0.5f;

			nPixelQuad[0][0] = nPixel[0] - 1;
			nPixelQuad[0][1] = nPixel[1];

			nPixelQuad[1][0] = nPixel[0];
			nPixelQuad[1][1] = nPixel[1];

			nPixelQuad[2][0] = nPixel[0] - 1;
			nPixelQuad[2][1] = nPixel[1] + 1;

			nPixelQuad[3][0] = nPixel[0];
			nPixelQuad[3][1] = nPixel[1] + 1;
			break;

		case 3:
		default:
			fRate[0] = fPixel[0] - 0.5f;
			fRate[1] = fPixel[1] - 0.5f;

			nPixelQuad[0][0] = nPixel[0];
			nPixelQuad[0][1] = nPixel[1];

			nPixelQuad[1][0] = nPixel[0] + 1;
			nPixelQuad[1][1] = nPixel[1];

			nPixelQuad[2][0] = nPixel[0];
			nPixelQuad[2][1] = nPixel[1] + 1;

			nPixelQuad[3][0] = nPixel[0] + 1;
			nPixelQuad[3][1] = nPixel[1] + 1;
			break;
		};

		D3DXVECTOR4 vColor[4];
		for ( DWORD i=0; i<4; ++i )
		{
			if ( nPixelQuad[i][0] < 0 )
				nPixelQuad[i][0] = 0;
			if ( nPixelQuad[i][1] < 0 )
				nPixelQuad[i][1] = 0;
			if ( nPixelQuad[i][0] >= static_cast<int>(pData->m_wwColorSize.wA) )
				nPixelQuad[i][0] = static_cast<int>(pData->m_wwColorSize.wA) - 1;
			if ( nPixelQuad[i][1] >= static_cast<int>(pData->m_wwColorSize.wB) )
				nPixelQuad[i][1] = static_cast<int>(pData->m_wwColorSize.wB) - 1;

			WORD wColor = pData->m_pwColorARRAY[ nPixelQuad[i][0] + nPixelQuad[i][1]*pData->m_wwColorSize.wA ];
			vColor[i].x = static_cast<float>((( wColor & 0x7c00 ) >> 10 ) << 3);
			vColor[i].y = static_cast<float>((( wColor & 0x3e0 ) >> 5 ) << 3);
			vColor[i].z = static_cast<float>(( wColor & 0x1f ) << 3);
			vColor[i].w = static_cast<float>( wColor >> 15 )*255;
		}

		D3DXVECTOR4 vColorLast;
		vColor[0] = vColor[0]*(1.f-fRate[0]) + vColor[1]*fRate[0];
		vColor[1] = vColor[2]*(1.f-fRate[0]) + vColor[3]*fRate[0];
		return vColorLast = vColor[0]*(1.f-fRate[1]) + vColor[1]*fRate[1];
	}
};
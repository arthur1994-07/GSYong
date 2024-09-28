#include "pch.h"
#include "NSTexCompress.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace NSTexCompress
{
	DWORD Filter = D3DX_DEFAULT;

	#ifndef ReleasePpo
		#define ReleasePpo(ppo) \
			if( *(ppo) != NULL ) \
			{ \
				( *(ppo) )->Release(); \
				*(ppo) = NULL; \
			} \
			else (VOID)0
	#endif

	BOOL g_bCubeMapFlags(FALSE);
	//BOOL IsCubeMap() { return FALSE; }
	BOOL IsCubeMap() { return g_bCubeMapFlags; }

    BOOL IsVolumeMap() { return FALSE; }
	//BOOL IsVolumeMap() { return ( m_dwDepth > 0 ); }

	DWORD	m_dwWidth( 0L );
	DWORD	m_dwHeight( 0L );
	DWORD	m_dwDepth( 0L );
	DWORD	m_numMips( 0L );
	DWORD	m_dwBltLevel( 0L );

	HRESULT BltAllLevels(
		D3DCUBEMAP_FACES FaceType, 
		LPDIRECT3DBASETEXTURE9 ptexSrc, 
		LPDIRECT3DBASETEXTURE9 ptexDest )
	{
		HRESULT hr = S_OK;
		LPDIRECT3DTEXTURE9 pmiptexSrc = NULL;
		LPDIRECT3DTEXTURE9 pmiptexDest = NULL;
		LPDIRECT3DCUBETEXTURE9 pcubetexSrc = NULL;
		LPDIRECT3DCUBETEXTURE9 pcubetexDest = NULL;
		LPDIRECT3DVOLUMETEXTURE9 pvoltexSrc = NULL;
		LPDIRECT3DVOLUMETEXTURE9 pvoltexDest = NULL;

		if( IsVolumeMap() )
		{
			pvoltexSrc = (LPDIRECT3DVOLUMETEXTURE9)ptexSrc;
			pvoltexDest = (LPDIRECT3DVOLUMETEXTURE9)ptexDest;
		}
		else if( IsCubeMap() )
		{
			pcubetexSrc = (LPDIRECT3DCUBETEXTURE9)ptexSrc;
			pcubetexDest = (LPDIRECT3DCUBETEXTURE9)ptexDest;
		}
		else
		{
			pmiptexSrc = (LPDIRECT3DTEXTURE9)ptexSrc;
			pmiptexDest = (LPDIRECT3DTEXTURE9)ptexDest;
		}

		for( DWORD iLevel=0; iLevel<m_dwBltLevel; iLevel++ )
		{
			if( IsVolumeMap() )
			{
				LPDIRECT3DVOLUME9 pvolSrc = NULL;
				LPDIRECT3DVOLUME9 pvolDest = NULL;

				hr = pvoltexSrc->GetVolumeLevel( iLevel, &pvolSrc );
				if( FAILED( hr ) )
					return hr;

				hr = pvoltexDest->GetVolumeLevel( iLevel, &pvolDest );
				if( FAILED( hr ) )
				{
					ReleasePpo( &pvolSrc );
					return hr;
				}

				hr = D3DXLoadVolumeFromVolume(
					pvolDest, NULL, NULL,
					pvolSrc, NULL, NULL,
					Filter, 0 );

				ReleasePpo( &pvolSrc );
				ReleasePpo( &pvolDest );

				if( FAILED( hr ) )
					return hr;
			}
			else if( IsCubeMap() )
			{
				LPDIRECT3DSURFACE9 psurfSrc = NULL;
				LPDIRECT3DSURFACE9 psurfDest = NULL;

				hr = pcubetexSrc->GetCubeMapSurface( FaceType, iLevel, &psurfSrc );
				if( FAILED( hr ) )
					return hr;

				hr = pcubetexDest->GetCubeMapSurface( FaceType, iLevel, &psurfDest );
				if( FAILED( hr ) )
				{
					ReleasePpo( &psurfSrc );
					return hr;
				}

				hr = D3DXLoadSurfaceFromSurface(
					psurfDest, NULL, NULL,
					psurfSrc, NULL, NULL,
					Filter, 0 );

				ReleasePpo( &psurfSrc );
				ReleasePpo( &psurfDest );

				if( FAILED( hr ) )
					return hr;
			}
			else
			{
				LPDIRECT3DSURFACE9 psurfSrc = NULL;
				LPDIRECT3DSURFACE9 psurfDest = NULL;

				hr = pmiptexSrc->GetSurfaceLevel( iLevel, &psurfSrc );
				if( FAILED( hr ) )
					return hr;

				hr = pmiptexDest->GetSurfaceLevel( iLevel, &psurfDest );
				if( FAILED( hr ) )
				{
					ReleasePpo( &psurfSrc );
					return hr;
				}

				hr = D3DXLoadSurfaceFromSurface(
					psurfDest, NULL, NULL, 
					psurfSrc, NULL, NULL,
					Filter, 0 );

				ReleasePpo( &psurfSrc );
				ReleasePpo( &psurfDest );

				if( FAILED( hr ) )
					return hr;
			}
		}

		return hr;
	}

	HRESULT ChangeFormat(
		LPDIRECT3DDEVICEQ pd3dDevice,
		LPDIRECT3DBASETEXTURE9 ptexCur,
		DWORD dwWidth,
		DWORD dwHeight,
		DWORD dwDepth,
		DWORD numMips,
		D3DFORMAT fmtTo, 
		LPDIRECT3DBASETEXTURE9* pptexNew,
		DWORD dwBltLevel )
	{
		m_dwWidth = dwWidth;
		m_dwHeight = dwHeight;
		m_dwDepth = dwDepth;
		m_numMips = numMips;
		m_dwBltLevel = dwBltLevel;

		if( m_numMips == 0 )
		{
			LONG lwTempW = dwWidth;
			LONG lwTempH = dwHeight;
			LONG lwPowsW = 0;
			LONG lwPowsH = 0;

			while( lwTempW > 0 )
			{
				lwPowsW++;
				lwTempW = lwTempW / 2;
			}
			while( lwTempH > 0 )
			{
				lwPowsH++;
				lwTempH = lwTempH / 2;
			}

			m_numMips = lwPowsW > lwPowsH ? lwPowsW : lwPowsH;
		}

		if( m_dwBltLevel == 0 || m_dwBltLevel > m_numMips )
			m_dwBltLevel = m_numMips;

		HRESULT hr = S_OK;
		LPDIRECT3DDEVICEQ pd3ddev = pd3dDevice;
		LPDIRECT3DTEXTURE9 pmiptex;
		LPDIRECT3DCUBETEXTURE9 pcubetex;
		LPDIRECT3DVOLUMETEXTURE9 pvoltex;
		D3DFORMAT fmtFrom;
		LPDIRECT3DTEXTURE9 pmiptexNew;
		LPDIRECT3DCUBETEXTURE9 pcubetexNew;
		LPDIRECT3DVOLUMETEXTURE9 pvoltexNew;

		if( IsVolumeMap() )
		{
			pvoltex = (LPDIRECT3DVOLUMETEXTURE9)ptexCur;
			D3DVOLUME_DESC vd;
			hr = pvoltex->GetLevelDesc( 0, &vd );
			if( FAILED( hr ) )
				return hr;
			fmtFrom = vd.Format;
		}
		else if( IsCubeMap() )
		{
			pcubetex = (LPDIRECT3DCUBETEXTURE9)ptexCur;
			D3DSURFACE_DESC sd;
			hr = pcubetex->GetLevelDesc( 0, &sd );
			if( FAILED( hr ) )
				return hr;
			fmtFrom = sd.Format;
		}
		else
		{
			pmiptex = (LPDIRECT3DTEXTURE9)ptexCur;
			D3DSURFACE_DESC sd;
			hr = pmiptex->GetLevelDesc( 0, &sd );
			if( FAILED( hr ) )
				return hr;
			fmtFrom = sd.Format;
		}

		if( fmtFrom == D3DFMT_DXT2 || fmtFrom == D3DFMT_DXT4 )
		{
			if( fmtTo == D3DFMT_DXT1 )
			{
				//AfxMessageBox( ID_ERROR_PREMULTTODXT1 );
			}
			else if( fmtTo != D3DFMT_DXT2 && fmtTo != D3DFMT_DXT4 )
			{
				//AfxMessageBox( ID_ERROR_PREMULTALPHA );
				return S_OK;
			}
		}

		if( IsVolumeMap() )
		{
			hr = pd3ddev->CreateVolumeTexture(
				m_dwWidth,
				m_dwHeight,
				m_dwDepth,
				m_numMips,
				0,
				fmtTo,
				D3DPOOL_SYSTEMMEM,
				&pvoltexNew,
				NULL );
			if( FAILED( hr ) )
				return hr;

			*pptexNew = pvoltexNew;

			if( FAILED( hr = BltAllLevels( D3DCUBEMAP_FACE_FORCE_DWORD, ptexCur, *pptexNew ) ) )
				return hr;
		}
		else if( IsCubeMap() )
		{
			hr = pd3ddev->CreateCubeTexture(
				m_dwWidth,
				m_numMips,
				0,
				fmtTo,
				D3DPOOL_MANAGED,
				&pcubetexNew,
				NULL );
			if( FAILED( hr ) )
				return hr;

			*pptexNew = pcubetexNew;

			if( FAILED( hr = BltAllLevels( D3DCUBEMAP_FACE_NEGATIVE_X, ptexCur, *pptexNew ) ) )
				return hr;
			if( FAILED( hr = BltAllLevels( D3DCUBEMAP_FACE_POSITIVE_X, ptexCur, *pptexNew ) ) )
				return hr;
			if( FAILED( hr = BltAllLevels( D3DCUBEMAP_FACE_NEGATIVE_Y, ptexCur, *pptexNew ) ) )
				return hr;
			if( FAILED( hr = BltAllLevels( D3DCUBEMAP_FACE_POSITIVE_Y, ptexCur, *pptexNew ) ) )
				return hr;
			if( FAILED( hr = BltAllLevels( D3DCUBEMAP_FACE_NEGATIVE_Z, ptexCur, *pptexNew ) ) )
				return hr;
			if( FAILED( hr = BltAllLevels( D3DCUBEMAP_FACE_POSITIVE_Z, ptexCur, *pptexNew ) ) )
				return hr;
		}
		else
		{
			if( ( fmtTo == D3DFMT_DXT1 || fmtTo == D3DFMT_DXT2 ||
				fmtTo == D3DFMT_DXT3 || fmtTo == D3DFMT_DXT4 ||
				fmtTo == D3DFMT_DXT5 ) && ( m_dwWidth % 4 != 0 || m_dwHeight % 4 != 0 ) )
			{
				//AfxMessageBox( ID_ERROR_NEEDMULTOF4 );
				return E_FAIL;
			}

			hr = pd3ddev->CreateTexture(
				m_dwWidth,
				m_dwHeight,
				m_numMips,
				0,
				fmtTo,
				D3DPOOL_MANAGED,
				&pmiptexNew,
				NULL );
			if( FAILED( hr ) )
				return hr;

			*pptexNew = pmiptexNew;

			if( FAILED( hr = BltAllLevels( D3DCUBEMAP_FACE_FORCE_DWORD, ptexCur, *pptexNew ) ) )
				return hr;
		}

		return S_OK;
	}

	HRESULT Compress(
		LPDIRECT3DDEVICEQ pd3dDevice,
		DWORD dwWidth,
		DWORD dwHeight,
		DWORD dwDepth,
		DWORD numMips,
		D3DFORMAT fmtTo, 
		LPDIRECT3DBASETEXTURE9 ptexOrig,
		LPDIRECT3DBASETEXTURE9& _ptexNew,
		DWORD dwBltLevel,
		BOOL bCubeMapFlags )
	{
		LPDIRECT3DBASETEXTURE9 ptexNew = NULL;

		g_bCubeMapFlags = bCubeMapFlags;

		HRESULT hr = ChangeFormat(
			pd3dDevice,
			ptexOrig, 
			dwWidth,
			dwHeight,
			dwDepth,
			numMips,
			fmtTo, 
			&ptexNew,
			dwBltLevel );
		if( FAILED( hr ) )
		{
			ReleasePpo( &ptexNew );
			return hr;
		}

		ReleasePpo( &_ptexNew );
		_ptexNew = ptexNew;

		return S_OK;
	}
}

namespace NSTexCompress
{
	HRESULT CopyRenderTargetToManaged(
		LPDIRECT3DDEVICEQ pd3dDevice,
		LPDIRECT3DSURFACEQ pRenderTarget,
		LPDIRECT3DTEXTUREQ& pTextureSystem,
		LPDIRECT3DTEXTUREQ& pTextureManaged )
	{
		D3DSURFACE_DESC sDesc;

		HRESULT hr = pRenderTarget->GetDesc( &sDesc );
		if( FAILED( hr ) )
			return hr;

		BOOL bCreateSystem = FALSE;

		if( !pTextureSystem )
		{
			hr = pd3dDevice->CreateTexture(
				sDesc.Width,
				sDesc.Height,
				1,
				0,
				sDesc.Format,
				D3DPOOL_SYSTEMMEM,
				&pTextureSystem,
				NULL );

			if( FAILED( hr ) )
				return hr;

			bCreateSystem = TRUE;
		}

		hr = GetRenderTargetData( pd3dDevice, pRenderTarget, pTextureSystem );
		if( FAILED( hr ) )
		{
			if( bCreateSystem )
				SAFE_RELEASE( pTextureSystem );

			return hr;
		}

		BOOL bCreateManaged = FALSE;

		if( !pTextureManaged )
		{
			hr = pd3dDevice->CreateTexture(
				sDesc.Width,
				sDesc.Height,
				0,
				0,
				sDesc.Format,
				D3DPOOL_MANAGED,
				&pTextureManaged,
				NULL );

			if( FAILED( hr ) )
			{
				if( bCreateSystem )
					SAFE_RELEASE( pTextureSystem );

				return hr;
			}

			bCreateManaged = TRUE;
		}

		hr = CopyTexture( (DWORD)sDesc.Width, (DWORD)sDesc.Height, sDesc.Format, pTextureSystem, pTextureManaged );
		if( FAILED( hr ) )
		{
			if( bCreateSystem )
				SAFE_RELEASE( pTextureSystem );

			if( bCreateManaged )
				SAFE_RELEASE( pTextureManaged );

			return hr;
		}

		if( bCreateSystem )
			SAFE_RELEASE( pTextureSystem );

		return S_OK;
	}

	HRESULT GetRenderTargetData(
		LPDIRECT3DDEVICEQ pd3dDevice,
		LPDIRECT3DSURFACEQ pRenderTarget,
		LPDIRECT3DTEXTUREQ pTextureSystem )
	{
		LPDIRECT3DSURFACEQ pSurfaceSystem = NULL;

		HRESULT hr = pTextureSystem->GetSurfaceLevel( 0, &pSurfaceSystem );
		if( FAILED( hr ) )
			return hr;

		hr = pd3dDevice->GetRenderTargetData( pRenderTarget, pSurfaceSystem );
		SAFE_RELEASE( pSurfaceSystem );

		return hr;
	}

	HRESULT CopyTexture(
		DWORD dwWidth,
		DWORD dwHeight,
		D3DFORMAT emFormat,
		LPDIRECT3DTEXTUREQ pTextureSystem,
		LPDIRECT3DTEXTUREQ pTextureManaged )
	{
		UINT uiTextureByte = 0;

		switch( emFormat )
		{
		case D3DFMT_R5G6B5:
		case D3DFMT_X1R5G5B5:
		case D3DFMT_A1R5G5B5:
		case D3DFMT_A4R4G4B4:
		case D3DFMT_X4R4G4B4:
			uiTextureByte = 2;
			break;

		case D3DFMT_A8R8G8B8:
		case D3DFMT_X8R8G8B8:
		case D3DFMT_A8B8G8R8:
		case D3DFMT_X8B8G8R8:
			uiTextureByte = 4;
			break;
		}

		if( uiTextureByte <= 0 )
			return E_FAIL;

		D3DLOCKED_RECT sLockSystem;

		HRESULT hr = pTextureSystem->LockRect( 0, &sLockSystem, NULL, D3DLOCK_READONLY );
		if( FAILED( hr ) )
		{
			CDebugSet::ToLogFile( std::string("pTextureSystem->LockRect( 0, &sLockSystem, NULL, D3DLOCK_READONLY )") );
			return hr;
		}

		D3DLOCKED_RECT sLockManaged;

		hr = pTextureManaged->LockRect( 0, &sLockManaged, NULL, 0 );
		if( FAILED( hr ) )
		{
			CDebugSet::ToLogFile( std::string("pTextureManaged->LockRect( 0, &sLockManaged, NULL, 0 )") );
			pTextureSystem->UnlockRect( 0 );
			return hr;
		}

		memcpy( sLockManaged.pBits, sLockSystem.pBits, dwWidth * dwHeight * uiTextureByte );

		pTextureManaged->UnlockRect( 0 );
		pTextureSystem->UnlockRect( 0 );

		return S_OK;
	}
}

namespace NSTexCompress
{
	BYTE* globalOutData = NULL;

	VOID EmitWord( WORD s )
	{
		globalOutData[ 0 ] = BYTE( ( s >> 0 ) & 255 );
		globalOutData[ 1 ] = BYTE( ( s >> 8 ) & 255 );
		globalOutData += 2;
	}

	VOID EmitDoubleWord( DWORD i )
	{
		globalOutData[ 0 ] = BYTE( ( i >> 0 ) & 255 );
		globalOutData[ 1 ] = BYTE( ( i >> 8 ) & 255 );
		globalOutData[ 2 ] = BYTE( ( i >> 16 ) & 255 );
		globalOutData[ 3 ] = BYTE( ( i >> 24 ) & 255 );
		globalOutData += 4;
	}

	WORD ColorTo565( const BYTE* color )
	{
		return ( ( color[ 0 ] >> 3 ) << 11 ) | ( ( color[ 1 ] >> 2 ) << 5 ) | ( color[ 2 ] >> 3 );
	}

	WORD ColorTo1555( const BYTE* color )
	{
		BOOL bAlphaON(FALSE);
		if( color[0] >= 240 )	
			bAlphaON = TRUE;

		return ( ( bAlphaON ? 1<<15 : 0 ) | ( color[ 1 ] >> 3 ) << 10 ) | ( ( color[ 2 ] >> 3 ) << 5 ) | ( color[ 3 ] >> 3 );
	}

	VOID ExtractBlock( const BYTE* inPtr, INT width, BYTE* colorBlock )
	{
		for( INT j=0; j<4; j++ )
		{
			memcpy( &colorBlock[ j*4*4 ], inPtr, 4 * 4 );
			inPtr += width * 4;
		}
	}

	#define INSET_SHIFT 4 // inset the bounding box with ( range >> shift )
	VOID GetMinMaxColors( const BYTE* colorBlock, BYTE* minColor, BYTE* maxColor )
	{
		INT i;
		BYTE inset[ 3 ];

		minColor[ 0 ] = minColor[ 1 ] = minColor[ 2 ] = 255;
		maxColor[ 0 ] = maxColor[ 1 ] = maxColor[ 2 ] = 0;

		for( i=0; i<16; i++ )
		{
			if( colorBlock[ i*4+0 ] < minColor[ 0 ] ) { minColor[ 0 ] = colorBlock[ i*4+0 ]; }
			if( colorBlock[ i*4+1 ] < minColor[ 1 ] ) { minColor[ 1 ] = colorBlock[ i*4+1 ]; }
			if( colorBlock[ i*4+2 ] < minColor[ 2 ] ) { minColor[ 2 ] = colorBlock[ i*4+2 ]; }
			if( colorBlock[ i*4+0 ] > maxColor[ 0 ] ) { maxColor[ 0 ] = colorBlock[ i*4+0 ]; }
			if( colorBlock[ i*4+1 ] > maxColor[ 1 ] ) { maxColor[ 1 ] = colorBlock[ i*4+1 ]; }
			if( colorBlock[ i*4+2 ] > maxColor[ 2 ] ) { maxColor[ 2 ] = colorBlock[ i*4+2 ]; }
		}

		inset[ 0 ] = ( maxColor[ 0 ] - minColor[ 0 ] ) >> INSET_SHIFT;
		inset[ 1 ] = ( maxColor[ 1 ] - minColor[ 1 ] ) >> INSET_SHIFT;
		inset[ 2 ] = ( maxColor[ 2 ] - minColor[ 2 ] ) >> INSET_SHIFT;

		minColor[ 0 ] = ( minColor[ 0 ] + inset[ 0 ] <= 255 ) ? minColor[ 0 ] + inset[ 0 ] : 255;
		minColor[ 1 ] = ( minColor[ 1 ] + inset[ 1 ] <= 255 ) ? minColor[ 1 ] + inset[ 1 ] : 255;
		minColor[ 2 ] = ( minColor[ 2 ] + inset[ 2 ] <= 255 ) ? minColor[ 2 ] + inset[ 2 ] : 255;

		maxColor[ 0 ] = ( maxColor[ 0 ] >= inset[ 0 ] ) ? maxColor[ 0 ] - inset[ 0 ] : 0;
		maxColor[ 1 ] = ( maxColor[ 1 ] >= inset[ 1 ] ) ? maxColor[ 1 ] - inset[ 1 ] : 0;
		maxColor[ 2 ] = ( maxColor[ 2 ] >= inset[ 2 ] ) ? maxColor[ 2 ] - inset[ 2 ] : 0;
	}

	#define C565_5_MASK 0xF8 // 0xFF minus last three bits
	#define C565_6_MASK 0xFC // 0xFF minus last two bits
	VOID EmitColorIndices( const BYTE* colorBlock, const BYTE* minColor, const BYTE* maxColor )
	{
		WORD colors[ 4 ][ 4 ];
		DWORD result = 0;

		colors[ 0 ][ 0 ] = ( maxColor[ 0 ] & C565_5_MASK ) | ( maxColor[ 0 ] >> 5 );
		colors[ 0 ][ 1 ] = ( maxColor[ 1 ] & C565_6_MASK ) | ( maxColor[ 1 ] >> 6 );
		colors[ 0 ][ 2 ] = ( maxColor[ 2 ] & C565_5_MASK ) | ( maxColor[ 2 ] >> 5 );
		colors[ 1 ][ 0 ] = ( minColor[ 0 ] & C565_5_MASK ) | ( minColor[ 0 ] >> 5 );
		colors[ 1 ][ 1 ] = ( minColor[ 1 ] & C565_6_MASK ) | ( minColor[ 1 ] >> 6 );
		colors[ 1 ][ 2 ] = ( minColor[ 2 ] & C565_5_MASK ) | ( minColor[ 2 ] >> 5 );
		colors[ 2 ][ 0 ] = ( 2 * colors[ 0 ][ 0 ] + 1 * colors[ 1 ][ 0 ] ) / 3;
		colors[ 2 ][ 1 ] = ( 2 * colors[ 0 ][ 1 ] + 1 * colors[ 1 ][ 1 ] ) / 3;
		colors[ 2 ][ 2 ] = ( 2 * colors[ 0 ][ 2 ] + 1 * colors[ 1 ][ 2 ] ) / 3;
		colors[ 3 ][ 0 ] = ( 1 * colors[ 0 ][ 0 ] + 2 * colors[ 1 ][ 0 ] ) / 3;
		colors[ 3 ][ 1 ] = ( 1 * colors[ 0 ][ 1 ] + 2 * colors[ 1 ][ 1 ] ) / 3;
		colors[ 3 ][ 2 ] = ( 1 * colors[ 0 ][ 2 ] + 2 * colors[ 1 ][ 2 ] ) / 3;

		for( INT i=15; i>=0; i-- )
		{
			INT c0 = colorBlock[ i*4+0 ];
			INT c1 = colorBlock[ i*4+1 ];
			INT c2 = colorBlock[ i*4+2 ];

			INT d0 = abs( colors[ 0 ][ 0 ] - c0 ) + abs( colors[ 0 ][ 1 ] - c1 ) + abs( colors[ 0 ][ 2 ] - c2 );
			INT d1 = abs( colors[ 1 ][ 0 ] - c0 ) + abs( colors[ 1 ][ 1 ] - c1 ) + abs( colors[ 1 ][ 2 ] - c2 );
			INT d2 = abs( colors[ 2 ][ 0 ] - c0 ) + abs( colors[ 2 ][ 1 ] - c1 ) + abs( colors[ 2 ][ 2 ] - c2 );
			INT d3 = abs( colors[ 3 ][ 0 ] - c0 ) + abs( colors[ 3 ][ 1 ] - c1 ) + abs( colors[ 3 ][ 2 ] - c2 );

			INT b0 = d0 > d3;
			INT b1 = d1 > d2;
			INT b2 = d0 > d2;
			INT b3 = d1 > d3;
			INT b4 = d2 > d3;

			INT x0 = b1 & b2;
			INT x1 = b0 & b3;
			INT x2 = b0 & b4;

			result |= ( x2 | ( ( x0 | x1 ) << 1 ) ) << ( i << 1 );
		}

		EmitDoubleWord( result );
	}

	#define DXT_ALIGN16 __declspec(align(16))
	typedef DXT_ALIGN16 BYTE BYTEA16;
	VOID CompressImageDXT1( const BYTE* inBuf, BYTE* outBuf, INT width, INT height )
	{
		BYTEA16 block[ 64 ];
		BYTEA16 minColor[ 4 ];
		BYTEA16 maxColor[ 4 ];

		globalOutData = outBuf;

		for( INT j=0; j<height; j+=4, inBuf+=width*4*4 )
		{
			for( INT i=0; i<width; i+=4 )
			{
				ExtractBlock( inBuf + i * 4, width, block );

				GetMinMaxColors( block, minColor, maxColor );

				EmitWord( ColorTo565( maxColor ) );
				EmitWord( ColorTo565( minColor ) );

				EmitColorIndices( block, minColor, maxColor );
			}
		}
	}

	HRESULT CompressDXT1(
		DWORD dwWidth,
		DWORD dwHeight,
		LPDIRECT3DTEXTUREQ pTextureRGBA,
		LPDIRECT3DTEXTUREQ pTextureDXT1 )
	{
		D3DLOCKED_RECT sLockRGBA;

		HRESULT hr = pTextureRGBA->LockRect( 0, &sLockRGBA, NULL, 0 );
		if( FAILED( hr ) )
			return hr;

		D3DLOCKED_RECT sLockDXT1;

		hr = pTextureDXT1->LockRect( 0, &sLockDXT1, NULL, 0 );
		if( FAILED( hr ) )
		{
			pTextureRGBA->UnlockRect( 0 );
			return hr;
		}

		CompressImageDXT1(
			(const BYTE*)sLockRGBA.pBits,
			(BYTE*)sLockDXT1.pBits,
			(INT)dwWidth,
			(INT)dwHeight );

		pTextureDXT1->UnlockRect( 0 );
		pTextureRGBA->UnlockRect( 0 );

		return S_OK;
	}
}
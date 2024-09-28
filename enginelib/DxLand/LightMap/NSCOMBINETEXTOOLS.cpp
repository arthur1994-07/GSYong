#include "pch.h"

#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/TextureManager.h"

#include "NSCOMBINETEXTOOLS.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSCOMBINETEXTOOLS
{
	void UpdateTexelEnable( WORDWORD* pTexelEnable, 
							bool abTexelState[], 
							DWORD& dwTexelUseNUM, 
							DWORD dwTexWidth, 
							DWORD dwTexHeight )
	{
		int nCount(0);
		for( WORD y=0; y<dwTexHeight; ++y )
		{
			for( WORD x=0; x<dwTexWidth; ++x )
			{
				if( abTexelState[ dwTexWidth*y + x ] )
				{
					pTexelEnable[nCount++] = WORDWORD( x, y );
				}
			}
		}

		dwTexelUseNUM = nCount;
	}

	BOOL GetTexelAndTexelMarking( WORDWORD& sTexelXY,
									int nIndexX, 
									int nIndexY, 
									bool abTexelState[], 
									WORDWORD* pTexelEnable,
									WORD* pTexelLast,
									DWORD dwTexelUseNUM, 
									DWORD dwTexWidth, 
									DWORD dwTexHeight )
	{
		BOOL bSucess(TRUE);
		for( DWORD i=0; i<dwTexelUseNUM; ++i )
		{
			bSucess = TRUE;

			// Note : 각 점들을 검사하여 그려도 된다면 마킹하고 위치를 넘겨준다.
			WORD wStartX = pTexelEnable[i].wA;
			WORD wEndX = pTexelEnable[i].wA + nIndexX-1;

			WORD wStartY = pTexelEnable[i].wB;
			WORD wEndY = pTexelEnable[i].wB + nIndexY-1;

			if( wEndX >= dwTexWidth || wEndY >= dwTexHeight )
			{
				// Note : 범위를 넘어설 경우 그리지 않는다.
				bSucess = FALSE;
			}
			else
			{
				if( !abTexelState[i] )
				{
					// 그려진 부위는 건너띄는 작업을 한다.
					int nCurX = i%dwTexWidth;
					i += pTexelLast[i] - nCurX;
					bSucess = FALSE;
				}
				else
				{
					for( WORD y=wStartY; y<=wEndY; ++y )
					{
						for( WORD x=wStartX; x<=wEndX; ++x )
						{
							if( !abTexelState[y*dwTexWidth+x] )
							{
                                // 이전 라인
                                WORD wPrevY = pTexelEnable[i].wB;

                                // 계산을 빠르게 하기 위함.
                                i += nIndexX;

                                // 현재 라인
                                WORD wCurY = pTexelEnable[i].wB;

                                if ( (wPrevY != wCurY) )
                                {
                                    i = dwTexWidth * (wPrevY+1) - 1;
                                }

								// 값셋팅.
								x = wEndX+1;
								y = wEndY+1;
								bSucess = FALSE;
							}
						}
					}
				}
			}

			// 성공.~!!
			if( bSucess )
			{
				// Note : 각 점을 마킹한다.
				WORD wStartX = pTexelEnable[i].wA;
				WORD wEndX = pTexelEnable[i].wA + nIndexX-1;

				WORD wStartY = pTexelEnable[i].wB;
				WORD wEndY = pTexelEnable[i].wB + nIndexY-1;

                DWORD dwIndex(0);
				for( WORD y=wStartY; y<=wEndY; ++y )
				{
					for( WORD x=wStartX; x<=wEndX; ++x )
					{
                        dwIndex = y*dwTexWidth+x;
						abTexelState[dwIndex] = false;
						pTexelLast[dwIndex] = wEndX;
					}
				}

				// Note : 리턴 값 셋팅.
				sTexelXY.dwData = pTexelEnable[i].dwData;

				return TRUE;
			}
		}

		return FALSE;
	}

    void SetFitHeight( bool* pbTexelState, 
						DWORD dwTexWidth, 
						DWORD& dwTexHeight )
	{
		DWORD dwTexHeightNEW(dwTexHeight);
		for( DWORD j=0; j<dwTexHeight; ++j )
		{
			BOOL bLineEnable(TRUE);

			// Note : 확인작업
			for( DWORD i=0; i<dwTexWidth; ++i )
			{
				if( pbTexelState[j*dwTexWidth+i] == false )
				{
					bLineEnable = FALSE;
					break;
				}
			}

			// Note : 그림이 그려지지 않았다.. 그러므로 텍스쳐를 줄일 여지가 있다.
			if( bLineEnable )
			{
				if( j <= 32 )			dwTexHeightNEW = 32;
				else if( j <= 64 )		dwTexHeightNEW = 64;
				else if( j <= 128 )		dwTexHeightNEW = 128;
				else if( j <= 256 )		dwTexHeightNEW = 256;
				else if( j <= 512 )		dwTexHeightNEW = 512;
				else if( j <= 1024 )	dwTexHeightNEW = 1024;
				else if( j <= 2048 )	dwTexHeightNEW = 2048;

				break;
			}
		}

        dwTexHeight = dwTexHeightNEW;
	}

	void SetFitHeightOfTexture( LPDIRECT3DDEVICEQ pd3dDevice, 
								bool* pbTexelState, 
								DWORD dwTexWidth, 
								DWORD& dwTexHeight, 
								IDirect3DTexture9** ppTextureDAY,
								IDirect3DTexture9** ppTextureDAY_NEW )
	{
		DWORD dwTexHeightNEW(dwTexHeight);
		for( DWORD j=0; j<dwTexHeight; ++j )
		{
			BOOL bLineEnable(TRUE);

			// Note : 확인작업
			for( DWORD i=0; i<dwTexWidth; ++i )
			{
				if( pbTexelState[j*dwTexWidth+i] == false )
				{
					bLineEnable = FALSE;
					break;
				}
			}

			// Note : 그림이 그려지지 않았다.. 그러므로 텍스쳐를 줄일 여지가 있다.
			if( bLineEnable )
			{
				if( j <= 32 )			dwTexHeightNEW = 32;
				else if( j <= 64 )		dwTexHeightNEW = 64;
				else if( j <= 128 )		dwTexHeightNEW = 128;
				else if( j <= 256 )		dwTexHeightNEW = 256;
				else if( j <= 512 )		dwTexHeightNEW = 512;
				else if( j <= 1024 )	dwTexHeightNEW = 1024;
				else if( j <= 2048 )	dwTexHeightNEW = 2048;

				break;
			}
		}

		if( dwTexHeight == dwTexHeightNEW )
		{
			(*ppTextureDAY_NEW) = (*ppTextureDAY);
			(*ppTextureDAY) = NULL;
		}
		else	// 변환된 사이즈 텍스쳐에 그림을 그려 놓는다.
		{
			dwTexHeight = dwTexHeightNEW;

			UINT uiWidth( dwTexWidth );
			UINT uiHeight( dwTexHeight );
			D3DFORMAT emFormat( D3DFMT_A8R8G8B8 );

            D3DXCreateTexture(
                pd3dDevice,
                uiWidth,
                uiHeight,
                1,
                0L,
                emFormat,
                D3DPOOL_SYSTEMMEM,
                &(*ppTextureDAY_NEW) );

			// Note : 낮용 수정.
			D3DLOCKED_RECT stLocked;
			DWORD* pSrc(NULL);
			DWORD* pDest(NULL);

			if( (*ppTextureDAY) )
			{
				HRESULT hr = (*ppTextureDAY)->LockRect( 0, &stLocked, NULL, 0 );
				if( FAILED(hr) )
				{
					GASSERT( 0 && _T("HRESULT hr = pTexture->LockRect( 0, &stLocked, NULL, 0 );") );
					return;
				}
				pSrc = (DWORD*)stLocked.pBits;

				hr = (*ppTextureDAY_NEW)->LockRect( 0, &stLocked, NULL, 0 );
				if( FAILED(hr) )
				{
					GASSERT( 0 && _T("HRESULT hr = pTexture->LockRect( 0, &stLocked, NULL, 0 );") );
					(*ppTextureDAY)->UnlockRect( 0 );
					return;
				}
				pDest = (DWORD*)stLocked.pBits;

				INT iPitch = stLocked.Pitch / 4;

				for( DWORD y=0; y<dwTexHeight; ++y )
				{
					for( DWORD x=0; x<dwTexWidth; ++x )
					{
						pDest[ x + y * iPitch ] = pSrc[ x + y * iPitch ];
					}
				}

				(*ppTextureDAY_NEW)->UnlockRect( 0 );
				(*ppTextureDAY)->UnlockRect( 0 );
			}
		}
	}

	void DrawTexel( DWORD* pTexData, 
					int iPitch, 
					DWORD* pColor,
					DWORD dwTexWidth, 
					WORDWORD sTexelXY,
					int nSizeX, 
					int nSizeY,
					int OffsetX, 
					int OffsetY )
	{
		int xx, yy;
		for( int y=0; y<nSizeY+OffsetX*2; ++y )
		{
			for( int x=0; x<nSizeX+OffsetY*2; ++x )
			{
				xx = x - OffsetX;
				yy = y - OffsetY;
				if( xx < 0 )					xx = 0;
				if( yy < 0 )					yy = 0;
				if( xx > (nSizeX-OffsetX) )		xx = (nSizeX-OffsetX);
				if( yy > (nSizeY-OffsetY) )		yy = (nSizeY-OffsetY);

				pTexData[ x + sTexelXY.wA + (y+sTexelXY.wB) * iPitch ] = pColor[yy*nSizeX+xx];
			}
		}
	}

	void DrawTexel_Tex2Array( DWORD* pTexData, 
								int iPitch, 
								DWORD* pColor,
								WORDWORD sTexelXY,
								int nSizeX, 
								int nSizeY,
								int OffsetX, 
								int OffsetY )
	{
		int xx, yy;
		for( int y=0; y<nSizeY+OffsetX*2; ++y )
		{
			for( int x=0; x<nSizeX+OffsetY*2; ++x )
			{
				xx = x - OffsetX;
				yy = y - OffsetY;
				if( xx < 0 )					xx = 0;
				if( yy < 0 )					yy = 0;
				if( xx > (nSizeX-OffsetX) )		xx = (nSizeX-OffsetX);
				if( yy > (nSizeY-OffsetY) )		yy = (nSizeY-OffsetY);

				pTexData[ x + sTexelXY.wA + (y+sTexelXY.wB) * iPitch ] = pColor[yy*nSizeX+xx];
			}
		}
	}

	void DrawTexel_Tex2Array( DWORD* pTexData, 
								int iPitch, 
								DWORD* pColor,
								WORDWORD sTexelXY,
								int nSizeX, 
								int nSizeY,
								int OffsetX, 
								int OffsetY,
								DWORD dwBitShading,
								DWORD dwOffsetShading,
								DWORD dwBitShadow,
								DWORD dwOffsetShadow )
	{
		int xx, yy;
		for( int y=0; y<nSizeY+OffsetX*2; ++y )
		{
			for( int x=0; x<nSizeX+OffsetY*2; ++x )
			{
				xx = x - OffsetX;
				yy = y - OffsetY;
				if( xx < 0 )					xx = 0;
				if( yy < 0 )					yy = 0;
				if( xx > (nSizeX-OffsetX) )		xx = (nSizeX-OffsetX);
				if( yy > (nSizeY-OffsetY) )		yy = (nSizeY-OffsetY);

				DWORD dwColor = pColor[yy*nSizeX+xx];
				DWORD dwR = (dwColor&dwBitShading) >> dwOffsetShading;
				DWORD dwB = (dwColor&dwBitShadow) >> dwOffsetShadow;
				DWORD dwColorNew = 0xff000000 + (dwR<<16) + dwB;

				pTexData[ x + sTexelXY.wA + (y+sTexelXY.wB) * iPitch ] = dwColorNew;
			}
		}
	}

	void DrawTexel_Tex2Tex( DWORD* pTexData, 
							int iPitch, 
							DWORD* pTexDataSrc,
							int iPitchSrc, 
							WORDWORD sTexelXY,
							int nSizeX, 
							int nSizeY,
							int OffsetX, 
							int OffsetY )
	{
		DWORD dwColor(0L);
		int xx, yy;
		for( int y=0; y<nSizeY+OffsetX*2; ++y )
		{
			for( int x=0; x<nSizeX+OffsetY*2; ++x )
			{
				xx = x - OffsetX;
				yy = y - OffsetY;
				if( xx < 0 )					xx = 0;
				if( yy < 0 )					yy = 0;
				if( xx > (nSizeX-OffsetX) )		xx = (nSizeX-OffsetX);
				if( yy > (nSizeY-OffsetY) )		yy = (nSizeY-OffsetY);

				dwColor = pTexDataSrc[ xx + yy * iPitchSrc ];
				pTexData[ x + sTexelXY.wA + (y+sTexelXY.wB) * iPitch ] = dwColor;
			}
		}
	}

    void DrawTexel_CheckTile( DWORD* pTexData, 
					        int iPitch,
					        int nSizeX, 
					        int nSizeY )
    {
        BOOL bBlackColor_Start(FALSE);
        BOOL bBlackColor(FALSE);
		for( int y=0; y<nSizeY; ++y )
		{
            bBlackColor = bBlackColor_Start;
            bBlackColor_Start = !bBlackColor_Start;

			for( int x=0; x<nSizeX; ++x )
			{
                if ( bBlackColor )
                {
				    pTexData[ x + y * iPitch ] = 0x00000000;
                }
                else
                {
				    pTexData[ x + y * iPitch ] = 0xffffffff;
                }

                bBlackColor = !bBlackColor;
			}
		}
    }

	void DrawTexel_Dot( DWORD* pTexData, 
							int iPitch,
							int nSizeX, 
							int nSizeY )
	{
		for( int y=0; y<nSizeY; ++y )
		{
			for( int x=0; x<nSizeX; ++x )
			{
				float fX = (static_cast<float>(x) / (static_cast<float>(nSizeX-1)*0.5f)) - 1.f;
				float fY = (static_cast<float>(y) / (static_cast<float>(nSizeY-1)*0.5f)) - 1.f;

				float fpow2XY = fX*fX + fY*fY;
				if ( fpow2XY >= 1.f )
				{
					pTexData[ x + y * iPitch ] = 0xff000000;
				}
				else
				{
					float fZ = 1.f - fpow2XY;
					DWORD dwDot = static_cast<DWORD>( fZ * 255.f );
					if ( dwDot > 255 )
						dwDot = 255;

					pTexData[ x + y * iPitch ] = 0xff000000 + (dwDot<<16) + (dwDot<<8) + dwDot;
				}
			}
		}
	}
}
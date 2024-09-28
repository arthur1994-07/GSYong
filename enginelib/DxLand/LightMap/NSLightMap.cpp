#include "pch.h"

#include <psapi.h>

#include "../../DxEffect/Mesh/DxStaticMeshCollBase.h"
#include "../../DxTools/Texture/NSTexCompress.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/DxMethods.h"
#include "../DxPiece/DxPieceManagerPSF.h"
#include "../DxLandMan.h"                   // ExportProgress 지우면 삭제하자.
#include "./NSCOMBINETEXTOOLS.h"
#include "./NSLightMapTools.h"
#include "./LightMapObject.h"

#include "NSLightMap.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//				LightMap 어떻게 작업되어있는지 기록해 놓는다.
//-----------------------------------------------------------------------------
//
// Piece User UV ( Detail - Mesh ) ( Cast, Receive - DxSingleTexMesh )
//  1. 각 Mesh 마다 UV 값을 얻는다. ( Detail도 들어감 )
//  2. UV 값을 0~1로 만든다.
//  3. DxSingleTexMesh 로 분리하는데, 그때 계산된 UV를 적용시킨다.
//  4. DxSingleTexMesh 를 만든다.
//
//
// Piece UV 1-Tex ( Detail, Cast, Receive - DxSingleTexMesh )
//  1. PIE 로 만들어져 있다.
//  2. DxSingleTexMesh 를 가지고 작업이 들어간다. 
//  3. UV를 계산한다. ( Detail 이 여기서 들어간다 )
//  4. DxSingleTexMesh 에 계산된 UV를 적용시킨다.
//
//
// Octree ( Detail - Mesh ) ( Cast, Receive - DxSingleTexMesh )
//  1. 각 Mesh 마다 정보를 구한다. ( Detail도 들어감 )
//  2. Mesh에 UV 값을 기록한다.
//  3. DxSingleTexMesh 로 분리하는데, 그때 계산된 UV를 적용시킨다.
//  4. DxSingleTexMesh 를 만든다.
//
//-----------------------------------------------------------------------------

namespace NSLightMap
{
    class TextureData
    {
    public:
        BOOL        m_bMakeTexture;
        DWORD       m_dwTexWidth;
        DWORD       m_dwTexHeight;
        DWORD       m_dwTexelUseNUM;
        DWORD       m_dwTexelEmptyNum;      // 비어있는 슬롯 갯수
	    bool*       m_pbTexelState;
	    WORD*       m_pTexelLast;
        DWORD*      m_pTexelLastDay;			// import 는 필요없다. (A) Direct Light + Indirect Light + Shadow
		DWORD*      m_pTexelLastDirect_Shadow;  // import 는 필요없다. (DS1,DS2) Direct Light 직접광(A,B), Shadow 그림자(G,R)
        DWORD*      m_pTexelLastIndirect;		// import 는 필요없다. (I) Indirect Light 간접광 ( RAD 로 생성되어짐. )

        std::vector<VEC_LIGHTMAPOBJ*> m_vecvecLightMapObject;

    public:
        void Import( const TextureData& sSrc )
        {
            CleanUp();

            m_dwTexWidth        = sSrc.m_dwTexWidth;
            m_dwTexHeight       = sSrc.m_dwTexHeight;
            m_dwTexelUseNUM     = sSrc.m_dwTexelUseNUM;
            m_dwTexelEmptyNum   = sSrc.m_dwTexelEmptyNum;

	        m_pbTexelState	        = new bool[m_dwTexelUseNUM];
	        m_pTexelLast		    = new WORD[m_dwTexelUseNUM];

            memcpy( m_pbTexelState, sSrc.m_pbTexelState, sizeof(bool)*m_dwTexelUseNUM );
            memcpy( m_pTexelLast, sSrc.m_pTexelLast, sizeof(WORD)*m_dwTexelUseNUM );

            m_vecvecLightMapObject = sSrc.m_vecvecLightMapObject;
        }

		void ImportALL( const TextureData& sSrc )
		{
			Import( sSrc );

			m_pTexelLastDay				= new DWORD[m_dwTexelUseNUM];
			m_pTexelLastDirect_Shadow	= new DWORD[m_dwTexelUseNUM];
			m_pTexelLastIndirect		= new DWORD[m_dwTexelUseNUM];

			memcpy( m_pTexelLastDay,			sSrc.m_pTexelLastDay,			sizeof(DWORD)*m_dwTexelUseNUM );
			memcpy( m_pTexelLastDirect_Shadow,	sSrc.m_pTexelLastDirect_Shadow,	sizeof(DWORD)*m_dwTexelUseNUM );
			memcpy( m_pTexelLastIndirect,		sSrc.m_pTexelLastIndirect,		sizeof(DWORD)*m_dwTexelUseNUM );
		}

    public:
        TextureData( DWORD dwTexWidth, DWORD dwTexHeight )
            : m_bMakeTexture(FALSE)
            , m_dwTexWidth(dwTexWidth)
            , m_dwTexHeight(dwTexHeight)
            , m_pTexelLastDay(NULL)
            , m_pTexelLastDirect_Shadow(NULL)
			, m_pTexelLastIndirect(NULL)
            , m_dwTexelEmptyNum(dwTexWidth*dwTexHeight)
        {
            m_dwTexelUseNUM         = dwTexWidth*dwTexHeight;
	        m_pbTexelState	        = new bool[m_dwTexelUseNUM];
	        m_pTexelLast		    = new WORD[m_dwTexelUseNUM];

            int nCount(0);
            for( DWORD j=0; j<dwTexHeight; ++j )
	        {
		        for( DWORD i=0; i<dwTexWidth; ++i )
		        {
			        m_pbTexelState[nCount] = true;
			        m_pTexelLast[nCount] = 0;
                    ++nCount;
		        }
	        }
        }

        ~TextureData()
        {
            CleanUp();
        }

        void CleanUp()
        {
            m_vecvecLightMapObject.clear();
	        SAFE_DELETE_ARRAY( m_pbTexelState );
	        SAFE_DELETE_ARRAY( m_pTexelLast );
            SAFE_DELETE_ARRAY( m_pTexelLastDay );
            SAFE_DELETE_ARRAY( m_pTexelLastDirect_Shadow );
			SAFE_DELETE_ARRAY( m_pTexelLastIndirect );
        }

        void CreateTexelColor()
        {
            // 텍스쳐를 다 만들었다면 재 생성하지 않는다.
            if ( m_bMakeTexture )
                return;

            if ( m_pTexelLastDay || m_pTexelLastDirect_Shadow || m_pTexelLastIndirect )
                return;

            SAFE_DELETE_ARRAY( m_pTexelLastDay );
            SAFE_DELETE_ARRAY( m_pTexelLastDirect_Shadow );
			SAFE_DELETE_ARRAY( m_pTexelLastIndirect );

            m_pTexelLastDay				= new DWORD[m_dwTexelUseNUM];
            m_pTexelLastDirect_Shadow	= new DWORD[m_dwTexelUseNUM];
			m_pTexelLastIndirect		= new DWORD[m_dwTexelUseNUM];

            memset( m_pTexelLastDay,			0, sizeof(DWORD)*m_dwTexelUseNUM );
            memset( m_pTexelLastDirect_Shadow,	0, sizeof(DWORD)*m_dwTexelUseNUM );
			memset( m_pTexelLastIndirect,		0, sizeof(DWORD)*m_dwTexelUseNUM );
        }
    };
};

namespace NSLightMap
{
    TSTRING g_strPath(_T(""));
    typedef std::vector<int> VEC_FACE_ID;
    std::vector<std::tr1::shared_ptr<TextureData>>  m_vecTextureData;
	std::vector<std::tr1::shared_ptr<TextureData>>  m_vecTextureData_BackUp;
    LPDIRECT3DDEVICEQ g_pd3dDevice;
    DWORD g_dwBaseWidth(512);

	std::map<DWORD,std::vector<std::tr1::shared_ptr<TextureData>>>	g_mapTextureData_BackUP;

	// 백업을 사용하기 위한 초기화
	void InitTextureDataBackup()
	{
		g_mapTextureData_BackUP.clear();
	}

	// 사용되고 있는 것을 백업 하도록 한다.
	// g_mapTextureData_BackUP <- m_vecTextureData 
	void SetTextureDataBackupIntoUsed( DWORD dwIndex )
	{
		std::vector<std::tr1::shared_ptr<TextureData>>  vecTextureData;
		for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
		{
			std::tr1::shared_ptr<TextureData> spNew = std::tr1::shared_ptr<TextureData>(new TextureData(1,1));
			spNew->ImportALL( *(m_vecTextureData[i].get()) );
			vecTextureData.push_back( spNew );
		}
		g_mapTextureData_BackUP.insert( std::make_pair( dwIndex, vecTextureData ) );
	}

	// 백업된 것을 사용하도록 한다.
	// m_vecTextureData <- g_mapTextureData_BackUP
	void SetTextureDataUsedIntoBackup( DWORD dwIndex )
	{
		m_vecTextureData.clear();

		std::map<DWORD,std::vector<std::tr1::shared_ptr<TextureData>>>::iterator iter = g_mapTextureData_BackUP.find( dwIndex );
		if ( iter != g_mapTextureData_BackUP.end() )
		{
			for ( DWORD i=0; i<(*iter).second.size(); ++i )
			{
				std::tr1::shared_ptr<TextureData> spNew = std::tr1::shared_ptr<TextureData>(new TextureData(1,1));
				spNew->ImportALL( *((*iter).second[i].get()) );
				m_vecTextureData.push_back( spNew );
			}
		}
	}

	enum
	{
		ENUM_TEXTURE_SIZE = 11,
		ENUM_TEXTURE_PIXEL_SIZE = 1024,
		//ENUM_TEXTURE_SIZE = 12,
		//ENUM_TEXTURE_PIXEL_SIZE = 2048,
	};

	// 0 - 1
	// 1 - 2
	// 2 - 4
	// 3 - 8
	// 4 - 16
	// 5 - 32
	// 6 - 64
	// 7 - 128
	// 8 - 256
	// 9 - 512
	// 10 - 1024
	// 11 - 2048
	WORDWORD*   g_pTexelEnable[ ENUM_TEXTURE_SIZE ];
	DWORD GetTextureSize( DWORD i )
	{
		switch( i )
		{
		case 0:		return 1;
		case 1:		return 2;
		case 2:		return 4;
		case 3:		return 8;
		case 4:		return 16;
		case 5:		return 32;
		case 6:		return 64;
		case 7:		return 128;
		case 8:		return 256;
		case 9:		return 512;
		case 10:	return 1024;
		case 11:	return 2048;
		default:	return 1;
		};
	}

	//BOOL g_bBlurOff(FALSE);
	BOOL g_bNoDXT1(FALSE);
	int	g_nSampling(1);

    void BackUp_Piece()
    {
		m_vecTextureData_BackUp.clear();

		for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
		{
			std::tr1::shared_ptr<TextureData> spTextureData = std::tr1::shared_ptr<TextureData>(new TextureData(1,1));
			spTextureData->Import( *m_vecTextureData[i] );
			m_vecTextureData_BackUp.push_back( spTextureData );
		}
    }

    void RollBack_Piece()
    {
		m_vecTextureData.clear();

		for ( DWORD i=0; i<m_vecTextureData_BackUp.size(); ++i )
		{
			std::tr1::shared_ptr<TextureData> spTextureData = std::tr1::shared_ptr<TextureData>(new TextureData(1,1));
			spTextureData->Import( *m_vecTextureData_BackUp[i] );
			m_vecTextureData.push_back( spTextureData );
		}
    }

    void Init( BOOL bPiece )
    {
        m_vecTextureData.clear();

        {
			for ( DWORD z=0; z<ENUM_TEXTURE_SIZE; ++z )
			{
				DWORD dwTexSize = GetTextureSize( z );

				SAFE_DELETE_ARRAY( g_pTexelEnable[z] );
				g_pTexelEnable[z] = new WORDWORD[dwTexSize*dwTexSize];

				for( DWORD j=0; j<dwTexSize; ++j )
				{
					for( DWORD i=0; i<dwTexSize; ++i )
					{
						g_pTexelEnable[z][j*dwTexSize+i].wA = static_cast<WORD>(i);
						g_pTexelEnable[z][j*dwTexSize+i].wB = static_cast<WORD>(j);
					}
				}
			}
        }
    }

    void CleanUp()
    {
		// 메모리 정리한다.
		for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
		{
			m_vecTextureData[i]->CleanUp();
		}
        m_vecTextureData.clear();

		for ( DWORD i=0; i<ENUM_TEXTURE_SIZE; ++i )
		{
			SAFE_DELETE_ARRAY( g_pTexelEnable[i] );
		}
    }

    BOOL InsertFaceID( BYTE* pVertiecsSrc, DWORD dwFVF, int i, int j, DWORD dwMeshGroup, VEC_FACE_ID& vecFaceGroupID, VEC_FACE_ID& vecFaceID )
    {
        D3DXVECTOR3* pVector3[2];
        D3DXVECTOR2* pVector2[2];
        DWORD*       pColor[2];
        UINT nSIZE = D3DXGetFVFVertexSize ( dwFVF );
        DWORD dwSizeADD = 0;

        for ( DWORD x=0; x<3; ++x )
        {
            for ( DWORD z=0; z<3; ++z )
            {
                dwSizeADD = 0;

                if ( dwFVF & D3DFVF_XYZ )
		        {
                    pVector3[0] = reinterpret_cast<D3DXVECTOR3*>( pVertiecsSrc + (vecFaceID[j]*3+x)*nSIZE + dwSizeADD );
			        pVector3[1] = reinterpret_cast<D3DXVECTOR3*>( pVertiecsSrc + (i*3+z)*nSIZE + dwSizeADD );

			        dwSizeADD += sizeof(D3DXVECTOR3);

                    if ( *pVector3[0] != *pVector3[1] )
                        continue;
		        }
		        if ( dwFVF & D3DFVF_NORMAL )
		        {
			        pVector3[0] = reinterpret_cast<D3DXVECTOR3*>( pVertiecsSrc + (vecFaceID[j]*3+x)*nSIZE + dwSizeADD );
			        pVector3[1] = reinterpret_cast<D3DXVECTOR3*>( pVertiecsSrc + (i*3+z)*nSIZE + dwSizeADD );

			        dwSizeADD += sizeof(D3DXVECTOR3);

                    if ( *pVector3[0] != *pVector3[1] )
                        continue;
		        }
		        if ( dwFVF & D3DFVF_DIFFUSE )
		        {
                    pColor[0] = reinterpret_cast<D3DCOLOR*>( pVertiecsSrc + (vecFaceID[j]*3+x)*nSIZE + dwSizeADD );
			        pColor[1] = reinterpret_cast<D3DCOLOR*>( pVertiecsSrc + (i*3+z)*nSIZE + dwSizeADD );

			        dwSizeADD += sizeof(DWORD);

                    if ( *pColor[0] != *pColor[1] )
                        continue;
		        }
                if ( ( dwFVF & D3DFVF_TEXCOUNT_MASK ) >= D3DFVF_TEX1 )
		        {
                    pVector2[0] = reinterpret_cast<D3DXVECTOR2*>( pVertiecsSrc + (vecFaceID[j]*3+x)*nSIZE + dwSizeADD );
			        pVector2[1] = reinterpret_cast<D3DXVECTOR2*>( pVertiecsSrc + (i*3+z)*nSIZE + dwSizeADD );

			        dwSizeADD += sizeof(D3DXVECTOR2);

                    if ( *pVector2[0] != *pVector2[1] )
                        continue;
		        }

                vecFaceGroupID[ i ] = dwMeshGroup;
                vecFaceID.push_back( i );

                return TRUE;
            }
        }

        return FALSE;
    }

    BOOL InsertFaceID_Index( const VEC_DWORD& vecIndex, int i, int j, DWORD dwMeshGroup, VEC_FACE_ID& vecFaceGroupID, VEC_FACE_ID& vecFaceID )
    {
        for ( DWORD x=0; x<3; ++x )
        {
            for ( DWORD z=0; z<3; ++z )
            {
                if ( vecIndex[ vecFaceID[j]*3+x ] != vecIndex[ i*3+z ] )
                    continue;

                vecFaceGroupID[ i ] = dwMeshGroup;
                vecFaceID.push_back( i );

                return TRUE;
            }
        }

        return FALSE;
    }

	void RemakeLightMapUserUV3( TextureData& sTextureData, DWORD dwTexWidth, DWORD& dwNewWidth, DWORD& dwNewHeight )
	{
		// 가장 멀리 쓰고 있는 곳까지 구한다.
		for( DWORD y=0; y<dwTexWidth; ++y )
		{
			for( DWORD x=0; x<dwTexWidth; ++x )
			{
				if ( sTextureData.m_pbTexelState[y*dwTexWidth+x] == false )
				{
					if ( x >= dwNewWidth )
						dwNewWidth = x+1;
					if ( y >= dwNewHeight )
						dwNewHeight = y+1;
				}
			}
		}
	}

	BOOL RemakeLightMapUserUV2( MAP_LIGHTMAPUV &mapLightMapUV, TextureData& sTextureData, VEC_DWORD &vecDWORD )
	{
		vecDWORD.clear();

		DWORD dwTexWidth = sTextureData.m_dwTexWidth;

		WORDWORD* pTexelEnable = new WORDWORD[ dwTexWidth*dwTexWidth ];

		for( DWORD j=0; j<dwTexWidth; ++j )
		{
			for( DWORD i=0; i<dwTexWidth; ++i )
			{
				pTexelEnable[j*dwTexWidth+i].wA = static_cast<WORD>(i);
				pTexelEnable[j*dwTexWidth+i].wB = static_cast<WORD>(j);
			}
		}

		int nCount(0);
		BOOL bRemindDrawPlace(TRUE);
		MAP_LIGHTMAPUV_ITER iter = mapLightMapUV.begin();
		for ( ; iter!=mapLightMapUV.end(); ++iter, ++nCount )
		{
			std::tr1::shared_ptr<LIGHTMAPUV> spLightMap = (*iter).second;
			if ( spLightMap->m_vecUV.empty() )
			{
				vecDWORD.push_back( nCount );		// LightMap 정보가 없지만, 다음 작업에서 삭제가 필요하기에 push_back 한다.
				continue;
			}

			WORDWORD sTexelXY( 0L );
			BOOL bDraw = NSCOMBINETEXTOOLS::GetTexelAndTexelMarking
			( 
				sTexelXY,
				spLightMap->m_wwTextureSize.wA, //pLightMapObject->GetTexelSizeX()+2, 
				spLightMap->m_wwTextureSize.wB, //pLightMapObject->GetTexelSizeY()+2, 
				sTextureData.m_pbTexelState, 
				pTexelEnable,
				sTextureData.m_pTexelLast,
				sTextureData.m_dwTexelUseNUM, 
				sTextureData.m_dwTexWidth, 
				sTextureData.m_dwTexHeight
			);

			// 그려야 하는 위치를 저장합니다.
			spLightMap->m_wwBaseTexelXY = sTexelXY;

			if( bDraw )
			{
				// 하지만 2048x2048 텍스쳐일 경우는 그 안에 들어가는 텍스쳐를 찾기 위해 vecDWORD 다 값을 넣어 놓는다.
				// 현재 상황에서 들어갈 수 있는 LightMap Texture 들..
				vecDWORD.push_back( nCount );
			}
			else
			{
				// 텍스쳐가 꽉 찼다 그릴 수가 없다. 
				// 하지만 2048x2048 텍스쳐일 경우는 그 안에 들어가는 텍스쳐를 찾기 위해 vecDWORD 다 값을 넣어 놓는다.
				bRemindDrawPlace = FALSE;
			}
		}

		SAFE_DELETE_ARRAY ( pTexelEnable );

		return bRemindDrawPlace;
	}

	// User 가 만든 LightMapUV 를 잘 사용가능하도록 변경한다.
	BOOL RemakeLightMapUserUV( MAP_LIGHTMAPUV& mapLightMapUV, SP_LIGHTMAPUV_MAIN spLightMapUV_OUTPUT, VEC_DWORD& vecDWORD )
	{
		//VEC_DWORD vecDWORD;
		//DWORD dwTexWidth(32);
		//DWORD dwNewWidth(0);
		//DWORD dwNewHeight(0);
		//for ( ; dwTexWidth <= 2048; dwTexWidth*=2 )
		//{
		//	TextureData sTextureData(dwTexWidth,dwTexWidth);
		//	if ( RemakeLightMapUserUV2( mapLightMapUV, sTextureData, vecDWORD ) ||	// 정상적으로 들어간다.
		//		dwTexWidth == 2048 )												// 최대 텍스쳐인데도 안된다면 추가로 텍스쳐를 만든다.
		//	{
		//		RemakeLightMapUserUV3( sTextureData, dwTexWidth, dwNewWidth, dwNewHeight );
		//		break;
		//	}
		//}

		int nCount(0);
		MAP_LIGHTMAPUV_ITER iter = mapLightMapUV.begin();
		for ( ; iter!=mapLightMapUV.end(); ++nCount )
		{
			if ( vecDWORD.empty() )
			{
				break;
			}

			if ( nCount == vecDWORD[0] )
			{
				spLightMapUV_OUTPUT->m_mapLightMapUV.insert( std::make_pair( (*iter).first, (*iter).second ) );
				iter = mapLightMapUV.erase( iter );
				VEC_DWORD_ITER iter2 = vecDWORD.begin();
				vecDWORD.erase( iter2 );
			}
			else
			{
				++iter;
			}
		}

		// UV 수치를 변경해 보자.
		{
			MAP_LIGHTMAPUV_ITER iter = spLightMapUV_OUTPUT->m_mapLightMapUV.begin();
			for ( ; iter!=spLightMapUV_OUTPUT->m_mapLightMapUV.end(); ++iter )
			{
				D3DXVECTOR2 vMax(-FLT_MAX,-FLT_MAX);
				D3DXVECTOR2 vMin(FLT_MAX,FLT_MAX);
				std::tr1::shared_ptr<LIGHTMAPUV> spLightMapUV = (*iter).second;
				//for ( DWORD i=0; i<spLightMapUV->m_vecUV.size(); ++i )
				//{
				//	if ( vMax.x < spLightMapUV->m_vecUV[i].x )
				//		vMax.x = spLightMapUV->m_vecUV[i].x;
				//	if ( vMin.x > spLightMapUV->m_vecUV[i].x )
				//		vMin.x = spLightMapUV->m_vecUV[i].x;
				//	if ( vMax.y < spLightMapUV->m_vecUV[i].y )
				//		vMax.y = spLightMapUV->m_vecUV[i].y;
				//	if ( vMin.y > spLightMapUV->m_vecUV[i].y )
				//		vMin.y = spLightMapUV->m_vecUV[i].y;
				//}

				//// 수치를 0~1 로 맞춘다.
				//D3DXVECTOR2 vMultiply(vMax - vMin);
				//vMultiply.x = 1.f / vMultiply.x;
				//vMultiply.y = 1.f / vMultiply.y;
				//for ( DWORD i=0; i<spLightMapUV->m_vecUV.size(); ++i )
				//{
				//	spLightMapUV->m_vecUV[i] -= vMin;
				//	spLightMapUV->m_vecUV[i].x *= vMultiply.x;
				//	spLightMapUV->m_vecUV[i].y *= vMultiply.y;
				//}

				float fWidthRate = static_cast<float>(spLightMapUV->m_wwTextureSize.wA) / spLightMapUV_OUTPUT->m_dwTextureWidth;		// 이렇게 줄어들었다.
				float fHeightRate = static_cast<float>(spLightMapUV->m_wwTextureSize.wB) / spLightMapUV_OUTPUT->m_dwTextureHeight;	// 이렇게 줄어들었다.
				float fWidthBase = static_cast<float>(spLightMapUV->m_wwBaseTexelXY.wA) / spLightMapUV_OUTPUT->m_dwTextureWidth;		// 이 위치로 이동되었다.
				float fHeightBase = static_cast<float>(spLightMapUV->m_wwBaseTexelXY.wB) / spLightMapUV_OUTPUT->m_dwTextureHeight;	// 이 위치로 이동되었다.

				for ( DWORD i=0; i<spLightMapUV->m_vecUV.size(); ++i )
				{
					spLightMapUV->m_vecUV[i].x *= fWidthRate;
					spLightMapUV->m_vecUV[i].x += fWidthBase;
					spLightMapUV->m_vecUV[i].y *= fHeightRate;
					spLightMapUV->m_vecUV[i].y += fHeightBase;
				}
			}
		}

		return TRUE;
	}

	// User 가 만든 LightMapUV 를 잘 사용가능하도록 변경한다.
	BOOL RemakeLightMapUserUV_STATICMESH( MAP_LIGHTMAPUV& mapLightMapUV, SP_LIGHTMAPUV_MAIN spLightMapUV_OUTPUT )
	{
		VEC_DWORD vecDWORD;
		DWORD dwTexWidth(32);
		DWORD dwNewWidth(0);
		DWORD dwNewHeight(0);
		for ( ; dwTexWidth <= ENUM_TEXTURE_PIXEL_SIZE; dwTexWidth*=2 )
		{
			TextureData sTextureData(dwTexWidth,dwTexWidth);
			if ( RemakeLightMapUserUV2( mapLightMapUV, sTextureData, vecDWORD ) ||	// 정상적으로 들어간다.
				dwTexWidth == ENUM_TEXTURE_PIXEL_SIZE )												// 최대 텍스쳐인데도 안된다면 추가로 텍스쳐를 만든다.
			{
				break;
			}
		}

		spLightMapUV_OUTPUT->m_dwTextureWidth = dwTexWidth;
		spLightMapUV_OUTPUT->m_dwTextureHeight = dwTexWidth;
		RemakeLightMapUserUV( mapLightMapUV, spLightMapUV_OUTPUT, vecDWORD );

		return TRUE;
	}

	// User 가 만든 LightMapUV 를 잘 사용가능하도록 변경한다.
	BOOL RemakeLightMapUserUV_PIECE( MAP_LIGHTMAPUV& mapLightMapUV, SP_LIGHTMAPUV_MAIN spLightMapUV_OUTPUT )
	{
		VEC_DWORD vecDWORD;
		DWORD dwTexWidth(32);
		DWORD dwNewWidth(0);
		DWORD dwNewHeight(0);
		for ( ; dwTexWidth <= ENUM_TEXTURE_PIXEL_SIZE; dwTexWidth*=2 )
		{
			TextureData sTextureData(dwTexWidth,dwTexWidth);
			if ( RemakeLightMapUserUV2( mapLightMapUV, sTextureData, vecDWORD ) ||	// 정상적으로 들어간다.
				dwTexWidth == ENUM_TEXTURE_PIXEL_SIZE )												// 최대 텍스쳐인데도 안된다면 추가로 텍스쳐를 만든다.
			{
				RemakeLightMapUserUV3( sTextureData, dwTexWidth, dwNewWidth, dwNewHeight );
				break;
			}
		}

		spLightMapUV_OUTPUT->m_dwTextureWidth = dwNewWidth;		// 이부분에서 FrameMesh 랑 Piece 랑 다르다.
		spLightMapUV_OUTPUT->m_dwTextureHeight = dwNewHeight;	// 이부분에서 FrameMesh 랑 Piece 랑 다르다.
		RemakeLightMapUserUV( mapLightMapUV, spLightMapUV_OUTPUT, vecDWORD );

		return TRUE;
	}

    BOOL EnableInsertTexel( VEC_LIGHTMAPOBJ& vecLightMapObjectDest, TextureData& sTextureData, DWORD TextureID, BOOL bUserUV )
    {
		DWORD dwTextureType(0);
        switch ( sTextureData.m_dwTexWidth )
        {
		case 1:		dwTextureType = 0;	break;
		case 2:		dwTextureType = 1;	break;
		case 4:		dwTextureType = 2;	break;
		case 8:		dwTextureType = 3;	break;
		case 16:	dwTextureType = 4;	break;
		case 32:	dwTextureType = 5;	break;
		case 64:	dwTextureType = 6;	break;
		case 128:	dwTextureType = 7;	break;
		case 256:	dwTextureType = 8;	break;
        case 512:	dwTextureType = 9;	break;
        case 1024:	dwTextureType = 10;	break;
        case 2048:	dwTextureType = 11;	break;
		default:	break;
        };
		WORDWORD* pTexelEnable = g_pTexelEnable[dwTextureType];

        for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
        {
            LightMapObject* pLightMapObject = vecLightMapObjectDest[i].get();            

            // Note : 썼다고 마킹하고 그려야 하는 위치를 얻어온다.
            {
				WORDWORD sTexelXY( 0L );
				BOOL bDraw(FALSE);
				if ( bUserUV )
				{
					bDraw = NSCOMBINETEXTOOLS::GetTexelAndTexelMarking
					( 
						sTexelXY,
						pLightMapObject->GetTexelSizeX(), 
						pLightMapObject->GetTexelSizeY(), 
						sTextureData.m_pbTexelState, 
						pTexelEnable,
						sTextureData.m_pTexelLast,
						sTextureData.m_dwTexelUseNUM, 
						sTextureData.m_dwTexWidth, 
						sTextureData.m_dwTexHeight
					);
				}
				else
				{
					bDraw = NSCOMBINETEXTOOLS::GetTexelAndTexelMarking
					( 
						sTexelXY,
						pLightMapObject->GetTexelSizeX()+2, 
						pLightMapObject->GetTexelSizeY()+2, 
						sTextureData.m_pbTexelState, 
						pTexelEnable,
						sTextureData.m_pTexelLast,
						sTextureData.m_dwTexelUseNUM, 
						sTextureData.m_dwTexWidth, 
						sTextureData.m_dwTexHeight
					);
				}

                // 그려야 하는 위치를 저장합니다.
                pLightMapObject->SetBaseTexelXY( sTexelXY );
                pLightMapObject->SetTextureID( TextureID );

		        if( !bDraw )
		        {
			        // 텍스쳐가 꽉 차서 그릴 수가 없다. 
                    return FALSE;
		        }
            }
        }

        // 정보를 넣어놓는다.
        sTextureData.m_vecvecLightMapObject.push_back( &vecLightMapObjectDest );

        return TRUE;
    }

    void CreateLightMapFinal( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwIndex, const TSTRING& strLightMapName, const TSTRING& strAddName, DWORD dwTextureID, int nBackUpNum, const TSTRING& strFinalName )
    {
        int i = dwIndex;

		// 범위를 넘어섰다.
		if ( dwIndex >= m_vecTextureData.size() )
			return;

        // 텍스쳐가 만들어진 상태이다.
        if ( m_vecTextureData[i]->m_bMakeTexture )
            return;

		_D3DFORMAT emFormat[4] = { D3DFMT_DXT1, D3DFMT_DXT1, D3DFMT_DXT1, D3DFMT_DXT1 };

        // 텍스쳐가 만들어졌다고 셋팅한다.
        m_vecTextureData[i]->m_bMakeTexture = TRUE;

        ExportProgress::SetStateString( "Texture 생성" );

        DWORD dwTexWidth( m_vecTextureData[i]->m_dwTexWidth );
        DWORD dwTexHeight( m_vecTextureData[i]->m_dwTexHeight );

        INT iPitch(0);
        DWORD* pData;
        IDirect3DTexture9* pTexture;
        TSTRING strAddNameARRAY[4] = { _T("_C_"), _T("_DS1_"), _T("_DS2_"), _T("_I_") };
        DWORD* pTexelDest[4] = { m_vecTextureData[ i ]->m_pTexelLastDay, 
                                m_vecTextureData[ i ]->m_pTexelLastDirect_Shadow,
								m_vecTextureData[ i ]->m_pTexelLastDirect_Shadow,
								m_vecTextureData[ i ]->m_pTexelLastIndirect };

        {
            D3DXCreateTexture(
                pd3dDevice,
                dwTexWidth,
                dwTexHeight,
                1,
                0L,
                D3DFMT_A8R8G8B8,
                D3DPOOL_SYSTEMMEM,
                &pTexture );
        }

        {
            ExportProgress::SetStateString( "텍스쳐를 저장." );
        }
        {
            int nTexCount = dwTextureID;

            int n100 = nTexCount/100;
            int n10 = (nTexCount-n100*100) / 10;
            int n1 = nTexCount%10;
            TCHAR c100 = (TCHAR)(n100+48);
            TCHAR c10 = (TCHAR)(n10+48);
            TCHAR c1 = (TCHAR)(n1+48);

            WORDWORD sBaseTexel(0,0);
            for ( DWORD j=0; j<4; ++j )
            {
                D3DLOCKED_RECT stLocked;
                HRESULT hr = pTexture->LockRect( 0, &stLocked, NULL, 0 );
				if ( FAILED(hr) )
				{
					GASSERT( 0 && _T("HRESULT hr = pTexture->LockRect( 0, &stLocked, NULL, 0 );") );
					return;	// intel 칩의 구형 그래픽드라이버 사용되면 오류가 발생할 수 있음.
				}

                pData = (DWORD*)stLocked.pBits;
                iPitch = stLocked.Pitch / 4;
                {
					if ( j == 1 )
					{
						// Note : 실제 텍스쳐에 그린다.
						NSCOMBINETEXTOOLS::DrawTexel_Tex2Array
						( 
							pData, 
							iPitch, 
							pTexelDest[j],
							sBaseTexel,
							dwTexWidth,
							dwTexHeight,
							0,
							0,
							0xff000000,
							24,
							0x000000ff,
							0
						);
					}
					else if ( j == 2 )
					{
						// Note : 실제 텍스쳐에 그린다.
						NSCOMBINETEXTOOLS::DrawTexel_Tex2Array
						( 
							pData, 
							iPitch, 
							pTexelDest[j],
							sBaseTexel,
							dwTexWidth,
							dwTexHeight,
							0,
							0,
							0x0000ff00,
							8,
							0x00ff0000,
							16
						);
					}
					else
					{
						// Note : 실제 텍스쳐에 그린다.
						NSCOMBINETEXTOOLS::DrawTexel_Tex2Array
						( 
							pData, 
							iPitch, 
							pTexelDest[j],
							sBaseTexel,
							dwTexWidth,
							dwTexHeight,
							0,
							0
						);
					}
                }
                pTexture->UnlockRect( 0 );
                
				TSTRING strTexture = _T("");
				strTexture = strTexture + strLightMapName + strAddName + strAddNameARRAY[j] + c100 + c10 + c1 + strFinalName + _T(".dds");
				TSTRING strFullName = g_strPath + strTexture.c_str();
				
				if ( g_bNoDXT1 )		// 압축하지 않겠다.
				{
					D3DXSaveTextureToFile( strFullName.c_str(), D3DXIFF_DDS, pTexture, NULL );
				}
				else					// 압축하겠다.
				{
					LPDIRECT3DBASETEXTURE9 pTexNew(NULL);						
					NSTexCompress::Compress
					(
						pd3dDevice,
						dwTexWidth,
						dwTexHeight,
						0,
						1,
						emFormat[j],
						pTexture,
						pTexNew
					);

					D3DXSaveTextureToFile( strFullName.c_str(), D3DXIFF_DDS, pTexNew, NULL );
					SAFE_RELEASE( pTexNew );
				}

				// 추가적으로 Back Texture를 만들도록 한다.
				if ( nBackUpNum > 0 )
				{
					TSTRINGSTREAM strStringStream;
					strStringStream << "BackUp_";
					strStringStream << nBackUpNum;
					strStringStream << "_";
					strTexture = strStringStream.str().c_str();
					strTexture = strTexture + strLightMapName + strAddName + strAddNameARRAY[j] + c100 + c10 + c1 + _T(".dds");
					strFullName = g_strPath + strTexture.c_str();
					D3DXSaveTextureToFile( strFullName.c_str(), D3DXIFF_DDS, pTexture, NULL );
				}
            }
        }

        // 삭제
        {
            // SAFE_RELEASE
            SAFE_RELEASE( pTexture );
        }

		// Radiosity 때문에 이곳에서 CleanUp을 하지 못한다.
		// 메모리 정리한다.
		//m_vecTextureData[i]->CleanUp();
    }

    DWORD GetTextureDataID_EmptyHighRate()
    {
        DWORD dwIndex(0);
        float fEmpty(0.f);
        float fEmptyHigh(0.f);
        for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
        {
            if ( m_vecTextureData[i]->m_bMakeTexture )
                continue;
            
            fEmpty = static_cast<float>(m_vecTextureData[i]->m_dwTexelEmptyNum)/(m_vecTextureData[i]->m_dwTexWidth*m_vecTextureData[i]->m_dwTexHeight);

            if ( fEmpty > fEmptyHigh )
            {
                dwIndex = i;
                fEmptyHigh = fEmpty;
            }
        }

        return dwIndex;
    }

    DWORD GetTextureDataID_EmptyLowRate()
    {
        DWORD dwIndex(0);
        float fEmpty(0.f);
        float fEmptyLow(1.f);
        for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
        {
            if ( m_vecTextureData[i]->m_bMakeTexture )
                continue;
            
            fEmpty = static_cast<float>(m_vecTextureData[i]->m_dwTexelEmptyNum)/(m_vecTextureData[i]->m_dwTexWidth*m_vecTextureData[i]->m_dwTexHeight);

            if ( fEmpty < fEmptyLow )
            {
                dwIndex = i;
                fEmptyLow = fEmpty;
            }
        }

        return dwIndex;
    }

    DWORD GetTextureDataID_EmptyHighBlock()
    {
        DWORD dwIndex(0);
        DWORD dwEmptyHigh(0);
        for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
        {
            if ( m_vecTextureData[i]->m_bMakeTexture )
                continue;

            if ( m_vecTextureData[i]->m_dwTexelEmptyNum > dwEmptyHigh )
            {
                dwIndex = i;
                dwEmptyHigh = m_vecTextureData[i]->m_dwTexelEmptyNum;
            }
        }

        return dwIndex;
    }

    DWORD GetTextureDataID_EmptyLowerBlock()
    {
        DWORD dwIndex(0);
        DWORD dwEmptyLower(INT_MAX);
        for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
        {
            if ( m_vecTextureData[i]->m_bMakeTexture )
                continue;

            if ( m_vecTextureData[i]->m_dwTexelEmptyNum < dwEmptyLower )
            {
                dwIndex = i;
                dwEmptyLower = m_vecTextureData[i]->m_dwTexelEmptyNum;
            }
        }

        return dwIndex;
    }

    //void CreateTextureDataLowerEmpty( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strLightMapName, const TSTRING& strAddName, BOOL bAlphaChange, BOOL bPiece, BOOL bUserUV )
    //{
    //    DWORD dwIndex = GetTextureDataID_EmptyLowRate();

    //    // 텍스쳐를 생성해야 하는 대상은 dwIndex
    //    {
    //        if ( m_vecTextureData[dwIndex]->m_pbTexelState )
    //        {
    //            NSCOMBINETEXTOOLS::SetFitHeight
    //            ( 
    //                m_vecTextureData[dwIndex]->m_pbTexelState, 
    //                m_vecTextureData[dwIndex]->m_dwTexWidth, 
    //                m_vecTextureData[dwIndex]->m_dwTexHeight
    //            );
    //        }

    //        for ( DWORD i=0; i<m_vecTextureData[dwIndex]->m_vecvecLightMapObject.size(); ++i )
    //        {
    //            VEC_LIGHTMAPOBJ* pvecLightMapObj = m_vecTextureData[dwIndex]->m_vecvecLightMapObject[i];
    //            for ( DWORD j=0; j<pvecLightMapObj->size(); ++j )
    //            {
    //                LightMapObject* pLightMapObject = (*pvecLightMapObj)[j].get();

    //                CreateLightMap2_Detail
    //                ( 
    //                    pLightMapObject,
				//		pLightMapObject->GetTextureID(),
    //                    bAlphaChange,
    //                    bPiece,
    //                    pLightMapObject->GetBaseTexelPieceXY(),
    //                    FALSE,
				//		bUserUV
    //                );

    //                // 메모리를 정리한다.
    //                pLightMapObject->DeleteTexelColor();
    //            }
    //        }
    //    }

    //    CreateLightMapFinal( pd3dDevice, dwIndex, strLightMapName, strAddName, dwIndex );

    //    // 메모리 정리한다.
    //    m_vecTextureData[dwIndex]->CleanUp();
    //}

    void CreateFaceGroup( VEC_FACE_ID& vecFaceGroupID, 
                        DWORD& dwMeshGroup, 
                        VEC_FACE_ID& vecFaceID, 
                        DWORD dwFaceNUM, 
                        BYTE* pVerticesSrc, 
                        DWORD dwFVF )
    {
        // 디폴트 값을 만들어 놓는다.
        {
            for ( DWORD i=0; i<dwFaceNUM; ++i )
            {
                vecFaceGroupID.push_back( -1 );
            }
        }

        // 첫번째 삼각형을 첫번째 그룹으로 만든다.
        {
            int nIndex(0);
            vecFaceGroupID[ nIndex ] = dwMeshGroup;
            vecFaceID.push_back( nIndex );
        }

		for ( DWORD i=1; i<dwFaceNUM; ++i )
		{
			vecFaceGroupID[i] = 0;
			vecFaceID.push_back( i );
		}

        //// 인접한 삼각형을 구한다.
        //{
        //    int nStartIndex(1);
        //    switch ( emLightMapType )
        //    {
        //    case NSLIGHTMAPDEF::LMT_GROUND:
        //        {
        //            for ( DWORD i=1; i<dwFaceNUM; ++i )
        //            {
        //                vecFaceGroupID[i] = 0;
        //                vecFaceID.push_back( i );
        //            }
        //        }
        //        break;

        //    case NSLIGHTMAPDEF::LMT_NORMAL:
        //        break;

        //    case NSLIGHTMAPDEF::LMT_UV:
        //        {
        //            while( 1 )
        //            {
        //                // 생성 및 초기화
        //                BOOL bNearTriangle = FALSE;

        //                // 그룹이 선택되지 않는 삼각형들을 순환해서 인접 삼각형이 있는지 찾는다.
        //                for ( int i=nStartIndex; i<static_cast<int>(dwFaceNUM); ++i )
        //                {
        //                    // 인덱스가 셋팅되어 있다면 작업하지 않는다.
        //                    if ( vecFaceGroupID[i] != -1 )
        //                    {
        //                        continue;
        //                    }

        //                    // 붙어있는 삼각형을 찾는다.
        //                    {
        //                        DWORD dwSize = static_cast< DWORD >( vecFaceID.size() );
        //                        for ( DWORD j=0; j<dwSize; ++j )
        //                        {
        //                            if ( InsertFaceID( pVerticesSrc, dwFVF, i, j, dwMeshGroup, vecFaceGroupID, vecFaceID, emLightMapType ) )
        //                            {
        //                                bNearTriangle = TRUE;
        //                                break;
        //                            }
        //                        }
        //                    }
        //                }

        //                // 가까운 삼각형이 없다면 다음 그룹을 만들어준다.
        //                if ( !bNearTriangle )
        //                {
        //                    BOOL bNewGroup(FALSE);
        //                    for ( DWORD i=nStartIndex; i<dwFaceNUM; ++i )
        //                    {
        //                        if ( vecFaceGroupID[i] == -1 )
        //                        {
        //                            bNewGroup = TRUE;
        //                            vecFaceID.clear();
        //                            vecFaceGroupID[ i ] = ++dwMeshGroup;
        //                            vecFaceID.push_back( i );
        //                            nStartIndex = i+1;
        //                            break;
        //                        }
        //                    }

        //                    // 새로운 그룹을 만들 수가 없다. while 을 벗어나도록 한다.
        //                    if ( !bNewGroup )
        //                    {
        //                        break;
        //                    }
        //                }
        //            }
        //        }
        //        break;
        //    };
        //}
    }

  //  // 
  //  BOOL CreateLightMap( LPDIRECT3DDEVICEQ pd3dDevice, 
  //                      const TSTRING& strLightMapName, 
  //                      const TSTRING& strAddName, 
  //                      BOOL bAlphaChange,
  //                      BOOL bPiece,
  //                      VEC_LIGHTMAPOBJ& vecLightMapObjectDest,
  //                      DWORD dwFaceNUM, 
  //                      BYTE* pVerticesSrc, 
  //                      DWORD dwFVF,
  //                      DxStaticMeshCollBase* pStaticMesh,
  //                      DxPieceManager* pPieceManager,
  //                      const D3DXMATRIX& matWorld,
  //                      DWORD& dwStartTextureSize,
  //                      float fLightMapDetail,
  //                      NSLIGHTMAPDEF::LIGHTMAPTYPE emLightMapType,
  //                      BOOL bDontMakeShadowLM )
  //  {
  //      g_pd3dDevice = pd3dDevice;

  //      vecLightMapObjectDest.clear();

  //      if ( dwFaceNUM == 0 )
  //          return TRUE;

		//// 텍스쳐 최대 사이즈는 2048를 넘을 수 없다.
		//if ( dwStartTextureSize > 2048 )
		//	dwStartTextureSize = 2048;

  //      if ( !bPiece && dwFaceNUM>1000 )
  //      {
  //          ExportProgress::SetStateString( "dwFaceNUM>1000" );
  //          ExportProgress::SetStateString( "인접한 삼각형 별로 분리,셋팅.1" );
  //      }

  //      // 인접한 삼각형 별로 분리,셋팅 한다.
  //      {    
  //          VEC_FACE_ID vecFaceGroupID;     // 각 Face 별로 어느 그룹에 속해 있는지 셋팅한다.
  //          DWORD       dwMeshGroup(0);     // 인접 삼각형 그룹
  //          VEC_FACE_ID vecFaceID;          // 그룹 선택이 바뀌면 clear 된다.

  //          // FaceGroup을 만든다.
  //          CreateFaceGroup( vecFaceGroupID, dwMeshGroup, vecFaceID, dwFaceNUM, pVerticesSrc, dwFVF, emLightMapType );

  //          // 그룹별로 LightMapObject 를 만들도록 한다.
  //          {
  //              for ( DWORD i=0; i<(dwMeshGroup+1); ++i )
  //              {
  //                  std::tr1::shared_ptr<LightMapObject> spNew = std::tr1::shared_ptr<LightMapObject>(new LightMapObject);

  //                  if ( dwFVF == VERTEX::FVF )
  //                  {
  //                      VERTEX* pVertices = reinterpret_cast<VERTEX*>( pVerticesSrc );

  //                      // 버텍스를 추가한다.
  //                      for ( DWORD j=0; j<dwFaceNUM; ++j )
  //                      {
  //                          if ( i == vecFaceGroupID[ j ] )
  //                          {
  //                              spNew->AddThreeVerties( &pVertices[j*3+0], emLightMapType, j );
  //                          }
  //                      }
  //                  }
  //                  else if ( dwFVF == VERTEXNORCOLORTEX::FVF )
  //                  {
  //                      VERTEXNORCOLORTEX* pVertices = reinterpret_cast<VERTEXNORCOLORTEX*>( pVerticesSrc );

  //                      // 버텍스를 추가한다.
  //                      for ( DWORD j=0; j<dwFaceNUM; ++j )
  //                      {
  //                          if ( i == vecFaceGroupID[ j ] )
  //                          {
  //                              spNew->AddThreeVerties( &pVertices[j*3+0], emLightMapType, j );
  //                          }
  //                      }
  //                  }

  //                  // 삽입한다.
  //                  vecLightMapObjectDest.push_back( spNew );
  //              }
  //          }
  //      }

  //      BOOL bTryMakeTexture(TRUE);
  //      float fLengthRate = fLightMapDetail;   // 0.25f // User가 정한 값을 넣는다.
  //      while ( bTryMakeTexture )
  //      {
  //          bTryMakeTexture = FALSE;

  //          if ( !bPiece && dwFaceNUM>1000 )
  //          {
  //              ExportProgress::SetStateString( "최소화 가능한 Pixel 검사중." );
  //          }

  //          // 그룹별로 LightMapObject 를 만들도록 한다.
  //          {
  //              for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
  //              {
  //                   // 텍셀 크기를 구한다.
  //                  vecLightMapObjectDest[i]->CreateTexelScale( fLengthRate );

  //                  // 최소화 가능한 Pixel 검사 루틴에서 Piece의 경우는 제외시킨다.
  //                  // Piece는 동일한 사이즈가 나와야하기 때문에 사이즈 및 위치 변경이 되면 안된다.
  //                  if ( !bPiece )
  //                  {
  //                      // 같은 Pixel이 같은색이라면, 그 부위는 면적을 대폭 줄인다.
  //                      vecLightMapObjectDest[i]->CreateTexel
		//				( 
		//					pStaticMesh, 
		//					pPieceManager, 
		//					matWorld, 
		//					TRUE, 
		//					0L, 
		//					emLightMapType, 
		//					bDontMakeShadowLM, 
		//					NULL, 
		//					g_nSampling, 
		//					NULL
		//				);
  //                  }
  //              }

  //              // 한 Pixel이 커버하는 면적을 늘려준다.
  //              fLengthRate *= 0.7f;
  //          }

  //          if ( !bPiece && dwFaceNUM>1000 )
  //          {
  //              ExportProgress::SetStateString( "알맞은 텍스쳐를 구한다...." );
  //          }

  //          // 알맞은 텍스쳐를 구한다.
  //          {
  //              DWORD dwActiveSlot(0L);
  //              for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
  //              {
  //                  LightMapObject* pLightMapObject = vecLightMapObjectDest[i].get();            
  //                  dwActiveSlot += (pLightMapObject->GetTexelSizeX()+2) * (pLightMapObject->GetTexelSizeY()+2);
  //              }

  //              BOOL bActiveTexture(FALSE);

  //              // 얻어온 Index 만 검사를 시작한다.
  //              // 큰텍스쳐 부터 데이터를 넣도록 작업한다.
  //              TextureData sTextureData(g_dwBaseWidth,g_dwBaseWidth);
  //              for ( DWORD t=2048; t>=g_dwBaseWidth; t/=2 )
  //              {
  //                  for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
  //                  {
  //                      // 사이즈가 같은 것만 검사한다.
  //                      if ( m_vecTextureData[i]->m_dwTexWidth != t )
  //                          continue;

  //                      // Piece 의 경우는 사이즈가 동일해야만 그 텍스쳐에 넣을 수 있다.
  //                      if ( bPiece )
  //                      {
  //                          if ( dwStartTextureSize != m_vecTextureData[ i ]->m_dwTexWidth )
  //                              continue;
  //                      }

  //                      // 텍스쳐가 만들어진 상태이다.
  //                      if ( m_vecTextureData[ i ]->m_bMakeTexture )
  //                          continue;

  //                      // 텍스쳐의 남아 있는 공간이 기록해야 할 공간 대비 1.1배 이하로 남아 있다면 다른 곳에 기록한다.
  //                      if ( dwActiveSlot > m_vecTextureData[ i ]->m_dwTexelEmptyNum*1.1 )
  //                      {
  //                          continue;
  //                      }

  //                      // 데이터를 복제한다.
  //                      sTextureData.Import( *m_vecTextureData[ i ] );

  //                      // 알맞은 텍스쳐를 구하고, 데이터를 Change 한다.
  //                      if ( EnableInsertTexel( vecLightMapObjectDest, sTextureData, i, FALSE ) )
  //                      {
  //                          t = 0;
  //                          bActiveTexture = TRUE;
  //                          m_vecTextureData[ i ]->Import( sTextureData );
  //                          m_vecTextureData[ i ]->m_dwTexelEmptyNum -= dwActiveSlot; 
  //                          break;
  //                      }
  //                  }
  //              }

  //              // 활성화 된 Texture가 없을 경우 새로 생성해준다.
  //              if ( !bActiveTexture )
  //              {
  //                  // 기본적인 텍스쳐 사이즈를 셋팅함
  //                  DWORD dwTextureSize = dwStartTextureSize;

  //                  // 그려야 할 곳이 그려야하는 크기보다 적다면 사이즈를 늘린다.
  //                  {
  //                      if ( dwTextureSize*dwTextureSize < dwActiveSlot )
  //                      {
  //                          dwTextureSize *= 2;

  //                          if ( dwTextureSize*dwTextureSize < dwActiveSlot )
  //                          {
  //                              dwTextureSize *= 2;

  //                              if ( dwTextureSize*dwTextureSize < dwActiveSlot )
  //                              {
  //                                  dwTextureSize *= 2;
  //                              }
  //                          }
  //                      }

  //                      if ( dwTextureSize > 2048 )
  //                      {
  //                          // 작업이 불가능하다.
  //                          bTryMakeTexture = TRUE;
  //                      }
  //                  }

  //                  for( DWORD i=0; i<3 && dwTextureSize <= 2048; ++i )
  //                  {
  //                      TextureData sTextureData(dwTextureSize,dwTextureSize);
  //                      
  //                      // 알맞은 텍스쳐를 구하고, 데이터를 Change 한다.
  //                      if ( EnableInsertTexel( vecLightMapObjectDest, sTextureData, static_cast< DWORD >( m_vecTextureData.size() ), FALSE ) )
  //                      {
  //                          std::tr1::shared_ptr<TextureData> spNew( std::tr1::shared_ptr<TextureData>(new TextureData(dwTextureSize,dwTextureSize)) );
  //                          spNew->Import( sTextureData );
  //                          spNew->m_dwTexelEmptyNum -= dwActiveSlot;
  //                          m_vecTextureData.push_back( spNew );
  //                          break;
  //                      }

  //                      // 안되면 더 큰 사이즈의 텍스쳐로 작업해 본다.
  //                      dwTextureSize *= 2;
  //                  }

  //                  if ( dwTextureSize > 2048 )
  //                  {
  //                      // 작업이 불가능하다.
  //                      bTryMakeTexture = TRUE;
  //                  }

  //                  // Piece 의 경우는 단위가 한번 높아지면 높아진 단위로 유지한다.
  //                  // 성공을 했을때만 셋팅을 한다.
  //                  if ( bPiece && !bTryMakeTexture )
  //                  {
  //                      dwStartTextureSize = dwTextureSize;
  //                  }
  //              }
  //          }
  //      }

  //      if ( !bPiece && dwFaceNUM>1000 )
  //      {
  //          ExportProgress::SetStateString( "텍셀 계산 중..." );
  //      }

  //      DWORD dwTestColor = 0xff000000 + (rand()%0xffffff);

  //      // 그룹별로 LightMapObject 를 만들도록 한다.
  //      {
  //          for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
  //          {
  //              // 텍셀을 만들고 계산하고..
  //              vecLightMapObjectDest[i]->CreateTexel
		//		( 
		//			pStaticMesh, 
		//			pPieceManager, 
		//			matWorld, 
		//			FALSE, 
		//			dwTestColor, 
		//			emLightMapType,
		//			bDontMakeShadowLM, 
		//			NULL, 
		//			g_nSampling,
		//			NULL
		//		);
  //          }
  //      }

  //      PROCESS_MEMORY_COUNTERS MemoryCounter;
  //      HANDLE hProcess = GetCurrentProcess();
  //      GetProcessMemoryInfo( hProcess, &MemoryCounter, sizeof(MemoryCounter) );
  //      SIZE_T nWorkingSetSize = MemoryCounter.WorkingSetSize / (1024*1024);
  //      if ( nWorkingSetSize > 800 )  // 800MB 가 넘어가면 텍스쳐 정리
  //      {
  //          // 800MB 이하로 줄인다.
  //          BOOL bEnableTexture(TRUE);
  //          while ( nWorkingSetSize > 800 && bEnableTexture )
  //          {
  //              CreateTextureDataLowerEmpty( pd3dDevice, strLightMapName, strAddName, bAlphaChange, bPiece, FALSE );

  //              bEnableTexture = FALSE;
  //              for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
  //              {
  //                  // 텍스쳐가 만들어진게 없어야 한다.
  //                  if ( !m_vecTextureData[ i ]->m_bMakeTexture )
  //                      bEnableTexture = TRUE;
  //              }

  //              hProcess = GetCurrentProcess();
  //              GetProcessMemoryInfo( hProcess, &MemoryCounter, sizeof(MemoryCounter) );
  //              nWorkingSetSize = MemoryCounter.WorkingSetSize / (1024*1024);
  //          }
  //      }

  //      return TRUE;
  //  }

	BOOL CreateFieldLightMapUserUV( LPDIRECT3DDEVICEQ pd3dDevice, 
									const TSTRING& strLightMapName, 
									const TSTRING& strAddName, 
									BOOL bAlphaChange,
									VEC_LIGHTMAPOBJ& vecLightMapObjectDest,
									const std::vector<WORDWORD>& vecTriangle_wwLightID,
									DWORD dwFaceNUM, 
									BYTE* pVerticesSrc, 
									DWORD dwFVF,
									DxStaticMeshCollBase* pStaticMesh,
									DxPieceManagerPSF* pPieceManager,
									const D3DXMATRIX& matWorld,
									const std::vector<BOOL>& vecReceiveShadowLM,
									DWORD dwTextureID,
									WORDWORD wwLightMapUserUV_TEXEL,
									LIGHTMAPUV* pLightMapUV )
	{
		g_pd3dDevice = pd3dDevice;

		vecLightMapObjectDest.clear();

		if ( dwFaceNUM == 0 )
			return TRUE;

		if ( dwFaceNUM>1000 )
		{
			ExportProgress::SetStateString( "dwFaceNUM>1000" );
			ExportProgress::SetStateString( "인접한 삼각형 별로 분리,셋팅.1" );
		}

		// 모든 삼각형을 붙인다.
		{    
			VEC_FACE_ID vecFaceGroupID;     // 각 Face 별로 어느 그룹에 속해 있는지 셋팅한다.
			DWORD       dwMeshGroup(0);     // 인접 삼각형 그룹
			VEC_FACE_ID vecFaceID;          // 그룹 선택이 바뀌면 clear 된다.

			// FaceGroup을 만든다. - 현재는 그룹을 한개만 만든다.
			CreateFaceGroup( vecFaceGroupID, dwMeshGroup, vecFaceID, dwFaceNUM, pVerticesSrc, dwFVF );

			// 그룹별로 LightMapObject 를 만들도록 한다.
			{
				DWORD i=0;
				//for ( DWORD i=0; i<(dwMeshGroup+1); ++i )
				{
					std::tr1::shared_ptr<LightMapObject> spNew = std::tr1::shared_ptr<LightMapObject>(new LightMapObject);

					//if ( dwFVF == VERTEXNORTEX2::FVF )
					//{
					//	VERTEXNORTEX2* pVertices = reinterpret_cast<VERTEXNORTEX2*>( pVerticesSrc );

					//	// 버텍스를 추가한다.
					//	for ( DWORD j=0; j<dwFaceNUM; ++j )
					//	{
					//		if ( i == vecFaceGroupID[ j ] )
					//		{
					//			spNew->AddThreeVerties( &pVertices[j*3+0], NSLIGHTMAPDEF::LMT_UV, j );
					//		}
					//	}
					//}
					//else if ( dwFVF == VERTEX_WORLD::FVF )
					if ( dwFVF == VERTEX_WORLD::FVF )
					{
						VERTEX_WORLD* pVertices = reinterpret_cast<VERTEX_WORLD*>( pVerticesSrc );

						// 버텍스를 추가한다.
						for ( DWORD j=0; j<dwFaceNUM; ++j )
						{
							if ( i == vecFaceGroupID[ j ] )
							{
								spNew->AddThreeVerties( &pVertices[j*3+0], j );
							}
						}
					}

					// 텍셀 크기, 텍스쳐ID를 셋팅한다.
					spNew->SetBaseTexelXY( pLightMapUV->m_wwBaseTexelXY );
					spNew->SetTexelScale( pLightMapUV->m_wwTextureSize );
					spNew->SetLightMapUV( pLightMapUV->m_vecUV, wwLightMapUserUV_TEXEL );
					spNew->SetTextureID( dwTextureID );
					spNew->SetTriangle_vecLightID( vecTriangle_wwLightID );

					// 삽입한다.
					vecLightMapObjectDest.push_back( spNew );					
				}
			}
		}

		if ( dwTextureID >= m_vecTextureData.size() )
		{
			std::tr1::shared_ptr<TextureData> spNew( std::tr1::shared_ptr<TextureData>(new TextureData(wwLightMapUserUV_TEXEL.wA,wwLightMapUserUV_TEXEL.wB)) );
			spNew->m_dwTexelEmptyNum = 0;
			for ( DWORD y=0; y<spNew->m_dwTexHeight; ++y )
			{
				for ( DWORD z=0; z<spNew->m_dwTexWidth; ++z )
				{
					spNew->m_pTexelLast[y*spNew->m_dwTexWidth+z] = static_cast<WORD>(spNew->m_dwTexWidth-1);
					spNew->m_pbTexelState[y*spNew->m_dwTexWidth+z] = false;
				}
			}
			m_vecTextureData.push_back( spNew );
		}

		if ( dwFaceNUM>1000 )
		{
			ExportProgress::SetStateString( "텍셀 계산 중..." );
		}

		// 그룹별로 LightMapObject 를 만들도록 한다.
		{
			for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
			{
				vecLightMapObjectDest[i]->CreateTexelTriangleID();
				{
					BOOL bEnoughUsedPixelLightMap(TRUE);

					// 텍셀을 만들고 계산하고..
					vecLightMapObjectDest[i]->CreateTexel
					( 
						pd3dDevice, 
						pStaticMesh, 
						pPieceManager, 
						matWorld, 
						FALSE, 
						&vecReceiveShadowLM, 
						g_nSampling,
						&wwLightMapUserUV_TEXEL,
						NULL,
						bEnoughUsedPixelLightMap
					);
				}
				vecLightMapObjectDest[i]->DeleteTexelTriangleID();
			}
		}

		return TRUE;
	}

	BOOL CreatePieceLightMapUserUV( LPDIRECT3DDEVICEQ pd3dDevice, 
									const TSTRING& strLightMapName, 
									const TSTRING& strAddName, 
									BOOL bAlphaChange,
									BOOL bPiece,
									VEC_LIGHTMAPOBJ& vecLightMapObjectDest,
									DWORD dwFaceNUM, 
									BYTE* pVerticesSrc, 
									DWORD dwFVF,
									DxStaticMeshCollBase* pStaticMesh,
									DxPieceManagerPSF* pPieceManager,
									const D3DXMATRIX& matWorld,
									DWORD& dwStartTextureSize,
									BOOL& bEnoughUsedPixelLightMap,
									const std::vector<BOOL>& vecReceiveShadowLM,
									WORDWORD wwTexelSize,
									const std::vector<D3DXVECTOR2>& vecUV,
									const WORDWORD& wwLightID4Piece )
	{
		g_pd3dDevice = pd3dDevice;

		vecLightMapObjectDest.clear();

		if ( dwFaceNUM == 0 )
			return TRUE;

		// 텍스쳐 최대 사이즈는 2048를 넘을 수 없다.
		if ( dwStartTextureSize > ENUM_TEXTURE_PIXEL_SIZE )
			dwStartTextureSize = ENUM_TEXTURE_PIXEL_SIZE;

		if ( !bPiece && dwFaceNUM>1000 )
		{
			ExportProgress::SetStateString( "dwFaceNUM>1000" );
			ExportProgress::SetStateString( "인접한 삼각형 별로 분리,셋팅.1" );
		}

		// 모든 삼각형을 붙인다.
		{    
			VEC_FACE_ID vecFaceGroupID;     // 각 Face 별로 어느 그룹에 속해 있는지 셋팅한다.
			DWORD       dwMeshGroup(0);     // 인접 삼각형 그룹
			VEC_FACE_ID vecFaceID;          // 그룹 선택이 바뀌면 clear 된다.

			// FaceGroup을 만든다. - 현재는 그룹을 한개만 만든다.
			CreateFaceGroup( vecFaceGroupID, dwMeshGroup, vecFaceID, dwFaceNUM, pVerticesSrc, dwFVF );

			// 그룹별로 LightMapObject 를 만들도록 한다.
			{
				DWORD i=0;
				//for ( DWORD i=0; i<(dwMeshGroup+1); ++i )
				{
					std::tr1::shared_ptr<LightMapObject> spNew = std::tr1::shared_ptr<LightMapObject>(new LightMapObject);

					if ( dwFVF == VERTEXNORCOLORTEX4::FVF )
					{
						VERTEXNORCOLORTEX4* pVertices = reinterpret_cast<VERTEXNORCOLORTEX4*>( pVerticesSrc );

						// 버텍스를 추가한다.
						for ( DWORD j=0; j<dwFaceNUM; ++j )
						{
							if ( i == vecFaceGroupID[ j ] )
							{
								spNew->AddThreeVerties( &pVertices[j*3+0], j );
							}
						}
					}

					spNew->SetLightMapUV( vecUV, wwTexelSize );

					// 삽입한다.
					vecLightMapObjectDest.push_back( spNew );					
				}
			}
		}

		BOOL bTryMakeTexture(TRUE);
		while ( bTryMakeTexture )
		{
			bTryMakeTexture = FALSE;

			if ( !bPiece && dwFaceNUM>1000 )
			{
				ExportProgress::SetStateString( "최소화 가능한 Pixel 검사중." );
			}

			// 그룹별로 LightMapObject 를 만들도록 한다.
			{
				for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
				{
					// 텍셀 크기를 셋팅한다.
					vecLightMapObjectDest[i]->SetTexelScale( wwTexelSize );
				}
			}

			if ( !bPiece && dwFaceNUM>1000 )
			{
				ExportProgress::SetStateString( "알맞은 텍스쳐를 구한다...." );
			}

			// 알맞은 텍스쳐를 구한다.
			{
				DWORD dwActiveSlot(0L);
				for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
				{
					LightMapObject* pLightMapObject = vecLightMapObjectDest[i].get();            
					dwActiveSlot += (pLightMapObject->GetTexelSizeX()) * (pLightMapObject->GetTexelSizeY());
				}

				BOOL bActiveTexture(FALSE);

				// 얻어온 Index 만 검사를 시작한다.
				// 큰텍스쳐 부터 데이터를 넣도록 작업한다.
				TextureData sTextureData(g_dwBaseWidth,g_dwBaseWidth);
				for ( DWORD t=ENUM_TEXTURE_PIXEL_SIZE; t>=g_dwBaseWidth; t/=2 )
				{
					for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
					{
						// 사이즈가 같은 것만 검사한다.
						if ( m_vecTextureData[i]->m_dwTexWidth != t )
							continue;

						// Piece 의 경우는 사이즈가 동일해야만 그 텍스쳐에 넣을 수 있다.
						if ( bPiece )
						{
							if ( dwStartTextureSize != m_vecTextureData[ i ]->m_dwTexWidth )
								continue;
						}

						// 텍스쳐가 만들어진 상태이다.
						if ( m_vecTextureData[ i ]->m_bMakeTexture )
							continue;

						// 텍스쳐에 사용 가능한 공간이 없으면 Path
						if ( dwActiveSlot > m_vecTextureData[ i ]->m_dwTexelEmptyNum )
						{
							continue;
						}

						// 데이터를 복제한다.
						sTextureData.Import( *m_vecTextureData[ i ] );

						// 알맞은 텍스쳐를 구하고, 데이터를 Change 한다.
						if ( EnableInsertTexel( vecLightMapObjectDest, sTextureData, i, TRUE ) )
						{
							t = 0;
							bActiveTexture = TRUE;
							m_vecTextureData[ i ]->Import( sTextureData );
							m_vecTextureData[ i ]->m_dwTexelEmptyNum -= dwActiveSlot; 
							break;
						}
					}
				}

				// 활성화 된 Texture가 없을 경우 새로 생성해준다.
				if ( !bActiveTexture )
				{
					// 기본적인 텍스쳐 사이즈를 셋팅함
					DWORD dwTextureSize = dwStartTextureSize;

					// 그려야 할 곳이 그려야하는 크기보다 적다면 사이즈를 늘린다.
					{
						if ( dwTextureSize*dwTextureSize < dwActiveSlot )
						{
							dwTextureSize *= 2;

							if ( dwTextureSize*dwTextureSize < dwActiveSlot )
							{
								dwTextureSize *= 2;

								if ( dwTextureSize*dwTextureSize < dwActiveSlot )
								{
									dwTextureSize *= 2;
								}
							}
						}

						if ( dwTextureSize > ENUM_TEXTURE_PIXEL_SIZE )
						{
							// 작업이 불가능하다.
							bTryMakeTexture = TRUE;
						}
					}

					for( DWORD i=0; i<3 && dwTextureSize <= ENUM_TEXTURE_PIXEL_SIZE; ++i )
					{
						TextureData sTextureData(dwTextureSize,dwTextureSize);

						// 알맞은 텍스쳐를 구하고, 데이터를 Change 한다.
						if ( EnableInsertTexel( vecLightMapObjectDest, sTextureData, static_cast< DWORD >( m_vecTextureData.size() ), TRUE ) )
						{
							std::tr1::shared_ptr<TextureData> spNew( std::tr1::shared_ptr<TextureData>(new TextureData(dwTextureSize,dwTextureSize)) );
							spNew->Import( sTextureData );
							spNew->m_dwTexelEmptyNum -= dwActiveSlot;
							m_vecTextureData.push_back( spNew );
							break;
						}

						// 안되면 더 큰 사이즈의 텍스쳐로 작업해 본다.
						dwTextureSize *= 2;
					}

					if ( dwTextureSize > ENUM_TEXTURE_PIXEL_SIZE )
					{
						// 작업이 불가능하다.
						bTryMakeTexture = TRUE;
					}

					// Piece 의 경우는 단위가 한번 높아지면 높아진 단위로 유지한다.
					// 성공을 했을때만 셋팅을 한다.
					if ( bPiece && !bTryMakeTexture )
					{
						dwStartTextureSize = dwTextureSize;
					}
				}
			}
		}

		if ( !bPiece && dwFaceNUM>1000 )
		{
			ExportProgress::SetStateString( "텍셀 계산 중..." );
		}

		// 그룹별로 LightMapObject 를 만들도록 한다.
		{
			for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
			{
				vecLightMapObjectDest[i]->CreateTexelTriangleID();
				{
					// 텍셀을 만들고 계산하고..
					vecLightMapObjectDest[i]->CreateTexel
					( 
						pd3dDevice, 
						pStaticMesh, 
						pPieceManager, 
						matWorld, 
						FALSE, 
						&vecReceiveShadowLM, 
						g_nSampling,
						NULL,
						&wwLightID4Piece,
						bEnoughUsedPixelLightMap
					);
				}
				vecLightMapObjectDest[i]->DeleteTexelTriangleID();
			}
		}

		//PROCESS_MEMORY_COUNTERS MemoryCounter;
		//HANDLE hProcess = GetCurrentProcess();
		//GetProcessMemoryInfo( hProcess, &MemoryCounter, sizeof(MemoryCounter) );
		//SIZE_T nWorkingSetSize = MemoryCounter.WorkingSetSize / (1024*1024);
		//if ( nWorkingSetSize > 800 )  // 800MB 가 넘어가면 텍스쳐 정리
		//{
		//	// 800MB 이하로 줄인다.
		//	BOOL bEnableTexture(TRUE);
		//	while ( nWorkingSetSize > 800 && bEnableTexture )
		//	{
		//		CreateTextureDataLowerEmpty( pd3dDevice, strLightMapName, strAddName, bAlphaChange, bPiece, TRUE );

		//		bEnableTexture = FALSE;
		//		for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
		//		{
		//			// 텍스쳐가 만들어진게 없어야 한다.
		//			if ( !m_vecTextureData[ i ]->m_bMakeTexture )
		//				bEnableTexture = TRUE;
		//		}

		//		hProcess = GetCurrentProcess();
		//		GetProcessMemoryInfo( hProcess, &MemoryCounter, sizeof(MemoryCounter) );
		//		nWorkingSetSize = MemoryCounter.WorkingSetSize / (1024*1024);
		//	}
		//}

		return TRUE;
	}

	void OneUseLineBlock3( TextureData* pTextureData, DWORD y )
	{
		for ( DWORD z=0; z<pTextureData->m_dwTexWidth; ++z )
		{
			pTextureData->m_pTexelLast[y*pTextureData->m_dwTexWidth+z] = static_cast<WORD>(pTextureData->m_dwTexWidth-1);
			pTextureData->m_pbTexelState[y*pTextureData->m_dwTexWidth+z] = false;
		}
	}

    void OneUseLineBlock2( TextureData* pTextureData )
    {
        for ( DWORD y=0; y<pTextureData->m_dwTexHeight; ++y )
        {
            for ( DWORD x=0; x<pTextureData->m_dwTexWidth; ++x )
            {
                if ( pTextureData->m_pbTexelState && !pTextureData->m_pbTexelState[y*pTextureData->m_dwTexWidth+x] )
                {
					OneUseLineBlock3( pTextureData, y );
                    break;
                }
            }
        }
    }

    // 1개의 Width 라도 사용된다면 그 라인은 Block 한다.
    void OneUseLineBlock()
    {
        for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
        {
            // 알맞은 텍스쳐를 구하고, 데이터를 Change 한다.
            OneUseLineBlock2( m_vecTextureData[i].get() );
        }
    }

	// 모든 라인을 블럭한다.
	void AllLineBlock()
	{
		for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
		{
			for ( DWORD y=0; y<m_vecTextureData[i].get()->m_dwTexHeight; ++y )
			{
				OneUseLineBlock3( m_vecTextureData[i].get(), y );
			}
		}
	}

    // LightMap Texture를 Resize 한다
    void ResizeLightMapTexture()
    {
        ExportProgress::SetStateString( "텍스쳐 사이즈 줄이기" );

        // Note : 최소로 줄일 수 있는 사이즈를 확인하고 텍스쳐만 일단 셋팅한다.
        for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
        {
            if ( !m_vecTextureData[i]->m_pbTexelState )
                continue;
            
            NSCOMBINETEXTOOLS::SetFitHeight
            ( 
                m_vecTextureData[i]->m_pbTexelState, 
                m_vecTextureData[i]->m_dwTexWidth, 
                m_vecTextureData[i]->m_dwTexHeight
            );
        }
    }

    // Texture 사이즈를 얻어온다.
    WORDWORD GetTextureSize( VEC_LIGHTMAPOBJ& vecLightMapObjectDest )
    {
        if ( vecLightMapObjectDest.empty() )
            return WORDWORD(0,0);

        DWORD dwTextureID = vecLightMapObjectDest[0]->GetTextureID();

        if ( dwTextureID >= m_vecTextureData.size() )
            return WORDWORD(0,0);

        return WORDWORD( static_cast<WORD>(m_vecTextureData[ dwTextureID ]->m_dwTexWidth), 
                        static_cast<WORD>(m_vecTextureData[ dwTextureID ]->m_dwTexHeight) );
    }

    DWORD GetLightMapTextureWidth()
    {
        if ( m_vecTextureData.size() != 1 )
            return 0;

        return m_vecTextureData[0]->m_dwTexWidth;
    }

    DWORD GetLightMapTextureHeight()
    {
        if ( m_vecTextureData.size() != 1 )
            return 0;

        return m_vecTextureData[0]->m_dwTexHeight;
    }

    // 
    void CreateLightMap2_Detail( LightMapObject* pLightMapObject,
								DWORD dwTextureID,
                                BOOL bAlphaChange,
                                BOOL bPiece,
                                WORDWORD wwBaseTexelXY,
                                BOOL bCreateTextureDataLowerEmpty,
								BOOL bUserUV )
    {
        if ( dwTextureID >= m_vecTextureData.size() )
        {
            return;
        }

        if ( pLightMapObject->IsGaussBlurSetUV() )
        {
            return;
        }

        DWORD dwTexWidth = m_vecTextureData[ dwTextureID ]->m_dwTexWidth;
        DWORD dwTexHeight = m_vecTextureData[ dwTextureID ]->m_dwTexHeight;

        // 어느 사이즈의 텍스쳐를 쓰는지 알아놓는다. - Tile,Ground를 위함.
        pLightMapObject->SetTextureSizeXY( WORDWORD( static_cast<WORD>(dwTexWidth), static_cast<WORD>(dwTexHeight) ) );
       
        // Note : 실제로 텍스쳐에 그리기를 시작한다.
        if ( !m_vecTextureData[ dwTextureID ]->m_bMakeTexture )
        {
            // TexelColor 셋팅
            m_vecTextureData[ dwTextureID ]->CreateTexelColor();

            if ( bCreateTextureDataLowerEmpty )
            {
            //    ExportProgress::SetStateString( "부위별 텍셀정보 저장" );
            }

            //------- 각각 위치에 기입한다. #2
            {
                DWORD* pTexelDest[3] = { m_vecTextureData[ dwTextureID ]->m_pTexelLastDay, 
                                        m_vecTextureData[ dwTextureID ]->m_pTexelLastDirect_Shadow,
										m_vecTextureData[ dwTextureID ]->m_pTexelLastIndirect };

                DWORD* pTexelCombined		= pLightMapObject->GetTexelColorCombined();
                DWORD* pTexelDirect_Shadow  = pLightMapObject->GetTexelColorDirect_Shadow();
				DWORD* pTexelIndirect		= pLightMapObject->GetTexelColorIndirect();

                DWORD* pTexel[3] = { pTexelCombined, pTexelDirect_Shadow, pTexelIndirect };
                
                //------- Texel 기록
                for ( DWORD i=0; i<3; ++i )
                {
                    // Tiling 에서 쓰인다.
                    if ( bAlphaChange )
                    {
                        int nAlpha;
                        DWORD dwColor;
                        for ( DWORD j=0; j<pLightMapObject->GetTexelSizeX()*pLightMapObject->GetTexelSizeY(); ++j )
                        {
                            dwColor = (pTexel[i][j]&0xffffff);
                            nAlpha = (pTexel[i][j]>>24);
                            nAlpha = static_cast<int>(nAlpha*1.2f);
                            nAlpha = nAlpha - static_cast<int>(255*0.2f);
                            if ( nAlpha > 255 )    nAlpha = 255;
                            if ( nAlpha < 0 )      nAlpha = 0;
                            pTexel[i][j] = (nAlpha<<24) + dwColor;
                        }
                    }

					if ( bUserUV )
					{
						// Note : 실제 텍스쳐에 그린다.
						NSCOMBINETEXTOOLS::DrawTexel_Tex2Array
						( 
							pTexelDest[i], 
							dwTexWidth, 
							pTexel[i],
							pLightMapObject->GetBaseTexelXY(),
							pLightMapObject->GetTexelSizeX(),
							pLightMapObject->GetTexelSizeY(),
							0,
							0
						);
					}
					else
					{
						// Note : 실제 텍스쳐에 그린다.
						NSCOMBINETEXTOOLS::DrawTexel_Tex2Array
						( 
							pTexelDest[i], 
							dwTexWidth, 
							pTexel[i],
							pLightMapObject->GetBaseTexelXY(),
							pLightMapObject->GetTexelSizeX(),
							pLightMapObject->GetTexelSizeY(),
							1,
							1
						);
					}
                }
            }

            if ( bCreateTextureDataLowerEmpty )
            {
            //    ExportProgress::SetStateString( "좌표 재 계산" );
            }

            {
                //------- UV 좌표를 재계산한다.
                {
					if ( !bUserUV )
					{
						pLightMapObject->CaleUV( 1, 1 );
					}

                    // Piece 일 경우는 - 연산을 더 해야 한다.
                    if ( bPiece )
                    {
                        pLightMapObject->SubUV( wwBaseTexelXY );
                    }
                }
            }
        }
    }

    // 
    void CreateLightMap2_PIECE( VEC_LIGHTMAPOBJ& vecLightMapObjectDest,
								const TSTRING& strLightMapName, 
								const TSTRING& strAddName, 
								BOOL bAlphaChange,
								BOOL bPiece,
								TSTRING& strTextureCombined, 
								TSTRING& strTextureDirect_Shadow_1,
								TSTRING& strTextureDirect_Shadow_2,
								TSTRING& strTextureIndirect,
								WORDWORD wwBaseTexelXY,
								BOOL bCreateTextureDataLowerEmpty,
								BOOL bUserUV )
    {
        if ( vecLightMapObjectDest.empty() )
            return;

        DWORD dwTextureID = vecLightMapObjectDest[0]->GetTextureID();

		if ( dwTextureID >= m_vecTextureData.size() )
			return;

        for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
        {
            LightMapObject* pLightMapObject = vecLightMapObjectDest[i].get();

			CreateLightMap2_Detail( pLightMapObject,
									dwTextureID,
									bAlphaChange,
									bPiece,
									wwBaseTexelXY,
									FALSE,
									bUserUV );
        }

        //if ( bCreateTextureDataLowerEmpty )
        //{
        //    PROCESS_MEMORY_COUNTERS MemoryCounter;
        //    HANDLE hProcess = GetCurrentProcess();
	       // GetProcessMemoryInfo( hProcess, &MemoryCounter, sizeof(MemoryCounter) );
        //    SIZE_T nWorkingSetSize = MemoryCounter.WorkingSetSize / (1024*1024);
        //    if ( nWorkingSetSize > 800 )  // 800MB 가 넘어가면 텍스쳐 정리
        //    {
        //        CreateTextureDataLowerEmpty( g_pd3dDevice, strLightMapName, strAddName, bAlphaChange, bPiece, bUserUV );
        //    }
        //}

        // Note : 텍스쳐 정보를 얻는다.
        {
            int nTexCount = dwTextureID;

            int n100 = nTexCount/100;
            int n10 = (nTexCount-n100*100) / 10;
            int n1 = nTexCount%10;
            TCHAR c100 = (TCHAR)(n100+48);
            TCHAR c10 = (TCHAR)(n10+48);
            TCHAR c1 = (TCHAR)(n1+48);
            strTextureCombined = _T("");
            strTextureDirect_Shadow_1 = _T("");
			strTextureDirect_Shadow_2 = _T("");
            strTextureIndirect = _T("");
            strTextureCombined = strTextureCombined + strLightMapName + strAddName + _T("_C_") + c100 + c10 + c1 + _T(".dds");

			// [CascadeShadow] 작업보류중
            strTextureDirect_Shadow_1 = strTextureDirect_Shadow_1 + strLightMapName + strAddName + _T("_DS1_") + c100 + c10 + c1 + _T(".dds");
			strTextureDirect_Shadow_2 = strTextureDirect_Shadow_2 + strLightMapName + strAddName + _T("_DS2_") + c100 + c10 + c1 + _T(".dds");
            strTextureIndirect = strTextureIndirect + strLightMapName + strAddName + _T("_I_") + c100 + c10 + c1 + _T(".dds");

            vecLightMapObjectDest[0]->SetLightMapName( strTextureCombined, strTextureDirect_Shadow_1, strTextureDirect_Shadow_2, strTextureIndirect );
        }
    }

	// 
	void CreateLightMap2_STATICMESH( VEC_LIGHTMAPOBJ& vecLightMapObjectDest,
									const TSTRING& strLightMapName, 
									const TSTRING& strAddName, 
									BOOL bAlphaChange,
									BOOL bPiece,
									TSTRING& strTextureCombined,
									TSTRING& strTextureDirect_Shadow_1,
									TSTRING& strTextureDirect_Shadow_2,
									TSTRING& strTextureIndirect,
									WORDWORD wwBaseTexelXY )
	{
		if ( vecLightMapObjectDest.empty() )
			return;

		DWORD dwTextureID = vecLightMapObjectDest[0]->GetTextureID();

		for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
		{
			LightMapObject* pLightMapObject = vecLightMapObjectDest[i].get();

			CreateLightMap2_Detail( pLightMapObject,
									dwTextureID,
									bAlphaChange,
									bPiece,
									wwBaseTexelXY,
									FALSE,
									TRUE );
		}

		// Note : 텍스쳐 정보를 얻는다.
		{
			int nTexCount = dwTextureID;

			int n100 = nTexCount/100;
			int n10 = (nTexCount-n100*100) / 10;
			int n1 = nTexCount%10;
			TCHAR c100 = (TCHAR)(n100+48);
			TCHAR c10 = (TCHAR)(n10+48);
			TCHAR c1 = (TCHAR)(n1+48);
			strTextureCombined = _T("");
			strTextureDirect_Shadow_1 = _T("");
			strTextureDirect_Shadow_2 = _T("");
			strTextureIndirect = _T("");
			strTextureCombined = strTextureCombined + strLightMapName + strAddName + _T("_C_") + c100 + c10 + c1 + _T(".dds");

			// [CascadeShadow] 작업보류중
			strTextureDirect_Shadow_1 = strTextureDirect_Shadow_1 + strLightMapName + strAddName + _T("_DS1_") + c100 + c10 + c1 + _T(".dds");
			strTextureDirect_Shadow_2 = strTextureDirect_Shadow_2 + strLightMapName + strAddName + _T("_DS2_") + c100 + c10 + c1 + _T(".dds");
			strTextureIndirect = strTextureIndirect + strLightMapName + strAddName + _T("_I_") + c100 + c10 + c1 + _T(".dds");

			vecLightMapObjectDest[0]->SetLightMapName( strTextureCombined, strTextureDirect_Shadow_1, strTextureDirect_Shadow_2, strTextureIndirect );
		}
	}

    void CreateLightMapFinal_PIECE( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strLightMapName, const TSTRING& strAddName, int nBackUpNum, const TSTRING& strFinalName )  // D3DFMT_DXT1
    {
        for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
        {
            CreateLightMapFinal( pd3dDevice, i, strLightMapName, strAddName, i, nBackUpNum, strFinalName );
        }
    }

	void CreateLightMapFinal_STATICMESH( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strLightMapName, const TSTRING& strAddName, DWORD dwTextureID, int nBackUpNum, const TSTRING& strFinalName )
	{
		if ( m_vecTextureData.empty() )
			return;

		CreateLightMapFinal( pd3dDevice, dwTextureID, strLightMapName, strAddName, dwTextureID, nBackUpNum, strFinalName );
	}

    //
    void CalcMinMax( D3DXVECTOR2& vMin, D3DXVECTOR2& vMax, const D3DXVECTOR2& vSrc )
	{
		if( vMin.x > vSrc.x )	vMin.x = vSrc.x;
		if( vMin.y > vSrc.y )	vMin.y = vSrc.y;
		if( vMax.x < vSrc.x )	vMax.x = vSrc.x;
		if( vMax.y < vSrc.y )	vMax.y = vSrc.y;
	}

    //
    void CalcMinMax( D3DXVECTOR3& vMin, D3DXVECTOR3& vMax, const D3DXVECTOR3& vSrc )
	{
		if( vMin.x > vSrc.x )	vMin.x = vSrc.x;
		if( vMin.y > vSrc.y )	vMin.y = vSrc.y;
		if( vMin.z > vSrc.z )	vMin.z = vSrc.z;
		if( vMax.x < vSrc.x )	vMax.x = vSrc.x;
		if( vMax.y < vSrc.y )	vMax.y = vSrc.y;
		if( vMax.z < vSrc.z )	vMax.z = vSrc.z;
	}

	// Texel 정보를 모으는 작업을 한다.
	void CreateLightMapInRadiosity_Detail( LightMapObject* pLightMapObject, DWORD dwTextureID )
	{
		if ( dwTextureID >= m_vecTextureData.size() )
		{
			return;
		}

		// Radiosity 에서 또다시 텍스쳐를 만들어야 되어서, FALSE 를 준다.
		m_vecTextureData[ dwTextureID ]->m_bMakeTexture = FALSE;

		DWORD dwTexWidth = m_vecTextureData[ dwTextureID ]->m_dwTexWidth;
		DWORD dwTexHeight = m_vecTextureData[ dwTextureID ]->m_dwTexHeight;

		// Note : 실제로 텍스쳐에 그리기를 시작한다.
		{
			{
				ExportProgress::SetStateString( _T("부위별 텍셀정보 저장") );
			}

			//------- 각각 위치에 기입한다. #2
			{
				DWORD* pTexelDest[3] = { m_vecTextureData[ dwTextureID ]->m_pTexelLastDay, 
											m_vecTextureData[ dwTextureID ]->m_pTexelLastDirect_Shadow,
											m_vecTextureData[ dwTextureID ]->m_pTexelLastIndirect };

				DWORD* pTexelCombined		= pLightMapObject->GetTexelColorCombined();
				DWORD* pTexelDirect_Shadow  = pLightMapObject->GetTexelColorDirect_Shadow();
				DWORD* pTexelIndirect		= pLightMapObject->GetTexelColorIndirect();
				DWORD* pTexelCombinedRAD	= pLightMapObject->GetTexelColorCombinedRAD();
				DWORD* pTexelNoDynamicLight	= pLightMapObject->GetTexelColorNoDynamicLight();

				//DWORD* pTexel[3] = { pTexelCombined, pTexelDirect_Shadow, pTexelIndirect };

				DWORD dwTexelSizeX = pLightMapObject->GetTexelSizeX();
				DWORD dwTexelSizeY = pLightMapObject->GetTexelSizeY();

				D3DXVECTOR3 vSrc;
				D3DXVECTOR3 vDest;

				for ( DWORD y=0; y<dwTexelSizeY; ++y )
				{
					for ( DWORD x=0; x<dwTexelSizeX; ++x )
					{
						vDest.x = static_cast<float>( (pTexelIndirect[dwTexelSizeX*y+x] & 0xff0000) >> 16 );
						vDest.y = static_cast<float>( (pTexelIndirect[dwTexelSizeX*y+x] & 0xff00) >> 8 );
						vDest.z = static_cast<float>( (pTexelIndirect[dwTexelSizeX*y+x] & 0xff) );

						vSrc.x = static_cast<float>( (pTexelCombined[dwTexelSizeX*y+x] & 0xff0000) >> 16 );
						vSrc.y = static_cast<float>( (pTexelCombined[dwTexelSizeX*y+x] & 0xff00) >> 8 );
						vSrc.z = static_cast<float>( (pTexelCombined[dwTexelSizeX*y+x] & 0xff) );

						vDest += vSrc;

						if ( vDest.x > 255 )	vDest.x = 255;
						if ( vDest.y > 255 )	vDest.y = 255;
						if ( vDest.z > 255 )	vDest.z = 255;

						pTexelCombinedRAD[dwTexelSizeX*y+x] = 0xff000000 + (static_cast<DWORD>(vDest.x)<<16) + (static_cast<DWORD>(vDest.y)<<8) + static_cast<DWORD>(vDest.z);
					}
				}

				for ( DWORD y=0; y<dwTexelSizeY; ++y )
				{
					for ( DWORD x=0; x<dwTexelSizeX; ++x )
					{
						vDest.x = static_cast<float>( (pTexelIndirect[dwTexelSizeX*y+x] & 0xff0000) >> 16 );
						vDest.y = static_cast<float>( (pTexelIndirect[dwTexelSizeX*y+x] & 0xff00) >> 8 );
						vDest.z = static_cast<float>( (pTexelIndirect[dwTexelSizeX*y+x] & 0xff) );

						vSrc.x = static_cast<float>( (pTexelNoDynamicLight[dwTexelSizeX*y+x] & 0xff0000) >> 16 );
						vSrc.y = static_cast<float>( (pTexelNoDynamicLight[dwTexelSizeX*y+x] & 0xff00) >> 8 );
						vSrc.z = static_cast<float>( (pTexelNoDynamicLight[dwTexelSizeX*y+x] & 0xff) );

						vDest += vSrc;

						if ( vDest.x > 255 )	vDest.x = 255;
						if ( vDest.y > 255 )	vDest.y = 255;
						if ( vDest.z > 255 )	vDest.z = 255;

						pTexelIndirect[dwTexelSizeX*y+x] = 0xff000000 + (static_cast<DWORD>(vDest.x)<<16) + (static_cast<DWORD>(vDest.y)<<8) + static_cast<DWORD>(vDest.z);
					}
				}

				//------- Texel 기록
				{
					// Note : 실제 텍스쳐에 그린다.
					NSCOMBINETEXTOOLS::DrawTexel_Tex2Array
					( 
						pTexelDest[0], 
						dwTexWidth, 
						pTexelCombinedRAD,
						pLightMapObject->GetBaseTexelXY(),
						pLightMapObject->GetTexelSizeX(),
						pLightMapObject->GetTexelSizeY(),
						0,
						0
					);
				}

				//------- Texel 기록
				{
					// Note : 실제 텍스쳐에 그린다.
					NSCOMBINETEXTOOLS::DrawTexel_Tex2Array
					( 
						pTexelDest[2], 
						dwTexWidth, 
						pTexelIndirect,
						pLightMapObject->GetBaseTexelXY(),
						pLightMapObject->GetTexelSizeX(),
						pLightMapObject->GetTexelSizeY(),
						0,
						0
					);
				}
			}
		}
	}

	// Texel 정보를 모으는 작업을 한다.
	void CreateLightMapInRadiosity( VEC_LIGHTMAPOBJ& vecLightMapObjectDest )
	{
		if ( vecLightMapObjectDest.empty() )
			return;

		DWORD dwTextureID = vecLightMapObjectDest[0]->GetTextureID();

		if ( dwTextureID >= m_vecTextureData.size() )
			return;

		for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
		{
			LightMapObject* pLightMapObject = vecLightMapObjectDest[i].get();
			CreateLightMapInRadiosity_Detail( pLightMapObject, dwTextureID );
		}
	}

	// Piece Object 만 작업된다.
	// Radiosity 를 굽는다.
	void BakeRadiosityForPiece( LPDIRECT3DDEVICEQ pd3dDevice, 
		DxLandMan* pLandMan, 
		VEC_LIGHTMAPOBJ& vecLightMapObjectDest, 
		const D3DXMATRIX& matWorld, 
		LPDIRECT3DTEXTUREQ pTextureSystem, 
		float fReflectance, 
		float fFogBlend,
		DWORD dwFogLength,
		DWORD dwRadiosityIndex )
	{
		for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
		{
			vecLightMapObjectDest[i]->CreateTexelTriangleID();
			{
				vecLightMapObjectDest[i]->BakeRadiosity
				( 
					pd3dDevice, 
					pLandMan, 
					matWorld, 
					pTextureSystem, 
					fReflectance, 
					fFogBlend,
					dwFogLength,
					dwRadiosityIndex, 
					g_nSampling,
					TRUE
				);
			}
			vecLightMapObjectDest[i]->DeleteTexelTriangleID();
		}

		CreateLightMapInRadiosity( vecLightMapObjectDest );
	}
}
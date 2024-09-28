#include "pch.h"

#include <psapi.h>

#include "../../DxEffect/Mesh/DxStaticMeshCollBase.h"
#include "../../DxTools/Texture/NSTexCompress.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/DxMethods.h"
#include "../DxPiece/DxPieceManagerPSF.h"
#include "../DxLandMan.h"                   // ExportProgress ����� ��������.
#include "./NSCOMBINETEXTOOLS.h"
#include "./NSLightMapTools.h"
#include "./LightMapObject.h"

#include "NSLightMap.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//				LightMap ��� �۾��Ǿ��ִ��� ����� ���´�.
//-----------------------------------------------------------------------------
//
// Piece User UV ( Detail - Mesh ) ( Cast, Receive - DxSingleTexMesh )
//  1. �� Mesh ���� UV ���� ��´�. ( Detail�� �� )
//  2. UV ���� 0~1�� �����.
//  3. DxSingleTexMesh �� �и��ϴµ�, �׶� ���� UV�� �����Ų��.
//  4. DxSingleTexMesh �� �����.
//
//
// Piece UV 1-Tex ( Detail, Cast, Receive - DxSingleTexMesh )
//  1. PIE �� ������� �ִ�.
//  2. DxSingleTexMesh �� ������ �۾��� ����. 
//  3. UV�� ����Ѵ�. ( Detail �� ���⼭ ���� )
//  4. DxSingleTexMesh �� ���� UV�� �����Ų��.
//
//
// Octree ( Detail - Mesh ) ( Cast, Receive - DxSingleTexMesh )
//  1. �� Mesh ���� ������ ���Ѵ�. ( Detail�� �� )
//  2. Mesh�� UV ���� ����Ѵ�.
//  3. DxSingleTexMesh �� �и��ϴµ�, �׶� ���� UV�� �����Ų��.
//  4. DxSingleTexMesh �� �����.
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
        DWORD       m_dwTexelEmptyNum;      // ����ִ� ���� ����
	    bool*       m_pbTexelState;
	    WORD*       m_pTexelLast;
        DWORD*      m_pTexelLastDay;			// import �� �ʿ����. (A) Direct Light + Indirect Light + Shadow
		DWORD*      m_pTexelLastDirect_Shadow;  // import �� �ʿ����. (DS1,DS2) Direct Light ������(A,B), Shadow �׸���(G,R)
        DWORD*      m_pTexelLastIndirect;		// import �� �ʿ����. (I) Indirect Light ������ ( RAD �� �����Ǿ���. )

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
            // �ؽ��ĸ� �� ������ٸ� �� �������� �ʴ´�.
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

	// ����� ����ϱ� ���� �ʱ�ȭ
	void InitTextureDataBackup()
	{
		g_mapTextureData_BackUP.clear();
	}

	// ���ǰ� �ִ� ���� ��� �ϵ��� �Ѵ�.
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

	// ����� ���� ����ϵ��� �Ѵ�.
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
		// �޸� �����Ѵ�.
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
		// ���� �ָ� ���� �ִ� ������ ���Ѵ�.
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
				vecDWORD.push_back( nCount );		// LightMap ������ ������, ���� �۾����� ������ �ʿ��ϱ⿡ push_back �Ѵ�.
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

			// �׷��� �ϴ� ��ġ�� �����մϴ�.
			spLightMap->m_wwBaseTexelXY = sTexelXY;

			if( bDraw )
			{
				// ������ 2048x2048 �ؽ����� ���� �� �ȿ� ���� �ؽ��ĸ� ã�� ���� vecDWORD �� ���� �־� ���´�.
				// ���� ��Ȳ���� �� �� �ִ� LightMap Texture ��..
				vecDWORD.push_back( nCount );
			}
			else
			{
				// �ؽ��İ� �� á�� �׸� ���� ����. 
				// ������ 2048x2048 �ؽ����� ���� �� �ȿ� ���� �ؽ��ĸ� ã�� ���� vecDWORD �� ���� �־� ���´�.
				bRemindDrawPlace = FALSE;
			}
		}

		SAFE_DELETE_ARRAY ( pTexelEnable );

		return bRemindDrawPlace;
	}

	// User �� ���� LightMapUV �� �� ��밡���ϵ��� �����Ѵ�.
	BOOL RemakeLightMapUserUV( MAP_LIGHTMAPUV& mapLightMapUV, SP_LIGHTMAPUV_MAIN spLightMapUV_OUTPUT, VEC_DWORD& vecDWORD )
	{
		//VEC_DWORD vecDWORD;
		//DWORD dwTexWidth(32);
		//DWORD dwNewWidth(0);
		//DWORD dwNewHeight(0);
		//for ( ; dwTexWidth <= 2048; dwTexWidth*=2 )
		//{
		//	TextureData sTextureData(dwTexWidth,dwTexWidth);
		//	if ( RemakeLightMapUserUV2( mapLightMapUV, sTextureData, vecDWORD ) ||	// ���������� ����.
		//		dwTexWidth == 2048 )												// �ִ� �ؽ����ε��� �ȵȴٸ� �߰��� �ؽ��ĸ� �����.
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

		// UV ��ġ�� ������ ����.
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

				//// ��ġ�� 0~1 �� �����.
				//D3DXVECTOR2 vMultiply(vMax - vMin);
				//vMultiply.x = 1.f / vMultiply.x;
				//vMultiply.y = 1.f / vMultiply.y;
				//for ( DWORD i=0; i<spLightMapUV->m_vecUV.size(); ++i )
				//{
				//	spLightMapUV->m_vecUV[i] -= vMin;
				//	spLightMapUV->m_vecUV[i].x *= vMultiply.x;
				//	spLightMapUV->m_vecUV[i].y *= vMultiply.y;
				//}

				float fWidthRate = static_cast<float>(spLightMapUV->m_wwTextureSize.wA) / spLightMapUV_OUTPUT->m_dwTextureWidth;		// �̷��� �پ�����.
				float fHeightRate = static_cast<float>(spLightMapUV->m_wwTextureSize.wB) / spLightMapUV_OUTPUT->m_dwTextureHeight;	// �̷��� �پ�����.
				float fWidthBase = static_cast<float>(spLightMapUV->m_wwBaseTexelXY.wA) / spLightMapUV_OUTPUT->m_dwTextureWidth;		// �� ��ġ�� �̵��Ǿ���.
				float fHeightBase = static_cast<float>(spLightMapUV->m_wwBaseTexelXY.wB) / spLightMapUV_OUTPUT->m_dwTextureHeight;	// �� ��ġ�� �̵��Ǿ���.

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

	// User �� ���� LightMapUV �� �� ��밡���ϵ��� �����Ѵ�.
	BOOL RemakeLightMapUserUV_STATICMESH( MAP_LIGHTMAPUV& mapLightMapUV, SP_LIGHTMAPUV_MAIN spLightMapUV_OUTPUT )
	{
		VEC_DWORD vecDWORD;
		DWORD dwTexWidth(32);
		DWORD dwNewWidth(0);
		DWORD dwNewHeight(0);
		for ( ; dwTexWidth <= ENUM_TEXTURE_PIXEL_SIZE; dwTexWidth*=2 )
		{
			TextureData sTextureData(dwTexWidth,dwTexWidth);
			if ( RemakeLightMapUserUV2( mapLightMapUV, sTextureData, vecDWORD ) ||	// ���������� ����.
				dwTexWidth == ENUM_TEXTURE_PIXEL_SIZE )												// �ִ� �ؽ����ε��� �ȵȴٸ� �߰��� �ؽ��ĸ� �����.
			{
				break;
			}
		}

		spLightMapUV_OUTPUT->m_dwTextureWidth = dwTexWidth;
		spLightMapUV_OUTPUT->m_dwTextureHeight = dwTexWidth;
		RemakeLightMapUserUV( mapLightMapUV, spLightMapUV_OUTPUT, vecDWORD );

		return TRUE;
	}

	// User �� ���� LightMapUV �� �� ��밡���ϵ��� �����Ѵ�.
	BOOL RemakeLightMapUserUV_PIECE( MAP_LIGHTMAPUV& mapLightMapUV, SP_LIGHTMAPUV_MAIN spLightMapUV_OUTPUT )
	{
		VEC_DWORD vecDWORD;
		DWORD dwTexWidth(32);
		DWORD dwNewWidth(0);
		DWORD dwNewHeight(0);
		for ( ; dwTexWidth <= ENUM_TEXTURE_PIXEL_SIZE; dwTexWidth*=2 )
		{
			TextureData sTextureData(dwTexWidth,dwTexWidth);
			if ( RemakeLightMapUserUV2( mapLightMapUV, sTextureData, vecDWORD ) ||	// ���������� ����.
				dwTexWidth == ENUM_TEXTURE_PIXEL_SIZE )												// �ִ� �ؽ����ε��� �ȵȴٸ� �߰��� �ؽ��ĸ� �����.
			{
				RemakeLightMapUserUV3( sTextureData, dwTexWidth, dwNewWidth, dwNewHeight );
				break;
			}
		}

		spLightMapUV_OUTPUT->m_dwTextureWidth = dwNewWidth;		// �̺κп��� FrameMesh �� Piece �� �ٸ���.
		spLightMapUV_OUTPUT->m_dwTextureHeight = dwNewHeight;	// �̺κп��� FrameMesh �� Piece �� �ٸ���.
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

            // Note : ��ٰ� ��ŷ�ϰ� �׷��� �ϴ� ��ġ�� ���´�.
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

                // �׷��� �ϴ� ��ġ�� �����մϴ�.
                pLightMapObject->SetBaseTexelXY( sTexelXY );
                pLightMapObject->SetTextureID( TextureID );

		        if( !bDraw )
		        {
			        // �ؽ��İ� �� ���� �׸� ���� ����. 
                    return FALSE;
		        }
            }
        }

        // ������ �־���´�.
        sTextureData.m_vecvecLightMapObject.push_back( &vecLightMapObjectDest );

        return TRUE;
    }

    void CreateLightMapFinal( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwIndex, const TSTRING& strLightMapName, const TSTRING& strAddName, DWORD dwTextureID, int nBackUpNum, const TSTRING& strFinalName )
    {
        int i = dwIndex;

		// ������ �Ѿ��.
		if ( dwIndex >= m_vecTextureData.size() )
			return;

        // �ؽ��İ� ������� �����̴�.
        if ( m_vecTextureData[i]->m_bMakeTexture )
            return;

		_D3DFORMAT emFormat[4] = { D3DFMT_DXT1, D3DFMT_DXT1, D3DFMT_DXT1, D3DFMT_DXT1 };

        // �ؽ��İ� ��������ٰ� �����Ѵ�.
        m_vecTextureData[i]->m_bMakeTexture = TRUE;

        ExportProgress::SetStateString( "Texture ����" );

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
            ExportProgress::SetStateString( "�ؽ��ĸ� ����." );
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
					return;	// intel Ĩ�� ���� �׷��ȵ���̹� ���Ǹ� ������ �߻��� �� ����.
				}

                pData = (DWORD*)stLocked.pBits;
                iPitch = stLocked.Pitch / 4;
                {
					if ( j == 1 )
					{
						// Note : ���� �ؽ��Ŀ� �׸���.
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
						// Note : ���� �ؽ��Ŀ� �׸���.
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
						// Note : ���� �ؽ��Ŀ� �׸���.
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
				
				if ( g_bNoDXT1 )		// �������� �ʰڴ�.
				{
					D3DXSaveTextureToFile( strFullName.c_str(), D3DXIFF_DDS, pTexture, NULL );
				}
				else					// �����ϰڴ�.
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

				// �߰������� Back Texture�� ���鵵�� �Ѵ�.
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

        // ����
        {
            // SAFE_RELEASE
            SAFE_RELEASE( pTexture );
        }

		// Radiosity ������ �̰����� CleanUp�� ���� ���Ѵ�.
		// �޸� �����Ѵ�.
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

    //    // �ؽ��ĸ� �����ؾ� �ϴ� ����� dwIndex
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

    //                // �޸𸮸� �����Ѵ�.
    //                pLightMapObject->DeleteTexelColor();
    //            }
    //        }
    //    }

    //    CreateLightMapFinal( pd3dDevice, dwIndex, strLightMapName, strAddName, dwIndex );

    //    // �޸� �����Ѵ�.
    //    m_vecTextureData[dwIndex]->CleanUp();
    //}

    void CreateFaceGroup( VEC_FACE_ID& vecFaceGroupID, 
                        DWORD& dwMeshGroup, 
                        VEC_FACE_ID& vecFaceID, 
                        DWORD dwFaceNUM, 
                        BYTE* pVerticesSrc, 
                        DWORD dwFVF )
    {
        // ����Ʈ ���� ����� ���´�.
        {
            for ( DWORD i=0; i<dwFaceNUM; ++i )
            {
                vecFaceGroupID.push_back( -1 );
            }
        }

        // ù��° �ﰢ���� ù��° �׷����� �����.
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

        //// ������ �ﰢ���� ���Ѵ�.
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
        //                // ���� �� �ʱ�ȭ
        //                BOOL bNearTriangle = FALSE;

        //                // �׷��� ���õ��� �ʴ� �ﰢ������ ��ȯ�ؼ� ���� �ﰢ���� �ִ��� ã�´�.
        //                for ( int i=nStartIndex; i<static_cast<int>(dwFaceNUM); ++i )
        //                {
        //                    // �ε����� ���õǾ� �ִٸ� �۾����� �ʴ´�.
        //                    if ( vecFaceGroupID[i] != -1 )
        //                    {
        //                        continue;
        //                    }

        //                    // �پ��ִ� �ﰢ���� ã�´�.
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

        //                // ����� �ﰢ���� ���ٸ� ���� �׷��� ������ش�.
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

        //                    // ���ο� �׷��� ���� ���� ����. while �� ������� �Ѵ�.
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

		//// �ؽ��� �ִ� ������� 2048�� ���� �� ����.
		//if ( dwStartTextureSize > 2048 )
		//	dwStartTextureSize = 2048;

  //      if ( !bPiece && dwFaceNUM>1000 )
  //      {
  //          ExportProgress::SetStateString( "dwFaceNUM>1000" );
  //          ExportProgress::SetStateString( "������ �ﰢ�� ���� �и�,����.1" );
  //      }

  //      // ������ �ﰢ�� ���� �и�,���� �Ѵ�.
  //      {    
  //          VEC_FACE_ID vecFaceGroupID;     // �� Face ���� ��� �׷쿡 ���� �ִ��� �����Ѵ�.
  //          DWORD       dwMeshGroup(0);     // ���� �ﰢ�� �׷�
  //          VEC_FACE_ID vecFaceID;          // �׷� ������ �ٲ�� clear �ȴ�.

  //          // FaceGroup�� �����.
  //          CreateFaceGroup( vecFaceGroupID, dwMeshGroup, vecFaceID, dwFaceNUM, pVerticesSrc, dwFVF, emLightMapType );

  //          // �׷캰�� LightMapObject �� ���鵵�� �Ѵ�.
  //          {
  //              for ( DWORD i=0; i<(dwMeshGroup+1); ++i )
  //              {
  //                  std::tr1::shared_ptr<LightMapObject> spNew = std::tr1::shared_ptr<LightMapObject>(new LightMapObject);

  //                  if ( dwFVF == VERTEX::FVF )
  //                  {
  //                      VERTEX* pVertices = reinterpret_cast<VERTEX*>( pVerticesSrc );

  //                      // ���ؽ��� �߰��Ѵ�.
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

  //                      // ���ؽ��� �߰��Ѵ�.
  //                      for ( DWORD j=0; j<dwFaceNUM; ++j )
  //                      {
  //                          if ( i == vecFaceGroupID[ j ] )
  //                          {
  //                              spNew->AddThreeVerties( &pVertices[j*3+0], emLightMapType, j );
  //                          }
  //                      }
  //                  }

  //                  // �����Ѵ�.
  //                  vecLightMapObjectDest.push_back( spNew );
  //              }
  //          }
  //      }

  //      BOOL bTryMakeTexture(TRUE);
  //      float fLengthRate = fLightMapDetail;   // 0.25f // User�� ���� ���� �ִ´�.
  //      while ( bTryMakeTexture )
  //      {
  //          bTryMakeTexture = FALSE;

  //          if ( !bPiece && dwFaceNUM>1000 )
  //          {
  //              ExportProgress::SetStateString( "�ּ�ȭ ������ Pixel �˻���." );
  //          }

  //          // �׷캰�� LightMapObject �� ���鵵�� �Ѵ�.
  //          {
  //              for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
  //              {
  //                   // �ؼ� ũ�⸦ ���Ѵ�.
  //                  vecLightMapObjectDest[i]->CreateTexelScale( fLengthRate );

  //                  // �ּ�ȭ ������ Pixel �˻� ��ƾ���� Piece�� ���� ���ܽ�Ų��.
  //                  // Piece�� ������ ����� ���;��ϱ� ������ ������ �� ��ġ ������ �Ǹ� �ȵȴ�.
  //                  if ( !bPiece )
  //                  {
  //                      // ���� Pixel�� �������̶��, �� ������ ������ ���� ���δ�.
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

  //              // �� Pixel�� Ŀ���ϴ� ������ �÷��ش�.
  //              fLengthRate *= 0.7f;
  //          }

  //          if ( !bPiece && dwFaceNUM>1000 )
  //          {
  //              ExportProgress::SetStateString( "�˸��� �ؽ��ĸ� ���Ѵ�...." );
  //          }

  //          // �˸��� �ؽ��ĸ� ���Ѵ�.
  //          {
  //              DWORD dwActiveSlot(0L);
  //              for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
  //              {
  //                  LightMapObject* pLightMapObject = vecLightMapObjectDest[i].get();            
  //                  dwActiveSlot += (pLightMapObject->GetTexelSizeX()+2) * (pLightMapObject->GetTexelSizeY()+2);
  //              }

  //              BOOL bActiveTexture(FALSE);

  //              // ���� Index �� �˻縦 �����Ѵ�.
  //              // ū�ؽ��� ���� �����͸� �ֵ��� �۾��Ѵ�.
  //              TextureData sTextureData(g_dwBaseWidth,g_dwBaseWidth);
  //              for ( DWORD t=2048; t>=g_dwBaseWidth; t/=2 )
  //              {
  //                  for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
  //                  {
  //                      // ����� ���� �͸� �˻��Ѵ�.
  //                      if ( m_vecTextureData[i]->m_dwTexWidth != t )
  //                          continue;

  //                      // Piece �� ���� ����� �����ؾ߸� �� �ؽ��Ŀ� ���� �� �ִ�.
  //                      if ( bPiece )
  //                      {
  //                          if ( dwStartTextureSize != m_vecTextureData[ i ]->m_dwTexWidth )
  //                              continue;
  //                      }

  //                      // �ؽ��İ� ������� �����̴�.
  //                      if ( m_vecTextureData[ i ]->m_bMakeTexture )
  //                          continue;

  //                      // �ؽ����� ���� �ִ� ������ ����ؾ� �� ���� ��� 1.1�� ���Ϸ� ���� �ִٸ� �ٸ� ���� ����Ѵ�.
  //                      if ( dwActiveSlot > m_vecTextureData[ i ]->m_dwTexelEmptyNum*1.1 )
  //                      {
  //                          continue;
  //                      }

  //                      // �����͸� �����Ѵ�.
  //                      sTextureData.Import( *m_vecTextureData[ i ] );

  //                      // �˸��� �ؽ��ĸ� ���ϰ�, �����͸� Change �Ѵ�.
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

  //              // Ȱ��ȭ �� Texture�� ���� ��� ���� �������ش�.
  //              if ( !bActiveTexture )
  //              {
  //                  // �⺻���� �ؽ��� ����� ������
  //                  DWORD dwTextureSize = dwStartTextureSize;

  //                  // �׷��� �� ���� �׷����ϴ� ũ�⺸�� ���ٸ� ����� �ø���.
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
  //                          // �۾��� �Ұ����ϴ�.
  //                          bTryMakeTexture = TRUE;
  //                      }
  //                  }

  //                  for( DWORD i=0; i<3 && dwTextureSize <= 2048; ++i )
  //                  {
  //                      TextureData sTextureData(dwTextureSize,dwTextureSize);
  //                      
  //                      // �˸��� �ؽ��ĸ� ���ϰ�, �����͸� Change �Ѵ�.
  //                      if ( EnableInsertTexel( vecLightMapObjectDest, sTextureData, static_cast< DWORD >( m_vecTextureData.size() ), FALSE ) )
  //                      {
  //                          std::tr1::shared_ptr<TextureData> spNew( std::tr1::shared_ptr<TextureData>(new TextureData(dwTextureSize,dwTextureSize)) );
  //                          spNew->Import( sTextureData );
  //                          spNew->m_dwTexelEmptyNum -= dwActiveSlot;
  //                          m_vecTextureData.push_back( spNew );
  //                          break;
  //                      }

  //                      // �ȵǸ� �� ū �������� �ؽ��ķ� �۾��� ����.
  //                      dwTextureSize *= 2;
  //                  }

  //                  if ( dwTextureSize > 2048 )
  //                  {
  //                      // �۾��� �Ұ����ϴ�.
  //                      bTryMakeTexture = TRUE;
  //                  }

  //                  // Piece �� ���� ������ �ѹ� �������� ������ ������ �����Ѵ�.
  //                  // ������ �������� ������ �Ѵ�.
  //                  if ( bPiece && !bTryMakeTexture )
  //                  {
  //                      dwStartTextureSize = dwTextureSize;
  //                  }
  //              }
  //          }
  //      }

  //      if ( !bPiece && dwFaceNUM>1000 )
  //      {
  //          ExportProgress::SetStateString( "�ؼ� ��� ��..." );
  //      }

  //      DWORD dwTestColor = 0xff000000 + (rand()%0xffffff);

  //      // �׷캰�� LightMapObject �� ���鵵�� �Ѵ�.
  //      {
  //          for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
  //          {
  //              // �ؼ��� ����� ����ϰ�..
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
  //      if ( nWorkingSetSize > 800 )  // 800MB �� �Ѿ�� �ؽ��� ����
  //      {
  //          // 800MB ���Ϸ� ���δ�.
  //          BOOL bEnableTexture(TRUE);
  //          while ( nWorkingSetSize > 800 && bEnableTexture )
  //          {
  //              CreateTextureDataLowerEmpty( pd3dDevice, strLightMapName, strAddName, bAlphaChange, bPiece, FALSE );

  //              bEnableTexture = FALSE;
  //              for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
  //              {
  //                  // �ؽ��İ� ��������� ����� �Ѵ�.
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
			ExportProgress::SetStateString( "������ �ﰢ�� ���� �и�,����.1" );
		}

		// ��� �ﰢ���� ���δ�.
		{    
			VEC_FACE_ID vecFaceGroupID;     // �� Face ���� ��� �׷쿡 ���� �ִ��� �����Ѵ�.
			DWORD       dwMeshGroup(0);     // ���� �ﰢ�� �׷�
			VEC_FACE_ID vecFaceID;          // �׷� ������ �ٲ�� clear �ȴ�.

			// FaceGroup�� �����. - ����� �׷��� �Ѱ��� �����.
			CreateFaceGroup( vecFaceGroupID, dwMeshGroup, vecFaceID, dwFaceNUM, pVerticesSrc, dwFVF );

			// �׷캰�� LightMapObject �� ���鵵�� �Ѵ�.
			{
				DWORD i=0;
				//for ( DWORD i=0; i<(dwMeshGroup+1); ++i )
				{
					std::tr1::shared_ptr<LightMapObject> spNew = std::tr1::shared_ptr<LightMapObject>(new LightMapObject);

					//if ( dwFVF == VERTEXNORTEX2::FVF )
					//{
					//	VERTEXNORTEX2* pVertices = reinterpret_cast<VERTEXNORTEX2*>( pVerticesSrc );

					//	// ���ؽ��� �߰��Ѵ�.
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

						// ���ؽ��� �߰��Ѵ�.
						for ( DWORD j=0; j<dwFaceNUM; ++j )
						{
							if ( i == vecFaceGroupID[ j ] )
							{
								spNew->AddThreeVerties( &pVertices[j*3+0], j );
							}
						}
					}

					// �ؼ� ũ��, �ؽ���ID�� �����Ѵ�.
					spNew->SetBaseTexelXY( pLightMapUV->m_wwBaseTexelXY );
					spNew->SetTexelScale( pLightMapUV->m_wwTextureSize );
					spNew->SetLightMapUV( pLightMapUV->m_vecUV, wwLightMapUserUV_TEXEL );
					spNew->SetTextureID( dwTextureID );
					spNew->SetTriangle_vecLightID( vecTriangle_wwLightID );

					// �����Ѵ�.
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
			ExportProgress::SetStateString( "�ؼ� ��� ��..." );
		}

		// �׷캰�� LightMapObject �� ���鵵�� �Ѵ�.
		{
			for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
			{
				vecLightMapObjectDest[i]->CreateTexelTriangleID();
				{
					BOOL bEnoughUsedPixelLightMap(TRUE);

					// �ؼ��� ����� ����ϰ�..
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

		// �ؽ��� �ִ� ������� 2048�� ���� �� ����.
		if ( dwStartTextureSize > ENUM_TEXTURE_PIXEL_SIZE )
			dwStartTextureSize = ENUM_TEXTURE_PIXEL_SIZE;

		if ( !bPiece && dwFaceNUM>1000 )
		{
			ExportProgress::SetStateString( "dwFaceNUM>1000" );
			ExportProgress::SetStateString( "������ �ﰢ�� ���� �и�,����.1" );
		}

		// ��� �ﰢ���� ���δ�.
		{    
			VEC_FACE_ID vecFaceGroupID;     // �� Face ���� ��� �׷쿡 ���� �ִ��� �����Ѵ�.
			DWORD       dwMeshGroup(0);     // ���� �ﰢ�� �׷�
			VEC_FACE_ID vecFaceID;          // �׷� ������ �ٲ�� clear �ȴ�.

			// FaceGroup�� �����. - ����� �׷��� �Ѱ��� �����.
			CreateFaceGroup( vecFaceGroupID, dwMeshGroup, vecFaceID, dwFaceNUM, pVerticesSrc, dwFVF );

			// �׷캰�� LightMapObject �� ���鵵�� �Ѵ�.
			{
				DWORD i=0;
				//for ( DWORD i=0; i<(dwMeshGroup+1); ++i )
				{
					std::tr1::shared_ptr<LightMapObject> spNew = std::tr1::shared_ptr<LightMapObject>(new LightMapObject);

					if ( dwFVF == VERTEXNORCOLORTEX4::FVF )
					{
						VERTEXNORCOLORTEX4* pVertices = reinterpret_cast<VERTEXNORCOLORTEX4*>( pVerticesSrc );

						// ���ؽ��� �߰��Ѵ�.
						for ( DWORD j=0; j<dwFaceNUM; ++j )
						{
							if ( i == vecFaceGroupID[ j ] )
							{
								spNew->AddThreeVerties( &pVertices[j*3+0], j );
							}
						}
					}

					spNew->SetLightMapUV( vecUV, wwTexelSize );

					// �����Ѵ�.
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
				ExportProgress::SetStateString( "�ּ�ȭ ������ Pixel �˻���." );
			}

			// �׷캰�� LightMapObject �� ���鵵�� �Ѵ�.
			{
				for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
				{
					// �ؼ� ũ�⸦ �����Ѵ�.
					vecLightMapObjectDest[i]->SetTexelScale( wwTexelSize );
				}
			}

			if ( !bPiece && dwFaceNUM>1000 )
			{
				ExportProgress::SetStateString( "�˸��� �ؽ��ĸ� ���Ѵ�...." );
			}

			// �˸��� �ؽ��ĸ� ���Ѵ�.
			{
				DWORD dwActiveSlot(0L);
				for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
				{
					LightMapObject* pLightMapObject = vecLightMapObjectDest[i].get();            
					dwActiveSlot += (pLightMapObject->GetTexelSizeX()) * (pLightMapObject->GetTexelSizeY());
				}

				BOOL bActiveTexture(FALSE);

				// ���� Index �� �˻縦 �����Ѵ�.
				// ū�ؽ��� ���� �����͸� �ֵ��� �۾��Ѵ�.
				TextureData sTextureData(g_dwBaseWidth,g_dwBaseWidth);
				for ( DWORD t=ENUM_TEXTURE_PIXEL_SIZE; t>=g_dwBaseWidth; t/=2 )
				{
					for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
					{
						// ����� ���� �͸� �˻��Ѵ�.
						if ( m_vecTextureData[i]->m_dwTexWidth != t )
							continue;

						// Piece �� ���� ����� �����ؾ߸� �� �ؽ��Ŀ� ���� �� �ִ�.
						if ( bPiece )
						{
							if ( dwStartTextureSize != m_vecTextureData[ i ]->m_dwTexWidth )
								continue;
						}

						// �ؽ��İ� ������� �����̴�.
						if ( m_vecTextureData[ i ]->m_bMakeTexture )
							continue;

						// �ؽ��Ŀ� ��� ������ ������ ������ Path
						if ( dwActiveSlot > m_vecTextureData[ i ]->m_dwTexelEmptyNum )
						{
							continue;
						}

						// �����͸� �����Ѵ�.
						sTextureData.Import( *m_vecTextureData[ i ] );

						// �˸��� �ؽ��ĸ� ���ϰ�, �����͸� Change �Ѵ�.
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

				// Ȱ��ȭ �� Texture�� ���� ��� ���� �������ش�.
				if ( !bActiveTexture )
				{
					// �⺻���� �ؽ��� ����� ������
					DWORD dwTextureSize = dwStartTextureSize;

					// �׷��� �� ���� �׷����ϴ� ũ�⺸�� ���ٸ� ����� �ø���.
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
							// �۾��� �Ұ����ϴ�.
							bTryMakeTexture = TRUE;
						}
					}

					for( DWORD i=0; i<3 && dwTextureSize <= ENUM_TEXTURE_PIXEL_SIZE; ++i )
					{
						TextureData sTextureData(dwTextureSize,dwTextureSize);

						// �˸��� �ؽ��ĸ� ���ϰ�, �����͸� Change �Ѵ�.
						if ( EnableInsertTexel( vecLightMapObjectDest, sTextureData, static_cast< DWORD >( m_vecTextureData.size() ), TRUE ) )
						{
							std::tr1::shared_ptr<TextureData> spNew( std::tr1::shared_ptr<TextureData>(new TextureData(dwTextureSize,dwTextureSize)) );
							spNew->Import( sTextureData );
							spNew->m_dwTexelEmptyNum -= dwActiveSlot;
							m_vecTextureData.push_back( spNew );
							break;
						}

						// �ȵǸ� �� ū �������� �ؽ��ķ� �۾��� ����.
						dwTextureSize *= 2;
					}

					if ( dwTextureSize > ENUM_TEXTURE_PIXEL_SIZE )
					{
						// �۾��� �Ұ����ϴ�.
						bTryMakeTexture = TRUE;
					}

					// Piece �� ���� ������ �ѹ� �������� ������ ������ �����Ѵ�.
					// ������ �������� ������ �Ѵ�.
					if ( bPiece && !bTryMakeTexture )
					{
						dwStartTextureSize = dwTextureSize;
					}
				}
			}
		}

		if ( !bPiece && dwFaceNUM>1000 )
		{
			ExportProgress::SetStateString( "�ؼ� ��� ��..." );
		}

		// �׷캰�� LightMapObject �� ���鵵�� �Ѵ�.
		{
			for ( DWORD i=0; i<vecLightMapObjectDest.size(); ++i )
			{
				vecLightMapObjectDest[i]->CreateTexelTriangleID();
				{
					// �ؼ��� ����� ����ϰ�..
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
		//if ( nWorkingSetSize > 800 )  // 800MB �� �Ѿ�� �ؽ��� ����
		//{
		//	// 800MB ���Ϸ� ���δ�.
		//	BOOL bEnableTexture(TRUE);
		//	while ( nWorkingSetSize > 800 && bEnableTexture )
		//	{
		//		CreateTextureDataLowerEmpty( pd3dDevice, strLightMapName, strAddName, bAlphaChange, bPiece, TRUE );

		//		bEnableTexture = FALSE;
		//		for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
		//		{
		//			// �ؽ��İ� ��������� ����� �Ѵ�.
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

    // 1���� Width �� ���ȴٸ� �� ������ Block �Ѵ�.
    void OneUseLineBlock()
    {
        for ( DWORD i=0; i<m_vecTextureData.size(); ++i )
        {
            // �˸��� �ؽ��ĸ� ���ϰ�, �����͸� Change �Ѵ�.
            OneUseLineBlock2( m_vecTextureData[i].get() );
        }
    }

	// ��� ������ ���Ѵ�.
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

    // LightMap Texture�� Resize �Ѵ�
    void ResizeLightMapTexture()
    {
        ExportProgress::SetStateString( "�ؽ��� ������ ���̱�" );

        // Note : �ּҷ� ���� �� �ִ� ����� Ȯ���ϰ� �ؽ��ĸ� �ϴ� �����Ѵ�.
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

    // Texture ����� ���´�.
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

        // ��� �������� �ؽ��ĸ� ������ �˾Ƴ��´�. - Tile,Ground�� ����.
        pLightMapObject->SetTextureSizeXY( WORDWORD( static_cast<WORD>(dwTexWidth), static_cast<WORD>(dwTexHeight) ) );
       
        // Note : ������ �ؽ��Ŀ� �׸��⸦ �����Ѵ�.
        if ( !m_vecTextureData[ dwTextureID ]->m_bMakeTexture )
        {
            // TexelColor ����
            m_vecTextureData[ dwTextureID ]->CreateTexelColor();

            if ( bCreateTextureDataLowerEmpty )
            {
            //    ExportProgress::SetStateString( "������ �ؼ����� ����" );
            }

            //------- ���� ��ġ�� �����Ѵ�. #2
            {
                DWORD* pTexelDest[3] = { m_vecTextureData[ dwTextureID ]->m_pTexelLastDay, 
                                        m_vecTextureData[ dwTextureID ]->m_pTexelLastDirect_Shadow,
										m_vecTextureData[ dwTextureID ]->m_pTexelLastIndirect };

                DWORD* pTexelCombined		= pLightMapObject->GetTexelColorCombined();
                DWORD* pTexelDirect_Shadow  = pLightMapObject->GetTexelColorDirect_Shadow();
				DWORD* pTexelIndirect		= pLightMapObject->GetTexelColorIndirect();

                DWORD* pTexel[3] = { pTexelCombined, pTexelDirect_Shadow, pTexelIndirect };
                
                //------- Texel ���
                for ( DWORD i=0; i<3; ++i )
                {
                    // Tiling ���� ���δ�.
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
						// Note : ���� �ؽ��Ŀ� �׸���.
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
						// Note : ���� �ؽ��Ŀ� �׸���.
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
            //    ExportProgress::SetStateString( "��ǥ �� ���" );
            }

            {
                //------- UV ��ǥ�� �����Ѵ�.
                {
					if ( !bUserUV )
					{
						pLightMapObject->CaleUV( 1, 1 );
					}

                    // Piece �� ���� - ������ �� �ؾ� �Ѵ�.
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
        //    if ( nWorkingSetSize > 800 )  // 800MB �� �Ѿ�� �ؽ��� ����
        //    {
        //        CreateTextureDataLowerEmpty( g_pd3dDevice, strLightMapName, strAddName, bAlphaChange, bPiece, bUserUV );
        //    }
        //}

        // Note : �ؽ��� ������ ��´�.
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

			// [CascadeShadow] �۾�������
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

		// Note : �ؽ��� ������ ��´�.
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

			// [CascadeShadow] �۾�������
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

	// Texel ������ ������ �۾��� �Ѵ�.
	void CreateLightMapInRadiosity_Detail( LightMapObject* pLightMapObject, DWORD dwTextureID )
	{
		if ( dwTextureID >= m_vecTextureData.size() )
		{
			return;
		}

		// Radiosity ���� �Ǵٽ� �ؽ��ĸ� ������ �Ǿ, FALSE �� �ش�.
		m_vecTextureData[ dwTextureID ]->m_bMakeTexture = FALSE;

		DWORD dwTexWidth = m_vecTextureData[ dwTextureID ]->m_dwTexWidth;
		DWORD dwTexHeight = m_vecTextureData[ dwTextureID ]->m_dwTexHeight;

		// Note : ������ �ؽ��Ŀ� �׸��⸦ �����Ѵ�.
		{
			{
				ExportProgress::SetStateString( _T("������ �ؼ����� ����") );
			}

			//------- ���� ��ġ�� �����Ѵ�. #2
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

				//------- Texel ���
				{
					// Note : ���� �ؽ��Ŀ� �׸���.
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

				//------- Texel ���
				{
					// Note : ���� �ؽ��Ŀ� �׸���.
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

	// Texel ������ ������ �۾��� �Ѵ�.
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

	// Piece Object �� �۾��ȴ�.
	// Radiosity �� ���´�.
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
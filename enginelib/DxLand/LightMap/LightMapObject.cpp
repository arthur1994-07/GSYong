#include "pch.h"

#include <omp.h>

#include "../../../SigmaCore/Math/Random.h"

#include "../../DxEffect/Mesh/DxStaticMeshLightMap.h"
#include "../../DxTools/TextureManager.h"
#include "../DxPiece/DxPieceManagerWLD.h"
#include "../DxPiece/NSPieceQuickSort.h"
#include "../NSLandThread.h"
#include "../DxLandMan.h"
#include "./NSLightMapTools.h"
#include "./NSLightMap.h"

#include "LightMapObject.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//-------------------------------------------------------
// LightMapObject_Data
LightMapObject_Data::LightMapObject_Data()
    : m_bChangeUV_XZ(FALSE)
    , m_wwBaseTexelXY(0,0)
    , m_wwTexelSize(0,0)
    , m_wwTextureSize(0,0)
    , m_vMaxUV(0.f,0.f)
    , m_vMinUV(0.f,0.f)
    , m_strTextureCombined(_T(""))
    , m_strTextureDirect_Shadow_1(_T(""))
	, m_strTextureDirect_Shadow_2(_T(""))
    , m_strTextureIndirect(_T(""))
{
}

//-------------------------------------------------------
// LightMapObject
LightMapObject::LightMapObject()
    : m_bChangeUV_XZ(FALSE)
    , m_wwBaseTexelXY(0,0)
    , m_wwBaseTexelPieceXY(0,0)
    , m_wwTexelSize(0,0)
    , m_wwTextureSize(0,0)
    , m_vMaxUV(0.f,0.f)
    , m_vMinUV(0.f,0.f)
    , m_dwTextureID(0)
	, m_pInTriangle(NULL)
    , m_pTexelCombined(NULL)
    , m_pTexelDirect_Shadow(NULL)
	, m_pTexelIndirect(NULL)
	, m_pTexelCombinedRAD(NULL)
	, m_pTexelNoDynamicLight(NULL)
	, m_pTexelTriangleID(NULL)
    , m_bGaussBlurSetUV(FALSE)
{
}

LightMapObject::~LightMapObject()
{
    DeleteTexelColor();
}

void LightMapObject::DeleteTexelColor()
{
	SAFE_DELETE_ARRAY( m_pInTriangle );
    SAFE_DELETE_ARRAY( m_pTexelCombined );
    SAFE_DELETE_ARRAY( m_pTexelDirect_Shadow );
	SAFE_DELETE_ARRAY( m_pTexelIndirect );
	SAFE_DELETE_ARRAY( m_pTexelCombinedRAD );
	SAFE_DELETE_ARRAY( m_pTexelNoDynamicLight );
}

DWORD LightMapObject::GetTextureID() const
{
    return m_dwTextureID;
}

DWORD LightMapObject::GetTexelSizeX() const
{ 
    return static_cast<DWORD>(m_wwTexelSize.wA); 
}

DWORD LightMapObject::GetTexelSizeY() const
{ 
    return static_cast<DWORD>(m_wwTexelSize.wB);
} 

void LightMapObject::GetLightMapUV_RATE( D3DXVECTOR2& vRate ) const
{
	vRate.x = static_cast<float>( m_wwTexelSize.wA ) / m_wwTextureSize.wA;
	vRate.y = static_cast<float>( m_wwTexelSize.wB ) / m_wwTextureSize.wB;
}

BOOL LightMapObject::IsGaussBlurSetUV() const
{
    return m_bGaussBlurSetUV;
}

DWORD* LightMapObject::GetTexelColorCombined() const
{ 
    return m_pTexelCombined; 
}

DWORD* LightMapObject::GetTexelColorDirect_Shadow() const
{ 
    return m_pTexelDirect_Shadow; 
}

DWORD* LightMapObject::GetTexelColorIndirect() const
{ 
	return m_pTexelIndirect; 
}

DWORD* LightMapObject::GetTexelColorCombinedRAD() const
{ 
	return m_pTexelCombinedRAD; 
}

DWORD* LightMapObject::GetTexelColorNoDynamicLight() const
{ 
	return m_pTexelNoDynamicLight; 
}

TextureResource LightMapObject::GetTexture( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strAddName ) const
{
    int nTexCount(0);

    int n100 = nTexCount/100;
    int n10 = (nTexCount-n100*100) / 10;
    int n1 = nTexCount%10;
    TCHAR c100 = (TCHAR)(n100+48);
    TCHAR c10 = (TCHAR)(n10+48);
    TCHAR c1 = (TCHAR)(n1+48);

    TSTRING strTexture = _T("");
    strTexture = strTexture + _T("Temp") + strAddName + c100 + c10 + c1 + _T(".tga");

    return TextureManager::GetInstance().LoadTexture(
        strTexture,
        false,
        TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
        1,
        D3DFMT_UNKNOWN,
        TEXTURE_RESIZE_NONE,
        TEXTURE_LAND,
        false,
		FALSE );
}

const WORDWORD& LightMapObject::GetBaseTexelXY() const
{
    return m_wwBaseTexelXY;
}

const WORDWORD& LightMapObject::GetBaseTexelPieceXY() const
{
    return m_wwBaseTexelPieceXY;
}

void LightMapObject::SetBaseTexelXY( WORDWORD& sTexelXY )
{ 
    m_wwBaseTexelXY = sTexelXY; 
}

void LightMapObject::SetTextureSizeXY( WORDWORD& sSizeXY )
{
    m_wwTextureSize = sSizeXY;
}

void LightMapObject::SetBaseTexelPieceXY( WORDWORD& sTexelXY )
{
    m_wwBaseTexelPieceXY = sTexelXY; 
}

void LightMapObject::SetTextureID( DWORD dwID )
{ 
    m_dwTextureID = dwID; 
}

void LightMapObject::SetLightMapName( const TSTRING& strTextureCombined,
                                    const TSTRING& strTextureDirect_Shadow_1,
									const TSTRING& strTextureDirect_Shadow_2,
                                    const TSTRING& strTextureIndirect )
{
    m_strTextureCombined		= strTextureCombined;			// LightMap 이름
    m_strTextureDirect_Shadow_1	= strTextureDirect_Shadow_1;	// LightMap 이름
	m_strTextureDirect_Shadow_2	= strTextureDirect_Shadow_2;	// LightMap 이름
	m_strTextureIndirect		= strTextureIndirect;			// LightMap 이름
}

void LightMapObject::GetLightMapName( TSTRING& strTextureCombined,
									 TSTRING& strTextureDirect_Shadow_1,
									 TSTRING& strTextureDirect_Shadow_2,
									 TSTRING& strTextureIndirect )
{
    strTextureCombined			= m_strTextureCombined;			// LightMap 이름
    strTextureDirect_Shadow_1	= m_strTextureDirect_Shadow_1;  // LightMap 이름
	strTextureDirect_Shadow_2	= m_strTextureDirect_Shadow_2;	// LightMap 이름
	strTextureIndirect			= m_strTextureIndirect;			// LightMap 이름
}

void LightMapObject::SetTriangle_vecLightID( const std::vector<WORDWORD>& vecTriangle_wwLightID )
{
	m_vecTriangle_wwLightID = vecTriangle_wwLightID;
}

//
//// 버텍스를 얻어옴.
//void LightMapObject::AddThreeVerties( const VERTEX* pVerticesSrc, NSLIGHTMAPDEF::LIGHTMAPTYPE emLightMapType, DWORD dwFace )
//{
//    m_vecFaces.push_back( dwFace );
//
//    VERTEXNORCOLORTEX4 sVertexDest;
//    for ( DWORD i=0; i<3; ++i )
//    {
//        sVertexDest.vPos = pVerticesSrc[i].vPos;
//        sVertexDest.vNor = pVerticesSrc[i].vNor;
//        sVertexDest.dwColor = 0xffffffff;
//        sVertexDest.vTex[0] = pVerticesSrc[i].vTex;
//        sVertexDest.vTex[1] = pVerticesSrc[i].vTex;
//        sVertexDest.vTex[2] = pVerticesSrc[i].vTex;
//		sVertexDest.vTex[3] = pVerticesSrc[i].vTex;
//
//        m_vecVertices.push_back( sVertexDest );
//    }
//
//    // LightMap UV 설정한다.
//    int nVertSize = static_cast< int >( m_vecVertices.size() );
//    switch ( emLightMapType )
//    {
//    case NSLIGHTMAPDEF::LMT_GROUND:
//        for ( int i=0; i<3; ++i )
//        {
//            m_vecVertices[nVertSize-3+i].vTex[2].x = pVerticesSrc[i].vPos.x;
//            m_vecVertices[nVertSize-3+i].vTex[2].y = pVerticesSrc[i].vPos.z;
//        }
//        break;
//    };
//}
//
//// 버텍스를 얻어옴.
//void LightMapObject::AddThreeVerties( const VERTEXNORCOLORTEX* pVerticesSrc, NSLIGHTMAPDEF::LIGHTMAPTYPE emLightMapType, DWORD dwFace )
//{
//    m_vecFaces.push_back( dwFace );
//
//    VERTEXNORCOLORTEX4 sVertexDest;
//    for ( DWORD i=0; i<3; ++i )
//    {
//        sVertexDest.vPos = pVerticesSrc[i].vPos;
//        sVertexDest.vNor = pVerticesSrc[i].vNor;
//        sVertexDest.dwColor = pVerticesSrc[i].dwColor;
//        sVertexDest.vTex[0] = pVerticesSrc[i].vTex;
//        sVertexDest.vTex[1] = pVerticesSrc[i].vTex;
//        sVertexDest.vTex[2] = pVerticesSrc[i].vTex;
//		sVertexDest.vTex[3] = pVerticesSrc[i].vTex;
//
//        m_vecVertices.push_back( sVertexDest );
//    }
//
//    // LightMap UV 설정한다.
//    int nVertSize = static_cast< int >( m_vecVertices.size() );
//    switch ( emLightMapType )
//    {
//    case NSLIGHTMAPDEF::LMT_GROUND:
//        for ( int i=0; i<3; ++i )
//        {
//            m_vecVertices[nVertSize-3+i].vTex[2].x = pVerticesSrc[i].vPos.x;
//            m_vecVertices[nVertSize-3+i].vTex[2].y = pVerticesSrc[i].vPos.z;
//        }
//        break;
//    };
//}
//
//// 버텍스를 얻어옴.
//void LightMapObject::AddThreeVerties( const VERTEXNORTEX2* pVerticesSrc, NSLIGHTMAPDEF::LIGHTMAPTYPE emLightMapType, DWORD dwFace )
//{
//	m_vecFaces.push_back( dwFace );
//
//	VERTEXNORCOLORTEX4 sVertexDest;
//	for ( DWORD i=0; i<3; ++i )
//	{
//		sVertexDest.vPos = pVerticesSrc[i].vPos;
//		sVertexDest.vNor = pVerticesSrc[i].vNor;
//		sVertexDest.dwColor = 0xffffffff;
//		sVertexDest.vTex[0] = pVerticesSrc[i].vTex[0];
//		sVertexDest.vTex[1] = pVerticesSrc[i].vTex[1];
//		sVertexDest.vTex[2] = pVerticesSrc[i].vTex[1];
//		sVertexDest.vTex[3] = pVerticesSrc[i].vTex[1];
//
//		m_vecVertices.push_back( sVertexDest );
//	}
//
//	// LightMap UV 설정한다.
//	int nVertSize = static_cast< int >( m_vecVertices.size() );
//	switch ( emLightMapType )
//	{
//	case NSLIGHTMAPDEF::LMT_GROUND:
//		for ( int i=0; i<3; ++i )
//		{
//			m_vecVertices[nVertSize-3+i].vTex[2].x = pVerticesSrc[i].vPos.x;
//			m_vecVertices[nVertSize-3+i].vTex[2].y = pVerticesSrc[i].vPos.z;
//		}
//		break;
//	};
//}

// 버텍스를 얻어옴.
void LightMapObject::AddThreeVerties( const VERTEX_WORLD* pVerticesSrc, DWORD dwFace )
{
	m_vecFaces.push_back( dwFace );

	VERTEX_WORLD sVertexDest;
	for ( DWORD i=0; i<3; ++i )
	{
		sVertexDest = pVerticesSrc[i];
		m_vecVertices.push_back( sVertexDest );
	}

	//// LightMap UV 설정한다.
	//int nVertSize = static_cast< int >( m_vecVertices.size() );
	//switch ( emLightMapType )
	//{
	//case NSLIGHTMAPDEF::LMT_GROUND:
	//	for ( int i=0; i<3; ++i )
	//	{
	//		m_vecVertices[nVertSize-3+i].vTex[2].x = pVerticesSrc[i].vPos.x;
	//		m_vecVertices[nVertSize-3+i].vTex[2].y = pVerticesSrc[i].vPos.z;
	//	}
	//	break;
	//};
}

// 버텍스를 얻어옴.
void LightMapObject::AddThreeVerties( const VERTEXNORCOLORTEX4* pVerticesSrc, DWORD dwFace )
{
	m_vecFaces.push_back( dwFace );

	VERTEX_WORLD sVertexDest;
	for ( DWORD i=0; i<3; ++i )
	{
		sVertexDest.vPos = pVerticesSrc[i].vPos;
		sVertexDest.vNor = pVerticesSrc[i].vNor;
		sVertexDest.dwColor = pVerticesSrc[i].dwColor;
		sVertexDest.vTex[0] = pVerticesSrc[i].vTex[0];
		sVertexDest.vTex[1] = pVerticesSrc[i].vTex[1];
		sVertexDest.vTex[2] = pVerticesSrc[i].vTex[2];
		sVertexDest.vTex[3] = pVerticesSrc[i].vTex[3];
		m_vecVertices.push_back( sVertexDest );
	}

	//// LightMap UV 설정한다.
	//int nVertSize = static_cast< int >( m_vecVertices.size() );
	//switch ( emLightMapType )
	//{
	//case NSLIGHTMAPDEF::LMT_GROUND:
	//	for ( int i=0; i<3; ++i )
	//	{
	//		m_vecVertices[nVertSize-3+i].vTex[2].x = pVerticesSrc[i].vPos.x;
	//		m_vecVertices[nVertSize-3+i].vTex[2].y = pVerticesSrc[i].vPos.z;
	//	}
	//	break;
	//};
}

// 알맞을 텍셀 크기를 구함.
void LightMapObject::CreateTexelScale( float fLengthRate )  // 기본 0.35f
{
    D3DXVECTOR3 vPosMin = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );
    D3DXVECTOR3 vPosMax = D3DXVECTOR3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
    D3DXVECTOR2 vMin = D3DXVECTOR2( FLT_MAX, FLT_MAX );
    D3DXVECTOR2 vMax = D3DXVECTOR2( -FLT_MAX, -FLT_MAX );

    for ( DWORD i=0; i<m_vecVertices.size(); ++i )
    {
        NSLightMap::CalcMinMax( vMin, vMax, m_vecVertices[i].vTex[2] );
        NSLightMap::CalcMinMax( vPosMin, vPosMax, m_vecVertices[i].vPos );
    }

    // 세로로 긴 모양이라면 가로로 길게 변경한다.
    if ( (vMax.y - vMin.y) > (vMax.x - vMin.x) )
    {
        m_bChangeUV_XZ = TRUE;

        float fTemp = vMax.y;
        vMax.y = vMax.x;
        vMax.x = fTemp;

        fTemp = vMin.y;
        vMin.y = vMin.x;
        vMin.x = fTemp;


        for ( DWORD i=0; i<m_vecVertices.size(); ++i )
        {
            float fTemp = m_vecVertices[i].vTex[2].x;
            m_vecVertices[i].vTex[2].x = m_vecVertices[i].vTex[2].y;
            m_vecVertices[i].vTex[2].y = fTemp;
        }
    }

    int nTexelX(0);
    int nTexelY(0);

    // 위치로 텍셀 수를 계산한다.
    {
        // Note : 들어가야 하는 Pixel 을 구한다. x,y
	    //float fIndexX = (vPosMax.x - vPosMin.x) * 0.5f;  // 0.5 로 하니깐 너무 많은 메모리를 사용한다.
	    //float fIndexY = (vPosMax.y - vPosMin.y) * 0.5f;  // 예로 0.5 일경우 낮밤 텍스쳐가 150MB가 넘는다.
     //   float fIndexZ = (vPosMax.z - vPosMin.z) * 0.5f;
        float fIndexX = (vPosMax.x - vPosMin.x) * fLengthRate;   // 면적은 0.5의 1/2 수준으로 줄어든다.
	    float fIndexY = (vPosMax.y - vPosMin.y) * fLengthRate;
        float fIndexZ = (vPosMax.z - vPosMin.z) * fLengthRate;

        typedef std::multiset<float,std::greater<float>>           SET_FLOAT;
        typedef std::multiset<float,std::greater<float>>::iterator SET_FLOAT_ITER;
        SET_FLOAT setLength;
        setLength.insert( fIndexX );
        setLength.insert( fIndexY );
        setLength.insert( fIndexZ );

        float fLength[2];
        int nCount(0);
        SET_FLOAT_ITER iter = setLength.begin();
        for ( ; iter!=setLength.end() && nCount<2; ++iter, ++nCount )
        {
            fLength[nCount] = (*iter);
        }

        nTexelX = static_cast<int>( fLength[0] );
        nTexelY = static_cast<int>( fLength[1] );

        //D3DXVECTOR2 vDir = vMax - vMin;

        //if ( vDir.x > vDir.y )
        //{
        //    nTexelX = static_cast<int>( fIndex * vDir.x / vDir.y );
        //    nTexelY = static_cast<int>( fIndex );
        //}
        //else
        //{
        //    nTexelX = static_cast<int>( fIndex );
        //    nTexelY = static_cast<int>( fIndex * vDir.y / vDir.x );
        //}
    }

    //// UV로 텍셀 수를 계산한다. 
    //// UV 0~1 이면 Pixel 사이즈는 16 이다.
    //{
    //    D3DXVECTOR2 vDir = vMax - vMin;
    //    int nTexelX_UV = static_cast<int>( vDir.x * 16 );
    //    int nTexelY_UV = static_cast<int>( vDir.y * 16 );

    //    // 큰 것이 있다면 그것을 쓴다.
    //    if ( nTexelX < nTexelX_UV )
    //        nTexelX = nTexelX_UV;
    //    if ( nTexelY < nTexelY_UV )
    //        nTexelY = nTexelY_UV;
    //}

    //if( nTexelX < 14 )	nTexelX = 14;
	//if( nTexelY < 14 )	nTexelY = 14;
    if( nTexelX < 6 )	    nTexelX = 6;    // 8-2 = 6
	if( nTexelY < 6 )	    nTexelY = 6;    // 8-2 = 6
	if( nTexelX > 2046 )	nTexelX = 2046; // 2048-2 = 2046
	if( nTexelY > 2046 )	nTexelY = 2046; // 2048-2 = 2046

    // 텍셀 사이즈를 얻음.
    m_wwTexelSize.wA = nTexelX;
    m_wwTexelSize.wB = nTexelY;

    // UV - max, min 수치를 얻음.
    m_vMaxUV = vMax;
    m_vMinUV = vMin;
}

// 텍셀을 셋팅해준다.
void LightMapObject::SetTexelScale( WORDWORD wwTexelSize )
{
	// 텍셀 사이즈를 얻음.
	m_wwTexelSize = wwTexelSize;


	D3DXVECTOR2 vMin = D3DXVECTOR2( FLT_MAX, FLT_MAX );
	D3DXVECTOR2 vMax = D3DXVECTOR2( -FLT_MAX, -FLT_MAX );
	for ( DWORD i=0; i<m_vecVertices.size(); ++i )
	{
		NSLightMap::CalcMinMax( vMin, vMax, m_vecVertices[i].vTex[2] );
	}

	// UV - max, min 수치를 얻음.
	m_vMaxUV = vMax;
	m_vMinUV = vMin;
}

void LightMapObject::SetLightMapUV( const std::vector<D3DXVECTOR2>& vecUV, WORDWORD wwTexelSize )
{
	m_vecLightMapUV = vecUV;
	m_wwTexelTriangleSize = wwTexelSize;
}

void LightMapObject::CreateTexelTriangleID()
{
	sc::DefaultLock sDefaultLock;

	WORDWORD wwTexelSize = m_wwTexelTriangleSize;

	SAFE_DELETE_ARRAY ( m_pTexelTriangleID );
	m_pTexelTriangleID = new listTriangleID[wwTexelSize.wA*wwTexelSize.wB];

	D3DXVECTOR2 vOffSet( 1.f/wwTexelSize.wA, 1.f/wwTexelSize.wB );
	int nFace = static_cast<int>( m_vecLightMapUV.size() ) / 3;

#pragma omp parallel for
	for ( int i=0; i<nFace; ++i )
	{
		// 삼각형 UV 의 AABB 값을 구한다.
		D3DXVECTOR2 vUV[3];
		D3DXVECTOR2 vMax(0.f,0.f);
		D3DXVECTOR2 vMin(1.f,1.f);
		for ( DWORD z=0; z<3; ++z )
		{
			vUV[z] = m_vecLightMapUV[i*3+z];

			if ( vMax.x < vUV[z].x )
				vMax.x = vUV[z].x;
			if ( vMax.y < vUV[z].y )
				vMax.y = vUV[z].y;

			if ( vMin.x > vUV[z].x )
				vMin.x = vUV[z].x;
			if ( vMin.y > vUV[z].y )
				vMin.y = vUV[z].y;
		}

		// AABB와 연결될 만한 픽셀들을 구한다.
		int nStartX = static_cast<int>(vMin.x * wwTexelSize.wA) - 1;
		int nEndX = static_cast<int>(vMax.x * wwTexelSize.wA) + 1;
		int nStartY = static_cast<int>(vMin.y * wwTexelSize.wB) - 1;
		int nEndY = static_cast<int>(vMax.y * wwTexelSize.wB) + 1;

		if ( nStartX < 0)
			nStartX = 0;
		if ( nStartY < 0)
			nStartY = 0;

		if ( nEndX >= wwTexelSize.wA )
			nEndX = wwTexelSize.wA-1;
		if ( nEndY >= wwTexelSize.wB )
			nEndY = wwTexelSize.wB-1;

		// 픽셀들과 삼각형의 각 변들과 충돌처리가 되는지 확인 후 충돌된다면 삼각형이 걸려 있는 것이다.
		{
			D3DXVECTOR2 vDir;
			D3DXVECTOR2 vCross[3];
			vCross[0] = m_vecLightMapUV[i*3+1] - m_vecLightMapUV[i*3+0];
			vCross[1] = m_vecLightMapUV[i*3+2] - m_vecLightMapUV[i*3+1];
			vCross[2] = m_vecLightMapUV[i*3+0] - m_vecLightMapUV[i*3+2];

			D3DXMATRIX matRotate;
			D3DXMatrixRotationZ( &matRotate, D3DX_PI*0.5f );
			D3DXVec2TransformNormal( &vCross[0], &vCross[0], &matRotate );
			D3DXVec2TransformNormal( &vCross[1], &vCross[1], &matRotate );
			D3DXVec2TransformNormal( &vCross[2], &vCross[2], &matRotate );

			// 제대로 Cross 값이 나왔는지 확인 작업.
			{
				D3DXVECTOR2 vCenter = m_vecLightMapUV[i*3+0];
				vCenter += m_vecLightMapUV[i*3+1];
				vCenter += m_vecLightMapUV[i*3+2];
				vCenter /= 3;

				// 방향이 안 맞아서 돌려 놓는다.
				vDir = vCenter - m_vecLightMapUV[i*3+0];
				if ( D3DXVec2Dot( &vDir, &vCross[0]) < 0 )
				{
					vCross[0] = -vCross[0];
					vCross[1] = -vCross[1];
					vCross[2] = -vCross[2];
				}
			}

			for ( int y=nStartY; y<=nEndY; ++y )
			{
				for ( int x=nStartX; x<=nEndX; ++x )
				{
					D3DXVECTOR2 sSquare[4];
					sSquare[0] = D3DXVECTOR2( static_cast<float>(x)/(wwTexelSize.wA), static_cast<float>(y)/(wwTexelSize.wB) );
					sSquare[1] = D3DXVECTOR2( static_cast<float>(x+1)/(wwTexelSize.wA), static_cast<float>(y)/(wwTexelSize.wB) );
					sSquare[2] = D3DXVECTOR2( static_cast<float>(x+1)/(wwTexelSize.wA), static_cast<float>(y+1)/(wwTexelSize.wB) );
					sSquare[3] = D3DXVECTOR2( static_cast<float>(x)/(wwTexelSize.wA), static_cast<float>(y+1)/(wwTexelSize.wB) );

					if ( COLLISION::IsCollision2DTriangleToSquare( &m_vecLightMapUV[i*3], &vCross[0], &sSquare[0] ) )
					{
						sDefaultLock.lockOn();

						if ( m_pTexelTriangleID[y*wwTexelSize.wA+x].nFaceNUM > 0 )
						{
							++m_pTexelTriangleID[y*wwTexelSize.wA+x].nFaceNUM;
							DWORD* pBackUp = new DWORD[m_pTexelTriangleID[y*wwTexelSize.wA+x].nFaceNUM];
							memcpy ( pBackUp, m_pTexelTriangleID[y*wwTexelSize.wA+x].pTriangleID, sizeof(DWORD)*m_pTexelTriangleID[y*wwTexelSize.wA+x].nFaceNUM-1 );
							pBackUp[ m_pTexelTriangleID[y*wwTexelSize.wA+x].nFaceNUM-1 ] = i;

							SAFE_DELETE_ARRAY ( m_pTexelTriangleID[y*wwTexelSize.wA+x].pTriangleID );
							m_pTexelTriangleID[y*wwTexelSize.wA+x].pTriangleID = pBackUp;
							pBackUp = NULL;

							//++m_pTexelTriangleID[y*wwTexelSize.wA+x].nFaceNUM;
							//SAFE_DELETE_ARRAY ( m_pTexelTriangleID[y*wwTexelSize.wA+x].pTriangleID );
							//m_pTexelTriangleID[y*wwTexelSize.wA+x].pTriangleID = new DWORD[m_pTexelTriangleID[y*wwTexelSize.wA+x].nFaceNUM];
							//memcpy ( m_pTexelTriangleID[y*wwTexelSize.wA+x].pTriangleID, pBackUp, sizeof(DWORD)*m_pTexelTriangleID[y*wwTexelSize.wA+x].nFaceNUM-1 );
						}
						else
						{
							++m_pTexelTriangleID[y*wwTexelSize.wA+x].nFaceNUM;
							SAFE_DELETE_ARRAY ( m_pTexelTriangleID[y*wwTexelSize.wA+x].pTriangleID );
							m_pTexelTriangleID[y*wwTexelSize.wA+x].pTriangleID = new DWORD[m_pTexelTriangleID[y*wwTexelSize.wA+x].nFaceNUM];
							m_pTexelTriangleID[y*wwTexelSize.wA+x].pTriangleID[ m_pTexelTriangleID[y*wwTexelSize.wA+x].nFaceNUM-1 ] = i;
						}

						sDefaultLock.lockOff();
					}
				}
			}
		}
	}
}

void LightMapObject::DeleteTexelTriangleID()
{
	SAFE_DELETE_ARRAY ( m_pTexelTriangleID );
}

// 텍셀을 만들고 계산하고..
void LightMapObject::CreateTexel( LPDIRECT3DDEVICEQ pd3dDevice, 
								 DxStaticMeshCollBase* pStaticMesh, 
                                 DxPieceManager* pPieceManager, 
                                 const D3DXMATRIX& matWorld,
                                 BOOL _bDontMakeShadowLM,
								 const std::vector<BOOL>* pvecReceiveShadowLM,
								 int nSampling,
								 const WORDWORD* pwwLightMapUserUV_TEXEL,
								 const WORDWORD* pwwLightID4Piece,
								 BOOL& bEnoughUsedPixelLightMap )
{
  //  // UV 값을 정리한다. 0~1 의 수치를 구한다.
  //  D3DXVECTOR2 vUV;
  //  std::vector<D3DXVECTOR2> vecUV;
  //  for ( DWORD i=0; i<m_vecLightMapUV.size(); ++i )
  //  {
  //      //vUV.x = (m_vecVertices[i].vTex[2].x-m_vMinUV.x) / (m_vMaxUV.x-m_vMinUV.x);
  //      //vUV.y = (m_vecVertices[i].vTex[2].y-m_vMinUV.y) / (m_vMaxUV.y-m_vMinUV.y);
		//vUV.x = m_vecVertices[i].vTex[2].x;
		//vUV.y = m_vecVertices[i].vTex[2].y;
  //      vecUV.push_back( vUV );

  //      // 같은 컬러인지 검사하지 않을때 실제 값을 넣는다.
  //      m_vecVertices[i].vTex[1].x = vUV.x;
  //      m_vecVertices[i].vTex[1].y = vUV.y;
  //  }

	WORDWORD wwBaseTexelXY;
	WORDWORD wwFullSize;
	if ( pwwLightMapUserUV_TEXEL )
	{
		wwBaseTexelXY = m_wwBaseTexelXY;
		wwFullSize = *pwwLightMapUserUV_TEXEL;
	}
	else
	{
		wwBaseTexelXY.dwData = 0L;
		wwFullSize = m_wwTexelSize;
	}

    std::vector<VERTEX_WORLD> vecVerticesMatrix;  // Matrix 연산 된것
    for ( DWORD i=0; i<m_vecVertices.size(); ++i )
    {
        vecVerticesMatrix.push_back( m_vecVertices[i] );

        D3DXVec3TransformCoord( &vecVerticesMatrix[i].vPos, &vecVerticesMatrix[i].vPos, &matWorld );
        D3DXVec3TransformNormal( &vecVerticesMatrix[i].vNor, &vecVerticesMatrix[i].vNor, &matWorld );
    }

	// 순수 LightMap 을 만들때는 m_listRadiosityRenderData 정보를 지우도록 한다.
	m_listRadiosityRenderData.clear();

    // 각 Texel 별로 Color 계산을 한다.
    {
		DWORD dwTexelSize = static_cast<DWORD>(m_wwTexelSize.wA) * m_wwTexelSize.wB;

        {
			SAFE_DELETE_ARRAY( m_pInTriangle );
			m_pInTriangle = new bool[ dwTexelSize ];

            SAFE_DELETE_ARRAY( m_pTexelCombined );
            m_pTexelCombined = new DWORD[ dwTexelSize ];

			SAFE_DELETE_ARRAY( m_pTexelDirect_Shadow );
			m_pTexelDirect_Shadow = new DWORD[ dwTexelSize ];

			SAFE_DELETE_ARRAY( m_pTexelIndirect );
			m_pTexelIndirect = new DWORD[ dwTexelSize ];

			SAFE_DELETE_ARRAY( m_pTexelNoDynamicLight );
			m_pTexelNoDynamicLight = new DWORD[ dwTexelSize ];
        }

		CreateTexelForRADNormal
		( 
			pd3dDevice, 
			pStaticMesh,
			pPieceManager,
			vecVerticesMatrix,
			_bDontMakeShadowLM,
			pvecReceiveShadowLM,
			nSampling,
			wwBaseTexelXY,
			wwFullSize,
			pwwLightID4Piece
		);

		// Pixel 을 충분히 사용하고 있는지 확인한다.
		// 낭비하는 부분이 많다면, 사용자에게 알리기 위해 FALSE 셋팅을 한다.
		{
			DWORD dwUsedPixel(0L);
			for ( DWORD i=0; i<dwTexelSize; ++i )
			{
				if ( m_pInTriangle[i] )
					++dwUsedPixel;
			}

			// 기준 수치보다 적다. 현재는 50%
			if ( dwUsedPixel < dwTexelSize*0.5f )
			{
				bEnoughUsedPixelLightMap = FALSE;
			}
		}
    }
}

void LightMapObject::CreateTexelForRADNormal( LPDIRECT3DDEVICEQ pd3dDevice, 
											 DxStaticMeshCollBase* pStaticMesh, 
											 DxPieceManager* pPieceManager,
											 std::vector<VERTEX_WORLD>& vecVerticesMatrix,
											 BOOL _bDontMakeShadowLM,
											 const std::vector<BOOL>* pvecReceiveShadowLM,
											 int nSampling,
											 WORDWORD wwBaseTexelXY,
											 WORDWORD wwLightMapUserUV_TEXEL,
											 const WORDWORD* pwwLightID4Piece )
{
	float fUpdateTime(1.f);
	int nTexLineCount(0);
	int nTexLineCount_FULL(0);
	BOOL bActiveCheckSameColor(FALSE);
	DWORD dwFirstColorDay = 0xff000000 + (rand()%0xffffff);
	DWORD dwFirstColorNight = 0xff000000 + (rand()%0xffffff);

	WORDWORD* pwwInTriangleSpot = new WORDWORD[ m_wwTexelSize.wA*m_wwTexelSize.wB ];

	sc::DefaultLock		sDefaultLock;

#pragma omp parallel for
	for ( int y=0; y<static_cast<int>(m_wwTexelSize.wB); ++y )
	{
		for ( DWORD x=0; x<m_wwTexelSize.wA; ++x )
		{
			//// --------------------------------------------테스트 코드---------------------------------------------------------------
			//if ( !bCheckSameColor )
			//{
			//    m_pTexelCombined[y*m_wwTexelSize.wA+x] = dwColor;
			//    m_pTexelNight[y*m_wwTexelSize.wA+x] = dwColor;
			//    continue;
			//}

			D3DXVECTOR3 vColorDay = D3DXVECTOR3(0.f,0.f,0.f);
			D3DXVECTOR4 vColorDirect_Shadow = D3DXVECTOR4(0.f,0.f,0.f,0.f);
			D3DXVECTOR3 vColorIndirect = D3DXVECTOR3(0.f,0.f,0.f);
			D3DXVECTOR3 vColorNoDynamicLight = D3DXVECTOR3(0.f,0.f,0.f);
			BOOL bInTriangle = SuperSamplingMain
			( 
				pd3dDevice, 
				pStaticMesh, 
				pPieceManager, 
				x, 
				y, 
				nSampling, 
				vColorDay,
				vColorDirect_Shadow,
				vColorIndirect,
				vColorNoDynamicLight,
				m_vecLightMapUV,
				vecVerticesMatrix,
				_bDontMakeShadowLM,
				pvecReceiveShadowLM,
				wwBaseTexelXY,
				wwLightMapUserUV_TEXEL,
				pwwLightID4Piece,
				&pwwInTriangleSpot[y*m_wwTexelSize.wA+x]
			);


			{
				m_pInTriangle[y*m_wwTexelSize.wA+x] = bInTriangle;
				m_pTexelCombined[y*m_wwTexelSize.wA+x] = NSLightMapTools::ConvertColor( vColorDay, 85 );	//255 );
				m_pTexelDirect_Shadow[y*m_wwTexelSize.wA+x] = NSLightMapTools::ConvertColor( vColorDirect_Shadow, 255, 85, 255, 85 );
				m_pTexelIndirect[y*m_wwTexelSize.wA+x] = NSLightMapTools::ConvertColor( vColorIndirect, 85 );
				m_pTexelNoDynamicLight[y*m_wwTexelSize.wA+x] = NSLightMapTools::ConvertColor( vColorNoDynamicLight, 85 );
			}
		}

		// Pixel 계산
		sDefaultLock.lockOn();
		{
			++nTexLineCount;
			++nTexLineCount_FULL;

			if( nTexLineCount >= 100 )
			{
				nTexLineCount = 0;
				TSTRINGSTREAM strStringStream;
				strStringStream << "CreateTexel - ";
				strStringStream << nTexLineCount_FULL;
				strStringStream << "/";
				strStringStream << m_wwTexelSize.wB;
				ExportProgress::SetStateString( strStringStream.str().c_str() );
			}
		}
		sDefaultLock.lockOff();
	}

	// Edge Color 를 적용한다.
	EdgeColorApply( m_pTexelCombined );
	EdgeColorApply( m_pTexelIndirect );
	EdgeColorApply( m_pTexelNoDynamicLight );
	EdgeColorApply_SDSD( m_pTexelDirect_Shadow, pwwInTriangleSpot );

	SAFE_DELETE_ARRAY( pwwInTriangleSpot );
}

// Edge Color 를 적용한다.
void LightMapObject::EdgeColorApply( DWORD* pTexelColor )
{
#pragma omp parallel for
	for ( int y=0; y<static_cast<int>(m_wwTexelSize.wB); ++y )
	{
		for ( int x=0; x<static_cast<int>(m_wwTexelSize.wA); ++x )
		{
			// 삼각형 안에 있는 것이라면 할 필요 없다.
			if ( m_pInTriangle[y*m_wwTexelSize.wA+x] )
				continue;

			DWORD dwA(0);
			DWORD dwR(0);
			DWORD dwG(0);
			DWORD dwB(0);
			int nCount(0);
			for ( int j=-1; j<2; ++j )
			{
				int yy = y+j;
				if ( yy < 0 )
					continue;

				if ( yy >= m_wwTexelSize.wB )
					continue;

				for ( int i=-1; i<2; ++i )
				{
					int xx = x+i;
					if ( xx < 0 )
						continue;

					if ( xx >= m_wwTexelSize.wA )
						continue;

					if ( m_pInTriangle[yy*m_wwTexelSize.wA+xx] )
					{
						++nCount;
						dwA += (pTexelColor[yy*m_wwTexelSize.wA+xx] & 0xff000000) >> 24;
						dwR += (pTexelColor[yy*m_wwTexelSize.wA+xx] & 0xff0000) >> 16;
						dwG += (pTexelColor[yy*m_wwTexelSize.wA+xx] & 0xff00) >> 8;
						dwB += pTexelColor[yy*m_wwTexelSize.wA+xx] & 0xff;
					}
				}
			}

			if( nCount > 0 )
			{
				dwA = dwA / nCount;
				dwR = dwR / nCount;
				dwG = dwG / nCount;
				dwB = dwB / nCount;

				pTexelColor[y*m_wwTexelSize.wA+x] = (dwA<<24) + (dwR<<16) + (dwG<<8) + dwB;
			}			
		}
	}
}

// Edge Color 를 적용한다.
void LightMapObject::EdgeColorApply_SDSD( DWORD* pTexelColor, const WORDWORD* pwwInTriangleSpot )
{
#pragma omp parallel for
	for ( int y=0; y<static_cast<int>(m_wwTexelSize.wB); ++y )
	{
		for ( int x=0; x<static_cast<int>(m_wwTexelSize.wA); ++x )
		{
			// 삼각형 밖에 있는건만 계산한다.
			if ( pwwInTriangleSpot[y*m_wwTexelSize.wA+x].wA == 0 )
			{
				
				DWORD dwA(0);
				DWORD dwB(0);
				int nCount(0);
				for ( int j=-1; j<2; ++j )
				{
					int yy = y+j;
					if ( yy < 0 )
						continue;

					if ( yy >= m_wwTexelSize.wB )
						continue;

					for ( int i=-1; i<2; ++i )
					{
						int xx = x+i;
						if ( xx < 0 )
							continue;

						if ( xx >= m_wwTexelSize.wA )
							continue;

						if ( pwwInTriangleSpot[yy*m_wwTexelSize.wA+xx].wA == 1 )
						{
							++nCount;
							dwA += (pTexelColor[yy*m_wwTexelSize.wA+xx] & 0xff000000) >> 24;
							dwB += pTexelColor[yy*m_wwTexelSize.wA+xx] & 0xff;
						}
					}
				}

				if( nCount > 0 )
				{
					dwA = dwA / nCount;
					dwB = dwB / nCount;

					DWORD dwR(0);
					DWORD dwG(0);
					dwR = (pTexelColor[y*m_wwTexelSize.wA+x] & 0xff0000) >> 16;
					dwG = (pTexelColor[y*m_wwTexelSize.wA+x] & 0xff00) >> 8;

					pTexelColor[y*m_wwTexelSize.wA+x] = (dwA<<24) + (dwR<<16) + (dwG<<8) + dwB;
				}	
			}

			if ( pwwInTriangleSpot[y*m_wwTexelSize.wA+x].wB == 0 )
			{
				DWORD dwR(0);
				DWORD dwG(0);
				int nCount(0);
				for ( int j=-1; j<2; ++j )
				{
					int yy = y+j;
					if ( yy < 0 )
						continue;

					if ( yy >= m_wwTexelSize.wB )
						continue;

					for ( int i=-1; i<2; ++i )
					{
						int xx = x+i;
						if ( xx < 0 )
							continue;

						if ( xx >= m_wwTexelSize.wA )
							continue;

						if ( pwwInTriangleSpot[yy*m_wwTexelSize.wA+xx].wB == 1 )
						{
							++nCount;
							dwR += (pTexelColor[yy*m_wwTexelSize.wA+xx] & 0xff0000) >> 16;
							dwG += (pTexelColor[yy*m_wwTexelSize.wA+xx] & 0xff00) >> 8;
						}
					}
				}

				if( nCount > 0 )
				{
					dwR = dwR / nCount;
					dwG = dwG / nCount;

					DWORD dwA(0);
					DWORD dwB(0);
					dwA = (pTexelColor[y*m_wwTexelSize.wA+x] & 0xff000000) >> 24;
					dwB = pTexelColor[y*m_wwTexelSize.wA+x] & 0xff;

					pTexelColor[y*m_wwTexelSize.wA+x] = (dwA<<24) + (dwR<<16) + (dwG<<8) + dwB;
				}	
			}
		}
	}
}

// 슈퍼샘플링
BOOL LightMapObject::SuperSamplingMain( LPDIRECT3DDEVICEQ pd3dDevice, 
									   DxStaticMeshCollBase* pStaticMesh, 
									   DxPieceManager* pPieceManager, 
									   DWORD x, 
									   DWORD y, 
									   int nSampling, 
									   D3DXVECTOR3& vColorDay, 
									   D3DXVECTOR4& vColorDirect_Shadow,
									   D3DXVECTOR3& vColorIndirect,
									   D3DXVECTOR3& vColorNoDynamicLight,
									   const std::vector<D3DXVECTOR2>& vecUV,
									   const std::vector<VERTEX_WORLD>& vecVerticesMatrix,
									   BOOL _bDontMakeShadowLM,
									   const std::vector<BOOL>* pvecReceiveShadowLM,
									   WORDWORD wwBaseTexelXY,
									   WORDWORD wwLightMapUserUV_TEXEL,
									   const WORDWORD* pwwLightID4Piece,
									   WORDWORD* pwwInTriangleSpot )
{
	//strText.Format ( "1 sample" );1
	//strText.Format ( "Quincunx" );2
	//strText.Format ( "2x2 grid" );3
	//strText.Format ( "2x2 RGSS" );4
	//strText.Format ( "8 rooks" );5
	int nSampleNUM(1);
	int nTexDetail(0);
	std::vector<D3DXVECTOR2> vecPointRate;
	switch ( nSampling )
	{
	case 1:	// 1 sample
		nTexDetail = 0;
		vecPointRate.push_back( D3DXVECTOR2( 0.5f, 0.5f ) );
		break;

	//case 2:	// Quincunx
	//	nSampleNUM = 5;
	//	vecPointRate.push_back( 0.0f, 0.0f );
	//	vecPointRate.push_back( 0.1f, 0.0f );
	//	vecPointRate.push_back( 0.0f, 0.1f );
	//	vecPointRate.push_back( 0.1f, 0.1f );
	//	vecPointRate.push_back( 0.5f, 0.5f );
	//	break;

	case 3:	// 2x2 grid
		nTexDetail = 1;
		vecPointRate.push_back( D3DXVECTOR2( 0.25f, 0.25f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.75f, 0.25f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.25f, 0.75f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.75f, 0.75f ) );
		break;

	case 4:	// 2x2 RGSS
		nTexDetail = 2;
		vecPointRate.push_back( D3DXVECTOR2( 0.125f, 0.125f+0.25f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.125f+0.5f, 0.125f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.125f+0.25f, 0.125f+0.75f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.125f+0.75f, 0.125f+0.5f ) );
		break;

	case 5:	// 8 rooks
		nTexDetail = 4;
		vecPointRate.push_back( D3DXVECTOR2( 0.025f, 0.25f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.25f, 0.15f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.55f, 0.025f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.1f, 0.53f ) );
		vecPointRate.push_back( D3DXVECTOR2( 1.f-0.025f, 1.f-0.25f ) );
		vecPointRate.push_back( D3DXVECTOR2( 1.f-0.25f, 1.f-0.15f ) );
		vecPointRate.push_back( D3DXVECTOR2( 1.f-0.55f, 1.f-0.025f ) );
		vecPointRate.push_back( D3DXVECTOR2( 1.f-0.1f, 1.f-0.53f ) );
		break;

	case 6:	// 4x4 grid
		nTexDetail = 2;
		vecPointRate.push_back( D3DXVECTOR2( 0.125f,		0.125f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.125f+0.25f, 0.125f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.125f+0.5f,	0.125f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.125f+0.75f, 0.125f ) );

		vecPointRate.push_back( D3DXVECTOR2( 0.125f,		0.125f+0.25f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.125f+0.25f, 0.125f+0.25f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.125f+0.5f,	0.125f+0.25f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.125f+0.75f, 0.125f+0.25f ) );

		vecPointRate.push_back( D3DXVECTOR2( 0.125f,		0.125f+0.5f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.125f+0.25f, 0.125f+0.5f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.125f+0.5f,	0.125f+0.5f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.125f+0.75f, 0.125f+0.5f ) );

		vecPointRate.push_back( D3DXVECTOR2( 0.125f,		0.125f+0.75f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.125f+0.25f, 0.125f+0.75f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.125f+0.5f,	0.125f+0.75f ) );
		vecPointRate.push_back( D3DXVECTOR2( 0.125f+0.75f, 0.125f+0.75f ) );

		break;

	default:
		return FALSE;
	};

	return SuperSampling
	(
		pd3dDevice, 
		pStaticMesh, 
		pPieceManager, 
		x, 
		y, 
		nTexDetail,
		vecPointRate,
		vColorDay, 
		vColorDirect_Shadow,
		vColorIndirect,
		vColorNoDynamicLight,
		vecUV,
		vecVerticesMatrix, 
		_bDontMakeShadowLM,
		pvecReceiveShadowLM,
		wwBaseTexelXY,
		wwLightMapUserUV_TEXEL,
		pwwLightID4Piece,
		pwwInTriangleSpot
	);
}

// 슈퍼샘플링
BOOL LightMapObject::SuperSampling( LPDIRECT3DDEVICEQ pd3dDevice, 
								   DxStaticMeshCollBase* pStaticMesh, 
								   DxPieceManager* pPieceManager, 
								   DWORD x, 
								   DWORD y, 
								   int nTexelDetail,
								   const std::vector<D3DXVECTOR2>& vecPointRate,
								   D3DXVECTOR3& vColorDay, 
								   D3DXVECTOR4& vColorDirect_Shadow,
								   D3DXVECTOR3& vColorIndirect,
								   D3DXVECTOR3& vColorNoDynamicLight,
								   const std::vector<D3DXVECTOR2>& vecUV,
								   const std::vector<VERTEX_WORLD>& vecVerticesMatrix,
								   BOOL _bDontMakeShadowLM,
								   const std::vector<BOOL>* pvecReceiveShadowLM,
								   WORDWORD wwBaseTexelXY,
								   WORDWORD wwLightMapUserUV_TEXEL,
								   const WORDWORD* pwwLightID4Piece,
								   WORDWORD* pwwInTriangleSpot )
{
	DWORD dwFaceNUM(0);
	DWORD dwIndex[3];
	D3DXVECTOR3 avPos[3];
	D3DXVECTOR3 avNor[3];
	D3DXVECTOR2 avUV[3];
	D3DXVECTOR3 vPos(0.f,0.f,0.f);
	D3DXVECTOR3 vNor(0.f,0.f,0.f);
	D3DXVECTOR2 vUV(0.f,0.f);

	D3DXVECTOR3 vColorDay_Enable(0.f,0.f,0.f);
	D3DXVECTOR4 vColorDirect_Shadow_Enable(0.f,0.f,0.f,0.f);
	D3DXVECTOR3 vColorIndirect_Enable(0.f,0.f,0.f);
	D3DXVECTOR3 vColorNoDynamicLight_Enable(0.f,0.f,0.f);

	BOOL bInTriangle(FALSE);		// 삼각형 안에 있는가~?

	DWORD dwInTriangle(0);			// 삼각형 안에 있는게 몇개 있는가~?
	for ( DWORD i=0; i<vecPointRate.size(); ++i )
	{
		vUV.x = static_cast<float>(x + vecPointRate[i].x + wwBaseTexelXY.wA) / (wwLightMapUserUV_TEXEL.wA);
		vUV.y = static_cast<float>(y + vecPointRate[i].y + wwBaseTexelXY.wB) / (wwLightMapUserUV_TEXEL.wB);

		// Note : 가장 가까운 삼각형을 구한다.
		float u, v;
		VEC_DWORD vecFaceNUM;
		for ( int i=0; i<m_pTexelTriangleID[(y+wwBaseTexelXY.wB)*wwLightMapUserUV_TEXEL.wA+(x+wwBaseTexelXY.wA)].nFaceNUM; ++i )
		{
			vecFaceNUM.push_back( m_pTexelTriangleID[(y+wwBaseTexelXY.wB)*wwLightMapUserUV_TEXEL.wA+(x+wwBaseTexelXY.wA)].pTriangleID[i] );
		}
		DWORD dwMainIndex(0);
		bInTriangle = NSLightMapTools::FindNearTriangle( vecUV, vUV, vecFaceNUM, dwFaceNUM, u, v, dwMainIndex );

		if ( bInTriangle )
		{
			++dwInTriangle;

			BOOL bDontMakeShadowLM(_bDontMakeShadowLM);
			if ( pvecReceiveShadowLM )
			{
				if ( dwFaceNUM < (*pvecReceiveShadowLM).size() )
				{
					bDontMakeShadowLM = (*pvecReceiveShadowLM)[ dwFaceNUM ];
				}
			}

			// Note : 노멀을 구한다.
			switch( dwMainIndex )
			{
			case 1:
				{
					dwIndex[0] = dwFaceNUM*3 + 1;
					dwIndex[1] = dwFaceNUM*3 + 2;
					dwIndex[2] = dwFaceNUM*3 + 0;
				}
				break;

			case 2:
				{
					dwIndex[0] = dwFaceNUM*3 + 2;
					dwIndex[1] = dwFaceNUM*3 + 0;
					dwIndex[2] = dwFaceNUM*3 + 1;
				}
				break;

			case 0:
			default:
				{
					dwIndex[0] = dwFaceNUM*3 + 0;
					dwIndex[1] = dwFaceNUM*3 + 1;
					dwIndex[2] = dwFaceNUM*3 + 2;
				}
				break;
			}

			avPos[0] = vecVerticesMatrix[ dwIndex[0] ].vPos;    // Matrix 연산 된 것 사용
			avPos[1] = vecVerticesMatrix[ dwIndex[1] ].vPos;    // Matrix 연산 된 것 사용
			avPos[2] = vecVerticesMatrix[ dwIndex[2] ].vPos;    // Matrix 연산 된 것 사용

			avNor[0] = vecVerticesMatrix[ dwIndex[0] ].vNor;    // Matrix 연산 된 것 사용
			avNor[1] = vecVerticesMatrix[ dwIndex[1] ].vNor;    // Matrix 연산 된 것 사용
			avNor[2] = vecVerticesMatrix[ dwIndex[2] ].vNor;    // Matrix 연산 된 것 사용

			{
				D3DXVECTOR3 vPos_Dir[2];
				vPos_Dir[0] = avPos[1] - avPos[0];
				vPos_Dir[1] = avPos[2] - avPos[0];
				vPos = avPos[0] + vPos_Dir[0]*( u ) + vPos_Dir[1]*( v );

				D3DXVECTOR3 vNor_Dir[2];
				vNor_Dir[0] = avNor[1] - avNor[0];
				vNor_Dir[1] = avNor[2] - avNor[0];
				vNor = avNor[0] + vNor_Dir[0]*( u ) + vNor_Dir[1]*( v );

				D3DXVec3Normalize( &vNor, &vNor );
			}

			// [CascadeShadow] 작업보류중
			// Color 를 얻어온다.
			if ( pwwLightID4Piece )
			{
				// Piece
				NSLightMapTools::GetColor
				( 
					pd3dDevice, 
					pStaticMesh, 
					pPieceManager, 
					vPos, 
					vNor,
					bDontMakeShadowLM, 
					vColorDay, 
					vColorDirect_Shadow, 
					vColorIndirect, 
					vColorNoDynamicLight,
					*pwwLightID4Piece, 
					NULL 
				);
			}
			else
			{
				// Field
				NSLightMapTools::GetColor
				( 
					pd3dDevice, 
					pStaticMesh, 
					pPieceManager, 
					vPos, 
					vNor, 
					bDontMakeShadowLM, 
					vColorDay, 
					vColorDirect_Shadow, 
					vColorIndirect, 
					vColorNoDynamicLight,
					m_vecTriangle_wwLightID[dwFaceNUM], 
					pwwInTriangleSpot 
				);
			}

			//// [CascadeShadow] 작업보류중	- 삭제 필요.
			//std::vector<DWORD> vecLight;
			//NSLightMapTools::GetColor( pd3dDevice, pStaticMesh, pPieceManager, vPos, vNor, bDontMakeShadowLM, vColorDay, vColorDirect_Shadow, vColorIndirect, vecLight );

			vColorDay_Enable += vColorDay;
			vColorDirect_Shadow_Enable += vColorDirect_Shadow;
			vColorIndirect_Enable += vColorIndirect;
			vColorNoDynamicLight_Enable += vColorNoDynamicLight;
		}
	}

	if ( dwInTriangle > 0 )
	{
		vColorDay = vColorDay_Enable / static_cast<float>( dwInTriangle );	//(nSampling*nSampling);
		vColorDirect_Shadow = vColorDirect_Shadow_Enable / static_cast<float>( dwInTriangle );	//(nSampling*nSampling);
		vColorIndirect = vColorIndirect_Enable / static_cast<float>( dwInTriangle );	//(nSampling*nSampling);
		vColorNoDynamicLight = vColorNoDynamicLight_Enable / static_cast<float>( dwInTriangle );	//(nSampling*nSampling);
		return TRUE;
	}

	return FALSE;
}

// Texture에 맞게 UV를 재계산한다.
void LightMapObject::CaleUV( std::vector<D3DXVECTOR2>& vecLightMapUV, DWORD dwOffsetX, DWORD dwOffsetY )
{
    // UV 값을 정리한다.
    for ( DWORD i=0; i<vecLightMapUV.size(); ++i )
    {        
        //                           // ----- 0 ~ 1 -------------   // 0~64(128,256,512) // 텍스쳐에 맞게 UV를 셋팅
        vecLightMapUV[i].x = ( vecLightMapUV[i].x * m_wwTexelSize.wA ) / m_wwTextureSize.wA;
        vecLightMapUV[i].y = ( vecLightMapUV[i].y * m_wwTexelSize.wB ) / m_wwTextureSize.wB;
        vecLightMapUV[i].x += static_cast<float>(m_wwBaseTexelXY.wA) / m_wwTextureSize.wA;
        vecLightMapUV[i].y += static_cast<float>(m_wwBaseTexelXY.wB) / m_wwTextureSize.wB;
        vecLightMapUV[i].x += static_cast<float>(dwOffsetX) / m_wwTextureSize.wA;
        vecLightMapUV[i].y += static_cast<float>(dwOffsetY) / m_wwTextureSize.wB;
    }

    //// Piece 가 아닐 경우만 m_wwBaseTexelXY 기준으로 UV를 이동한다.
    //if ( !bPiece )
    //{
    //    // 최소 BaseTexel를 얻어온다.
    //    WORDWORD wwBaseTexelXY(4095,4095);
    //    {
    //        for ( DWORD i=0; i<m_vecVertices.size(); ++i )
    //        {
    //            if ( wwBaseTexelXY.wA > m_wwBaseTexelXY.wA )
    //                wwBaseTexelXY.wA = m_wwBaseTexelXY.wA;
    //            if ( wwBaseTexelXY.wB > m_wwBaseTexelXY.wB )
    //                wwBaseTexelXY.wB = m_wwBaseTexelXY.wB;
    //        }
    //    }

    //    // 최소 BaseTexel를 얻어온다.
    //    {
    //        for ( DWORD i=0; i<m_vecVertices.size(); ++i )
    //        {
    //            m_vecVertices[i].vTex[1].x -= static_cast<float>(wwBaseTexelXY.wA) / dwTexWidth;
    //            m_vecVertices[i].vTex[1].y -= static_cast<float>(wwBaseTexelXY.wB) / dwTexHeight;
    //        }
    //    }
    //}
}

// Texture에 맞게 UV를 재계산한다.
void LightMapObject::CaleUV( DWORD dwOffsetX, DWORD dwOffsetY )
{
	// 작업한 것은 다시 하지 않는다.
	if ( m_bGaussBlurSetUV )
		return;

	m_bGaussBlurSetUV = TRUE;

	CaleUV( m_vecLightMapUV, dwOffsetX, dwOffsetY );
}

// UV에 - 연산을 해준다.
void LightMapObject::SubUV( std::vector<D3DXVECTOR2>& vecLightMapUV, WORDWORD wwBaseTexelXY )
{
	float fUV_X = static_cast<float>(wwBaseTexelXY.wA) / m_wwTextureSize.wA;
	float fUV_Y = static_cast<float>(wwBaseTexelXY.wB) / m_wwTextureSize.wB;

	for ( DWORD i=0; i<vecLightMapUV.size(); ++i )
	{
		vecLightMapUV[i].x -= fUV_X;
		vecLightMapUV[i].y -= fUV_Y;
	}
}

// UV에 - 연산을 해준다.
void LightMapObject::SubUV( WORDWORD wwBaseTexelXY )
{
	SubUV( m_vecLightMapUV, wwBaseTexelXY );
}
//
//// VERTEXNORTEX2 정보를 얻어온다.
//void LightMapObject::GetVerticesData( VERTEXNORTEX2* pVertices, DWORD dwFaceStartNum, DWORD& dwFaceNum )
//{
//    dwFaceNum += static_cast< DWORD >( m_vecVertices.size() ) / 3;
//    for ( DWORD i=0; i<m_vecVertices.size()/3; ++i )
//    {
//        for ( DWORD j=0; j<3; ++j )
//        {
//            pVertices[(dwFaceStartNum+i)*3+j].vPos = m_vecVertices[i*3+j].vPos;
//            pVertices[(dwFaceStartNum+i)*3+j].vNor = m_vecVertices[i*3+j].vNor;
//            pVertices[(dwFaceStartNum+i)*3+j].vTex[0] = m_vecVertices[i*3+j].vTex[0];
//            pVertices[(dwFaceStartNum+i)*3+j].vTex[1] = m_vecVertices[i*3+j].vTex[1];
//        }
//    }
//}
//
//void LightMapObject::GetVerticesData( VERTEXNORCOLORTEX2* pVertices, DWORD dwFaceStartNum, DWORD& dwFaceNum )
//{
//    dwFaceNum += static_cast< DWORD >( m_vecVertices.size() ) / 3;
//    for ( DWORD i=0; i<m_vecVertices.size()/3; ++i )
//    {
//        for ( DWORD j=0; j<3; ++j )
//        {
//            pVertices[(dwFaceStartNum+i)*3+j].vPos = m_vecVertices[i*3+j].vPos;
//            pVertices[(dwFaceStartNum+i)*3+j].vNor = m_vecVertices[i*3+j].vNor;
//            pVertices[(dwFaceStartNum+i)*3+j].dwColor = m_vecVertices[i*3+j].dwColor;
//            pVertices[(dwFaceStartNum+i)*3+j].vTex[0] = m_vecVertices[i*3+j].vTex[0];
//            pVertices[(dwFaceStartNum+i)*3+j].vTex[1] = m_vecVertices[i*3+j].vTex[1];
//        }
//    }
//}

// LightMap UV 셋팅
void LightMapObject::SetLightMapUV( std::vector<VERTEX_WORLD>& vecVertices, DWORD*pIndices, DWORD dwLightMapUVIndex )
{
    DWORD dwCount(0);
    VEC_DWORD_ITER iter = m_vecFaces.begin();
    for ( ; iter!=m_vecFaces.end(); ++iter )
    {
        for ( DWORD i=0; i<3; ++i, ++dwCount )
        {
            DWORD dwIndex = pIndices[(*iter)*3+i];

            if ( dwIndex >= vecVertices.size() )
                return; // 에러임.

            if ( dwCount >= m_vecVertices.size() )
                return; // 에러임.

            vecVertices[dwIndex].vTex[ dwLightMapUVIndex ] = m_vecLightMapUV[dwCount];
        }
    }
}

void LightMapObject::Export( LightMapObject_Data& sData )
{
    sData.m_bChangeUV_XZ				= m_bChangeUV_XZ;
    sData.m_wwBaseTexelXY				= m_wwBaseTexelXY;
    sData.m_wwTexelSize					= m_wwTexelSize;
    sData.m_wwTextureSize				= m_wwTextureSize;
    sData.m_vMaxUV						= m_vMaxUV;
    sData.m_vMinUV						= m_vMinUV;
    sData.m_strTextureCombined			= m_strTextureCombined;
    sData.m_strTextureDirect_Shadow_1	= m_strTextureDirect_Shadow_1;
	sData.m_strTextureDirect_Shadow_2	= m_strTextureDirect_Shadow_2;
	sData.m_strTextureIndirect			= m_strTextureIndirect;
}

// RAD Normal 을 위한 Vertex 정보 셋팅.
void LightMapObject::SetVertexForRADNormal( LPDIRECT3DDEVICEQ pd3dDevice, std::vector<VERTEX_WORLD>& vecVerticesMatrix, DWORD dwRADNormalIndex, const D3DXMATRIX& matWorld )
{
	if ( (dwRADNormalIndex!=0) && (dwRADNormalIndex!=1) && (dwRADNormalIndex!=2) )
	{
		for ( DWORD i=0; i<m_vecVertices.size(); ++i )
		{
			vecVerticesMatrix.push_back( m_vecVertices[i] );

			D3DXVec3TransformCoord( &vecVerticesMatrix[i].vPos, &vecVerticesMatrix[i].vPos, &matWorld );
			D3DXVec3TransformNormal( &vecVerticesMatrix[i].vNor, &vecVerticesMatrix[i].vNor, &matWorld );
		}
	}
	else
	{
		D3DXVECTOR3 vBumpBasis;
		switch ( dwRADNormalIndex )
		{
		case 0:
			vBumpBasis.x = 0.81649661064147949f;
			vBumpBasis.y = 0.0f;
			vBumpBasis.z = 0.57735025882720947f;
			break;

		case 1:
			vBumpBasis.x = -0.40824833512306213f;
			vBumpBasis.y = 0.70710676908493042f;
			vBumpBasis.z = 0.57735025882720947f;
			break;

		case 2:
			vBumpBasis.x = -0.40824821591377258f;
			vBumpBasis.y = -0.70710676908493042f;
			vBumpBasis.z = 0.57735025882720947f;
			break;

		default:
			vBumpBasis.x = 0.f;
			vBumpBasis.y = 0.f;
			vBumpBasis.z = 1.f;
			break;
		}

		std::vector<D3DXVECTOR3>		vecTangent;      // Tangent
		std::vector<D3DXVECTOR3>		vecBinormal;     // Binormal

		// Tangent Space 로 작동
		DWORD dwVertexNum = static_cast<DWORD>( m_vecVertices.size() );
		if ( dwVertexNum > 0 )
		{
			// Note : 일단 메쉬를 만듬.
			LPD3DXMESH pMesh(NULL);
			if ( dwVertexNum > 65000 )
			{
				D3DXCreateMeshFVF( dwVertexNum/3, dwVertexNum, D3DXMESH_32BIT|D3DXMESH_MANAGED, VERTEX::FVF, pd3dDevice, &pMesh );
			}
			else
			{
				D3DXCreateMeshFVF( dwVertexNum/3, dwVertexNum, D3DXMESH_MANAGED, VERTEX::FVF, pd3dDevice, &pMesh );
			}

			VERTEX* pVertices(NULL);
			WORD* pIndices(NULL);
			pMesh->LockVertexBuffer( 0L, (VOID**)&pVertices );
			pMesh->LockIndexBuffer( 0L, (VOID**)&pIndices );

			// Note : Index Buffer
			for( DWORD j=0; j<dwVertexNum; ++j )
			{
				pIndices[j] = static_cast<WORD>(j);
			}

			// Note : Vertex Buffer
			for( DWORD j=0; j<dwVertexNum; ++j )
			{
				pVertices[j].vPos = m_vecVertices[j].vPos;
				pVertices[j].vNor = m_vecVertices[j].vNor;
				pVertices[j].vTex = m_vecVertices[j].vTex[0];
			}

			pMesh->UnlockVertexBuffer();
			pMesh->UnlockIndexBuffer();


			// Tangent 와 Binormal 을 얻어온다.
			LPD3DXMESH pNew(NULL);
			CreateTangentBinormalMesh( pd3dDevice, pMesh, &pNew );
			if ( pNew )
			{
				struct VERTEXNORTEXTANGENTBINORMAL_LOCAL
				{ 
					D3DXVECTOR3 vPos;
					D3DXVECTOR3 vNor;
					D3DXVECTOR2 vTex;
					D3DXVECTOR3 vTan;
					D3DXVECTOR3 vBin;
				};

				D3DXVECTOR4 vVector4;
				VERTEXNORTEXTANGENTBINORMAL_LOCAL* pVerticesTB(NULL);
				pNew->LockVertexBuffer( 0L, (VOID**)&pVerticesTB );

				for ( DWORD i=0; i<pNew->GetNumVertices(); ++i )
				{
					vecTangent.push_back ( pVerticesTB[i].vTan );
					vecBinormal.push_back ( pVerticesTB[i].vBin );
				}

				pNew->UnlockVertexBuffer();

				SAFE_RELEASE ( pNew );
			}

			SAFE_RELEASE ( pMesh );
		}

		// Tangent Base 로 Normal 값 계산.
		if ( vecTangent.size() == m_vecVertices.size() )
		{
			D3DXVECTOR3 vTempNor;
			for ( DWORD i=0; i<m_vecVertices.size(); ++i )
			{
				vecVerticesMatrix.push_back( m_vecVertices[i] );

				D3DXVec3TransformCoord( &vecVerticesMatrix[i].vPos, &vecVerticesMatrix[i].vPos, &matWorld );

				D3DXVec3TransformNormal( &vTempNor, &vecVerticesMatrix[i].vNor, &matWorld );
				D3DXVec3Normalize( &vTempNor, &vTempNor );

				D3DXVec3TransformNormal( &vecTangent[i], &vecTangent[i], &matWorld );
				D3DXVec3Normalize( &vecTangent[i], &vecTangent[i] );

				D3DXVec3TransformNormal( &vecBinormal[i], &vecBinormal[i], &matWorld );
				D3DXVec3Normalize( &vecBinormal[i], &vecBinormal[i] );

				vecVerticesMatrix[i].vNor = (vecTangent[i] * vBumpBasis.x) + (vecBinormal[i] * vBumpBasis.y) + (vTempNor * vBumpBasis.z);
				D3DXVec3Normalize( &vecVerticesMatrix[i].vNor, &vecVerticesMatrix[i].vNor );
			}
		}
		else
		{
			// 정점 갯수가 동일하지 않다. 문제가 있다.
			for ( DWORD i=0; i<m_vecVertices.size(); ++i )
			{
				vecVerticesMatrix.push_back( m_vecVertices[i] );

				D3DXVec3TransformCoord( &vecVerticesMatrix[i].vPos, &vecVerticesMatrix[i].vPos, &matWorld );
				D3DXVec3TransformNormal( &vecVerticesMatrix[i].vNor, &vecVerticesMatrix[i].vNor, &matWorld );
			}
		}
	}
}

// Radiosity 를 굽는다.
void LightMapObject::BakeRadiosity( LPDIRECT3DDEVICEQ pd3dDevice, 
								   DxLandMan* pLandMan, 
								   const D3DXMATRIX& matWorld, 
								   LPDIRECT3DTEXTUREQ pTextureSystem,
								   float fReflectance, 
								   float fFogBlend, 
								   DWORD dwFogLength,
								   DWORD dwRADNormalIndex,		// RAD Normal 생성시 사용. -1 일 경우 사용하지 않음.
								   int nSampling,
								   BOOL bPiece )
{
	BOOL bPieceInstancing = NSPieceQuickSort::g_bPieceInstancing;
	NSPieceQuickSort::g_bPieceInstancing = FALSE;

	WORDWORD wwBaseTexelXY(0,0);
	if ( !bPiece )
	{
		wwBaseTexelXY = m_wwBaseTexelXY;
	}
	WORDWORD wwLightMapUserUV_TEXEL = m_wwTextureSize;

	std::vector<VERTEX_WORLD> vecVerticesMatrix;  // Matrix 연산 된것
	SetVertexForRADNormal( pd3dDevice, vecVerticesMatrix, dwRADNormalIndex, matWorld );

	// 각 Texel 별로 Color 계산을 한다.
	{
		{
			DWORD dwTexelSize = static_cast<DWORD>(m_wwTexelSize.wA) * m_wwTexelSize.wB;

			SAFE_DELETE_ARRAY( m_pTexelCombinedRAD );
			m_pTexelCombinedRAD = new DWORD[ dwTexelSize ];
			ZeroMemory ( m_pTexelCombinedRAD, sizeof(DWORD)*dwTexelSize );
		}

		int nTexLineCount(0);
		int nTexLineCount_FULL(0);

		if ( m_listRadiosityRenderData.empty() )
		{
			sc::DefaultLock sDefaultLock;

#pragma omp parallel for
			for ( int y=0; y<static_cast<int>(m_wwTexelSize.wB); ++y )
			{
				for ( DWORD x=0; x<m_wwTexelSize.wA; ++x )
				{
					D3DXVECTOR2 vUV;

					// Note : 가장 가까운 삼각형을 구한다.
					float u, v;
					DWORD dwFaceNUM(0);
					DWORD dwIndex[3];
					VEC_DWORD vecFaceNUM;

					if ( bPiece )
					{
						vUV.x = static_cast<float>(0.5f + x) / (m_wwTexelSize.wA);
						vUV.y = static_cast<float>(0.5f + y) / (m_wwTexelSize.wB);

						for ( int i=0; i<m_pTexelTriangleID[y*m_wwTexelSize.wA+x].nFaceNUM; ++i )
						{
							vecFaceNUM.push_back( m_pTexelTriangleID[y*m_wwTexelSize.wA+x].pTriangleID[i] );
						}
					}
					else
					{
						vUV.x = static_cast<float>(0.5f + x + wwBaseTexelXY.wA) / (m_wwTextureSize.wA);
						vUV.y = static_cast<float>(0.5f + y + wwBaseTexelXY.wB) / (m_wwTextureSize.wB);

						for ( int i=0; i<m_pTexelTriangleID[(y+wwBaseTexelXY.wB)*m_wwTextureSize.wA+(x+wwBaseTexelXY.wA)].nFaceNUM; ++i )
						{
							vecFaceNUM.push_back( m_pTexelTriangleID[(y+wwBaseTexelXY.wB)*m_wwTextureSize.wA+(x+wwBaseTexelXY.wA)].pTriangleID[i] );
						}
					}

					DWORD dwMainIndex(0);
					BOOL bInTriangle = NSLightMapTools::FindNearTriangle( m_vecLightMapUV, vUV, vecFaceNUM, dwFaceNUM, u, v, dwMainIndex );
					if ( bInTriangle )


					{
						D3DXVECTOR3 avPos[3];
						D3DXVECTOR3 avNor[3];
						D3DXVECTOR2 avUV[3];
						D3DXVECTOR3 vPos(0.f,0.f,0.f);
						D3DXVECTOR3 vNor(0.f,0.f,0.f);

						// Note : 노멀을 구한다.
						switch( dwMainIndex )
						{
						case 1:
							{
								dwIndex[0] = dwFaceNUM*3 + 1;
								dwIndex[1] = dwFaceNUM*3 + 2;
								dwIndex[2] = dwFaceNUM*3 + 0;
							}
							break;

						case 2:
							{
								dwIndex[0] = dwFaceNUM*3 + 2;
								dwIndex[1] = dwFaceNUM*3 + 0;
								dwIndex[2] = dwFaceNUM*3 + 1;
							}
							break;

						case 0:
						default:
							{
								dwIndex[0] = dwFaceNUM*3 + 0;
								dwIndex[1] = dwFaceNUM*3 + 1;
								dwIndex[2] = dwFaceNUM*3 + 2;
							}
							break;
						}

						avPos[0] = vecVerticesMatrix[ dwIndex[0] ].vPos;    // Matrix 연산 된 것 사용
						avPos[1] = vecVerticesMatrix[ dwIndex[1] ].vPos;    // Matrix 연산 된 것 사용
						avPos[2] = vecVerticesMatrix[ dwIndex[2] ].vPos;    // Matrix 연산 된 것 사용

						avNor[0] = vecVerticesMatrix[ dwIndex[0] ].vNor;    // Matrix 연산 된 것 사용
						avNor[1] = vecVerticesMatrix[ dwIndex[1] ].vNor;    // Matrix 연산 된 것 사용
						avNor[2] = vecVerticesMatrix[ dwIndex[2] ].vNor;    // Matrix 연산 된 것 사용

						{
							D3DXVECTOR3 vPos_Dir[2];
							vPos_Dir[0] = avPos[1] - avPos[0];
							vPos_Dir[1] = avPos[2] - avPos[0];
							vPos = avPos[0] + vPos_Dir[0]*( u ) + vPos_Dir[1]*( v );

							D3DXVECTOR3 vNor_Dir[2];
							vNor_Dir[0] = avNor[1] - avNor[0];
							vNor_Dir[1] = avNor[2] - avNor[0];
							vNor = avNor[0] + vNor_Dir[0]*( u ) + vNor_Dir[1]*( v );

							D3DXVec3Normalize( &vNor, &vNor );
						}

						RadiosityRenderData sData;
						sData.x = x;
						sData.y = y;
						sData.vPos = vPos;
						sData.vNor = vNor;

						sDefaultLock.lockOn();
						m_listRadiosityRenderData.push_back( sData );
						sDefaultLock.lockOff();
					}
				}

				// Pixel 계산
				{
					++nTexLineCount;
					++nTexLineCount_FULL;

					if( nTexLineCount >= 20 )
					{
						nTexLineCount = 0;
						TSTRINGSTREAM strStringStream;
						strStringStream << "Radiosity - ";
						strStringStream << nTexLineCount_FULL/20;
						strStringStream << "/";
						strStringStream << m_wwTexelSize.wB/20;
						ExportProgress::SetStateString( strStringStream.str().c_str() );
					}
				}
			}
		}

		//// 순수 라이트도 RAD NormalMap 에 영향을 주도록 한다.
		//if ( (dwRADNormalIndex==0) || (dwRADNormalIndex==1) || (dwRADNormalIndex==2) )
		//{
		//	CreateTexelForRADNormal
		//	(
		//		pd3dDevice, 
		//		pLandMan->GetStaticMesh(),
		//		pLandMan->GetPieceManager(),
		//		vecVerticesMatrix,
		//		FALSE, 
		//		NULL, 
		//		nSampling,
		//		wwBaseTexelXY,
		//		m_wwTexelSize 
		//	);
		//}

		nTexLineCount = 0;
		nTexLineCount_FULL = 0;

		pd3dDevice->BeginScene();
		{
			int nCount(0);
			CLIPVOLUME sCV;
			D3DXMATRIX matView;
			D3DXMATRIX matProj;
			D3DXVECTOR3 vLookatPtOUT;
			std::list<RadiosityRenderData>::iterator iter = m_listRadiosityRenderData.begin();
			for ( ; iter!=m_listRadiosityRenderData.end(); ++iter, ++nCount )
			{
				pLandMan->GetClipVolumeRAD( sCV, matView, matProj, vLookatPtOUT, (*iter).vPos, (*iter).vNor );

				NSLandThread::FrameMoveRAD( pLandMan->GetArrayEntityRenderListSwap(), sCV, (*iter).vPos, vLookatPtOUT );

				m_pTexelIndirect[((*iter).y*m_wwTexelSize.wA)+(*iter).x] = RadiosityRender( pd3dDevice, pLandMan, (*iter).vPos, (*iter).vNor, pTextureSystem, fReflectance, fFogBlend, dwFogLength, TRUE );
				
				// 
				{
					++nTexLineCount;
					++nTexLineCount_FULL;

					if( nTexLineCount >= 100 )
					{
						nTexLineCount = 0;
						TSTRINGSTREAM strStringStream;
						strStringStream << "Radiosity Rendering - ";
						strStringStream << nCount+1;
						strStringStream << "/";
						strStringStream << m_listRadiosityRenderData.size();
						ExportProgress::SetStateString( strStringStream.str().c_str() );
					}
				}
			}
		}
		pd3dDevice->EndScene();

		// Edge Color 를 적용한다.
		EdgeColorApply( m_pTexelIndirect );
	}

	NSPieceQuickSort::g_bPieceInstancing = bPieceInstancing;
}

// Radiosity Render
DWORD LightMapObject::RadiosityRender( LPDIRECT3DDEVICEQ pd3dDevice, 
									  DxLandMan* pLandMan, 
									  const D3DXVECTOR3& vPos, 
									  const D3DXVECTOR3& vNor, 
									  LPDIRECT3DTEXTUREQ pTextureSystem,
									  float fReflectance, 
									  float fFogBlend,
									  DWORD dwFogLength,
									  BOOL bDay )
{
	return pLandMan->Render_Radiosity( pd3dDevice, vPos, vNor, pTextureSystem, fReflectance, fFogBlend, dwFogLength, bDay );
}

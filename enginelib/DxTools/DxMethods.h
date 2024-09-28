#if !defined(_DXMATRIXDECOMP_H__INCLUDED_)
#define _DXMATRIXDECOMP_H__INCLUDED_

#include "../../SigmaCore/gassert.h"
#include <string>
#include "../../InternalCommonLib/dxstdafx.h"

// Format of RGBA colors is
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |    alpha      |      red      |     green     |     blue      |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define RGBA_GETALPHA(rgb)      ((rgb) >> 24)
#define RGBA_GETRED(rgb)        (((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb)      (((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)       ((rgb) & 0xff)
#define RGBA_MAKE(r, g, b, a)   ((D3DCOLOR) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))


void D3DXVec3Combine ( D3DXVECTOR3* pCOMBINE,
					 const D3DXVECTOR3* V1, const float fScale1,
					 const D3DXVECTOR3* v2, const float fScale2 );

float D3DXMatrixDeterminantX ( const D3DXMATRIX* pM );

enum ENUMDECOMP
{
	DECOMP_SCALEX = 0,
	DECOMP_SCALEY = 1,
	DECOMP_SCALEZ = 2,

	DECOMP_SHEARXY = 3,
	DECOMP_SHEARXZ = 4,
	DECOMP_SHEARYZ = 5,

	DECOMP_ROTATEX = 6,
	DECOMP_ROTATEY = 7,
	DECOMP_ROTATEZ = 8,

	DECOMP_TRANSX = 9,
	DECOMP_TRANSY = 10,
	DECOMP_TRANSZ = 11,

	DECOMP_PERSPX = 12,
	DECOMP_PERSPY = 13,
	DECOMP_PERSPZ = 14,
	DECOMP_PERSPW = 15
};

struct DXDECOMP
{
    union
	{
		struct
		{
			float fScaleX, fScaleY, fScaleZ;
			float fShearXY, fShearXZ, fShearYZ;
			float fRotateX, fRotateY, fRotateZ;
			float fTransX, fTransY, fTransZ;
			float fPerspX, fPerspY, fPerspZ, fPerspW;
		};

        struct
		{
            float buffer[16];
        };
    };

	float& operator[] ( const int nIndex )
	{
		return buffer[nIndex];
	}

	DXDECOMP () :
		fScaleX(1.0f), fScaleY(1.0f), fScaleZ(1.0f),
		fShearXY(0.0f), fShearXZ(0.0f), fShearYZ(0.0f),
		fRotateX(0.0f), fRotateY(0.0f), fRotateZ(0.0f),
		fTransX(0.0f), fTransY(0.0f), fTransZ(0.0f),
		fPerspX(0.0f), fPerspY(0.0f), fPerspZ(0.0f), fPerspW(0.0f)
	{
	}
};

BOOL D3DXMatrixDecomp ( const D3DMATRIX &Mat, DXDECOMP &Decomp );
BOOL D3DXMatrixDecompX ( const D3DMATRIX &Mat, D3DXVECTOR3 &vTrans, D3DXVECTOR3 &vRotate, D3DXVECTOR3 &vScale );
BOOL D3DXMatrixCompX ( D3DMATRIX &Mat, const D3DXVECTOR3 &vTrans, const D3DXVECTOR3 &vRotate, const D3DXVECTOR3 &vScale );

struct DXAFFINEPARTS
{
	D3DXVECTOR3 vTrans;
	D3DXVECTOR3 vRotate;
	D3DXVECTOR3 vScale;

	DXAFFINEPARTS () :
		vTrans(D3DXVECTOR3(0,0,0)),
		vRotate(D3DXVECTOR3(0,0,0)),
		vScale(D3DXVECTOR3(1,1,1))
	{
	}
};

typedef DXAFFINEPARTS* LPDXAFFINEPARTS;

inline BOOL D3DXMatrixCompX ( D3DMATRIX &Mat, const DXAFFINEPARTS &AffineParts )
{
	return D3DXMatrixCompX ( Mat, AffineParts.vTrans, AffineParts.vRotate, AffineParts.vScale );
}

class DXAFFINEMATRIX
{
public:
	LPD3DXMATRIX	m_pmatLocal;
	LPDXAFFINEPARTS	m_pAffineParts;

public:
	void UseAffineMatrix ()
	{
		SAFE_DELETE(m_pmatLocal);
		SAFE_DELETE(m_pAffineParts);

		m_pmatLocal = new D3DXMATRIX;
		m_pAffineParts = new DXAFFINEPARTS;

		D3DXMatrixIdentity ( m_pmatLocal );
	}

	BOOL IsUseAffineMatrix ()	{	return m_pmatLocal!=NULL && m_pAffineParts!=NULL; }

	void SetAffineValue ( const LPDXAFFINEPARTS pAffineParts=NULL )
	{
		GASSERT(m_pmatLocal);
		GASSERT(m_pAffineParts);

		if ( pAffineParts )
		{
			*m_pAffineParts = *pAffineParts;
		}

		D3DXMatrixCompX ( *m_pmatLocal, *pAffineParts );

		SerialData ();
	}

	virtual void SerialData ()	{}

public:
	DXAFFINEMATRIX() :
		m_pmatLocal(NULL),
		m_pAffineParts(NULL)
	{
	}

	virtual ~DXAFFINEMATRIX()
	{
		SAFE_DELETE(m_pmatLocal);
		SAFE_DELETE(m_pAffineParts);
	}
};


//	Note : 백터의 각도를 계산하는 식이 (1,0,0) 이므로 이 방향으로 Direction 을
//		회전한 뒤에 각도를  계산하여 사용함. ( 특정 방향 원점 방향 vDirOrig )
//		( -PI ~ PI ) 영역의 각도임.
//
extern const D3DXVECTOR3 vDIRORIG;
inline float DXGetThetaYFromDirection(const D3DXVECTOR3& vDir, const D3DXVECTOR3& vDirOrig)
{
	D3DXMATRIX matYRot;
	float fLengthXZ, fThetaY;

	fLengthXZ = (float) sqrt ( vDirOrig.x*vDirOrig.x + vDirOrig.z*vDirOrig.z );
	if ( fLengthXZ == 0 )	fLengthXZ = 0.001f;
	
	fThetaY = (float) acos ( vDirOrig.x / fLengthXZ );
	if ( vDirOrig.z>0 )	fThetaY = -fThetaY;

	D3DXMatrixRotationY ( &matYRot, -fThetaY );

	D3DXVECTOR3 vRotDir;
	D3DXVec3TransformCoord ( &vRotDir, &vDir, &matYRot );

	fLengthXZ = (float) sqrt ( vRotDir.x*vRotDir.x + vRotDir.z*vRotDir.z );
	if ( fLengthXZ == 0 )	fLengthXZ = 0.001f;

	fThetaY = (float) acos ( vRotDir.x / fLengthXZ );
	if ( vRotDir.z>0 )	fThetaY = -fThetaY;

	return fThetaY;
}

inline BOOL DxIsZeroVector ( D3DXVECTOR3 &vDir )
{
	return vDir.x == 0.0f && vDir.y == 0.0f && vDir.z == 0.0f;
}

inline BOOL DxIsMinVector ( D3DXVECTOR3 &vDir, float fError )
{
	return ( fabs(vDir.x) < fError && fabs(vDir.y) < fError && fabs(vDir.z) < fError );
}

void		DxBillboardLookAt ( D3DXMATRIX &matOut, const D3DXVECTOR3 *BillboardPos, const D3DXMATRIX &matView );
void		DxBillboardLookAt ( D3DXMATRIX &matOut, const D3DXVECTOR3 *BillboardPos, D3DXVECTOR3 *CarmeraPos, D3DXVECTOR3 *UpPos );
void		DxBillboardLookAtHeight ( D3DXMATRIX &matOut, const D3DXVECTOR3 *BillboardPos, const D3DXMATRIX &matView );
void		DxBillboardLookAtGround ( D3DXMATRIX &matOut, const D3DXVECTOR3 *BillboardPos, D3DXVECTOR3 *CarmeraPos );
void		DxBillboardLookAtDir ( D3DXMATRIX &matOut, const D3DXVECTOR3 *BillboardPos, const D3DXVECTOR3 *CarmeraPos );
void		DxBillboardLookAtDir ( D3DXMATRIX &matOut, const D3DXVECTOR3 *Dir );
void		DxBillboardLookAtDirXZ (D3DXMATRIX &matOut,  D3DXVECTOR3 *Dir );
D3DXVECTOR3 DxSplit ( D3DXPLANE *vPlane, D3DXVECTOR3 *vPos1, D3DXVECTOR3 *vPos2 );
D3DXVECTOR3 DxBezier ( D3DXVECTOR3 vParent, D3DXVECTOR3 vPrevPos, D3DXVECTOR3 vPrevPos2 );
DWORD		VectortoRGBA( D3DXVECTOR3* v, FLOAT fHeight );

void		DxImageMap ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pSrcTexture, LPDIRECT3DSURFACEQ pDestSurface, 
							const DWORD dwTexSize );
void		DxImageMap( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pSrcTex0, LPDIRECT3DTEXTUREQ pSrcTex1, 
						LPDIRECT3DSURFACEQ pDestSurf, const DWORD dwTexSize );
void D3DXImageBlur( const LPDIRECT3DDEVICEQ pd3dDevice, const LPDIRECT3DTEXTUREQ pSrcTex, const float fTexWidth, const float fTexHeight );
void D3DXImageCopy( const LPDIRECT3DDEVICEQ pd3dDevice, const LPDIRECT3DTEXTUREQ pSrcTex,	const LPDIRECT3DSURFACEQ pDestSuf,
					const UINT nPoint, const UINT nFull, const UINT nTexSize );
void		DxImageMap ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pSrcTexture, LPDIRECT3DSURFACEQ pDestSurface, 
							D3DCOLOR cColor, const DWORD dwTexSize );
void		DxImageMapRHW ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pSrcTexture, LPDIRECT3DSURFACEQ pDestSurface, 
							const BOOL bNew, const DWORD dwTexSize );
void		DxImageMapRHW ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pSrcTexture, LPDIRECT3DSURFACEQ pDestSurface, 
							LPDIRECT3DSURFACEQ pDestZBuffer, const BOOL bNew, const DWORD dwWidth, const DWORD dwHeight );

D3DXVECTOR2	DxPosToTexUV( D3DXVECTOR3 vPos, const D3DXMATRIX& matVP, const DWORD dwWidth, const DWORD dwheight );
void		DxSetTextureMatrix( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwStage, const D3DXMATRIX& matVP );
void		DxSetTextureMatrix( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwStage, const D3DXVECTOR3& vDir );	// 방향으로 내적을 구해 값을 0~1 사이의 값을 얻는다.
void		DxResetTextureMatrix( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwStage );

//	Note : vMoveUV	- 텍스쳐 이동
//			bNew	- 처음 사용 한 것, 또는 추가적으로 사용 한 것
//
//	Warning : 이 함수를 부르기 전의 pd3dDevice->SetRenderTarget (,) 을 꼭 돌려 놔야 한다.
//				안할 경우 이상한 일이 벌어 질 수 있음
//
void DxImageMove(
    LPDIRECT3DDEVICEQ pd3dDevice,
    LPDIRECT3DTEXTUREQ pSrcTexture,
    LPDIRECT3DSURFACEQ pDestSurface, 
    const D3DXVECTOR2& vMoveUV,
    const BOOL bNew,
    const DWORD dwFVF,
    const D3DCOLOR cColor,
    const D3DXVECTOR2& vTexUV,
    const DWORD dwTexSize=128);

void DxImageMoveDiffuse(
    LPDIRECT3DDEVICEQ pd3dDevice,
    const D3DXVECTOR2& vMoveUV,
    const D3DCOLOR cColor,
    const DWORD dwTexSize,
    const D3DXVECTOR2& vTexUV);

void DxImageMoveNoDiffuse(
    LPDIRECT3DDEVICEQ pd3dDevice,
    const D3DXVECTOR2& vMoveUV,
    const DWORD dwTexSize,
    const D3DXVECTOR2& vTexUV);

//	Matrix 관련 Method
void D3DXMatrixMultiply_MM ( D3DXMATRIX& sOut, const D3DXMATRIX& sM1, const D3DXMATRIX& sM2 );		// Out = M * M
void D3DXMatrixMultiply_RM1 ( D3DXMATRIX& sOut, const D3DXMATRIX& sRotate, const D3DXMATRIX& sM );	// Out = R * M
void D3DXMatrixMultiply_RM2 ( const D3DXMATRIX& sRotate, D3DXMATRIX& sOut );						// Out = R * Out
void D3DXMatrixMultiply_RM3 ( D3DXMATRIX& sOut, const D3DXMATRIX& sRotate );						// Out = Out * R
void D3DXMatrixMultiply_SM_fUp ( const float fScale, D3DXMATRIX& sOut );							// Out = fS * Out
void D3DXMatrixMultiply_SM_fDown ( const float fScale, D3DXMATRIX& sOut );							// Out = fS * Out
void D3DXMatrixMultiply_SM ( D3DXMATRIX& sOut, const D3DXVECTOR3& vScale );							// Out = Out * vS
void D3DXMatrixMultiply_MIRROR ( D3DXMATRIX& sOut );												// 거울행렬

void D3DXMatrixBillboard ( D3DXMATRIX& sOut, const D3DXVECTOR3 *Dir );
void D3DXMatrixBillboardGround ( D3DXMATRIX& sOut );

void D3DXMatrixAABBTransform( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin, const D3DXMATRIX& sM );
BOOL D3DXMatrixToSQT( D3DXVECTOR3& vScale, D3DXQUATERNION& sQuat, D3DXVECTOR3 &Trans, const D3DXMATRIX& matWorldIN );

HRESULT CreateTangentBinormalMesh( IDirect3DDevice9* pd3dDevice, ID3DXMesh* pMeshIn, ID3DXMesh** ppMeshOut );

// Note : LPD3DXMESH
namespace sc {
    class SerialFile;
}

void LPD3DXMESH_Save( sc::SerialFile& SFile, LPD3DXMESH pMesh );
void LPD3DXMESH_Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::SerialFile& SFile, LPD3DXMESH& pMesh );

// 폴더 생성하게 하는 것.
void MakeFolder(const char *dir);


// ClipVolume 관련 Method
#include "DxCustomTypes.h"
void D3DXClipVolumeTransform ( CLIPVOLUME &pOut, const CLIPVOLUME &pCV, const D3DXMATRIX& matWorld );

// Math 관련 Method
#define FP_BITS(fp) (*(DWORD *)&(fp))

static unsigned int fast_sqrt_table[0x10000];  // declare table of square roots 

void build_sqrt_table();

inline float fastsqrt(float n)
{
  if (FP_BITS(n) == 0)
    return 0.0;                 // check for square root of 0
  
  FP_BITS(n) = fast_sqrt_table[(FP_BITS(n) >> 8) & 0xFFFF] | ((((FP_BITS(n) - 0x3F800000) >> 1) + 0x3F800000) & 0x7F800000);
  
  return n;
}

namespace NSMATH
{
	BOOL IsOffsetSamePos2( const D3DXVECTOR2& lhs, const D3DXVECTOR2& rhs, float fOffset );
	BOOL IsOffsetSamePos3( const D3DXVECTOR3& lhs, const D3DXVECTOR3& rhs, float fOffset );

	// 2개의 값을 1개로 만든다.
	float ExportVector2ToFloat( float fX, float fY );
};

namespace MIN_BOOST
{
	// boost::polymorphic_downcast 꺼 가져옴.
	//
	// dynamic_cast 는 성능 때문에 쓰기가 꺼려진다면 이 함수를 써 볼만하다.
	// ReleaseD 때는 GASSERT 가 동작하므로 error 체크를 하면 되고,
	// Release 때는 검토 된 사항으로 간주하고 static_cast 한다.
	//
	template <class Target, class Source>
	inline Target polymorphic_downcast(Source* x)
	{
		GASSERT( dynamic_cast<Target>(x) == x );  // detect logic error
		return static_cast<Target>(x);
	}
};

//HRESULT CreateTagentMesh( IDirect3DDevice9* pd3dDevice, ID3DXMesh* pMeshIn, ID3DXMesh** ppMeshOut );
//HRESULT CreateTagentSkinMeshVS( IDirect3DDevice9* pd3dDevice, ID3DXMesh* pMeshIn, ID3DXMesh** ppMeshOut );

#endif // !defined(_DXMATRIXDECOMP_H__INCLUDED_)
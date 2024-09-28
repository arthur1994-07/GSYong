// ReplaceContainer.h: interface for the CReplaceContainer class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <map>
#include <string>
#include <hash_map>
#include <boost/scoped_ptr.hpp>

#include "../../Common/StlFunctions.h"
#include "../../DxTools/TextureManager.h"
#include "./_new/DxSkinMeshContainer9.h"
#include "./_new/DxSkinMesh9.h"
#include "DxPieceDefine.h"
#include "DxSkinObject.h"
#include "DxSkinAnimationManager.h"
#include "DxSkinPieceBase.h"
#include "SSkinCharLODInfo.h"

#define STRACE_NSIZE	(32)
#define STRACE_IMAGE01 ("IMAGE01")		// 0
#define STRACE_IMAGE02 ("IMAGE02")
#define STRACE_IMAGE11 ("IMAGE11")
#define STRACE_IMAGE12 ("IMAGE12")
#define STRACE_IMAGE21 ("IMAGE21")
#define STRACE_IMAGE22 ("IMAGE22")		//	5

#define STRACE_STRIKE01 ("STRIKE01")	//	6
#define STRACE_STRIKE02 ("STRIKE02")
#define STRACE_STRIKE11 ("STRIKE11")
#define STRACE_STRIKE12 ("STRIKE12")	//	9

#define STRACE_ARROW01 ("ARROW01")		//	10

#define STRACE_RAIN01 ("RAIN01")		//	11
#define STRACE_RAIN02 ("RAIN02")
#define STRACE_RAIN03 ("RAIN03")		//	13

#define STRACE_BLOOD01 ("BLOOD01")		//	14

#define STRACE_ENERGY01 ("ENERGY01")	//	15
#define STRACE_ENERGY02 ("ENERGY02")
#define STRACE_ENERGY03 ("ENERGY03")	//	17

#define MAX_TRACE	(18)
extern char szTRACE_NAME[MAX_TRACE][STRACE_NSIZE];

#ifndef DxEffChar
	class DxEffChar;
#endif

struct EffectMaterialData;
class DxSkeletonMaintain;
class DxPhysXCloth;


class sc::SerialFile;

typedef std::vector<DxEffChar*>						GLEFFCHAR_VEC;
typedef GLEFFCHAR_VEC::iterator						GLEFFCHAR_VEC_ITER;
typedef GLEFFCHAR_VEC::reverse_iterator				GLEFFCHAR_VEC_RITER;

struct STRACOR
{
	char			m_szName[STRACE_NSIZE];
	SVERTEXINFLU	m_sVertexInflu;
	D3DXVECTOR3		m_vPos;
	D3DXVECTOR3		m_vNormal;
	D3DXMATRIX		m_BoneMatrice;

	STRACOR ();
	STRACOR ( const char* _szName, const SVERTEXINFLU &_sVertexInflu );
	STRACOR ( const STRACOR& sTracor );

	bool operator== ( const STRACOR& lvalue );
};
typedef std::vector<STRACOR>	VECTRACE;
typedef VECTRACE::iterator		VECTRACE_ITER;

struct STRACOR_OPER
{
	bool operator() ( const STRACOR& lvalue, const STRACOR& rvalue )
	{
		return strcmp(lvalue.m_szName, rvalue.m_szName) < 0;
	}

	bool operator() ( const STRACOR& lvalue, const char* rvalue_szName )
	{	
		return strcmp(lvalue.m_szName, rvalue_szName) < 0;
	}

	//
	//mjeon.migration
	//
	// Debug_Iterator checks whether Predicate is commutative.	-> If A is TRUE, B MUST be FALSE;
	//

	bool operator() ( const char* lvalue_szName, const STRACOR& rvalue )
	{	
		return strcmp(lvalue_szName, rvalue.m_szName ) < 0;
	}
};

struct SMATERIAL_PIECE
{
	enum
	{
		VERSION				= 0x0104,
		FLAG_ALPHATEXTURE	= 0x0001,
	};

	enum SKIN_MS_TYPE
	{
		SKIN_MS_TYPE_CUBEMAP		= 0x01,
		SKIN_MS_TYPE_NORMALMAP		= 0x02,
		SKIN_MS_TYPE_HAIR			= 0x04,
		SKIN_MS_TYPE_CLOTH			= 0x08,
	};

	std::string			m_strTexture;
	TextureResource		m_textureRes;
	TextureResource		m_textureRes_mask;			// R-Fake_SSS,				G-None,					B-CubeBlend,		A-NM_R
	TextureResource		m_textureRes_mask2;			// R-Spec_Intensity,		G-Spec_Power,			B-Level_Mask		A-NM_G
	TextureResource		m_textureRes_HairMask;		// R-Hair_Shift_Tangent,	G-Hair_Specular_Mask

	LPDIRECT3DTEXTUREQ	m_pBakeMaskTex;		// 데이터가 없어서 프로그램상에서 만든 텍스쳐 ( For - Fake_SSS )
	LPDIRECT3DTEXTUREQ	m_pBakeMask2Tex;	// 데이터가 없어서 프로그램상에서 만든 텍스쳐 ( For - Spec_Color, Spec_Power )

	DWORD				m_dwFlags;
	DWORD				m_dwZBias;
	DWORD				m_dwBlendFlag;		// 0 - Normal, 1 - SRC+DEST(ALPHA)
	float				m_fZBias;			// Optimize - m_dwZBias -	Don`t Save Load
	DWORD				m_dwDyVertStart;	// 동적버텍스 렌더시작점-	Don`t Save Load
	BOOL				m_bBaseRenderOff;	// 기본Texture 를 렌더링 하지 않는다.

	//////////////////////////////////////////////////////////////////////////
	//				Material System 으로 새로 사용되는 부분.
	DWORD			m_dwMaterialSystemFlags;				// SKIN_MS_TYPE
	D3DXVECTOR4		m_vHairSpecColor_DirectionIntensity;	// xyz - SpecColor,		w - DirectionBright
	D3DXVECTOR4		m_vHairShiftUV_SpecExp_DL_CL;			// xy - ShiftTangentUV, z - SpecExp_DiretionLight,			w - SpecExp_Camera
	float			m_fClothDampingCoefficient;
	int				m_nClothSolverFrequency;
	D3DXVECTOR4		m_vClothConfiguration;					// x - eSTRETCHING,	y -	eSTRETCHING_HORIZONTAL,				z - eSHEARING,			w - eBENDING_ANGLE
	//D3DXVECTOR3			m_vHairSpecColor;
	//D3DXVECTOR2			m_vHairShiftUV;
	//float				m_vSpecExpR;
	//float				m_vSpecExpTRT;
	//float				m_fTangentOffsetTRT;

	SMATERIAL_PIECE ()
		: m_pBakeMaskTex(NULL)
		, m_pBakeMask2Tex(NULL)
		, m_dwFlags(NULL)
		, m_dwZBias(0)
		, m_dwBlendFlag(0)
		, m_fZBias(0.f)
		, m_dwDyVertStart(0)
		, m_bBaseRenderOff(FALSE)
		, m_dwMaterialSystemFlags(0L)
		, m_vHairSpecColor_DirectionIntensity(1.f,1.f,1.f,1.f)
		, m_vHairShiftUV_SpecExp_DL_CL(1.f,1.f,512.f,32.f)
		, m_fClothDampingCoefficient(0.5f)
		, m_nClothSolverFrequency(120)
		, m_vClothConfiguration( 1.f, 1.f, 0.75f, 0.5f )
	{
	}

	~SMATERIAL_PIECE ()
	{
		SAFE_RELEASE( m_pBakeMaskTex );
		SAFE_RELEASE( m_pBakeMask2Tex );
	}

	/*
	afx_msg void OnBnClickedColorUpDown();
	afx_msg void OnEnChangeColorUpDownSpeed();
	afx_msg void OnBnClickedLevel();
	afx_msg void OnBnClickedCubeMap();
	afx_msg void OnEnChangeCubeMapValue();
	afx_msg void OnBnClickedNormalMap();
	afx_msg void OnEnChangeSkinSpecPower();
	afx_msg void OnEnChangeSkinSpecIntensity();
	*/

	void CreateBakeMask( LPDIRECT3DDEVICEQ pd3dDevice,
							LPDIRECT3DTEXTUREQ pLevelTex,
							LPDIRECT3DTEXTUREQ pCubeTex );

	void CreateBakeMask2( LPDIRECT3DDEVICEQ pd3dDevice,
							LPDIRECT3DTEXTUREQ pLevelTex,
							LPDIRECT3DTEXTUREQ pCubeTex );

	void CreateBakeMaskALL ( LPDIRECT3DDEVICEQ pd3dDevice,
							LPDIRECT3DTEXTUREQ pLevelTex,
							LPDIRECT3DTEXTUREQ pCubeTex );

	void CreateBakeMaskDetail( LPDIRECT3DDEVICEQ pd3dDevice, 
								LPD3DXEFFECT pFX, 
								LPDIRECT3DTEXTUREQ& pBakeMaskTex, 
								int nDivision,
								LPDIRECT3DTEXTUREQ pLevelTex,
								LPDIRECT3DTEXTUREQ pCubeTex );

	void PrecalculateForMaterialSystem( EffectMaterialData* pEffMaterialData, DWORD dwMaterialID ) const;

	void LoadMaskTex ( LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL IsLoadingOK_MainTexLoad();

	void SaveFile ( sc::SerialFile &SFile );
	void LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

	void LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	void LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	// 데이터 포맷 변경 진행
	void LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	// SRC+DEST(ALPHA) 기능 추가함.
	void LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	// Material System 에서 쓰이는 것 추가됨.
	void LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	// Cloth 시뮬레이션 정보 추가됨.

	HRESULT LoadTexture();

	void InvalidateDeviceObjects ();

	static LPD3DXEFFECT SetFX( LPDIRECT3DDEVICEQ pd3dDevice );
	static LPD3DXEFFECT SetFX_mask2( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnResetDevice_STATIC ( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnLostDevice_STATIC ();
};

// 모자와 헤어와의 관계. 헤어를 어떻게 할 것인가~?
enum HAT_HAIR_OPTION
{
	HAT_HAIR_NONE	= 0,	// 헤어를 렌더링 안한다.
	HAT_HAIR_BASE	= 1,	// 기본 헤어를 렌더링한다.
	HAT_HAIR_NEW	= 2,	// HAT 셋팅시 설정된 새로운 헤어를 렌더링 한다.
};

struct DxSkinPiece : public DxSkinPieceBase
{
public:
	const static DWORD	VERSION;
	const static char	m_szPIECETYPE[PIECE_SIZE][64];
	const static char	m_szATTBONETYPE[ATTBONE_SIZE][64];
	const static char	m_szV_PARTSTYPE[PART_V_SIZE][64];

public:

	EMPICETYPE			m_emPieceType;

	char*				m_szFileName;

	DWORD				m_dwRef;
	
	char*				m_szXFileName;
	DxSkinMeshResource  m_skinMeshRes;

	char*				m_szSkeleton;
	DxSkeletonResource	m_skeletonRes;

	EMPIECECHAR			m_emType;
	EMPIECEATTBONE		m_emAttBoneType;
	EMPIECEVEHICLE		m_emV_PartType;

	EMPEACEZONEWEAPON	m_emWeaponWhereBack;
	char*				m_szMeshName;
	PSMESHCONTAINER		m_pmcMesh;				// Fixed <-> MaterialSystem 으로 전환될 때 마다 재로딩이 필요하다.
	DWORD				m_dwVertexNUM;			// 정점 갯수를 미리.. 넣어 놓는다.

	DWORD				m_dwMaterialNum;
	SMATERIAL_PIECE*	m_arrayMaterialPiece;

	// Note : 무슨 효과가 들어있는지를 알아내는 Flag 실시간 변함
	//
	DWORD				m_dwFlags;

	// Note : 효과 셋팅, Save, Load
	DWORD				m_dwFlag;

	//	Note : 추적 위치값 들.
	//
	VECTRACE			m_vecTrace;

	//	Note : 스킨 조각에 부착된 효과들.
	//
	GLEFFCHAR_VEC		m_vecEFFECT;
	//GLEFFCHAR_VEC		m_vecEFFECT_CharPart;		// DxCharPart에서 쓰이는 것

	// Note : 유저 Back Slot
	TSTRING				m_strUserBackSlot;
	D3DXMATRIX			m_matUserBackSlot;
	BOOL				m_bKeepUserBackSlot;

	BOOL				m_bCompletedLOAD;
	BOOL				m_bActiveErrorLog;

    // LOD 정보를 가진다.
    SSkinCharLODInfo    m_kLODInfo;

	// 0x0113 이전버젼은 mask 맵을 만들도록 한다.
	BOOL				m_bMakeMaskMap;

private:
	BOOL										m_bMaterialSystem;		// Material System 으로 동작되는지 확인하는 코드.
	boost::scoped_ptr<EffectMaterialData>	m_scpEffectMaterialData;

	boost::scoped_ptr<DxSkeletonMaintain>	m_scpSkeleton;

	boost::scoped_ptr<DxPhysXCloth>			m_scpPhysXCloth;

	// Overlay or UserColor 가 쓰인다면 원본 텍스쳐 대신 이 텍스쳐를 쓰도록 하자.
	LPDIRECT3DTEXTUREQ*	m_apOverlayTex;		// OverlayTexture

	// 모자와 헤어와의 관계. 헤어를 어떻게 할 것인가~?
	HAT_HAIR_OPTION m_emHairHairOption;

	// 모자 사용시 추가로 cps 를 렌더링 할 수 있도록 한다.
	// 모자의 일부머리카락을 표현하기 위한 cps 로 쓰인다.
	// m_bHairRenderOff 가 TRUE 일 경우만 m_strAddHairCps 이 동작되도록 한다.
	TSTRING	m_strAddHairCps;

protected:
	void SumEffFlag ();

public:
	BOOL SelectMesh ( LPDIRECT3DDEVICEQ pd3dDevice, PSMESHCONTAINER pmcMesh );

public:
	void AddEffList ( DxEffChar* pEffChar );
	void DelEffList ( LPDIRECT3DDEVICEQ pd3dDevice, DxEffChar* pEffChar );
	void ClearEffList ();

	// Bone Matrix를 돌려준다.
	D3DXMATRIX GetInfluenceMatrix();

	// Set Bone Matrix
	void SetChangeMatrix( const D3DXMATRIX& matWorld );

	// 모자와 헤어와의 관계. 헤어를 어떻게 할 것인가~?
	HAT_HAIR_OPTION GetHatHairOption() const							{ return m_emHairHairOption; }
	void			SetHatHairOption( HAT_HAIR_OPTION emType )			{ m_emHairHairOption=emType; }
	BOOL			IsHatHairOption( HAT_HAIR_OPTION emType ) const		{ return m_emHairHairOption==emType ? TRUE : FALSE; }

	// 모자 사용시 추가로 cps 를 렌더링 할 수 있도록 한다.
	// 모자의 일부머리카락을 표현하기 위한 cps 로 쓰인다.
	// m_bHairRenderOff 가 TRUE 일 경우만 m_strAddHairCps 이 동작되도록 한다.
	const TSTRING& GetAddHairCps() const	{ return m_strAddHairCps; }

public:
	HRESULT SetSkinMesh ( const char* szXFileName, const char* szSkeleton, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );
	HRESULT ReSetSkinMesh ( LPDIRECT3DDEVICEQ pd3dDevice );
	bool	IsValidSkinMesh();
	void	ReAttachSkinMesh( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	const TCHAR* GetFileName() const	override	{ return m_szFileName; }
	const TCHAR* GetBoneName() const	override	{ return m_szSkeleton; }
	DWORD GetSlot() const				override	{ return m_emType; }
	DWORD GetVehicleSlot() const		override	{ return m_emV_PartType; }

	const TCHAR* GetLinkBoneName() const;

public:
    HRESULT LoadPiece( const std::string& strFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread, boost::function<void (const TCHAR*)> pArchive = 0 );
	HRESULT SavePiece ( const char* szFile, BOOL bCharTool = FALSE, boost::function<void (const TCHAR*)> pArchive = 0 );

	HRESULT LoadPiece_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );
	HRESULT LoadPiece_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );
	HRESULT LoadPiece_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );
	HRESULT LoadPiece_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );
	HRESULT LoadPiece_0105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );
	HRESULT LoadPiece_0106 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );
	HRESULT LoadPiece_0107 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );
	HRESULT LoadPiece_0108 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );	//	데이터 포맷 변경 진행
	HRESULT LoadPiece_0109 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );	//	데이터 포맷 변경 진행 버그 수정
	HRESULT LoadPiece_0110 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );	//	APS 추가
    HRESULT LoadPiece_0111 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );	//	LOD 추가
	HRESULT LoadPiece_0112_0113 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );	//	UserSlot 추가
	HRESULT LoadPiece_0114 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );	//	PIECE_CAPE 추가
	HRESULT LoadPiece_0115 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );	//	DxEffCharSingle 의 Matrix 계산 오류에 따른 수정.
	HRESULT LoadPiece_0116 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );	//	헤드기어가 헤어를 렌더링 On/Off 할 수 있도록 작업.
	//HRESULT LoadPiece_0113 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );	//	오른손2, 왼손2 추가;

	void LoadUBS( const TSTRING& strFile );
	void SaveUBS( const TCHAR* szFile );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects();
	void ClearAll ();

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIXA16 &matRot, BOOL bCHAR_EDIT_PIECE_RENDER_IDENTITY );
	HRESULT MeshEditRender ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matRot, DxSkeleton* pSkeleton );
	HRESULT RenderTEST( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matRot, DxSkeleton* pSkeleton );

private:
	HRESULT RenderFixed( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matRot, BOOL bCHAR_EDIT_PIECE_RENDER_IDENTITY );
	HRESULT RenderMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIXA16 &matRot, BOOL bCHAR_EDIT_PIECE_RENDER_IDENTITY );

private:
	// m_pmcMesh 값이 없을 경우 FALSE 가 나옴.
	// m_vecEFFECT 쪽의 텍스쳐들이 로딩이 안되었을 경우 FALSE 가 나옴.
	// 일반 텍스쳐는 로딩이 안되어도 TRUE 가 나옴.
	//
	// m_vecEFFECT 쪽 텍스쳐 로딩이 안되면 FALSE 를 주는 이유는 Mask 맵을 생성할 때 해놓고
	// 다 되었으면 Mask 맵을 또 생성해야 하는지 묻지 않지 위함이다.
	//
	// [2014.09.25][shhan] bSetBaseTexture 추가됨.
	//						cps 상에서 새로운 Mesh 를 생성할 때는 TRUE를 줘서 원본 텍스쳐를 사용하게 하고(CharEdit상),
	//						chf 상에서 cps 를 읽을 때, 다른 Texture 가 셋팅된 상태라면 FALSE 를 줘서 다른 Texture 를 읽도록 한다(CharEdit 및 Game).
	BOOL SetMaterialSetting( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bSetBaseTexture, BOOL bTexThread );

	void CaculateBoneMatrices( const SMeshContainer& sMeshContainer, 
								D3DXMATRIXA16* arrayBoneMatrices,
								const DxSkeletonMaintain& sSkeleton );

public:
	HRESULT DrawMeshContainer ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bOriginDraw=TRUE, BOOL bAlpha=FALSE, BOOL bShadow=FALSE, BOOL bAlphaTex=TRUE );
	HRESULT DrawTraceVert ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bIdentity = TRUE );
	HRESULT DrawSelTraceVert ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szSelVert, BOOL bIdentity = TRUE );

public:
	SVERTEXINFLU* GetTracePos ( std::string strTrace );
	void SetTracePos ( std::string strTrace, SVERTEXINFLU &sVertInflu );
	void DelTracePos ( std::string strTrace );

	//	pBoneMatrice -> (주의) 영향을 받는 본의 갯수가 여러개일 경우에 문제가 생김.
	HRESULT CalculateVertexInflu ( std::string strTrace, D3DXVECTOR3 &vVert, D3DXVECTOR3 &vNormal, LPD3DXMATRIX pBoneMatrice=NULL );

	HRESULT CalculateBoundingBox ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

    void ReloadVMFX( LPDIRECT3DDEVICEQ pd3dDevice );

    float GetDistanceFromCamera(const D3DXVECTOR3& vPos);
    HRESULT UpdateLOD(const D3DXVECTOR3& vPos, LPDIRECT3DDEVICEQ pd3dDevice);    
    int GetLODLevel(const D3DXVECTOR3& vPos);

public:
	DWORD GetVertexNUM()	{ return m_dwVertexNUM; }	// 정점 갯수.

	// NSThreadCORE 관련
public:
	BOOL NSThreadCORE_UpdateData( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bTexThread );
    BOOL UpdateLODData( int nLevel, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL ImmediatelyLoad( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void CreateCartoonMesh( LPDIRECT3DDEVICEQ pd3dDevice, float fThickness, float fSizeADD );
	void CreateNormalMapMESH( LPDIRECT3DDEVICEQ pd3dDevice );

	void SetBaseRenderOff();

public:
	friend class CSkinPiecePage;

public:
	DxSkinPiece ();
	~DxSkinPiece ();
};
typedef DxSkinPiece*	PDXSKINPIECE;

class DxSkinPieceContainer  
{
private:
    typedef std::tr1::unordered_map<std::string, DxSkinPiece*> PIECEMAP;
    typedef PIECEMAP::iterator                                 PIECEMAP_ITER;
    typedef PIECEMAP::value_type                               PIECEMAP_VALUE;

protected:
	char m_szPath[MAX_PATH];

public:
	void  SetPath(char* szPath);
	char* GetPath()				{ return m_szPath; }

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	PIECEMAP			m_mapLoading;
	PIECEMAP			m_mapSkinPiece;

	// m_vecEFFECT 의 ExportEffTexture 관련 텍스쳐가 존재하는지 확인 코드
	// 없다면 렌더링 자체가 안되서 보완작업용.
private:
	std::set<TSTRING>	m_setExportEffTexture_NoHave_TextureName;

public:
	void Insert_ExportEffTexture_NoHave_TextureName( const TSTRING& strName );
	void Dialog_ExportEffTexture_NoHave_TextureName();

public:
	DxSkinPiece* LoadPiece( const std::string& FileName, DWORD dwFlags );
	HRESULT ReleasePiece( const std::string& FileName );
	HRESULT DeletePiece ( const char* szFile );
    HRESULT DeletePiece ( DxSkinPiece* pDeletePiece );
    HRESULT DeletePiece( const TSTRING& FileName );
    DxSkinPiece* FindPiece( const TSTRING& FileName );

public:
	HRESULT CleanUp ();

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();

public:
    void Archive(boost::function<void (const TCHAR*)> fun);

public:
	// 없는 텍스쳐를 찾기위해 로딩을 전부 해보도록 한다.
	void OnFindDontHaveTexture();

	// NSThreadCORE 관련
public:
	void NSThreadCORE_UpdateData( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime );

protected:
	DxSkinPieceContainer();

public:
	~DxSkinPieceContainer();

public:
	static DxSkinPieceContainer& GetInstance();
};


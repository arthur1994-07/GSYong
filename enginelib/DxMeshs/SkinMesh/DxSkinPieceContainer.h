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

	LPDIRECT3DTEXTUREQ	m_pBakeMaskTex;		// �����Ͱ� ��� ���α׷��󿡼� ���� �ؽ��� ( For - Fake_SSS )
	LPDIRECT3DTEXTUREQ	m_pBakeMask2Tex;	// �����Ͱ� ��� ���α׷��󿡼� ���� �ؽ��� ( For - Spec_Color, Spec_Power )

	DWORD				m_dwFlags;
	DWORD				m_dwZBias;
	DWORD				m_dwBlendFlag;		// 0 - Normal, 1 - SRC+DEST(ALPHA)
	float				m_fZBias;			// Optimize - m_dwZBias -	Don`t Save Load
	DWORD				m_dwDyVertStart;	// �������ؽ� ����������-	Don`t Save Load
	BOOL				m_bBaseRenderOff;	// �⺻Texture �� ������ ���� �ʴ´�.

	//////////////////////////////////////////////////////////////////////////
	//				Material System ���� ���� ���Ǵ� �κ�.
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
	void LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	// ������ ���� ���� ����
	void LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	// SRC+DEST(ALPHA) ��� �߰���.
	void LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	// Material System ���� ���̴� �� �߰���.
	void LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	// Cloth �ùķ��̼� ���� �߰���.

	HRESULT LoadTexture();

	void InvalidateDeviceObjects ();

	static LPD3DXEFFECT SetFX( LPDIRECT3DDEVICEQ pd3dDevice );
	static LPD3DXEFFECT SetFX_mask2( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnResetDevice_STATIC ( LPDIRECT3DDEVICEQ pd3dDevice );
	static void OnLostDevice_STATIC ();
};

// ���ڿ� ������ ����. �� ��� �� ���ΰ�~?
enum HAT_HAIR_OPTION
{
	HAT_HAIR_NONE	= 0,	// �� ������ ���Ѵ�.
	HAT_HAIR_BASE	= 1,	// �⺻ �� �������Ѵ�.
	HAT_HAIR_NEW	= 2,	// HAT ���ý� ������ ���ο� �� ������ �Ѵ�.
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
	PSMESHCONTAINER		m_pmcMesh;				// Fixed <-> MaterialSystem ���� ��ȯ�� �� ���� ��ε��� �ʿ��ϴ�.
	DWORD				m_dwVertexNUM;			// ���� ������ �̸�.. �־� ���´�.

	DWORD				m_dwMaterialNum;
	SMATERIAL_PIECE*	m_arrayMaterialPiece;

	// Note : ���� ȿ���� ����ִ����� �˾Ƴ��� Flag �ǽð� ����
	//
	DWORD				m_dwFlags;

	// Note : ȿ�� ����, Save, Load
	DWORD				m_dwFlag;

	//	Note : ���� ��ġ�� ��.
	//
	VECTRACE			m_vecTrace;

	//	Note : ��Ų ������ ������ ȿ����.
	//
	GLEFFCHAR_VEC		m_vecEFFECT;
	//GLEFFCHAR_VEC		m_vecEFFECT_CharPart;		// DxCharPart���� ���̴� ��

	// Note : ���� Back Slot
	TSTRING				m_strUserBackSlot;
	D3DXMATRIX			m_matUserBackSlot;
	BOOL				m_bKeepUserBackSlot;

	BOOL				m_bCompletedLOAD;
	BOOL				m_bActiveErrorLog;

    // LOD ������ ������.
    SSkinCharLODInfo    m_kLODInfo;

	// 0x0113 ���������� mask ���� ���鵵�� �Ѵ�.
	BOOL				m_bMakeMaskMap;

private:
	BOOL										m_bMaterialSystem;		// Material System ���� ���۵Ǵ��� Ȯ���ϴ� �ڵ�.
	boost::scoped_ptr<EffectMaterialData>	m_scpEffectMaterialData;

	boost::scoped_ptr<DxSkeletonMaintain>	m_scpSkeleton;

	boost::scoped_ptr<DxPhysXCloth>			m_scpPhysXCloth;

	// Overlay or UserColor �� ���δٸ� ���� �ؽ��� ��� �� �ؽ��ĸ� ������ ����.
	LPDIRECT3DTEXTUREQ*	m_apOverlayTex;		// OverlayTexture

	// ���ڿ� ������ ����. �� ��� �� ���ΰ�~?
	HAT_HAIR_OPTION m_emHairHairOption;

	// ���� ���� �߰��� cps �� ������ �� �� �ֵ��� �Ѵ�.
	// ������ �ϺθӸ�ī���� ǥ���ϱ� ���� cps �� ���δ�.
	// m_bHairRenderOff �� TRUE �� ��츸 m_strAddHairCps �� ���۵ǵ��� �Ѵ�.
	TSTRING	m_strAddHairCps;

protected:
	void SumEffFlag ();

public:
	BOOL SelectMesh ( LPDIRECT3DDEVICEQ pd3dDevice, PSMESHCONTAINER pmcMesh );

public:
	void AddEffList ( DxEffChar* pEffChar );
	void DelEffList ( LPDIRECT3DDEVICEQ pd3dDevice, DxEffChar* pEffChar );
	void ClearEffList ();

	// Bone Matrix�� �����ش�.
	D3DXMATRIX GetInfluenceMatrix();

	// Set Bone Matrix
	void SetChangeMatrix( const D3DXMATRIX& matWorld );

	// ���ڿ� ������ ����. �� ��� �� ���ΰ�~?
	HAT_HAIR_OPTION GetHatHairOption() const							{ return m_emHairHairOption; }
	void			SetHatHairOption( HAT_HAIR_OPTION emType )			{ m_emHairHairOption=emType; }
	BOOL			IsHatHairOption( HAT_HAIR_OPTION emType ) const		{ return m_emHairHairOption==emType ? TRUE : FALSE; }

	// ���� ���� �߰��� cps �� ������ �� �� �ֵ��� �Ѵ�.
	// ������ �ϺθӸ�ī���� ǥ���ϱ� ���� cps �� ���δ�.
	// m_bHairRenderOff �� TRUE �� ��츸 m_strAddHairCps �� ���۵ǵ��� �Ѵ�.
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
	HRESULT LoadPiece_0108 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );	//	������ ���� ���� ����
	HRESULT LoadPiece_0109 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );	//	������ ���� ���� ���� ���� ����
	HRESULT LoadPiece_0110 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );	//	APS �߰�
    HRESULT LoadPiece_0111 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );	//	LOD �߰�
	HRESULT LoadPiece_0112_0113 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );	//	UserSlot �߰�
	HRESULT LoadPiece_0114 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );	//	PIECE_CAPE �߰�
	HRESULT LoadPiece_0115 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );	//	DxEffCharSingle �� Matrix ��� ������ ���� ����.
	HRESULT LoadPiece_0116 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );	//	���� �� ������ On/Off �� �� �ֵ��� �۾�.
	//HRESULT LoadPiece_0113 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bTexThread );	//	������2, �޼�2 �߰�;

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
	// m_pmcMesh ���� ���� ��� FALSE �� ����.
	// m_vecEFFECT ���� �ؽ��ĵ��� �ε��� �ȵǾ��� ��� FALSE �� ����.
	// �Ϲ� �ؽ��Ĵ� �ε��� �ȵǾ TRUE �� ����.
	//
	// m_vecEFFECT �� �ؽ��� �ε��� �ȵǸ� FALSE �� �ִ� ������ Mask ���� ������ �� �س���
	// �� �Ǿ����� Mask ���� �� �����ؾ� �ϴ��� ���� ���� �����̴�.
	//
	// [2014.09.25][shhan] bSetBaseTexture �߰���.
	//						cps �󿡼� ���ο� Mesh �� ������ ���� TRUE�� �༭ ���� �ؽ��ĸ� ����ϰ� �ϰ�(CharEdit��),
	//						chf �󿡼� cps �� ���� ��, �ٸ� Texture �� ���õ� ���¶�� FALSE �� �༭ �ٸ� Texture �� �е��� �Ѵ�(CharEdit �� Game).
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

	//	pBoneMatrice -> (����) ������ �޴� ���� ������ �������� ��쿡 ������ ����.
	HRESULT CalculateVertexInflu ( std::string strTrace, D3DXVECTOR3 &vVert, D3DXVECTOR3 &vNormal, LPD3DXMATRIX pBoneMatrice=NULL );

	HRESULT CalculateBoundingBox ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

    void ReloadVMFX( LPDIRECT3DDEVICEQ pd3dDevice );

    float GetDistanceFromCamera(const D3DXVECTOR3& vPos);
    HRESULT UpdateLOD(const D3DXVECTOR3& vPos, LPDIRECT3DDEVICEQ pd3dDevice);    
    int GetLODLevel(const D3DXVECTOR3& vPos);

public:
	DWORD GetVertexNUM()	{ return m_dwVertexNUM; }	// ���� ����.

	// NSThreadCORE ����
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

	// m_vecEFFECT �� ExportEffTexture ���� �ؽ��İ� �����ϴ��� Ȯ�� �ڵ�
	// ���ٸ� ������ ��ü�� �ȵǼ� �����۾���.
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
	// ���� �ؽ��ĸ� ã������ �ε��� ���� �غ����� �Ѵ�.
	void OnFindDontHaveTexture();

	// NSThreadCORE ����
public:
	void NSThreadCORE_UpdateData( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime );

protected:
	DxSkinPieceContainer();

public:
	~DxSkinPieceContainer();

public:
	static DxSkinPieceContainer& GetInstance();
};


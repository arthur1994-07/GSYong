#pragma once

#include <boost/tr1/unordered_map.hpp>

#include "../../DxTools/Material/MaterialFieldDef.h"
#include "../../DxTools/DxVertexFVF.h"

class DxPiece;
class DxPieceAABB;
class DxPieceInstancing;
class DxStaticAniFrame;
struct DxPieceNode;

// 옵션
enum
{
	COLL_ACTION_DONTMOVE = 0x0001,	// 충돌되면 움직일 수 없다.
};

// Note : 충돌메쉬를 사용할 것인가 ? 사용한다면 무엇을 사용할 것인가에 대한 옵션
enum EM_COLLMESH_OPT
{
	COLL_NONE = 0,
	COLL_MESH = 1,
	COLL_BSP = 2,
};

typedef std::vector<DxPiece*>	VEC_PIECE;
typedef VEC_PIECE::iterator		VEC_PIECE_ITER;

typedef std::set<DxPiece*>		SET_PIECE;
typedef SET_PIECE::iterator		SET_PIECE_ITER;

typedef DxPiece*		PDXPIECE;
typedef DxPieceNode*	PDXPIECENODE;

enum PIECE_OPTION
{
	//EMPO_SELECT_ANI		= 0x001,	// 선택했을 때만 애니메이션 되도록 작업함.
	//EMPO_BED				= 0x002,	// 침대인가~?
	//EMPO_HAVE_LIGHT		= 0x004,	// 조명을 가지고 있는가~?
	EMPO_ONLY_AMBIENT		= 0x008,	// 조명은 Ambient만 사용하겠는가~?
	EMPO_TALK_ALPHA_BLEND	= 0x010,	// NPC 대화시 카메라에 걸리면 알파블렌딩 하겠는가~?
    EMPO_PICK_ALPHA			= 0x020,	// Picking 된다면 AlphaBlend를 쓸것인가 ?
    EMPO_CAMERA_COLL		= 0x040,	// 카메라 충돌을 할 것인가 ?
    EMPO_MOUSE_PICKING		= 0x080,	// 마우스로 클릭하면 픽킹 활성/비활성
    EMPO_SHADOW_RENDER		= 0x100,	// 그림자 렌더링	TRUE/FALSE
    EMPO_FAR_RENDER			= 0x200,	// Far Render		TRUE/FALSE
	EMPO_UNSUPPORT_RAD		= 0x400,	// LightMap 을 굽지 않는다.~?	TRUE/FALSE
	EMPO_UNUSED				= 0x800,	// 디자이너가 LightMap UV 를 만든것을 사용한다. TRUE/FALSE - 사용되지 않는다.
	EMPO_ONLY_VOXELCOLOR	= 0x1000,	// Voxel Color 만 사용되어진다.
	EMPO_ANI_PLAY_ONCE		= 0x2000,	// Animation Play 는 한번만 된다.(loop가 안됨) 시작시간도 기존 랜덤에서 0 부터 시작한다.
};

enum EM_PIECE_VIEW
{
	EM_PIECE_VIEW_HIGH		= 0,
	EM_PIECE_VIEW_MIDDLE	= 1,
	EM_PIECE_VIEW_LOW		= 2,
};

// PieceName, PieceAABB
typedef std::tr1::unordered_map< std::string, DxPieceAABB* >	MAPNAMEPIECE;
typedef MAPNAMEPIECE::iterator									MAPNAMEPIECE_ITER;
typedef MAPNAMEPIECE::const_iterator							MAPNAMEPIECE_CITER;
typedef MAPNAMEPIECE::value_type								MAPNAMEPIECE_VALUE;

struct LightMapBoxColorData
{
	D3DXVECTOR4*	m_pAmbientCube;
	float*			m_fAmbientCubeSpot1;
	float*			m_fAmbientCubeSpot2;
	D3DXVECTOR4*	m_pVoxelColor;
	D3DXVECTOR2*	m_pSpot1_Spot2;

	LightMapBoxColorData::LightMapBoxColorData()
		: m_pAmbientCube(NULL)
		, m_fAmbientCubeSpot1(NULL)
		, m_fAmbientCubeSpot2(NULL)
		, m_pVoxelColor(NULL)
		, m_pSpot1_Spot2(NULL)
	{
	}
};

struct TriangleLinkLightForPiece
{
	PS_3_0_TYPE				m_emPS_3_0_Type;			// PS_3_0 버젼의 렌더타입
	//PS_3_0_TYPE_USED		m_emPS_3_0_Type_USED;		// PS_3_0 버젼의 렌더타입 - 실제 사용되어지는 Type
	VEC_DWORD				m_vecLightID;				// 영향 받는 Light를 구하도록 한다. - 중간값 ( 영향받는건 다 받는다. )
	WORDWORD				m_wwLightID;				// 영향 받는 Light를 구하도록 한다. - 최종값 ( 최대 2개만 골라낸다. )

	TriangleLinkLightForPiece::TriangleLinkLightForPiece()
		: m_emPS_3_0_Type(PS_3_0_TYPE_ZERO)
		//, m_emPS_3_0_Type_USED(PS_3_0_TYPE_USED_ZERO)
		, m_wwLightID(WORDWORD(-1,-1))
	{
	}
};

struct PieceParameter
{
	LPDIRECT3DDEVICEQ m_pd3dDevice;
	const D3DXMATRIX *m_rmatWorld;			// 참조만 한다.
	D3DXVECTOR2 m_vLightMapUV_Offset;
	float m_fAniTime;
	float m_fAlpha;
	int m_nRenderMode;
	LPDIRECT3DTEXTUREQ m_pTextureCombined;
	LPDIRECT3DTEXTUREQ m_pTextureDirect_Shadow_1;
	LPDIRECT3DTEXTUREQ m_pTextureDirect_Shadow_2;
	LPDIRECT3DTEXTUREQ m_pTextureIndirect;
	LightMapBoxColorData m_sLMBCData;
	const TriangleLinkLightForPiece*	m_pTriangleLinkLight;

	PieceParameter()
		: m_pd3dDevice(NULL)
		, m_rmatWorld(NULL)
		, m_pTextureCombined(NULL)
		, m_pTextureDirect_Shadow_1(NULL)
		, m_pTextureDirect_Shadow_2(NULL)
		, m_pTextureIndirect(NULL)
		, m_pTriangleLinkLight(NULL)
	{
	};
};

struct PIECE_INSTANCING_FIRST
{
	const DxStaticAniFrame*	m_rStaticAniFrame;		// 참조만 한다.
	TSTRING					m_strTextureCombined;	// 

	bool operator<(const PIECE_INSTANCING_FIRST& rhs)    const
	{
		if ( std::make_pair(m_rStaticAniFrame,m_strTextureCombined) < std::make_pair(rhs.m_rStaticAniFrame,rhs.m_strTextureCombined) )
			return true;

		return false;				
	}

	bool operator>(const PIECE_INSTANCING_FIRST& rhs)    const
	{
		if ( std::make_pair(m_rStaticAniFrame,m_strTextureCombined) > std::make_pair(rhs.m_rStaticAniFrame,rhs.m_strTextureCombined) )
			return true;

		return false;
	}

	bool operator != ( const PIECE_INSTANCING_FIRST& rhs ) const
	{
		if ( std::make_pair(m_rStaticAniFrame,m_strTextureCombined) != std::make_pair(rhs.m_rStaticAniFrame,rhs.m_strTextureCombined) )
			return true;
		else
			return false;
	}

	PIECE_INSTANCING_FIRST( const DxStaticAniFrame*	pStaticAniFrame, const TSTRING& strTextureCombined )
		: m_rStaticAniFrame(pStaticAniFrame)
		, m_strTextureCombined(strTextureCombined)
	{

	}
};

// Piece 종류, VecPiece 정보
typedef std::map<PIECE_INSTANCING_FIRST,std::tr1::shared_ptr<DxPieceInstancing>>	MAP_PIECE_INSTANCING;
typedef MAP_PIECE_INSTANCING::iterator												MAP_PIECE_INSTANCING_ITER;
typedef MAP_PIECE_INSTANCING::value_type											MAP_PIECE_INSTANCING_VALUE;


enum
{
	EM_INSTANCING_PIECE_MIN = 4,	// Instancing 이 되기 위한 최소 Piece 갯수
};
#pragma once

#include <boost/tr1/unordered_map.hpp>

#include "../../DxTools/Material/MaterialFieldDef.h"
#include "../../DxTools/DxVertexFVF.h"

class DxPiece;
class DxPieceAABB;
class DxPieceInstancing;
class DxStaticAniFrame;
struct DxPieceNode;

// �ɼ�
enum
{
	COLL_ACTION_DONTMOVE = 0x0001,	// �浹�Ǹ� ������ �� ����.
};

// Note : �浹�޽��� ����� ���ΰ� ? ����Ѵٸ� ������ ����� ���ΰ��� ���� �ɼ�
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
	//EMPO_SELECT_ANI		= 0x001,	// �������� ���� �ִϸ��̼� �ǵ��� �۾���.
	//EMPO_BED				= 0x002,	// ħ���ΰ�~?
	//EMPO_HAVE_LIGHT		= 0x004,	// ������ ������ �ִ°�~?
	EMPO_ONLY_AMBIENT		= 0x008,	// ������ Ambient�� ����ϰڴ°�~?
	EMPO_TALK_ALPHA_BLEND	= 0x010,	// NPC ��ȭ�� ī�޶� �ɸ��� ���ĺ��� �ϰڴ°�~?
    EMPO_PICK_ALPHA			= 0x020,	// Picking �ȴٸ� AlphaBlend�� �����ΰ� ?
    EMPO_CAMERA_COLL		= 0x040,	// ī�޶� �浹�� �� ���ΰ� ?
    EMPO_MOUSE_PICKING		= 0x080,	// ���콺�� Ŭ���ϸ� ��ŷ Ȱ��/��Ȱ��
    EMPO_SHADOW_RENDER		= 0x100,	// �׸��� ������	TRUE/FALSE
    EMPO_FAR_RENDER			= 0x200,	// Far Render		TRUE/FALSE
	EMPO_UNSUPPORT_RAD		= 0x400,	// LightMap �� ���� �ʴ´�.~?	TRUE/FALSE
	EMPO_UNUSED				= 0x800,	// �����̳ʰ� LightMap UV �� ������� ����Ѵ�. TRUE/FALSE - ������ �ʴ´�.
	EMPO_ONLY_VOXELCOLOR	= 0x1000,	// Voxel Color �� ���Ǿ�����.
	EMPO_ANI_PLAY_ONCE		= 0x2000,	// Animation Play �� �ѹ��� �ȴ�.(loop�� �ȵ�) ���۽ð��� ���� �������� 0 ���� �����Ѵ�.
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
	PS_3_0_TYPE				m_emPS_3_0_Type;			// PS_3_0 ������ ����Ÿ��
	//PS_3_0_TYPE_USED		m_emPS_3_0_Type_USED;		// PS_3_0 ������ ����Ÿ�� - ���� ���Ǿ����� Type
	VEC_DWORD				m_vecLightID;				// ���� �޴� Light�� ���ϵ��� �Ѵ�. - �߰��� ( ����޴°� �� �޴´�. )
	WORDWORD				m_wwLightID;				// ���� �޴� Light�� ���ϵ��� �Ѵ�. - ������ ( �ִ� 2���� ��󳽴�. )

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
	const D3DXMATRIX *m_rmatWorld;			// ������ �Ѵ�.
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
	const DxStaticAniFrame*	m_rStaticAniFrame;		// ������ �Ѵ�.
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

// Piece ����, VecPiece ����
typedef std::map<PIECE_INSTANCING_FIRST,std::tr1::shared_ptr<DxPieceInstancing>>	MAP_PIECE_INSTANCING;
typedef MAP_PIECE_INSTANCING::iterator												MAP_PIECE_INSTANCING_ITER;
typedef MAP_PIECE_INSTANCING::value_type											MAP_PIECE_INSTANCING_VALUE;


enum
{
	EM_INSTANCING_PIECE_MIN = 4,	// Instancing �� �Ǳ� ���� �ּ� Piece ����
};
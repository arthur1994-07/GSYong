#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <set>
#include <map>
#include <list>
#include <vector>
#include "../../../RanThirdParty/DirectX/Include/d3dx9math.h"

#define DIV_1_255	0.003922f

//float	g_1_RAND_MAX	= 1.f/RAND_MAX;	//0.000031f, 3.05185e-005
#define RANDOM_01		(((FLOAT)rand())*0.000031f)	// 0.f ~ 1.f
#define RANDOM_11		( (RANDOM_01*2.f)-1.f )			// -1.f ~ 1.f

typedef std::set<WORD>		SET_WORD;
typedef SET_WORD::iterator	SET_WORD_ITER;

typedef std::set<DWORD>				SET_DWORD;
typedef SET_DWORD::iterator			SET_DWORD_ITER;
typedef SET_DWORD::const_iterator	SET_DWORD_CITER;

typedef std::list<DWORD>		LIST_DWORD;
typedef LIST_DWORD::iterator	LIST_DWORD_ITER;

typedef std::vector<DWORD>	VEC_DWORD;
typedef VEC_DWORD::iterator	VEC_DWORD_ITER;

typedef std::vector<BOOL>	VEC_BOOL;
typedef VEC_BOOL::iterator	VEC_BOOL_ITER;

typedef std::vector<D3DXVECTOR3>	VEC_VECTOR3;
typedef VEC_VECTOR3::iterator		VEC_VECTOR3_ITER;

#ifdef UNICODE
    typedef std::vector<std::wstring>	VEC_TSTRING;
    typedef VEC_TSTRING::iterator		VEC_TSTRING_ITER;

    typedef std::map<std::wstring,DWORD>		MAP_TSTRING_DWORD;
    typedef MAP_TSTRING_DWORD::iterator			MAP_TSTRING_DWORD_ITER;
    typedef MAP_TSTRING_DWORD::const_iterator	MAP_TSTRING_DWORD_CITER;
#else
    typedef std::vector<std::string>	VEC_TSTRING;
    typedef VEC_TSTRING::iterator		VEC_TSTRING_ITER;

    typedef std::tr1::unordered_map<std::string, DWORD>	MAP_TSTRING_DWORD;
    typedef MAP_TSTRING_DWORD::iterator		        	MAP_TSTRING_DWORD_ITER;
    typedef MAP_TSTRING_DWORD::const_iterator	        MAP_TSTRING_DWORD_CITER;
    typedef MAP_TSTRING_DWORD::value_type    	        MAP_TSTRING_DWORD_VALUE;
#endif

struct VERTEX 
{ 
	D3DXVECTOR3 vPos; 
	D3DXVECTOR3 vNor; 
	D3DXVECTOR2 vTex;
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
};
typedef VERTEX* PVERTEX;

struct VERTEX_WORLD	// World 상에서 쓰이는 것 // Color는 Blend에 필요. // Tex[2] 는 LightMap에 필요함.
{ 
	D3DXVECTOR3 vPos; 
	D3DXVECTOR3 vNor;
	DWORD		dwColor;
	D3DXVECTOR2 vTex[4];
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX4;
};
typedef VERTEX_WORLD* PVERTEX_WORLD;

struct VERTEXNOR
{ 
	D3DXVECTOR3 vPos; 
	D3DXVECTOR3 vNor; 
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL;

	VERTEXNOR& operator*( const float r )
	{
		vPos *= r;
		vNor *= r;
		D3DXVec3Normalize( &vNor, &vNor );
		return *this;
	}

	VERTEXNOR& operator/( const float r )
	{
		vPos /= r;
		vNor /= r;
		D3DXVec3Normalize( &vNor, &vNor );
		return *this;
	}

	VERTEXNOR& operator+( const VERTEXNOR& r )
	{
		vPos += r.vPos;
		vNor += r.vNor;
		return *this;
	}
};
typedef VERTEXNOR* PVERTEXNOR;

struct VERTEXNORTEX2
{ 
	D3DXVECTOR3 vPos; 
	D3DXVECTOR3 vNor; 
	D3DXVECTOR2 vTex[2];
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2;
};
typedef VERTEXNORTEX2* PVERTEXNORTEX2;


struct VERTEXNORTEXTANGENT
{ 
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vNor;
	D3DXVECTOR2 vTex;
	D3DXVECTOR3 vTan;
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2|D3DFVF_TEXCOORDSIZE3(1);
};
typedef VERTEXNORTEXTANGENT* PVERTEXNORTEXTANGENT;


struct VERTEXNORTEXTANGENT_VS
{ 
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vNor;
	D3DXVECTOR2 vTex;
	D3DXVECTOR3 vTan;
    D3DXVECTOR4 vWeight;
    D3DXVECTOR4 vIndies;
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX4|D3DFVF_TEXCOORDSIZE3(1)|D3DFVF_TEXCOORDSIZE4(2)|D3DFVF_TEXCOORDSIZE4(3);
};
typedef VERTEXNORTEXTANGENT_VS* PVERTEXNORTEXTANGENT_VS;

struct POSONLY
{ 
	D3DXVECTOR3 vPos; 
	static const DWORD FVF = D3DFVF_XYZ;
};
typedef POSONLY* PPOSONLY;

struct POSTEX
{ 
	D3DXVECTOR3 vPos; 
	D3DXVECTOR2 vTex;
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_TEX1;
};
typedef POSTEX* PPOSTEX;

struct POSCUBETEX
{ 
	D3DXVECTOR3 vPos; 
	D3DXVECTOR3 vTex;
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE3(0);
};
typedef POSCUBETEX* PPOSCUBETEX;

struct POSTEX2
{ 
	D3DXVECTOR3 vPos; 
	D3DXVECTOR2 vTex[2];
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_TEX2;
};
typedef POSTEX2* PPOSTEX2;

struct VERTEXCOLOR
{
	D3DXVECTOR3 vPos;
	DWORD		dwColor;
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE;
};
typedef VERTEXCOLOR* PVERTEXCOLOR;

struct VERTEXCOLORTEX
{
	union
	{
		struct
		{
			D3DXVECTOR3 vPos;
			DWORD		dwColor;
			D3DXVECTOR2 vTex;
		};

		struct
		{
			float x,y,z;
			DWORD dwColor;
			float tex_u1,tex_v1;
		};

	};

	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
};
typedef VERTEXCOLORTEX* PVERTEXCOLORTEX;

struct VERTEXCOLOR2TEX
{
	D3DXVECTOR3 vPos;
	DWORD		dwColor;
	DWORD		dwSpecular;
	D3DXVECTOR2 vTex;
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_SPECULAR|D3DFVF_TEX1;
};
typedef VERTEXCOLOR2TEX* PVERTEXCOLOR2TEX;


struct VERTEXNORCOLORTEX
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vNor;
	DWORD		dwColor;
	D3DXVECTOR2 vTex;
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1;
};
typedef VERTEXNORCOLORTEX* PVERTEXNORCOLORTEX;


struct VERTEXNORCOLORTEX2
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vNor;
	DWORD		dwColor;
	D3DXVECTOR2 vTex[2];
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX2;
};

struct VERTEXNORCOLORTEX3
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vNor;
	DWORD		dwColor;
	D3DXVECTOR2 vTex[3];
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX3;
};

struct VERTEXNORCOLORTEX4
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vNor;
	DWORD		dwColor;
	D3DXVECTOR2 vTex[4];
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX4;
};

struct VERTEXNORCOLORTEX5
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vNor;
	DWORD		dwColor;
	D3DXVECTOR2 vTex[5];
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX5;
};


struct VERTEXCOLORTEX2
{ 
	union
	{
		struct
		{
			D3DXVECTOR3 vPos;
			DWORD		dwColor;
			D3DXVECTOR2 vTex01;
			D3DXVECTOR2 vTex02;
		};

		struct
		{
			float x,y,z;
			DWORD dwColor;
			float tex_u1,tex_v1;
			float tex_u2,tex_v2;
		};
	};
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2;
};
typedef VERTEXCOLORTEX2* PVERTEXCOLORTEX2;

struct VERTEXCOLORTEX3
{ 
	D3DXVECTOR3 vPos;
	DWORD		dwColor;
	D3DXVECTOR2 vTex01;
	D3DXVECTOR2 vTex02;
	D3DXVECTOR2 vTex03;
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX3;
};
typedef VERTEXCOLORTEX3* PVERTEXCOLORTEX3;

struct VERTEXRHW
{
	D3DXVECTOR4 vPos;
	D3DXVECTOR2 vTex;
	static const DWORD FVF = D3DFVF_XYZRHW|D3DFVF_TEX1;
};
typedef VERTEXRHW* PVERTEXRHW;

struct VERTEXRHWTEX2
{
	D3DXVECTOR4 vPos;
	D3DXVECTOR2 vTex1;
	D3DXVECTOR2 vTex2;
	static const DWORD FVF = D3DFVF_XYZRHW|D3DFVF_TEX2;
};
typedef VERTEXRHWTEX2* PVERTEXRHWTEX2;

struct VERTEXCOLORRHW
{
	D3DXVECTOR4 vPos;
	D3DCOLOR	dwColor;
	D3DXVECTOR2 vTex;
	static const DWORD FVF = D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1;
};
typedef VERTEXCOLORRHW* PVERTEXCOLORRHW;

struct VERTEXTEX2
{ 
	D3DXVECTOR3 vPos;
	D3DXVECTOR2 vTex01;
	D3DXVECTOR2 vTex02;
	float		fGarbage;	// 불필요 물건		// Data를 맞추기 위하여 넣음.
	static const DWORD FVF = D3DFVF_XYZ|D3DFVF_TEX3|D3DFVF_TEXCOORDSIZE1(2);
};
typedef VERTEXTEX2* PVERTEXTEX2;

//struct CLIPVOLUME
//{
//	D3DXPLANE pLeft, pRight;
//	D3DXPLANE pTop, pBottom;
//	D3DXPLANE pNear, pFar;
//
//	CLIPVOLUME() {}
//};

//struct	DIFFUSEVERTEX
//{
//	union
//	{
//		struct
//		{
//			float x, y, z;
//		};
//		struct
//		{
//			D3DXVECTOR3 vPos;
//		};
//	};
//    
//	DWORD	Diffuse;
//
//public:
//	DIFFUSEVERTEX ()
//	{		
//	}
//
//	DIFFUSEVERTEX ( float _x, float _y, float _z, DWORD diffuse )
//	{
//		x = _x; y = _y; z = _z; Diffuse = diffuse;
//	}
//	DIFFUSEVERTEX ( D3DXVECTOR3 pos, DWORD diffuse )
//	{
//		vPos = pos; Diffuse = diffuse;
//	}
//
//	const static DWORD FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE;
//};

struct WORDWORD
{
	union
	{
		struct
		{
			WORD wA;
			WORD wB;
		};
		struct
		{
			DWORD dwData; //! 64 bit problem			
		};
	};

	WORDWORD() :
		dwData(0L)
	{
	}

	~WORDWORD()
	{
	}

	WORDWORD( DWORD _dwData )
	{
		dwData = _dwData;
	}

	WORDWORD( WORD _wA, WORD _wB )
	{
		wA = _wA;	wB = _wB;
	}

	void Init()
	{
		dwData = 0L;
	}
};

#include <list>
typedef std::list<ULONG_PTR>			LOADINGDATALIST;
typedef LOADINGDATALIST::iterator	LOADINGDATALIST_ITER;
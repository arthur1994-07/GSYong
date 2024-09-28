#ifndef	__DXCUSTOMTYPES__H__
#define	__DXCUSTOMTYPES__H__

#pragma once

#include <list>
#include "../../=DX_Include/d3dx9math.h"

struct OCTREEVERTEX
{
	union
	{
		struct
		{
			float x, y, z;
		};
		struct
		{
			D3DXVECTOR3 vPos;
		};
	};

	union
	{
		struct
		{
			float nx, ny, nz;
		};
		struct
		{
			D3DXVECTOR3 vNormal;
		};
	};

	float tu, tv;
	
	OCTREEVERTEX() :
		x(0),y(0),z(0),
		nx(0.5f),ny(0.5f),nz(0.5f),
		tu(0),tv(0)
	{
	}
	OCTREEVERTEX(float X , float Y, float Z) :
		nx(0.5f),ny(0.5f),nz(0.5f),
		tu(0),tv(0)
	{ 
	}
	const static DWORD FVF;
};

struct CLIPVOLUME
{
	D3DXPLANE pLeft, pRight;
	D3DXPLANE pTop, pBottom;
	D3DXPLANE pNear, pFar;

	CLIPVOLUME() {}
};

struct	DIFFUSEVERTEX
{
	union
	{
		struct
		{
			float x, y, z;
		};
		struct
		{
			D3DXVECTOR3 vPos;
		};
	};
    
	DWORD	Diffuse;

public:
	DIFFUSEVERTEX ()
	{		
	}

	DIFFUSEVERTEX ( float _x, float _y, float _z, DWORD diffuse )
	{
		x = _x; y = _y; z = _z; Diffuse = diffuse;
	}
	DIFFUSEVERTEX ( D3DXVECTOR3 pos, DWORD diffuse )
	{
		vPos = pos; Diffuse = diffuse;
	}

	const static DWORD FVF;
};

typedef std::list<DWORD_PTR>            LOADINGDATALIST;
typedef LOADINGDATALIST::iterator	    LOADINGDATALIST_ITER;
typedef LOADINGDATALIST::const_iterator LOADINGDATALIST_CITER;

#endif	//	__DXCUSTOMTYPES__H__
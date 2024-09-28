#ifndef	__DXCUSTOMTYPES__H__
#define	__DXCUSTOMTYPES__H__

#pragma once

#include "../../InternalCommonLib/dxstdafx.h"
#include <list>

struct CLIPVOLUME
{
	D3DXPLANE pLeft, pRight;
	D3DXPLANE pTop, pBottom;
	D3DXPLANE pNear, pFar;

	D3DXPLANE pDetail[4];

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

#endif	//	__DXCUSTOMTYPES__H__
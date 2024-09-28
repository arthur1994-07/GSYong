#pragma once

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../DxTools/DxVertexFVF.h"

class DxLandMan;
class NavigationMesh;

//------------------------------------------------------------------------------------------------------------
//								D	x		F	o	o	t		C	o	l	l
//------------------------------------------------------------------------------------------------------------
class DxFootColl
{
private:
	enum
	{
		VERSION = 0x0100,
	};

	enum MODE
	{
		MODE_NONE = 0,
		MODE_POINT2 = 1,
		MODE_POINT4 = 2,
	};

	// Data
private:
	MODE		m_emMode;			// 4�� �����ΰ�~? 2�� �������ΰ�~? �׳� �⺻���ΰ�~?
	VEC_VECTOR3 m_vecFootColl_2;	// 2�� ������....
	VEC_VECTOR3 m_vecFootColl_4;	// 4�� ����

	// Game
public:
	BOOL GetMatrix( D3DXMATRIX &matWorldInOut, DxLandMan *pLandMan, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, const D3DXVECTOR3 &vDirOrig, NavigationMesh* pNavigation = NULL ) const;

private:
	BOOL CalculateCollisionY( const VEC_VECTOR3& vecFootColl, DxLandMan *pLandMan, const D3DXVECTOR3 &vPos, const D3DXMATRIX &matRotate ) const;

public:

private:
	friend class CFootCollPage;

public:
	void CleanUp();

	void Save( sc::SerialFile& SFile );
	void Load( sc::BaseStream& SFile );

public:
	DxFootColl();
	~DxFootColl();
};
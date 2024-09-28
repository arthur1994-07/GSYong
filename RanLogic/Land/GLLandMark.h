#ifndef GLLANDMARK_H_
#define GLLANDMARK_H_

#pragma once

#include "../../SigmaCore/File/BaseStream.h"
#include "../../EngineLib/DxTools/Collision.h"
#include "../../EngineLib/DxTools/DxMethods.h"

struct GLLANDMARK : public DXAFFINEMATRIX//, public OBJAABB
{
	enum { VERSION = 0x0003, };
	const static D3DXVECTOR3	m_vHIGHER;
	const static float			m_fSPHERE_S1;

	std::string			m_strMARK;				// KeyWord
	DWORD				m_nKeyWordIndex;		// KeyWordIndex
	DWORD				m_dwColor;
	TSTRING				m_strConvertName;		// Dont Save Load	- 변환된 이름.
	DWORD				m_dwID;					// Dont Save Load

	D3DXVECTOR3			m_vMax;					//	AABB에 사용. Dont Save Load
	D3DXVECTOR3			m_vMin;					//	AABB에 사용. Dont Save Load

	//GLLANDMARK*			m_pNext;

public:
	void operator= ( const GLLANDMARK &sVALUE );
	void CALC_BOX ();
	BOOL IsCollision ( CLIPVOLUME &cv ) const;
	const TSTRING& GetKeyWordName() const	{ return m_strMARK;	}
	const TSTRING& GetConvertName() const	{ return m_strConvertName; }
	DWORD GetColor() const					{ return m_dwColor; }
	const D3DXVECTOR3& GetPosition() const	{ return m_pAffineParts->vTrans; }

public:
	void ConvertName();

//public:
//	void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
//	{
//		vMax = m_vMax; vMin = m_vMin;
//	}
//
//	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
//	{
//		return COLLISION::IsWithInPoint ( vDivMax, vDivMin, DXAFFINEMATRIX::m_pAffineParts->vTrans );
//	}
//
//	virtual OBJAABB* GetNext ()
//	{
//		return m_pNext;
//	}
//
//	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );
	HRESULT RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );

public:
	BOOL SAVE ( sc::SerialFile &SFile );
	BOOL LOAD ( sc::BaseStream &SFile );

public:
	GLLANDMARK ();
	~GLLANDMARK ();
};

typedef GLLANDMARK* PLANDMARK;

typedef std::vector<PLANDMARK>	VECLANDMARK;
typedef VECLANDMARK::iterator	VECLANDMARK_ITER;

#endif // GLLANDMARK_H_
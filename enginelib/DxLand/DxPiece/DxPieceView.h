#ifndef _DX_PIECE_VIEW_H_
#define _DX_PIECE_VIEW_H_

#pragma once

#include "../../DxTools/DxVertexFVF.h"

class DxPieceEdit;

namespace sc
{
    class SerialFile;
};


class DxPieceView	// Edit 할때만 사용한다.
{
private:
	enum
	{
		VERSION = 0x0102,
	};

private:
	VEC_TSTRING	m_vecPIE;
	VEC_TSTRING	m_vecSPT;

	LPDIRECT3DDEVICEQ m_pd3dDevice;

public:
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );

//-------------------------------------------------------------
//							E	D	I	T
private:
	BOOL InsertPIE( const TCHAR* pName );	// Return : Sucessed or Failed
	BOOL InsertSPT( const TCHAR* pName );	// Return : Sucessed or Failed
	void DeletePIE( int nIndex );
	void DeleteSPT( int nIndex );
	void DeletePIE_ALL();
	void DeleteSPT_ALL();
	DxPieceEdit* GetPiece( DWORD dwIndex );
	const TCHAR* GetPieceName( DWORD dwIndex );
	const TCHAR* GetSPTName( DWORD dwIndex );

//-------------------------------------------------------------
//		E	D	I	T		F	U	N	C	T	I	O	N
private:
	friend class CDlgPieceEdit;
	//friend class CDlgSPTEdit;

//-------------------------------------------------------------
//			S	A	V	E		&&		L	O	A	D
public:
	void Save( sc::SerialFile &SFile );
	void Load( sc::SerialFile &SFile );

	void CleanUp();

public:
	DxPieceView();
	~DxPieceView();
};

#endif // _DX_PIECE_VIEW_H_
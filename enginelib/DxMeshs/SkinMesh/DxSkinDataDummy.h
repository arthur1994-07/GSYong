#pragma once
#include <string>
#include "../../../SigmaCore/Container/MList.h"
#include "DxSkinCharData.h"

class DxSkinDataDummy
{
	typedef std::vector<SANIMCONINFO*>	VECANIMCONINFO;

public:
	std::string				m_strFileName;

	float					m_fScale;
	float					m_fHeightYPos;
	std::string				m_strPIECE[PIECE_SIZE];
	std::string				m_strSkeleton;

	VECANIMCONINFO			m_vecANIMINFO;
	D3DXVECTOR3				m_vMax, m_vMin;

protected:
	BOOL LOAD_0100 ( sc::BaseStream &SFile );
	BOOL LOAD_0101 ( sc::BaseStream &SFile );
	BOOL LOAD_0104 ( sc::BaseStream &SFile );
	BOOL LOAD_0105 ( sc::BaseStream &SFile );
	BOOL LOAD_0106 ( sc::BaseStream &SFile );
    BOOL LOAD_0107_108 ( sc::BaseStream &SFile );

public:
    BOOL LoadFile(const std::string& FileName);
	void CleanUp ();

public:
	SANIMCONINFO* FindAniInfo ( const char* szName );
	SANIMCONINFO* FindAniInfo ( EMANI_MAINTYPE emMain, EMANI_SUBTYPE emSub );

public:
	DxSkinDataDummy(void);
	~DxSkinDataDummy(void);
};

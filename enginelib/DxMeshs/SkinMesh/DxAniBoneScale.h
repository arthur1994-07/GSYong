#pragma once

#include <map>
#include "../../../SigmaCore/File/SerialFile.h"
#include "./DxSkeletonManager.h"


enum UPDATETYPE
{
	UPDATE_X = 0,
	UPDATE_Y,
	UPDATE_Z
};

//------------------------------------------------------------------------------------------------------------
//						D	x		A	n	i		S	c	a	l	e		M	A	I	N
//------------------------------------------------------------------------------------------------------------
struct DxAniScaleMAIN
{
	typedef std::map<DWORD,D3DXVECTOR3>	 MAP_ANISCALEDATA;		//  KEY, SCALE
	typedef MAP_ANISCALEDATA::iterator	 MAP_ANISCALEDATA_ITER;
    typedef MAP_ANISCALEDATA::value_type MAP_ANISCALEDATA_VALUE;

	MAP_ANISCALEDATA		m_mapAniScaleData;

	void UpdateBoneScale( DxBoneTrans* pBoneTrans, float fWeight, BOOL bFirst, float fTime, float fSTime, float fETime, float UnitTime, UPDATETYPE emType );

	void InsertData( DWORD dwKey, const D3DXVECTOR3& vScale );
	void ModifyData( int nIndex, const D3DXVECTOR3& vScale );
	void DeleteData( int nIndex );

	void Save( sc::SerialFile& SFile );
	void Load( sc::BaseStream& SFile );

	DxAniScaleMAIN& operator = ( DxAniScaleMAIN &value );
};

//------------------------------------------------------------------------------------------------------------
//								D	x		A	n	i		S	c	a	l	e
//------------------------------------------------------------------------------------------------------------
class DxAniScale
{
public:
    typedef std::map<std::string, DxAniScaleMAIN*> MAP_ANISCALEMAIN;
	typedef MAP_ANISCALEMAIN::iterator			   MAP_ANISCALEMAIN_ITER;
    typedef MAP_ANISCALEMAIN::value_type           MAP_ANISCALEMAIN_VALUE;

	MAP_ANISCALEMAIN	m_mapAniScaleMain;

public:
	void UpdateBoneScale( DxSkeleton* pSkeleton, float fWeight, BOOL bFirst, float fTime, float fSTime, float fETime, float UnitTime );

public:
    void InsertAniScale(const std::string& AniName);
	void DeleteAniScale( int nIndex );

	DxAniScaleMAIN* GetAniScaleMAIN( int nIndex );

public:
	void CleanUp();

	void Save(sc::SerialFile& SFile);
	void Load(sc::BaseStream& SFile);

	DxAniScale& operator = ( DxAniScale &value );

public:
	DxAniScale();
	~DxAniScale();
};
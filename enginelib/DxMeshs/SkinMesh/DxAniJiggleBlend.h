#pragma once

namespace sc
{
	class SerialFile;
	class BaseStream;
};

class DxAniJiggleBlend
{
private:
	enum
	{
		VERSION = 0x100,
	};

public:
	//				Key, BLEND;
	typedef std::map<DWORD,float>					MAP_KEY_BLEND;
	typedef std::map<DWORD,float>::iterator			MAP_KEY_BLEND_ITER;
	typedef std::map<DWORD,float>::const_iterator	MAP_KEY_BLEND_CITER;

	//		[Base, AttachBone], MAP_KEY_BLEND;
	typedef std::map<int,std::tr1::shared_ptr<MAP_KEY_BLEND>>	MAP_ATTACHBONE_KEY_BLEND;
	typedef MAP_ATTACHBONE_KEY_BLEND::iterator					MAP_ATTACHBONE_KEY_BLEND_ITER;
	typedef MAP_ATTACHBONE_KEY_BLEND::const_iterator			MAP_ATTACHBONE_KEY_BLEND_CITER;

private:
	MAP_ATTACHBONE_KEY_BLEND	m_mapAttachBoneKeyBlend;

public:
	void Insert( int nIndex, DWORD dwKey, float fBlend );
	void Delete( int nIndexA, DWORD dwIndex );

	// 0 KeyFrame <-> 1 JiggleBone
	void GetKeyJiggleWeight_PureThread( float afKeyJiggleWeight[], DWORD dwKeyJiggleWeightNUM, DWORD dwKey ) const;

	const MAP_KEY_BLEND* GetMapKeyBlend( int nIndex );

private:
	MAP_KEY_BLEND* GetMapKeyBlend_NoneConst( int nIndex );

//public:
//	friend class CAniJiggleBlendPage;

public:
	BOOL Save( sc::SerialFile& SFile );
	BOOL Load( sc::BaseStream& SFile );

public:
	DxAniJiggleBlend();
	~DxAniJiggleBlend();
};
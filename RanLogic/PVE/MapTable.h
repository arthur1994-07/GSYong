#ifndef PVE_MAP_TABLE_H
#define PVE_MAP_TABLE_H

#include <vector>
#include "../=MsgPack/include/msgpack.hpp"

typedef __time64_t _TIME_TYPE;

/**
	인던 DB 저장 테이블
	클라/서버에서 공용으로 사용한다.

 */
struct MAP_TABLE
{
	TSTRING strID;		//@ 고유 구분자, 캐릭터 DB ID + MAPID(main + sub)
	DWORD dwCharDbNum;	//@ 캐릭터 DB ID
	DWORD dwMapID;		//@ 맵 ID
	int nInCount;		//@ 입장 카운트
	int nGenCount;		//@ 생성 카운트
    int nLevel;         //@ 입장 한 맵의 레벨(등급)
	_TIME_TYPE tGenTime;//@ 서버에서는 생성 한 시간스템프, 클라에서는 젠까지 남은 시간(초)

	MSGPACK_DEFINE(strID, dwCharDbNum, dwMapID, nInCount, nGenCount, nLevel, tGenTime);
};

typedef std::vector<MAP_TABLE> MapTableContainer;
typedef MapTableContainer::iterator MapTableContainerIterator;
typedef MapTableContainer::const_iterator MapTableContainerConstIterator;

#endif

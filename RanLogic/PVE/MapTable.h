#ifndef PVE_MAP_TABLE_H
#define PVE_MAP_TABLE_H

#include <vector>
#include "../=MsgPack/include/msgpack.hpp"

typedef __time64_t _TIME_TYPE;

/**
	�δ� DB ���� ���̺�
	Ŭ��/�������� �������� ����Ѵ�.

 */
struct MAP_TABLE
{
	TSTRING strID;		//@ ���� ������, ĳ���� DB ID + MAPID(main + sub)
	DWORD dwCharDbNum;	//@ ĳ���� DB ID
	DWORD dwMapID;		//@ �� ID
	int nInCount;		//@ ���� ī��Ʈ
	int nGenCount;		//@ ���� ī��Ʈ
    int nLevel;         //@ ���� �� ���� ����(���)
	_TIME_TYPE tGenTime;//@ ���������� ���� �� �ð�������, Ŭ�󿡼��� ������ ���� �ð�(��)

	MSGPACK_DEFINE(strID, dwCharDbNum, dwMapID, nInCount, nGenCount, nLevel, tGenTime);
};

typedef std::vector<MAP_TABLE> MapTableContainer;
typedef MapTableContainer::iterator MapTableContainerIterator;
typedef MapTableContainer::const_iterator MapTableContainerConstIterator;

#endif

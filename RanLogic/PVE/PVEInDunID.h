#ifndef SPVEInDunID_H
#define SPVEInDunID_H

#include <map>
#include "../=MsgPack/include/msgpack.hpp"

/**
    인던 구분 구조체
    현재는 캐릭터 고유 DB ID 와 맵 ID 로 고유하게 구분 한다.
    향후 동일 맵이라도 등급에 따라 구분해 줘야 한다면 이곳에 추가해 주면 된다.

 */
struct SPVEInDunID
{
	SPVEInDunID();

	void	Init();
	bool	IsVaild() const;

    DWORD   dwCharDbNum;    //@ 파티장 캐릭터 고유 DB ID
    DWORD   dwMapID;        //@ 생성할 인던 맵 ID, (생성된이 아님에 주의)
    int     nLevel;         //@ 생성할 인던 등급(난이도)
	int		nNum;			//@ 인던 생성 카운트(고유한 구분을 위해 사용 된다.)

    /**
        정렬 및 검색 기준으로써의 연산자 정의
        우리는 캐릭터 고유 DB ID 와 맵 ID 로 구분 한다.
        만약 이것들이 동일하다면 동일한 인던으로 정의 한다.

     */
    bool    operator < (const SPVEInDunID& rhs) const;

	// 동등 비교 연산자
	bool    operator == (const SPVEInDunID& rhs) const;
	bool    operator != (const SPVEInDunID& rhs) const;

    MSGPACK_DEFINE(dwCharDbNum, dwMapID, nLevel, nNum);
};

#endif
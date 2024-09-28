#ifndef SPVEInDunID_H
#define SPVEInDunID_H

#include <map>
#include "../=MsgPack/include/msgpack.hpp"

/**
    �δ� ���� ����ü
    ����� ĳ���� ���� DB ID �� �� ID �� �����ϰ� ���� �Ѵ�.
    ���� ���� ���̶� ��޿� ���� ������ ��� �Ѵٸ� �̰��� �߰��� �ָ� �ȴ�.

 */
struct SPVEInDunID
{
	SPVEInDunID();

	void	Init();
	bool	IsVaild() const;

    DWORD   dwCharDbNum;    //@ ��Ƽ�� ĳ���� ���� DB ID
    DWORD   dwMapID;        //@ ������ �δ� �� ID, (�������� �ƴԿ� ����)
    int     nLevel;         //@ ������ �δ� ���(���̵�)
	int		nNum;			//@ �δ� ���� ī��Ʈ(������ ������ ���� ��� �ȴ�.)

    /**
        ���� �� �˻� �������ν��� ������ ����
        �츮�� ĳ���� ���� DB ID �� �� ID �� ���� �Ѵ�.
        ���� �̰͵��� �����ϴٸ� ������ �δ����� ���� �Ѵ�.

     */
    bool    operator < (const SPVEInDunID& rhs) const;

	// ���� �� ������
	bool    operator == (const SPVEInDunID& rhs) const;
	bool    operator != (const SPVEInDunID& rhs) const;

    MSGPACK_DEFINE(dwCharDbNum, dwMapID, nLevel, nNum);
};

#endif
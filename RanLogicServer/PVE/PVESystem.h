#ifndef ServerPVESystem_H
#define ServerPVESystem_H

#include "../../RanLogic/PVE/MapTable.h"
#include "../../RanLogic/PVE/PVEInDunID.h"
#include "../../RanLogic/Party/GLPartyDefine.h"
#include "../TriggerSystem/ServerMapInfoMessage.h"

struct GLPartyAgent;

namespace PVESystem
{
    class CPVEInDun;

	/**
		PVE DB �� �ʱ�ȭ ��Ų��.

		DB �� �ʱ�ȭ, �ε� ��Ų��.
		DB �� ����ϴ� ������ ���ؼ�(���� ������Ʈ) ȣ�� �Ѵ�.

	 */
	bool			        InitPVEDB();

	/**
		PVE DB �� �ı� �Ѵ�.
		���������� DB �� ���Ϸ� ������ �� �ڷᱸ���� ���� �Ѵ�.

	 */
	void			        DestroyPVEDB();

    /**
    	�̸� ��ũ��Ʈ�� ���� �δ� ������ ĳ�� �� �д�.
        ��� ������ DB�� ������� �ʰ� ���� ĳ���� ����� ���� �ִ�.
        �׷��� ���� �д�.

     */
    void                    InitPVECache();

    /**
    	�δ� ������ ĳ���� ���� �Ѵ�.
        ������ �����ϸ� �ٽ� �о�� �ϹǷ� ���� �Ѵ�.

     */
    void                    DestroyPVECache();
	void					DestroyPVEIndun();

    /**
    	�ԷµǴ� ���̺� ���� �ð� Ÿ�ӽ������� �Է��� �ش�.
    	�Է� ���� �� �ð� �Է��� �Ѱ����� ������ ����.

     */
    void                    SetCurTime(MAP_TABLE& tbl);

    /**
    	���� �ð����� �Էµ� Ÿ�� ���������� ���̸� �Էµ� ���� �ʿ����� ���� �ʸ� ��� �Ѵ�.

    	\param timestamp ���� Ÿ�ӽ������� ���̸� ����� Ÿ�� ��������
    	\param sec ���� ��, �̸� �������� ���� ���̸� ��� �Ѵ�.
    	\return ���� �ʸ� �������� ���� ��
     */
    _TIME_TYPE              DiffTime(_TIME_TYPE timestamp, int sec);

	/**
		�� ���� �� ã�´�.

		\param info ã�� �� ID �� ã�� �� ����
			        info.mapid �� �̸� �Է� �Ǿ�� �� ������ ã�� �� �ִ�.
		\return ���� ����
	 */
    bool			        GetMapInfo(TriggerSystem::SServerTriggerMapInfo& info);

	/**
		�� DB �� ���ڵ� �� ã�´�.
		���� �ش� ���ڵ尡 ���ٸ� ���ڵ带 ���� �Ѵ�.

		\param tbl ã�� �� ���� �� ã�� �� ���ڵ� ����
			       tbl.dwCharNum �� tbl.dwMapID �� �̸� �Է� �Ǿ�� �� ������ ã�� �� �ִ�.
		\return ���� ����
	 */
	bool			        GetMapTable(MAP_TABLE& tbl);
    bool			        GetMapTable( MAP_TABLE& tbl OUT, const DWORD& dwChaNum IN, const DWORD& dwMapID IN, const int& nLevel IN );
	const MAP_TABLE	        GetMapTable(const SPVEInDunID& id);
	//bool					GetMapTable(MAP_TABLE& tbl, DWORD dwCharDbNum, DWORD dwMapID, int nLevel);	

	/**
		�� DB �� ���ڵ带 ���� �Ѵ�.
		�ܺο����� strID �� ������ �ʿ䰡 ����.

		\param sMapTbl ������ ���̺� ���ڵ�
	 */
	bool			        SetMapTable(const MAP_TABLE& tbl);

    /**
        �ش� ID �� ������ ĳ������ �����̺��� ��� ������.
        Ŭ���̾�Ʈ�� ������ �����ӿ� ���� �Ѵ�.

    	\param dwChaNum ĳ���� DB ID
        \param con ��� ���� �����̳�
		\param tbl ���� �ڽ��� ���� �δ� ����
    	\return ���� ����
     */
    bool                    CollectMapTable(DWORD dwChaNum, MapTableContainer& con, MAP_TABLE& tbl);

    /** 
    	�δ� ���� ó���⸦ ã�´�.
        ���� ���ٸ� ���� �����Ͽ� ���� �Ѵ�.
        ��� �δ��� �̸� ���� ���� �ȴ�.

     */
    CPVEInDun*              FindInstantDungeon(const SPVEInDunID& sInDunID);

    /**
        �δ� ������� ����, ���� �Ѵ�.

     */
    void                    InsertInDunMember(const DWORD dwChaDbNum, const SPVEInDunID& sInDunID);
    void                    RemoveInDunMember(const DWORD dwChaDbNum);

    /** 
    	�Էµ� DB ĳ���Ͱ� ���� �Ǿ��ִ� �δ� ���� ó���⸦ ã�´�.
		�̰��� �����ϸ� �δ��� �Ҽӵ� ĳ���� ������ Ȯ�� �� �� �ִ�.
		�ܺο��� PVE �δ��� �Ҽӵ� ���������� �˰� ������ Ȱ���� �� �ִ�.

     */
    CPVEInDun*              FindInstantDungeon(const DWORD dwChaDbNum);

    /**
    	PVE �ý����� ���� ��Ų��.
        
    	\param fElpaseTime ���� �����ӿ��� ���� ������ ���� �ð�(1 = 1��)
     */
    void                    FrameMove(float fElpaseTime);

} // end namespace

#endif

#pragma once

#include "../s_NetGlobal.h"
#include "../../RanLogic/PVE/MapTable.h"
#include "../../RanLogic/PVE/PVEInDunID.h"
#include "../../SigmaCore/String/MinGuid.h"

class DxMsgServer;

/**
	PVE ���� �޽������� ���� �Ѵ�.
	PVE �� ���� ������ ��� 1���� �ִ� ��Ȳ�̴�.
	
    ū Ʋ�� ���ش� �Ʒ� ��Ŷ �÷ο� ��Ű�� ���� ���������� �帧�� �����غ��� ������ �� �������� �����Ѵ�.
    �� �� �������� �κ��� ���� �ڵ带 ���� �����ϸ� �ȴ�.
	��Ŷ �÷ο�� http://minwiki.mincoms.co.kr/index.php/PVE_%EB%AC%B4%ED%95%9C%EC%9D%98_%EA%B3%84%EB%8B%A8 ���� �Ѵ�.

 */
namespace GLMSG
{
    /**
        1. �δ� ���� ó��

        NET_PVE_MAPTABLE_REQ_CA : PVE �� ���̺� ��û
        NET_PVE_MAPTABLE_ACK_AC : PVE �� ���̺� ����
     */

    struct NET_PVE_MAPTABLE_REQ_CA
    {
        DWORD dwGaeaID;

        MSGPACK_DEFINE(dwGaeaID);
        NET_PVE_MAPTABLE_REQ_CA() : dwGaeaID(0) {}
    };

    struct NET_PVE_MAPTABLE_ACK_AC
    {
        MapTableContainer tables;			//@ �ڽ��� ����ų� �ߴ� �δ� ������
		MAP_TABLE table;					//@ ���� �ڽ��� ���� �δ� ����

		MSGPACK_DEFINE(tables, table);
    };

    /**
        2. �δ� ���� ó��

        NET_PVE_ENTRANCE_REQ_CA : ��û
        NET_PVE_ENTRANCE_ACK_AC : ����
     */

	enum ENTRANCE_ACK
	{
		ENTRANCE_ACK_NONE,					//@ ���� OK
		ENTRANCE_ACK_NOT_ENTRANCE,   		//@ ��Ƽ ���� ���ǿ� ���� �ʽ��ϴ�.
	};

    struct NET_PVE_ENTRANCE_REQ_CA
    {
        SPVEInDunID sInDunID;				//@ ���� �δ� ID
        DWORD dwGateID;						//@ ���� ����Ʈ

		MSGPACK_DEFINE(sInDunID, dwGateID);
        NET_PVE_ENTRANCE_REQ_CA() : dwGateID(0) {}
    };

    struct NET_PVE_ENTRANCE_ACK_AC
    {
        SPVEInDunID sInDunID;
		BYTE byResult;		

		NET_PVE_ENTRANCE_ACK_AC() : byResult(ENTRANCE_ACK_NONE) {}
        MSGPACK_DEFINE(sInDunID, byResult);
    };

    /**
        3. �δ� ���� Ȯ�� ó��

        NET_PVE_ENTRANCE_CONFIRM_REQ_CA : ��û
        NET_PVE_ENTRANCE_CONFIRM_ACK_AC : ����
     */

    enum ENTRANCE_CONFIRM_ACK
    {
        ENTRANCE_ACK_CONFIRM_NONE,        
        ENTRANCE_ACK_CONFIRM_TIMEOVER,      //@ ���� ��� �ð� ������ ���� ����
        ENTRANCE_ACK_CONFIRM_CANCEL,        //@ ��Ƽ�� ���� ��ҷ� ���� ����        
        ENTRANCE_ACK_CONFIRM_OK,            //@ ��Ƽ�� ����
        ENTRANCE_ACK_CONFIRM_IMPOSSIBLE,    //! ���� �Ұ��� ����
    };

    enum ENTRANCE_CONFIRM_REQ
    {
        ENTRANCE_CONFIRM_REQ_FALSE = 0,     //! ���� ���
        ENTRANCE_CONFIRM_REQ_TRUE,          //! ����
        ENTRANCE_CONFIRM_REQ_IMPOSSIBLE     //! ���� �Ұ�
    };
    struct NET_PVE_ENTRANCE_CONFIRM_REQ
    {
        DWORD       dwChaDbNum;
        BYTE        byOk;                    //@ ���� Ȯ�� ��� (true = ����, false = ���� ���)
        SPVEInDunID sInDunID;

        MSGPACK_DEFINE(dwChaDbNum, byOk, sInDunID);
        NET_PVE_ENTRANCE_CONFIRM_REQ() : byOk(false) {}
    };

    struct NET_PVE_ENTRANCE_CONFIRM_ACK_AC
    {
        DWORD dwChaDbNum;
        BYTE byResult;                      //@ ���� Ȯ�� ����� ���� ó��(ENTRANCE_CONFIRM_ACK ���� ������)

        NET_PVE_ENTRANCE_CONFIRM_ACK_AC() : byResult(ENTRANCE_ACK_CONFIRM_NONE) {}
        MSGPACK_DEFINE(byResult, dwChaDbNum);
    };

    /**
        4. �δ� �ͼ� ó��

        NET_PVE_CLEAROFBELONGING_REQ_CA : ���� ��û
		NET_PVE_CLEAROFBELONGING_ACK_AC : ���� ���

     */

    struct NET_PVE_CLEAROFBELONGING_REQ_CA
    {
        SPVEInDunID sInDunID;

		MSGPACK_DEFINE(sInDunID);
    };

	struct NET_PVE_CLEAROFBELONGING_ACK_AC
	{
		SPVEInDunID sInDunID;

		MSGPACK_DEFINE(sInDunID);
	};


    /**
        5. �δ� ���� �� ���� ó��

        NET_PVE_CREATE_INSTANT_MAP_REQ_AF : �δ� ���� ��û
        NET_PVE_CREATE_INSTANT_MAP_ACK_FA : �δ� ���� ��û ��� ����
        NET_PVE_ENTRANCE_INSTANT_MAP_REQ_AF : �δ� ���� ��û
        NET_PVE_ENTRANCE_INSTANT_MAP_ACK_FA : �δ� ���� ��û ��� ����
     */

    struct NET_PVE_CREATE_INSTANT_MAP_REQ_AF
    {
        int         nStage;                 //@ ���� ��������
        SPVEInDunID sInDunID;               //@ ���� �δ� ���� ������
        DWORD	    dwBaseMapID;            //@ ���� ���� �� �ν��Ͻ� ���� �⺻ ID
        DWORD	    dwInstanceMapID;        //@ ���� ���� �� �ν��Ͻ� �� ID
		GLPartyID	sPartyID;				//@ �δ� ������ ��ƼID, �δ��� �����Ϸ��� �ʿ�
        std::string strGuid;                //! �α׸� ��������� �δ� ����ID

        MSGPACK_DEFINE(nStage, sInDunID, dwBaseMapID, dwInstanceMapID, sPartyID, strGuid);
        NET_PVE_CREATE_INSTANT_MAP_REQ_AF() : nStage(0), dwBaseMapID(0), dwInstanceMapID(0){}
    };

    struct NET_PVE_CREATE_INSTANT_MAP_ACK_FA
    {
        bool        bResult;                //@ ���� ��û ���
		DWORD	    dwInstanceMapID;        //@ ���� �� �ν��Ͻ� �� ID
        SPVEInDunID sInDunID;               //@ ���� �δ� ���� ������

        MSGPACK_DEFINE(bResult, dwInstanceMapID, sInDunID);
        NET_PVE_CREATE_INSTANT_MAP_ACK_FA() : bResult(false), dwInstanceMapID(0) {}
    };

    struct NET_PVE_ENTRANCE_INSTANT_MAP_REQ_AF
    {
        DWORD	    dwCharDbNum;            //@ ������ ĳ������ Db ID
        DWORD	    dwMapID;                //@ ������ �δ� �� ID(�δ� ���� ID �� ��ID���� �ٸ��� ����)
        DWORD	    dwGateID;               //@ ������ ����Ʈ ID
        SPVEInDunID sInDunID;               //@ �δ� ���� ID, ��������� ��� �Ѵ�.

        MSGPACK_DEFINE(dwCharDbNum, dwMapID, dwGateID, sInDunID);
        NET_PVE_ENTRANCE_INSTANT_MAP_REQ_AF() : dwCharDbNum(0), dwMapID(0), dwGateID(0) {}
    };

    struct NET_PVE_ENTRANCE_INSTANT_MAP_ACK_FA
    {
        bool        bResult;                //@ ���� ��û ���
        DWORD	    dwCharDbNum;            //@ ���� �� ĳ������ Db ID
        SPVEInDunID sInDunID;               //@ ���� �δ� ���� ������

        MSGPACK_DEFINE(bResult, dwCharDbNum, sInDunID);
        NET_PVE_ENTRANCE_INSTANT_MAP_ACK_FA() : bResult(false), dwCharDbNum(0) {}
    };

    /**
        6. �δ� ���� ���� ó��

        NET_PVE_ENTRANCE_STATE_FC : ���� ���� ����

     */

    struct NET_PVE_ENTRANCE_STATE_FC
    {
        int nCur;                           //@ ���� ���� �ο�
        int nAll;                           //@ �� ���� �ο�
        int nTime;                          //@ ���� ��� ���� �ð�(sec)
		int nStage;							//@ ���� ��������

        MSGPACK_DEFINE(nCur, nAll, nTime, nStage);
        NET_PVE_ENTRANCE_STATE_FC() : nCur(0), nAll(0), nTime(0), nStage(0) {}
    };

    /**
        7. �δ� �������� ���� ó��

        NET_PVE_START_STAGE_FC : �������� ���� ����
        NET_PVE_MSG_START_STAGE_FC : �������� ���� �޽��� ����
        NET_PVE_MSG_FC : ���������� ��� �޽��� ����
        NET_PVE_MONSTERCLEAR_STATE_FC : ���� ���� ����
        NET_PVE_POSITIONINGCLEAR_STATE_FC : ��ġ ��� ���� ����
        NET_PVE_STAGE_STATE_FC : �������� ���� ����

     */

    enum
    {
        STAGE_STATE_ONGOING = 0,            //@ ������
        STAGE_STATE_TIMEOVER,               //@ �������� Ÿ�� ����
        STAGE_STATE_STAGECLEAR,             //@ �������� Ŭ����
        STAGE_STATE_RETRY,                  //@ �絵�� ����
        STAGE_STATE_ALLDIED,                //@ ���� ����
        STAGE_STATE_DIED,                   //@ �ڽ� ����

        STAGE_STATE_GAMEROVER_FIREPARTY,    //@ ���� ���� : ��Ƽ ����
        STAGE_STATE_GAMEROVER_TIMEOVER,     //@ ���� ���� : �δ� �ð� ����
        STAGE_STATE_GAMEROVER_NOTENOUGH,    //@ ���� ���� : �絵�� ���� - �� ����
        STAGE_STATE_GAMEROVER_WAITTIME,     //@ ���� ���� : �絵�� ���� - ��� �ð� ����
        STAGE_STATE_GAMEROVER_NOTTRY,       //@ ���� ���� : �絵�� ���� - ��Ƽ�� �� ����
        STAGE_STATE_GAMEROVER_COMPLETED,    //@ ���� ���� : ��� �������� �Ϸ�
		STAGE_STATE_GAMEROVER_FAILED,		//@ ���� ���� : ��Ÿ
        STAGE_STATE_GAMEROVER_RETRY,		//@ ���� ���� : ��Ÿ
    };

    struct NET_PVE_START_STAGE_FC
    {
        int         nStage;                 //@ ���� ��������
        SPVEInDunID sInDunID;               //@ ���� �δ� ���� ������
        DWORD       dwCharDbNum;            //@ �������� ���� ��û

        MSGPACK_DEFINE(nStage, sInDunID, dwCharDbNum);
        NET_PVE_START_STAGE_FC() : nStage(0), dwCharDbNum(0) {}
    };

    struct NET_PVE_MSG_START_STAGE_FC
    {
        int nStage;                         //@ ���� ���� ��������
        int	nTime;                          //@ �޽��� ��� �ð�

        MSGPACK_DEFINE(nStage, nTime);
        NET_PVE_MSG_START_STAGE_FC() : nStage(0), nTime(0) {}
    };

    struct NET_PVE_MSG_FC
    {
        std::string strMsg;                 //@ �޽��� ����(XML Ű����)
        int	nTime;                          //@ �޽��� ��� �ð�

        MSGPACK_DEFINE(strMsg, nTime);
        NET_PVE_MSG_FC() : nTime(0) {}
    };

    struct NET_PVE_MONSTERCLEAR_STATE_FC
    {
        int nCur;                           //@ ���� ���� ���� ��
        int nAll;                           //@ �� ��ƾ� �ϴ� ���� ��

        MSGPACK_DEFINE(nCur, nAll);
        NET_PVE_MONSTERCLEAR_STATE_FC() : nCur(0), nAll(0) {}
    };

    struct NET_PVE_POSITIONINGCLEAR_STATE_FC
    {
        int nX;                             //@ �̵� �ؾ� �� ��ġ X
        int nY;                             //@ �̵� �ؾ� �� ��ġ Y
        int nNow;                           //@ ���� �Ϸ��� ���
        int nNeed;                          //@ �̼� �Ϸῡ �ʿ��� ���
		bool bCompleted;					//@ �̼� �Ϸ� ����

        MSGPACK_DEFINE(nX, nY, nNow, nNeed, bCompleted);
        NET_PVE_POSITIONINGCLEAR_STATE_FC() : nX(0), nY(0), nNow(0), nNeed(0), bCompleted(false) {}
    };

    struct NET_PVE_STAGE_STATE_FC
    {
        int nStage;                         //@ ���� ��������
        int nTime;                          //@ ���� �ð�
        int nState;                         //@ �������� ���� ����

        MSGPACK_DEFINE(nStage, nTime, nState);
        NET_PVE_STAGE_STATE_FC() : nStage(0), nTime(0), nState(STAGE_STATE_ONGOING) {}
    };

    /**
        8. �δ� �絵�� ��û
         - ��� ����ϰų� �ð��� �����Ǿ����� �絵�� �� �� �ִ�.

         NET_PVE_RETRY_REQ_CF: �絵�� ��û
		 NET_PVE_RETRY_ACK_FC: �絵�� ��� ����
         NET_PVE_RETRY_REQ_FA: �絵�� ��û
     */

	enum PVE_RETRY_RESULT_CODE
	{
		RESULT_CODE_CANCEL,					//@ ��Ƽ���� ��ҿ� ���� ����
		RESULT_CODE_NOT_ENOUGH,				//@ �ڽ��� �Ӵ� ������ ���� ����
		RESULT_CODE_ANOTHER_NOT_ENOUGH,		//@ Ÿ ��Ƽ�� �Ӵ� ������ ���� ����
		RESULT_CODE_TIMEOVER,				//@ �絵�� ��� �ð� ���ῡ ���� ����
        RESULT_CODE_OK,				        //! ����
	};

    struct NET_PVE_RETRY_REQ_CF
    {
        bool bOk;							//@ true : �絵�� ��, No : �絵�� ����

        MSGPACK_DEFINE(bOk);
        NET_PVE_RETRY_REQ_CF() : bOk(false) {}
    };

	struct NET_PVE_RETRY_ACK_FC
	{
        DWORD dwChaDbNum;
		int nResultCode;

		MSGPACK_DEFINE(dwChaDbNum, nResultCode);
		NET_PVE_RETRY_ACK_FC() : nResultCode(RESULT_CODE_CANCEL) {}
	};

    struct NET_PVE_RETRY_REQ_FA
    {
        SPVEInDunID sInDunID;               //@ �δ� ���� ������

        MSGPACK_DEFINE(sInDunID);
    };

    /**
        9. �δ� ��Ż �� �̵� ó��
         - ���� ��Ż�δ� ���ϴ� ��� �ϱ� ����� ���� ��Ż ��Ŷ�� ����
         - ������ ���� ���� ���� ���¿� �¾ƾ� ��Ż�� ��Ų��.

         NET_PVE_PORTAL_STATE_FC : ��Ż ����(Ȱ��, ��Ȱ��) ���� ��û
         NET_PVE_PORTAL_REQ_CF : ��Ż �̵� ��û
         NET_PVE_MOVE_REQ_FA : ���� �������� �δ����� �̵� ��û
         NET_PVE_RETURN_REQ_AF : ��ȯ ��û
         NET_PVE_RETURN_REQ_FA : ��ȯ ��û
     */

    struct NET_PVE_PORTAL_STATE_FC
    {
        bool  bOpen;                        //@ ����Ʈ Ȱ��, ��Ȱ�� ����
        DWORD dwGateID;                     //@ ����Ʈ ID
        DWORD dwToGateID;                   //@ �̵��� ����Ʈ ID
        DWORD dwToMapID;                    //@ �̵��� �� ID

        MSGPACK_DEFINE(bOpen, dwGateID, dwToGateID, dwToMapID);
        NET_PVE_PORTAL_STATE_FC() : bOpen(false), dwGateID(0), dwToGateID(0), dwToMapID(0) {}
    };

    struct NET_PVE_PORTAL_REQ_CF
    {
        DWORD dwFromMapID;                  //@ ��û �� ID
        DWORD dwFromGateID;                 //@ ��û ����Ʈ ID
        DWORD dwToGateID;                   //@ �̵��� ����Ʈ ID
        DWORD dwToMapID;                    //@ �̵��� �� ID

        MSGPACK_DEFINE(dwFromMapID, dwFromGateID, dwToGateID, dwToMapID);
        NET_PVE_PORTAL_REQ_CF() : dwFromMapID(0), dwFromGateID(0), dwToGateID(0), dwToMapID(0) {}
    };

    struct NET_PVE_MOVE_REQ_FA
    {
        SPVEInDunID sInDunID;               //@ ���� �δ� ���� ������        
        DWORD       dwToGateID;             //@ �̵��� ����Ʈ ID
        DWORD       dwToMapID;              //@ �̵��� �� ID
        int         nStage;                 //@ �̵� �� ����� �δ� ��������
		bool		bIsItem;				//@ ������ �������
		WORD		wPosX;					//@ �κ��丮 X��ǥ
		WORD		wPosY;					//@ �κ��丮 y��ǥ
		DWORD		dwCharDbNum;			//Q �ɸ��� DB NUM

        MSGPACK_DEFINE(sInDunID, dwToGateID, dwToMapID, nStage, bIsItem, wPosX, wPosY, dwCharDbNum);
        NET_PVE_MOVE_REQ_FA() : dwToGateID(0), dwToMapID(0), nStage(0), bIsItem(false), wPosX(0), wPosY(0), dwCharDbNum(0) {}
    };

    struct NET_PVE_RETURN_REQ_AF
    {
        DWORD dwCharDbNum;                  //@ ��ȯ ��ų ĳ���� DB Num
        int nWhy;                           //@ ���� ����

        MSGPACK_DEFINE(dwCharDbNum, nWhy);
        NET_PVE_RETURN_REQ_AF() : dwCharDbNum(0), nWhy(0) {}
    };

    struct NET_PVE_RETURN_REQ_FA
    {
        SPVEInDunID sInDunID;               //@ �δ� ���� ������
        int nWhy;                           //@ ���� ����

        MSGPACK_DEFINE(sInDunID, nWhy);
        NET_PVE_RETURN_REQ_FA() : nWhy(0) {}
    };

    /**
        10. �δ� �������� ���(����) ó��

        NET_PVE_STAGE_RESULT_FC : �������� ���� ����(��� Ŭ���� or ����)�� Ŭ�� �δ� �������� ���(����) ���� ����
        NET_PVE_STAGE_RESULT_FA_OR_AF : ��� �����ؼ� ���������� ó���ؾ� �ϹǷ� �����̸� ���� �ʿ� ���� �������� ��� ����

     */

    struct CharResult
    {
        DWORD       charDbNum;              //@ ĳ���� DB ID
        std::string chaName;                //@ ĳ����
        DWORD       dwClass;                //@ ĳ���� Ŭ����
        DWORD       dwDamage;               //@ ���ط�
        DWORD       dwHeal;                 //@ ġ����
        bool        bExp;                   //@ ����ġ ���� ����
        bool        bMoney;                 //@ ���� ���� ����
        bool        bMVP;                   //@ MVP ����

        MSGPACK_DEFINE(charDbNum, chaName, dwClass, dwDamage, dwHeal, bExp, bMoney, bMVP);
    };

    typedef std::vector<CharResult> StageResult;
    typedef StageResult::iterator StageResultIt;
    typedef StageResult::const_iterator StageResultCIt;

    struct NET_PVE_STAGE_RESULT_FC
    {
        StageResult results;                //@ �������� ���
        int         nTimer;                 //@ �������� ����� ������ �ð�
        int         nStage;                 //@ ���� ��������
        bool        bLast;                  //@ ������ ������������ ����

        MSGPACK_DEFINE(results, nTimer, nStage, bLast);
        NET_PVE_STAGE_RESULT_FC() : nTimer(0), nStage(0), bLast(false) {}
    };

    struct NET_PVE_STAGE_RESULT_FA_OR_AF
    {
        StageResult results;                //@ �������� ���
        SPVEInDunID sInDunID;               //@ �δ� ���� ������
        SNATIVEID   sNativeInDunID;         //@ ���� ������ �δ� ID

        MSGPACK_DEFINE(results, sInDunID, sNativeInDunID);
        NET_PVE_STAGE_RESULT_FA_OR_AF() {}
    };

    /**
        11. �δ� ������ ó��
		 - ���������� ����(���� or ����) �Ǹ� ������ ��ư�� ���� ���� �� �ִ�.
        NET_PVE_EXIT_REQ_CF : �δ� ������ ��û
        
     */

    struct NET_PVE_EXIT_REQ_CF
    {
        DWORD dwCharDbNum;                  //@ ������ ��û�� Char		
        SPVEInDunID sInDunID;               //@ �δ� ���� ������

        MSGPACK_DEFINE(dwCharDbNum, sInDunID);
        NET_PVE_EXIT_REQ_CF() : dwCharDbNum(0) {}
    };

	/**
        12. ��Ÿ.
        NET_PVE_SAVEPOS_REQ_AF : ���� ��ġ ���� ��û, ���� �� ���� ��ġ �����
        NET_PVE_REMOVE_INDUN_AC : �δ� �Ҹ� �˸�, �δ��� �Ҹ�Ǿ��� ��� �˷��ش�.
		NET_PVE_REBIRTH_FC : ���ڸ����� �ε� ���� ��Ȱ�� ��û �Ѵ�.
		NET_PVE_GM_RELOAD_TRIGGER : ������ Ʈ���Ÿ� ��ε� �ϵ��� ��û �Ѵ�.(��ȹ�� ���� ���ǿ�)
     */

    struct NET_PVE_SAVEPOS_REQ_AF
    {
        DWORD dwCharDbNum;                  //@ ��ġ ���� ��û�� Char

        MSGPACK_DEFINE(dwCharDbNum);
        NET_PVE_SAVEPOS_REQ_AF() : dwCharDbNum(0) {}
    };

	struct NET_PVE_REMOVE_INDUN_AC
	{
		SPVEInDunID sInDunID;               //@ �δ� ���� ������

		MSGPACK_DEFINE(sInDunID);
	};

	struct NET_PVE_REBIRTH_FC
	{
		DWORD dwGaeaID;						//@ ��Ȱ�� ĳ����

		MSGPACK_DEFINE(dwGaeaID);
		NET_PVE_REBIRTH_FC() : dwGaeaID(0) {}
	};

	/**
		13. GM ��ɾ�
		
		// ���ε� GM ��ɾ�
		NET_PVE_GM_RELOAD_TRIGGER_REQ_CA_OR_AF
		NET_PVE_GM_RELOAD_TRIGGER_ACK_AC
		NET_PVE_GM_RELOAD_TRIGGER_ACK_FA

		// Ư�� �������� ���� ��ɾ�
		NET_PVE_GM_GOSTAGE_CA_OR_AF
	 */

	struct NET_PVE_GM_RELOAD_TRIGGER_REQ_CA_OR_AF
	{
		DWORD dwCharDbNum;					//@ ���ε��� ��û�� ĳ���� DB

		MSGPACK_DEFINE(dwCharDbNum);
		NET_PVE_GM_RELOAD_TRIGGER_REQ_CA_OR_AF() : dwCharDbNum(0) {}
	};

	struct NET_PVE_GM_RELOAD_TRIGGER_ACK_AC
	{
		bool bResult;						//@ ���ε� ���� ����
		bool bAgent;						//@ ���ε� �� ����(true : ������Ʈ, false : �ʵ�)
		DWORD dwFieldSvr;					//@ ���ε� �� ������ �ʵ�� ��� �ʵ� ����.

		MSGPACK_DEFINE(bResult, bAgent, dwFieldSvr);
		NET_PVE_GM_RELOAD_TRIGGER_ACK_AC() : bResult(false), bAgent(true), dwFieldSvr(0) {}
	};

	struct NET_PVE_GM_RELOAD_TRIGGER_ACK_FA
	{
		bool bResult;						//@ ���ε� ���� ����
		DWORD dwFieldSvr;					//@ ���ε��� �ʵ� ���� �ѹ�
		DWORD dwCharDbNum;					//@ ���ε��� ��û�� ĳ���� DB

		MSGPACK_DEFINE(bResult, dwFieldSvr, dwCharDbNum);
		NET_PVE_GM_RELOAD_TRIGGER_ACK_FA() : bResult(false), dwFieldSvr(0), dwCharDbNum(0) {}
	};

	struct NET_PVE_GM_GOSTAGE_CF
	{
		DWORD dwCharDbNum;					//@ ���ε��� ��û�� ĳ���� DB
		int nStage;							//@ �̵��� Ư�� ��������
		bool bIsItem;

		MSGPACK_DEFINE(dwCharDbNum, nStage, bIsItem);
		NET_PVE_GM_GOSTAGE_CF() : dwCharDbNum(0), nStage(0), bIsItem(false) {}
	};

} // end namespace
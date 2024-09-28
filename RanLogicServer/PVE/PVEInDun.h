#ifndef CPVEInDun_H
#define CPVEInDun_H

#include <map>
#include "PVEInDunParty.h"
#include "PVETimer.h"

#include "../../SigmaCore/String/MinGuid.h"

typedef TriggerSystem::SServerTriggerMapInfo MAP_INFO;

namespace PVESystem
{

/**
	PVE �ͼ� �δ��� ���� �Ѵ�.
	
	PVE �δ� ������ �δ� �ý����� ���� �Ͽ���.
	�� �δ� �ý����� Ȱ���� PVE �δ��� ���������
	PVE �δ��� ���� �ʰ��� �ٸ� ������ ������ �ӿ� �����ؾ� �Ѵ�.

	�� PVE �δ��� 3�� ���������� �ϴ��� 3�� ���������� ��(�δ�)�� ���������� ���������� �ʴ´�.
	�������� ������ �ʵ� ������ �δ� �ý��ۿ� ���� ���� �Ǿ� ����.

 */
class CPVEInDun
{
public:
    CPVEInDun();
	~CPVEInDun();

    enum EIndunState
    {
        INDUNSTATE_NONE,        //@ �ƹ��� ���� �ƴ�, �ʱⰪ
        INDUNSTATE_CONFIRM,     //@ �δ� ���� Ȯ�� ����, ��� ��Ƽ���� OK �ؾ� �δ� ���� ó��
        INDUNSTATE_CREATING,    //@ �δ� ���� ��, �ʵ带 ���ؾ� �ϹǷ� �ð��� �ҿ��
        INDUNSTATE_CREATED,     //@ �δ� ���� �Ϸ� ����, �������� ���� �� ����
    };

    /**
        �δ� ���� �����ڸ� �����Ѵ�.
        ���� �������ٵ� �ֿ켱 ���� �Ǿ�� �Ѵ�.

     */
    void                SetID(const SPVEInDunID& id) { m_id = id; }
    const SPVEInDunID&  GetID() const { return m_id; }

    /**
    	������Ʈ ������ ���� �Ѵ�.
        ������ ���� ��Ŷ�� ������.

     */
    void                SetServer(GLAgentServer* sever) { m_server = sever; }

    /**
        ��Ƽ�����Ϳ� ����� DB ������ �� ���� ������ �Է� �Ѵ�.
        �̸� �������� ���� Ȯ�� ���� ������ ó�� �Ѵ�.
        ���� ���� �����ӿ� ���� �Ѵ�.

     */
    void                SetInfo(const MAP_INFO& mapinfo) { m_mapinfo = mapinfo; }
	const MAP_INFO&		GetInfo() const { return m_mapinfo; }

    /**
        ���� ���� �ν��Ͻ� �� ������ ��� �Ѵ�.		

     */
    const DWORD         GetEntranceMapID() const { return m_indunid; }	
    const DWORD         GetEntranceGateID() const { return m_gateid; }
	const DWORD         GetPreEntranceMapID() const { return m_preid; }

	/**
		���� �������� ��ȣ�� �����´�.

	 */
	int					GetStage() { return m_stage; };

    /**
    	���� or ��� ��Ƽ������ ���� ó���� ���� �Ѵ�.
		�츮�� ���� �� ���� ���� ������ üũ �ϰ� ������ ��� ���� ��Ų��.

     */
    bool                Entrance(GLPartyAgent* pParty);
	bool                Entrance(DWORD dwCharDbNum);

	/**
		�δ� ���¸� ��� �Ѵ�.

		IsIdle : �δ� �ʱ� ����
		IsConfirming : �δ� ���� Ȯ�� ����
		IsCreating : �δ� ���� ��
		IsRunning : �δ� ���� ��
	 */	
	bool				IsIdle() const { return IsState(INDUNSTATE_NONE); }
	bool				IsConfirming() const { return IsState(INDUNSTATE_CONFIRM); }
	bool				IsCreating() const { return IsState(INDUNSTATE_CREATING); }
	bool                IsRunning() const { return IsState(INDUNSTATE_CREATED); }

    /**
    	���� Ȯ�� �� ��Ƽ ����� ó�� �Ѵ�.
        ��� ��Ƽ���� Ȯ���� �ؾ����� �Դ��� �ȴ�.

     */
    void                PassMember(DWORD member);

    void                Impossible( DWORD member);
    /**
    	��Ƽ�� �߿� ������ ����ϸ� �߻� �Ѵ�.
        ���� ��Ƽ���� �������̶�� ���� ��� �ð��� �����Ƿ� 
        �̸� ���� ó�� �Ѵ�.
        
     */
    void                Cancel(DWORD member);

    /**
        �ͼ� ������ ó�� �Ѵ�.
        �δ��� ó�� �����ҋ� ��Ƽ�����Ϳ� ���ӵǰ�
        �����Ͱ� �ͼ� ���� ��û�� �ϸ� ���� �δ��� �������
		��Ƽ���� �ͼ� ������ ��û�ϸ� �ش� �δ��� ������ ��� ����.

     */
    bool                ClearOfBelonging(DWORD member);

    /**
        �絵���� ó�� �Ѵ�.
        �絵���� ��� �װų� �ð� ������ �� �� �ִ�.
        ��� ��Ƽ������ �絵���� OK �ؾ� �絵�� �ȴ�.

     */
    void                Retry();

    /**
    	��� ��Ƽ������ ���信 �������� �� �δ��� ���� �Ѵ�.
        �δ��� ������Ʈ�� �ʵ忡 ���� �ִµ� �ϵ� ������Ʈ�� ���� ������ ��
        �ʵ忡 ������ ��û�� �� ��� �Ѵ�.

     */
    bool                CreateInstanceMap(DWORD dwBaseMapID, DWORD dwGateID, int nStage);
    void				DeleteInstanceMap(DWORD dwGaeaMapID);

    /**        
        �� �Լ��� �ʵ忡�� ���������� ������ ���� �Ǿ��ٴ� �޽����� ���� ȣ�� �ȴ�.
        �츮�� ������� �δ��� ���� ���� ��Ų��.

     */
    bool                PostCreateInstanceMap(DWORD dwGaeaMapID, bool bResult);

    /**
    	��Ƽ������ �ԷµǴ� ���� ����Ʈ�� ��� �̵� ��Ų��.

     */
    void                MoveParty(DWORD dwMapID, DWORD dwGateID, int nStage);

    /**        
        PostCreateInstanceMap �� ���� �ʵ忡 ���� ������ ��û�� ����� ���� ȣ�� �ȴ�.
        �츮�� ������ �Ϸ�Ǹ� �׿� ���� ���� ī��Ʈ ���� ���� ���� ó���� ���� �Ѵ�.

     */
    void                PostEntranceInstanceMap(DWORD dwCharDbNum, bool bResult);

    /**
    	�ð��� ���� ��Ų��.
        �츮�� ���� Ȯ�� �ð��� ������ �����Ƿ�
        �� �ð����� ��� �̷����� �Ѵ�.

    	\param fElpaseTime �ʴ��� ���� �ð�
     */
    void                FrameMove(float fElpaseTime);

    /**
        �������� ����� ���� �Ѵ�.
        �� ���������� Ŭ���� �ϸ� ���� �Ǿ� ����
        PVE �� ����Ǳ� ������ �����Ǿ� ���� ������ ó�� �Ѵ�.

     */
    void                SetStageResult(const StageResult& results);

    /**
        ����� ��ȯ ��Ų��.
        �츮�� ���� �Ϸ� �� �����⸦ Ŭ���ϸ� ��� �Ѵ�.
        ��� ��ȯ ��Ű�� �ʰ� �ʵ忡 ��ȯ�� �˸� �� ����� �޾� ��ȯ(Return) ��Ų��.

     */    
    void                RestoreAll(int nWhy);

    
    // ��Ƽ ���̵� ��ȯ�Ѵ�.
    const GLPartyID     GetPartyID() const { return m_party.GetPartyID(); }

protected:
    /**
    	�δ� ���¸� �ʱ�ȭ �Ѵ�.
		���������θ� ����ϵ��� �Ѵ�.

    	\param bSucceed �������� ���� ���ῡ ���� �������� ����
     */
    void                Init(bool bSucceed = false);

    /**
    	��Ƽ ���°� �ı� �Ǹ� ȣ�� �ȴ�.
		�츮�� ��Ƽ�� �ı��ɶ�(DestroyParty) �δ��� ���� ��Ų��.

     */    
    void                DestroyParty(GLPartyAgent* pPartyAgent);

    /**
        ��Ƽ �ݹ��� ����, ���� �Ѵ�.		

     */
    void                SetPartyCallback();
    void                RemovePartyCallback();

    /**
    	���� ���ɼ� ������ üũ �Ѵ�.

     */    
    bool                IsEntrance();
	bool				IsEntrance(DWORD dwCharDbNum);

    /**
        ���¸� ó�� �Ѵ�.
        �츮�� �� ���¸� ������� ���� �δ��� ���¸� �� �� �ִ�.        

     */
    void                SetState(const EIndunState state);
	bool                IsState(const EIndunState state) const { return m_state == state; }
    void                ChangeState(const EIndunState from, const EIndunState to);

    /**
    	���� ��� �ð��� �Ϸ�Ǿ��� ��� ȣ�� �ȴ�.
        ������ ������ ���� �� �߻� �� �� �ִ�.
        �츮�� ������ ��� ��Ų��.

     */
    void                Timerover();

	/**
		�δ� ������ �������� ��� ȣ�� �ȴ�.
		� ��쿡 ���� �δ� ������ �������� ���� ó�� �Ѵ�.

	 */
	void				FailedCreate();

    /**
        �δ��� ������ �Ϸ�Ǿ��� �� ȣ�� �ȴ�.
        �Ϸῡ ���� ���� ó���� ���� �Ѵ�.

     */
    void                Terminate();

	/**
		���� ���� �� DB ���� ó���� ���� �Ѵ�.
		�츮�� ��������� ���� ī��Ʈ �� ��Ÿ DB ���� ó���� ���� �Ѵ�.

	 */
	void				ProcessDB();

	/**
		���� �ð��� �ʱ�ȭ �Ѵ�.
		�δ��� �����ϸ� �ͼ� �ð����� ������ ����������
		�δ��� �����ϸ� �ͼ��� �Ҹ� �ȴ�. �ͼ��� ���� �ð����� ���� �߻� �ǹǷ�
		���� �ð��� �Ҹ� ��Ŵ���ν� �ͼ��� ��� ���� �ȴ�.

	 */
	bool				ClearGenTime();

	/**
		���忡 ���� ������ Ŭ���̾�Ʈ�� ���� �Ѵ�.	
		�ߺ� �ڵ带 ���� �Ѵ�.

	 */
	void				EntranceAck(GLMSG::ENTRANCE_ACK eAck, DWORD dwCharDbNum = 0);

	/**
		���� Ȯ�ο� ���� ������ Ŭ���̾�Ʈ�� ���� �Ѵ�.
		�ߺ� �ڵ带 ���� �Ѵ�.

	 */
	void				EntranceConfirmAck(GLMSG::ENTRANCE_CONFIRM_ACK eAck, DWORD dwChaNum=0);

	/**
		�δ��� �Ҽ� �ð��� ������ �δ��� Ŭ���� �Ѵ�.

	 */
	void				AutoClearOfBelonging();

    /**
        �ܺ� �������̽�

     */
    GLAgentServer*      m_server;       //@ ����� ������Ʈ ����

    /**
        �δ� ����

     */
    SPVEInDunID         m_id;           //@ PVE �δ��� �����ϴ� ���� ID
    MAP_INFO            m_mapinfo;      //@ PVE �δ��� ���� ��ȹ ���� ������
	DWORD				m_dwChannelNum;	//@ PVE �δ��� ������ ä�� ����

    /**
        ���� �� �� ����

     */
	DWORD				m_preid;		//@ ���� ���� �� ����(Base�� �ƴ�) �δ� �� ID
    DWORD               m_indunid;      //@ ���� ���� �� ����(Base�� �ƴ�) �δ� �� ID
    DWORD               m_gateid;       //@ ���� ���� �� �δ��� ����Ʈ ID
	DWORD				m_RealIndunid;	//@ ���� ���� �� �δ� �� ID (Lev ���Ͽ� �����ִ� ID)

    /**
        ��Ƽ ����

     */
    CPVEInDunParty      m_party;        //@ �δ� ��Ƽ ������

    /**
        �δ� ����

     */
    EIndunState         m_state;        //@ ���� �δ� ����
    int                 m_stage;        //@ ���� �δ��� �������� �ܰ�
    float               m_lifetime;     //@ �δ� �ͼ� �ð�

    /**
        ��Ÿ

     */    
    CCbTimer            m_timers;       //@ Ÿ�̸� ������

public:
    const sc::MGUID&    GetGUID() { return m_Guid; }

private:
    sc::MGUID           m_Guid;         //@ �α׸� ��������� �δ� ����ID
};

typedef std::map<SPVEInDunID, CPVEInDun> PVEInDunContainer;
typedef PVEInDunContainer::iterator PVEInDunContainerIt;
typedef PVEInDunContainer::const_iterator PVEInDunContainerCIt;

} // end namespace

#endif
#ifndef CServerRewardInstance_H
#define CServerRewardInstance_H

#include "ServerTriggerInstance.h"
#include <map>

namespace TriggerSystem
{

/**
    보상 정보
    보상 처리를 위해 필요한 요소들
	우리는 현재 맵에 해당 캐릭터가 없다면 보상을 수여하지 않음에 주의 하자.

 */
struct SRewardInfo
{
        SRewardInfo() : dwDamage(0), dwHeal(0), dwMoney(0), dwExp(0), dwMVP(0) {}

		/**
			캐릭터 정보
			보상과는 직접적인 관계는 없지만 캐릭터가 접종을 해버리면 이 정보를
			얻을 수 없는데 표시는 해줘야 하므로 가지고 있는다.

		 */
		DWORD			dwCharDbNum;//@ 캐릭터의 DB 넘버
		std::string		strCharName;//@ 캐릭터명
		DWORD			dwClass;	//@ 캐릭터의 클래스

		/**
			보상 정보
			보상과 관련된 직접적인 정보

		 */
        DWORD           dwDamage;   //@ 모든 몬스터에 데미지양
        DWORD           dwHeal;     //@ 모든 아군에게 힐양
        DWORD           dwMoney;    //@ 보상돈
        DWORD           dwExp;      //@ 보상 경험치(MVP에게만 해당)
        DWORD           dwMVP;      //@ MVP 점수(MVP가 여럿 존재할 수 있다.)
};

typedef std::map<DWORD, SRewardInfo>    CharRewardInfo;
typedef CharRewardInfo::iterator        CharRewardInfoIt;
typedef CharRewardInfo::const_iterator  CharRewardInfoCIt;

/**
	보상을 처리 한다.
    유저들의 몹 잡음 등에 의한 보상을 위한 제반 사항을
    갱신 하고 최종적으로 보상을 수행 하고 클라에 정보를 전송 한다.

    보상에서 유저를 리턴(귀환) 시키는 처리도 하고 있음에 주의 한다.
    이는 최종적으로 보상 후 귀환 시킴으로 같이 처리 하게 한다.
	
 */
class CServerRewardInstance
    : public CServerTriggerInstance
{
public:
        CServerRewardInstance();

        /**
            인스턴스가 생성될 때 호출 된다.
            시작 될때 우리는 맵의 모든 인원을 얻는다.
            이렇게 얻은 인원들에 최종 정보를 보낸다.

         */
        void		    Start();

        /**
        	보상이 모두 완료되었으므로
            우리는 해당 캐릭터들을 모두 귀환 시킨다.

         */
        void            Exit();

        /**        	
            유저 입장을 처리 한다.
            만약 어떠한 이유로 새롭게 유입한 유저가 있다면
            결과 출력을 위해 보상 정보를 만들어 준다.

         */
        void            EntryPC(SMessage* msg);

        /**
            캐릭터가 데미지를 줄 경우 호출 된다.
            우리는 데미지 량을 누적 한다.

        */
        void            Damage(SMessage* msg);

        /**
            캐릭터가 힐을 줄 경우 호출 된다.
            우리는 힐 량을 누적 한다.

        */
        void            Heal(SMessage* msg);

        /**
            귀환하기 전에
            현재 가지는 보상 요소들로 보상을 처리 한다.
            이 것은 유저들이 모두 포기하거나, 귀속 시간이 끝나거나, 
            모든 스테이지를 클리어 하면 호출 되어 진다.

         */
        void            ReturnPC(SMessage* msg);

		/**
			모든 유저들을 강제로 즉시 귀환 시킨다.
			특별한 경우에 사용 되어 진다.

		 */
		void            ReturnAllPC(SMessage* msg);

        /**
            스테이지 탈출을 요청 한다.
			우리는 모든 보상이 끝나고 유저가 나가기를 클릭하면
			현재 인던에서 탈출이 요청 된다.

         */
        void            StageExit(SMessage* msg);

        /**
            에이전트로 부터 보상 정보를 전송 한다.
            전송된 보상 정보는 이전 스테이지의 보상 정보를 의미 한다.

         */
        void            RewardAdd(SMessage* msg);

        /**
            보상을 처리 한다.
            스테이지 종료(실패 or 모든 스테이지 성공) 시 처리 된다.

         */
        void            RewardReq(SMessage* msg);

protected:
        /**
            보상을 처리 한다.

         */
        void            Reward();

        /**
            현재 보상 요소를 바탕으로 보상 결과를 계산 한다.

         */
        void            CalcReward();

        /**
            계산된 보상을 서버에 적용 한다.
            MVP 는 경험치 뿐 아니라 돈도 준다.

         */
        void            ApplyServer();

        /**
            계산된 보상을 클라이언트에 적용 한다.
            우리는 패킷을 날려 클라에 보상을 알린다.

         */
        void            ApplyClient();

        /**
            한 스테이지가 종료 되면 보상 정보가 날라가므로
            계속 이어하기 위해 에이전트에 보상 정보를 전달 한다.

         */
        void            RelayRewardInfo();

        /**
            모든 유저들을 저장된 위치로 귀환 시킨다.

			ReturnAll() : 에이전트를 경우해 귀환 시킨다.
			ReturnImmidiately() : 즉시 귀환 시킨다.
         */
        void            ReturnAll(int nWhy);
		void            ReturnImmidiatelyAll();
        void            ReturnImmidiately(DWORD dwCharDbNum);

        /**
            보상 돈과 경험치, MVP를 스크립트로 부터 계산한다.
            
         */
        const LONGLONG  RewardMoney(const DWORD dwCharDbNum);
        const int       RewardExp(const DWORD dwCharDbNum);
        const int       RewardMVP(const DWORD dwCharDbNum, const DWORD cDamage, const DWORD cHeal);

        bool            GetChar(const DWORD dwCharDbNum, GLChar*& pChar);

        CharRewardInfo  m_info;     	//@ 캐릭터 보상을 위한 요소
        float           m_timer;    	//@ 종료 처리용 타이머
        bool            m_return;   	//@ 귀환 처리 유무
        bool            m_last;     	//@ 마지막 스테이지 유무
		bool			m_completed;	//@ 스테이지를 완료 했는지 유무
		bool			m_immediately;	//@ 즉시 귀환 유무(에이전트를 통하지 않고 귀환)

        DWORD           m_dwMaxMVP;       //! MVP가 될수있는 최고 점수 ( 점수가 모두 받는다.)

        bool            m_isReword;
		
};

} // end namespace

#endif
#pragma once

#include "GLMiniGameManagerDefine.h"

/* QQQ;
 * 맵이동/접속 종료에 의헤 리셋 되어야 함;
*/

/*
 * 다소 느릴지라도 확실성을 위해 모든 행위는 요청->응답->처리순으로 이루어져야 함;
 *
 * 기획팀 요청에 의해 미니 게임 활성화시, 제어 안됨;
 * 이동에 관련 된 부분은 위험 요소가 될 수 있음;
 *  
 */

namespace MiniGame
{
	class IMiniGame;
	class GLManagerClient
	{
	public:
		void FrameMove(const float fTime, const float fElapsedTime);


		/*
		 * 일반적인 MessageProcess들과 달리네트워크 통신에 의한 메세지 뿐만 아닌;
		 * 내부간(외부 로직 -> 매니저 -> 인터페이스) 통신용으로도 사용 함;
		 * doMessage() 함수를 통해 서로간의 간섭 내지는 참조 없이;
		 * 로직을 만들어야 함;
		 */
		const bool doMessage(NET_MSG_GENERIC* const pNetMsg);


		void RequestOpen(const DWORD dwMiniGameID, const SNATIVEID& NPCID = SNATIVEID(false), const DWORD dwNPCGaeaID = GAEAID_NULL) const;
		void RequestClose(void) const;

		const IMiniGame* const GetMiniGame(void) const;
		const IMiniGame* const GetMiniGame(const EMMINIGAME_TYPE emMiniGameType) const;
		
		void Reset(void);

		// odd-even;
		void doMessageRequestBegin(void);
		void doMessageRequestSelectOddEven(const bool _bOdd);		
		void doMessageRequestRoll(void);
		void doMessageRequestResult(void);
		void doMessageRequestReward(void);
		void doMessageRequestSelectReward(const unsigned int _indexReward);	
		void doMessageRequestReceiveReward(void);
		const int getStageCurrent(void) const;
		const int getStepCurrent(void) const;
		const int getStepMaximum(void) const;
		const int getMoneyCost(void) const;
		const DWORD getItemIDReward(const unsigned int _indexItemReward) const;
		const bool isWin(void) const;
		const float getTimeLimit() const;

	private:
		// 미니 게임 객체 생성/파괴;
		// request~()함수에 의해 서버로 요청한 결과에 따라 호출 됨;
		// 직접 호출하는 일은 없도록 하자;
		const bool _doMiniGame(const DWORD dwGaeaID, const DWORD dwMiniGameID, const float fBeginTime, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID);
		const bool _endMiniGame(void);


		/*
		 * Manager Class에서는 생성/파괴에 대한 메세지 처리만 함;
		 * 나머지는 각자 구현한 미니게임의 doMessage()에서 처리하면 됨;
		 */
		const bool _messageProcedureDoOpen(const GLMSG::NET_MINIGAME_MESSAGE* const pMiniGameMessage);
		const bool _messageProcedureDoClose(const GLMSG::NET_MINIGAME_MESSAGE* const pMiniGameMessage);

	public:
		const GLManagerClient& operator =(const GLManagerClient&){}
		GLManagerClient(GLGaeaClient* const pGaeaClient);
		~GLManagerClient(void);

	private:
		GLGaeaClient* const m_pGaeaClient;

		IMiniGame* m_pMiniGame;
	};
}



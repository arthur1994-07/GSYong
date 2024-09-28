#pragma once

#include "GLMiniGameManagerDefine.h"

/* QQQ;
 * ���̵�/���� ���ῡ ���� ���� �Ǿ�� ��;
*/

/*
 * �ټ� �������� Ȯ�Ǽ��� ���� ��� ������ ��û->����->ó�������� �̷������ ��;
 *
 * ��ȹ�� ��û�� ���� �̴� ���� Ȱ��ȭ��, ���� �ȵ�;
 * �̵��� ���� �� �κ��� ���� ��Ұ� �� �� ����;
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
		 * �Ϲ����� MessageProcess��� �޸���Ʈ��ũ ��ſ� ���� �޼��� �Ӹ� �ƴ�;
		 * ���ΰ�(�ܺ� ���� -> �Ŵ��� -> �������̽�) ��ſ����ε� ��� ��;
		 * doMessage() �Լ��� ���� ���ΰ��� ���� ������ ���� ����;
		 * ������ ������ ��;
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
		// �̴� ���� ��ü ����/�ı�;
		// request~()�Լ��� ���� ������ ��û�� ����� ���� ȣ�� ��;
		// ���� ȣ���ϴ� ���� ������ ����;
		const bool _doMiniGame(const DWORD dwGaeaID, const DWORD dwMiniGameID, const float fBeginTime, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID);
		const bool _endMiniGame(void);


		/*
		 * Manager Class������ ����/�ı��� ���� �޼��� ó���� ��;
		 * �������� ���� ������ �̴ϰ����� doMessage()���� ó���ϸ� ��;
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



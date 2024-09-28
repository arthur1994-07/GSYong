#pragma once

#include "../../../RanLogic/MiniGame/OddEven/GLOddEven.h"

/* ����;
 * ��û->����->ó���� �̷������ ����,
 * ���� �߿� ��û�� �� �� �� �ִ� ���� �����ؼ� �ۼ� �� ��;*/

class GLGaeaServer;
namespace MiniGame
{
	class GLOddEvenField : public MiniGame::GLOddEven
	{
	public:
		virtual void FrameMove(const float fTime, const float fElapsedTime) override;
		virtual const bool doMessage(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD dwGaeaID) override;
		virtual const bool doBegin(void) override;
		virtual const bool doEnd(void) override;

	private:
		const bool _setDetermineNumber(void);

		const bool _doMessageRequestBegin(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD dwGaeaID);
		const bool _doMessageRequestSelectOddEven(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD dwGaeaID);
		const bool _doMessageRequestRoll(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD dwGaeaID);
		const bool _doMessageRequestResult(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD dwGaeaID);		
		const bool _doMessageRequestReward(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD dwGaeaID);
		const bool _doMessageRequestSelectReward(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD dwGaeaID);
		const bool _doMessageRequestReceiveReward(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD dwGaeaID);
		
	public:
		GLOddEvenField(GLGaeaServer* const pGaeaServer, const DWORD dwGaeaID, const DWORD dwMiniGameIndex, const float fBeginTime, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID);
		~GLOddEvenField(void);

	private:
		GLGaeaServer* const m_pGaeaServer;		
	};
}

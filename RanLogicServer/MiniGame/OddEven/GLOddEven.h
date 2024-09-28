#pragma once

#include "../../../RanLogic/MiniGame/OddEven/GLOddEven.h"

/* 주의;
 * 요청->응답->처리로 이루어지다 보니,
 * 응답 중에 요청이 또 올 수 있는 점에 주의해서 작성 할 것;*/

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

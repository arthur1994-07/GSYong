#pragma once

#include "../../../RanLogic/MiniGame/OddEven/GLOddEven.h"

class GLGaeaClient;

namespace MiniGame
{
	class GLOddEvenClient : public MiniGame::GLOddEven
	{
	public:
		virtual void FrameMove(const float fTime, const float fElapsedTime) override;
		virtual const bool doMessage(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD dwGaeaID) override;
		virtual const bool doBegin(void) override;
		virtual const bool doEnd(void) override;

		const MiniGame::EMODDEVEN getResult(void) const;

	private:
		const bool _setDetermineNumber(const DWORD* const nDetermineNumber); // call by client;

		const bool _doMessageSettings(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage);
		const bool _doMessageRequestBegin(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage);
		const bool _doMessageRequestSelectOddEven(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage);		
		const bool _doMessageRequestRoll(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage);
		const bool _doMessageRequestResult(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage);
		const bool _doMessageRequestReward(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage);
		const bool _doMessageRequestSelectReward(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage);	
		const bool _doMessageRequestReceiveReward(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage);			

		const bool _doMessageDoBegin(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage);
		const bool _doMessageDoSelectOddEven(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage);
		const bool _doMessageDoRoll(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage);
		const bool _doMessageDoResult(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage);
		const bool _doMessageDoReward(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage);
		const bool _doMessageDoSelectReward(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage);
		const bool _doMessageDoReceiveReward(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage);

	public:
		const GLOddEvenClient& operator =(const GLOddEvenClient&){}
		GLOddEvenClient(GLGaeaClient* const pGaeaClient, const DWORD dwGaeaID, const DWORD dwMiniGameIndex, const float fBeginTime, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID);
		~GLOddEvenClient(void);

	private:
		GLGaeaClient* const m_pGaeaClient;
	};
}
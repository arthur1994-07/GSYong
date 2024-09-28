#pragma once

#include "GLMiniGameManagerDefine.h"

namespace MiniGame
{
	/*
	 * 게임 머니 혹은 아이템에 대한 안정화 이유로(?);
	 * 미니 게임 활성화시 입력 제한/UI 꺼짐 등의 제약 사항이 생기므로;
	 * 1PC : 1Game을 원칙으로 함 : by 기획팀;
	*/	
	class GLManagerField
	{
	public:
		void FrameMove(const float fTime, const float fElapsedTime);
		const bool MessageProcedure(NET_MSG_GENERIC* nmg, const DWORD dwGaeaID);


		/*
		 * dwMiniGameID = RanLogic/MiniGame/IMiniGameDefine.h/EMMINIGAME_TYPE에 정의;
		 *
		 * 1. doMiniGame(const DWORD, const DWORD); 
		 *   : 조건 검사 없이 게임 생성;
		 * 2. doMiniGame(const DWORD, const DWORD, const SNATIVEID&, const DWORD);
		 *   : NPC유효성 검사 후 게임 생성;
		 *
		 * 생성에 대해서만 검사를 하므로 미니게임 내 이벤트에 한해서는 각각 처리 해야 함;
		 */
		const bool doMiniGame(const DWORD dwGaeaID, const DWORD dwMiniGameID);
		const bool doMiniGame(const DWORD dwGaeaID, const DWORD dwMiniGameID, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID);

		const bool endMiniGame(const DWORD dwGaeaID, const DWORD nErrorCode = 0);		

	private:
		const bool _doMiniGame(const DWORD dwGaeaID, const DWORD dwMiniGameIndex, const SNATIVEID& NPCID = SNATIVEID(false), const DWORD dwNPCGaeaID = GAEAID_NULL);

		const bool _messageProcedureRequestOpen(const GLMSG::NET_MINIGAME_MESSAGE* const pMiniGameMessage, const DWORD dwGaeaID);
		const bool _messageProcedureRequestClose(const GLMSG::NET_MINIGAME_MESSAGE* const pMiniGameMessage, const DWORD dwGaeaID);

	public:
		const GLManagerField& operator =(const GLManagerField&){}
		GLManagerField(GLGaeaServer* const pGaeaServer);
		~GLManagerField(void);

	private:
		GLGaeaServer* const m_pGaeaServer;

		typedef std::map<DWORD, IMiniGame*> MiniGameMap; // dwGaeaID, 미니게임 객체;
		typedef MiniGameMap::const_iterator MiniGameMapCIter;
		typedef MiniGameMap::iterator MiniGameMapIter;
		typedef MiniGameMap::value_type MiniGameMapValue;

		MiniGameMap m_MiniGameMap;
		float m_fCurrentTime;
	};
}


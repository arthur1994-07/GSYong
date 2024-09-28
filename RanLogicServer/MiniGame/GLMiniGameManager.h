#pragma once

#include "GLMiniGameManagerDefine.h"

namespace MiniGame
{
	/*
	 * ���� �Ӵ� Ȥ�� �����ۿ� ���� ����ȭ ������(?);
	 * �̴� ���� Ȱ��ȭ�� �Է� ����/UI ���� ���� ���� ������ ����Ƿ�;
	 * 1PC : 1Game�� ��Ģ���� �� : by ��ȹ��;
	*/	
	class GLManagerField
	{
	public:
		void FrameMove(const float fTime, const float fElapsedTime);
		const bool MessageProcedure(NET_MSG_GENERIC* nmg, const DWORD dwGaeaID);


		/*
		 * dwMiniGameID = RanLogic/MiniGame/IMiniGameDefine.h/EMMINIGAME_TYPE�� ����;
		 *
		 * 1. doMiniGame(const DWORD, const DWORD); 
		 *   : ���� �˻� ���� ���� ����;
		 * 2. doMiniGame(const DWORD, const DWORD, const SNATIVEID&, const DWORD);
		 *   : NPC��ȿ�� �˻� �� ���� ����;
		 *
		 * ������ ���ؼ��� �˻縦 �ϹǷ� �̴ϰ��� �� �̺�Ʈ�� ���ؼ��� ���� ó�� �ؾ� ��;
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

		typedef std::map<DWORD, IMiniGame*> MiniGameMap; // dwGaeaID, �̴ϰ��� ��ü;
		typedef MiniGameMap::const_iterator MiniGameMapCIter;
		typedef MiniGameMap::iterator MiniGameMapIter;
		typedef MiniGameMap::value_type MiniGameMapValue;

		MiniGameMap m_MiniGameMap;
		float m_fCurrentTime;
	};
}


#pragma once

#include "IMiniGameDefine.h"

namespace MiniGame
{
	class IMiniGame
	{
	public:
		virtual void FrameMove(const float fTime, const float fElapsedTime) = 0;
		virtual const bool doMessage(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD dwGaeaID = GAEAID_NULL) = 0;

		/*
		 * 게임이 성공적으로 생성 되거나 파괴 될 때;
		 * 아래 doBegin()/doEnd() 함수가 호출 됨;
		 * 일반적인 호출 순서는;
		 * server:doBegin() -> client:doBegin() -> server:doEnd() -> client:doEnd();
		 */
		virtual const bool doBegin(void) = 0;
		virtual const bool doEnd(void) = 0;		

	public:
		const IMiniGame& operator =(const IMiniGame&){}
		IMiniGame(const DWORD dwGaeaID, const DWORD dwMiniGameID, const float fBeginTime, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID);
		virtual ~IMiniGame(void);

	public:
		const DWORD m_dwMiniGameID; // 게임 ID;
		const float m_fBeginTime; // 게임 시작 시간;

		const SNATIVEID m_NPCID; // 게임을 진행중인 NPC의 NativeID(데이타 번호);
		const DWORD m_dwNPCGaeaID; // 게임을 진행중인 NPC의 GaeaID;
		const DWORD m_dwOwnerGaeaID; // 게임을 진행중인 PC의 GaeaID;
	};
}
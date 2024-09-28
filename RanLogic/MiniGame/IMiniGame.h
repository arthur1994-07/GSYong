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
		 * ������ ���������� ���� �ǰų� �ı� �� ��;
		 * �Ʒ� doBegin()/doEnd() �Լ��� ȣ�� ��;
		 * �Ϲ����� ȣ�� ������;
		 * server:doBegin() -> client:doBegin() -> server:doEnd() -> client:doEnd();
		 */
		virtual const bool doBegin(void) = 0;
		virtual const bool doEnd(void) = 0;		

	public:
		const IMiniGame& operator =(const IMiniGame&){}
		IMiniGame(const DWORD dwGaeaID, const DWORD dwMiniGameID, const float fBeginTime, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID);
		virtual ~IMiniGame(void);

	public:
		const DWORD m_dwMiniGameID; // ���� ID;
		const float m_fBeginTime; // ���� ���� �ð�;

		const SNATIVEID m_NPCID; // ������ �������� NPC�� NativeID(����Ÿ ��ȣ);
		const DWORD m_dwNPCGaeaID; // ������ �������� NPC�� GaeaID;
		const DWORD m_dwOwnerGaeaID; // ������ �������� PC�� GaeaID;
	};
}
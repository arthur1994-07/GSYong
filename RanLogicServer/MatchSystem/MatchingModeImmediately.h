#pragma once

#include "../../RanLogic/MatchSystem/IMatchingMode.h"

namespace MatchingSystem
{

	class CMatchingModeImmediately : public IMatchingMode
	{
	public:
		CMatchingModeImmediately ( MatchingInstanceBase* pMatchingInstance, sc::LuaInstance* pScript );
		virtual ~CMatchingModeImmediately ();

	public:
		// Matching Mode Callback Function;
		virtual void		CALLBACK	OnInitialize ();
		virtual void		CALLBACK	OnFrameMove ( float fElapsedTime );
		virtual bool		CALLBACK	OnEventCustomMsg ( const InstanceSystem::InstanceMapID nInstanceMapID, const double fParam1, const double fParam2, const double fParam3, const double fParam4 );
		virtual void		CALLBACK	OnMapCreate ( DWORD dwInstanceMapID, bool bDestroyComplete );
		virtual void		CALLBACK	OnMapMultiCreate ( RoomMultyReceipt* _RoomReceipt );
		virtual const bool	CALLBACK	OnDestroy ( DWORD dwInstanceMapID, bool bDestroyComplete );
		virtual const bool	CALLBACK	OnReset ();

		virtual void		CALLBACK	OnReadyGroupAnswer ( DWORD dwGroupID, bool bOK );
		virtual void		CALLBACK	OnReadyAnswer ( );

		virtual const bool	CALLBACK	OnReceiveError ( DWORD dwErrorCode );
		virtual const bool	CALLBACK	OnNoticeResult ( const InstanceSystem::InstanceMapID _instanceID, const DWORD _FactionID, const BOOL _IsWinner );

		// Matching Mode Invoke Function;
		virtual void		StartMatching ();

	public:
		void SetInfoMatchingImmediately ( SNATIVEID sMapID );

	private:
		SNATIVEID m_sMapID;						// 매칭에서 만드는 인던의 맵 ID;

		bool m_bCreateInstanceMap;

		// Script;
		// 외부 설정 데이터를 로딩하기 위해 사용한다;
		sc::LuaInstance* m_pScript;

		// Matching Instance Base;
		MatchingInstanceBase* m_pMatchingInstance;
	};

}
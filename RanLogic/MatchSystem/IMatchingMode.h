#pragma once

#include "./MatchModel.h"

namespace MatchingSystem
{

	class IMatchingMode
	{
	public:
		IMatchingMode () : m_bYield ( false ) { }
		virtual ~IMatchingMode () { }

	public:
		// Matching Mode Callback Function;
		virtual void		CALLBACK	OnInitialize () = 0;
		virtual void		CALLBACK	OnFrameMove ( float fElapsedTime ) = 0;
		virtual bool		CALLBACK	OnEventCustomMsg ( const InstanceSystem::InstanceMapID nInstanceMapID, const double fParam1, const double fParam2, const double fParam3, const double fParam4 ) = 0;
		virtual void		CALLBACK	OnMapCreate ( DWORD dwInstanceMapID, bool bDestroyComplete ) = 0;
		virtual void		CALLBACK	OnMapMultiCreate ( RoomMultyReceipt* _RoomReceipt ) = 0;
		virtual const bool	CALLBACK	OnDestroy ( DWORD dwInstanceMapID, bool bDestroyComplete ) = 0;
		virtual const bool	CALLBACK	OnReset () = 0;

		virtual void		CALLBACK	OnReadyGroupAnswer ( DWORD dwGroupID, bool bOK ) = 0;
		virtual void		CALLBACK	OnReadyAnswer ( ) = 0;

		virtual const bool	CALLBACK	OnReceiveError ( DWORD dwErrorCode ) = 0;
		virtual const bool	CALLBACK	OnNoticeResult ( const InstanceSystem::InstanceMapID _instanceMapID, const DWORD _FactionID, const BOOL _IsWinner ) = 0;

		// Matching Mode Invoke Function;
		virtual void		StartMatching () = 0;

	public:
		// Matching Mode State Management;
		inline void			MatchingModeYield ( )		{ m_bYield = true; }
		inline void			MatchingModeResume ( )		{ m_bYield = false; }
		inline const bool	GetMatchingModeState ( )	{ return m_bYield; }

	private:
		bool m_bYield;
	};

}
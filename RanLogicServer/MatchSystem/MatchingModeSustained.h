#pragma once

#include "../../RanLogic/MatchSystem/IMatchingMode.h"

namespace MatchingSystem
{

	class CMatchingModeSustained : public IMatchingMode
	{
	protected:
		struct SRATING_INFO
		{
			typedef		std::vector< DWORD >		VEC_GROUP_LIST;
			typedef		VEC_GROUP_LIST::iterator	VEC_GROUP_LIST_ITER;

			DWORD dwMaxRating;
			std::vector< DWORD > vecGroupList;

			SRATING_INFO ()
				: dwMaxRating ( 0 )
			{

			}
		};

	public:
		CMatchingModeSustained ( MatchingInstanceBase* pMatchingInstance, sc::LuaInstance* pScript );
		virtual ~CMatchingModeSustained ();

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

	private:
		// Inner Function;
		//void _CalculateRating ( DWORD dwGroupID );
		//void _MatchingGame ( );

	public:
		void SetInfoMatchingSustained ( DWORD dwMaxTeamParticipant, DWORD dwMinimumGameParticipant, SNATIVEID sMapID, bool bConfirmJoin );
		/*bool AddRatingTable ( DWORD dwMaxRating );
		bool AddRatingTableMember ( DWORD dwMaxRating, DWORD dwGroupID );*/
		
	private:
		DWORD m_dwMaxTeamParticipant;			// �� ���� �ִ� ������ ��;
		DWORD m_dwMinimumGameParticipant;		// ������ ���۵� �� �ִ� �ְ����� ���� ������ ��;
		bool m_bConfirmJoin;					// ���� Ȯ�� ����;

		SNATIVEID m_sMapID;						// ��Ī���� ����� �δ��� �� ID;

		// Rating Table;
		//std::vector< SRATING_INFO > m_vecRatingTable;

		// �����Ǿ��� �ӽ� ��;
		std::vector< DWORD > m_vecTempMatchingTeam;

		// Script;
		// �ܺ� ���� �����͸� �ε��ϱ� ���� ����Ѵ�;
		sc::LuaInstance* m_pScript;

		// Matching Instance Base;
		MatchingInstanceBase* m_pMatchingInstance;
	};

}
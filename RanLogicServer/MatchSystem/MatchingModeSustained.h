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
		DWORD m_dwMaxTeamParticipant;			// 각 팀의 최대 참가자 수;
		DWORD m_dwMinimumGameParticipant;		// 게임이 시작될 수 있는 최고한의 게임 참가자 수;
		bool m_bConfirmJoin;					// 입장 확인 여부;

		SNATIVEID m_sMapID;						// 매칭에서 만드는 인던의 맵 ID;

		// Rating Table;
		//std::vector< SRATING_INFO > m_vecRatingTable;

		// 생성되어질 임시 팀;
		std::vector< DWORD > m_vecTempMatchingTeam;

		// Script;
		// 외부 설정 데이터를 로딩하기 위해 사용한다;
		sc::LuaInstance* m_pScript;

		// Matching Instance Base;
		MatchingInstanceBase* m_pMatchingInstance;
	};

}
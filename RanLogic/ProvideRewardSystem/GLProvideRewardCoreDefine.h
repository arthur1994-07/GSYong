#pragma once

#include <vector>
#include <queue>

namespace ProvideRewardSystem
{

	// ���� ���� ����;
	class PROVIDE_REWARD_DATA
	{
	public:
		enum EMPROVIDE_TYPE
		{
			EMPROVIDE_TYPE_IMMEDIATELY,			// ��� ����;
			EMPROVIDE_TYPE_LOGIN,				// Login �� ����;
		};

		enum EMREWARD_TYPE
		{
			EMREWARD_TYPE_MONEY,				// ��;
			EMREWARD_TYPE_ITEM,					// ������;
			EMREWARD_TYPE_MONEY_AND_ITEM,		// ���� ������;
		};

	public:
		PROVIDE_REWARD_DATA ( )
			: m_dwUniqueID ( 0 )
			, m_dwChaDBNum ( 0 )
			, m_emProvideType ( EMPROVIDE_TYPE_IMMEDIATELY )
			, m_emRewardType ( EMREWARD_TYPE_MONEY )
			, m_dwRewardValue1 ( 0 )
			, m_dwRewardValue2 ( 0 )
			, m_dwRewardValue3 ( 0 )
			, m_dwRewardValue4 ( 0 )
			, m_dwTextIndex ( 0 )
		{

		}

		~PROVIDE_REWARD_DATA ( ) { }

	public:
		// ���� ���� ���� ���� ( �⺻ ������ ��ȯ );
		inline void				SetUniqueID ( DWORD _dwUniqueID )									{ m_dwUniqueID = _dwUniqueID; }
		inline void				SetCharacter ( DWORD _dwChaDBNum )									{ m_dwChaDBNum = _dwChaDBNum; }
		inline void				SetProvideType ( EMPROVIDE_TYPE _emProvideType )					{ m_emProvideType = _emProvideType; }
		inline void				SetRewardType ( EMREWARD_TYPE _emRewardType )						{ m_emRewardType = _emRewardType; }
		inline void				SetTextIndex ( DWORD _dwTextIndex )									{ m_dwTextIndex = _dwTextIndex; }


		/************************************************************************/
		/* ���� ���� ���� Value ����;											*/
		/* - Money Type															*/
		/*  -> Value1 ( Money )													*/																		
		/* - Item Type															*/
		/*  -> Value1 ( Main ID )												*/
		/*  -> Value2 ( Sub ID )												*/
		/*  -> Value3 ( Item Count )											*/
		/* - Money & Item Type													*/
		/*  -> Value1 ( Money )													*/
		/*  -> Value2 ( Main ID )												*/
		/*  -> Value3 ( Sub ID )												*/
		/*  -> Value4 ( Item Count )											*/
		/* - Victorious Country                                                 */
		/*  -> Value1 ( Map Main ID )                                           */
		/*  -> Value2 ( Map Sub ID )                                            */
		/*  -> Value3 ( Victorious Country ID )                                 */
		/************************************************************************/
		inline void				SetRewardValue (
			DWORD _dwRewardValue1,
			DWORD _dwRewardValue2 = 0,
			DWORD _dwRewardValue3 = 0,
			DWORD _dwRewardValue4 = 0 )
		{
			m_dwRewardValue1 = _dwRewardValue1;
			m_dwRewardValue2 = _dwRewardValue2;
			m_dwRewardValue3 = _dwRewardValue3;
			m_dwRewardValue4 = _dwRewardValue4;
		}

	public:
		// ���� ���� ���� Ȯ��;
		inline DWORD			GetUniqueID ( ) const		{ return m_dwUniqueID; }
		inline DWORD			GetChaDBNum ( )	const		{ return m_dwChaDBNum; }
		inline EMPROVIDE_TYPE	GetProvideType ( ) const	{ return m_emProvideType; }
		inline EMREWARD_TYPE	GetRewardType ( ) const		{ return m_emRewardType; }
		inline DWORD			GetTextIndex ( ) const		{ return m_dwTextIndex; }

		inline DWORD			GetRewardValue1 ( ) const	{ return m_dwRewardValue1; }
		inline DWORD			GetRewardValue2 ( ) const	{ return m_dwRewardValue2; }
		inline DWORD			GetRewardValue3 ( )	const	{ return m_dwRewardValue3; }
		inline DWORD			GetRewardValue4 ( )	const	{ return m_dwRewardValue4; }

	private:
		// ����ũ ID;
		DWORD					m_dwUniqueID;

		// ������ ������ ĳ����;
		DWORD					m_dwChaDBNum;

		// ���� ���;
		EMPROVIDE_TYPE			m_emProvideType;

		// ���� ����;
		EMREWARD_TYPE			m_emRewardType;
		DWORD					m_dwRewardValue1;
		DWORD					m_dwRewardValue2;
		DWORD					m_dwRewardValue3;
		DWORD					m_dwRewardValue4;

		// ���� ���� ���� �ؽ�Ʈ;
		DWORD					m_dwTextIndex;
	};

	typedef	std::queue< PROVIDE_REWARD_DATA >	QUEUE_PROVIDE_REWARD;

	typedef std::vector< PROVIDE_REWARD_DATA >	VEC_PROVIDE_REWARD;
	typedef VEC_PROVIDE_REWARD::iterator		VEC_PROVIDE_REWARD_ITER;

}
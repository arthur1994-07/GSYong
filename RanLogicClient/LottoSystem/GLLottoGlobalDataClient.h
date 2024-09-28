#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "./GLLottoDataTypeClient.h"

#include "../../RanLogic/Msg/GLContrlLottoSystemMsg.h"

namespace LottoSystem
{

	/* Lotto System의 Client용 Data Manager;
	 * 설정 Data를 관리한다;
	 * 기본적으로 Server에서 설정된 Data를 Sync해서 사용한다;
	 */
	class GLLottoGlobalDataClient : public CSingletone<GLLottoGlobalDataClient>
	{
	protected:
		friend CSingletone;

	public:
		GLLottoGlobalDataClient() : m_systemCount( 0 ) { }
		~GLLottoGlobalDataClient() { }

	public:
		// 서버와의 데이터 동기화 ( Login 시 동기화를 수행한다 );
		void SyncGlobalValue( GLMSG::NET_LOTTO_MESSAGE_SYNC_GLOBAL_VALUE_AC* pSyncMsg );
		void SyncStatus( GLMSG::NET_LOTTO_MESSAGE_SYNC_STATUS_AC* pSyncMsg );

	public:
		// Getter;
		inline const unsigned int GetLottoSystemNum() const { return m_systemCount; }
		inline const LOTTO_GLOBAL_VALUE GetLottoGlobalValue() const { return m_lottoGlobalValue; }
		inline const LOTTO_SYSTEM_CLIENT_VALUE GetLottoSystemValue( unsigned int id) const
		{
			if( id >= 0 && id < MAX_LOTTO_SYSTEM )
				return m_lottoSystemValues[ id ];

			return LOTTO_SYSTEM_CLIENT_VALUE();
		}

	private:
		// 로또 시스템 공통 설정 값;
		LOTTO_GLOBAL_VALUE m_lottoGlobalValue;

		/* 로또 시스템 별 설정 값;
		 * 자주 바뀌는 동적 데이터가 아니므로, 정적배열로 사용;
		 * 이렇게 사용함으로서 id와 index가 같아진다; */
		LOTTO_SYSTEM_CLIENT_VALUE m_lottoSystemValues[ MAX_LOTTO_SYSTEM ];
		unsigned int m_systemCount;
	};
}
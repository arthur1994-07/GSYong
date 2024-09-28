#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

#include "./GLLottoDataTypeClient.h"

#include "../../RanLogic/Msg/GLContrlLottoSystemMsg.h"

namespace LottoSystem
{

	/* Lotto System�� Client�� Data Manager;
	 * ���� Data�� �����Ѵ�;
	 * �⺻������ Server���� ������ Data�� Sync�ؼ� ����Ѵ�;
	 */
	class GLLottoGlobalDataClient : public CSingletone<GLLottoGlobalDataClient>
	{
	protected:
		friend CSingletone;

	public:
		GLLottoGlobalDataClient() : m_systemCount( 0 ) { }
		~GLLottoGlobalDataClient() { }

	public:
		// �������� ������ ����ȭ ( Login �� ����ȭ�� �����Ѵ� );
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
		// �ζ� �ý��� ���� ���� ��;
		LOTTO_GLOBAL_VALUE m_lottoGlobalValue;

		/* �ζ� �ý��� �� ���� ��;
		 * ���� �ٲ�� ���� �����Ͱ� �ƴϹǷ�, �����迭�� ���;
		 * �̷��� ��������μ� id�� index�� ��������; */
		LOTTO_SYSTEM_CLIENT_VALUE m_lottoSystemValues[ MAX_LOTTO_SYSTEM ];
		unsigned int m_systemCount;
	};
}
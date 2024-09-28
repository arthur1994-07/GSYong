#include "pch.h"
#include "MessageEnumMixer.h"
#include "../RanLogic/s_NetGlobal.h"

MessageEnumMixer& MessageEnumMixer::GetInstance()
{
	static MessageEnumMixer Instance;
	return Instance;
}

MessageEnumMixer::MessageEnumMixer( void )
{
	// 시드를 VERSION값을 가진 고유값으로 세팅한다.

	// 지난날짜 + 시 * 분
	// MAX = 0~365 + 24 * 60
	int nSeed = VersionNumbering::MAJOR + VersionNumbering::MINOR + VersionNumbering::DAYOFYERAR + (VersionNumbering::HOUR * VersionNumbering::MIN);
	srand( nSeed );

	// Boost Multi Index는 삽입된후에는 상수처리되어 변경할수 없기 때문에
	// 미리 배열을 만들어서 셔플한뒤 삽입한다.
	int arrInt[NET_MSG_GCTRL_MAX] = {0};

	// 순차 삽입
	for( int i =0 ; i<NET_MSG_GCTRL_MAX; i++)
		arrInt[i] = i;

	bool bUseShuffle = true;
	for( int i =0 ; i<NET_MSG_GCTRL_MAX; i++)
	{		
		if( bUseShuffle == false )
			break;

		// NET_MSG_LOGIN_2 패킷까지는
		//변경되어서는 안되는 패킷이기 때문에 셔플하지 않는다.
		if( i < NET_MSG_LOGIN_2 )
			continue;

		// 스왑
		int nShuffleKey = rand()%(NET_MSG_GCTRL_MAX-NET_MSG_LOGIN_2)+NET_MSG_LOGIN_2;

		int Temp = arrInt[i];
		arrInt[i] = arrInt[nShuffleKey];
		arrInt[nShuffleKey] = Temp;
	}


	// 초기값 세팅
	for ( int i=NET_MSG_BASE_ZERO ; i<NET_MSG_GCTRL_MAX ; i++ )
	{
		MessageEnum me;
		{
			me.emNetMsg		= static_cast<EMNET_MSG>(i);
			me.nMessageNum	= arrInt[i];
		}

		meMultyIndex.insert( me );
	}


	// seed를 초기화한다.
	sc::Random::getInstance();
}

EMNET_MSG MessageEnumMixer::DataToEmnet( const int nIndex ) 
{
	const intType& nType = meMultyIndex.get<Key_Int>();

	MessageEnumMultiIndex::index<Key_Int>::type::iterator it = nType.find( nIndex );
	MessageEnum me = *it;

	if( it != nType.end() )
		return me.emNetMsg;
	else
		return NET_MSG_BASE_ZERO;
}

int MessageEnumMixer::EmnetToData( const EMNET_MSG emNetMsg ) 
{
	const enumType& emType = meMultyIndex.get<Key_EMNET_MSG>();

	enumType::iterator it = emType.find( emNetMsg );
	MessageEnum me = *it;

	if( it != emType.end() )
		return me.nMessageNum;
	else
		return 0;
}


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
	// �õ带 VERSION���� ���� ���������� �����Ѵ�.

	// ������¥ + �� * ��
	// MAX = 0~365 + 24 * 60
	int nSeed = VersionNumbering::MAJOR + VersionNumbering::MINOR + VersionNumbering::DAYOFYERAR + (VersionNumbering::HOUR * VersionNumbering::MIN);
	srand( nSeed );

	// Boost Multi Index�� ���Ե��Ŀ��� ���ó���Ǿ� �����Ҽ� ���� ������
	// �̸� �迭�� ���� �����ѵ� �����Ѵ�.
	int arrInt[NET_MSG_GCTRL_MAX] = {0};

	// ���� ����
	for( int i =0 ; i<NET_MSG_GCTRL_MAX; i++)
		arrInt[i] = i;

	bool bUseShuffle = true;
	for( int i =0 ; i<NET_MSG_GCTRL_MAX; i++)
	{		
		if( bUseShuffle == false )
			break;

		// NET_MSG_LOGIN_2 ��Ŷ������
		//����Ǿ�� �ȵǴ� ��Ŷ�̱� ������ �������� �ʴ´�.
		if( i < NET_MSG_LOGIN_2 )
			continue;

		// ����
		int nShuffleKey = rand()%(NET_MSG_GCTRL_MAX-NET_MSG_LOGIN_2)+NET_MSG_LOGIN_2;

		int Temp = arrInt[i];
		arrInt[i] = arrInt[nShuffleKey];
		arrInt[nShuffleKey] = Temp;
	}


	// �ʱⰪ ����
	for ( int i=NET_MSG_BASE_ZERO ; i<NET_MSG_GCTRL_MAX ; i++ )
	{
		MessageEnum me;
		{
			me.emNetMsg		= static_cast<EMNET_MSG>(i);
			me.nMessageNum	= arrInt[i];
		}

		meMultyIndex.insert( me );
	}


	// seed�� �ʱ�ȭ�Ѵ�.
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


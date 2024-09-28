#pragma once
#include <iostream>

#include <boost/noncopyable.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/composite_key.hpp>

#include "../VersionMaker/VersionNo.h"

#include "../SigmaCore/Math/Random.h"


// 메시지 셔플 기능
//#define _USE_MESSAGE_SHUFFLE_

enum EMNET_MSG;

class MessageEnum
{
public:
	EMNET_MSG	emNetMsg;
	int			nMessageNum;
};

struct Key_EMNET_MSG{};
struct Key_Int{};

typedef boost::multi_index_container
<
MessageEnum,
	boost::multi_index::indexed_by
	<
		boost::multi_index::ordered_unique
		< 
			boost::multi_index::tag<Key_EMNET_MSG>, boost::multi_index::member< MessageEnum, EMNET_MSG, &MessageEnum::emNetMsg >
		>, 
		boost::multi_index::ordered_unique
		< 
			boost::multi_index::tag<Key_Int>, boost::multi_index::member< MessageEnum, int, &MessageEnum::nMessageNum > 
		>
	>
> MessageEnumMultiIndex;



class MessageEnumMixer : private boost::noncopyable
{
public:
	static MessageEnumMixer& GetInstance();

	MessageEnumMixer();
	~MessageEnumMixer(void){}

	// Data To EMNET
	EMNET_MSG	DataToEmnet( const int nIndex );

	// EMNET To Data
	int			EmnetToData( const EMNET_MSG emNetMsg );

private:
	MessageEnumMultiIndex meMultyIndex;
	typedef MessageEnumMultiIndex::index<Key_Int>::type			intType;
	typedef MessageEnumMultiIndex::index<Key_EMNET_MSG>::type	enumType;


};

class EmType
{
public:
	EmType() : m_nType(0) {}

	EmType( EMNET_MSG emType )
	{
		MessageEnumMixer& messageMixer = MessageEnumMixer::GetInstance();
		m_nType = messageMixer.EmnetToData( emType );
	}

	inline EMNET_MSG operator =( EMNET_MSG emType )
	{
		MessageEnumMixer& messageMixer = MessageEnumMixer::GetInstance();
		m_nType = messageMixer.EmnetToData( emType );

		return emType;
	}

	inline operator EMNET_MSG() const
	{
		return GetType();
	}

	inline bool operator ==( EMNET_MSG emType )
	{
		MessageEnumMixer& messageMixer = MessageEnumMixer::GetInstance();

		return ( messageMixer.EmnetToData( emType ) == m_nType );
	}

	inline EMNET_MSG GetType() const
	{
		MessageEnumMixer& messageMixer = MessageEnumMixer::GetInstance();
		EMNET_MSG em = messageMixer.DataToEmnet( m_nType );

		return em;
	}

	//Operators
	inline friend std::ostream &operator <<(std::ostream &c, const EmType &em)
	{
		c << em.GetType();
		return c;
	}

private:
	int m_nType;
};

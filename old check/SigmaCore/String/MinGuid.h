#ifndef _SC_MIN_GUID_H_
#define _SC_MIN_GUID_H_

#pragma once

#include <guiddef.h>
#include <boost/serialization/map.hpp>
#include "../MsgPack/MinMsgPack.h"

namespace sc
{
    /*
    typedef struct _GUID {
        ULONG	Data1;
        USHORT	Data2;
        USHORT	Data3;
        BYTE	Data4[8];
    } GUID;
    */
    //! 2012-04-17 jgkim
    //! https://github.com/HeliumProject/Helium/blob/master/Foundation/GUID.h
    //! https://github.com/HeliumProject/Helium/blob/master/Foundation/GUID.cpp
    struct MGUID : public GUID
    {
        MGUID(); // Null ID
        MGUID(const MGUID& id);
        MGUID(
            ULONG _Data1,
            USHORT _Data2,
            USHORT _Data3,
            BYTE _BData0,
            BYTE _BData1,
            BYTE _BData2,
            BYTE _BData3,
            BYTE _BData4,
            BYTE _BData5,
            BYTE _BData6,
            BYTE _BData7);
        MGUID& operator=(const MGUID& rhs);

        bool operator == (const MGUID& rhs) const;
        bool operator != (const MGUID& rhs) const;
        bool operator < (const MGUID& rhs) const;
        
        void Reset();

		MSGPACK_DEFINE( Data1, Data2, Data3, Data4[0], Data4[1], Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7] );

		private:
			friend class boost::serialization::access;
			template <typename Archive>
			void serialize(Archive &ar, const unsigned int version)
			{
				ar & Data1;
				ar & Data2;
				ar & Data3;
				ar & Data4[0];
				ar & Data4[1];
				ar & Data4[2];
				ar & Data4[3];
				ar & Data4[4];
				ar & Data4[5];
				ar & Data4[6];
				ar & Data4[7];
			}
    };
} // namespace sc

#endif // _SC_MIN_GUID_H_
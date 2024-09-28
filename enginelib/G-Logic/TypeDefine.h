#ifndef _ENGINE_TYPE_DEFINE_H_
#define _ENGINE_TYPE_DEFINE_H_

#pragma once

typedef DWORD GAEAID;

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include "../../=MsgPack/include/msgpack.hpp"

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

struct SNATIVEID
{
    enum
    {
        ID_NULL	= 0xFFFF,
    };

    union
    {
        struct
        {
            DWORD dwID;
        };

        struct
        {
            WORD wMainID; //! 주 ID
            WORD wSubID;  //! 보조 ID
        };
    };

    MSGPACK_DEFINE(dwID);

    SNATIVEID()
        : wMainID(0)
        , wSubID(0)
    {
    }

    SNATIVEID(bool bvalid) 
        : wMainID(0)
        , wSubID(0)
    {
        if (!bvalid)
        {
            wMainID = ID_NULL;
            wSubID = ID_NULL;
        }
    }

    SNATIVEID(DWORD dwid) 
        : dwID(dwid)
    {
    }

    SNATIVEID(WORD wid, WORD wsubid) 
        : wMainID(wid)
        , wSubID(wsubid)
    {
    }

    bool operator == (const SNATIVEID& nID) const
    {
        return (wMainID == nID.wMainID) && (wSubID == nID.wSubID);
    }

    bool operator != (const SNATIVEID& nID) const
    {
        return !((wMainID == nID.wMainID) && (wSubID == nID.wSubID));
    }

    bool operator < (const SNATIVEID& rhs) const
    {
		if ( wMainID < rhs.wMainID )
			return true;
		else if ( wMainID == rhs.wMainID )
		{
			return wSubID < rhs.wSubID;
		}
		
		return false;        
    }

	void Reset ( void )
	{
		wMainID = ID_NULL;
		wSubID = ID_NULL;
	}

    bool IsValidNativeID() const
    {
        return (wMainID != ID_NULL) && (wSubID != ID_NULL);
    }

    //! 주 ID
    inline WORD Mid() const { return wMainID; }
    inline void SetMid( WORD wMid ) { wMainID = wMid; }

    //! 보조 ID.
    inline WORD	Sid() const { return wSubID; }
    inline void SetSid( WORD wSid ) { wSubID = wSid; }

    inline DWORD Id() const { return dwID; }

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & dwID;
	}
};

inline SNATIVEID NATIVEID_NULL()	{ return SNATIVEID(SNATIVEID::ID_NULL,SNATIVEID::ID_NULL); }

struct SNATIVEID_MAP_PRED
{
	bool operator () (const SNATIVEID& lhs, const SNATIVEID& rhs) const
	{
		if (lhs.wMainID != rhs.wMainID)
		{
			return (lhs.wMainID < rhs.wMainID);
		}

		return (lhs.wSubID < rhs.wSubID);
	}
};

class MapID
{
public:
	const SNATIVEID& getBaseMapID(void) const { return baseMapID; }
	const SNATIVEID& getGaeaMapID(void) const { return gaeaMapID; }

	const bool isSameID(void) const { return baseMapID == gaeaMapID; }
	const bool isValid(void) const { return gaeaMapID != SNATIVEID(false); }
	const bool IsNULL(void) const { return ( baseMapID == SNATIVEID(false) && gaeaMapID == SNATIVEID(false)); }

	void setBaseMapID(const SNATIVEID& _baseMapID) { baseMapID = _baseMapID; }
	void setGaeaMapID(const SNATIVEID& _gaeaMapID) { gaeaMapID = _gaeaMapID; }
	void reset(void) { baseMapID = gaeaMapID = SNATIVEID(false); }

public:
	inline const MapID& operator =(const MapID& _rhs)
	{
		baseMapID = _rhs.baseMapID;
		gaeaMapID = _rhs.gaeaMapID;
		return *this;
	}
	inline const bool operator ==(const MapID& _rhs) const
	{
		return gaeaMapID == _rhs.gaeaMapID;
	}
	inline const bool operator !=(const MapID& _rhs) const
	{
		return gaeaMapID != _rhs.gaeaMapID;
	}
	MapID(void)
		: baseMapID(false)
		, gaeaMapID(false)
	{
	}
	MapID(const SNATIVEID& _baseMapID)
		: baseMapID(_baseMapID)
		, gaeaMapID(_baseMapID)
	{
	}
	MapID(const SNATIVEID& _baseMapID, const SNATIVEID& _gaeaMapID)
		: baseMapID(_baseMapID)
		, gaeaMapID(_gaeaMapID)
	{
	}
	MapID(const DWORD _baseMapID, const DWORD _gaeaMapID)
		: baseMapID(_baseMapID)
		, gaeaMapID(_gaeaMapID)
	{
	}

	MSGPACK_DEFINE(baseMapID, gaeaMapID);

private:
	// 해당 map이 가지는 고유의 ID(ex> dbNum);
	SNATIVEID baseMapID;

	// 해당 map이 메모리에 생성된 index ID(ex GaeaID);
	SNATIVEID gaeaMapID;

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & baseMapID;
		ar & indexMapID;
	}
};


//mjeon.instance
//  64-bit uniqueID

struct UNIQUEID
{
    enum
    {
        UNIQUE_NULL	= 0xFFFFFFFF,
    };

    union
    {
        struct
        {
			DWORD64		dw64id;
        };

        struct	//ChaNum first - little endian
        {
			DWORD		dwChaNum;		//identity which is valid only in the ServerGroup (ChaNum)
            DWORD		dwSvrGroup;		//server-group number            
        };
    };

    MSGPACK_DEFINE(dw64id);

    UNIQUEID()
        :dwSvrGroup(0)
        ,dwChaNum(0)
    {
    }

    UNIQUEID(bool bvalid)
		:dwSvrGroup(0)
        ,dwChaNum(0)
    {
        if (!bvalid)
        {
            dwSvrGroup	= UNIQUE_NULL;
            dwChaNum	= UNIQUE_NULL;
        }
    }

    UNIQUEID(DWORD64 &_dw64id)
		:dw64id(_dw64id)
	{
    }

    UNIQUEID(DWORD _dwSvrGroup, DWORD _dwChaNum)
		:dwSvrGroup(_dwSvrGroup)
		,dwChaNum(_dwChaNum)
    {
    }

    bool operator == (const UNIQUEID& rhs) const
    {
        return ((dwSvrGroup == rhs.dwSvrGroup) && (dwChaNum == rhs.dwChaNum));
    }

    bool operator != (const UNIQUEID& rhs) const
    {
        return !((dwSvrGroup == rhs.dwSvrGroup) && (dwChaNum == rhs.dwChaNum));
    }

    bool operator < (const UNIQUEID& rhs) const
    {
        return (dw64id < rhs.dw64id);
    }

    bool IsValid()
    {
        return (dwSvrGroup != UNIQUE_NULL) && (dwChaNum != UNIQUE_NULL);
    }

	inline DWORD		SvrGroup() const	{return dwSvrGroup;}
	inline DWORD		ChaDbNum() const	{return dwChaNum;}
	inline const DWORD64&	Id() const		{return dw64id;}
};

inline UNIQUEID UNIQUEID_NULL()		{ return UNIQUEID(false); }

struct UNIQUEID_PRED
{
	bool operator() (const UNIQUEID& lhs, const UNIQUEID& rhs) const
	{
		if (lhs.dwSvrGroup != rhs.dwSvrGroup)
		{
			return (lhs.dwSvrGroup < rhs.dwSvrGroup);
		}
		else
		{
			return (lhs.dwChaNum < rhs.dwChaNum);
		}
	}
};

#define _KEY_

#endif // _ENGINE_TYPE_DEFINE_H_

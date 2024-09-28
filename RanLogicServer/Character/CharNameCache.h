#pragma once

#include <string>
#include <boost/unordered_map.hpp>
#include <boost/tr1/memory.hpp>

enum EMUSERTYPE;

//! DB 억세스를 줄이기 위해서 일부 정보를 Cache 한다
struct CHAR_BASE_CACHE_DATA
{
    DWORD m_ChaDbNum; //! 캐릭터 DB 번호
    DWORD m_UserNum;  //! 유저 DB 번호
    BYTE  m_UserType; //! 유저 타입, EMUSERTYPE
    std::string m_ChaName; //! 캐릭터 이름
	int	  m_iClass;
	int	  m_iLv;

    CHAR_BASE_CACHE_DATA(DWORD ChaDbNum, DWORD UserNum, BYTE UserType, const std::string& ChaName)
        : m_ChaDbNum(ChaDbNum)
        , m_UserNum(UserNum)
        , m_UserType(UserType) // EMUSERTYPE USER_COMMON
        , m_ChaName(ChaName)
    {
    }

	CHAR_BASE_CACHE_DATA(DWORD ChaDbNum, DWORD UserNum, BYTE UserType, const std::string& ChaName, int iClass, int iLv)
		: m_ChaDbNum(ChaDbNum)
		, m_UserNum(UserNum)
		, m_UserType(UserType) // EMUSERTYPE USER_COMMON
		, m_ChaName(ChaName)
		, m_iClass(iClass)
		, m_iLv(iLv)
	{
	}

    inline DWORD ChaDbNum() const { return m_ChaDbNum; }
    inline DWORD UserDbNum() const { return m_UserNum; }
    inline BYTE  UserType() const { return m_UserType; } // EMUSERTYPE 
    inline const std::string& ChaName() const { return m_ChaName; }

	inline int	 GetClass() const { return m_iClass; }
	inline int	 GetLv()	const { return m_iLv; }
};

//! 캐릭터이름/DB 번호, DB 번호/캐릭터 이름 관리
//! 서버가 시작도면 계속해서 캐릭터이름/DB 번호를 수집해서
//! 다른곳에서 사용시 빠르게 검색한다.
//! 캐릭터 이름이 변경될때 잘 처리해야 한다.
//! 이곳에 없는 캐릭터는 서버가 가동되고 한번도 접속하지 않은 캐릭터이다.
//! 이때는 DB 에서 직접 가져와야 한다.
//! thread safe 해야한다.
//! 2012-07-30 jgkim
class CharNameCache
{
public:
    CharNameCache();
    ~CharNameCache();

protected:
    //! 캐릭터이름/DB 번호
    typedef boost::unordered_map<std::string, std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> > NAME_DBNUM;
    typedef NAME_DBNUM::iterator       NAME_DBNUM_ITER;
    typedef NAME_DBNUM::const_iterator NAME_DBNUM_CITER;
    typedef NAME_DBNUM::value_type     NAME_DBNUM_VALUE;

    //! DB 번호/캐릭터 이름
    typedef boost::unordered_map<DWORD, std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> > DBNUM_NAME;
    typedef DBNUM_NAME::iterator       DBNUM_NAME_ITER;
    typedef DBNUM_NAME::const_iterator DBNUM_NAME_CITER;
    typedef DBNUM_NAME::value_type     DBNUM_NAME_VALUE;

    NAME_DBNUM m_NameDb; //! 캐릭터이름/DB 번호
    DBNUM_NAME m_DbName; //! DB 번호/캐릭터 이름

    CRITICAL_SECTION m_Lock;

public:
    void Add(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType);
	void Add(CHAR_BASE_CACHE_DATA& sData);
    void Del(const std::string& ChaName);
    void Del(const DWORD ChaDbNum);

    //! 캐릭터이름 -> DB 번호
    DWORD FindChaDbNum(const std::string& ChaName);

    //! 캐릭터 DB 번호 -> 캐릭터 이름
    std::string FindChaName(DWORD ChaDbNum);    

    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> Find(const std::string& ChaName);
    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> Find(DWORD ChaDbNum);
};
#pragma once

#include <string>
#include <boost/unordered_map.hpp>
#include <boost/tr1/memory.hpp>

enum EMUSERTYPE;

//! DB �＼���� ���̱� ���ؼ� �Ϻ� ������ Cache �Ѵ�
struct CHAR_BASE_CACHE_DATA
{
    DWORD m_ChaDbNum; //! ĳ���� DB ��ȣ
    DWORD m_UserNum;  //! ���� DB ��ȣ
    BYTE  m_UserType; //! ���� Ÿ��, EMUSERTYPE
    std::string m_ChaName; //! ĳ���� �̸�
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

//! ĳ�����̸�/DB ��ȣ, DB ��ȣ/ĳ���� �̸� ����
//! ������ ���۵��� ����ؼ� ĳ�����̸�/DB ��ȣ�� �����ؼ�
//! �ٸ������� ���� ������ �˻��Ѵ�.
//! ĳ���� �̸��� ����ɶ� �� ó���ؾ� �Ѵ�.
//! �̰��� ���� ĳ���ʹ� ������ �����ǰ� �ѹ��� �������� ���� ĳ�����̴�.
//! �̶��� DB ���� ���� �����;� �Ѵ�.
//! thread safe �ؾ��Ѵ�.
//! 2012-07-30 jgkim
class CharNameCache
{
public:
    CharNameCache();
    ~CharNameCache();

protected:
    //! ĳ�����̸�/DB ��ȣ
    typedef boost::unordered_map<std::string, std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> > NAME_DBNUM;
    typedef NAME_DBNUM::iterator       NAME_DBNUM_ITER;
    typedef NAME_DBNUM::const_iterator NAME_DBNUM_CITER;
    typedef NAME_DBNUM::value_type     NAME_DBNUM_VALUE;

    //! DB ��ȣ/ĳ���� �̸�
    typedef boost::unordered_map<DWORD, std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> > DBNUM_NAME;
    typedef DBNUM_NAME::iterator       DBNUM_NAME_ITER;
    typedef DBNUM_NAME::const_iterator DBNUM_NAME_CITER;
    typedef DBNUM_NAME::value_type     DBNUM_NAME_VALUE;

    NAME_DBNUM m_NameDb; //! ĳ�����̸�/DB ��ȣ
    DBNUM_NAME m_DbName; //! DB ��ȣ/ĳ���� �̸�

    CRITICAL_SECTION m_Lock;

public:
    void Add(const std::string& ChaName, DWORD ChaDbNum, DWORD UserDbNum, EMUSERTYPE UserType);
	void Add(CHAR_BASE_CACHE_DATA& sData);
    void Del(const std::string& ChaName);
    void Del(const DWORD ChaDbNum);

    //! ĳ�����̸� -> DB ��ȣ
    DWORD FindChaDbNum(const std::string& ChaName);

    //! ĳ���� DB ��ȣ -> ĳ���� �̸�
    std::string FindChaName(DWORD ChaDbNum);    

    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> Find(const std::string& ChaName);
    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> Find(DWORD ChaDbNum);
};
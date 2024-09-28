#pragma once

#include <map>
#include <boost/tr1/unordered_map.hpp>
#include <boost/function.hpp>
#include <string>

class GLStringTable
{
    typedef std::map<std::string, std::string>	DESMAP_FOR_SAVE;
    typedef DESMAP_FOR_SAVE::iterator           DESMAPSAVE_ITER;

    typedef std::tr1::unordered_map<std::string, std::string> DESMAP;
	typedef DESMAP::iterator					              DESMAP_ITER;
	typedef DESMAP::const_iterator		                      DESMAP_CITER;
    typedef DESMAP::value_type                                DESMAP_VALUE;

	DESMAP	m_mapItemStringTable;
	DESMAP	m_mapSkillStringTable;
	DESMAP	m_mapCrowStringTable;

	DESMAP * m_pLoadMap;

public:
	enum EMST_TYPE
	{
		ITEM,
		SKILL,
		CROW
	};

	void CLEAR(void);
	bool LOADFILE( const TCHAR * szFileName, EMST_TYPE emType, boost::function<void (std::string)> PercentCallbackFunc=NULL );
	bool SAVEFILE( EMST_TYPE emType );

    const TCHAR* GetString(const std::string& strKey, EMST_TYPE emType);
    const std::string& GetItemString(const std::string& strKey) const;
    const std::string& GetSkillString(const std::string& strKey) const;
    const std::string& GetCrowString(const std::string& strKey) const;

	void InsertString( std::string & strKey, std::string & strSub, EMST_TYPE emType );
	void DeleteString( std::string & strKeyName, std::string & strKeyDesc, EMST_TYPE emType );
	void DeleteString( WORD wMID, WORD wSID, EMST_TYPE emType );

private:
	void SetCurrentMap( EMST_TYPE emType );
	DESMAP* GetCurrentMap();

public:
	GLStringTable(void);
	~GLStringTable(void);

public:
	static GLStringTable & GetInstance();
};
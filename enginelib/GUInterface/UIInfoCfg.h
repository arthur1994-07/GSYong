#pragma once

//--------------------------------------------------------------------------------------------//

#include "../../SigmaCore/Xml/2.5.3/ticpp.h"

//--------------------------------------------------------------------------------------------//

#include "UIInfoCfg_Interface.h"

//--------------------------------------------------------------------------------------------//

#include <string>
#include <boost/noncopyable.hpp>

//--------------------------------------------------------------------------------------------//
class SUBPATH;

class CUIInfoCfg : public IUIInfoCfg, private boost::noncopyable
{

public:
    enum { UIINFO_VERSION = 0x001 };

public:
    CUIInfoCfg ();
    ~CUIInfoCfg ();

public:
    const bool   Load  (const SUBPATH* pPath);
    const bool   Save  (const SUBPATH* pPath);
    void         Clear ();         

public:
    const DWORD                  GetVersion () { return m_dwVersion; }

	void SetAddPathName(const std::string& name) { m_AddPathName = name; }
	void SetAddPathNameServerNum(DWORD num) { m_AddPathNameServerNum = num; }
public:
    virtual NSUIINFOCFG::CONTROL LoadControlInfo ( const std::string& strID );
    virtual NSUIINFOCFG::CONTROL SaveControlInfo ( const std::string& strID, const DWORD dwVersion=0 );

private:
    const bool Create ();

private:
    std::string GetFilePath(const SUBPATH* pPath);

private:
    ticpp::Document m_XmlDoc;
    DWORD           m_dwVersion;

	std::string				     m_AddPathName;
	DWORD						 m_AddPathNameServerNum;
private:
    ticpp::Element* m_pInfo;

public:
    static CUIInfoCfg& GetInstance();

};

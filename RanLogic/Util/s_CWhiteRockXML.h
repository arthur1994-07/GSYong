#ifndef S_CWHITEROCKXML_H_
#define S_CWHITEROCKXML_H_

#pragma once

#include "../../SigmaCore/Xml/2.5.2/tinyxml.h"

class CWhiteRockXml
{
	TIXMLOLD::TiXmlDocument m_XmlDoc;	

public:
	CWhiteRockXml();
	~CWhiteRockXml();

public:

	bool LoadXml( const CString& strFileName );
	bool LoadString( const CString& strXmlData );
    bool SaveXml( const CString& strFileName );
	void SaveData();

	bool SetData( const CString& strChildPath, const CString& strData );
	CString GetData ( const CString& strChildPath );
	const char* GetXmlString ();

	void Init();

private:
	bool SetXMLNodeText( TIXMLOLD::TiXmlElement* rootElem, const CString& strChildPath, const CString& strData );
	TIXMLOLD::TiXmlElement* GetChildElement( TIXMLOLD::TiXmlElement* rootElem, const CString& strChildPath );
	bool GetXMLNodeText( TIXMLOLD::TiXmlElement* rootElem, const CString& strChildPath, CString& strNodeText );
};

#endif // S_CWHITEROCKXML_H_
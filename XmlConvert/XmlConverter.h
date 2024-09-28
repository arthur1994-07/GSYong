#pragma once

#define _AFXDLL

#include "../SigmaCore/Xml/2.5.2/tinyxml.h"

using namespace TIXMLOLD;
class WStringFile;

struct QueryAtrb
{
	char* atrbName;
	char* atrbData;
	QueryAtrb( char* strName, char* strData )
		: atrbName(strName), atrbData(strData)
	{

	}
};

class XmlConverter
{
public:
	enum XML_RETURN_GBGIM
	{
		OK					= 0,		// ����;
		NOEXIST_ELEM		= 1,		// ���������ʴ� ���;
		NOEXIST_ELEM_OF_ATRB= 2,		// �̸��� ������Ʈ�� �����ϳ� �Ӽ��� ��������;
		OVERLAP				= 3,		// �ΰ��̻� ������;
		INVAILD_ROOTELEM	= 4,
		N_SIZE,
	};

private:
	TiXmlDocument	m_XmlDocument;
	TiXmlElement*	m_pXmlRootElem;

public:
	XmlConverter( const char* szVersion, const char* pEncoding );
	~XmlConverter(void);

public:
	const XML_RETURN_GBGIM GetChildElement( TIXMLOLD::TiXmlElement** outXmlElem, 
		TIXMLOLD::TiXmlElement* rootElem,
		const char* atrbName,
		const char* atrbData);
	const XML_RETURN_GBGIM GetChildElement_MultiAtrb( TIXMLOLD::TiXmlElement** outXmlElem, 
		TIXMLOLD::TiXmlElement* rootElem,
		QueryAtrb* Atrb,
		const int nCount );

	bool RanTextConvertToXml_Sentence( const char* szFile, const char* szSave, const char* szNational, bool bSave );
	bool RanTipTextConvertToXml( const char* szFile, const char* szSave, const char* szNational, bool bSave );
	bool RanTextConvertToXml_Word( const char* szFile, const char* szSave, const char* szNational, bool bSave );
	bool RanTextConvertToXml_EX( const char* szFile, const char* szSave, const char* szNational, bool bSave );
	bool RanTextConvertToXml_CommentLua( const char* szFile, const char* szSave, const char* szNational, bool bSave );
private:
	// Lua
	TiXmlElement* CompositionData( WStringFile& WStrFile, sc::CStringFile& StrFile, CString& strCurrentLine, TiXmlElement* pParentElem, const char* szNation );
	TiXmlElement* WCSCompositionSentence( WStringFile& WStrFile, CString& strSentence, std::wstring& wstrSentence, TiXmlElement* pParentElem, const char* szNation );

public:
	bool RanTextConvert_Lua(const char* szFile, const char* szSave, const char* szNational, bool bSave);
	BOOL CompositionSentence_Lua(CString& strSentence, TiXmlElement* pParentElem, const char* szNation);

	// Comment lua /**/
	bool RanTextConvertToXml_CommentLua_OnlyText(const char* szFile, const char* szSave, const char* szNational, bool bSave);
private:
	bool _Lua_2_Composition(const char* atrbData
		, std::string* strArray
		, const int nSize
		, const char* szNation );

	void LoadHelp(
		const std::string& strTitle,
		std::string* pList,
		int nCount );

	bool LOADFILE_LUA( const std::string& strPath );
	bool ParseCommentLua();
	void ClearLuaDefaultString();
	/**/
public:	// PetComment lua /**/
	bool RanTextConvertToXml_PetCommentLua_OnlyText(const char* szFile, const char* szSave, const char* szNational, bool bSave);

private:
	bool _Lua_3_Composition(const char* atrbData
		, std::vector<std::string>& vecString
		, const char* szNation );

	bool LOADFILE_LUA_PET( const std::string& strPath );
	bool ParsePetCommentLua();
	void ClearPetLuaDefaultString();
	/**/

private:
	std::string unicodeToUtf8(std::wstring& szW);
public:
	bool RanTextConvert_Emoticon(const char* szFile, const char* szSave, const char* szNational, bool bSave);

public:
	bool RanTextConvert_MobLogic(const char* szFile, const char* szSave, const char* szNational, bool bSave);

	// Excel
public:
	bool RanTextConvert_Excel(const char* szFile, const char* szSave, const char* szNational, bool bSave);
	bool RanTextConvert_ExcelToXml(const char* szFile, const char* szSave, const char* szSheet, bool bSave);

	// ���� �ڵ忡 ���Ͽ� �⺻ ���ڿ� ä����;
public:
	bool RanTextConvert_Skip(const char* szFile, const char* szSave, const char* szNational, bool bSave);
};

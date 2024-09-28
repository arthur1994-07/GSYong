#pragma once

#include "GLWidgetScript.h"

class GLWidgetProp
{
public :
	GLWidgetProp();
	~GLWidgetProp();

public :
	struct SVALUE
	{
		enum EMVALUETYPE
		{
			V_NONE = 0,
			V_INT,
			V_DWORD,
			V_DOUBLE,
			V_BOOL,
			V_STRING,
		};

		EMVALUETYPE emType;
		
		int			nValue;
		DWORD		dwValue;
		double		dValue;
		bool		bValue;
		std::string	strValue;

		bool		bIsValid;

		SVALUE()
			: emType( V_NONE )
			, bIsValid( false )
		{
		}
	};

public :
	// 프로퍼티, 값
	typedef stdext::hash_map< std::string, SVALUE > MAP_PROP;
	typedef MAP_PROP::iterator						MAP_PROP_ITER;

public :
	MAP_PROP m_mapProp;

public :
	void InsertValue( SVALUE::EMVALUETYPE emType, const std::string& strProp );

	void SetInt( const std::string& strProp, int nValue );
	void SetDWORD( const std::string& strProp, DWORD dwValue );
	void SetDouble( const std::string& strProp, double dValue );
	void SetBool( const std::string& strProp, bool bValue );
	void SetString( const std::string& strProp, const std::string& strValue );

	bool GetInt( const std::string& strProp, int& nValue );
	bool GetDWORD( const std::string& strProp, DWORD& dwValue );
	bool GetDouble( const std::string& strProp, double& dValue );
	bool GetBool( const std::string& strProp, bool& bValue );
	bool GetString( const std::string& strProp, std::string& strValue );
};

//--------------------------------------------------------------------------------------------------------

class GLWidgetSaveProp
{
private :
	GLWidgetSaveProp();
	GLWidgetSaveProp( const GLWidgetSaveProp& value );
	~GLWidgetSaveProp();

public :
	static GLWidgetSaveProp& GetInstance();

private :
	static const std::string m_XMLNAME;

private :
	// 인스턴스 네임, 프로퍼티
	typedef stdext::hash_map< std::string, GLWidgetProp >	MAP_INSTPROP;
	typedef MAP_INSTPROP::iterator							MAP_INSTPROP_ITER;

	// 위젯ID, 인스턴스-프로퍼티
	typedef stdext::hash_map< WORD, MAP_INSTPROP >			MAP_SAVEWIDGETPROP;
	typedef MAP_SAVEWIDGETPROP::iterator					MAP_SAVEWIDGETPROP_ITER;

private :
	MAP_SAVEWIDGETPROP m_mapWidgetProp;	
	std::string m_strName;

public :
	float fUIAlpha;
	int m_nBasicChat_Size;

public :
	void SaveXML();
	void LoadXML( std::string strName );

	// 루아에서 저장할 프로퍼티 설정값 읽기
	void Load( LuaTable& tbSaveProp );

	// 저장된 값 초기화
	void ResetValue();

public :
	// 저장된 값으로 해당 UI 프로퍼티 설정
	void SetProperty( WORD wWidgetID, WORD wSubID );
	// 해당 UI 프로퍼티 저장
	void SaveProperty( WORD wWidgetID, WORD wSubID );
};
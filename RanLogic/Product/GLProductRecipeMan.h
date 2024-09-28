#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <string>

#include "./GLProductRecipe.h"
#include "../Character/GLCharDefine.h"
//-----------------------------
//  [2/8/2013 hsshin]
// 제작 시스템 리뉴얼
//-----------------------------

typedef std::vector< SProductRecipe* > PtrVector_ProductRecipe;

typedef std::tr1::unordered_map< DWORD, SProductRecipe > PRODUCT_RECIPE_KEYMAP;			// 조합법을 조합법고유키를 사용하여 맵으로 저장하기 위한 타입선언
typedef PRODUCT_RECIPE_KEYMAP::iterator PRODUCT_RECIPE_KEYMAP_ITER;

typedef std::tr1::unordered_map< std::string, SProductRecipe > ProductRecipe_STRMAP;    // 조합법을 문자열을 사용하여 맵으로 저장하기 위한 타입선언 (삭제 예정)
typedef ProductRecipe_STRMAP::iterator ProductRecipe_STRMAP_ITER;

struct SINVENITEM;
struct SITEM;

class GLProductRecipeMan
{
public:
	enum
	{
		VERSION	= 0x0101, //조합시스템 추가로 인해 버젼 1.00 -> 1.01로
	};
	enum
	{
		NONEFIND_CLASS = GLCL_NUM_CLASS,  // 조합법 검색시 클래스 검색 안할 때 사용
	};

public:
	static GLProductRecipeMan& GetInstance();

private:
	bool                    m_bServer;

	std::string             m_strPATH;

public:
	PRODUCT_RECIPE_KEYMAP	m_mapKeyProductRecipe;			                                                        // 제조법 리스트맵 제작키로 저장
	ProductRecipe_STRMAP	m_mapStrProductRecipe;			                                                        // 제조법 리스트맵 스트링을 키값으로 하여 저장 (삭제 예정)

	GLProductRecipeMan();
	~GLProductRecipeMan();

public:
	void            CleanUp();
	void            InsertRecipe( SProductRecipe& SRecipe );			                                            //제조법리스트맵에 제조법을 추가

	void            SortMeterialItem( SProductRecipe& SRecipe );		                                            //제조법의 재료아이템을 정렬한다.
	void            SortInvenItem( SINVENITEM* InvenItem );

	SProductRecipe* GetProductRecipe( const DWORD dwKey );		                                                    //고유키값으로 해당 조합법을 얻어옴
	SProductRecipe* GetProductRecipeClient( SProductRecipe& SRecipe );	                                            //삭제 예정
	SProductRecipe* GetProductRecipeEmulator( SProductRecipe& SRecipe );	                                        //삭제 예정

	/**	
	 조건에 맞는 조합법을 찾는 함수
	 eProductType는 찾을 제작분야를 검색 조건으로.
	 eFindClass는 착용 클래스를 검색 조건으로.
	 나오게되는 조합법벡터가 제작UI쪽에서 보여지게 됩니다.
	 
	 eProductType = 검색할 제작 분야
	 eFindClass   = 검사할 클래스 : EMCLASSINDEX(클래스인덱스로검색),NONEFIND_CLASS(클래스검사안함)
	**/
	void            Find( PtrVector_ProductRecipe& vecData, BYTE eProductType, BYTE eFindClass = NONEFIND_CLASS );

public:
	HRESULT         LoadFile_OLD( const char* szFile, const std::string& FileName, BOOL bServer, bool bToolMode );  //이전 형식 파일을 읽는 함수인듯?
    HRESULT         LoadFile_OLD( const std::string& FileName, BOOL bServer, bool bToolMode );		                //이전 형식 파일을 읽는 함수인듯?
	HRESULT         LoadFile( const char* szFile, const std::string& FileName, BOOL bServer );
    HRESULT         LoadFile( const std::string& FileName, BOOL bServer );	                                        //조합법 리스트 파일을 읽는다
	HRESULT         SaveFile( const char* szFile );							                                        //조합법 리스트를 저장
	bool            SaveCsv( std::fstream &SFile );							                                        //조합법 리스트를 엑셀 파일 형태로 저장
	bool            LoadCsv( std::string& strFileName );						                                    //조합법 리스트를 엑셀 파일에서 읽어옴

	void			SaveSQLFile ( void );

public:
	void            SetPath(const std::string& strPATH) { m_strPATH = strPATH; }
	std::string     GetPath() const { return m_strPATH; }

private:
    WORD            FindSuitType( const SITEM* pItem );                                                             //제작분야를 알아내기위해 사용 (해당물건이 어떤 제작분야인지 검출) // [2/8/2013 hsshin] 숙련도시스템 이전버전방식
	bool            IsValidClass( const SITEM* pItem, BYTE eFindClass );				                            //아이템을 해당 클래스가 사용 할 수 있는지 판별
	bool            IsRandomBoxValidClass( const SITEM* pItem, BYTE eFindClass );		
};

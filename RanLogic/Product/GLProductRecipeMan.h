#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <string>

#include "./GLProductRecipe.h"
#include "../Character/GLCharDefine.h"
//-----------------------------
//  [2/8/2013 hsshin]
// ���� �ý��� ������
//-----------------------------

typedef std::vector< SProductRecipe* > PtrVector_ProductRecipe;

typedef std::tr1::unordered_map< DWORD, SProductRecipe > PRODUCT_RECIPE_KEYMAP;			// ���չ��� ���չ�����Ű�� ����Ͽ� ������ �����ϱ� ���� Ÿ�Լ���
typedef PRODUCT_RECIPE_KEYMAP::iterator PRODUCT_RECIPE_KEYMAP_ITER;

typedef std::tr1::unordered_map< std::string, SProductRecipe > ProductRecipe_STRMAP;    // ���չ��� ���ڿ��� ����Ͽ� ������ �����ϱ� ���� Ÿ�Լ��� (���� ����)
typedef ProductRecipe_STRMAP::iterator ProductRecipe_STRMAP_ITER;

struct SINVENITEM;
struct SITEM;

class GLProductRecipeMan
{
public:
	enum
	{
		VERSION	= 0x0101, //���սý��� �߰��� ���� ���� 1.00 -> 1.01��
	};
	enum
	{
		NONEFIND_CLASS = GLCL_NUM_CLASS,  // ���չ� �˻��� Ŭ���� �˻� ���� �� ���
	};

public:
	static GLProductRecipeMan& GetInstance();

private:
	bool                    m_bServer;

	std::string             m_strPATH;

public:
	PRODUCT_RECIPE_KEYMAP	m_mapKeyProductRecipe;			                                                        // ������ ����Ʈ�� ����Ű�� ����
	ProductRecipe_STRMAP	m_mapStrProductRecipe;			                                                        // ������ ����Ʈ�� ��Ʈ���� Ű������ �Ͽ� ���� (���� ����)

	GLProductRecipeMan();
	~GLProductRecipeMan();

public:
	void            CleanUp();
	void            InsertRecipe( SProductRecipe& SRecipe );			                                            //����������Ʈ�ʿ� �������� �߰�

	void            SortMeterialItem( SProductRecipe& SRecipe );		                                            //�������� ���������� �����Ѵ�.
	void            SortInvenItem( SINVENITEM* InvenItem );

	SProductRecipe* GetProductRecipe( const DWORD dwKey );		                                                    //����Ű������ �ش� ���չ��� ����
	SProductRecipe* GetProductRecipeClient( SProductRecipe& SRecipe );	                                            //���� ����
	SProductRecipe* GetProductRecipeEmulator( SProductRecipe& SRecipe );	                                        //���� ����

	/**	
	 ���ǿ� �´� ���չ��� ã�� �Լ�
	 eProductType�� ã�� ���ۺо߸� �˻� ��������.
	 eFindClass�� ���� Ŭ������ �˻� ��������.
	 �����ԵǴ� ���չ����Ͱ� ����UI�ʿ��� �������� �˴ϴ�.
	 
	 eProductType = �˻��� ���� �о�
	 eFindClass   = �˻��� Ŭ���� : EMCLASSINDEX(Ŭ�����ε����ΰ˻�),NONEFIND_CLASS(Ŭ�����˻����)
	**/
	void            Find( PtrVector_ProductRecipe& vecData, BYTE eProductType, BYTE eFindClass = NONEFIND_CLASS );

public:
	HRESULT         LoadFile_OLD( const char* szFile, const std::string& FileName, BOOL bServer, bool bToolMode );  //���� ���� ������ �д� �Լ��ε�?
    HRESULT         LoadFile_OLD( const std::string& FileName, BOOL bServer, bool bToolMode );		                //���� ���� ������ �д� �Լ��ε�?
	HRESULT         LoadFile( const char* szFile, const std::string& FileName, BOOL bServer );
    HRESULT         LoadFile( const std::string& FileName, BOOL bServer );	                                        //���չ� ����Ʈ ������ �д´�
	HRESULT         SaveFile( const char* szFile );							                                        //���չ� ����Ʈ�� ����
	bool            SaveCsv( std::fstream &SFile );							                                        //���չ� ����Ʈ�� ���� ���� ���·� ����
	bool            LoadCsv( std::string& strFileName );						                                    //���չ� ����Ʈ�� ���� ���Ͽ��� �о��

	void			SaveSQLFile ( void );

public:
	void            SetPath(const std::string& strPATH) { m_strPATH = strPATH; }
	std::string     GetPath() const { return m_strPATH; }

private:
    WORD            FindSuitType( const SITEM* pItem );                                                             //���ۺо߸� �˾Ƴ������� ��� (�ش繰���� � ���ۺо����� ����) // [2/8/2013 hsshin] ���õ��ý��� �����������
	bool            IsValidClass( const SITEM* pItem, BYTE eFindClass );				                            //�������� �ش� Ŭ������ ��� �� �� �ִ��� �Ǻ�
	bool            IsRandomBoxValidClass( const SITEM* pItem, BYTE eFindClass );		
};

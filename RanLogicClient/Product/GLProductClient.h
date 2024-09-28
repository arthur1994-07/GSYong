#pragma once

#include "../../RanLogic/Product/GLProduct.h"
#include "../../RanLogic/Item/GLItemCustom.h"

#include "../Widget/GLWidgetScript.h"

class GLGaeaClient;
struct NET_MSG_GENERIC;

class GLProductClient : public GLProduct
{
private:
	GLGaeaClient* m_pGaeaClient;

	int m_iMakeCount;				// �Ϻ� 1�� �̻��� �������� �����ϴ� ������ �־� �̸� ���Ʒ��ִ� ����

public:
	GLProductClient( GLGaeaClient* pGaeaClent  );
	virtual ~GLProductClient();

    //Ŭ���̾�Ʈ�� ���� �˻� �Լ�
    bool        CheckProduct( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic, BYTE& eState, const WORD nCheckCount, bool bOngoing=false );    //���� �˻�
	bool        CheckProductMaterial( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic, WORD wMultiProduct, BYTE& eState );    // ��� �˻�;
    WORD        CountProduct( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic, BYTE& eState );    //���������� �˻����� �� � ���� �������� ī��Ʈ

	WORD		GetCountProduct( DWORD nID	);															// CountProduct �Լ��� ��ũ��Ʈ���� ��� �����ϰ� ����� �Լ� 

    // ���ۺо߿� ����Ŭ������ ���չ��� �˻� (�ش� ���չ� ���͸� �����Ͽ� ����)
    bool        FindProductList( PtrVector_ProductRecipe& vecData, const BYTE eFindType, const BYTE eFindClass );

    //���۰��� �޽��� ���
    void		ProductOutputText( BYTE eState, const SProductRecipe* sRecipe );				//���� �ý��� �޽��� ���
    void		ProductOutputText( BYTE eState, WORD wKey );

    //���� �ý��۰��� ��Ŷ ���� ��û �Լ�
	HRESULT		ReqProduct( DWORD nID, WORD nCount, bool bOngoing=false );						//���� ��û
	HRESULT		ReqProductCancel();																//���� ��� ��û
	HRESULT		ReqLearnProductBook( WORD wPosX, WORD wPosY );									//���ռ� ������ ��� ��û
    HRESULT     ReqProductTypeAdd( WORD wProductType );											//���ۺо� �н� ��û
    HRESULT     ReqProductTypeDel( std::vector<int>& vecProductList, WORD wPosX, WORD wPosY );	//���ۺо� ���� ��û
    HRESULT     ReqProductRecipeList();												            //���� ���ռ� ����Ʈ ��û
    HRESULT     ReqProductTypeList();                               		                    //�н��� ���ۺо� ����Ʈ ��û
    HRESULT     ReqProductExpList();												            //�������� ���õ��� ������ ��� ��û
  
    //���� �ý��� ���� ���ú� �޽��� ó�� �Լ�
public:
    void        ProductMsgProcess( NET_MSG_GENERIC* pMsg );							            //���� �ý��� ���� �޽��� �й� ó���Լ�
protected:
    void        MsgProductFb( NET_MSG_GENERIC* pMsg );								            //���� ��� �޽��� ó��
    void        MsgLearnProductBookFb( NET_MSG_GENERIC* pMsg );						            //���ռ� ������ ��� ��� �޽��� ó��
    void        MsgProductRecipeListAck( NET_MSG_GENERIC* pMsg );					            //������ ���ռ� ����Ʈ ��û ��� �޽��� ó��
    void        MsgProductTypeListtAck( NET_MSG_GENERIC* pMsg );					            //�н��� ���ۺо� ����Ʈ ��û ��� �޽��� ó��
    void        MsgProductExpListAck( NET_MSG_GENERIC* pMsg );						            //�������� ���õ��� ������ ��� ��û ��� �޽��� ó��
    void        MsgProductTypeAddAck( NET_MSG_GENERIC* pMsg );						            //���ۺо� �н� ��û ��� �޽��� ó��           
    void        MsgProductTypeDelAck( NET_MSG_GENERIC* pMsg );						            //���ۺо� �н� ��û ��� �޽��� ó��           

public:

	lua_tinker::table	GetStudiedProducts();																				// ��� ���� TYPE��;
	lua_tinker::table	GetProductList( const BYTE eFindType, const BYTE eFindClass, const bool bShowProductAble = false );	// ���۸�� ��������;

	lua_tinker::table	GetSelectedProductData( const unsigned int nScrollPos );											// �����׸� ����Ÿ(���);
	lua_tinker::table	GetSelectedProductRequirment( const unsigned int nScrollPos );										// �����׸� �䱸�����ؽ�Ʈ����Ÿ;
	std::string 		GetSelectedProductName( const unsigned int nScrollPos );											// �����׸� ������ �̸�;
	const DWORD 		GetSelectedProductCost( const unsigned int nScrollPos );											// �����׸� ���;
	lua_tinker::table	GetSelectedProductResultData( const unsigned int nScrollPos );												// �����׸� ID;
	const DWORD			GetSelectedProductKey( const unsigned int nScrollPos );												// �����׸� key;
	const FLOAT			GetSelectedProductDelay( const unsigned int nScrollPos );											// �����׸� ���۽ð�;
	const FLOAT			GetSelectedProductRate( const unsigned int nScrollPos );											// �����׸� ���� Ȯ��;

	const BYTE			GetStudyState( WORD wProductType );																	// ���ۺо� �н�����;

	void				ReqProductTypeDelete( lua_tinker::table tbProductList, WORD wPosX, WORD wPosY );					// ���ۺо� ���� ��û;

protected:
	void				StartProduct( const char* cItemName, int nCount, float fDelaySec );									// ���α׷����� ����;
	void				EndProduct();																						// ���α׷����� �ݱ�;

private:
	void				FilteringProduct();																					// ���۸�� �� ���۰����׸� ���͸�;
	PtrVector_ProductRecipe	m_PtrVectorProductRecipe;
};

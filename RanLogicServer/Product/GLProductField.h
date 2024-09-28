#pragma once

#include "../../RanLogic/Product/GLProduct.h"
#include "../../RanLogic/Msg/ProductMsg.h"

class GLGaeaServer;
class GLChar;
struct NET_MSG_GENERIC;

//hsshin ���� ���� �ʵ�
class GLProductField : public GLProduct
{
public:
	GLProductField( GLGaeaServer* pGaeaServer );
    
    //�ʵ弭���� ���� �˻�
    bool CheckProduct_FieldServer( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic, BYTE& eState , const WORD nCheckCount = 1 );
	bool CheckProductMaterial_FieldServer( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic, BYTE& eState );

    bool ReqProduct( NET_MSG_GENERIC* nmg, GLChar* pChar );
    void CancleProduct(GLChar* pChar );
    bool LearnProductBook( NET_MSG_GENERIC* nmg, GLChar* pChar );
    bool TypeReset( NET_MSG_GENERIC* nmg, GLChar* pChar );

	bool NeedConfirmProductMaterial( DWORD nKey, GLChar* pChar );
    void DoProductMeterial( DWORD nKey, GLChar* pChar );
	bool DoProductResult( DWORD nKey, GLChar* pChar, GLMSG::SNET_PRODUCT_FA* pMsg );	

    DWORD GetCurrentProductRecipeKey(){ return m_dwCurrentProductRecipeKey; }
    WORD GetProductCount(){ return m_nProductCount; }

	void Reset();

    WORD  m_nProductCurrentCount;        // ���� �������� ����		(�������� �����)
	bool m_bConfirmMaterial;			 // ��� Ȯ�� ����			(�������� �����);

private:
    bool CheckTypeReset(NET_MSG_GENERIC* nmg, GLChar* pChar);
    void CallbackProduct();

    DWORD m_dwCurrentProductRecipeKey;  // ���� �������� ���չ�Ű
    WORD  m_nProductCount;	            // ��ǥ �������� ����
    GLGaeaServer* m_pGaeaServer;
};
#pragma once

#include "../../RanLogic/Product/GLProduct.h"
#include "../../RanLogic/Msg/ProductMsg.h"

class GLGaeaServer;
class GLChar;
struct NET_MSG_GENERIC;

//hsshin 제작 로직 필드
class GLProductField : public GLProduct
{
public:
	GLProductField( GLGaeaServer* pGaeaServer );
    
    //필드서버용 제작 검사
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

    WORD  m_nProductCurrentCount;        // 현재 제작진행 개수		(서버에만 사용중)
	bool m_bConfirmMaterial;			 // 재료 확인 여부			(서버에만 사용중);

private:
    bool CheckTypeReset(NET_MSG_GENERIC* nmg, GLChar* pChar);
    void CallbackProduct();

    DWORD m_dwCurrentProductRecipeKey;  // 현재 제작중인 조합법키
    WORD  m_nProductCount;	            // 목표 제작진행 개수
    GLGaeaServer* m_pGaeaServer;
};
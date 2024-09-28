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

	int m_iMakeCount;				// 일부 1개 이상의 아이템을 생성하는 제작이 있어 이를 세아려주는 변수

public:
	GLProductClient( GLGaeaClient* pGaeaClent  );
	virtual ~GLProductClient();

    //클라이언트용 제작 검사 함수
    bool        CheckProduct( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic, BYTE& eState, const WORD nCheckCount, bool bOngoing=false );    //제작 검사
	bool        CheckProductMaterial( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic, WORD wMultiProduct, BYTE& eState );    // 재료 검사;
    WORD        CountProduct( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic, BYTE& eState );    //종합적으로 검사했을 때 몇개 제작 가능한지 카운트

	WORD		GetCountProduct( DWORD nID	);															// CountProduct 함수를 스크립트에서 사용 가능하게 만드는 함수 

    // 제작분야와 착용클래스로 조합법을 검색 (해당 조합법 벡터를 정렬하여 리턴)
    bool        FindProductList( PtrVector_ProductRecipe& vecData, const BYTE eFindType, const BYTE eFindClass );

    //제작관련 메시지 출력
    void		ProductOutputText( BYTE eState, const SProductRecipe* sRecipe );				//제작 시스템 메시지 출력
    void		ProductOutputText( BYTE eState, WORD wKey );

    //제작 시스템관련 패킷 전송 요청 함수
	HRESULT		ReqProduct( DWORD nID, WORD nCount, bool bOngoing=false );						//제작 요청
	HRESULT		ReqProductCancel();																//제작 취소 요청
	HRESULT		ReqLearnProductBook( WORD wPosX, WORD wPosY );									//조합서 아이템 사용 요청
    HRESULT     ReqProductTypeAdd( WORD wProductType );											//제작분야 학습 요청
    HRESULT     ReqProductTypeDel( std::vector<int>& vecProductList, WORD wPosX, WORD wPosY );	//제작분야 삭제 요청
    HRESULT     ReqProductRecipeList();												            //습득 조합서 리스트 요청
    HRESULT     ReqProductTypeList();                               		                    //학습한 제작분야 리스트 요청
    HRESULT     ReqProductExpList();												            //조합으로 숙련도를 습득한 목록 요청
  
    //제작 시스템 관련 리시브 메시지 처리 함수
public:
    void        ProductMsgProcess( NET_MSG_GENERIC* pMsg );							            //제작 시스템 관련 메시지 분배 처리함수
protected:
    void        MsgProductFb( NET_MSG_GENERIC* pMsg );								            //제작 결과 메시지 처리
    void        MsgLearnProductBookFb( NET_MSG_GENERIC* pMsg );						            //조합서 아이템 사용 결과 메시지 처리
    void        MsgProductRecipeListAck( NET_MSG_GENERIC* pMsg );					            //습득한 조합서 리스트 요청 결과 메시지 처리
    void        MsgProductTypeListtAck( NET_MSG_GENERIC* pMsg );					            //학습한 제작분야 리스트 요청 결과 메시지 처리
    void        MsgProductExpListAck( NET_MSG_GENERIC* pMsg );						            //조합으로 숙련도를 습득한 목록 요청 결과 메시지 처리
    void        MsgProductTypeAddAck( NET_MSG_GENERIC* pMsg );						            //제작분야 학습 요청 결과 메시지 처리           
    void        MsgProductTypeDelAck( NET_MSG_GENERIC* pMsg );						            //제작분야 학습 요청 결과 메시지 처리           

public:

	lua_tinker::table	GetStudiedProducts();																				// 배운 제작 TYPE들;
	lua_tinker::table	GetProductList( const BYTE eFindType, const BYTE eFindClass, const bool bShowProductAble = false );	// 제작목록 가져오기;

	lua_tinker::table	GetSelectedProductData( const unsigned int nScrollPos );											// 선택항목 데이타(재료);
	lua_tinker::table	GetSelectedProductRequirment( const unsigned int nScrollPos );										// 선택항목 요구조건텍스트데이타;
	std::string 		GetSelectedProductName( const unsigned int nScrollPos );											// 선택항목 아이템 이름;
	const DWORD 		GetSelectedProductCost( const unsigned int nScrollPos );											// 선택항목 비용;
	lua_tinker::table	GetSelectedProductResultData( const unsigned int nScrollPos );												// 선택항목 ID;
	const DWORD			GetSelectedProductKey( const unsigned int nScrollPos );												// 선택항목 key;
	const FLOAT			GetSelectedProductDelay( const unsigned int nScrollPos );											// 선택항목 제작시간;
	const FLOAT			GetSelectedProductRate( const unsigned int nScrollPos );											// 선택항목 제작 확률;

	const BYTE			GetStudyState( WORD wProductType );																	// 제작분야 학습유무;

	void				ReqProductTypeDelete( lua_tinker::table tbProductList, WORD wPosX, WORD wPosY );					// 제작분야 삭제 요청;

protected:
	void				StartProduct( const char* cItemName, int nCount, float fDelaySec );									// 프로그래스바 열기;
	void				EndProduct();																						// 프로그래스바 닫기;

private:
	void				FilteringProduct();																					// 제작목록 중 제작가능항목 필터링;
	PtrVector_ProductRecipe	m_PtrVectorProductRecipe;
};

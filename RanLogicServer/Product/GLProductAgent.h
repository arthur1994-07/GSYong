#pragma once

#include "../../RanLogic/Product/GLProduct.h"
#include "../../RanLogic/Product/GLProductRecipeMan.h"
#include "../../RanLogic/GLogicData.h"

#include "./GLProductDataTypeServer.h"

class GLAgentServer;
class GLCharAG;

class GLProductAgent : public GLProduct
{
public:
    GLProductAgent();
    virtual ~GLProductAgent(){};

public:
    //������Ʈ������ ���� �˻�
    bool CheckProduct_AgentServer(const WORD wLevel, const SProductRecipe* pProductRecipe, BYTE& eState );
    int IsProduct( DWORD nKey, GLAgentServer* pServer, GLCharAG* pChar );

public:
    //! ���ռ�
    bool IsUsableBook( DWORD nItemID );
    void SetBookList( const std::vector<DWORD>& vecBookList, GLAgentServer* pServer, GLCharAG* pChar );

public:
    //! ���պо� 
    void SetTypeList( const std::vector<WORD>& vecType, GLAgentServer* pServer, GLCharAG* pChar );
    void AddType( int nType, GLAgentServer* pServer, GLCharAG* pChar );
    bool IsDelType( const std::vector<WORD>& m_vecProductType, GLAgentServer* pServer, GLCharAG* pChar );

public:
    //! ����ġ����
    void SetIsGetExpList( const std::vector<WORD>& vecIsGetExp, GLAgentServer* pServer, GLCharAG* pChar, bool bFirst, bool bLast );
    void AddIsGetExp( DWORD dwID, GLAgentServer* pServer, DWORD dwChaDbNum );

public:
    //! Ŭ���̾�Ʈ���� ����
    void MegSendBookList( GLAgentServer* pServer, GLCharAG* pChar );
    void MsgSendTypeList( GLAgentServer* pServer, GLCharAG* pChar );
    void MsgSendIsGetExpList( GLAgentServer* pServer, GLCharAG* pChar, bool bDBMsg, bool bFirst, bool bLast );
    void MsgSendAdd( WORD nType, bool bOK, GLAgentServer* pServer, GLCharAG* pChar );

    //! �ʵ忡�� ���� �޽���
    int  ProductSuccess( const DWORD RecipeID, GLAgentServer* pServer, DWORD ChaDbNum );
    void LearnBookSuccess( DWORD dwBookID );
    void TypeDeleteSuccess( const std::vector<WORD>& vecProductType, GLAgentServer* pServer, DWORD ChaDbNum );
	void GenerateItemSuccess( const DWORD RecipeID, GLAgentServer* pServer, DWORD ChaDbNum, bool bExp );
	void GenerateItemFail( const DWORD RecipeID, GLAgentServer* pServer, DWORD ChaDbNum );

public:
	void LogProductExchangeMaterial( PRODUCT_EXCHANGE_LOG productLogData, GLAgentServer* pServer, DWORD ChaDbNum );
	void LogProductExchange( const DWORD RecipeID, LONGLONG llMoney, GLAgentServer* pServer, DWORD ChaDbNum, bool bExp, __int64 iResultItemMakeNum );

private:
    bool IsLoadedBook( GLAgentServer* pServer, DWORD dwChaDbNum );
    bool IsLoadedType( GLAgentServer* pServer, DWORD dwChaDbNum );
    bool IsLoadedIsGetExp( GLAgentServer* pServer, DWORD dwChaDbNum );
   
    bool m_bIsLoadedBook;
    bool m_bIsLoadedType;
    bool m_bIsLoadedIsGetExp;
	bool m_bIsLoadingIsGetExp;
};
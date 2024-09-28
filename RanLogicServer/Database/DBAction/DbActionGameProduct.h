#pragma once

#include "DbAction.h"

namespace db
{
    class ProductGetType: public DbAction
    {
    public:
        ProductGetType(DWORD dwChaDbNum);
        virtual ~ProductGetType() {};
        virtual int Execute(NetServer* pServer) override;
    protected:
        DWORD           m_dwChaDbNum;        
    };

    class ProductAddType: public DbAction
    {
    public:
        ProductAddType(DWORD dwChaDbNum, int nProdutType);
        virtual ~ProductAddType() {};
        virtual int     Execute(NetServer* pServer) override;
    protected:
        DWORD           m_dwChaDbNum;
        int             m_nProdutType;
    };

    class ProductDelType: public DbAction
    {
    public:
        ProductDelType(DWORD dwChaDbNum, int nProdutType);
        virtual ~ProductDelType() {};
        virtual int     Execute(NetServer* pServer) override;
    protected:
        DWORD           m_dwChaDbNum;
        int             m_nProdutType;
    };


    class ProductIsGetExpSelect: public DbAction
    {
    public:
        ProductIsGetExpSelect(DWORD dwChaDbNum);
        virtual ~ProductIsGetExpSelect() {};
        virtual int Execute(NetServer* pServer) override;
    protected:
        DWORD           m_dwChaDbNum;
    };

    class ProductIsGetExpInsert: public DbAction
    {
    public:
        ProductIsGetExpInsert(DWORD dwChaDbNum, DWORD dwProductID);
        virtual ~ProductIsGetExpInsert() {};
        virtual int Execute(NetServer* pServer) override;
    protected:
        DWORD           m_dwChaDbNum;
        DWORD           m_dwProductID;
    };

    class ProductIsGetExpDelete: public DbAction
    {
    public:
        ProductIsGetExpDelete(DWORD dwChaDbNum,
			const std::vector<PRODUCT_ITEM>& vecProductID );
        virtual ~ProductIsGetExpDelete() {};
        virtual int Execute(NetServer* pServer) override;
    protected:
        DWORD						m_dwChaDbNum;
        std::vector<PRODUCT_ITEM>	m_vecProductID;
    };

    class ProductBookSave: public DbAction
    {
    public:
        ProductBookSave(DWORD dwChaDbNum, const std::vector<DWORD>& setLearnProductBook);
        virtual ~ProductBookSave() {};
        virtual int Execute(NetServer* pServer) override;
    protected:
        DWORD           m_dwChaDbNum;
        std::vector<DWORD> m_vecLearnProductBook;
    };

    class ProductBookLoad: public DbAction
    {
    public:
        ProductBookLoad(DWORD dwChaDbNum);
        virtual ~ProductBookLoad() {};
        virtual int Execute(NetServer* pServer) override;
    protected:
        DWORD           m_dwChaDbNum;
    };

    class ProductBookDelete: public DbAction
    {
    public:
        ProductBookDelete(DWORD dwChaDbNum, const std::vector<PRODUCT_ITEM>& vecBookID );
        virtual ~ProductBookDelete() {};
        virtual int Execute(NetServer* pServer) override;
    protected:
        DWORD           m_dwChaDbNum;
        std::vector<PRODUCT_ITEM> m_vecBookID;
    };

	class CharProductLog : public DbAction
	{
	public:
		CharProductLog( const CHAR_PRODUCT_LOG& productLog );
		virtual ~CharProductLog() {};
		virtual int Execute( NetServer* pServer ) override;
	protected:
		CHAR_PRODUCT_LOG m_productLog;        
	};

	class ProductExchangeLog : public DbAction
	{
	public:
		ProductExchangeLog( const PRODUCT_EXCHANGE_LOG& exchangeLog );
		virtual ~ProductExchangeLog() { };
		virtual int Execute( NetServer* pServer ) override;

	protected:
		PRODUCT_EXCHANGE_LOG m_exchangeLog;
	};

} // namespace db
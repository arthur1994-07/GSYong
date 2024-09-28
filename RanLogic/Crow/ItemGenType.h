#pragma once

// 발생 정보 타입
enum EMGENINFOTYPE
{
    EMGENINFOTYPE_NONE                = 0, // 없음
    EMGENINFOTYPE_ONLY_QITEM          = 1, // Q 아이템 전용
    EMGENINFOTYPE_ONLY_MONEY          = 2, // 금액 전용
    EMGENINFOTYPE_ONLY_ITEMSKILLFACT  = 3, // 버프 전용
};

// 발생 아이템 타입
enum EMGENITEMTYPE
{
    EMGENITEMTYPE_ITEM          = 0,
    EMGENITEMTYPE_QITEM         = 1,
    EMGENITEMTYPE_MONEY         = 2, 
    EMGENITEMTYPE_ITEMSKILLFACT = 3,
};

// 발생 결과 타입
enum EMGENRESULTTYPE
{
    EMGENRESULTTYPE_NULL  = 0,
    EMGENRESULTTYPE_ITEM  = 1,
    EMGENRESULTTYPE_MONEY = 2,
};

// 발생 결과
//--------------------------------------------------------------------------------------------
struct SGENRESULT
{
    EMGENRESULTTYPE emType;

    union
    {
        struct { DWORD     dwMoney; }; //! 금액
        struct { SNATIVEID sItemID; }; //! 아이템 ID

    } Data;

    SGENRESULT() 
        : emType(EMGENRESULTTYPE_NULL)
    {
        ZeroMemory( &Data, sizeof(Data) );
    }

    const bool IsNull() 
    { 
        return (emType == EMGENRESULTTYPE_NULL); 
    }
};

// 발생할 아이템 갯수 정보
//--------------------------------------------------------------------------------------------
struct SGENNUM
{
    WORD   wNum;
    float  fRate;

    SGENNUM()
        : wNum(0)
        , fRate(0.0f)
    {
    }
};

// 발생할 아이템 정보
//--------------------------------------------------------------------------------------------
struct GLCHARLOGIC;

struct SGENITEMBASE
{
    EMGENITEMTYPE   emType;
    float           fRate;
    WORD            wGrade;

    SGENITEMBASE( EMGENITEMTYPE _emType )
        : emType(_emType)
        , fRate(0.0f)
        , wGrade(0)
    {
    }

    virtual ~SGENITEMBASE() 
    {
    }

    struct LESS : public std::binary_function<SGENITEMBASE, SGENITEMBASE, const bool> 
    {
        const bool operator () ( const SGENITEMBASE* plhs, const SGENITEMBASE* prhs ) const
        {
            return plhs->fRate < prhs->fRate;
        }
    };

    virtual const bool IsGen( GLCHARLOGIC* pChar ) { return (fRate != 0.0f); }
    virtual void       GetResult( OUT SGENRESULT& sResult ) = 0;
};

// 발생할 아이템 군 정보
//--------------------------------------------------------------------------------------------
struct SGENITEMSPEC : public SGENITEMBASE
{
    DWORD dwSpecID;

    SGENITEMSPEC () 
        : SGENITEMBASE( EMGENITEMTYPE_ITEM )
        , dwSpecID(UINT_MAX)
    {
    }

    virtual void GetResult( OUT SGENRESULT& sResult );
};

// 발생할 아이템 ID 정보
//--------------------------------------------------------------------------------------------
struct SGENITEMID : public SGENITEMBASE
{
    SNATIVEID sItemID;

    SGENITEMID () 
        : SGENITEMBASE( EMGENITEMTYPE_ITEM )
        , sItemID(SNATIVEID(false))
    {
    }

    virtual void GetResult( OUT SGENRESULT& sResult );
};


// 발생할 Q아이템 ID 정보
//--------------------------------------------------------------------------------------------
struct SGENQITEMID : public SGENITEMBASE
{
    SNATIVEID sQItemID;

    SGENQITEMID () 
        : SGENITEMBASE( EMGENITEMTYPE_QITEM )
        , sQItemID(SNATIVEID(false))
    {
    }

    virtual void GetResult( OUT SGENRESULT& sResult );
};


// 발생할 금액 정보
//--------------------------------------------------------------------------------------------
struct SGENMONEY : public SGENITEMBASE
{
    DWORD dwMoney;

    SGENMONEY () 
        : SGENITEMBASE( EMGENITEMTYPE_MONEY )
        , dwMoney(0)
    {
    }

    virtual void GetResult( OUT SGENRESULT& sResult );
};

// 발생할 아이템 군 정보 ( 특정 버프가 걸렸을 경우만 드롭되는 아이템 )
//--------------------------------------------------------------------------------------------
struct SGENITEMSPECSKILLFACT : public SGENITEMBASE
{
    SNATIVEID sSkillID;
    DWORD	  dwSpecID;

    SGENITEMSPECSKILLFACT () 
        : SGENITEMBASE( EMGENITEMTYPE_ITEMSKILLFACT )
        , sSkillID(SNATIVEID(false))
        , dwSpecID(UINT_MAX)
    {
    }

    virtual const bool IsGen( GLCHARLOGIC* pChar );
    virtual void       GetResult( OUT SGENRESULT& sResult );
};

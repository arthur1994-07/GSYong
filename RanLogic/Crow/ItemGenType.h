#pragma once

// �߻� ���� Ÿ��
enum EMGENINFOTYPE
{
    EMGENINFOTYPE_NONE                = 0, // ����
    EMGENINFOTYPE_ONLY_QITEM          = 1, // Q ������ ����
    EMGENINFOTYPE_ONLY_MONEY          = 2, // �ݾ� ����
    EMGENINFOTYPE_ONLY_ITEMSKILLFACT  = 3, // ���� ����
};

// �߻� ������ Ÿ��
enum EMGENITEMTYPE
{
    EMGENITEMTYPE_ITEM          = 0,
    EMGENITEMTYPE_QITEM         = 1,
    EMGENITEMTYPE_MONEY         = 2, 
    EMGENITEMTYPE_ITEMSKILLFACT = 3,
};

// �߻� ��� Ÿ��
enum EMGENRESULTTYPE
{
    EMGENRESULTTYPE_NULL  = 0,
    EMGENRESULTTYPE_ITEM  = 1,
    EMGENRESULTTYPE_MONEY = 2,
};

// �߻� ���
//--------------------------------------------------------------------------------------------
struct SGENRESULT
{
    EMGENRESULTTYPE emType;

    union
    {
        struct { DWORD     dwMoney; }; //! �ݾ�
        struct { SNATIVEID sItemID; }; //! ������ ID

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

// �߻��� ������ ���� ����
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

// �߻��� ������ ����
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

// �߻��� ������ �� ����
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

// �߻��� ������ ID ����
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


// �߻��� Q������ ID ����
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


// �߻��� �ݾ� ����
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

// �߻��� ������ �� ���� ( Ư�� ������ �ɷ��� ��츸 ��ӵǴ� ������ )
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

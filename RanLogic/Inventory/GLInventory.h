#pragma once

#include <map>
#include <boost/serialization/map.hpp>

#include "../../SigmaCore/String/MinGuid.h"
#include "../../EngineLib/Common/ByteStream.h"
#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../Item/GLItem.h"

struct SITEMCUSTOM;
struct SINVENITEM;
struct SITEM;

enum
{
	EInvenX				= 6,
	EInvenY				= 10,
	EInvenPage			= 5, // ���� �κ� ������ 3������ 5���� ���� 2015.01.26

	EInvenPremiumLine	= 1, // ���� �κ� ������ 3������ 5���� �����ϸ鼭 �����̾� ȿ�� ���濡 ���� ����
	EInvenTotalLine		= EInvenY * EInvenPage,

	EInven1PageSlot		= EInvenX * EInvenY,
	EInvenTotalSlot		= EInven1PageSlot * EInvenPage,
};

enum
{
	EInvenBarrierEmpty	= 0x00,	// �� ����
	EInvenBarrierUsed	= 0x01,	// ������ ����
};

// 
// struct sTruct;
// 
// typedef boost::multi_index::multi_index_container
// < 
//     sTruct, 
//     boost::multi_index::indexed_by
//     < 
//         boost::multi_index::hashed_non_unique
//         < 
//             boost::multi_index::member
//             < 
//                 sTruct,	std::string, &sTruct::name 
//             > 
//         >, 
//         boost::multi_index::hashed_non_unique
//         < 
//             boost::multi_index::member
//             < 
//                 sTruct, int, &sTruct::age 
//             > 
//         > 
//     > 
// > person_multi; 
// 


struct sInvenCoordinate
{
    WORD first;
    WORD second;
    sInvenCoordinate() : first(0), second(0){}
    sInvenCoordinate( WORD wX_, WORD wY_)
        : first( wX_ )
        , second( wY_ )
    {}
    bool operator< (const sInvenCoordinate &t ) const
    {   
        // Y���� �������� �տ������� �Ҹ�ǵ��� �Ѵ�.
        if( t.second > second)
            return true;
        else
            if( t.second == second )
                if( t.first > first )
                    return true;                

        return false;
    }

private:
    friend class boost::serialization::access;
    template <typename Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & first;
        ar & second;
    }
};
class GLInventory : private boost::noncopyable
{
private:
	class Barrier
	{
    private:
        enum{ NotValid = -1, };

        int IsValid( const WORD nPosX, const WORD nPosY ) const
        {
            if( m_wCellSX <= nPosX)
                return NotValid;
            if( m_wCellSY <= nPosY)
                return NotValid;

            const unsigned int nIndex = nPosX + (m_wCellSX * nPosY);
            if( nIndex < m_vecByte.size() )
                return nIndex;

            return NotValid;
        }

        std::vector<BYTE> m_vecByte;
        WORD m_wCellSX;
        WORD m_wCellSY;

	public:
		Barrier( const WORD _sizeX, const WORD _sizeY )
			: m_wCellSX( _sizeX )
			, m_wCellSY( _sizeY )
		{
			m_vecByte.clear();
			m_vecByte.reserve(_sizeX*_sizeY);

            for( int i = 0 ; i < m_wCellSX*m_wCellSY ; i++ )
				m_vecByte.push_back( EInvenBarrierEmpty );
		}

		void SetBarrier( const BYTE nType, const WORD nPosX, const WORD nPosY )
		{
			int nIndex = IsValid( nPosX, nPosY );
			
			if( nIndex != NotValid )
                m_vecByte[nIndex] = nType;
		}

		void ResetBarrier( const BYTE nType, const WORD nPosX, const WORD nPosY )
		{
			int nIndex = IsValid( nPosX, nPosY );

			if( nIndex != NotValid )
                m_vecByte[nIndex] &= ~nType;
		}

		bool GetInvenState( const WORD nPosX, const WORD nPosY, BYTE& nType ) const
		{
			int nIndex = IsValid( nPosX, nPosY );

			if( nIndex != NotValid )
			{
				nType = m_vecByte[nIndex];
				return true;
			}
			
			return false;
		}

		void Clear()
		{
			m_vecByte.clear();

            for( int i = 0 ; i < m_wCellSX*m_wCellSY ; i++ )
                m_vecByte.push_back( EInvenBarrierEmpty );
		}

	private:
		friend class boost::serialization::access;
		template <typename Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & m_vecByte;
		}
	};


public:
	//typedef std::pair<WORD, WORD>           CELL_KEY;
    typedef sInvenCoordinate                CELL_KEY;
	typedef std::map<CELL_KEY, SINVENITEM*> CELL_MAP;
	typedef CELL_MAP::iterator              CELL_MAP_ITER;    
	typedef CELL_MAP::const_iterator        CELL_MAP_CITER;
    typedef CELL_MAP::reverse_iterator      CELL_MAP_RITER;    
    typedef CELL_MAP::value_type            CELL_MAP_VALUE;

protected:
	WORD		m_wCellSX;
	WORD		m_wCellSY;
	WORD		m_wADDLINE;
	bool		m_bLimit;

	CELL_MAP	m_ItemMap;

	//BYTE		m_btBarrier[ EInvenX ][ EInvenTotalLine ];
	Barrier		m_Barrier;

public:
	GLInventory();
	GLInventory( WORD _sizeX, WORD _sizeY );
	virtual ~GLInventory();

// private:
// 	GLInventory( const GLInventory &Inven ) { GASSERT( 0 && "������ ���� ����!" ); }
// 	GLInventory& operator = ( GLInventory &Inven ) { GASSERT( 0 && "������ ���� ����!" ); return *this; }

public:
	virtual void Assign( const GLInventory &Inven );
	void SetState( WORD wCellSX, WORD wCellSY );
	void SetAddLine( WORD wLine, bool bLimit );
	void SetItemGenTime();

protected:
	void SetBarrier( BYTE nType, WORD nPosX, WORD nPosY );
	void ResetBarrier( BYTE nType, WORD nPosX, WORD nPosY );

public:
	WORD GetSizeX() { return m_wCellSX; }
	WORD GetSizeY() { return m_wCellSY; }

public:
    BOOL IsInsertable( const WORD wPosX, const WORD wPosY, bool bLOAD=false );

public:
	SINVENITEM* FindPosItem( const WORD wPosX, const WORD wPosY );
	SINVENITEM* FindItemByGenNumber( sc::MGUID& guid/*LONGLONG llGenNum, SNATIVEID sID, BYTE cGenType*/ );
    BOOL FindInsertable( WORD &wPosX, WORD &wPosY, bool bAllLine=false );
	BOOL HaveEventItem();

	BOOL ValidPileInsrt( const WORD wINSRTNUM, const SNATIVEID &sNID, const WORD wPILENUM, bool bAllLine=false );
	BOOL ValidCheckInsrt( const WORD wNum, bool bAllLine );

	DWORD SimulateDelSlotCnt( const SNATIVEID _sNID, WORD wDelCount );


    //������ ���� �˻�
	DWORD CountItem( const SNATIVEID& sNID );   //�ش� ������ ����
    DWORD CountItem( EMITEM_TYPE emType );  //�ش� Ÿ�� ����
    DWORD CountTurnItem( const SNATIVEID& sNID );   //�ش� �������� ��밡�ɼ���
    DWORD CountTurnItem( EMITEM_TYPE emType );  //�ش� Ÿ���� ��밡�� ����
	DWORD CountTurnItem( EMITEM_TYPE emType, ITEM_VALUE typeValue, int _value );  //�ش� Ÿ���� ��밡�� ����
	DWORD CountDrugItem( EMITEM_DRUG emDrug );


    /** �ش� ������ ���� ���� ( ������ ����ϱ� ���� �Լ��̴�. )
        �� ���ܰ� �ִµ� ��ħ ���ɰ������� �ʰ��� ������ �ִ� ��ħ ���ɰ����� ����Ѵ�.
        999/30 ���� 30���� ����Ѵ�.
        30/30 ��쿡�� 30���� ����Ѵ�.
        29/30 ��쿡�� 29���� ����Ѵ�.
        1/30 ��쿡�� 1���� ����Ѵ�.
        �̷��� �Ͽ� �� ������ ����Ѵ�.  
    */
    DWORD CountPileItem( const SNATIVEID& sNID );

	SINVENITEM* FindItem( const SNATIVEID& sNID, int index=0 );
	SINVENITEM* FindItem( const EMITEM_TYPE emTYPE );
	SINVENITEM* FindItem( const EMITEM_TYPE emTYPE, const ITEM_VALUE _typeValue, int _value );
	SINVENITEM* FindItem( const EMITEM_TYPE emTYPE, const SNATIVEID& sNID );
	SINVENITEM* FindItem( const SITEMCUSTOM& rItemCustom );
	SINVENITEM* FindDrugItem( EMITEM_DRUG emDrug );
	SINVENITEM* FindItemBack( const WORD wBackX, const WORD wBackY );

public:
    //!	�ε�(Assign) �ÿ��� ���� ���ΰ� ������ �־���.
    //! LoadCheck �ÿ� ������ ã�� �������� ������ �ٸ��� �Ѵ�.
	BOOL InsertItem(
        const SITEMCUSTOM& ItemCustom,
        const WORD wPosX,
        const WORD wPosY,
        bool bLOAD=false,
		bool bNew = false );

	SINVENITEM* InsertItemNew( const SITEMCUSTOM& ItemCustom, const WORD wPosX, const WORD wPosY, bool bLOAD = false );

	BOOL InsertItem(
        const SITEMCUSTOM& ItemCustom,
        WORD wPosX,
        WORD wPosY,
        WORD wBackX,
        WORD wBackY);

    //! �������� �ִ´�. ���� ��ġ�� �𸦶�
    BOOL InsertItem( const SITEMCUSTOM& ItemCustom, bool Force=false);
    BOOL InsertItemGetPos( const SITEMCUSTOM& ItemCustom, WORD& wOutPosX, WORD& wOutPosY, bool Force=false);  //���Ե� ��ġ�� �����ش�.
    BOOL InsertItemGetInvenItem( const SITEMCUSTOM& ItemCustom, SINVENITEM& pInvenItem, bool Force=false ); //���ε� �κ��������� �����ش�.
	
	//! �������� ��ģ��/���ļ� ����
	BOOL InsertItemEx( const SITEMCUSTOM &ItemCustom );
    BOOL InsertItemExGetInvenItem( const SITEMCUSTOM &ItemCustom , std::vector<SINVENITEM>& vecItem );  //���Ե� �κ������� ���� ������

	SINVENITEM* GetItem( WORD wPosX, WORD wPosY );
    SINVENITEM* GetItem( WORD wPosX, WORD wPosY ) const;

public:
	BOOL DeleteItem( WORD wPosX, WORD wPosY );
    //BOOL DeleteItem( WORD wPosX, WORD wPosY, LONGLONG GenNum );
	void DeleteItemAll();
	void DeleteItem( const SNATIVEID& sNID, DWORD& dwDeletedCount );

public:
	//GLInventory::CELL_MAP* GetItemList() { return &m_ItemMap; }
	const GLInventory::CELL_MAP* GetItemList() const { return &m_ItemMap; }
	bool IsEmpty() const { return m_ItemMap.empty(); }

	DWORD GetNumItems() { return static_cast< DWORD >( m_ItemMap.size() ); }
	DWORD GetAmountDrugItem( EMITEM_DRUG emDrug );

	//! �κ��丮 ��������
    DWORD GetAmountEmpty( bool bAllLine );
    //���� �κ��丮�� ���� ���ִ� �ش� ��ħ ���� �������� �ִ� ������ ����
    DWORD GetAmountEmptyPile( const SNATIVEID& sNID, bool bAllLine );

	bool GetCharResetItem( WORD &wPosX, WORD &wPosY );

	//	��ħ ���� ���� ��ŭ ���� �ִ� ���� ��ȯ.
	bool GetPileItem( const SNATIVEID _sNID, WORD &wPosX, WORD &wPosY );

	WORD GetValidCellY( bool bAllLine ) const;
	WORD GETAddLine() const;
	bool IsLimit() const { return m_bLimit; }

public:
	DWORD		GetItemDataSize();
	BOOL		GetItemData( se::SerializeWritableData& data );
	DWORD		GetItemStorageDataSize();
	BOOL		GetItemStorageData( se::SerializeWritableData& data );
	BOOL		SETITEM_BYBUFFER( se::ByteStream &ByteStream );
	BOOL		GETITEM_BYBUFFER( se::ByteStream &ByteStream ) const;
	BOOL		GETITEM_BYBUFFER_FORSTORAGE( se::ByteStream &ByteStream ) const;
	
	BOOL SET_ITEM_BY_VECTOR( const std::vector< SINVENITEM_SAVE >& vecItem );
	BOOL GET_ITEM_BY_VECTOR( std::vector< SINVENITEM_SAVE >& vecItem );

	BOOL SETITEM_BYBUFFER_FOR_MIGRATION( se::ByteStream &ByteStream );

	// Ż�� ������ �����ɼ� ��ȿ�� �˻�
	BOOL ValidateVehicleRandomOption();

	// ������ �ִ� ���� ������ �˻� - �±�
	BOOL ValidateWrappedItem( std::vector< SITEMCUSTOM >& vecPostItems, std::vector< SINVENITEM* >& vecUnWrappedItems );

	// OP ������ ��ȿ�� �˻�
	BOOL ValidateRandomOption( std::vector< SITEMCUSTOM >& vecDeleteItems );

public:
    /**
        ��ĥ �� �ִ� �����۵���
        ���ļ� �κ��丮 ������ ����ȭ ��Ų��.
        ������ �κ� ������ ȣ�� �� ȣ��Ǹ� �׿� �ʿ�� ��Ȱ�� �� �� �ִ�.

     */
    void OptimalPile();

	void OptimalPileServer( std::vector< DELETED_ITEM >& DeletedItems );

    /**
        �����۵��� �κ��� �������� ���� �Ѵ�.

     */
    void Aligning();

    /**
        ��ĥ �� �ִ� �������� ���� �ϴ��� Ȯ�� �Ѵ�.
        ���� ���� �������� ������ �ε�
        �̹� ���������� �ִ�� ������ �ִٸ� false �� �ʿ� ���� �Ѵ�.
        ���� ���������� �ִ�� ������ ���� �ʴٸ� true �� �ȴ�.

     */
    bool HasPileItem();

	BOOL GetInvalidInventoryLineItem( std::vector< SINVENITEM* >& vecInvenItems, bool bUsePremium );

protected:
    void RecusiveOptimalPile(CELL_MAP& invenMap);
	void RecusiveOptimalPileServer( CELL_MAP& invenMap, std::vector< DELETED_ITEM >& DeletedItems );
	bool RecusiveHasPileItem(CELL_MAP& invenMap);
    bool IsPileItem(const SITEM* pInvenData, const SITEMCUSTOM& sItem);

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & m_wCellSX;
		ar & m_wCellSY;
		ar & m_wADDLINE;
		ar & m_bLimit;

		ar & m_ItemMap;

		//ar & m_btBarrier;
        ar & m_Barrier;
	}
};

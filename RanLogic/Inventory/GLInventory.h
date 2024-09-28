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
	EInvenPage			= 5, // 기존 인벤 페이지 3개에서 5래로 변경 2015.01.26

	EInvenPremiumLine	= 1, // 기존 인벤 페이지 3개에서 5래로 변경하면서 프리미엄 효과 변경에 따른 수정
	EInvenTotalLine		= EInvenY * EInvenPage,

	EInven1PageSlot		= EInvenX * EInvenY,
	EInvenTotalSlot		= EInven1PageSlot * EInvenPage,
};

enum
{
	EInvenBarrierEmpty	= 0x00,	// 빈 공간
	EInvenBarrierUsed	= 0x01,	// 아이템 점유
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
        // Y축을 기준으로 앞에서부터 소모되도록 한다.
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
// 	GLInventory( const GLInventory &Inven ) { GASSERT( 0 && "묵시적 복사 불허!" ); }
// 	GLInventory& operator = ( GLInventory &Inven ) { GASSERT( 0 && "묵시적 복사 불허!" ); return *this; }

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


    //아이템 수량 검사
	DWORD CountItem( const SNATIVEID& sNID );   //해당 아이템 수량
    DWORD CountItem( EMITEM_TYPE emType );  //해당 타입 수량
    DWORD CountTurnItem( const SNATIVEID& sNID );   //해당 아이템의 사용가능수량
    DWORD CountTurnItem( EMITEM_TYPE emType );  //해당 타입의 사용가능 수량
	DWORD CountTurnItem( EMITEM_TYPE emType, ITEM_VALUE typeValue, int _value );  //해당 타입의 사용가능 수량
	DWORD CountDrugItem( EMITEM_DRUG emDrug );


    /** 해당 아이템 소지 수량 ( 슬롯을 계산하기 위한 함수이다. )
        단 예외가 있는데 겹침 가능개수가를 초과한 개수는 최대 겹침 가능개수로 계산한다.
        999/30 경우는 30개로 계산한다.
        30/30 경우에는 30개로 계산한다.
        29/30 경우에는 29개로 계산한다.
        1/30 경우에는 1개로 계산한다.
        이렇게 하여 총 갯수를 계산한다.  
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
    //!	로드(Assign) 시에는 가용 라인과 별개로 넣어짐.
    //! LoadCheck 시에 파일을 찾지 못했을때 리턴을 다르게 한다.
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

    //! 아이템을 넣는다. 넣을 위치를 모를때
    BOOL InsertItem( const SITEMCUSTOM& ItemCustom, bool Force=false);
    BOOL InsertItemGetPos( const SITEMCUSTOM& ItemCustom, WORD& wOutPosX, WORD& wOutPosY, bool Force=false);  //삽입된 위치를 돌려준다.
    BOOL InsertItemGetInvenItem( const SITEMCUSTOM& ItemCustom, SINVENITEM& pInvenItem, bool Force=false ); //삽인된 인벤아이템을 돌려준다.
	
	//! 아이템을 겹친다/겹쳐서 삽입
	BOOL InsertItemEx( const SITEMCUSTOM &ItemCustom );
    BOOL InsertItemExGetInvenItem( const SITEMCUSTOM &ItemCustom , std::vector<SINVENITEM>& vecItem );  //삽입된 인벤아이템 벡터 돌려줌

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

	//! 인벤토리 남은공간
    DWORD GetAmountEmpty( bool bAllLine );
    //현재 인벤토리에 넣을 수있는 해당 겹침 가능 아이템의 최대 갯수를 구함
    DWORD GetAmountEmptyPile( const SNATIVEID& sNID, bool bAllLine );

	bool GetCharResetItem( WORD &wPosX, WORD &wPosY );

	//	겹침 가능 갯수 많큼 겹쳐 있는 것을 반환.
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

	// 탈것 아이템 랜덤옵션 유효성 검사
	BOOL ValidateVehicleRandomOption();

	// 겹쳐져 있는 포장 아이템 검사 - 태국
	BOOL ValidateWrappedItem( std::vector< SITEMCUSTOM >& vecPostItems, std::vector< SINVENITEM* >& vecUnWrappedItems );

	// OP 아이템 유효성 검사
	BOOL ValidateRandomOption( std::vector< SITEMCUSTOM >& vecDeleteItems );

public:
    /**
        겹칠 수 있는 아이템들을
        겹쳐서 인벤토리 공간을 최적화 시킨다.
        유저가 인벤 정렬을 호출 시 호출되며 그외 필요시 재활용 할 수 있다.

     */
    void OptimalPile();

	void OptimalPileServer( std::vector< DELETED_ITEM >& DeletedItems );

    /**
        아이템들을 인벤의 앞쪽으로 정렬 한다.

     */
    void Aligning();

    /**
        겹칠 수 있는 아이템이 존재 하는지 확인 한다.
        만약 동일 아이템이 여러개 인데
        이미 순차적으로 최대로 겹쳐져 있다면 false 가 됨에 주의 한다.
        만약 순차적으로 최대로 겹쳐져 있지 않다면 true 가 된다.

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

#pragma once

namespace sc 
{
	class BaseStream;
	class SerialFile;
}

namespace se
{
	class ByteStream;
}

#define COSSDATA COSTUMESTATSDATA::CCostumeStatData::GetInstance()

#define COSS_GN_MAX     COSSDATA.GetShareMax(COSTUMESTATSDATA::CCostumeStatData::E_SHARE_GN_TYPE)
#define COSS_VAR_MAX    COSSDATA.GetShareMax(COSTUMESTATSDATA::CCostumeStatData::E_SHARE_VAR_TYPE)
#define COSS_ADDED_MAX  COSSDATA.GetShareMax(COSTUMESTATSDATA::CCostumeStatData::E_SHARE_ADDED_TYPE)

namespace COSTUMESTATSDATA
{

// �ڽ�Ƭ Parts Data
typedef struct _sPsData
{
  //COSTUME_STATS	 eStatIndex;	// �ɷ�ġ �ε���
  WORD			 wMaxPt;		// �ִ� ����Ʈ
  double		 fInc;			// 1����Ʈ�� ������/������

  _sPsData() : /*eStatIndex( E_NONE ),*/ wMaxPt(0), fInc(0) {};
  _sPsData( WORD wValue, double fValue ) : wMaxPt(wValue), fInc(fValue) {};
  ~_sPsData(){};
  
} sPsData, *pPsData;

class CBasePartsDt
{
public :
	CBasePartsDt() { m_ePart = SUIT_NSIZE; }
	CBasePartsDt( EMSUIT eParts );
	virtual ~CBasePartsDt(){};

	virtual bool ParseData( int iPart, std::string strName ) { return false; }

protected :
	EMSUIT	m_ePart;
};

class CPartsDt : public CBasePartsDt
{

public :
	CPartsDt() {};
	CPartsDt( EMSUIT eParts );
	virtual ~CPartsDt(){};

	typedef std::vector<WORD>	vLvPoint;
	typedef vLvPoint::iterator  _iter_LvPt;

	typedef std::vector<sPsData>	vPsData;
	typedef vPsData::iterator		_iter_PsData;

	//typedef std::map<COSTUME_STATS, vPsData> mPsData;
	typedef std::map<COSTUME_STATS, sPsData> mPsData;
	typedef mPsData::iterator				 m_PsIter;

private :
	
	mPsData	 m_mData;	// ������ �ɷ�ġ ��������
	vLvPoint m_vLvPt;	// ������ ��밡���� ����Ʈ

public :
	void SetPartstat( int iStat, const sPsData& sData );
	void InsertLvPoint( WORD wPt );
	bool ParseData( int iPart, std::string strName );

	const sPsData* GetPsData( COSTUME_STATS eState );
	const mPsData* const GetPsData();
		  WORD	   GetLvValue( WORD wLv );
};

// �ߺ� �й��ϸ� �ȵǴ� ����Ʈ
class CNDuplication
{
public :
	CNDuplication();
	~CNDuplication();

	typedef std::vector< WORD > vData;
	typedef vData::iterator	  vIter;
	
	typedef std::map< WORD, vData > mList;
	typedef mList::iterator			mIter;

protected :
	mList	m_List;

public :
	bool ParseData(std::string strName);
	bool IsNDupleExist(COSTUME_STATS eSt, vData& vList);
	void GetNDupleList(COSTUME_STATS eSt, mList& mList);

};

class CCostumeStatData
{
public :
	enum SHARE_TYPE {
		E_SHARE_GN_TYPE    = 0,
		E_SHARE_VAR_TYPE   = 1,
		E_SHARE_ADDED_TYPE = 2
	};
private :
	CCostumeStatData();
public :	
	~CCostumeStatData();

	static CCostumeStatData& GetInstance();

private :

	typedef std::map<SHARE_TYPE, WORD>  mShareCount;
	typedef mShareCount::iterator       mShareIter;
	typedef mShareCount::const_iterator mSharecIter;

	typedef std::map<EMSUIT, std::tr1::shared_ptr<CBasePartsDt>> mPartsData;
	typedef mPartsData::iterator			mPIter;
	typedef mPartsData::value_type			mvalue_type;

	mPartsData		m_Parts;
	CNDuplication	m_NDuplication;

	mShareCount		m_ShareMax;

public :
	bool	LoadData( std::string strFileName );

	bool						   IsPartsData(EMSUIT eSuit);
	bool						   IsNDupleExist(COSTUME_STATS eStat, CNDuplication::vData& vList);

	void						   GetNDupleList(COSTUME_STATS eStat, CNDuplication::mList& mList);
	WORD					       GetShareMax( SHARE_TYPE eType ); 
	WORD						   GetPartsEnableMaxPt( EMSUIT eSuit, WORD wLv );
	WORD						   GetStatMaxPt( EMSUIT eSuit, COSTUME_STATS eStat );
	double						   GetStatInc( EMSUIT eSuit, COSTUME_STATS eStat );
	const sPsData*			       GetStatInfo( EMSUIT eSuit, COSTUME_STATS eStat );
	const CPartsDt::mPsData* const GetStatInfo( EMSUIT eSuit );

protected:
	bool	ParseData();
	
};

};

namespace COSTUMEUSERVALUE
{
	struct sCostumeUser
	{
	public:
		struct sInvest
		{
			void reset(void);
			const bool operator ==(const sCostumeUser::sInvest& _rhs) const;
			const bool operator !=(const sCostumeUser::sInvest& _rhs) const;
			const sCostumeUser::sInvest& operator =(const sCostumeUser::sInvest& rhs);
			sInvest(const BYTE _cStatType, const WORD _wInvestPt );
			sInvest(void);

			BYTE		cStatType;  // �ɷ�ġ Ÿ��

			WORD		wInvestPt;  // ������ ����Ʈ
			
			MSGPACK_DEFINE(cStatType, wInvestPt);

			friend class boost::serialization::access;
			template <typename Archive>
			void serialize(Archive &ar, const unsigned int version)
			{
				ar & cStatType;
				ar & wInvestPt;
			}
		};

		enum { VERSION = 0x0001, MAX_COS_SIZE = 7, COSTUME_TYPE_NULL = 0xffffffff };

	public:	
		// ����� ���� ���� ��(0�� ���� �˻�);
		const bool setInvest(const BYTE nType, const WORD wPt);

		// ������ ���� ���� ���� ��;
		const bool setInvest(const DWORD nIndex, const BYTE nType, const WORD wPt);

		void remove(const DWORD nIndex);
		const BYTE getType(const DWORD nIndex) const;
		const __time64_t getEndTime() const;
		const WORD getInvestPt(const DWORD nIndex) const;

		bool  getTypeInfo( BYTE  nType, sInvest& sinfo ) const;

		bool  IsCSApplied() const;
		void  ResetInvest() ;

	private:
		const DWORD _findEmptySlot(void) const; 
		const DWORD _size(void) const;

	public:
		const sCostumeUser& operator=(const sCostumeUser& _rhs);
		const bool operator ==(const sCostumeUser& _rhs);
		sCostumeUser(void);
		sCostumeUser(const sCostumeUser& _rhs);

		float getValue( int iNum, int iPart );

	public:
				
		sInvest Invest[MAX_COS_SIZE];	

		__time64_t	tEndTime;  // ����ð�

		friend class boost::serialization::access;
		template <typename Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			for( int i = 0; i < sCostumeUser::MAX_COS_SIZE; ++i )
				ar & Invest[i];

			ar & tEndTime;

		}

		MSGPACK_DEFINE(Invest[0], Invest[1], Invest[2], Invest[3], Invest[4], Invest[5], Invest[6], tEndTime);
	};

};

#pragma once

#include "../../SigmaCore/gassert.h"
#include "../../RanLogic/Character/GLCharData.h"

#include <list>
#include <boost/pool/pool_alloc.hpp>

class GLGaeaClient;
class GLReservationAction;

struct GLREACTION
{
	friend class GLReservationAction;

private:
	//! ���� �������� �����Ҽ� �����ϴ�
	EMREACTION   emREACTION;   // ��������.
	mutable bool bREGISTATION; // ��� ����

protected:
	//! ������� ���� ����
	struct ATTRIBUTION
	{
	public:
		bool		bPRINTFAILED;   //  ���н� ���� ���
		bool		bPRESERVATION;	//	���� �ݺ�.

	public:
		ATTRIBUTION()
			: bPRINTFAILED(true)
			, bPRESERVATION(false)
		{
		}

	} ATTR;

protected:
	union
	{
		//TRACE
		struct
		{
			STARGETID		TRACE_sTARID;		// ������ǥ
			D3DXVECTOR3		TRACE_vPOS;			// �̵���ġ
			bool			TRACE_bNONMOBCOLL;	// �� �浹üũ ���Ұ�����
		};

		//MOB
		struct
		{
			STARGETID		MOB_sTARID;		// ��������.
			D3DXVECTOR3		MOB_vPOS;		// ������ġ.
			bool			MOB_bNONTRACE;	// ���� ���� ����������.
			bool			MOB_bNONCANCEL; // �ٸ� �ൿ�� ������� ��������
		};

		//P2P
		struct
		{
			STARGETID		P2P_sTARID;		// ��������.
			D3DXVECTOR3		P2P_vPOS;		// ������ġ.
		};

		//PVP
		struct
		{
			STARGETID		PVP_sTARID;		// ��������.
			D3DXVECTOR3		PVP_vPOS;		// ������ġ.
			bool			PVP_bNONTRACE;	// ���� ���� ����������.
			bool			PVP_bNONCANCEL; // �ٸ� �ൿ�� ������� ��������
		};

		//NPC
		struct
		{
			STARGETID		NPC_sTARID;		// ��������.
			D3DXVECTOR3		NPC_vPOS;		// ������ġ.
			bool			NPC_bONLYTRACE; // ������ �Ұ�����.
		};

		//ITEM
		struct
		{
			STARGETID		ITEM_sTARID;	 // ��������.
			D3DXVECTOR3		ITEM_vPOS;		 // ������ġ.
			bool			ITEM_bONLYTRACE; // ������ �Ұ�����.
			bool			ITEM_bManualFind;	// ���� ã���ΰ�?
		};

		//SKILL
		struct
		{
			STARGETID		SKILL_sTARID;			// ��������.
			D3DXVECTOR3		SKILL_vPOS;				// ������ġ.
			SNATIVEID		SKILL_sSKILLID;			// ����� ��ų ID
			WORD			SKILL_wCHANGERUNSKILL;	// ������(�����) ��ų ����
			bool			SKILL_bRESERVED;		// ���� ��ų������ ���� �ɼ�
			bool			SKILL_bRETARGETING;		// Ÿ���� �缳�� �Ұ�����
			bool			SKILL_bDEFENSE;			// ���潺��ų
		};

		//GATHERING
		struct
		{
			STARGETID		GATHERING_sTARID;	  // ��������.
			D3DXVECTOR3		GATHERING_vPOS;		  // ������ġ.
			bool			GATHERING_bONLYTRACE; // ������ �Ұ�����.
		};

		//TOGGLE_VEHICLE
		struct
		{
			bool TOGGLE_VEHICLE_bACTIVE; // Ȱ��/��Ȱ�� ����
			bool TOGGLE_VEHICLE_bREQ;	 // ��û ����
		};

		//SWAP_ARMS
		struct
		{
			bool SWAP_ARMS_bREQ; // ��û ����
		};

	} DATA;

public:
	template<typename T>
	operator T& ()
	{
		if( GETTYPE() != REACT_NULL )
		{
			GASSERT( GETTYPE() == T::REACTION_TYPE );
		}

		return static_cast<T&>((*this));
	}

	template<typename T>
	operator const T& () const
	{
		if( GETTYPE() != REACT_NULL )
		{
			GASSERT( GETTYPE() == T::REACTION_TYPE );
		}

		return static_cast<const T&>((*this));
	}


	template<typename T>
	operator const T& ()
	{
		if( GETTYPE() != REACT_NULL )
		{
			GASSERT( GETTYPE() == T::REACTION_TYPE );
		}

		return static_cast<const T&>((*this));
	}

	template<typename T>
	void COPYATTR ( IN const T& src )
	{
		ATTR = src.ATTR;
	}

	template<typename T>
	void COPYDATA ( IN const T& src )
	{
		DATA = src.DATA;
	}

public:
	GLREACTION()
		: emREACTION(REACT_NULL)
		, bREGISTATION(false)
	{
		 ZeroMemory(&DATA, sizeof(DATA));
	}

	GLREACTION( EMREACTION _emREACTION ) 
		: emREACTION(_emREACTION)
		, bREGISTATION(false) 
	{
		ZeroMemory(&DATA, sizeof(DATA));
	}

	explicit GLREACTION( const GLREACTION& src ) 
		: emREACTION(src.emREACTION)
		, bREGISTATION(false) 
	{
		COPYATTR(src);
		COPYDATA(src);
	}

protected:
	virtual void _INITIALIZE() 
	{
		//Blank
	}

public:
	void SETPRINTFAILED( const bool bEnable )
	{
		ATTR.bPRINTFAILED = bEnable;
	}
	
	void SETPRESERVATION( const bool bEnable )
	{
		ATTR.bPRESERVATION = bEnable;
	}

	const EMREACTION GETTYPE() const
	{
		return emREACTION;
	}

	const bool ISREGISTATION() const
	{
		return bREGISTATION;
	}

	const bool ISPRINTFAILED() const
	{
		return ATTR.bPRINTFAILED;
	}

	const bool ISPRESERVATION() const
	{
		return ATTR.bPRESERVATION;
	}

public:
	bool ISNULL() const 
	{ 
		return (emREACTION==REACT_NULL); 
	}

	STARGETID& GETTARID()
	{
		switch(emREACTION)
		{
		case REACT_TRACE:		return DATA.TRACE_sTARID;
		case REACT_MOB:			return DATA.MOB_sTARID;
		case REACT_NPC:			return DATA.NPC_sTARID;
		case REACT_P2P:			return DATA.P2P_sTARID;
		case REACT_PVP:			return DATA.PVP_sTARID;
		case REACT_ITEM:		return DATA.ITEM_sTARID;
		case REACT_SKILL:		return DATA.SKILL_sTARID;
		case REACT_GATHERING:	return DATA.GATHERING_sTARID;
		case REACT_SUMMON_NPC:  return DATA.NPC_sTARID;

		case REACT_NULL:
		default:
			break;
		};

		static STARGETID sNULL;
		return sNULL;
	}

private:
	void SETREGISTATION( const bool bEnable ) const
	{
		bREGISTATION = bEnable;
	}
};

// [2014.09.03][shhan] boost::fast_pool_allocator<GLREACTION> ����ϸ鼭 Memory Leak �߻�. ���� �ľ� �ȵ�.
//typedef std::list<GLREACTION, boost::fast_pool_allocator<GLREACTION> >	GLREACTION_LIST;
typedef std::list<GLREACTION>											GLREACTION_LIST;
typedef GLREACTION_LIST::iterator										GLREACTION_ITER;
typedef GLREACTION_LIST::reverse_iterator								GLREACTION_RITER;


#define DECLARE_CLASSIFIED_GLREACTION(name,type)			\
	static const EMREACTION REACTION_TYPE = type;			\
	name()													\
		: GLREACTION(REACTION_TYPE)							\
	{														\
		_INITIALIZE();										\
	}														\
	explicit name( const GLREACTION& src )					\
		: GLREACTION(REACTION_TYPE)							\
	{														\
		operator=(src);										\
	}														\
	const name& operator=( const GLREACTION& rhs )			\
	{														\
		if( rhs.GETTYPE() != REACT_NULL )					\
		{													\
			GASSERT( rhs.GETTYPE() == name::REACTION_TYPE );\
		}													\
															\
		COPYATTR(rhs);										\
		COPYDATA(rhs);										\
															\
		return (*this);										\
	}

//NULL
struct GLREACTION_NULL : public GLREACTION
{
	DECLARE_CLASSIFIED_GLREACTION(GLREACTION_NULL, REACT_NULL);
};

//TRACE
struct GLREACTION_TRACE : public GLREACTION
{
	DECLARE_CLASSIFIED_GLREACTION(GLREACTION_TRACE, REACT_TRACE);

	void SetTARID( const STARGETID& sTARID ) { DATA.TRACE_sTARID = sTARID; }
	const STARGETID& GetTARID()	const { return DATA.TRACE_sTARID;   }

	void SetPOS( const D3DXVECTOR3& vPOS ) { DATA.TRACE_vPOS = vPOS; }
	const D3DXVECTOR3& GetPOS() const { return DATA.TRACE_vPOS; }

	void SetNONMOBCOLL( const bool bEnable ) { DATA.TRACE_bNONMOBCOLL = bEnable; }
	const bool GetNONMOBCOLL()	const { return DATA.TRACE_bNONMOBCOLL; }
};

//MOB
struct GLREACTION_MOB : public GLREACTION
{
	DECLARE_CLASSIFIED_GLREACTION(GLREACTION_MOB, REACT_MOB);

	void SetTARID( const STARGETID& sTARID ) { DATA.MOB_sTARID = sTARID; }
	const STARGETID& GetTARID()	const { return DATA.MOB_sTARID; }

	void SetPOS( const D3DXVECTOR3& vPOS ) { DATA.MOB_vPOS = vPOS; }
	const D3DXVECTOR3& GetPOS() const { return DATA.MOB_vPOS; }

	void SetNONTRACE( const bool bENABLE ) { DATA.MOB_bNONTRACE = bENABLE; }
	const bool IsNONTRACE() const { return DATA.MOB_bNONTRACE; }

	void SetNONCANCEL( const bool bENABLE ) { DATA.MOB_bNONCANCEL = bENABLE; }
	const bool IsNONCANCEL() const { return DATA.MOB_bNONCANCEL; }
};

//P2P
struct GLREACTION_P2P : public GLREACTION
{
	DECLARE_CLASSIFIED_GLREACTION(GLREACTION_P2P, REACT_P2P);

	void SetTARID( const STARGETID& sTARID ) { DATA.P2P_sTARID = sTARID; }
	const STARGETID& GetTARID()	const { return DATA.P2P_sTARID; }

	void SetPOS( const D3DXVECTOR3& vPOS ) { DATA.P2P_vPOS = vPOS; }
	const D3DXVECTOR3& GetPOS() const { return DATA.P2P_vPOS; }
};

//PVP
struct GLREACTION_PVP : public GLREACTION
{
	DECLARE_CLASSIFIED_GLREACTION(GLREACTION_PVP, REACT_PVP);

	void SetTARID( const STARGETID& sTARID ) { DATA.PVP_sTARID = sTARID; }
	const STARGETID& GetTARID()	const { return DATA.PVP_sTARID; }

	void SetPOS( const D3DXVECTOR3& vPOS ) { DATA.PVP_vPOS = vPOS; }
	const D3DXVECTOR3& GetPOS() const { return DATA.PVP_vPOS; }

	void SetNONTRACE( const bool bENABLE ) { DATA.PVP_bNONTRACE = bENABLE; }
	const bool IsNONTRACE() const { return DATA.PVP_bNONTRACE; }

	void SetNONCANCEL( const bool bENABLE ) { DATA.PVP_bNONCANCEL = bENABLE; }
	const bool IsNONCANCEL() const { return DATA.PVP_bNONCANCEL; }
};

//NPC
struct GLREACTION_NPC : public GLREACTION
{
	DECLARE_CLASSIFIED_GLREACTION(GLREACTION_NPC, REACT_NPC);

	void SetTARID( const STARGETID& sTARID ) { DATA.NPC_sTARID = sTARID; }
	const STARGETID& GetTARID()	const { return DATA.NPC_sTARID; }

	void SetPOS( const D3DXVECTOR3& vPOS ) { DATA.NPC_vPOS = vPOS; }
	const D3DXVECTOR3& GetPOS() const { return DATA.NPC_vPOS; }

	void SetONLYTRACE( const bool bENABLE ) { DATA.NPC_bONLYTRACE = bENABLE; }
	const bool IsONLYTRACE() const { return DATA.NPC_bONLYTRACE; }
};

//ITEM
struct GLREACTION_ITEM : public GLREACTION
{
	DECLARE_CLASSIFIED_GLREACTION(GLREACTION_ITEM, REACT_ITEM);

	void SetTARID( const STARGETID& sTARID ) { DATA.ITEM_sTARID = sTARID; }
	const STARGETID& GetTARID()	const { return DATA.ITEM_sTARID; }

	void SetPOS( const D3DXVECTOR3& vPOS ) { DATA.ITEM_vPOS = vPOS; }
	const D3DXVECTOR3& GetPOS() const { return DATA.ITEM_vPOS; }

	void SetONLYTRACE( const bool bENABLE ) { DATA.ITEM_bONLYTRACE = bENABLE; }
	const bool IsONLYTRACE() const { return DATA.ITEM_bONLYTRACE; }

	void SetManualFind( bool bManual ) { DATA.ITEM_bManualFind = bManual; }
	bool GetManualFind() { return DATA.ITEM_bManualFind; }
};

//SKILL
struct GLREACTION_SKILL : public GLREACTION
{
	DECLARE_CLASSIFIED_GLREACTION(GLREACTION_SKILL, REACT_SKILL);

	virtual void _INITIALIZE() override
	{
		DATA.SKILL_wCHANGERUNSKILL = USHRT_MAX;
		DATA.SKILL_bRESERVED	   = false;
		DATA.SKILL_bRETARGETING	   = false;
		DATA.SKILL_bDEFENSE		   = false;
	}

	void SetTARID( const STARGETID& sTARID ) { DATA.SKILL_sTARID = sTARID; }
	const STARGETID& GetTARID()	const { return DATA.SKILL_sTARID; }

	void SetPOS( const D3DXVECTOR3& vPOS ) { DATA.SKILL_vPOS = vPOS; }
	const D3DXVECTOR3& GetPOS() const { return DATA.SKILL_vPOS; }

	void SetSKILLID( const SNATIVEID& sSKILLID ) { DATA.SKILL_sSKILLID = sSKILLID; }
	const SNATIVEID& GetSKILLID() const { return DATA.SKILL_sSKILLID; }

	void SetCHANGERUNSKILL( const WORD wQUICKSKILLSLOT ) { DATA.SKILL_wCHANGERUNSKILL = wQUICKSKILLSLOT; }
	const WORD GetCHANGERUNSKILL() const { return DATA.SKILL_wCHANGERUNSKILL; }
	const bool IsCHANGERUNSKILL() const { return (DATA.SKILL_wCHANGERUNSKILL != USHRT_MAX); } 

	void SetRESERVED( const bool bENABLE ) { DATA.SKILL_bRESERVED = bENABLE; }
	const bool IsRESERVED() const { return DATA.SKILL_bRESERVED; }

	void SetRETARGETING( const bool bENABLE ) { DATA.SKILL_bRETARGETING = bENABLE; }
	const bool IsRETARGETING() const { return DATA.SKILL_bRETARGETING; }
	
	void SetDEFENSE( const bool bENABLE ) { DATA.SKILL_bDEFENSE = bENABLE; }
	const bool IsDEFENSE() const { return DATA.SKILL_bDEFENSE; }
	
};

//GATHERING
struct GLREACTION_GATHERING : public GLREACTION
{
	DECLARE_CLASSIFIED_GLREACTION(GLREACTION_GATHERING, REACT_GATHERING);

	void SetTARID( const STARGETID& sTARID ) { DATA.GATHERING_sTARID = sTARID; }
	const STARGETID& GetTARID()	const { return DATA.GATHERING_sTARID; }

	void SetPOS( const D3DXVECTOR3& vPOS ) { DATA.GATHERING_vPOS = vPOS; }
	const D3DXVECTOR3& GetPOS() const { return DATA.GATHERING_vPOS; }

	void SetONLYTRACE( const bool bENABLE ) { DATA.GATHERING_bONLYTRACE = bENABLE; }
	const bool IsONLYTRACE() const { return DATA.GATHERING_bONLYTRACE; }
};

//TOGGLE_VEHICLE
struct GLREACTION_TOGGLE_VEHICLE : public GLREACTION
{
	DECLARE_CLASSIFIED_GLREACTION(GLREACTION_TOGGLE_VEHICLE, REACT_TOGGLE_VEHICLE);

	void SetACTIVE( const bool bACTIVE ) { DATA.TOGGLE_VEHICLE_bACTIVE = bACTIVE; }
	const bool IsACTIVE() const { return DATA.TOGGLE_VEHICLE_bACTIVE;   }

	void SetREQ() { DATA.TOGGLE_VEHICLE_bREQ = true; }
	const bool IsREQ() const { return DATA.TOGGLE_VEHICLE_bREQ; }
};

//SWAP_ARMS
struct GLREACTION_SWAP_ARMS : public GLREACTION
{
	DECLARE_CLASSIFIED_GLREACTION(GLREACTION_SWAP_ARMS, REACT_SWAP_ARMS);

	void SetREQ() { DATA.SWAP_ARMS_bREQ = true; }
	const bool IsREQ() const { return DATA.SWAP_ARMS_bREQ; }
};

//SUMMON_NPC
struct GLREACTION_SUMMON_NPC : public GLREACTION
{
	DECLARE_CLASSIFIED_GLREACTION(GLREACTION_SUMMON_NPC, REACT_SUMMON_NPC);

	void SetTARID( const STARGETID& sTARID ) { DATA.NPC_sTARID = sTARID; }
	const STARGETID& GetTARID()	const { return DATA.NPC_sTARID; }

	void SetPOS( const D3DXVECTOR3& vPOS ) { DATA.NPC_vPOS = vPOS; }
	const D3DXVECTOR3& GetPOS() const { return DATA.NPC_vPOS; }

	void SetONLYTRACE( const bool bENABLE ) { DATA.NPC_bONLYTRACE = bENABLE; }
	const bool IsONLYTRACE() const { return DATA.NPC_bONLYTRACE; }
};
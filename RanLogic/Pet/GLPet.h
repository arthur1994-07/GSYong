#ifndef GLPET_H_
#define GLPET_H_

#pragma once

#include <boost/tr1/unordered_map.hpp>	
#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../Item/GLItem.h"
#include "../Skill/GLSkill.h"
#include "./GLPetDefine.h"

struct NET_MSG_GENERIC;
class GLChar;

namespace GLPETDEFINE
{
	extern D3DXVECTOR3 RANDPOS[8];
	
	D3DXVECTOR3 GetRandomPosition ();
}

struct PETSKILL
{
	static DWORD VERSION;
	static DWORD SIZE;

	SNATIVEID	sNativeID;
	WORD		wLevel;

	PETSKILL () :
		sNativeID(SNATIVEID::ID_NULL,SNATIVEID::ID_NULL),
		wLevel(0)
	{
	}

	PETSKILL ( const SNATIVEID &_sNID, const WORD _wLvl ) :
		sNativeID(_sNID.dwID),
		wLevel(_wLvl)
	{
	}
};

struct PETREVIVEINFO
{
	DWORD		m_PetDbNum;
	char		szPetName[PETNAMESIZE+1];
	SNATIVEID	sPetCardID;
	int		emType;

	PETREVIVEINFO () :	
		m_PetDbNum(0),
		sPetCardID(NATIVEID_NULL()),
		emType(PETTYPE_NONE)
	{
		memset ( szPetName, 0, PETNAMESIZE+1 );
	}

	PETREVIVEINFO ( const DWORD dwID, const char* szName, SNATIVEID sCardID, int emTYPE ) :
		m_PetDbNum(dwID),
		sPetCardID(sCardID),
		emType(emTYPE)
	{
		StringCchCopy ( szPetName, PETNAMESIZE, szName );
	}

	PETREVIVEINFO ( const PETREVIVEINFO& value ) { operator=(value); }
	PETREVIVEINFO& operator= ( const PETREVIVEINFO& rvalue );
};

typedef std::map<DWORD, PETSKILL>    PETSKILL_MAP;
typedef PETSKILL_MAP::iterator		 PETSKILL_MAP_ITER;
typedef PETSKILL_MAP::const_iterator PETSKILL_MAP_CITER;

typedef std::map<DWORD, float> PETDELAY_MAP;
typedef PETDELAY_MAP::iterator PETDELAY_MAP_ITER;

typedef std::map<DWORD, PETREVIVEINFO> PETREVIVEINFO_MAP;
typedef PETREVIVEINFO_MAP::iterator	   PETREVIVEINFO_MAP_ITER;

struct SPETCARDINFO
{
	char			m_szName[PETNAMESIZE+1];
	int			    m_emTYPE;
	int				m_nFull;
	SNATIVEID		m_sActiveSkillID[MAX_PETSKILLSLOT];
	WORD			m_wMaxSkillSlot;
	SITEMCUSTOM		m_PutOnItems[ACCETYPESIZE];
	PETSKILL_MAP	m_ExpSkills;

	SPETCARDINFO()
        : m_emTYPE(PETTYPE_NONE)
        , m_nFull(0)
        , m_wMaxSkillSlot(0)
	{
		m_ExpSkills.clear();
		memset(m_szName, 0, PETNAMESIZE+1);

		for (WORD i=0; i<MAX_PETSKILLSLOT; ++i)
			m_sActiveSkillID[i] = NATIVEID_NULL();
	}

	SPETCARDINFO (const SPETCARDINFO& value) { operator=( value ); }
	SPETCARDINFO& operator = (const SPETCARDINFO& rvalue);
};

typedef std::map<DWORD, SPETCARDINFO> PETCARDINFO_MAP;
typedef PETCARDINFO_MAP::iterator	  PETCARDINFO_MAP_ITER;

struct SPetSkinPackState
{
	SNATIVEID petSkinMobID;
	float	  fPetScale;

	bool bUsePetSkinPack;

	bool bNotFoundGuard;
	bool bNotFoundRun;
	bool bNotFoundWalk;
	bool bNotFoundGesture;
	bool bNotFoundAttack;
	bool bNotFoundSad;

	SPetSkinPackState()
	{
		Init();
	}

	void Init()
	{
		petSkinMobID     = NATIVEID_NULL();

		bUsePetSkinPack  = FALSE;

		bNotFoundGuard   = FALSE;
		bNotFoundRun	 = FALSE;
		bNotFoundWalk	 = FALSE;
		bNotFoundGesture = FALSE;
		bNotFoundAttack	 = FALSE;
		bNotFoundSad	 = FALSE;

		fPetScale		 = 1.0f;
	}


};

struct SPETACTIVESKILLINFO
{
	SKILL::EMTYPES emTypes;
	WORD		   wLevel;
	WORD		   wCount;

	SPETACTIVESKILLINFO()
		: emTypes(SKILL::EMFOR_HP)
		, wLevel(0)
		, wCount(0)
	{
	}

	bool operator == ( const SPETACTIVESKILLINFO& sSkillInfo )
	{
		return (emTypes == sSkillInfo.emTypes) && (wLevel == sSkillInfo.emTypes);
	}
};

typedef std::vector<SPETACTIVESKILLINFO>		    PETACTIVESKILLLIST_VEC;
typedef std::vector<SPETACTIVESKILLINFO>::iterator  PETACTIVESKILLLIST_VEC_ITER;

struct GLPET
{
    enum { PET_SKILL_MAIN = 26, };

	char			m_szName[PETNAMESIZE+1];
	int			m_emTYPE;
	DWORD			m_dwGUID;
	SNATIVEID		m_sPetID;
	
	SNATIVEID		m_sActiveSkillID[MAX_PETSKILLSLOT];
	WORD			m_wMaxSkillSlot;

	PETSKILL_MAP	m_ExpSkills;
	DWORD			m_dwOwner; //! 주인 GaeaID
	WORD			m_wStyle;
	WORD			m_wColor;
	float			m_fWalkSpeed;
	float			m_fRunSpeed;
	int				m_nFull; //! 펫 포만감
	SITEMCUSTOM		m_PutOnItems[ACCETYPESIZE];
	MapID			m_sMapID;
	DWORD			m_dwCellID;
	DWORD			m_DbNum; //! Pet DB 번호
	SNATIVEID		m_sPetCardID;				// 소환을 요청한 팻카드의 Mid/Sid
	SPetSkinPackState	m_sPetSkinPackState;
	SPETSKINPACKDATA	m_sPetSkinPackData;

    bool bChangeableColor;
    int nFullDecrement[MAX_PETSKILLSLOT]; // 포만감 감소치

public:
	// constructor & operator
	GLPET():
		m_emTYPE(PETTYPE_NONE),
		m_dwGUID(UINT_MAX),
		m_sPetID(NATIVEID_NULL()),
		m_wMaxSkillSlot(0),
		m_dwOwner(0),
		m_wStyle(0),
		m_wColor(0),
		m_fWalkSpeed(0.0f),
		m_fRunSpeed(0.0f),
		m_nFull(0),
		m_dwCellID(0),
		m_DbNum(0),
		m_sPetCardID(NATIVEID_NULL()),
        bChangeableColor(false)
	{
		m_sPetSkinPackState.Init();
		m_sPetSkinPackData.Init();
		m_ExpSkills.clear();
		memset ( m_szName, 0, PETNAMESIZE+1 );

		for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
			m_sActiveSkillID[i] = NATIVEID_NULL();
	}
   

	GLPET ( const GLPET& value ) { operator=( value ); }
	GLPET& operator= ( const GLPET& rvalue );
    virtual ~GLPET() {}

    inline void SetName(const char* szName)
    {
        if (szName)
            StringCchCopyA(m_szName, PETNAMESIZE+1, szName);
    }

    inline void SetName(const std::string& Name)
    {    
        StringCchCopyA(m_szName, PETNAMESIZE+1, Name.c_str());
    }

    inline const char* GetName() const { return m_szName; }

    inline void  SetDbNum(DWORD dwPetID) { m_DbNum = dwPetID; }
    inline DWORD DbNum() const { return m_DbNum; }

    //! 펫 포만감
    inline int Satiation() const { return m_nFull; }

    BOOL GETPUTONITEMS_BYBUF ( se::ByteStream &ByteStream ) const;
	BOOL GETPUTONITEMS_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItem );
	void SETPUTONITEM( ACCESSORYTYPE emSlot, const SITEMCUSTOM& sItem );

	void ASSIGN ( const GLPET& sPetData );
	void RESET ();
	bool IncreaseFull ( const WORD volume, const BOOL bRatio = TRUE );
	void LEARN_SKILL ( const SNATIVEID & sSkillID );
	BOOL ISLEARNED_SKILL ( const SNATIVEID & sSkillID );

	BOOL IsNotEnoughFull () { return m_nFull < 10; }

	SITEMCUSTOM GetSlotitembySuittype ( EMSUIT emSuit );
	bool SetSlotItem( EMSUIT emType, SITEMCUSTOM& sCustomItem );
	bool ReSetSlotItem ( EMSUIT emType );
	BOOL CheckSlotItem ( SNATIVEID sNativeID, EMSUIT emSUIT );

	//! File Load
    BOOL LoadFile(const std::string& FileName, int emType);

	// 스킬 중복 검사
	BOOL				IsSkillDuplicated	  ( const SNATIVEID& sSkillID, const WORD wIndex ); 
	EMPETEQUALITY_SKILL IsSkillDuplicatedEx   ( BYTE** ppOutActiveSkillArray=NULL );

	// 습득 스킬 Flag
	DWORD				GetSkillGetItemFlag ();
};

typedef GLPET* PGLPET;

struct SDROPPET
{
	char			m_szName[PETNAMESIZE+1];
	int			m_emTYPE;
	DWORD			m_dwGUID;
	SNATIVEID		m_sPetID;
	SNATIVEID		m_sActiveSkillID[MAX_PETSKILLSLOT];
	WORD			m_dwMaxSkillSlot;
	
	DWORD			m_dwOwner;					// 주인 GaeaID
	WORD			m_wStyle;
	WORD			m_wColor;
	float			m_fWalkSpeed;
	float			m_fRunSpeed;
	int				m_nFull;
	SITEMCUSTOM		m_PutOnItems[ACCETYPESIZE];
	
	MapID			m_sMapID;
	DWORD			m_dwCellID;

	D3DXVECTOR3		m_vPos;
	D3DXVECTOR3		m_vDir;
	D3DXVECTOR3		m_vTarPos;

	DWORD			m_dwActionFlag;
	EMPETACTIONTYPE m_emPETACTYPE;

	SPETSKINPACKDATA m_petSkinPackData;

	WORD			m_wAniSub;					 // 애니메이션 관련

	DWORD			m_dwPetID;

	SDROPPET () :
		m_emTYPE(PETTYPE_NONE),
		m_dwGUID(UINT_MAX),
		m_sPetID(NATIVEID_NULL()),
		m_dwMaxSkillSlot(0),
		m_dwOwner(0),
		m_wStyle(0),
		m_wColor(0),
		m_fWalkSpeed(0.0f),
		m_fRunSpeed(0.0f),
		m_nFull(0),
		m_dwCellID(0),

		m_vPos (0,0,0),
		m_vDir (0,0,-1),

		m_vTarPos(0,0,0),
		
		m_dwActionFlag(0),
		m_emPETACTYPE(PETAT_IDLE),

		m_wAniSub(1),
		m_dwPetID(0)
	{
		m_petSkinPackData.Init();
		memset ( m_szName, 0, PETNAMESIZE+1 );

		for ( WORD i = 0; i < MAX_PETSKILLSLOT; ++i )
		{
			m_sActiveSkillID[i] = NATIVEID_NULL();
		}

	}

	SDROPPET ( const SDROPPET& value ) { operator=( value ); }
	SDROPPET& operator= ( const SDROPPET& rvalue );

	void RESET ();

    void SetName(const char* szName)
    {
        if (szName)
            StringCchCopy(m_szName, PETNAMESIZE+1, szName);
    }
};

typedef SDROPPET* PSDROPPET;

// PET 부활 LIST
struct PET_RELIST
{
	int       nPetNum;
	char      szPetName[PETNAMESIZE+1];
	SNATIVEID sPetCardID;
	int	      emType;

	PET_RELIST()
		: nPetNum(0)
		, sPetCardID(NATIVEID_NULL())
		, emType(PETTYPE_NONE)
	{
		memset(szPetName, 0, sizeof(char) * (PETNAMESIZE+1));
	}

    void SetName(const char* Name)
    {
        if (Name)
            ::StringCchCopy(szPetName, PETNAMESIZE+1, Name);
    }

    void SetName(const std::string& Name)
    {
        ::StringCchCopy(szPetName, PETNAMESIZE+1, Name.c_str());
    }
};

typedef std::vector<std::string> VEC_PET_STR;

struct PET_STYLECARD_UI
{
    WORD wPetType, wPetStyle;

    WORD wWindowPosX, wWindowPosY, wWindowWidth, wWindowHeight;

    std::string strTexture;
    WORD wTextureSizeX, wTextureSizeY;
    WORD wTexturePosX, wTexturePosY, wTextureWidth, wTextureHeight;

    PET_STYLECARD_UI(
        WORD _wPetType
        , WORD _wPetStyle
        , WORD _wWindowPosX
        , WORD _wWindowPosY
        , WORD _wWindowWidth
        , WORD _wWindowHeight
        , std::string _strTexture
        , WORD _wTextureSizeX
        , WORD _wTextureSizeY
        , WORD _wTexturePosX
        , WORD _wTexturePosY
        , WORD _wTextureWidth
        , WORD _wTextureHeight
            )
        : wPetType(_wPetType) 
        , wPetStyle(_wPetStyle)
        , wWindowPosX(_wWindowPosX)
        , wWindowPosY(_wWindowPosY)
        , wWindowWidth(_wWindowWidth)
        , wWindowHeight(_wWindowHeight)
        , strTexture(_strTexture)
        , wTextureSizeX(_wTextureSizeX)
        , wTextureSizeY(_wTextureSizeY)
        , wTexturePosX(_wTexturePosX)
        , wTexturePosY(_wTexturePosY)
        , wTextureWidth(_wTextureWidth)
        , wTextureHeight(_wTextureHeight)
    {
    }
};
typedef std::vector<PET_STYLECARD_UI> VEC_PET_STYLE_UI;


namespace COMMENT
{
	extern VEC_PET_STR PET_TYPE;
	extern std::string PET_SKILLSLOT_TYPE[MAX_PETSKILLSLOT];
    extern VEC_PET_STR szPetInitFile;
    extern VEC_PET_STR szPetStyleCardText;
    extern VEC_PET_STYLE_UI PetStyleCardUI;
};

#endif // GLPET_H_
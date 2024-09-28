#include "pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/Common/GLTexFile.h"
#include "../../EngineLib/G-Logic/Glogic.h"

#include "../GlogicData.h"
#include "../Item/GLItemMan.h"
#include "../GLogicDataMan.h"
#include "./GLPet.h"
#include "./GLPetDefine.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace GLPETDEFINE
{
	D3DXVECTOR3 RANDPOS[8] = {
		D3DXVECTOR3(0,0,GLCONST_PET::fOwnerDistance),
		D3DXVECTOR3(0,0,-GLCONST_PET::fOwnerDistance),
		D3DXVECTOR3(GLCONST_PET::fOwnerDistance,0,0),
		D3DXVECTOR3(-GLCONST_PET::fOwnerDistance,0,0),
		D3DXVECTOR3(cos(45.0f)*GLCONST_PET::fOwnerDistance,0,cos(45.0f)*GLCONST_PET::fOwnerDistance),
		D3DXVECTOR3(-cos(45.0f)*GLCONST_PET::fOwnerDistance,0,cos(45.0f)*GLCONST_PET::fOwnerDistance),
		D3DXVECTOR3(cos(45.0f)*GLCONST_PET::fOwnerDistance,0,-cos(45.0f)*GLCONST_PET::fOwnerDistance),
		D3DXVECTOR3(-cos(45.0f)*GLCONST_PET::fOwnerDistance,0,-cos(45.0f)*GLCONST_PET::fOwnerDistance)
	};

	D3DXVECTOR3 GetRandomPosition ()
	{
		//srand(unsigned int(time(NULL)));
		WORD idx = (WORD)(rand() % 8);
		return RANDPOS[idx];
	}
};

SPETCARDINFO& SPETCARDINFO::operator= ( const SPETCARDINFO& rvalue )
{
	m_emTYPE			= rvalue.m_emTYPE;
	m_nFull				= rvalue.m_nFull;
	m_wMaxSkillSlot		= rvalue.m_wMaxSkillSlot;

	m_ExpSkills.clear();
	PETSKILL_MAP_CITER iter = rvalue.m_ExpSkills.begin();
	PETSKILL_MAP_CITER iter_end = rvalue.m_ExpSkills.end();
	for ( ;iter != iter_end; ++iter )
	{
		const PETSKILL& sPetSkill = (*iter).second;
		m_ExpSkills.insert ( std::make_pair(sPetSkill.sNativeID.dwID,sPetSkill) );
	}

	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		m_PutOnItems[i] = rvalue.m_PutOnItems[i];
	}

	for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
	{
		m_sActiveSkillID[i] = rvalue.m_sActiveSkillID[i];
	}

	StringCchCopy ( m_szName, PETNAMESIZE+1, rvalue.m_szName );

	return *this;
}

PETREVIVEINFO& PETREVIVEINFO::operator= (const PETREVIVEINFO& rvalue)
{
	m_PetDbNum	= rvalue.m_PetDbNum;
	sPetCardID  = rvalue.sPetCardID;
	emType		= rvalue.emType;
	StringCchCopy(szPetName, PETNAMESIZE+1, rvalue.szPetName);

	return *this;
}

GLPET& GLPET::operator= ( const GLPET& rvalue )
{
	m_emTYPE			= rvalue.m_emTYPE;
	m_dwGUID			= rvalue.m_dwGUID;
	m_sPetID			= rvalue.m_sPetID;
	m_dwOwner			= rvalue.m_dwOwner;
	m_wStyle			= rvalue.m_wStyle;
	m_wColor			= rvalue.m_wColor;
	m_fWalkSpeed		= rvalue.m_fWalkSpeed;
	m_fRunSpeed			= rvalue.m_fRunSpeed;
	m_nFull				= rvalue.m_nFull;
	m_sMapID			= rvalue.m_sMapID;
	m_dwCellID			= rvalue.m_dwCellID;
	m_DbNum			= rvalue.m_DbNum;
	m_sPetCardID		= rvalue.m_sPetCardID;
	m_sPetSkinPackData  = rvalue.m_sPetSkinPackData;
	m_wMaxSkillSlot     = rvalue.m_wMaxSkillSlot;

	m_ExpSkills.clear();
	PETSKILL_MAP_CITER iter = rvalue.m_ExpSkills.begin();
	PETSKILL_MAP_CITER iter_end = rvalue.m_ExpSkills.end();
	for ( ;iter != iter_end; ++iter )
	{
		const PETSKILL& sPetSkill = (*iter).second;
		m_ExpSkills.insert ( std::make_pair(sPetSkill.sNativeID.dwID,sPetSkill) );
	}
	
	SetName(rvalue.GetName());

	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		m_PutOnItems[i] = rvalue.m_PutOnItems[i];
	}

	for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
	{
		m_sActiveSkillID[i] = rvalue.m_sActiveSkillID[i];
	}

	return *this;
}

void GLPET::ASSIGN ( const GLPET& sPetData )
{
	m_emTYPE			= sPetData.m_emTYPE;
	m_dwGUID			= sPetData.m_dwGUID;
	m_sPetID			= sPetData.m_sPetID;
	m_dwOwner			= sPetData.m_dwOwner;
	m_wStyle			= sPetData.m_wStyle;
	m_wColor			= sPetData.m_wColor;
	m_fWalkSpeed		= sPetData.m_fWalkSpeed;
	m_fRunSpeed			= sPetData.m_fRunSpeed;
	m_nFull				= sPetData.m_nFull;
	m_sMapID			= sPetData.m_sMapID;
	m_dwCellID			= sPetData.m_dwCellID;
	m_DbNum			= sPetData.m_DbNum;
	m_sPetCardID		= sPetData.m_sPetCardID;
	m_sPetSkinPackData  = sPetData.m_sPetSkinPackData;
	m_wMaxSkillSlot     = sPetData.m_wMaxSkillSlot;


	m_ExpSkills.clear();
	PETSKILL_MAP_CITER iter = sPetData.m_ExpSkills.begin();
	PETSKILL_MAP_CITER iter_end = sPetData.m_ExpSkills.end();
	for ( ;iter != iter_end; ++iter )
	{
		const PETSKILL& sPetSkill = (*iter).second;
		m_ExpSkills.insert ( std::make_pair(sPetSkill.sNativeID.dwID,sPetSkill) );
	}

	SetName(sPetData.GetName());

	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		m_PutOnItems[i] = sPetData.m_PutOnItems[i];
	}

	for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
	{
		m_sActiveSkillID[i] = sPetData.m_sActiveSkillID[i];
	}
}

void GLPET::LEARN_SKILL ( const SNATIVEID & sSkillID )
{
	// 레벨 : 1 ==> 인덱스 : 0
	// 스킬습득직후 스킬 발동 가능케 하기 위해
	// PETSKILL sPetSkill( sSkillID, 1 ); 
	PETSKILL sPetSkill( sSkillID, 0 );
	m_ExpSkills.insert ( std::make_pair(sPetSkill.sNativeID.dwID,sPetSkill) );
}

BOOL GLPET::ISLEARNED_SKILL ( const SNATIVEID & sSkillID )
{
	//	Note : 스킬 정보 가져옴.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.wMainID, sSkillID.wSubID );
	if ( !pSkill )											return FALSE;

	//	Note : 익힌 스킬인지 검사.
	//
	PETSKILL_MAP_ITER learniter = m_ExpSkills.find ( sSkillID.dwID );
	if ( learniter==m_ExpSkills.end() )			return FALSE;

	return TRUE;
}

void GLPET::RESET ()
{
	m_emTYPE		 = PETTYPE_NONE;
	m_dwGUID		 = UINT_MAX;
	m_dwOwner		 = 0;
	m_wStyle		 = 0;
	m_wColor		 = 0;
	m_fWalkSpeed	 = 0.0f;
	m_fRunSpeed		 = 0.0f;
	m_nFull			 = 0;
	m_sMapID		 = NATIVEID_NULL();
	m_dwCellID		 = 0;
	m_DbNum		 = 0;
	m_sPetID		 = NATIVEID_NULL();
	m_sPetCardID	 = NATIVEID_NULL();
	m_sPetSkinPackData.Init();
	m_wMaxSkillSlot  = 0;

	m_ExpSkills.clear();

	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		m_PutOnItems[i] = SITEMCUSTOM( NATIVEID_NULL() );
	}

	for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
	{
		m_sActiveSkillID[i] = NATIVEID_NULL();
	}
}

BOOL GLPET::LoadFile(const std::string& FileName, int emType)
{
	if (FileName.empty())
        return FALSE;

	std::string strPath(GLOGIC::GetPath());
	strPath.append(FileName);

	gltexfile cFILE;
	cFILE.reg_sep("\t ,{}[]()|");

	if (GLOGIC::UseLogicZipFile()) // by 경대
		cFILE.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile());

	if (!cFILE.open(strPath, true, GLOGIC::UseLogicPackFile()))
	{
        sc::writeLogError(
            sc::string::format(
                "GLPET::LoadFile %1% %2%",
                GLogicData::GetInstance().GetGlogicZipFile(),
                strPath));		
		return FALSE;
	}

	WORD dwVERSION = 0;
	cFILE.getflag( "VERSION", 1, 1, dwVERSION, true );

	cFILE.getflag( "emTYPE",		1, 1, m_emTYPE, true );
	cFILE.getflag( "wStyle",		1, 1, m_wStyle, true );
	cFILE.getflag( "wColor",		1, 1, m_wColor, true );
	cFILE.getflag( "fWalkSpeed",  1, 1, m_fWalkSpeed, true );
	cFILE.getflag( "fRunSpeed",	1, 1, m_fRunSpeed, true );
	cFILE.getflag( "nFull",		1, 1, m_nFull, true );
    // 컬러변경 가능 유무
    cFILE.getflag( "bChangeableColor", 1, 1, bChangeableColor, true );

    // 포만도 감소치
    cFILE.getflag( "nFullDecrement" , 1, 1, nFullDecrement[0], true );
    cFILE.getflag( "nFullDecrement2", 1, 1, nFullDecrement[1], true );

	GLCONST_PET::pGLPET[emType] = this;

	// 스타일 관련
	cFILE.getflag( "wSTYLENum", 1, 1, GLCONST_PET::sPETSTYLE[emType].wSTYLENum, true );
	if ( GLCONST_PET::sPETSTYLE[emType].wSTYLENum > MAX_HAIR ) GLCONST_PET::sPETSTYLE[emType].wSTYLENum = MAX_HAIR;

	for ( WORD i = 0; i <  GLCONST_PET::sPETSTYLE[emType].wSTYLENum; ++i )
		cFILE.getflag( "strSTYLE_CPS", i+1, GLCONST_PET::sPETSTYLE[emType].wSTYLENum, GLCONST_PET::sPETSTYLE[emType].strSTYLE_CPS[i], true );

	for ( WORD i = 0; i <  GLCONST_PET::sPETSTYLE[emType].wSTYLENum; ++i )
	{
		cFILE.getflag( "dwSTYLE_COLOR", i+1, GLCONST_PET::sPETSTYLE[emType].wSTYLENum, GLCONST_PET::sPETSTYLE[emType].wSTYLE_COLOR[i], true );
	}

	m_wColor = GLCONST_PET::sPETSTYLE[emType].wSTYLE_COLOR[m_wStyle];
	
	return TRUE;
}

BOOL GLPET::CheckSlotItem ( SNATIVEID sNativeID, EMSUIT emSUIT )
{
	if ( sNativeID==NATIVEID_NULL() )		return FALSE;

	const SITEM* pItem = GLogicData::GetInstance().GetItem ( sNativeID );
	if ( !pItem )							return FALSE;

	// 복장류 아이템이어야 한다.
	if ( pItem->sBasicOp.emItemType != ITEM_SUIT ) return FALSE;

	// 팻타입과 아이템의 팻타입이 일치해야 한다.
	if ( pItem->sPet.emPetType != m_emTYPE )		return FALSE;
/*
	// 팻데이터용 고유ID가 일치해야 한다.
	if ( pItem->sPet.sPetID != m_sPetID )			return FALSE;
*/
	// 악세서리 슬롯이 일치해야 한다.
	if ( pItem->sSuitOp.emSuit != emSUIT )  return FALSE;

	return TRUE;
}

bool GLPET::IncreaseFull ( const WORD volume, const BOOL bRatio )
{
	int nMaxFull = GLCONST_PET::pGLPET[m_emTYPE]->m_nFull;

	// 포만감이 가득찼으면
	if ( m_nFull >= nMaxFull ) return false;
	if ( bRatio )
	{
		m_nFull += ( nMaxFull*volume )/100;
		if ( m_nFull > nMaxFull ) m_nFull = nMaxFull;
	}
	else
	{
		m_nFull += volume;
		if ( m_nFull > nMaxFull ) m_nFull = nMaxFull;
	}

	return true;
}

BOOL GLPET::GETPUTONITEMS_BYBUF ( se::ByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();

	ByteStream << SITEMCUSTOM::VERSION;
	ByteStream << (DWORD)sizeof(SITEMCUSTOM);
	
	DWORD dwSize = (DWORD) ACCETYPESIZE;
	ByteStream << dwSize;

	ByteStream.WriteBuffer ( (LPBYTE)&m_PutOnItems[0], (DWORD)sizeof(SITEMCUSTOM)*dwSize );

	return TRUE;
}

BOOL GLPET::GETPUTONITEMS_BYVECTOR( std::vector< SINVENITEM_SAVE >& vecItem )
{
	for( int i = 0; i < ACCETYPESIZE; i++ )
	{
		if ( m_PutOnItems[i].GetNativeID() != NATIVEID_NULL() )
		{
			SINVENITEM_SAVE sItem( m_PutOnItems[i], i, 0 );
			vecItem.push_back( sItem );
		}
	}
	return TRUE;
}

void GLPET::SETPUTONITEM( ACCESSORYTYPE emSlot, const SITEMCUSTOM& sItem )
{
	if ( emSlot >= 0 && emSlot < ACCETYPESIZE )
	{
		m_PutOnItems[emSlot] = sItem;
	}
}

SITEMCUSTOM GLPET::GetSlotitembySuittype( EMSUIT emSuit )
{ 
	WORD i = ( WORD ) emSuit - ( WORD ) SUIT_PET_A;
	if ( i >= ACCETYPESIZE )
		return SITEMCUSTOM();
	return m_PutOnItems[i];
}

bool GLPET::SetSlotItem( EMSUIT emType, SITEMCUSTOM& sCustomItem )
{
	WORD i = ( WORD ) emType - ( WORD ) SUIT_PET_A;
	if ( i >= ACCETYPESIZE )
		return false;
	m_PutOnItems[i] = sCustomItem;
	return true;
}

bool GLPET::ReSetSlotItem( EMSUIT emType )
{ 
	WORD i = ( WORD ) emType - ( WORD ) SUIT_PET_A;
	if ( i >= ACCETYPESIZE )
		return false;
	m_PutOnItems[i] = SITEMCUSTOM ( NATIVEID_NULL() );
	return true;
}

// Note : 스킬 중복 검사
BOOL GLPET::IsSkillDuplicated ( const SNATIVEID& sSkillID, const WORD wIndex )
{
	if ( sSkillID == NATIVEID_NULL() )
	{
		return FALSE;
	}

	for ( WORD i=0; i<m_wMaxSkillSlot; ++i )
	{
		if ( i == wIndex ) continue;

		if ( m_sActiveSkillID[i] == sSkillID )
		{
			return TRUE;
		}
	}

	return FALSE;
}

// Note : 스킬 중복 검사 확장
EMPETEQUALITY_SKILL GLPET::IsSkillDuplicatedEx ( BYTE** ppOutActiveSkillArray /*=NULL*/ )
{
	static BYTE sActiveSkillArray[SKILL::FOR_TYPE_SIZE];
	memset ( sActiveSkillArray, 0, sizeof(sActiveSkillArray) );

	if ( ppOutActiveSkillArray )
	{
		(*ppOutActiveSkillArray) = sActiveSkillArray;
	}

	// Note : 스킬 중복 검사
	BOOL IsDuplicated = FALSE;

	for ( WORD i=0; i<m_wMaxSkillSlot; ++i )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sActiveSkillID[i] );
		if ( !pSkill )	continue;

		SKILL::EMTYPES emTypes = pSkill->m_sAPPLY.emBASIC_TYPE;

		sActiveSkillArray[emTypes] += 1;

		// Note : 두개 이상이면 중복 체크
		if ( 2 <= sActiveSkillArray[emTypes] )
		{
			IsDuplicated = TRUE;
		}
	}

	if ( IsDuplicated )
	{
		return EMPETEQUALITY_SKILL_DUPLICATED;
	}

	// Note : 스킬 상등성 검사
	//

	// Note : HP회복과 모두 회복일때
	//
	if ( sActiveSkillArray[SKILL::EMFOR_PET_HEAL] && sActiveSkillArray[SKILL::EMFOR_PET_SUPPROT] )
	{
		return EMPETEQUALITY_SKILL_DUPLICATIONSTATS;
	}

	return EMPETEQUALITY_SKILL_NONE;
}

// Note : 습득 스킬 Flag
DWORD GLPET::GetSkillGetItemFlag ()
{
	DWORD dwGetItemFlag = EMPETSKILL_NONE;

	for ( WORD i=0; i<m_wMaxSkillSlot; ++i )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sActiveSkillID[i] );
		if ( !pSkill )	continue;

		switch ( pSkill->m_sAPPLY.emBASIC_TYPE )
		{
		case SKILL::EMFOR_PET_GETALLITEMS:  dwGetItemFlag |= EMPETSKILL_GETALL;		break;
		case SKILL::EMFOR_PET_GETRAREITEMS: dwGetItemFlag |= EMPETSKILL_GETRARE;	break;
		case SKILL::EMFOR_PET_GETPOTIONS:	dwGetItemFlag |= EMPETSKILL_GETPOTIONS; break;
		case SKILL::EMFOR_PET_GETSTONE:		dwGetItemFlag |= EMPETSKILL_GETSTONE;	break;
		case SKILL::EMFOR_PET_GETMONEY:		dwGetItemFlag |= EMPETSKILL_GETMONEY;	break;
		case SKILL::EMFOR_PET_GETQBOX:		dwGetItemFlag |= EMPETSKILL_GETQBOX;	break;
		case SKILL::EMFOR_PET_GETMATERIALS:		dwGetItemFlag |= EMPETSKILL_GETMATERIALS;	break;
		}
	}

	return dwGetItemFlag;
}

SDROPPET& SDROPPET::operator= ( const SDROPPET& rvalue )
{
	m_emTYPE			= rvalue.m_emTYPE;
	m_dwGUID			= rvalue.m_dwGUID;
	m_sPetID			= rvalue.m_sPetID;
	m_dwOwner			= rvalue.m_dwOwner;
	m_dwMaxSkillSlot	= rvalue.m_dwMaxSkillSlot;
	m_wStyle			= rvalue.m_wStyle;
	m_wColor			= rvalue.m_wColor;
	m_fWalkSpeed		= rvalue.m_fWalkSpeed;
	m_fRunSpeed			= rvalue.m_fRunSpeed;
	m_nFull				= rvalue.m_nFull;

	m_sMapID			= rvalue.m_sMapID;
	m_dwCellID			= rvalue.m_dwCellID;

	m_vPos				= rvalue.m_vPos;
	m_vDir				= rvalue.m_vDir;
	m_vTarPos			= rvalue.m_vTarPos;

	m_dwPetID			= rvalue.m_dwPetID;

	StringCchCopy ( m_szName, PETNAMESIZE+1, rvalue.m_szName );

	m_emPETACTYPE		= rvalue.m_emPETACTYPE;

	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		m_PutOnItems[i] = rvalue.m_PutOnItems[i];
	}

	for ( WORD i = 0; i < MAX_PETSKILLSLOT; ++i )
	{
		m_sActiveSkillID[i] = rvalue.m_sActiveSkillID[i];
	}

	return *this;
}

void SDROPPET::RESET ()
{
	m_emTYPE		 = PETTYPE_NONE;
	m_dwGUID		 = UINT_MAX;
	m_sPetID		 = NATIVEID_NULL();
	m_dwMaxSkillSlot = 0;
	m_dwOwner		 = 0;
	m_wStyle		 = 0;
	m_wColor		 = 0;
	m_fWalkSpeed	 = 0.0f;
	m_fRunSpeed		 = 0.0f;
	m_nFull			 = 0;

	m_sMapID		 = NATIVEID_NULL();
	m_dwCellID		 = 0;

	m_vPos			 = D3DXVECTOR3(0,0,0);
	m_vDir			 = D3DXVECTOR3(0,0,-1);
	m_vTarPos		 = D3DXVECTOR3(0,0,0);

	memset ( m_szName, 0, PETNAMESIZE+1 );

	m_emPETACTYPE	 = PETAT_IDLE;

	m_dwPetID		 = 0;

	for ( WORD i = 0; i < MAX_PETSKILLSLOT; ++i )
	{
		m_sActiveSkillID[i] = NATIVEID_NULL();
	}
}

namespace COMMENT
{
    VEC_PET_DEFINE PET_DEFINE;

	VEC_PET_STR PET_TYPE;
	std::string PET_SKILLSLOT_TYPE[MAX_PETSKILLSLOT] = { "A", "B" };
    VEC_PET_STR szPetInitFile;
    VEC_PET_STR szPetStyleCardText;
    VEC_PET_STYLE_UI PetStyleCardUI;

    int GetPetDefineIndex( const unsigned int nIndex )
    {
        if ( nIndex >= PET_DEFINE.size() )
            return PETTYPE_NONE;
        else
            return PET_DEFINE[nIndex];
    }
};

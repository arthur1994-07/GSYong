#pragma once

#include <string>
#include <list>
#include <vector>
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/Container/GLList.h"

#include "../../EngineLib/EngineTypeDefine.h"
#include "../../EngineLib/Common/ByteStream.h"
#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../EngineLib/DxMeshs/DxSimpleMeshMan.h"
#include "../../EngineLib/DxLand/DxLandDef.h"
#include "../../EngineLib/G-Logic/GLQuadTree.h"

#include "../Party/GLPartyDefine.h"
#include "../Pet/GLPetDefine.h"
#include "../Character/GLCharDefine.h"
#include "./GLItemBox.h"
#include "./GLItemDrug.h"
#include "./GLItemGenerate.h"
#include "./GLItemPet.h"
#include "./GLItemQuestion.h"
#include "./GLItemRandomBox.h"
#include "./GLItemSuit.h"
#include "./GLItemVehecle.h"
#include "./GLItemBasic.h"
#include "./GLItemRandomOption.h"
#include "./GLITEMLMT.h"
#include "./GLItemDef.h"
#include "./GLItemSkillBook.h"
#include "./GLItemGrinding.h"
//#include "./GLItemCustom_Addon.h"
#include "./GLItemCustom.h"
#include "./GLItemSplit.h"
#include "./ItemSet/GLItemSet.h"
#include "./GLItemSkill.h"

namespace sc
{
    class TxtFile;
}
 
//! 아이템 고정 설정
struct SITEM
{
	enum
	{
		VERSION = 0x0107,

		FILE_SBASIC			= 1,	// 기본아이템
		FILE_SSUIT			= 2,	// 의복
		FILE_SDRUG			= 3,	// 약품
		FILE_SSKILLBOOK		= 4,	// 스킬북
		FILE_SGRINDING		= 5,	// 연마아이템
		FILE_SGENERATE		= 6,	// 
		FILE_BOX			= 7,	// ? 선물상자
		FILE_RANDOMBOX		= 8,	// 랜덤박스
		FILE_QUESTIONITEM	= 9,	// Question Item
		FILE_RANDOMITEM		= 10,	// 랜덤아이템
		FILE_PET			= 11,	// 펫 아이템	// PetData
		FILE_VEHICLE		= 12,   // 탈 것 
		FILE_PETSKINPACK	= 13,	// 펫 스킨팩
		FILE_SPLIT			= 14,
		FILE_SKILL			= 15,

		FILE_END_DATA		= 0xEDEDEDED,
	};

private:
	void LoadAfter();
	std::string	strRealName_NO_SAVE;		// 아이템의 실제 스트링 -- 저장하지 않음에 주의 

public:
	ITEM::SBASIC		sBasicOp;
	ITEM::SSUIT			sSuitOp;
	ITEM::SDRUG			sDrugOp;
	ITEM::SSKILLBOOK	sSkillBookOp;
	ITEM::SGRINDING		sGrindingOp;
	ITEM::SGENERATE		sGenerateOp;
	ITEM::SBOX			sBox; //! 선물상자
	ITEM::SRANDOMBOX	sRandomBox;
	ITEM::SQUESTIONITEM	sQuestionItem;
	ITEM::SRANDOM_OPT	sRandomOpt;
	ITEM::SPET			sPet;	// PetData
	ITEM::SVEHICLE		sVehicle;
	ITEM::SPETSKINPACKITEM  sPetSkinPack;
	ITEM::SSPLIT		sSplit;
	ITEM::SISKILL		sItemSkill;

	BOOL SaveFile(sc::SerialFile& SFile);
    BOOL SaveFileLua(sc::TxtFile& LuaFile);
	BOOL LoadFile(sc::BaseStream& SFile);

	// LOAD함수를 항상 새로 만들 필요 없다.
	BOOL Load104(sc::BaseStream& SFile);		// SRANDOM_OPT Load방식 변경됨 
	//BOOL Load105(sc::BaseStream& SFile);
	BOOL LoadCurrent(sc::BaseStream& SFile);	// 현재 버전 읽기

    void LoadFileGenerate( sc::BaseStream &SFile );
    void LoadFileSkillBook( sc::BaseStream &SFile );
    void LoadFileDrug( sc::BaseStream &SFile );
    void LoadFileSuit( sc::BaseStream &SFile );
    void LoadFileBasic( sc::BaseStream &SFile );
	static VOID SaveCsvHead ( std::fstream &SFile );
	VOID SaveCsv ( std::fstream &SFile );
	VOID LoadCsv ( CStringArray &StrArray );


	SITEM ()
	{
	}

	SITEM ( const SITEM &value )
	{
		operator=(value);
	}

	SITEM& operator = ( const SITEM& rvalue );

	bool ISINSTANCE() const { return sDrugOp.bInstance != FALSE; }

	//bool IsTIMELMT() const
	//{
	//	return (sBasicOp.dwFlags&ITEM_TIMELMT)!=NULL && sDrugOp.tTIME_LMT!=0;
	//}

	const bool IsBasicStatExist() const;
	//! 염색 가능한 아이템인지 점검
	const bool IsColorChange() const;

	//! 고유 착용 아이템인지 점검;
	const bool  IsUnique() const;

    //! 착용 가능한 아이템인지 점검
    const bool IsSuitable() const;

	// 미리보기 가능한 아이템?
	const bool IsPreview() const;

	//! 겹침 가능한 아이템인지 점검.
    inline bool ISPILE() const { return (sDrugOp.bInstance && (sDrugOp.wPileNum>1)); }

    //! 아이템 타입
    inline EMITEM_TYPE BasicType() const { return sBasicOp.Type(); }

    //! 약품 타입
    inline EMITEM_DRUG DrugType() const { return sDrugOp.GetType(); }

    inline SNATIVEID BasicId() const { return sBasicOp.GetNativeID(); }

	//inline bool ISEVENTITEM () const { return (sBasicOp.dwFlags&TRADE_EVENT_SGL) != NULL; }

    //! 아이템 착용 타입
    inline EMSUIT Suit() const { return sSuitOp.Suit(); }

    //! Point 카드일경우 Point 충전량
    inline DWORD GetPointCharge() const { return sDrugOp.GetPointCharge(); }

	WORD GETAPPLYNUM () const;

	LONGLONG GETSELLPRICE ( WORD wTurnNum ) const
	{
		if ( ISPILE() )
		{
			WORD wAPPLYNUM = GETAPPLYNUM();
			LONGLONG lPRICE = LONGLONG(sBasicOp.dwSellPrice * wTurnNum / float(wAPPLYNUM));
			return lPRICE;
		}

		return sBasicOp.dwSellPrice;
	}

	inline float GetExpMultiple() const	{ return sBasicOp.fExpMultiple; }

	const char* GetName() const;
    const std::string GetNameStr() const;
	const char* GetSelfBodyEffect() const { return sBasicOp.strSelfBodyEffect.c_str(); }
	const char* GetTargBodyEffect() const { return sBasicOp.strTargBodyEffect.c_str(); }
	const char* GetTargetEffect() const { return sBasicOp.strTargetEffect.c_str(); }
	const char* GetGeneralEffect() const { return sBasicOp.strGeneralEffect.c_str(); }

	const char* GetFieldFile() const { return sBasicOp.strFieldFile.c_str(); }
	const char* GetInventoryFile() const { return sBasicOp.strInventoryFile.c_str(); }
	const char* GetRandomOptFile() const { return sRandomOpt.szNAME; }

	const char* GetWearingFile(EMCHARINDEX emIndex) const { return sBasicOp.strWearingFile[emIndex].c_str(); }
	const char* GetWearingFileEx(EMCHARINDEX emIndex) const { return sBasicOp.strWearingFileEx[emIndex].c_str(); }
	const char* GetSubInventoryFile(EMCHARINDEX emIndex) const { return sBasicOp.strArrInventoryFile[emIndex].c_str(); }
	const char* GetComment();

	const std::string& GetGrindingOptFile() const { return sGrindingOp.strGRINDER_OPT_FILE; }

    ////! 개인간 거래가능한가
    //inline bool CanExchange() const { return sBasicOp.IsEXCHANGE(); }

    inline VEHICLE_TYPE VehicleType() const { return sVehicle.Type(); }

	// PET
	const char* GetPetWearingFile() const { return sBasicOp.strPetWearingFile.c_str(); }

	inline const EMITEMLEVEL GetItemLevel ( void ) const { return sBasicOp.GetItemLevel(); }

	// 기본 공격력, 방어력;
	inline const GLPADATA GetAttackDamage ( void ) const { return sSuitOp.GetAttackDamage(); }
	inline const WORD GetDefence ( void ) const { return sSuitOp.GetDefence(); }

	inline const EMITEM_SKILL_LINK GetLinkSkillTarget ( void ) const { return sItemSkill.GetTarget(); }
	inline const SNATIVEID& GetLinkSkillID ( void ) const { return sItemSkill.GetSkillID(); }
	inline const WORD GetLinkSkillLevel ( void ) const { return sItemSkill.GetSkillLv(); }
	inline const float GetLinkSkillOccurRate ( void ) const { return sItemSkill.GetOccurRate(); }

	// 기본 공격력, 방어력 랜덤 세트 매니저 Index;
	inline const int GetBasicAttackDamageType ( void ) const { return sSuitOp.GetBasicAttackDamageType(); }
	inline const int GetBasicAttackDamageIndex ( void ) const { return sSuitOp.GetBasicAttackDamageIndex(); }
	inline const int GetBasicDefenceType ( void ) const { return sSuitOp.GetBasicDefenceType(); }
	inline const int GetBasicDefenceIndex ( void ) const { return sSuitOp.GetBasicDefenceIndex(); }

	// 링크 스킬 랜덤 세트 Index;
	inline const int GetLinkSkillIndex ( void ) const { return sItemSkill.GetLinkSkillIndex(); }

	// 기본 공격력, 방어력 개조 가능 여부;
	inline const bool IsEditableBasicAttackDamage ( void ) const { return sSuitOp.IsEditableBasicAttackDamage(); }
	inline const bool IsEditableBasicDefence ( void ) const { return sSuitOp.IsEditableBasicDefence(); }
};

struct SITEM_LOBY_101
{
public:
	SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
	SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.

	LONGLONG	lnGenNum;					//	생성 번호.
	WORD		wTurnNum;					//	사용가능량. ( wUsedNum )
	BYTE		cGenType;					//	생성 타입.
	BYTE		cChnID;						//	채널 번호.

	BYTE		cFieldID;					//	필드 번호.

public:
	BYTE		cDAMAGE;					//	공격력 개조 등급.
	BYTE		cDEFENSE;					//	방어력 개조 등급.
	BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
	BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
	BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
	BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
	BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.

	//	랜덤 옵션 사용.
	BYTE		cOptTYPE1; // 공격력
	BYTE		cOptTYPE2; // 기저항
	BYTE		cOptTYPE3;
	BYTE		cOptTYPE4;

	short		nOptVALUE1; // 공격력 %
	short		nOptVALUE2; // 기저항 %
	short		nOptVALUE3;
	short		nOptVALUE4;
};

struct SITEM_LOBY_102
{	// by luxes.
public:
    SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
    SNATIVEID	nidDISGUISE;				//	코스툼 복장 스킨.

    LONGLONG	lnGenNum;					//	생성 번호.
    WORD		wTurnNum;					//	사용가능량. ( wUsedNum )
    BYTE		cGenType;					//	생성 타입.
    BYTE		cChnID;						//	채널 번호.

    BYTE		cFieldID;					//	필드 번호.

public:
    BYTE		cDAMAGE;					//	공격력 개조 등급.
    BYTE		cDEFENSE;					//	방어력 개조 등급.
    BYTE		cRESIST_FIRE;				//	저항(화) 개조 등급.
    BYTE		cRESIST_ICE;				//	저항(빙) 개조 등급.
    BYTE		cRESIST_ELEC;				//	저항(전) 개조 등급.
    BYTE		cRESIST_POISON;				//	저항(독) 개조 등급.
    BYTE		cRESIST_SPIRIT;				//	저항(정) 개조 등급.

    //	랜덤 옵션 사용.
    BYTE		cOptTYPE1; // 공격력
    BYTE		cOptTYPE2; // 기저항
    BYTE		cOptTYPE3;
    BYTE		cOptTYPE4;

    short		nOptVALUE1; // 공격력 %
    short		nOptVALUE2; // 기저항 %
    short		nOptVALUE3;
    short		nOptVALUE4;

    // 염색 코스튬 정보. by luxes.
    DWORD		dwMainColor;
    DWORD		dwSubColor;

public:
    SITEM_LOBY_102();
    SITEM_LOBY_102(const SNATIVEID& sNID);

    bool operator == (const SITEM_LOBY_102& value);
    bool operator != (const SITEM_LOBY_102& value);

    void Assign(const SITEMCUSTOM& sItemCustom);
};

struct SITEM_LOBY
{	// by luxes.
public:
	enum { VERSION = 0x0103 };

	SNATIVEID	sNativeID;   //! 고유 ID. (아이템 설정 최초 생성시에 부여되는 고유 ID)
	SNATIVEID	nidDISGUISE; //! 코스툼 복장 스킨.
	
public:    
	DWORD dwMainColor; //! 염색 코스튬 정보. by luxes.
	DWORD dwSubColor; //! 염색 코스튬 정보. by luxes.

    BYTE cDAMAGE;  //! 공격력 개조 등급.
    BYTE cDEFENSE; //! 방어력 개조 등급.

    MSGPACK_DEFINE(sNativeID, nidDISGUISE, dwMainColor, dwSubColor, cDAMAGE, cDEFENSE);

public:
	SITEM_LOBY();
	SITEM_LOBY(const SNATIVEID&	sNID);

	bool operator == (const SITEM_LOBY &value);
	bool operator != (const SITEM_LOBY &value);

	void Assign(const SITEMCUSTOM& sItemCustom);
};

BOOL SETPUTONITEMS_BYBUF(SITEM_LOBY* pPutOnItems, se::ByteStream& ByteStream);
BOOL SETPUTONITEMS_BYBUF(std::vector<SITEM_LOBY>& vPutOnItems, se::ByteStream& ByteStream);
BOOL SETPUTONITEMS_BYBUF_FOR_MIGRATION( std::vector< SITEM_LOBY >& vPutOnItems, se::ByteStream& ByteStream );

//	Note : 클라이언트용 간략 정보. ( 플래이어 자신 것 이외의 경우. )
//
struct SITEMCLIENT
{
	SNATIVEID sNativeID;
	SNATIVEID nidDISGUISE;

    // 염색 코스튬 정보. by luxes.
    DWORD dwMainColor;
    DWORD dwSubColor;

    WORD		wTurnNum;
    short		nOptVALUE1; // 이속랜덤옵션만 받아옵니다.
    BYTE		cOptTYPE1; // 이속랜덤옵션만 받아옵니다.
	BYTE		cGRADE;

    MSGPACK_DEFINE(
        sNativeID,
        nidDISGUISE, 
        dwMainColor,
        dwSubColor,
        wTurnNum,
        nOptVALUE1,
        cOptTYPE1,
        cGRADE);

	SITEMCLIENT() 
		: sNativeID(false)
		, nidDISGUISE(false)
		, cGRADE(0)
		, wTurnNum(0)
		, cOptTYPE1(0) 
		, nOptVALUE1(0) 
		
		, dwMainColor(0)
		, dwSubColor(0)
	{
	}

	SITEMCLIENT(const SNATIVEID& sNID)
		: sNativeID(sNID)
		, nidDISGUISE(false)
		, cGRADE(0)
		, wTurnNum(0)
		, cOptTYPE1(0) 
		, nOptVALUE1(0)
		, dwMainColor(0)
		, dwSubColor(0)
	{
	}	

	SITEMCLIENT& Assign(const SITEMCUSTOM& Input)
	{
		sNativeID = Input.GetBoxWrappedItemID();
		nidDISGUISE = Input.nidDISGUISE;
		wTurnNum = Input.wTurnNum;
		cGRADE = Input.grade_damage()>Input.grade_defense() ? Input.grade_damage() : Input.grade_defense();

		nOptVALUE1 = Input.GETOptVALUE_SHORT( RandomOption::RANDOM_OPTION_RATE_SPEED_MOVE );		
		if ( nOptVALUE1 != 0 )
            cOptTYPE1 = (BYTE) RandomOption::RANDOM_OPTION_RATE_SPEED_MOVE;

		dwMainColor = Input.dwMainColor;
		dwSubColor = Input.dwSubColor;
		
		return *this;
	}

	RandomOption::RANDOM_OPTION GETOptTYPE1() const { return (RandomOption::RANDOM_OPTION) cOptTYPE1; }
	BYTE GETGRADE() const { return cGRADE; }
	BYTE GETGRADE_EFFECT() const;
};

//	Note : 아이템이 필드에 떨어졌을 때의 정보.
//
struct SDROP_ITEM
{
	SITEMCUSTOM	sItemCustom;			//	아이템 대이터.

	SNATIVEID	sMapID;					//	맵 ID.
	DWORD		dwCeID;					//	셀 ID.
	DWORD		dwGlobID;				//	생성 메모리 인덱스용.
	//D3DXVECTOR3	vPos;					//	맵 위치.
    se::Vector3 vPos;

    MSGPACK_DEFINE(sItemCustom, sMapID, dwCeID, dwGlobID, vPos);

	SDROP_ITEM ()
		: dwCeID(0)
		, dwGlobID(0)
		, vPos(0,0,0)
	{
	}
};

struct SDROP_CLIENT_ITEM
{
	SITEMCLIENT	sItemClient;
	DWORD		dwGlobID;	// 생성 메모리 인덱스용
	//D3DXVECTOR3	vPos;		// 맵 위치
    se::Vector3 vPos;
    float		fAge;
	BOOL		bCanGet;	// 아이템 습득 가능 여부

    MSGPACK_DEFINE(sItemClient, dwGlobID, vPos, fAge, bCanGet);

	SDROP_CLIENT_ITEM()
		: dwGlobID(0)
		, vPos(0,0,0)
		, fAge(0)
		, bCanGet(FALSE)
	{
	}

	SDROP_CLIENT_ITEM& Assign(SDROP_ITEM& Input)
	{
		sItemClient.Assign(Input.sItemCustom);
		dwGlobID = Input.dwGlobID;
		vPos = Input.vPos;
		return *this;
	}
};

#ifndef GLLandMan
class GLLandMan;
class GLLandNode;
typedef SQUADNODE<GLLandNode> LANDQUADNODE;
#endif //GLLandMan

#ifndef GLLandManClient
class GLClientNode;
class GLLandNode;
typedef SQUADNODE<GLClientNode>		CLIENTQUADNODE;
#endif //GLLandMan

//	Note : Server 용 ItemDrop 관리 클래스.
//
class CItemDrop
{
public:
	union
	{
		struct
		{
			SDROP_ITEM			sDrop;			//	드롭 정보.
		};

		struct
		{
			SITEMCUSTOM			sItemCustom;	//	아이템 대이터.

			SNATIVEID			sMapID;			//	맵 ID.
			DWORD				dwCeID;			//	셀 ID.
			DWORD				dwGlobID;		//	생성 메모리 인덱스용.
			D3DXVECTOR3			vPos;			//	맵 위치.
		};
	};

public:
	float					fAge;			//	경과 시간.

	EMGROUP					emGroup;		//	임시 소유권자 단위.
	DWORD					dwHoldGID;		//	임시 소유권자.

	LANDQUADNODE*			pQuadNode;		//	쿼드 트리 노드.
	sc::SGLNODE<CItemDrop*>*	pCellList;		//	셀 리스트 노드.
	//sc::SGLNODE<CItemDrop*>*	pGlobList;		//	전역 리스트 노드.

	bool					bLock;

public:
	bool IsTakeItem(const GLPartyID& PartyID, const DWORD dwGaeaID);
	bool IsDropOut();

	void Update ( float fElapsedTime );

public:
	CItemDrop(void)
		: fAge(0.0f)
		
		, emGroup(EMGROUP_ONE)
		, dwHoldGID(GAEAID_NULL)
		
		, pQuadNode(NULL)
		, pCellList(NULL)
		, bLock( false )
		//, pGlobList(NULL)
	  {
	  }
};

typedef CItemDrop* PITEMDROP;

typedef sc::CGLLIST<PITEMDROP>	ITEMDROPLIST;
typedef sc::SGLNODE<PITEMDROP>	ITEMDROPNODE;


//	Note : Client 용 ItemDrop 관리 클래스.
//
class DxSimMesh;
class DxLandMan;
struct CLIPVOLUME;
struct ActorNameCollision;

class CItemClientDrop
{
public:
	union
	{
		struct
		{
			SDROP_CLIENT_ITEM	sDrop;			//	드롭 정보.
		};

		struct
		{
			SITEMCLIENT			sItemClient;	//	아이템 대이터.

			SNATIVEID			sMapID;			//	맵 ID.
			DWORD				dwCeID;			//	셀 ID.
			DWORD				dwGlobID;		//	생성 메모리 인덱스용.
			D3DXVECTOR3			vPos;			//	맵 위치.

			BOOL				bCanGet;				//  아이템 습득 가능 여부
		};
	};

public:
	float						fAge;			//	경과 시간.
	float						fNextMsgDelay;	//	이미 주을려고 메시지 보냇을 경우 다음으로 시도가능시간 설정.

	D3DXMATRIX					matWld;			//	아이탬 위치.
	D3DXVECTOR3					vMax;
	D3DXVECTOR3					vMin;

	DxSimMesh*					pSimMesh;

	//CLIENTQUADNODE*				pQuadNode;		//	쿼드 트리 노드.
	//sc::SGLNODE<CItemClientDrop*>*	pCellList;		//	셀 리스트 노드.
	//sc::SGLNODE<CItemClientDrop*>*	pGlobList;		//	전역 리스트 노드.

private:
	std::tr1::shared_ptr<ActorNameCollision>	m_spNameCollision;

public:
	CItemClientDrop (void);		

	BOOL	IsVisibleDisplayName() const;
	bool IsCollision ( const D3DXVECTOR3 &vFromPt, const D3DXVECTOR3 &vTargetPt ) const;
	bool IsCollision ( const CLIPVOLUME &cv ) const;
	void FrameMove( const boost::shared_ptr<DxLandMan>& spLandMan, float fElapsedTime );
	HRESULT RenderItem ( LPDIRECT3DDEVICEQ pd3dDevice, D3DMATERIALQ* pMaterials=NULL );
};

typedef CItemClientDrop*			PITEMCLIENTDROP;

typedef sc::CGLLIST<PITEMCLIENTDROP>	ITEMCLIENTDROPLIST;
typedef sc::SGLNODE<PITEMCLIENTDROP>	ITEMCLIENTDROPNODE;


//	Note : 각종 아이템 정보 관리, 리스트 관리 클래스.
//
enum EMITEMMAN
{
	ITEMMAN_EDITOR		= 0x0001,
	ITEMID_NOTFOUND		= 0xFFFF
};


//	Note : 인벤토리에 아이템을 넣을때 저장되는 구조체.
//
struct SINVENITEM_SAVE_100
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_100	sItemCustom;
};

struct SINVENITEM_SAVE_101
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_101	sItemCustom;
};

struct SINVENITEM_SAVE_102
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_102	sItemCustom;
};

struct SINVENITEM_SAVE_103
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_103	sItemCustom;
};

struct SINVENITEM_SAVE_104
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_104	sItemCustom;
};

struct SINVENITEM_SAVE_105
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_105	sItemCustom;
};

struct SINVENITEM_SAVE_106
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_106	sItemCustom;
};

struct SINVENITEM_SAVE_107
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_107	sItemCustom;
};

struct SINVENITEM_SAVE_108
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_108	sItemCustom;
};

struct SINVENITEM_SAVE_109
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_109	sItemCustom;
};

struct SINVENITEM_SAVE_110
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_110	sItemCustom;
};

struct SINVENITEM_SAVE_111
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_111	sItemCustom;
};

struct SINVENITEM_SAVE_112
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_112	sItemCustom;
};

struct SINVENITEM_SAVE_113
{
    WORD		wPosX;
    WORD		wPosY;

    SITEMCUSTOM_113	sItemCustom;
};

struct SINVENITEM_SAVE_114
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_114	sItemCustom;
};

struct SINVENITEM_SAVE_115
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_115	sItemCustom;
};

struct SINVENITEM_SAVE_116
{
    WORD		wPosX;
    WORD		wPosY;

    SITEMCUSTOM_116	sItemCustom;
};

struct SINVENITEM_SAVE_117
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_117	sItemCustom;
};

struct SINVENITEM_SAVE_118
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_118	sItemCustom;
};

struct SINVENITEM_SAVE_119
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_119	sItemCustom;
};

struct SINVENITEM_SAVE_120
{
	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM_120	sItemCustom;
};

struct SINVENITEM_SAVE
{
	enum { VERSION = 0x0121 };

	WORD		wPosX;
	WORD		wPosY;

	SITEMCUSTOM	sItemCustom;

    MSGPACK_DEFINE(wPosX, wPosY, sItemCustom);

	SINVENITEM_SAVE ()
		: wPosX(0)
		, wPosY(0)
	{
	}

	SINVENITEM_SAVE( const SITEMCUSTOM& sItem, WORD PosX, WORD PosY )
		: wPosX( PosX )
		, wPosY( PosY )
		, sItemCustom( sItem )
	{
	}

	void Assign ( const SINVENITEM_SAVE_100 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_101 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_102 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_103 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_104 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_105 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_106 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_107 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_108 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_109 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_110 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_111 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_112 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

    void Assign ( const SINVENITEM_SAVE_113 &sOld )
    {
        wPosX = sOld.wPosX;
        wPosY = sOld.wPosY;

        sItemCustom.Assign ( sOld.sItemCustom );
    }

	void Assign ( const SINVENITEM_SAVE_114 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_115 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

    void Assign ( const SINVENITEM_SAVE_116 &sOld )
    {
        wPosX = sOld.wPosX;
        wPosY = sOld.wPosY;

        sItemCustom.Assign ( sOld.sItemCustom );
    }

	void Assign ( const SINVENITEM_SAVE_117 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_118 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_119 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}

	void Assign ( const SINVENITEM_SAVE_120 &sOld )
	{
		wPosX = sOld.wPosX;
		wPosY = sOld.wPosY;

		sItemCustom.Assign ( sOld.sItemCustom );
	}
};

//! 아이템을 인벤토리에 넣을때 쓰는 구조.
struct SINVENITEM
{
	union
	{
		struct
		{
			SINVENITEM_SAVE		sSaveData;
		};

		struct
		{
			WORD				wPosX;
			WORD				wPosY;

			SITEMCUSTOM			sItemCustom;
		};
	};

	WORD					wBackX;
	WORD					wBackY;

	bool					bNew;

    MSGPACK_DEFINE(wPosX, wPosY, sItemCustom, wBackX, wBackY);

	SINVENITEM () 
		: wBackX(0)
		, wBackY(0)
		, bNew( false )
	{
	}

	SINVENITEM ( WORD _wPosX, WORD _wPosY ) 
		: wBackX( _wPosX )
		, wBackY( _wPosY )
		, bNew( false )
	{
	}

	bool operator== (const SINVENITEM &value )
	{
		return ( wPosX==value.wPosX&&wPosY==value.wPosY&&sItemCustom==value.sItemCustom);
	}

	bool operator!= (const SINVENITEM &value )
	{
		return ( !operator==(value) );
	}

    inline SNATIVEID GetNativeID() const { return sItemCustom.GetNativeID(); }
    inline WORD Mid() const { return sItemCustom.Mid(); }
    inline WORD Sid() const { return sItemCustom.Sid(); }
    inline DWORD Id() const { return sItemCustom.Id(); }
    inline const SITEMCUSTOM& CustomItem() const { return sItemCustom; }
    inline WORD TurnNum() const { return sItemCustom.TurnNum(); }
    
    //! 탈것 ID (DB번호)
    inline DWORD VehicleDbNum() const { return sItemCustom.VehicleDbNum(); }

    //! GMCharEdit 로 생성된 아이템인지 검사한다.
    bool IsGM_GENITEM() const { return sItemCustom.IsGM_GENITEM(); }

	const int GetDbState() const { return sItemCustom.GetDbState(); }
	void SetDbState( int emDbState, bool bForce = false ) { sItemCustom.SetDbState( emDbState, bForce ); }

	sc::MGUID GetGuid() const { return sItemCustom.GetGuid(); }

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & wPosX;
		ar & wPosY;
		ar & sItemCustom;
		ar & wBackX;
		ar & wBackY;
	}
};

struct SINVEN_POS	// ITEMREBUILD_MARK
{
	union
	{
		struct
		{
			DWORD	dwPos;
		};

		struct
		{
			WORD	wPosX;		//	주 ID.
			WORD	wPosY;		//	보조 ID.
		};
	};

	SINVEN_POS()
		: wPosX( USHRT_MAX )
		, wPosY( USHRT_MAX )
	{
	}
	SINVEN_POS( WORD _wposx, WORD _wposy )
		: wPosX( _wposx )
		, wPosY( _wposy )
	{
	}
	VOID SET( WORD _wposx, WORD _wposy )
	{
		wPosX = _wposx;
		wPosY = _wposy;
	}
	BOOL VALID() const
	{
		return (BOOL)( wPosX != USHRT_MAX && wPosY != USHRT_MAX );
	}
	VOID RESET()
	{
		wPosX = USHRT_MAX;
		wPosY = USHRT_MAX;
	}

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & dwPos;
	}
};

struct SINVEN_MOVE_ITEM_SORT
{
	WORD nOrigin;
	WORD nTarget;

};

struct SITEM_NUM
{
	SNATIVEID	sNativeID; //! 아이템 번호
	DWORD		dwNum;     //! 아이템 개수
	
	SITEM_NUM()
        : sNativeID(false)
        , dwNum(0)
	{		
	}

    SITEM_NUM(SNATIVEID _ItemId, DWORD _ItemNum)
        : sNativeID(_ItemId)
        , dwNum(_ItemNum)
    {
    }

	void Reset() 
	{
		sNativeID = false;
		dwNum = 0;
	}

	bool IsValid()
	{
		if (sNativeID == NATIVEID_NULL() || dwNum == 0)
            return false;
        else
		    return true;
	}

	bool operator== ( SITEM_NUM &value )
	{
		return ( sNativeID== value.sNativeID && dwNum == value.dwNum );
	}

    //! 아이템 번호
    SNATIVEID GetNativeId() const { return sNativeID; }

    //! 아이템 개수
    DWORD GetCount() const { return dwNum; }
};

BOOL SETPUTONITEMS_VECTOR_NEW( std::vector< SITEM_LOBY >& vPutOnItems, std::vector< SINVENITEM_SAVE >& vItems );

namespace RebuyItem
{
    struct SREBUY_ITEM
    {
        SITEMCUSTOM sItemCustom;
        LONGLONG nPrice;
        bool bGetState;

        SREBUY_ITEM( SITEMCUSTOM _sItemCustom, LONGLONG _nPrice )
            : sItemCustom(_sItemCustom)
            , nPrice(_nPrice)
            , bGetState(false)
        {
        }

        inline SITEMCUSTOM& ItemCustom()
        {
            return sItemCustom;
        }

        inline LONGLONG Price()
        {
            return nPrice;
        }
    };

    typedef std::vector<SREBUY_ITEM>	REBUY_VEC;
    typedef REBUY_VEC::iterator			REBUY_VEC_ITER;
	typedef REBUY_VEC::const_iterator	REBUY_VEC_CITER;
}


namespace COMMENT
{
	extern std::string ADDON_NO[ITEM::SSUIT::ADDON_SIZE];
}

struct DELETED_ITEM
{
	sc::MGUID guid;
	int DbState;

	DELETED_ITEM()
		: DbState( 0 )
	{
	}
};

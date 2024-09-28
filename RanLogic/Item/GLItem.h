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
 
//! ������ ���� ����
struct SITEM
{
	enum
	{
		VERSION = 0x0107,

		FILE_SBASIC			= 1,	// �⺻������
		FILE_SSUIT			= 2,	// �Ǻ�
		FILE_SDRUG			= 3,	// ��ǰ
		FILE_SSKILLBOOK		= 4,	// ��ų��
		FILE_SGRINDING		= 5,	// ����������
		FILE_SGENERATE		= 6,	// 
		FILE_BOX			= 7,	// ? ��������
		FILE_RANDOMBOX		= 8,	// �����ڽ�
		FILE_QUESTIONITEM	= 9,	// Question Item
		FILE_RANDOMITEM		= 10,	// ����������
		FILE_PET			= 11,	// �� ������	// PetData
		FILE_VEHICLE		= 12,   // Ż �� 
		FILE_PETSKINPACK	= 13,	// �� ��Ų��
		FILE_SPLIT			= 14,
		FILE_SKILL			= 15,

		FILE_END_DATA		= 0xEDEDEDED,
	};

private:
	void LoadAfter();
	std::string	strRealName_NO_SAVE;		// �������� ���� ��Ʈ�� -- �������� ������ ���� 

public:
	ITEM::SBASIC		sBasicOp;
	ITEM::SSUIT			sSuitOp;
	ITEM::SDRUG			sDrugOp;
	ITEM::SSKILLBOOK	sSkillBookOp;
	ITEM::SGRINDING		sGrindingOp;
	ITEM::SGENERATE		sGenerateOp;
	ITEM::SBOX			sBox; //! ��������
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

	// LOAD�Լ��� �׻� ���� ���� �ʿ� ����.
	BOOL Load104(sc::BaseStream& SFile);		// SRANDOM_OPT Load��� ����� 
	//BOOL Load105(sc::BaseStream& SFile);
	BOOL LoadCurrent(sc::BaseStream& SFile);	// ���� ���� �б�

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
	//! ���� ������ ���������� ����
	const bool IsColorChange() const;

	//! ���� ���� ���������� ����;
	const bool  IsUnique() const;

    //! ���� ������ ���������� ����
    const bool IsSuitable() const;

	// �̸����� ������ ������?
	const bool IsPreview() const;

	//! ��ħ ������ ���������� ����.
    inline bool ISPILE() const { return (sDrugOp.bInstance && (sDrugOp.wPileNum>1)); }

    //! ������ Ÿ��
    inline EMITEM_TYPE BasicType() const { return sBasicOp.Type(); }

    //! ��ǰ Ÿ��
    inline EMITEM_DRUG DrugType() const { return sDrugOp.GetType(); }

    inline SNATIVEID BasicId() const { return sBasicOp.GetNativeID(); }

	//inline bool ISEVENTITEM () const { return (sBasicOp.dwFlags&TRADE_EVENT_SGL) != NULL; }

    //! ������ ���� Ÿ��
    inline EMSUIT Suit() const { return sSuitOp.Suit(); }

    //! Point ī���ϰ�� Point ������
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

    ////! ���ΰ� �ŷ������Ѱ�
    //inline bool CanExchange() const { return sBasicOp.IsEXCHANGE(); }

    inline VEHICLE_TYPE VehicleType() const { return sVehicle.Type(); }

	// PET
	const char* GetPetWearingFile() const { return sBasicOp.strPetWearingFile.c_str(); }

	inline const EMITEMLEVEL GetItemLevel ( void ) const { return sBasicOp.GetItemLevel(); }

	// �⺻ ���ݷ�, ����;
	inline const GLPADATA GetAttackDamage ( void ) const { return sSuitOp.GetAttackDamage(); }
	inline const WORD GetDefence ( void ) const { return sSuitOp.GetDefence(); }

	inline const EMITEM_SKILL_LINK GetLinkSkillTarget ( void ) const { return sItemSkill.GetTarget(); }
	inline const SNATIVEID& GetLinkSkillID ( void ) const { return sItemSkill.GetSkillID(); }
	inline const WORD GetLinkSkillLevel ( void ) const { return sItemSkill.GetSkillLv(); }
	inline const float GetLinkSkillOccurRate ( void ) const { return sItemSkill.GetOccurRate(); }

	// �⺻ ���ݷ�, ���� ���� ��Ʈ �Ŵ��� Index;
	inline const int GetBasicAttackDamageType ( void ) const { return sSuitOp.GetBasicAttackDamageType(); }
	inline const int GetBasicAttackDamageIndex ( void ) const { return sSuitOp.GetBasicAttackDamageIndex(); }
	inline const int GetBasicDefenceType ( void ) const { return sSuitOp.GetBasicDefenceType(); }
	inline const int GetBasicDefenceIndex ( void ) const { return sSuitOp.GetBasicDefenceIndex(); }

	// ��ũ ��ų ���� ��Ʈ Index;
	inline const int GetLinkSkillIndex ( void ) const { return sItemSkill.GetLinkSkillIndex(); }

	// �⺻ ���ݷ�, ���� ���� ���� ����;
	inline const bool IsEditableBasicAttackDamage ( void ) const { return sSuitOp.IsEditableBasicAttackDamage(); }
	inline const bool IsEditableBasicDefence ( void ) const { return sSuitOp.IsEditableBasicDefence(); }
};

struct SITEM_LOBY_101
{
public:
	SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
	SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.

	LONGLONG	lnGenNum;					//	���� ��ȣ.
	WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )
	BYTE		cGenType;					//	���� Ÿ��.
	BYTE		cChnID;						//	ä�� ��ȣ.

	BYTE		cFieldID;					//	�ʵ� ��ȣ.

public:
	BYTE		cDAMAGE;					//	���ݷ� ���� ���.
	BYTE		cDEFENSE;					//	���� ���� ���.
	BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
	BYTE		cRESIST_ICE;				//	����(��) ���� ���.
	BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
	BYTE		cRESIST_POISON;				//	����(��) ���� ���.
	BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.

	//	���� �ɼ� ���.
	BYTE		cOptTYPE1; // ���ݷ�
	BYTE		cOptTYPE2; // ������
	BYTE		cOptTYPE3;
	BYTE		cOptTYPE4;

	short		nOptVALUE1; // ���ݷ� %
	short		nOptVALUE2; // ������ %
	short		nOptVALUE3;
	short		nOptVALUE4;
};

struct SITEM_LOBY_102
{	// by luxes.
public:
    SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
    SNATIVEID	nidDISGUISE;				//	�ڽ��� ���� ��Ų.

    LONGLONG	lnGenNum;					//	���� ��ȣ.
    WORD		wTurnNum;					//	��밡�ɷ�. ( wUsedNum )
    BYTE		cGenType;					//	���� Ÿ��.
    BYTE		cChnID;						//	ä�� ��ȣ.

    BYTE		cFieldID;					//	�ʵ� ��ȣ.

public:
    BYTE		cDAMAGE;					//	���ݷ� ���� ���.
    BYTE		cDEFENSE;					//	���� ���� ���.
    BYTE		cRESIST_FIRE;				//	����(ȭ) ���� ���.
    BYTE		cRESIST_ICE;				//	����(��) ���� ���.
    BYTE		cRESIST_ELEC;				//	����(��) ���� ���.
    BYTE		cRESIST_POISON;				//	����(��) ���� ���.
    BYTE		cRESIST_SPIRIT;				//	����(��) ���� ���.

    //	���� �ɼ� ���.
    BYTE		cOptTYPE1; // ���ݷ�
    BYTE		cOptTYPE2; // ������
    BYTE		cOptTYPE3;
    BYTE		cOptTYPE4;

    short		nOptVALUE1; // ���ݷ� %
    short		nOptVALUE2; // ������ %
    short		nOptVALUE3;
    short		nOptVALUE4;

    // ���� �ڽ�Ƭ ����. by luxes.
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

	SNATIVEID	sNativeID;   //! ���� ID. (������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID)
	SNATIVEID	nidDISGUISE; //! �ڽ��� ���� ��Ų.
	
public:    
	DWORD dwMainColor; //! ���� �ڽ�Ƭ ����. by luxes.
	DWORD dwSubColor; //! ���� �ڽ�Ƭ ����. by luxes.

    BYTE cDAMAGE;  //! ���ݷ� ���� ���.
    BYTE cDEFENSE; //! ���� ���� ���.

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

//	Note : Ŭ���̾�Ʈ�� ���� ����. ( �÷��̾� �ڽ� �� �̿��� ���. )
//
struct SITEMCLIENT
{
	SNATIVEID sNativeID;
	SNATIVEID nidDISGUISE;

    // ���� �ڽ�Ƭ ����. by luxes.
    DWORD dwMainColor;
    DWORD dwSubColor;

    WORD		wTurnNum;
    short		nOptVALUE1; // �̼ӷ����ɼǸ� �޾ƿɴϴ�.
    BYTE		cOptTYPE1; // �̼ӷ����ɼǸ� �޾ƿɴϴ�.
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

//	Note : �������� �ʵ忡 �������� ���� ����.
//
struct SDROP_ITEM
{
	SITEMCUSTOM	sItemCustom;			//	������ ������.

	SNATIVEID	sMapID;					//	�� ID.
	DWORD		dwCeID;					//	�� ID.
	DWORD		dwGlobID;				//	���� �޸� �ε�����.
	//D3DXVECTOR3	vPos;					//	�� ��ġ.
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
	DWORD		dwGlobID;	// ���� �޸� �ε�����
	//D3DXVECTOR3	vPos;		// �� ��ġ
    se::Vector3 vPos;
    float		fAge;
	BOOL		bCanGet;	// ������ ���� ���� ����

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

//	Note : Server �� ItemDrop ���� Ŭ����.
//
class CItemDrop
{
public:
	union
	{
		struct
		{
			SDROP_ITEM			sDrop;			//	��� ����.
		};

		struct
		{
			SITEMCUSTOM			sItemCustom;	//	������ ������.

			SNATIVEID			sMapID;			//	�� ID.
			DWORD				dwCeID;			//	�� ID.
			DWORD				dwGlobID;		//	���� �޸� �ε�����.
			D3DXVECTOR3			vPos;			//	�� ��ġ.
		};
	};

public:
	float					fAge;			//	��� �ð�.

	EMGROUP					emGroup;		//	�ӽ� �������� ����.
	DWORD					dwHoldGID;		//	�ӽ� ��������.

	LANDQUADNODE*			pQuadNode;		//	���� Ʈ�� ���.
	sc::SGLNODE<CItemDrop*>*	pCellList;		//	�� ����Ʈ ���.
	//sc::SGLNODE<CItemDrop*>*	pGlobList;		//	���� ����Ʈ ���.

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


//	Note : Client �� ItemDrop ���� Ŭ����.
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
			SDROP_CLIENT_ITEM	sDrop;			//	��� ����.
		};

		struct
		{
			SITEMCLIENT			sItemClient;	//	������ ������.

			SNATIVEID			sMapID;			//	�� ID.
			DWORD				dwCeID;			//	�� ID.
			DWORD				dwGlobID;		//	���� �޸� �ε�����.
			D3DXVECTOR3			vPos;			//	�� ��ġ.

			BOOL				bCanGet;				//  ������ ���� ���� ����
		};
	};

public:
	float						fAge;			//	��� �ð�.
	float						fNextMsgDelay;	//	�̹� �������� �޽��� ������ ��� �������� �õ����ɽð� ����.

	D3DXMATRIX					matWld;			//	������ ��ġ.
	D3DXVECTOR3					vMax;
	D3DXVECTOR3					vMin;

	DxSimMesh*					pSimMesh;

	//CLIENTQUADNODE*				pQuadNode;		//	���� Ʈ�� ���.
	//sc::SGLNODE<CItemClientDrop*>*	pCellList;		//	�� ����Ʈ ���.
	//sc::SGLNODE<CItemClientDrop*>*	pGlobList;		//	���� ����Ʈ ���.

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


//	Note : ���� ������ ���� ����, ����Ʈ ���� Ŭ����.
//
enum EMITEMMAN
{
	ITEMMAN_EDITOR		= 0x0001,
	ITEMID_NOTFOUND		= 0xFFFF
};


//	Note : �κ��丮�� �������� ������ ����Ǵ� ����ü.
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

//! �������� �κ��丮�� ������ ���� ����.
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
    
    //! Ż�� ID (DB��ȣ)
    inline DWORD VehicleDbNum() const { return sItemCustom.VehicleDbNum(); }

    //! GMCharEdit �� ������ ���������� �˻��Ѵ�.
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
			WORD	wPosX;		//	�� ID.
			WORD	wPosY;		//	���� ID.
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
	SNATIVEID	sNativeID; //! ������ ��ȣ
	DWORD		dwNum;     //! ������ ����
	
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

    //! ������ ��ȣ
    SNATIVEID GetNativeId() const { return sNativeID; }

    //! ������ ����
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

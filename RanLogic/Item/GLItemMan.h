#ifndef _GL_ITEM_MAN_H_
#define _GL_ITEM_MAN_H_

#pragma once

#include <string>
#include <boost/function.hpp>
#include "./GLItem.h"
#include "../../EngineLib/DxMeshs/DxSimpleMeshMan.h"
#include "../../SigmaCore/Memory/ObjectPool.h"

namespace sc
{
    class Excel;
}

struct DxSkinPiece;
class DxSimMesh;
class SUBPATH;
struct SGENINFO;

class CItemNode
{
public:
	SITEM				m_sItem;
	
	boost::shared_ptr<SGENINFO>	m_spDecomposeGenInfo;		// 분해시 젠될 아이템;
	RandomOption::Entity*		m_rSetRandomOption;
	SGRINDING_OPT*				m_rGRINDING_SET;
	//DxSkinPiece*				m_pSkinPiece[GLCI_NUM_ETC];	//	Wear Piece Mesh.
	DxSimMeshResource			m_InvenMeshRes;	//	Inventory Mesh.
	DxSimMeshResource			m_FieldMeshRes;	//	Field Mesh.

public:
	CItemNode(void)
		: m_rSetRandomOption(NULL)
		, m_rGRINDING_SET(NULL)
	{
		//m_pSkinPiece[0] = NULL;
		//m_pSkinPiece[1] = NULL;
		//m_pSkinPiece[2] = NULL;
		//m_pSkinPiece[3] = NULL;
	}

	~CItemNode ()
	{
	}
};
typedef CItemNode* PITEMNODE;


// typedef std::list<SRANDOM_DATA> RandomDataList;
// typedef RandomDataList::const_iterator RandomDataListConstIterator;
// typedef RandomDataList::iterator RandomDataListIterator;
// 
// typedef std::map<unsigned int, RandomDataList> RandomDataListMap;
// typedef RandomDataListMap::const_iterator RandomDataListMapConstIterator;
// typedef RandomDataListMap::iterator RandomDataListMapIterator;
// typedef RandomDataListMap::value_type RandomDataListValue;	


class GLItemMan
{
public:
	enum
	{
		ENCODE_VER		= 0x0100,

		VERSION			= 0x0100,
		MAX_MID			= 4096,
		MAX_SID			= 2048,

		MONEYMESH_NUM	= 3,
		SPECID_NUM		= 4096,
		SHUFFLE_NUM		= 5,

        QITEM_NUM_NOT_SP_KOREA      = 5,
		MAX_NUM                     = 8, // ?아이템 해킹대처용 렌덤 배열 수
	};

public:
    GLItemMan();
    virtual ~GLItemMan();

    static GLItemMan& GetInstance();
	static void ReleaseInstance();

public:
	static const char* _FILEHEAD;
	static const char* _LOGFILE;
	static const char* _STRINGTABLE;
	static const char* _SEARCHLIST;

	typedef std::vector<SNATIVEID>		NID_VEC;
	typedef NID_VEC::iterator			NID_VEC_ITER;
	typedef std::vector<WORD>			WRD_VEC;
//	typedef std::vector<SRANDOM_DATA>	VEC_RANDOM;
	typedef std::vector<SGRINDING_DATA>	VEC_GRINDING;

	struct SGENITEMS
	{
		std::vector<SNATIVEID>	m_vecItems;
		WRD_VEC					m_sRendom;
		WORD					m_wNowCur;

		SGENITEMS () :
			m_wNowCur(0)
		{
		}
	};

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
    std::string			m_FileName;

	bool				m_bModify;
	PITEMNODE**			m_ppItem;
	SGENITEMS			m_sGenItems[SPECID_NUM];
//	VEC_RANDOM			m_sRandom;
	VEC_GRINDING		m_sGrinding;

	std::string			m_strMoneyMesh[MONEYMESH_NUM];
	DxSimMeshResource	m_MoneyMeshRes[MONEYMESH_NUM];

	std::vector<CString>	m_vecSrcString;

// 	// Random Option file map;
// 	RandomOption::RandomDataListMap mapRandomDataList;

	// 아이템 해킹 대처용
	SNATIVEID			m_sQITEMID[MAX_NUM];

public:
	CItemNode* GetItemNode(WORD wMID, WORD wSID);

	// GLItemMan::m_vecSrcString 에서 strItemName 과 동일한 이름이 있는 아이템 이름을 vecOut 에 넘겨준다.
	void GetList_SameWord( std::vector<CString>& vecOut, const CString& strItemName );

public:
	inline BOOL ValidTable() const { return m_ppItem!=NULL; }

	bool ValidData() const;
	inline bool IsModify() const { return m_bModify; }

	SITEM* GetItem(WORD wMID, WORD wSID);
	SITEM* GetItem(const SNATIVEID& sNativeID);
    SITEM* GetItem(DWORD ItemId);
    const std::string& GetItemName(const SNATIVEID& sNativeID);
	RandomOption::Entity* GetItemRandomOpt(SNATIVEID sNativeID);
	SGRINDING_OPT* GetItemGrindingOpt ( SNATIVEID sNativeID );

	SNATIVEID GetRandomQItemID ( SNATIVEID sNativeID, int nMaxNum ); // 랜덤 ?아이템 발생 (헤킹 대처!)

	EMITEM_QUESTION GetRandQItem();

	BOOL ItemGenerateSet ();
	SNATIVEID RendomGenSpecID ( DWORD dwSpecID );

	const SGENITEMS& GetGenSpecItems(DWORD dwSpecID); 

	//DxSkinPiece* LoadSkinPiece ( SNATIVEID sNativeID, EMCHARINDEX emIndex )		{ return LoadSkinPiece ( sNativeID.wMainID, sNativeID.wSubID, emIndex ); }
	//DxSkinPiece* LoadSkinPiece ( WORD wMID, WORD wSID, EMCHARINDEX emIndex );

	DxSimMesh* GetFieldMesh ( SNATIVEID sNativeID )			{ return GetFieldMesh ( sNativeID.wMainID, sNativeID.wSubID ); }
	DxSimMesh* GetFieldMesh ( WORD wMID, WORD wSID );

	//DxSimMesh* GetInvenMesh ( SNATIVEID sNativeID )			{ return GetInvenMesh ( sNativeID.wMainID, sNativeID.wSubID ); }
	//DxSimMesh* GetInvenMesh ( WORD wMID, WORD wSID );

	DxSimMesh* GetMoneyMesh ( DWORD i )						{ return m_MoneyMeshRes[i].get(); }

//	RandomOption::Entity*   LoadRandomGenData ( const char* szName );
	SGRINDING_OPT* LoadGrindingOptData ( const std::string& strName, const bool IsRegister=true );

	void DeleteGrindingOptData( const std::string& strName );

	WORD FindFreeMID ();
	WORD FindFreeSID ( WORD wMainID );
	
	BOOL InsertItem ( SNATIVEID sNativeID, PITEMNODE pItem, bool binner=false )		{ return InsertItem ( sNativeID.wMainID, sNativeID.wSubID, pItem, binner ); }
	BOOL InsertItem ( WORD wMID, WORD wSID, PITEMNODE pItem, bool binner=false );

	BOOL DeleteItem ( WORD wMID, WORD wSID );
	BOOL DeleteItem ( SNATIVEID sNativeID );

	HRESULT OneTimeSceneInit ();
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects ();
	HRESULT FinalCleanup ();

	HRESULT CleanUp ()
	{
		HRESULT hr;
		hr = DeleteDeviceObjects ();
		if ( FAILED(hr) )	return hr;
		
		hr = FinalCleanup ();
		if ( FAILED(hr) )	return hr;

		return S_OK;
	}

	HRESULT LoadFile(const std::string& FileName, boost::function<void (std::string)> PercentCallbackFunc=NULL);
    
    HRESULT SaveFile(const std::string& FileName);
    HRESULT SaveFileLua(const std::string& FileName);
	HRESULT SaveSearchList(const char* szFile); // by luxes.
    void    SaveItemDataSimple(sc::Excel& Xls, int Sheet, const std::string& SheetName);

	HRESULT	SyncStringTable();
	HRESULT SyncUpdateData(const SUBPATH* pPath);
	HRESULT SaveCsvFile(CWnd* pWnd);
	HRESULT LoadCsvFile(CWnd* pWnd);
	HRESULT LoadStringTableExcel(const char* szPath);
	HRESULT SaveStringTableExcel(std::string& strSavePath);
    std::string SaveSqlFile();
	std::string SaveSqlFileWithPileNum();

    size_t GetMaxMid() const { return MAX_MID; }
    size_t GetMaxSid() const { return MAX_SID; }

public:
	sc::BOOST_OBJECT_POOL_LOCK_CHECKER<SINVENITEM> m_poolSINVENITEM;
};

#endif // _GL_ITEM_MAN_H_
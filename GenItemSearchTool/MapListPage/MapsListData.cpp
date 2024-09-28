// MapsListData.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../GenItemTool.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "./MapsListData.h"

std::string strInstanceType[SMAPNODE::EMINSTANCE_NSIZE] =
{
	"Normal Map", "Old Instance", "New Instance"
};
// MapsListData 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMapsListData, CDialog)
CMapsListData::CMapsListData(CWnd* pParent /*=NULL*/)
	: CDialog(CMapsListData::IDD, pParent)
{
}

CMapsListData::~CMapsListData()
{
}

void CMapsListData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMapsListData, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

const int PROHIBIT_CONTROL[PROHIBIT_ITEM_SIZE] = 
{
	IDC_CHECK_PROHIBIT_TAXI_CARD,
	IDC_CHECK_PROHIBIT_TELEPORT_CARD,
	IDC_CHECK_PROHIBIT_RECALL,
	IDC_CHECK_PROHIBIT_CURE,
};

// MapsListData 메시지 처리기입니다.

BOOL CMapsListData::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	SetWin_Num_int( this, IDC_EDIT_MID, m_sMapNode.m_NativeID.wMainID );
	SetWin_Num_int( this, IDC_EDIT_SID, m_sMapNode.m_NativeID.wSubID );
	SetWin_Text( this, IDC_EDIT_MAPNAME, m_sMapNode.m_MapID.c_str() );
	SetWin_Check ( this, IDC_CHECK_MAP_USE, m_sMapNode.m_bUse );
	SetWin_Text( this, IDC_EDIT_FILENAME, m_sMapNode.m_LevelFile.c_str() );
	SetWin_Num_int( this, IDC_EDIT_FIELDSERVER, m_sMapNode.m_FieldServerID );
	SetWin_Text( this, IDC_EDIT_BGM, m_sMapNode.m_BgmName.c_str() );
	SetWin_Text( this, IDC_EDIT_LOADING_IMAGE, m_sMapNode.m_LoadingImageName.c_str() );

	SetWin_Check ( this, IDC_CHECK_PEACEZONE, m_sMapNode.m_bPeaceZone );
	SetWin_Check ( this, IDC_CHECK_COMMISSION, !m_sMapNode.m_bCommission );
	SetWin_Check ( this, IDC_CHECK_PKENABLE, m_sMapNode.m_bPKZone );
	SetWin_Check ( this, IDC_CHECK_FREEPK, m_sMapNode.m_bFreePK );
	SetWin_Check ( this, IDC_CHECK_ITEMDROP, m_sMapNode.m_bItemDrop );
	SetWin_Check ( this, IDC_CHECK_FRIENDCARD, m_sMapNode.m_bFriendMove );
	SetWin_Check ( this, IDC_CHECK_RESTART, m_sMapNode.m_bRestart );
	SetWin_Check ( this, IDC_CHECK_PETACTIVITY, m_sMapNode.m_bPetActivity );
	SetWin_Check ( this, IDC_CHECK_EXPDROP, m_sMapNode.m_bDecExp );
	SetWin_Check ( this, IDC_CHECK_VEHICLEACTIVITY, m_sMapNode.m_bVehicleActivity );
	SetWin_Check ( this, IDC_CHECK_CLUBBATTLEZONE, m_sMapNode.m_bClubBattleZone );
	SetWin_Check ( this, IDC_CHECK_QBOXDROP, m_sMapNode.m_bQBoxDrop );
	SetWin_Check ( this, IDC_CHECK_LUNCHBOXFORBID, m_sMapNode.m_bLunchBoxForbid );
	SetWin_Check ( this, IDC_CHECK_CPRESET, m_sMapNode.m_bCPReset );
	SetWin_Check ( this, IDC_CHECK_PKMAP, m_sMapNode.m_bPKMap );
	SetWin_Check ( this, IDC_CHECK_UI_MAP_SELECT, m_sMapNode.m_bUIMapSelect );
	SetWin_Check ( this, IDC_CHECK_UI_MAP_INFO, m_sMapNode.m_bUIMapInfo );
	SetWin_Check ( this, IDC_CHECK_PARTY_MAP, m_sMapNode.m_bPartyMap );
	SetWin_Check ( this, IDC_CHECK_EXPEDITION_MAP, m_sMapNode.m_bExpeditionMap );
    SetWin_Check ( this, IDC_CHECK_PM_OPENABLE, m_sMapNode.m_bPrivateMarketOpenable );
    SetWin_Check ( this, IDC_CHECK_SPARRING, m_sMapNode.m_bSparring );
	SetWin_Check ( this, IDC_CHECK_CULL_BY_OBJECT, m_sMapNode.m_bCullByObjectMap);
	SetWin_Check ( this, IDC_CHECK_PROHIBIT_DROP_ITEM, m_sMapNode.m_bDropBlockItem);
	SetWin_Combo_Init(this, IDC_COMBO_INSTANCE_TYPE, strInstanceType, SMAPNODE::EMINSTANCE_NSIZE);
	SetWin_Combo_Sel(this, IDC_COMBO_INSTANCE_TYPE, m_sMapNode.m_emInstanceType);	

	for (int i = 0; i < PROHIBIT_ITEM_SIZE; ++i)
	{
		SetWin_Check ( this, PROHIBIT_CONTROL[i], m_sMapNode.IsBlockDrugItem((SMAPNODE::ItemTypeValue)PROHIBIT_ITEM_LIST[i]) );
	}

	SetWin_Check ( this, IDC_CHECK_REBIRTHBLOCK, m_sMapNode.m_bRebirthBlock );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMapsListData::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}

void CMapsListData::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	m_sMapNode.m_NativeID.wMainID = GetWin_Num_int( this, IDC_EDIT_MID );
	m_sMapNode.m_NativeID.wSubID = GetWin_Num_int( this, IDC_EDIT_SID );
	m_sMapNode.m_MapID = GetWin_Text ( this, IDC_EDIT_MAPNAME );
	m_sMapNode.m_bUse = GetWin_Check ( this, IDC_CHECK_MAP_USE );
	m_sMapNode.m_LevelFile = GetWin_Text ( this, IDC_EDIT_FILENAME );
	m_sMapNode.m_FieldServerID = GetWin_Num_int( this, IDC_EDIT_FIELDSERVER );
	m_sMapNode.m_BgmName = GetWin_Text ( this, IDC_EDIT_BGM );
	m_sMapNode.m_LoadingImageName = GetWin_Text ( this, IDC_EDIT_LOADING_IMAGE );

	m_sMapNode.m_bPeaceZone = GetWin_Check ( this, IDC_CHECK_PEACEZONE );
	m_sMapNode.m_bCommission = !GetWin_Check ( this, IDC_CHECK_COMMISSION );
	m_sMapNode.m_bPKZone = GetWin_Check ( this, IDC_CHECK_PKENABLE );
	m_sMapNode.m_bFreePK = GetWin_Check ( this, IDC_CHECK_FREEPK );
	m_sMapNode.m_bItemDrop = GetWin_Check ( this, IDC_CHECK_ITEMDROP );
	m_sMapNode.m_bFriendMove = GetWin_Check ( this, IDC_CHECK_FRIENDCARD );
	m_sMapNode.m_bRestart = GetWin_Check ( this, IDC_CHECK_RESTART );
	m_sMapNode.m_bPetActivity = GetWin_Check ( this, IDC_CHECK_PETACTIVITY );
	m_sMapNode.m_bDecExp = GetWin_Check ( this, IDC_CHECK_EXPDROP );
	m_sMapNode.m_bVehicleActivity = GetWin_Check ( this, IDC_CHECK_VEHICLEACTIVITY );
	m_sMapNode.m_bClubBattleZone = GetWin_Check ( this, IDC_CHECK_CLUBBATTLEZONE );
	m_sMapNode.m_emInstanceType = SMAPNODE::EMINSTANCE_TYPE(GetWin_Combo_Sel( this, IDC_COMBO_INSTANCE_TYPE ));
	m_sMapNode.m_bQBoxDrop = GetWin_Check ( this, IDC_CHECK_QBOXDROP );
	m_sMapNode.m_bLunchBoxForbid = GetWin_Check( this, IDC_CHECK_LUNCHBOXFORBID );
	m_sMapNode.m_bCPReset = GetWin_Check ( this, IDC_CHECK_CPRESET );
	m_sMapNode.m_bPKMap = GetWin_Check ( this, IDC_CHECK_PKMAP );
	m_sMapNode.m_bUIMapSelect = GetWin_Check( this, IDC_CHECK_UI_MAP_SELECT );
	m_sMapNode.m_bUIMapInfo = GetWin_Check( this, IDC_CHECK_UI_MAP_INFO );
	m_sMapNode.m_bPartyMap = GetWin_Check( this, IDC_CHECK_PARTY_MAP );
	m_sMapNode.m_bExpeditionMap = GetWin_Check( this, IDC_CHECK_EXPEDITION_MAP );
    m_sMapNode.m_bPrivateMarketOpenable = GetWin_Check( this, IDC_CHECK_PM_OPENABLE );
    m_sMapNode.m_bSparring = GetWin_Check( this, IDC_CHECK_SPARRING );
	m_sMapNode.m_bCullByObjectMap = GetWin_Check(this, IDC_CHECK_CULL_BY_OBJECT);
	m_sMapNode.m_bDropBlockItem = GetWin_Check(this, IDC_CHECK_PROHIBIT_DROP_ITEM);

	m_sMapNode.m_prohibiteditems.clear();

	for (int i = 0; i < PROHIBIT_ITEM_SIZE; ++i)
	{
		BOOL bBlock = GetWin_Check ( this, PROHIBIT_CONTROL[i] );

		if (bBlock)
		{
			m_sMapNode.m_prohibiteditems.push_back((SMAPNODE::ItemTypeValue)PROHIBIT_ITEM_LIST[i]);//SMAPNODE::ItemTypeValue(i));
		}
	}

	m_sMapNode.m_bRebirthBlock = GetWin_Check( this, IDC_CHECK_REBIRTHBLOCK );

	// 데이터 체크

	if( m_sMapNode.m_NativeID == NATIVEID_NULL() )
	{
		MessageBox( "Mid,Sid Error" );
		return;
	}
	if( m_sMapNode.m_MapID.empty() || m_sMapNode.m_MapID == "null" )
	{
		MessageBox( "MapName Error" );
		return;
	}

	if( m_sMapNode.m_LevelFile.empty() || m_sMapNode.m_LevelFile == "null" )
	{
		MessageBox( "FileName Error" );
		return;
	}

	CDialog::OnOK();
}






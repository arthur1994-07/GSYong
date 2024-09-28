#pragma once

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Skill/GLSkill.h"
#include "../../RanLogic/GLColorTable.h"

namespace NSWIDGET_SCRIPT
{
	// 디버깅 중?
	extern bool g_bWidgetDebug;

	//--------------------------------------------------------------------------------
	// 단축키

	// 채팅 매크로 단축키 입력
	// params : char* strText
	static const char* g_strFunc_HotKey_AddChatMacro = "CF_HotKey_AddChatMacro";
	// 인벤토리 창 열기
	// params : void
	static const char* g_strFunc_HotKey_Inventory = "CF_HotKey_Inventory";
	// 캐릭터 창 열기
	// params : void
	static const char* g_strFunc_HotKey_Character = "CF_HotKey_Character";
	// 탈것 창 열기
	// params : void
	static const char* g_strFunc_HotKey_Vehicle = "CF_HotKey_Vehicle";
	// 스킬 창 열기
	// params : void
	static const char* g_strFunc_HotKey_Skill = "CF_HotKey_Skill";
	// 채팅 매크로 창 열기
	// params : void
	static const char* g_strFunc_HotKey_ChatMacro = "CF_HotKey_ChatMacro";
	// 스킬 퀵슬롯 그룹
	// params : int nIndex
	static const char* g_strFunc_HotKey_SQSGroup = "CF_HotKey_SQSGroup";
	// 스킬 퀵슬롯 트레이
	// params : void
	static const char* g_strFunc_HotKey_SQSTray = "CF_HotKey_SQSTray";
	// 파티 창 열기
	// params : void
	static const char* g_strFunc_HotKey_Party = "CF_HotKey_Party";
	// 펫 창 열기
	// params : void
	static const char* g_strFunc_HotKey_Pet = "CF_HotKey_Pet";
	// 제작 창 열기
	// params : void
	static const char* g_strFunc_HotKey_Product = "CF_HotKey_Product";
	// 포인트샵 창 열기
	// params : void
	static const char* g_strFunc_HotKey_PointShop = "CF_HotKey_PointShop";
	// 시스템 메뉴창
	// params : void
	static const char* g_strFunc_HotKey_System = "CF_HotKey_System";
	// 메시지 박스 끄기
	// params : void
	static const char* g_strFunc_HotKey_CancelMsgBox = "CF_HotKey_CancelMsgBox";
	// 엔터키 확인 (메시지박스 확인, 채팅창 포커스 등)
	// params : void
	static const char* g_strFunc_HotKey_Confirm = "CF_HotKey_Confirm";
	// 전체맵
	// params : void
	static const char* g_strFunc_HotKey_LargeMap = "CF_HotKey_LargeMap";
	// 전장UI
	// params : void 
	static const char* g_strFunc_HotKey_Competition = "CF_HotKey_Competition";
	// 은행
	// params : void
	static const char* g_strFunc_HotKey_Bank = "CF_HotKey_Bank";
	// 클럽창 호출
	// params : void
	static const char* g_strFunc_HotKey_Club = "CF_HotKey_Club";
	// 커뮤니티 창 열기
	// params : void
	static const char* g_strFunc_HotKey_Community = "CF_HotKey_Community";
	// DPS
	// params : void
	static const char* g_strFunc_HotKey_DPS = "CF_HotKey_DPS";
	// 플라잉 카메라
	// params : void
	static const char* g_strFunc_HotKey_FlyCamCtrl = "CF_HotKey_FlyCamCtrl";
	// 생활기록부
	// params : void
	static const char* g_strFunc_HotKey_StudentRecord = "CF_HotKey_StudentRecord";
	// 도움말 
	// params : void
	static const char* g_strFunc_HotKey_Help = "CF_HotKey_Help";
	// 퀘스트
	// params : void
	static const char* g_strFunc_HotKey_Quest = "CF_HotKey_Quest";
	// 큐박스
	// params : void
	static const char* g_strFunc_HotKey_QBox = "CF_HotKey_QBox";

	// 단축키 관련 함수 끝
	//--------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------
	// 루아 호출 함수 이름

	// 스케일폼 코어 초기화 완료
	// params : void
	static const char* g_strFunc_CreateCoreComplete = "CF_CreateCoreComplete";
	// 리셋 디바이스 (코어 초기화가 완료된 뒤부터만 호출)
	// params : int nScreenWidth, int nScreenHeight bool bResetUI
	static const char* g_strFunc_ResetDevice = "CF_ResetDevice";
	// 업데이트
	// params : float fElapsedTime
	static const char* g_strFunc_FrameMove = "CF_FrameMove";

	// 아이템 슬롯 헤더
	// params : table tbItemHeader
	static const char* g_strFunc_GetItemTooltipHeader = "CF_GetItemTooltipHeader";
	// 아이템 슬롯 툴팁
	// params : table tbItemData
	static const char* g_strFunc_GetItemTooltip = "CF_GetItemTooltip";
	// 아이템 슬롯 교환 상태 툴팁
	// params : void
	static const char* g_strFunc_GetItemTooltipTrade = "CF_GetItemTooltipTrade";
	// 스킬 슬롯 헤더
	// params : table tbSkillHeader
	static const char* g_strFunc_GetSkillTooltipHeader = "CF_GetSkillTooltipHeader";
	// 스킬 슬롯 툴팁
	// params : table tbSkillData
	static const char* g_strFunc_GetSkillTooltip = "CF_GetSkillTooltip";
	// 툴팁 추가 슬롯 개수
	// params : void
	static const char* g_strFunc_GetTootipAddSlotCount = "CF_GetTooltipAddSlotCount";
	// 툴팁 추가 슬롯 바인딩
	// params : void
	static const char* g_strFunc_GetTooltipAddSlotBinding = "CF_GetTooltipAddSlotBinding";
	// 툴팁 교환 슬롯 개수
	// params : void
	static const char* g_strFunc_GetTooltipTradeSlotCount = "CF_GetTooltipTradeSlotCount";
	// 툴팁 교환 슬롯 바인딩
	// params : void
	static const char* g_strFunc_GetTooltipTradeSlotBinding = "CF_GetTooltipTradeSlotBinding";
	// 툴팁 하이드
	// params : void
	static const char* g_strFunc_HideTooltipSlot = "CF_HideTooltipSlot";

	// 착용중인 아이템 비교 툴팁
	// params : WORD wMID, WORD wSID, string strHeader, string strTootlip
	static const char* g_strFunc_ComparePutOnItemTooltip = "CF_ComparePutOnItemTooltip";

	// 로그인 화면 진입
	// params : void
	static const char* g_strFunc_EnterLoginStage = "CF_EnterLoginStage";
	// 캐릭터 선택 화면 진입
	// params : void
	static const char* g_strFunc_EnterCharSelectStage = "CF_EnterCharSelectStage";
	// In 게임 진입
	// params : void
	static const char* g_strFunc_EnterGameStage = "CF_EnterGameStage";
	// 모든 NPC 관련 UI 닫기
	// params : void
	static const char* g_strFunc_CloseAllNpc = "CF_CloseAllNpc";

	// 홀드 아이템 버리기
	// params : int nHoldItemPlace, WORD wSlotPosX, WORD wSlotPosY, float fPosX, float fPosY, float fPosZ
	static const char* g_strFunc_HoldItemToField = "CF_HoldItemToField";
	// 겹침 아이템 분리
	// params : int nPosX, int nPosY, WORD wItemCount
	static const char* g_strFunc_SeparateOverlapItem = "CF_SeparateOverlapItem";


	static const char* g_strFunc_BasicGetCurrentLogID = "CF_BasicChat_GetCurrentLogID";
	static const char* g_strFunc_BasicGetCurrentChatType = "CF_BasicChat_GetCurrentChatType";
	// 채팅 링크 추가
	// params : table tbLinkData, char* strLinkName
	static const char* g_strFunc_AddChatLink = "CF_AddChatLink";

	// 중복 계정 로그인
	// params : void
	static const char* g_strFunc_DuplicateLogin = "CF_DuplicateLogin";
	static const char* g_strFunc_CloseConnection = "CF_CloseConnection";
	// 오프라인 누적시간 부족
	// params : void
	static const char* g_strFunc_LoginGainTime = "CF_LoginGainTime";

	// 클라이언트 손상.
	// params : void
	static const char* g_strFunc_Login_FailLoadClientVersion = "CF_Login_FailLoadClientVersion";
	// 버전이 다름.
	// params : void
	static const char* g_strFunc_Login_DifferentVersion = "CF_Login_DefferentVersion";
	// 로그인 실패 메시지
	// params : int nError
	static const char* g_strFunc_Login_FailLogin = "CF_Login_FailLogin";

	// 캐릭터 정보 리스트 초기화
	// params : void
	static const char* g_strFunc_CharSelect_InitList = "CF_CharSelect_InitSelectCharList";
	// 캐릭터 접속 실패 메시지	
	// params : int nError
	static const char* g_strFunc_CharSelect_JoinFail = "CF_CharSelect_JoinFail";
	// 캐릭터 삭제 성공
	// params : void
	static const char* g_strFunc_CharSelect_SucceedDelChar = "CF_CharSelect_SucceedDelChar";
	// 캐릭터 삭제 실패
	// params : bool bClub
	static const char* g_strFunc_CharSelect_FailDelChar = "CF_CharSelect_FailDelChar";
	// 캐릭터 선택
	// params : int nCharSelect, int nCharStart
	static const char* g_strFunc_CharSelect_SetSelecrCharIndex = "CF_CharSelect_SetSelectCharIndex";
	// 캐릭터 더블클릭
	// params : void
	static const char* g_strFunc_CharSelect_DClickJoin = "CF_CharSelect_DClickJoin";

	// 캐릭터 생성 결과
	// params : int nResult
	static const char* g_strFunc_CharCreate_NewCharResult = "CF_CharCreate_NewCharResult";
	
	// 캐릭터 기본정보 업데이트
	// params : void
	static const char* g_strFunc_BasicInfo_Update = "CF_BasicInfo_Update";
	// 스탯 포인트 적용 결과
	// params : void
	static const char* g_strFunc_Character_ApplyStatPoint = "CF_CharInfo_ApplyStatPoint";
	// 장비 슬롯 업데이트
	// params : void
	static const char* g_strFunc_Character_UpdateSlot = "CF_CharInfo_UpdateSlot";
	// 장비 스왑
	// params : bool bUseArmSub
	static const char* g_strFunc_Character_ArmSwap = "CF_CharInfo_ArmSwap";
	// 소모품 아이템 개수 업데이트
	// params : int nSlot, WORD wTurnNum, int nType
	static const char* g_strFunc_Character_UpdatePutOnCount = "CF_CharInfo_UpdatePutOnCount";
	// 소모품 모두 소진
	// params : int nSlot
	static const char* g_strFunc_Character_RemovePutOnCount = "CF_CharInfo_RemovePutOnCount";

	// 소지금 업데이트
	// params : LONGLONG llMoney
	static const char* g_strFunc_Inventory_UpdateMoney = "CF_Inventory_UpdateMoney";
	// 포인트샵 머니 업데이트
	// params : LONGLONG llMoney
	static const char* g_strFunc_Inventory_UpdatePoint = "CF_Inventory_UpdatePoint";
	// 인벤토리 슬롯 전체 업데이트
	// params : void
	static const char* g_strFunc_Inventory_InvenAllSlotUpdate = "CF_Inventory_InvenAllSlotUpdate";
	// 인벤토리 슬롯 업데이트
	// params : int nPosX, int nPosY
	static const char* g_strFunc_Inventory_InvenSlotUpdate = "CF_Inventory_InvenSlotUpdate";
	// 인벤토리 은행 슬롯 클릭
	// params : string strName, int nPosX, int nPosY
	static const char* g_strFunc_Inventory_BankItemClick = "CF_Inventory_BankItemClick";
	// 인벤토리 은행 전체 슬롯 업데이트
	// parmas : void
	static const char* g_strFunc_Inventory_BankAllSlotUpdate = "CF_Inventory_BankAllSlotUpdate";
	// 인벤토리 은행 슬롯 업데이트
	// parmas : int nPosX, int nPosY
	static const char* g_strFunc_Inventory_BankSlotUpdate = "CF_Inventory_BankSlotUpdate";
	// 인벤토리 위시리스트 아이템 멀티 툴팁 보이기
	// params : WORD wMID, WORD wSID, string strHeader, string strInfo
	static const char* g_strFunc_Inventory_WishItemShowTooltip = "CF_Inventory_WishItemShowTooltip";
	// 인벤토리 위시리스트 슬롯 업데이트
	// params : int nPosX, int nPosY
	static const char* g_strFunc_Inventory_WishSlotUpdate = "CF_Inventory_WishSlotUpdate";
	// 인벤토리 슬롯 확장
	// params : void
	static const char* g_strFunc_Inventory_AddInvenLine = "CF_Inventory_AddInvenLine";
	// 인벤토리 휴지통 결과
	// params : int nError
	static const char* g_strFunc_InvenTrash_TrashResult = "CF_IvenTrash_TrashResult";
	// 인벤토리 휴지통 열기
	// params : void
	static const char* g_strFunc_InvenTrash_Open = "CF_InvenTrash_Open";
	// 휴지통 열렸나?
	static const char* g_strFunc_InvenTrash_IsOpen = "CF_InvenTrash_IsOpen";
	// 인벤토리 정렬 가능한가?
	// params : void
	static const char* g_strFunc_IsInvenItemSort = "CF_IsInvenItemSort";
	// 아이템 분해중인가?
	// params : void
	static const char* g_strFunc_Inventory_IsItemDecompose = "CF_Inventory_IsItemDecompose";
	// 아이템 분해 확인
	// params : int nPosX, int nPosY
	static const char* g_strFunc_Inventory_ConfirmItemDecompose = "CF_Inventory_ConfirmItemDecompose";

	// 스킬 퀵슬롯 최근 사용 스킬 ID
	// params : DWORD dwID
	static const char* g_strFunc_SQS_LastSkillID = "CF_SQS_LastSkillID";
	// 스킬 퀵슬롯 업데이트
	// params : void
	static const char* g_strFunc_SQS_UpdateSlot = "CF_SQS_UpdateSlot";
	// 스킬 퀵슬롯 업데이트
	// params : WORD wIndex
	static const char* g_strFunc_SQS_UpdateSlotIndex = "CF_SQS_UpdateSlotIndex";
	// 현재 스킬 탭 인덱스
	// params : void
	static const char* g_strFunc_SQS_CurrentTab = "CF_SQS_CurrentTab";
	// 스킬 퀵슬롯 탭당 개수
	// params : void
	static const char* g_strFunc_SQS_TabSlotCount = "CF_SQS_TabSlotCount";
	// 스킬 퀵슬롯 무기 슬롯 스왑
	// parmas : bool bMain
	static const char* g_strFunc_SQS_ArmSlotSwap = "CF_SQS_ArmSlotSwap";

	// 아이템 퀵슬롯 업데이트
	// params : void
	static const char* g_strFunc_IQS_UpdateSlot = "CF_IQS_UpdateSlot";
	// 아이템 퀵슬롯 트레이 업데이트
	// params : int nIndex
	static const char* g_strFunc_IQS_UpdateTraySlot = "CF_IQS_UpdateTraySlot";

	// 채팅 메시지 
	// params : table tbData
	static const char* g_strFunc_BasicChat_ReceiveMsg = "CF_BasicChat_ReceiveMsg";
	// 채팅 새로고침
	static const char* g_strFuc_BasicChat_Refresh = "CF_BasicChat_Refresh";
	// 귓속말 실패
	// params : int nError, string strName
	static const char* g_strFunc_BasicChat_PrivateFail = "CF_BasicChat_PrivateFail";
	// 채팅 블락
	// params : LONGLONG llLeftMinute
	static const char* g_strFunc_BasicChat_Block = "CF_BasicChat_Block";
	// 채팅 로그창을 통한 캐릭터 이동 여부 설정
	static const char* g_strFunc_BasicChat_MovaleOnChatLog = "CF_BasicChat_MovalbeOnChatBG";
	// 채팅 귓속말 전환
	// params : string strName
	static const char* g_strFunc_BasicChat_SetWhisper = "CF_BasicChat_SetWhisper";
	// 아이템 링크 위치 변경
	// params : DWORD dwPos1, DWORD dwPos2
	static const char* g_strFunc_BasicChat_SwapItemLinkPos = "CF_BasicChat_SwapItemLinkPos";
	
	
	// params : table tbData
	static const char* g_strFunc_PrintLinkItemSystemMsg = "CF_PrintMsg_LinkItemSystemMsg";

    //! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
	static const char* g_strFunc_PrintItemLinkAndReferLinkSystemMsg = "CF_PrintMsg_ItemLinkAndReferLinkSystemMsg";
	
	//baischat is focus in?
	static const char* g_strFunc_BasicChat_IsFocusIn = "CF_BasicChat_IsFocusIn";

	// 팁 텍스트 설정
	static const char* g_strFunc_ShowTip = "CF_TipMsgShow";
	// 아이템 링크 클릭
	// params : int nMID, int nSID, char* strHeader, char* strInfo
	static const char* g_strFunc_ItemLinkInfo_Open = "CF_ItemLinkInfo_Open";
	// 파티 링크 클릭
	// params : table tbMemberList, bool bOnce, int nLinkIndex
	static const char* g_strFunc_PartyRecruit_Open = "CF_PartyRecruit_Open";
	// 파티 링크 지원 요청
	// params : DWORD dwGaeaID, int nClass, string strName, DWORD dwHashKey, DWORD dwTimeKey, DWORD dwMasterID
	static const char* g_strFunc_PartyRecruit_RequestRecv = "CF_PartyRecruit_RequestRecv";

	// 캐릭터 스킬 리스트 업데이트
	// params : void
	static const char* g_strFunc_Skill_UpdateList = "CF_Skill_UpdateList";
	// 캐릭터 스킬 포인트 업데이트
	// params : void
	static const char* g_strFunc_Skill_UpdatePoint = "CF_Skill_UpdatePoint";

	// 파티 생성
	// params : void
	static const char* g_strFunc_Party_New = "CF_Party_New";
	// 파티 슬롯 업데이트 (파티 인덱스 추가/제거/순서 변경 등)
	// params : void
	static const char* g_strFunc_Party_UpdateSlot = "CF_Party_UpdateSlot";
	// 파티 해산
	// params : void
	static const char* g_strFunc_Party_Dissolve = "CF_Party_Dissolve";
	// 파티원 맵이동
	// params : DWORD dwIndex, DWORD dwMapID, DOWR dwGaeaMapID
	static const char* g_strFunc_Party_MembeMoveMap = "CF_Party_MemberMoveMap";
	// 파티원 정보 업데이트
	// params : DWORD dwIndex, int nPosX, int nPosY, DWORD dwHPMax, DWORD dwHPNow, WORD wLevel
	static const char* g_strFunc_Party_UpdateMember = "CF_Party_UpdateMember";
	// 파티원 이름 변경
	// params : DWORD dwIndex, char* strName
	static const char* g_strFunc_Party_MemberRename = "CF_Party_MemberRename";
	// 파티 옵션 변경
	// params : int nItemOp, int nMoneyOp, int nItemFilterOp
	static const char* g_strFunc_Party_ChangeOption = "CF_PartyChangeOption";
	// 파티 초대 수신
	static const char* g_strFunc_Party_MsgInvite = "CF_PartyMsgInvite";
	// 파티원 버프 추가
	// params : DWORD dwGaeaID, DWORD dwSkillID, DWORD dwSlot
	static const char* g_strFunc_Party_AddBuff = "CF_Party_AddBuff";
	// 파티원 버프 삭제
	// params : DWORD dwGaeaID, DWORD dwSkillID, DWORD dwSlot
	static const char* g_strFunc_Party_RemoveBuff = "CF_Party_RemoveBuff";
	// 파티원 모든 버프 삭제
	// params : DWORD dwGaeaID
	static const char* g_strFunc_Party_RemoveAllBuff = "CF_Party_RemoveAllBuff";


	// NPC 대화
	// param : void
	static const char* g_strFunc_NpcTalk_Open = "CF_NpcTalk_Open";
	// NPC 대화창 마켓탭 열려있는지 확인
	// param : void
	static const char* g_strFunc_NpcTalk_IsMarketTabOpen = "CF_NpcTalk_IsMarketTabOpen";
	// NPC 대회창 현재 활성된 마켓탭 인덱스
	// param : void
	static const char* g_strFunc_NpcTalk_GetActiveMarket = "CF_NpcTalk_GetActiveMarket";
	// Npc에게 아이템 판매
	// params : bool bSale, bool bConfirm, string strItemName, int nPosX, int nPosY
	static const char* g_strFunc_NpcTalk_SellItem = "CF_NpcTalk_SellItem";
	// Npc 마켓 아이템 구매
	// params : string strName, int nPosX, int nPosY, bool bPile
	static const char* g_strFunc_NpcTalk_BuyItem = "CF_NpcTalk_BuyItem";
	// Npc 마켓 카트에 아이템 추가
	// params : string strName, WORD wMID, WORD wSID, int nPosX, int nPosY, 
	//			bool bPile, WORD wApplyNum, WORD wPileNum
	static const char* g_strFunc_NpcTalk_AddCart = "CF_NpcTalk_AddCart";
	// Npc 마켓 카트 아이템 제거
	// params : WORD wIndex
	static const char* g_strFunc_NpcTalk_RemoveCart = "CF_NpcTalk_RemoveCart";
	// Npc 재매입 탭 업데이트
	// params : void
	static const char* g_strFunc_NpcTalk_UpdateRebuy = "CF_NpcTalk_UpdateRebuy";
	// NPC 마켓 탭으로 열기
	// params : void
	static const char* g_strFunc_NpcTalk_OpenMarket = "CF_NpcTalk_OpenMarket";
	// Npc 클럽 창설
	// params : void
	static const char* g_strFunc_NpcTalk_CreateClub = "CF_NpcTalk_CreateClub";

	// 시스템 메시지 출력
	// params : char* strMsg, DWORD dwColor
	static const char* g_strFunc_PrintSystemMsg = "CF_PrintMsg_SystemMsg";
	// 시스템 메시지 박스 출력
	// params : char* strMsg, DWORD dwColor
	static const char* g_strFunc_PrintSystemBox = "CF_PrintMsg_SystemBox";
	// 시스템 메시지 & 시스템 메시지 박스 출력
	// params : char* strMsg, DWORD dwColor
	static const char* g_strFunc_PrintSystemMsgBox = "CF_PrintMsg_SystemMsgBox";
	// 채팅창 일반 메시지 추가
	// params : char* strMsg, DWORD dwColor
	static const char* g_strFunc_AddString = "CF_PrintMsg_AddString";

	// 상호 작용 메뉴 오픈
	static const char* g_strFunc_OpenInteractMenu = "CF_OpenInteractMenu";
	// 대련 신청 UI호출
	static const char* g_strFunc_Conft = "CF_Open_ConftUI";
	// 거래창 UI 호출
	static const char* g_strFunc_Trade = "CF_Open_TradeUI";
	// 거래창 닫기
	static const char* g_strFunc_TradeClose = "CF_Close_TradeUI";
	// 상대방 잠금버튼 잠금
	static const char* g_strFunc_TradeTarButtonLock = "CF_Trade_SetTarButtonLock";
	// 상대방 돈 텍스트 변경
	static const char* g_strFunc_TradeTarTextMoney = "CF_Trade_SetTarTextMoney";
	// 상대방 승인 버튼 상태변경
	static const char* g_strFunc_TradeTarButtonAgree = "CF_Trade_SetTarButtonAgree";
	// 슬롯 업데이트
	// params : WORD wPosX, WORD wPosY, bool bMySlot
	static const char* g_strFunc_Trade_UpdateSlot = "CF_Trade_UpdateSlot";
	// 거래 등록 내 인벤 아이템 업데이트
	// params : WORD wPosX, WORD wPosY, bool bEnable
	static const char* g_strFunc_Trade_UpdateInvenSlot = "CF_Trade_UpdateInvenSlot";
	// 내 잠금버튼 상태변경
	static const char* g_strFunc_TradeMineButtonLock = "CF_Trade_SetMineButtonLock";
	// 내 승인버튼 상태변경
	static const char* g_strFunc_TradeMineButtonAgree = "CF_Trade_SetMineButtonAgree";
	// 거래창 열기 전에 메시지 박스 호출
	static const char* g_strFunc_TradeTarMessageBox = "CF_Open_Trade_ReqMessageBox";
	// 거래창 열려있나?
	static const char* g_strFunc_Trade_IsOpen = "CF_Trade_IsOpen";

	// 그룹 채팅 빈방인지 확인
	// params : int nIndex
	static const char* g_strFunc_GroupChat_IsRoomFree = "CF_GroupChat_IsRoomFree";
	// 그룹 채팅 방 개설
	// params : int nIndex, DWORD dwKey, string strOwnerName
	static const char* g_strFunc_GroupChat_CreateRoom = "CF_GroupChat_CreateRoom";
	// 그룹 채팅 방 삭제
	// params : DWORD dwKey
	static const char* g_strFunc_GroupChat_DeleteRoom = "CF_GroupChat_DeleteRoom";
	// 그룹 채팅 키 가져오기
	// params : int nIndex
	static const char* g_strFunc_GroupChat_GetKey = "CF_GroupChat_GetKey";
	// 그룹 채팅 초대 수신
	// params : string strName
	static const char* g_strFunc_GroupChat_InviteRecv = "CF_GroupChat_InviteRecv";
	// 그룹 채팅 멤버 업데이트
	// params : DWORD dwKey, string strName, bool bJoin, bool bPrintMsg
	static const char* g_strFunc_GroupChat_UpdateMember = "CF_GroupChat_UpdateMember";
	// 그룹 채팅 메시지 수신
	// params : DWORD dwKey, string strName, string strMsg, table tbLinkItem
	static const char* g_strFunc_GroupChat_RecvMsg = "CF_GroupChat_RecvMsg";
	// 그룹 채팅 활성된 채팅방 키 가져오기
	// params : void
	static const char* g_strFunc_GroupChat_GetEnableRoomKey = "CF_GroupChat_GetEnableRoomKey";
	// 그룹 채팅 링크 아이템 업데이트
	// params : DWORD dwKey
	static const char* g_strFunc_GroupChat_UpdateLinkItem = "CF_GroupChat_UpdateLinkItem";
	
	// 정보보기창 열기
	// params : void
	static const char* g_strFunc_ReferChar_Open = "CF_ReferChar_Open";
	// 정보보기창 캐릭터 이름 설정
	// params : string strName
	static const char* g_strFunc_ReferChar_SetName = "CF_ReferChar_SetName";
	// 정보보기창 캐릭터 이름 얻기
	// params : void
	static const char* g_strFunc_ReferChar_GetName = "CF_ReferChar_GetName";

	// 제작초기화 열기
	// param : void
	static const char* g_strFunc_ProductReset_Open = "CF_ProductReset_Open";
	// 제작 프로그래스바 열기
	// param : void
	static const char* g_strFunc_ProductStart = "CF_ProductStart";
	// 제작 계속 시작;
	static const char* g_strFunc_ProductOngoingStart = "CF_ProductOngoingStart";
	// 제작 프로그래스바 닫기
	// param : void
	static const char* g_strFunc_ProductEnd = "CF_ProductEnd";
	// 제작 새로고침
	// params : void
	static const char* g_strFunc_ProductRefresh = "CF_Product_Refresh";
	// 제작 타입	 업데이트
	// params : void
	static const char* g_strFunc_ProductUpdateType = "CF_Product_UpdateType";
	// 제작 완료
	static const char* g_strFunc_ProductSuccess = "CF_ProductSuccess";
	
	// 펫창 닫기
	// param : void
	static const char* g_strFunc_Pet_CloseWindow = "CF_Pet_CloseWindow";
	// 펫창 열기
	// param : void
	static const char* g_strFunc_Pet_OpenWindow = "CF_Pet_OpenWindow";
	// 펫창 업데이트
	// param : void
	static const char* g_strFunc_Pet_UpdateUI = "CF_Pet_UpdateUI";
	// 펫 이름 변경
	// params : void
	static const char* g_strFunc_Pet_Rename = "CF_Pet_Rename";

	// 머리 스타일 변경 카드 사용
	// param : void
	static const char* g_strFunc_StyleHairUse = "CF_StyleHairUse";
	// 머리 스타일 변경 에러
	// param : int nError, WORD wHair
	static const char* g_strFunc_StyleHairError = "CF_StyleHairError";

	// 얼굴 스타일 변경 카드 사용
	// param : void
	static const char* g_strFunc_StyleFaceUse = "CF_StyleFaceUse";
	// 얼굴 스타일 변경 에러
	// param : int nError, WORD wFace
	static const char* g_strFunc_StyleFaceError = "CF_StyleFaceError";

	// 성별 변경 카드 사용
	// param : void
	static const char* g_strFunc_StyleGenderUse = "CF_StyleGenderUse";
	// 성별 변경 에러
	// param : int nError
	static const char* g_strFunc_StyleGenderError = "CF_StyleGenderError";

	// 펫 스타일 변경 카드 사용
	// param : void
	static const char* g_strFunc_StylePetUse = "CF_StylePetUse";
	// 펫 스타일 변경 에러
	// param : int nError, WORD wStyle, WORD wColor
	static const char* g_strFunc_StylePetError = "CF_StylePetError";
	
	// 포인트샵 카트 아이템갱신
	// params : void
	static const char* g_strFunc_PointShop_UpdateCart = "CF_PointShop_UpdateCart";
	// 포인트샵 카트 아이템 총 가격
	// params : void
	static const char* g_strFunc_PointShop_GetCartItemPrice = "CF_PointShop_GetCartItemPrice";
	// 포인트샵 UI 열기
	// params : void
	static const char* g_strFunc_PointShop_Open = "CF_PointShop_Open";
	// 포인트샵 UI 닫기
	// params : void
	static const char* g_strFunc_PointShop_Close = "CF_PointShop_Close";
	
	// 개인상점 검색 열기
	// params : void 
	static const char* g_strFunc_PrivateMarketSearch_Open = "CF_PrivateMarketSearch_Open";
	// 개인상점 검색 포인트 거래 아이템 판매 등록
	// params : int nResult, WORD wMID, WORD wSID, WORD wPosX, WORD wPosY
	static const char* g_strFunc_PrivateMarketSearch_RegisterItem = "CF_PrivateMarketSearch_RegisterItem";
	// 개인상점 검색 
	// params : void
	static const char* g_strFunc_PrivateMarketSearch_IsOpen = "CF_PrivateMarketSearch_IsOpen";
	// 개인상점 검색 페이지 업데이트
	// params : void
	static const char* g_strFunc_PrivateMarketSearch_UpdateSearchPage = "CF_PrivateMarketSearch_UpdateSearchPage";
	// 검색 에러 코드 반환
	// params : int nError, bool bUpdate
	static const char* g_strFunc_PrivateMarketSearch_Error = "CF_PrivateMarketSearch_Error";
	// 개인상점 검색 거래내역 업데이트
	// params : void
	static const char* g_strFunc_PrivateMarketSearch_UpdateLog = "CF_PrivateMarketSearch_UpdateLog";
	// 개인 상점 검색 포인트 거래 리스트 업데이트
	// params : void
	static const char* g_strFunc_PrivateMarketSearch_UpdatePointTrade = "CF_PrivateMarketSearch_UpdatePointTrade";
	// 개인 상점 검색 검색 최대 페이지 업데이트
	// params : int nMaxPage
	static const char* g_strFunc_PrivateMarketSearch_UpdateSearchMaxPage = "CF_PrivateMarketSearch_UpdateSearchMaxPage";
	// 개인 상점 검색 장바구니 최대 페이지 업데이트
	// params : int nMaxPage
	static const char* g_strFunc_PrivateMarketSearch_UpdateWishMaxPage = "CF_PrivateMarketSearch_UpdateWishMaxPage";

	// 현재 맵 변경
	// params : string strMapName, string strTexture, bool bInstantMap,
	//			DWORD dwServerID, DWORD dwClubID, string strClubName
	static const char* g_strFunc_Minimap_MoveActiveMap = "CF_Minimap_MoveActiveMap";

	// 우편함
	// 우편함 열기;
	static const char* g_strFunc_PostBox_Open = "CF_PostBox_Open";
	// 우편함 닫기
	// params : void
	static const char* g_strFunc_PostBox_Close = "CF_PoseBox_Close";
	// 우편함 열려 있나?
	// params : void
	static const char* CF_PostBox_IsOpen = "CF_PostBox_IsOpen";
	// 받은 우편 목록 받기완료 메시지;
	static const char* g_strFunc_PostBox_Msg_ReceivePost = "CF_PostBox_ReceivePost";
	// 우편 선택 완료 메시지;
	static const char* g_strFunc_PostBox_SelectedPostOpen = "CF_PostBox_SelectedPost";
	// 받은 우편 목록 업데이트
	static const char* g_strFunc_PostBox_ReceivePageUpdate = "CF_PostBox_ReceivePageUpdate";
	// 우편 형황 리스트 업데이트
	// params : void
	static const char* g_strFunc_PostBox_HistoryPageUpdate = "CF_PostBox_HistoryPageUpdate";
	// 우편 현황 보내기 목록 업데이트
	// params : void
	static const char* g_strFunc_PostBox_HistoryPageSendUpdate = "CF_PostBox_HistoryPageSendUpdate";
	// 우편 현황 받기 목록 업데이트
	// params : void
	static const char* g_strFunc_PostBox_HistoryPageRecieveUpdate = "CF_PostBox_HistoryPageRecieveUpdate";
	// 우편보내기 아이템 드래그 완료;
	static const char* g_strFunc_PostBox_SendPageAttachSlot = "CF_PostBox_SendPageAttachSlot";
	// 우편보내기 슬롯 리셋;
	static const char* g_strFunc_PostBox_SendPageResetSlot = "CF_PostBox_SendPageResetSlot";
	// 읽지 않은 우편 갯수 업데이트
	static const char* g_strFunc_PostBox_AcceptPost = "CF_PostBox_AcceptPost";
	// 우편 아이템 받기 - 인벤 공간 부족
	// parmas : int nError
	static const char* g_strFunc_PostBox_ErrMsg = "CF_PostBox_ErrMsg";

	// 스탯 초기화 열기
	// param : table tbData, tbEvenlyStats, tbReturnItem
	static const char* g_strFunc_StatsReset_Open = "CF_StatsReset_Open";
	// 스탯 초기화 열려있나?
	// params : void
	static const char* g_strFunc_StatsReset_IsOpen = "CF_StatsReset_IsOpen";
	// 스탯 초기화 재계산
	// parmas : void
	static const char* g_strFunc_StatsReset_ReCalc = "CF_StatsReset_ReCalc";
	// 스탯 초기화 결과
	// params : int nError, WORD wStatNum, WORD wSkillNum, WORD wItemNum
	static const char* g_strFunc_StatsReset_Result = "CF_StatsReset_Result";

	// 전체맵 캐릭터 검색 결과
	// params : table tbResult
	static const char* g_strFunc_LargeMap_SearchCharResult = "CF_LargeMap_SearchCharResult";
	// 전체맵 검색한 캐릭터 세부 정보
	// params : int nLevel, int nSchool, int nClass, DWORD dwGaeaID, bool bParty,
	//			string strName, string strTitle, string strClubName
	static const char* g_strFunc_LargeMap_SearchCharDeatil = "CF_LargeMap_SearchCharDetail";
	// 전체맵 검색한 캐릭터 지도에 표시
	// params : table tbMapPos
	static const char* g_strFunc_LargeMap_SearchCharMapPos = "CF_LargeMap_SearchCharMapPos";
	// 전체맵 캐릭터 이동 아이콘 끄기
	// params : void
	static const char* g_strFunc_LargeMap_HideMoveIcon = "CF_LargeMap_HideMoveIcon";
	
	// 라지맵 몹 아이콘 툴팁 텍스트 얻기;
	// param정보는 SetMobDataInTable에서 낱개로 보내는 몹정보와 SfLargemap_MakeTooltip_Mob와 동일하게 사용된다; 
	// params : table
	static const char* g_strFunc_LargeMap_GetMobTooltip = "CF_LargeMap_GetMobTooltip";

	// 생활기록부 열기
	// param : void
	static const char* g_strFunc_StudentRecord_Open = "CF_StudentRecord_Open";
	// 생활기록부 닫기
	// params : void
	static const char* g_strFunc_StudentRecord_Close = "CF_StudentRecord_Close";
	// 특별활동 페이지 업데이트
	// params void
	static const char* g_strFunc_StudentRecord_Update_Act = "CF_StudentRecord_ActivityPage_Update";
	// 특별활동 진행 메세지 출력;
	static const char* g_strFunc_StudentRecord_ProgressMessege = "CF_StudentRecord_ProgressMessege_Update";
	// 출석부 페이지 업데이트
	// params : void
	static const char* g_strFunc_StudentRecord_Update_Attendance = "CF_StudentRecord_AttendancePage_Update";
	// 출석부 내용 업데이트
	// params : void
	static const char* g_strFunc_StudentRecord_Update_AttendanceContents = "CF_StudentRecord_AttendancePage_UpdateContents";

	// 탈것 색상 변경 카드 사용
	// params : string strName
	static const char* g_strFunc_VehicleColor_ItemUse = "CF_VehicleColor_ItemUse";
	// 탈것 색상 변경 UI 열렸있나?
	// params : void
	static const char* g_strFunc_VehicleColor_IsOpen = "CF_VehicleColor_IsOpen";
	// 탈것 색상 변경 UI 끄기
	// params : void
	static const char* g_strFunc_VehicleColor_Close = "CF_VechicleColor_Close";
	// 펫 색상 변경 카드 사용
	// params : void
	static const char* g_strFunc_PetColor_ItemUse = "CF_PetColor_ItemUse";
	// 코스튬 색상 변경 카드 사용
	// params : bool bRestrict
	static const char* g_strFunc_CostumeColor_ItemUse = "CF_CostumeColor_ItemUse";

	// 전투 상태 알림(텍스쳐);
	// params : flaot fTime, int nState;
	static const char* g_strFunc_Toggle_NotifyCompetitionTexture = "CF_Toggle_Nofity_Competition_Texture";

	// CDM 전투 결과 list clear;
	static const char* g_strFunc_Competition_CDM_Clear_List = "CF_Competition_CDM_Result_Clear_List";
	// CDM 행 정보 채우기;
	// params : table Information of Column;
	static const char* g_strFunc_Competition_CDM_Add_List_Item = "CF_Competition_CDM_Result_Add_List_Item";	
	// CDM 전투 결과 open;
	static const char* g_strFunc_Competition_CDM_Open = "CF_Competition_CDM_Result_Open";
	// CDM 전투 결과 close;
	static const char* g_strFunc_Competition_CDM_Close = "CF_Competition_CDM_Result_Close";
	// CDM 현황판 갱신;
	// param : table Information of Club-Ranking;
	static const char* g_strFunc_Competition_CDM_Status_Board_Update = "CF_Competition_CDM_Status_Board_Update";
	// CDM 현황판 끄기;
	static const char* g_strFunc_Competition_CDM_Status_Board_Close = "CF_Competition_CDM_Status_Board_Close";
	// CDM 더블 포인트
	// params : bool bVisible
	static const char* g_strFunc_CDM_ShowDoublePoint = "CF_CDM_ShowDoublePoint";

	// 미니 게임 : 주사위;
	// params : void
	static const char* g_strFunc_MiniGame_Dice_Open = "CF_MiniGame_Dice_Open";
	static const char* g_strFunc_MiniGame_Dice_Close = "CF_MiniGame_Dice_Close";
	static const char* g_strFunc_MiniGame_Dice_Turn_Stage = "CF_MiniGame_Dice_Turn_Stage";

	// 클럽 공지 변경
	// params : void
	static const char* g_strFunc_Club_NoticeChange = "CF_Club_NoticeChange";
	// 클럽 창고 아이템 갱신
	// params : DWORD dwChannel, WORD wPosX, WORD wPosY
	static const char* g_strFunc_Club_ItemChange = "CF_Club_StorageItem_Change";
	// 클럽 창고 돈 갱신
	// params : LONGLONG llIncomeMoney, LONGLONG llMoney
	static const char* g_strFunc_Club_UpdateMoney = "CF_Club_Storage_UpdateMoney";
	// 클럽 별명 변경
	// params : string strNickName
	static const char* g_strFunc_Club_ChangeNickName = "CF_Club_NickName_Change";
	// 클럽 계급 이름 변경
	// params : void
	static const char* g_strFunc_Club_ChangeGradeName = "CF_Club_GradeName_Change";
	// 클럽 계급 권한 설정
	// params : void
	static const char* g_strFunc_Club_ChangeGradeAuth = "CF_Club_SetGrade_Auth";
	// 클럽 자동 추방 설정
	// params : void
	static const char* g_strFunc_Club_AutoBan = "CF_Club_AutoBan";
	// 클럽 창고로 열기
	// params : void
	static const char* g_strFunc_Club_OpenStorage = "CF_Club_OpenStorage";
	// 클럽 동맹/적대 클럽 리스트 업데이트
	// params : void
	static const char* g_strFunc_Club_UpdateAlliance = "CF_Club_UpdateAlliance";
	// 클럽 멤버 리스트 업데이트
	// params : void
	static const char* g_strFunc_Club_UpdateMember = "CF_Club_UpdateMember";
	// 클럽 권한 위임 메시지
	// params : string strClubName
	static const char* g_strFunc_Club_AuthorityMemberAsk = "CF_Club_AuthorityMemberAsk";
	// 클럽 로그 업데이트
	// params : void
	static const char* g_strFunc_Club_UpdateLog = "CF_Club_UpdateLog";
	// 클럽 동맹 가입 메시지
	// params : DWORD dwChiefCharID, string strChiefCharName
	static const char* g_strFunc_Club_AllianceJoinAsk = "CF_Club_AllianceJoinAsk";
	// 클럽 휴전 제안 메시지
	// params : DWORD dwClubID, string strClubName, bool bAlliance
	static const char* g_strFunc_Club_ArmisticeAsk = "CF_Club_ArmisticeAsk";
	// 클럽 배틀 신청 메시지
	// params : DWORD dwMasterID, string strClubMaster, DWORD dwBattleTime, bool bAlliance
	static const char* g_strFunc_Club_BattleAsk = "CF_Club_BattleAsk";
	// 클럽 초대(받는 쪽)
	static const char* g_strFunc_Club_JoinAsk = "CF_Ask_Club_Join";
	// 클럽 해산
	// params : bool bDissolution
	static const char* g_strFunc_Club_Dissolution = "CF_Club_Dissolution";
	// 클럽 등급 업데이트
	// params : void
	static const char* g_strFunc_Club_UpdateRank = "CF_Club_UpdateRank";
	// 클럽 이름 업데이트
	// params : void
	static const char* g_strFunc_Club_UpdateName = "CF_Club_UpdateName";
	// 클럽 선도 맵 업데이트
	// params : void
	static const char* g_strFunc_Club_UpdateGuidanceMap = "CF_Club_UpdateGuidanceMap";
	// 클럽 적대 클럽 리스트 업데이트
	static const char* g_strFunc_Club_UpdateBattleClubList = "CF_Club_UpdateBattleClubList";
	// 클럽 동맹 클럽 리스트 업데이트
	static const char* g_strFunc_Club_UpdateAllianceClubList = "CF_Club_UpdateAllianceClubList";
	// 클럽 전적 업데이트 
	static const char* g_strFunc_Club_UpdateBattleRecords = "CF_Club_UpdateBattleRecords";

	// 캐릭터 부활 정보
	// params : bool bResurrectionSkill, float fResurrectTime
	static const char* g_strFunc_Resurrection_ReceiveInfo = "CF_Resurrection_ReceiveInfo";
	// 부활창 UI 끄기
	// params : void
	static const char* g_strFunc_Resurrection_Close = "CF_Resurrection_Close";
	
	// 당첨자 업데이트
	// params : int nID
	static const char* g_strFunc_Lotto_WinManUpdate = "CF_Lotto_UpdateWinnerList";
	// 당첨 번호 업데이트
	// params : int nID, table tbNum
	static const char* g_strFunc_Lotto_WinNumUpdate = "CF_Lotto_UpdateWinNum";
	// 회차 누적 금액 업데이트
	// params : int nID, LONGLONG llMoney, bool bPrev
	static const char* g_strFunc_Lotto_AccumulateMoney = "CF_Lotto_AccumulateMoney";
	// 현재 구매리스트 업데이트
	// params : int nID, bool bPrev
	static const char* g_strFunc_Lotto_BuyListUpdate = "CF_Lotto_UpdateBuyList";
	// 다음턴 진행
	// params : int nID
	static const char* g_strFunc_Lotto_NextTurn = "CF_Lotto_NextTurn";
	// 로또 상태 변경
	// params : int nID, int nState
	static const char* g_strFunc_Lotto_ChangeState = "CF_Lotto_ChangeState";

	//퀘스트 시작
	static const char* g_strFunc_Quest_Start = "CF_Quest_Start";
	//퀘스트 끝
	static const char* g_strFunc_Quest_End = "CF_Quest_End";
	//퀘스트 진행(몹 제거)
	static const char* g_strFunc_Quest_ProgMobKill = "CF_Quest_ProGMobkill";
	//퀘스트 진행(퀘스트 아이템 습득)
	static const char* g_strFunc_Quest_ProgQItem = "CF_Quest_ProGQItem";
	//퀘스트 진행(퀘스트 아이템 사용)
	static const char* g_strFunc_Quest_ProgItemUse = "CF_Quest_ProGItemUse";
	//퀘스트 진행(연마 퀘스트 아이템 연마)
	static const char* g_strFunc_Quest_ProgItemGrinding = "CF_Quest_ProGItemGrinding";
	//퀘스트 진행(대련)
	static const char* g_strFunc_Quest_ProgConfting = "CF_Quest_ProGConfting";
	//퀘스트 진행(위치 도달)
	static const char* g_strFunc_Quest_ProgReachZone = "CF_Quest_ProGReachZone";
	//퀘스트 진행(경과 시간)
	static const char* g_strFunc_Quest_ProgTime = "CF_Quest_ProGTime";
	//퀘스트 진행(제한 시간 초과)
	static const char* g_strFunc_Quest_ProgTimeOver = "CF_Quest_ProGTimeOver";
	//퀘스트 진행(사망 제한)
	static const char* g_strFunc_Quest_ProgNonDie = "CF_Quest_ProGNonDie";
	//퀘스트 진행(레벨 도달)
	static const char* g_strFunc_Quest_ProgLevel = "CF_Quest_ProGLevel";
	//퀘스트 진행(레벨 업)
	static const char* g_strFunc_Quest_ProgLevelUp = "CF_Quest_ProGLevelUp";
	//퀘스트 진행(Npc 대화)
	static const char* g_strFunc_Quest_ProgNpcTalk = "CF_Quest_ProGNpcTalk";
	//퀘스트 진행(맵 이동 제한)
	static const char* g_strFunc_Quest_ProgLeaveMap = "CF_Quest_ProGLeaveMap";
	//퀘스트 진행(퀘스트 아이템 삽입)
	static const char* g_strFunc_Quest_ProgItemInsert = "CF_Quest_ProGItemInsert";
	//퀘스트 진행(퀘스트 아이템 삭제)
	static const char* g_strFunc_Quest_ProgItemDelete = "CF_Quest_ProGItemDelete";
	//퀘스트 진행(퀘스트 아이템 갯수 변화)
	static const char* g_strFunc_Quest_ProgItemCount= "CF_Quest_ProGItemCount";
	//퀘스트 진행(퀘스트 아이템 줏음)
	static const char* g_strFunc_Quest_ProgItemPickUp = "CF_Quest_ProGItemPickUp";

	// 퀘스트 (UI ) 업데이트
	static const char* g_strFunc_Quest_Update = "CF_Quest_Update";


	// 퀘스트 (UI ) 클리어 
	static const char* g_strFunc_Quest_Claer = "CF_Quest_Clear";

	//퀘스트 진행단계 변경
	static const char* g_strFunc_Quest_ProgChange = "CF_Quest_ProgChange";
	//퀘스트 진행중에서 삭제
	static const char* g_strFunc_Quest_DelProg = "CF_Quest_DelProG";
	//퀘스트 완료에서 삭제
	static const char* g_strFunc_Quest_DelEnd = "CF_Quest_DelEnd";

	// 개조 카드 UI 열기
	// params : -b
	static const char* g_strFunc_RebuildCard_Open = "CF_RebuildCard_Open";
	// 개조 카드 UI 닫기
	// params : void
	static const char* g_strFunc_RebuildCard_Close = "CF_RebuildCard_Close";
	// 개조 카드 아이템 등록
	// params : WORD wPosX, WORD WPosY, table tbItemInfo
	static const char* g_strFunc_RebuildCard_RegistItem = "CF_RebuildCard_RegistItem";
	// 개조 카드 인장 등록
	// params : WORD wMID, WORD wSID, string strItemName, WORD wFixNum
	static const char* g_strFunc_RebuildCard_RegistStemp = "CF_RebuildCard_RegistStemp";
	// 개조 카드 아이템 슬롯 해제
	// params : void
	static const char* g_strFunc_RebuildCard_RemoveItem = "CF_RebuildCard_RemoveItem";
	// 개조 카드 스템프 슬롯 해제
	// params : void
	static const char* g_strFunc_RebuildCard_RemoveStemp = "CF_RebuildCard_RemoveStemp";
	// 개조 카드 랜덤 옵션 갱신
	// params : table tbItemInfo, int nFixedOptionIndex
	static const char* g_strFunc_RebuildCard_Update = "CF_RebuildCard_Update";
	// 개조 카드 인장 갱신
	// params : void
	static const char* g_strFunc_Update_FixdOption = "CF_RebuildCard_Update_FixdOption";
	// 개조 카드 인장 체크해제
	// params : void
	static const char* g_strFunc_RebuildCard_StampUncheck = "CF_RebuildCard_StampUncheck";
	// 개조 확정 및 취소 팝업
	// params : void
	static const char* g_strFunc_RebuildCard_FixPopupUI = "CF_RebuildCard_FixPopupUI";
	// 개조 타입 변경
	// params : bool
	static const char* g_strFunc_RebuildCard_ChangedRebuildType = "CF_RebuildCard_ChangedRebuildType";
	// CTF 정보 업데이트
	// params : void
	static const char* g_strFunc_Competition_UpdateCTF = "CF_Competition_UpdateCTF";
	// CDM 랭크 정보 설정
	// param : table
	static const char* g_strFunc_Competition_CDMRankInfo = "CF_Competition_CDMRankInfo";
	// PVP 인던 정보 업데이트
	// params : void
	static const char* g_strFunc_Competition_UpdatePVP = "CF_Competition_UpdatePVP";
	// 단계형 인던 정보 업데이트
	// params : void
	static const char* g_strFunc_Competition_UpdatePVE = "CF_Competition_UpdatePVE";
	// 전장 UI : PvE인던 입장 확인 요청
	//params : bool bShow, DWORD dwMapID
	static const char* g_strFunc_Competition_PVE_AckEnternce= "CF_Competition_PVEAckEntrance";
	// 전장 UI : PvE인던 입장 확인 요청 결과
	//params : -n, -s
	static const char* g_strFunc_Competition_PVE_AckEnternceConfirm = "CF_Competition_PVE_AckEnternceConfirm";
	
	// 토너먼트 대진표 : 배팅정보
	// params : -l, -l, -dw
	static const char* g_strFunc_Tournament_BettingInfo = "CF_Tournament_BettingInfo";
	// 토너먼트 대진표 : 배팅그룹 정보
	// params : -dw, -dw, -l, -dw, -f
	static const char* g_strFunc_Tournament_BettingGroupInfo = "CF_Tournament_BettingGroupInfo";

	// 커뮤니티창 친구탭 업데이트
	// params : void
	static const char* g_strFunc_Community_FriendTab_Update		= "CF_Community_FriendTab_Update";
	static const char* g_strFunc_RnCommunity_FriendTab_Update	= "CF_RnCommunity_FriendTab_Update";
	static const char* g_strFunc_RnCommunity_BlockTab_Update	= "CF_Community_BlockTab_Update";
	// 친구 요청
	// params : string strName, bool bNotify
	static const char* g_strFunc_Community_ReqFriend = "CF_Community_ReqFriend";

	static const char* g_strFunc_Community_AddGroup = "CF_Community_FriendGroupAdd";
	static const char* g_strFunc_Community_RemoveGroup = "CF_Community_FriendGroupDelete";
	static const char* g_strFunc_Community_AddFriend = "CF_Community_FriendAdd";
	static const char* g_strFunc_Community_RemoveFriend = "CF_Community_FriendDelete";
	// Param: -n, -n
	// 이전 그룹, 새 그룹 번호;
	static const char* g_strFunc_Community_FriendSetGroup = "CF_Community_FriendSetGroup";
	static const char* g_strFunc_Community_FriendStateUpdate = "CF_Community_FriendStateUpdate";

	// DPS 업데이트
	// params : void
	static const char* g_strFunc_DPS_Update = "CF_DPS_Update";

	// CTF Notify Display;
	static const char* g_strFunc_CTFNotifyInfo = "CF_DisplayProgressNotify";
	static const char* g_strFunc_CTFAuthCountInit = "CF_CTFAuthCountInit";
	static const char* g_strFunc_CTFNotify_Ready = "CF_DisplayProgressNotify_Ready";

	// CTF Info
	static const char* g_strFunc_CTFDisplayInfo_Open = "CF_OpenCTFDisplayInfo";
	static const char* g_strFunc_CTFDisplayInfo_Close = "CF_CloseCTFDisplayInfo";
	
	static const char* g_strFunc_CTFieldHpDisplaySetPos = "CF_CTFieldHpDisplay_SetPos";
	static const char* g_strFunc_CTFieldHpDisplayClose = "CF_CTFieldHpDisplay_Close";

	static const char* g_strFunc_CTFieldDI_SetTime = "CTFieldDisplayInfo_SetTimeSync";
	static const char* g_strFunc_CTFieldDI_AuthenticatorUpdate = "CF_CTFieldDisplayInfo_AuthenticatorUpdate";
	static const char* g_strFunc_CTFieldDI_SetState = "CF_CTFieldDisplayInfo_SetCTFState";


	// 무한의 제단
	static const char* g_strFunc_PVE_OpenWidget				= "CF_OpenInstanceSystemUI";
	static const char* g_strFunc_PVE_EntranceState			= "CF_SetPVEEntranceState";	
	static const char* g_strFunc_PVE_Stage					= "CF_SetPVEStage";	
	static const char* g_strFunc_PVE_Message				= "CF_SetPVEMessage";	
	static const char* g_strFunc_PVE_MonsterIndicator		= "CF_SetPVEMonsterIndicator";	
	static const char* g_strFunc_PVE_PositioningIndicator	= "CF_SetPVEPositioningIndicator";	
	static const char* g_strFunc_PVE_StageIndicator			= "CF_SetPVEStageIndicator";	
	static const char* g_strFunc_PVE_StageState				= "CF_SetPVEStageState";	
	static const char* g_strFunc_PVE_PVERetry				= "CF_SetPVERetry";	
	static const char* g_strFunc_PVE_StageResult			= "CF_SetPVEStageResult";	

	// Matching System;
	static const char* g_strFunc_Mathing_ConfirmJoin		= "CF_MatchingConfirmJoin";

	// New Instance System;
	static const char* g_strFunc_NewInstance_Retry			= "CF_SetNewInstanceRetry";

	// 택시 카드 UI 열기
	// params : VOID
	static const char* g_strFunc_TaxiCard_Open = "CF_TaxiCard_Open";
	// 개조 카드 UI 닫기
	// params : void
	static const char* g_strFunc_TaxiCard_Close = "CF_TaxiCard_Close";

	// 공용 메세지 알림(메세지 박스X, 지정된 시간 동안 화면 상중단에 보여지는 문구);
	// param : -s, -n, -n, -n;
	static const char* g_strFunc_Toggle_Public_Notify_Message = "CF_Toggle_Public_Notify_Message";

	// ItemPreivew
	static const char* g_strFunc_ItemPreview_Open = "CF_ItemPreview_Open";
	static const char* g_strFunc_ItemPreview_IsOpen = "CF_ItemPreview_IsOpen";
	static const char* g_strFunc_ItemPreview_SetSlot = "CF_ItemPreview_SetSlot";

	// 스킬 사용 예약
	// params : DWORD dwID
	static const char* g_strFunc_SkillReserve_SetSkill = "CF_SkillReserve_SetSkill";

	// 스킬 창 임시 사용 금액
	static const char* g_strFunc_SkillUpMoney = "CF_Skill_GetSkillUpMoney";

	// 스킬 창 임시 포인트
	static const char* g_strFunc_SkillUsingPoint = "CF_Skill_GetUsingPoint";

	// 커서 타겟 정보 (NONE)
	// params : void
	static const char* g_strFunc_CursorTargetInfo_SetNone = "CF_CursorTargetInfo_SetNone";
	// 커서 타겟 정보 (PC)
	// params : string strName, WORD wLevel, WORD wSchool, int emClassIndex, int nPartyState, 
	//			DWORD dwPartyID, DWORD dwClubID, string strClubName, bool bPK
	static const char* g_strFunc_CursorTargetInfo_SetPC = "CF_CursorTargetInfo_SetPC";
	// 커서 타겟 정보 (NPC)
	// params : string strName, table tbTalk, bool bQuestStart, bool bQuestStep
	static const char* g_strFunc_CursorTargetInfo_SetNPC = "CF_CursorTargetInfo_SetNPC";
	// 커서 타겟 정보 (Mob)
	// params : string strName, WORD wLevel, bool bNonDmg
	static const char* g_strFunc_CursorTargetInfo_SetMob = "CF_CursorTargetInfo_SetMob";
	// 커거 타겟 HP
	// params : DWORD dwNow, DWORD dwMax
	static const char* g_strFunc_CursorTargetInfo_SetHP = "CF_CursorTargetInfo_SetHP";
	
	// CTField 보상 결과창 오픈
	// param : -n
	static const char* g_strFunc_CTFieldRewardResult_Open = "CF_CTFieldRewardResult_Open";

	// 게이트 이동창
	// params : table tbMapName
	static const char* g_strFunc_GateOpen = "CF_Gate_Open";
	// 게이트 이동창 끄기
	// params : void
	static const char* g_strFunc_GateClose = "CF_Gate_Close";

	// 버프 등록
	// params : DWORD dwSlot, int nBuffType, DWORD dwSkillID, WORD wLevel, float fAge, float fLife, bool bExpired
	static const char* g_strFunc_Buff_Reg = "CF_Buff_Reg";
	// 도시락 버프 중첩 확인
	// params : bool bOverlap, string strPrevSkill, string strCurSkill, WORD wPosX, WORD wPosY
	static const char* g_strFunc_Buff_LunchBoxOverlap = "CF_Buff_LunchBoxOverlap";
	// 버프 삭제 불가능 지역
	// params : string strSkillName
	static const char* g_strFunc_Buff_ImpossibleDelete = "CF_Buff_ImpossibleDelete";
	// 버프 삭제 확인
	// params : DWORD dwSkillID, string strSkillName, string strBinding
	static const char* g_strFunc_Buff_MsgDelete = "CF_Buff_MsgDelete";
	// 버프 삭제 완료
	// params : DWORD dwSlot
	static const char* g_strFunc_Buff_Deleted = "CF_Buff_Deleted";
	// 도시락 버프 받음
	// params : DWORD dwIncreaseRate, DWORD dwTotalRate, DWORD dwAccumCount, DWORD dwMaxCount
	//			string strSkillName, string strDesc, table tbTime
	static const char* g_strFunc_Buff_RecvLunchBox = "CF_Buff_RecvLunchBox";
	// 경험치 도시락 버프 중첩 횟수 설정
	// params : DWORD dwTotalRate, DWORD dwAccumCount, DWORD dwMaxCount
	static const char* g_strFunc_Buff_SetExpAccume = "CF_Buff_SetExpAccum";

	// 단축키 설정 UI 열려있나?
	// params : void
	static const char* g_strFunc_HotKeyConfig_IsOpen = "CF_HotKeyConfig_IsOpen";

	// 큐박스 버튼 잠금 설정
	// params : bool bEnable
	static const char* g_strFunc_QuickMenu_SetQBoxLock = "CF_QuickMenu_SetQBoxLock";
	// 큐박스 버튼 잠금 상태
	// params : void
	static const char* g_strFunc_QuickMenu_GetQBoxLock = "CF_QuickMenu_GetQBoxLock";
	// 달리기 상태 설정
	// params : bool bRun
	static const char* g_strFunc_QuickMenu_SetRun = "CF_QuickMenu_SetRun";
	// 탈것 장착
	// prams : bool bEquip, WORD wMID, WORD wSID, int nFuel
	static const char* g_strFunc_QuickMenu_EquipVehicle = "CF_QuickMenu_EquipVehicle";
	// 파티 아이템 분배 버튼 활성화;
	// params: bool - open/close
	static const char* g_strFunc_QuickMenu_OpenPartyTenderNotifyButton = "CF_QuickMenu_OpenPartyTenderNotifyButton";
	// 무기 부스터 설정;
	// params: 
	// 1- 일정 비율 이하시 퀵메뉴에 알리기위한 부스트비율;
	// 2- 현재 남아있는 부스트양;
	// 3- 부스트 총양;
	static const char* g_strFunc_QuickMenu_SetWeaponBoostInfo = "CF_QuickMenu_SetWeaponBoostInfo";
	// 매크로 상태 업데이트;
	static const char* g_strFunc_QuickMenu_MacroStateUpdate = "CF_QuickMenu_MacroStateUpdate";
	// 매크로 시간 세팅;
	// param : -f
	static const char* g_strFunc_QuickMenu_SetMacroTime = "CF_QuickMenu_SetMacroTime";
	
	// 귀환 매크로 버튼 해제
	static const char* g_strFun_ResetReMainButtons = "CF_Mo_ResetReMainButtons";

	// 퀵메뉴 UI 움직였는가?
	// return : bool
	static const char* g_strFunc_QuickMenu_IsMovedUI = "CF_QUICKMENU_ISMOVEDUI";

	// 퀵메뉴 UI 움직였는지 설정
	// param : -b
	static const char* g_strFunc_QuickMenu_SetMovedUI = "QuickMenu_SetMovedUI";

	// 베이직 채팅 폰트 크기
	static const char* g_strFunc_BasicChat_GetSize = "CF_BASICCHAT_GETSIZE";
	static const char* g_strFunc_BasicChat_SetSize = "CF_BASICCHAT_SETSIZE";
	
	// 큐박스 습득
	// params : int nType, float fTime
	static const char* g_strFunc_QBox_Take = "CF_QBox_Take";
	// 큐박스 리셋
	// params : void
	static const char* g_strFunc_QBox_Reset = "CF_QBox_Reset";
	// 2차 비번 입력
	// params : void
	static const char* g_strFunc_SecondPassword_Input = "CF_SecondPassword_Input";
	// 2차 비번 신규 설정
	// params : void
	static const char* g_strFunc_SecondPassword_New = "CF_SecondPassword_New";
	// 2차 비번 결과
	// params : int nError
	static const char* g_strFunc_SecondPassword_Result = "CF_SecondPassword_Result";

	// 버스 UI 오픈
	// params : void
	static const char* g_strFunc_BusStation_Open = "CF_BusStation_Open";

	// 플라잉 카메라 에러 메시지
	// params : string strError
	static const char* g_strFunc_FlyCamCtrl_MsgError = "CF_FlyCamCtrl_MsgError";
	// 플라잉 카메라 이동 속도 설정
	// params : float fVelo
	static const char* g_strFunc_FlyCamCtrl_SetMoveVelo = "CF_FlyCamCtrl_SetMoveVelo";
	// 플라잉 카메라 회전 속도 설정
	// params : float fVelo
	static const char* g_strFunc_FlyCamCtrl_SetRotateVelo = "CF_FlyCamCtrl_SetRotateVelo";
	// 플라잉 카메라 키프레임 추가
	// params : table tbKeyFrame
	static const char* g_strFunc_FlyCamCtrl_AddKeyFrame = "CF_FlyCamCtrl_AddKeyFrame";
	// 플라잉 카메라 키프레임 포지션 가져오기
	// params : void
	static const char* g_strFunc_FlyCamCtrl_GetKeyFramePos = "CF_FlyCamCtrl_GetKeyFramePos";
	// 플라잉 카메라 프로그래스바 업데이트
	// params : int nCurrentKey
	static const char* g_strFunc_FlyCamCtrl_UpdateProgress = "CF_FlyCamCtrl_UpdateProgress";
	
	// 캐릭터 이름 변경
	// params : void
	static const char* g_strFunc_CharRename = "CF_CharRename";

	// 클럽 이름 변경
	// params : void
	static const char* g_strFunc_ClubRename = "CF_ClubRename";

	// 개인상점
	// 개인상점 이름설정 메시지박스 열기
	static const char* g_strFunc_PrivateMarket_TitleMsgBox_Open = "CF_PrivateMarket_TitleMsgBox_Open";
	static const char* g_strFunc_PrivateMarket_Close = "CF_PrivateMarket_Close";
	static const char* g_strFunc_PrivateMarket_IsOpen = "CF_PrivateMarket_IsOpen";
	static const char* g_strFunc_PrivateMarket_IsOpener = "CF_PrivateMarket_IsOpener";
	static const char* g_strFunc_PrivateMarket_RegItem = "CF_PrivateMarket_RegItem";
	static const char* g_strFunc_PrivateMarket_InsertItem = "CF_PrivateMarket_InsertItem";
	// 개인 상점 아이템 업데이트
	// params : table tbItemList
	static const char* g_strFunc_PrivateMarket_Update = "CF_PrivateMarket_Update";
	// 개인 상점 블락 아이템인지 확인
	// params : WORD wPosX, WORD wPosY
	//static const char* g_strFunc_PrivateMarket_IsBlock = "CF_PrivateMarket_IsBlock";

	// 개인상점 개설관련
	static const char* g_strFunc_PrivateMarket_MsgTitleFb = "CF_PrivateMarket_MsgTitleFb";
	static const char* g_strFunc_PrivateMarket_MsgOpenFb = "CF_PrivateMarket_MsgOpenFb";
	static const char* g_strFunc_PrivateMarket_OpenFb = "CF_PrivateMarket_OpenFb";

	// 타 캐릭터 개인상점 열기
	// params : DWORD dwID, string strName, string strTitle
	static const char* g_strFunc_PrivateMarket_ReqPMInfo = "CF_PrivateMarket_ReqPMInfo";

	// 타 캐릭터 개인상점 닫기
	static const char* g_strFunc_PrivateMarket_ReqPMClose = "CF_PrivateMarket_CloseWidget";

	// 개인상점 물품 구입
	static const char* g_strFunc_PrivateMarket_ReqBuy = "CF_PrivateMarket_ReqBuy";
	static const char* g_strFunc_PrivateMarket_ReqBuyEx = "CF_PrivateMarket_ReqBuyEx";

	// 락커 열려있나?
	// params : void
	static const char* g_strFunc_Locker_IsOpen = "CF_Locker_IsOpen";
	// 락커 연결 카드 사용 가능
	// params : WORD wPosX, WORD wPosY
	static const char* g_strFunc_Locker_CanUseOpenCard = "CF_Locker_CanUseOpenCard";
	// 락커 돈 업데이트
	// params  : LONGLONG llMoney
	static const char* g_strFunc_Locker_UpdateMoney = "CF_Locker_UpdateMoney";
	// 락커 UI 열기
	// params : void
	static const char* g_strFunc_Locker_Open = "CF_Locker_Open";
	// 락커 아이템 업데이트
	// params : int nIndex
	static const char* g_strFunc_Locker_UpdateItem = "CF_Locker_UpdateItem";
	// 락커 확장 열기 
	// params : WORD wPosX, WORD wPosY (락커 확장 카드 인벤 위치)
	static const char* g_strFunc_LockerExpand_Open = "CF_LockerExpand_Open";
	// 락커 연결 카드 사용
	// params : void
	static const char* g_strFunc_Locker_UseOpenCard = "CF_Locker_UseOpenCard";
	// 락커 확장 성공, 락커 업데이트
	// params : void
	static const char* g_strFunc_Locker_Update = "CF_Locker_Update";

	// 프리미엄 상태 변경
	// params : void
	static const char* g_strFunc_UpdatePremium = "CF_UpdatePremium";

	// 아이템 박스 사용
	// params : WORD wPosX, WORD wPosY, string strItemName
	static const char* g_strFunc_UseItemBox = "CF_UseItemBox";
	// 아이템 포인트 카드
	// params : WORD wPosX, WORD wPosY, string strItemName
	static const char* g_strFunc_UseItemPointCard = "CF_UseItemPointCard";

	// 제작 재료 아이템(랜덤옵션이 있는) 확인
	static const char* g_strFunc_ProductNeedConfirm = "CF_ProductNeedConfirm";

	// 대련 디스플레이 출력
	// params : int nIndex
	static const char* g_strFunc_ConftDisplay = "CF_ConftDisplay";

	// 수수료 설정 메시지 박스
	// params : void
	static const char* g_strFunc_SetCommission = "CF_SetCommission";
	// NPC 경험치 회복
	// params : DWORD dwNpcGlobalID, LONGLONG llReExp, LONGLONG llDecMoney
	static const char* g_strFunc_RecoveryNpcExp = "CF_RecoveryNpcExp";
	// 맵 진입 조건 불일치
	// params : void
	static const char* g_strFunc_RegenEntryFail = "CF_RegenEntryFail";

	// 보너스 타임 시작
	// params : bool bBusterTime, int nTime, int nRemainTime
	static const char* g_strFunc_BonusTime_StartEvent = "CF_BonusTime_StartEvent";
	// 보너스 타임 끝
	// params : void
	static const char* g_strFunc_BonusTime_EndEvent = "CF_BonusTime_EndEvent";

	// 탈것 연료 업데이트
	// params : int nFuel
	static const char* g_strFunc_Vehicle_UpdateFuel = "CF_Vehicle_UpdateFuel";
	// 탈것 슬롯 바인딩 ID -> 파츠 타입
	// params : string strBinding
	static const char* g_strFunc_Vehicle_Binding2Parts = "CF_Vehicle_Binding2Parts";
	// 탈것 UI 업데이트
	// params : void
	static const char* g_strFunc_Vehicle_Update = "CF_Vehicle_Update";
	// 탈것 장착
	// params : void
	static const char* g_strFunc_Vehicle_Equip = "CF_Vehicle_Equip";
	// 탈것 UI 열려 있나?
	// params : void
	static const char* g_strFunc_Vehicle_IsOpen = "CF_Vehicle_IsOpen";
	// 탈것 부품 슬롯 하이라이트
	// params : int nVehicleType, int nPartsType
	static const char* g_strFunc_Vehicle_SlotHighlight = "CF_Vehicle_SlotHighlight";

	// 깃발전 UI 오픈
	// params : bool bFlag
	static const char* g_strFunc_CTFlag_Open = "CF_CTFlag_Open";
	// 깃발전 타이머 설정
	// params : float fTime
	static const char* g_strFunc_CTFlag_SetTime = "CF_CTFlag_SetTime";
	// 깃발전 깃발 소유자 이름 설정
	// params : bool bLeft, string strName, DWORD dwColor
	static const char* g_strFunc_CTFlag_SetName = "CF_CTFlag_SetName";
	// 깃발전 점수 설정
	// params : bool bLeft, int nScore, DWORD dwColor
	static const char* g_strFunc_CTFlag_SetScore = "CF_CTFlag_SetScore";
	// 깃발전 아이콘 설정
	// params : bool bLeft, string strXMLKey
	static const char* g_strFunc_CTFlag_SetIcon = "CF_CTFlag_SetIcon";
	// 깃발전 이미지 설정
	// params : bool bLeft, string strXMLKey, bool bBlink
	static const char* g_strFunc_CTFlag_SetImage = "CF_CTFlag_SetImage";
	// 깃발전 팀 오브젝트 텍스트 설정
	// params : string strTeamObject, DWORD dwColor
	static const char* g_strFunc_CTFlag_SetObjectText = "CF_CTFlag_SetObjectText";

	// 세트 아이템 미리보기 열기
	// params : string strSetItemName, int nWoreCount, int nTotal, table tbItemList
	static const char* g_strFunc_SetItem_Open = "CF_SetItem_Open";

	// 스킬 캔슬;
	// Param : Void
	static const char* g_strFunc_ReservationSkillCancel_All = "SkillReservation_CancelAll";
	// 상호관계 UI를 통해 탑승 요청에 대한 모달
	// Param : str - 요청자 이름
	static const char* g_strFunc_Modal_VehicleWith = "CF_Modal_VehicleWith";
	// 탈것에 탑승함;
	// Param : bool - 운전자/동승자, bool - 부스터
	static const char* g_strFunc_RideVehicle = "CF_RideVehicle";
	// 탈것에서 내림;
	// Param : bool - 운전자/동승자
	static const char* g_strFunc_GetoffVehicle = "CF_GetoffVehicle";
	// 동승 요청한 유저에게서 답변이 옴;
	// param1 : bool - 수락/거절;
	// param2 : dw - 답장온 유저 아이디;
	static const char* g_strFunc_VehicleReqTargetAnswer = "CF_VehicleReqTargetAnswer";
	static const char* g_strFunc_QuickMenuUpdateRideWithState = "CF_QuickMenu_UpdateRideWithState";

	// 국가 선택 업데이트
	// params : WORD wContinentID, WORD wCountryID
	static const char* g_strFunc_UpdateSelectCountry = "CF_UpdateSelectCountry";

	// 디스플레이 국가 이미지 프레임 ID 얻기
	// params : DWORD dwCountryID
	static const char* g_strFunc_Display_GetCountryFrameID = "CF_Display_GetCountryFrameID";
	// 난사군도 순위 업데이트
	// params : table tbRankList, int nType
	static const char* g_strFunc_CTI_UpdateRank = "CF_CTI_UpdateRank";
	// 난사군도 내 순위 업데이트
	// params : int nRank, int nScore
	static const char* g_strFunc_CTI_UpdateMyRank = "CF_CTI_UpdateMyRank";
	// 난사군도 승전국 업데이트
	// params : WORD wCountryID, string strCountryName
	static const char* g_strFunc_CTI_UpdateVictoriousCountry = "CF_CTI_UpdateVictoriousCountry";
	// 난사군도 진행 정보 업데이트
	// params : int nXMLIndex, int nProgressIndex, int nScore, float fTime
	static const char* g_strFunc_CTI_UpdateCountryProgressInfo = "CF_CTI_UpdateCountryProgressInfo";
	// 난사군도 인증기
	// params : bool bVisible, float fTime
	static const char* g_strFunc_CTI_Auth = "CF_CTI_Auth";
	// 난사군도 전장 서버 입장 실패
	// params : int nError
	static const char* g_strFunc_CTI_BattleServerEntranceFail = "CF_CTI_BattleServerEntranceFail";
	// 난사군도 전장 서버 입장 대기
	// params : void
	static const char* g_strFunc_CTI_BattleServerEnterWait = "CF_CTI_BattleServerEnterWait";
	// 난사군도 결과 리스트 타이틀 설정
	// params : table tbData
	static const char* g_strFunc_CTI_SetRewardTitle = "CF_CTI_SetRewardTitle";
	// 난사군도 결과 리스트 데이터
	// params : table tbData 
	static const char* g_strFunc_CTI_AddRewardData = "CF_CTI_AddRewardData";
	// 난사군도 결과 UI 오픈
	// params : int nOutTime
	static const char* g_strFunc_CTI_OpenReward = "CF_CTI_OpenReward";
	// 국가 초기화 카드 사용
	// params : bool bIsSelected, WORD wInvenPosX, WORD wInvenPosY
	static const char* g_strFunc_UseResetCountryCard = "CF_UseResetCountryCard";

	// 난사군도 미니맵 보급품, 인증기 위치
	// params : table tbList
	static const char* g_strFunc_CTI_MinimapAuthPos = "CF_CTI_MinimapAuthPos";
	// 선도전 미니맵 인증기, 가속기 위치
	// params : table tbList
	static const char* g_strFunc_Guidance_MinimapAuthPos = "CF_Guidance_MinimapAuthPos";

	// 코스튬 능력치 부여 사용 확인
	// params : int nType, int nInvenPosX, int nInvenPosY
	static const char* g_strFunc_CostumeStat_Use = "CF_CostumeStat_Use";
	// 코스튬 능력치 부여 관련 에러
	// params : int nError
	static const char* g_strFunc_CostumeStat_Error = "CF_CostumeStat_Error";
	// 코스튬 능력치 해제
	// params : WORD wPosX, WORD wPosY, WORD wHoldPosX, WORD wHoldPosY
	static const char* g_strFunc_CostumeStat_Release = "CF_CostumeStat_Release";

	// 아이템 수리 UI 오픈
	// params : void
	static const char* g_strFunc_ItemRepair_Open = "CF_ItemRepair_Open";
	// 아이템 수리 업데이트
	// params : void
	static const char* g_strFUnc_ItemRepair_Update = "CF_ItemRepair_Update";

	// 부스터 상태
	// params : int nState
	static const char* g_strFunc_BoosterGauge_State = "CF_BoosterGauge_State";
	// 부스터 배움
	// params : bool bRide
	static const char* g_strFunc_BoosterGauge_Learn = "CF_BoosterGauge_Learn";

	// 일시강화 메세지박스
	static const char* g_strFunc_TempEnchant = "CF_TempEnchant";

	// 캐릭터 정보창 갱신(이름)
	static const char* g_strFunc_CharInfo_Update = "CF_CharInfo_Update";

	static const char* g_strFunc_OpenMsgBox = "CF_OpenMsgBox";

	// 부스터 툴팁 헤더, 몸통;
	static const char* g_strFunc_GetItemDurabilityTooltipHeader = "CF_GetDurabilityTooltipHeader";
	static const char* g_strFunc_GetItemDurabilityTooltip = "CF_GetDurabilityTooltipInfo";

	// 소환수 슬롯 on/off;
	// param : -b, -b;
	// Visible 여부, 고용 여부;
	static const char* g_strFunc_SetVisibleSummonState = "CF_SetVisibleSummonState";
	// 소환수 상태 업데이트;
	// param : -dwAction;
	static const char* g_strFunc_UpdateSummonAction = "CF_UpdateSummonAction";
	// 파티 분배 아이템 리스트 갱신;
	static const char* g_strFunc_PDD_Update = "CF_PDD_Update";
	// 파티 분배와 관련된 UI 모두 끄기;
	static const char* g_strFunc_PDU_Close = "CF_ClosePartyDistributionUI";

	// 헤러 컬러 변경 카드 사용
	// params : void
	static const char* g_strFunc_UseHairColorCard = "CF_HairColor_Use";

	// 헤어 컬러 변경 위젯이 열려있는가.
	static const char* g_strFunc_HairColor_IsOpen = "CF_HairColor_IsOpen";

	// 경고 이미지 켜기/끄기
	// params : bool bShow
	static const char* g_strFunc_ShowWarning = "CF_ShowWarning";

	// 깃발전 결과 타이틀 정보 설정
	// params : table tbTitle 
	static const char* g_strFunc_CTFlagResult_SetTitle = "CF_CTFlagResult_SetTitle";
	// 깃발전 결과 리스트 정보 추가
	// params : table tbInfo 
	static const char* g_strFunc_CTFlagResult_AddInfo = "CF_CTFlagResult_AddInfo";
	// 깃발전 결과 오픈
	// params : int nOutTime
	static const char* g_strFunc_CTFlagResult_Open = "CF_CTFlagResult_Open";

	// 선도전 결과창 타이틀
	// params : table tbTitle
	static const char* g_strFunc_GuidanceResult_SetTitle = "CF_GuidanceResult_SetTitle";
	// 선도전 결과 리스트 정보 추가
	// params : table tbInfo
	static const char* g_strFunc_GuidancResult_AddInfo = "CF_GuidanceResult_AddInfo";
	// 선도전 결과 오픈
	// params : int nOutTime
	static const char* g_strFunc_GuidanceResult_Open = "CF_GuidanceResult_Open";

	// 기간 연장 확인 메세지 박스;
	// params : word wHoldPosX, wHoldPosY, wInvenPosX, wInvenPosY
	static const char* g_strFunc_PeriodExConfirm = "CF_PeriodExConfirm";

	// 기간 연장 시도시 기간 확인 메세지 박스;
	// params : word wHoldPosX, wHoldPosY, wInvenPosX, wInvenPosY
	static const char* g_strFunc_PeriodExReconfirmOpenMsgBox = "CF_PeriodExReconfirmOpenMsgBox";

	// 학원 변경 메시지
	// params : void
	static const char* g_strFunc_SchoolChangeOK = "CF_SchoolChangeOK";

	// HTML 뷰 위치, 크기
	// params : void
	static const char* g_strFunc_GetHTMLViewSize = "CF_GetHTMLViewSize";

	// 출석부 과제 완료 디스플레이 켜기
	// params : string strInfo
	static const char* g_strFunc_AttendanceComplete_Show = "CF_AttendanceComplete_Show";

	// 인던 나가기
	// parmas : int nOutTime
	static const char* g_strFunc_DungeonExit_Show = "CF_DungeonExit_Show";

	// 포장 아이템 풀기
	// params : string strItemName, WORD wPosX, WORD wPosY
	static const char* g_strFunc_UnwrapItem = "CF_UnwrapItem";

	// 펫 스킨 카드 사용
	// params : DWORD dwItemID, WORD wPosX, WORD wPosY, WORD wHoldPosX, WORD wHoldPosY
	static const char* g_strFunc_UsePetSkinCard = "CF_UsePetSkinCard";

	// 인벤토리 아이템 위치 변경
	// params : int nPosX1, int nPosY1, int nPosX2, int nPosY2
	static const char* g_strFunc_SwapInvenItemPos = "CF_SwapInvenItemPos";

	// 코스튬 씌울 경우 기간 연장 메시지
	// params : int nPosX, int nPosY, int nHoldPosX, int nHoldPosY
	static const char* g_strFunc_CostumeDisguiseTimeCheck = "CF_CostumeDisguiseTimeCheck";

	// 
	static const char* g_strFunc_SelformBox_SetBoxID = "CF_SelformBox_SetBoxID";

	// 경험치 캡슐 아이템 사용 여부 확인 팝업;
	static const char* g_strFunc_UseExpCapsuleItem = "CF_UseExpCapsuleItem";

	// 경험치 압축 아이템 사용 여부 확인 팝업;
	static const char* g_strFunc_UseExpCompressorItem = "CF_UseExpCompressorItem";

	// 캐릭터 슬롯 확장 아이템 사용 여부 확인 팝업;
	static const char* g_strFunc_UseUnlockCharacterSlotItem = "CF_UseUnlockCharacterSlotItem";

	// 로비 대기 메세지 박스 닫기;
	static const char* g_strFunc_LobbyWaitMsgBoxClose = "CF_LobbyWaitMsgBoxClose";

	// UI TimerMessageBox
	static const char* g_strFunc_UITimerMessageBox = "CF_UITimerMessageBox";

	// 란 모바일 계정 연동 결과
	static const char* g_strFunc_RanMobile_MsgRecognize = "CF_RanMobile_MsgRecognize";
	// 란 모바일 계정 정보
	static const char* g_strFunc_RanMobile_MsgInfo = "CF_RanMobile_MsgInfo";
	// 란 모바일 계정 정보
	static const char* g_strFunc_RanMobile_MsgInfoAll = "CF_RanMobile_MsgInfoAll";
	// 란 모바일 M포인트 요청 결과
	static const char* g_strFunc_RanMobile_MsgGetPoint = "CF_RanMobile_MsgGetPoint";
	// 란 모바일 시간 연장 결과
	static const char* g_strFunc_RanMobile_MsgExtendTime = "CF_RanMobile_MsgExtendTime";
	// 란 모바일 보상 교환 결과
	static const char* g_strFunc_RanMobile_MsgChangeItem = "CF_RanMobile_MsgChangeItem";
	// 란 모바일 보석 전송 결과
	static const char* g_strFunc_RanMobile_MsgSendJewel = "CF_RanMobile_MsgSendJewel";
	// 란 모바일 일일 보상 결과
	static const char* g_strFunc_RanMobile_MsgDailyReward = "CF_RanMobile_MsgDailyReward";
	// 란 모바일 연동권 사용
	static const char* g_strFRunc_RanMobile_UseRecognizeItem = "CF_RanMobile_UseRecognizeItem";

	// 캐릭터가 피해를 입을 경우 호출
	static const char* g_strFunc_ReceiveDamage_Character = "CF_ReceiveDamage_Character";

	//----------------------------------------------------------------------------------
	// 디스플레이 무비클립

	// 이름 디스플레이 --------------------------------------- 
	extern float g_fDef_Display_ShowNameDistanceRatio;				// 이름 출력할 거리 비율
	extern std::string g_strDef_Display_NameMC;						// 이름 디스플레이 Linkage
	extern std::string g_strDef_Display_NameMC_tfName;				// 이름 텍스트 필드 인스턴스
	extern std::string g_strDef_Display_NameMC_mcNameBG;			// 이름 텍스트 배경
	extern std::string g_strDef_Display_NameMC_tfClub;				// 클럽 텍스트 필드 인스턴스
	extern std::string g_strDef_Display_NameMC_mcClubBG;			// 클럽 텍스트 배경
	extern std::string g_strDef_Display_NameMC_mcSchool;			// 학원 아이콘 무비클립 인스턴스
	extern std::string g_strDef_Display_NameMC_mcCountry;			// 국가 이미지 인스턴스
	extern std::string g_strDef_Display_NameMC_mcParty;				// 파티 아이콘 무비클립 인스턴스
	extern std::string g_strDef_Display_NameMC_rtClubIcon;			// 클럽 아이콘 렌더텍스처 인스턴스
	extern std::string g_strDef_Display_NameMC_rtClubIconMC;		// 클럽 아이콘 렌더텍스처 무비클립 ID
	extern std::string g_strDef_Display_NameMC_rtClubIconLink;		// 클럽 아이콘 비트맵 ID
	extern std::string g_strDef_Display_NameMC_barHP;				// HP 바
	extern int g_nDef_Display_NameMC_bgOffsetX;						// 이름 디스플레이 배경 오프셋 X
	extern int g_nDef_Display_NameMC_bgOffsetY;						// 이름 디스플레이 배경 오프셋 Y
	extern std::string g_strDef_Display_ItemNameMC;					// 아이템 이름 디스플레이 Linkage

	// 학원 아이콘 프레임 ID
	extern std::string g_strDef_Display_NameMC_NoSchool;
	extern std::string g_strDef_Display_NameMC_SMSchool;
	extern std::string g_strDef_Display_NameMC_HASchool;
	extern std::string g_strDef_Display_NameMC_BHSchool;

	// 파티 아이콘 프레임 ID
	extern std::string g_strDef_Display_NameMC_NoParty;
	extern std::string g_strDef_Display_NameMC_PartyMaster;
	extern std::string g_strDef_Display_NameMC_PartyTarget;
	extern std::string g_strDef_Display_NameMC_PartyGeneral;

	// 헤드쳇 디스플레이 ----------------------------------------
	extern std::string g_strDef_Display_HeadChatMC;		// 헤드쳇 디스플레이 Linkage
	extern std::string g_strDef_Display_HeadChat_tf;	// 텍스트 필드 인스턴스
	extern std::string g_strDef_Display_HeadChat_bg;	// 배경 인스턴스
	extern float g_fDef_Display_HeadChatLifeTime;		// 보여질 시간

	// HP 디스플레이 --------------------------------------------
	extern std::string g_strDef_Display_HpRedMC;
	extern std::string g_strDef_Display_HpGreenMC;
	extern std::string g_strDef_Display_HpBlueMC;
	extern std::string g_strDef_Display_HpGreenSimpleMC;

	extern std::string g_strDef_Display_Hp_tf;
	extern std::string g_strDef_Display_Hp_bar;

	// 데미지 디스플레이 -----------------------------------------
	extern std::string g_strDef_Display_DmgMc;			// 데미지 무비클립
	extern float g_fDef_Display_DmgLifeTime;			// 데미지 리셋 시간
	extern float g_fDef_Display_DmgRandOffsetRange;		// 데미지 위치 랜덤 오프셋 범위
	extern float g_fDef_Display_DmgHeadOffsetX;			// 데미지 위치 머리로부터 오프셋 X
	extern float g_fDef_Display_DmgHeadOffsetY;			// 데미지 위치 머리로부터 오프셋 Y

	// 개인상점 디스플레이 ----------------------------------------
	extern std::string g_strDef_Display_PrivateMarket;
	extern std::string g_strDef_Display_PrivateMarket_Button;
	extern std::string g_strDef_Display_PrivateMarket_TextPM;
	extern std::string g_strDef_Display_PrivateMarket_TextName;
	extern std::string g_strDef_Display_PrivateMarket_TextPMName;

	extern int g_nDisplayLimitCount;

	//----------------------------------------------------------------------------------

	extern bool g_bDef_WidgetCLoseToDeleteUseBatch;	// UI 삭제 대기 시간 일괄 설정 사용?
	extern float g_fDef_WidgetCloseToDeleteTime;	// UI 닫은 후 삭제될 시간

	extern int g_nDef_SlotTooltipMaxWidth;			// 슬롯 툴팁 Width 최대값
	extern int g_nDef_SlotTooltipMinWidth;			// 슬롯 툴팁 Width 최소값

	//----------------------------------------------------------------------------------

	extern DWORD g_dwDef_ItemSlotCountColor;		// 아이템 슬롯 개수 컬러
	extern DWORD g_dwDef_ItemSlotMaxCountColor;		// 아이템 슬롯 최대 개수 컬러

	//----------------------------------------------------------------------------------
	enum EMMSGBOX_TYPE	// MessageBox.lua에 있는 전역 변수들과 동일해야한다;
	{
		SFMSG_BOX_TYPE_OK 		= 1,
		SFMSG_BOX_TYPE_CANCEL 	= 2,
		SFMSG_BOX_TYPE_OKCANCEL = 3,
		SFMSG_BOX_TYPE_INPUT 	= 4,
		SFMSG_BOX_TYPE_INFO 	= 5
	};

	// 버프 스킬 타입
	enum EMSKILL_BUFF_TYPE
	{
		EMBUFF_NONE = -1,

		EMBUFF_NORMAL = 0,
		EMBUFF_ADD,
		EMBUFF_LUNCHBOX,
		EMBUFF_EXP_LUNCHBOX,
		EMBUFF_SYSTEM,
		EMBUFF_CTF_REWARD,
	};

	// 인벤 휴지통 에러코드
	enum EMINVEN_TRASH
	{
		EMINVEN_TRASH_OK		= 0,	// 성공
		EMINVEN_TRASH_NOREGIST	= 1,	// 등록된 아이템 없음.
		EMINVEN_TRASH_NOINVEN	= 2,	// 인벤에 아이템이 없음.
		EMINVEN_TRASH_MONEY		= 3,	// 돈이 부족함.
		EMINVEN_TRASH_NOITEM	= 4,	// 아이템을 찾을 수 없음.
	};

	// 대련 디스플레이 인덱스
	enum EMCONFT_DISP
	{
		EMCONFT_DISP_WIN	= -1,
		EMCONFT_DISP_LOSE	= -2,
		EMCONFT_DISP_DRAW	= -3,
	};

	//----------------------------------------------------------------------------------

	enum EMPREPARE_HTML
	{
		HTML_GT	= 0,
		HTML_LT,
		HTML_SEMICOLON,
		HTML_AMP,
		HTML_QUOT,
		HTML_APOS,

		HTML_SIZE
	};

	static const char* g_strPrepareHTML[ HTML_SIZE ][ 3 ] =
	{
		{ ">", "&gt<;>", "&gt;" },
		{ "<", "&lt<;>", "&lt;" },
		{ ";", "<;>", ";" },
		{ "&", "&amp<;>", "&amp;" },
		{ "\"", "&quot<;>", "&quot;" },
		{ "'", "&apos<;>", "&apos;" },
	};

	static const wchar_t* g_wstrPrepareHTML[ HTML_SIZE ][ 3 ] =
	{
		{ L">", L"&gt<;>", L"&gt;" },
		{ L"<", L"&lt<;>", L"&lt;" },
		{ L";", L"<;>", L";" },
		{ L"&", L"&amp<;>", L"&amp;" },
		{ L"\"", L"&quot<;>", L"&quot;" },
		{ L"'", L"&apos<;>", L"&apos;" },
	};

	// HTML 작성을 위해 기존 문자열에 특수문자가 들어간다면 HTML 태그로 변경
	static std::string PrepareHTMLData( const char* str )
	{
		std::string strRet = "";

		for( ; *str; ++str )
		{
			if( 0 == strncmp( str, g_strPrepareHTML[ HTML_GT ][ 0 ], 1 ) )
			{
				strRet += g_strPrepareHTML[ HTML_GT ][ 1 ];
				continue;
			}
			else if( 0 == strncmp( str, g_strPrepareHTML[ HTML_LT ][ 0 ], 1 ) )
			{
				strRet += g_strPrepareHTML[ HTML_LT ][ 1 ];
				continue;
			}
			else if( 0 == strncmp( str, g_strPrepareHTML[ HTML_SEMICOLON ][ 0 ], 1 ) )
			{
				strRet += g_strPrepareHTML[ HTML_SEMICOLON ][ 1 ];
				continue;
			}
			else if( 0 == strncmp( str, g_strPrepareHTML[ HTML_AMP ][ 0 ], 1 ) )
			{
				strRet += g_strPrepareHTML[ HTML_AMP ][ 1 ];
				continue;
			}
			else if( 0 == strncmp( str, g_strPrepareHTML[ HTML_QUOT ][ 0 ], 1 ) )
			{
				strRet += g_strPrepareHTML[ HTML_QUOT ][ 1 ];
				continue;
			}
			else if( 0 == strncmp( str, g_strPrepareHTML[ HTML_APOS ][ 0 ], 1 ) )
			{
				strRet += g_strPrepareHTML[ HTML_APOS ][ 1 ];
				continue;
			}

			strRet += str[ 0 ];
		}

		return strRet;
	}

	static std::string PrepareHTMLString( const char* str )
	{
		std::string strRet = "";

		for( ; *str; ++str )
		{
			if( 0 == strncmp( str, g_strPrepareHTML[ HTML_GT ][ 0 ], 1 ) )
			{
				strRet += g_strPrepareHTML[ HTML_GT ][ 2 ];
				continue;
			}
			else if( 0 == strncmp( str, g_strPrepareHTML[ HTML_LT ][ 0 ], 1 ) )
			{
				strRet += g_strPrepareHTML[ HTML_LT ][ 2 ];
				continue;
			}
			else if( 0 == strncmp( str, g_strPrepareHTML[ HTML_SEMICOLON ][ 0 ], 1 ) )
			{
				strRet += g_strPrepareHTML[ HTML_SEMICOLON ][ 2 ];
				continue;
			}
			else if( 0 == strncmp( str, g_strPrepareHTML[ HTML_AMP ][ 0 ], 1 ) )
			{
				strRet += g_strPrepareHTML[ HTML_AMP ][ 2 ];
				continue;
			}
			else if( 0 == strncmp( str, g_strPrepareHTML[ HTML_QUOT ][ 0 ], 1 ) )
			{
				strRet += g_strPrepareHTML[ HTML_QUOT ][ 2 ];
				continue;
			}
			else if( 0 == strncmp( str, g_strPrepareHTML[ HTML_APOS ][ 0 ], 1 ) )
			{
				strRet += g_strPrepareHTML[ HTML_APOS ][ 2 ];
				continue;
			}

			strRet += str[ 0 ];
		}

		return strRet;
	}

	static std::wstring PrepareHTMLStringW( const wchar_t* str )
	{
		std::wstring strRet = L"";

		for( ; *str; ++str )
		{
			if( 0 == wcsncmp( str, g_wstrPrepareHTML[ HTML_GT ][ 0 ], 1 ) )
			{
				strRet += g_wstrPrepareHTML[ HTML_GT ][ 2 ];
				continue;
			}
			else if( 0 == wcsncmp( str, g_wstrPrepareHTML[ HTML_LT ][ 0 ], 1 ) )
			{
				strRet += g_wstrPrepareHTML[ HTML_LT ][ 2 ];
				continue;
			}
			else if( 0 == wcsncmp( str, g_wstrPrepareHTML[ HTML_SEMICOLON ][ 0 ], 1 ) )
			{
				strRet += g_wstrPrepareHTML[ HTML_SEMICOLON ][ 2 ];
				continue;
			}
			else if( 0 == wcsncmp( str, g_wstrPrepareHTML[ HTML_AMP ][ 0 ], 1 ) )
			{
				strRet += g_wstrPrepareHTML[ HTML_AMP ][ 2 ];
				continue;
			}
			else if( 0 == wcsncmp( str, g_wstrPrepareHTML[ HTML_QUOT ][ 0 ], 1 ) )
			{
				strRet += g_wstrPrepareHTML[ HTML_QUOT ][ 2 ];
				continue;
			}
			else if( 0 == wcsncmp( str, g_wstrPrepareHTML[ HTML_APOS ][ 0 ], 1 ) )
			{
				strRet += g_wstrPrepareHTML[ HTML_APOS ][ 2 ];
				continue;
			}

			strRet += str[ 0 ];
		}

		return strRet;
	}

	static wchar_t* AnsiToUnicode( const char* str )
	{
		wchar_t strTemp[ 1024 ];
		MultiByteToWideChar( CP_ACP, MB_COMPOSITE, str, -1, strTemp, 1024 );

		CStringW strTempW = strTemp;		
		return strTempW.GetBuffer();
	}

	static char* UnicodeToAnsi( const wchar_t* str )
	{
		char strTemp[ 1024 ];
		WideCharToMultiByte( CP_ACP, 0, str, -1, strTemp, 1024, NULL, NULL );

		CString strTempA = strTemp;
		return strTempA.GetBuffer();
	}

	int GetStringLength( const char* str );
	std::string SplitString( const char* str, int nBegin, int nEnd );

	// 플래그 더하기/빼기
	static DWORD FlagAdd( DWORD dwFlag, DWORD dwValue )
	{
		return dwFlag | dwValue;
	}
	static DWORD FlagSub( DWORD dwFlag, DWORD dwValue )
	{
		return dwFlag & ~dwValue;
	}

	// 일반 텍스트를 HTML 컬러 텍스트로 변경
	static std::string ToHtmlColorText( const std::string& strText, DWORD dwColor )
	{
		CString strHtml;
		strHtml.Format( "<font color=\"#%x\">%s</font>", dwColor, strText.c_str() );

		return strHtml.GetBuffer();
	}

	static std::wstring ToHtmlColorText( const std::wstring& strText, DWORD dwColor )
	{
		CStringW strHtml;
		strHtml.Format( L"<font color=\"#%x\">%s</font>", dwColor, strText.c_str() );

		return strHtml.GetBuffer();
	}

	static std::string ToPrepareHTMLColorText(const char* str, DWORD dwColor)
	{
		return ToHtmlColorText( PrepareHTMLString(str), dwColor );
	}

	static std::wstring ToPrepareHTMLColorText(const wchar_t* str, DWORD dwColor)
	{
		return ToHtmlColorText( PrepareHTMLStringW(str), dwColor );
	}


	// 랜덤 컬러
	static DWORD GetRandomColor( bool bRestrict )
	{
		DWORD dwColor = 0;

		if( true == bRestrict )
		{
			DWORD dwRestrictSize = (DWORD) GLCONST_COLORTABLE::dw_COLORSELECTOR_TABLE_RESTRICT.size();        
			dwColor = GLCONST_COLORTABLE::dw_COLORSELECTOR_TABLE_RESTRICT[ rand() % dwRestrictSize ];
		}
		else
		{
			dwColor = D3DCOLOR_ARGB( 255, rand() % 0x100, rand() % 0x100, rand() % 0x100 );
		}

		return dwColor;
	}

	static bool haveSymbolCha( const char* strCha )
	{
		std::wstring strUnicode = AnsiToUnicode( strCha );
		return sc::string::haveSymbolCha( strUnicode );
	}

	static int StringFindFirst( const char* sz, const char* szFind );


	// 스킬 효과 슬롯을 버프 타입으로 변환
	EMSKILL_BUFF_TYPE SkillFactSlotToType( DWORD dwSlot );

	// 아이템 정보
	std::string GetItemInfoHeader( const SITEMCUSTOM& sItemCustom );
	std::string GetItemInfo( const SITEMCUSTOM& sItemCustom, int nType = 0 );
	std::string GetDurabilityInfoHeader(const SITEMCUSTOM& sItemCustom, int nType);
	std::string GetDurabilityInfo(const SITEMCUSTOM& sItemCustom, int nType);
	std::string GenerateItemInfoHeader( WORD wMID, WORD wSID );
	std::string GenerateItemInfo( WORD wMID, WORD wSID );

	// 스킬 정보
	std::string GetTypeText_SkillNextLevel();
	std::string GetSkillTypeText_MultiSkill();
	std::string GetSkillTypeText_LinkSkill();
	std::string GetSkillTypeText_DelaySkill();
	std::string GetSkillTypeText_ActiveSkill();
	std::string GetSkillInfoHeader( const SNATIVEID& sID, WORD wSlotType, int nLevel = -1 );
	std::string GetSkillInfo( const SNATIVEID& sID, WORD wSlotType, int nLevel = -1, bool bConditionCheck = true, int iDisguise = DISGUISE_NONE );
	std::string GenerateSkillInfoHeader( WORD wMID, WORD wSID, int nLevel );
	std::string GenerateSkillInfo( WORD wMID, WORD wSID, int nLevel );
}

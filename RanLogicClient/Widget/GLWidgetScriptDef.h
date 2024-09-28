#pragma once

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Skill/GLSkill.h"
#include "../../RanLogic/GLColorTable.h"

namespace NSWIDGET_SCRIPT
{
	// ����� ��?
	extern bool g_bWidgetDebug;

	//--------------------------------------------------------------------------------
	// ����Ű

	// ä�� ��ũ�� ����Ű �Է�
	// params : char* strText
	static const char* g_strFunc_HotKey_AddChatMacro = "CF_HotKey_AddChatMacro";
	// �κ��丮 â ����
	// params : void
	static const char* g_strFunc_HotKey_Inventory = "CF_HotKey_Inventory";
	// ĳ���� â ����
	// params : void
	static const char* g_strFunc_HotKey_Character = "CF_HotKey_Character";
	// Ż�� â ����
	// params : void
	static const char* g_strFunc_HotKey_Vehicle = "CF_HotKey_Vehicle";
	// ��ų â ����
	// params : void
	static const char* g_strFunc_HotKey_Skill = "CF_HotKey_Skill";
	// ä�� ��ũ�� â ����
	// params : void
	static const char* g_strFunc_HotKey_ChatMacro = "CF_HotKey_ChatMacro";
	// ��ų ������ �׷�
	// params : int nIndex
	static const char* g_strFunc_HotKey_SQSGroup = "CF_HotKey_SQSGroup";
	// ��ų ������ Ʈ����
	// params : void
	static const char* g_strFunc_HotKey_SQSTray = "CF_HotKey_SQSTray";
	// ��Ƽ â ����
	// params : void
	static const char* g_strFunc_HotKey_Party = "CF_HotKey_Party";
	// �� â ����
	// params : void
	static const char* g_strFunc_HotKey_Pet = "CF_HotKey_Pet";
	// ���� â ����
	// params : void
	static const char* g_strFunc_HotKey_Product = "CF_HotKey_Product";
	// ����Ʈ�� â ����
	// params : void
	static const char* g_strFunc_HotKey_PointShop = "CF_HotKey_PointShop";
	// �ý��� �޴�â
	// params : void
	static const char* g_strFunc_HotKey_System = "CF_HotKey_System";
	// �޽��� �ڽ� ����
	// params : void
	static const char* g_strFunc_HotKey_CancelMsgBox = "CF_HotKey_CancelMsgBox";
	// ����Ű Ȯ�� (�޽����ڽ� Ȯ��, ä��â ��Ŀ�� ��)
	// params : void
	static const char* g_strFunc_HotKey_Confirm = "CF_HotKey_Confirm";
	// ��ü��
	// params : void
	static const char* g_strFunc_HotKey_LargeMap = "CF_HotKey_LargeMap";
	// ����UI
	// params : void 
	static const char* g_strFunc_HotKey_Competition = "CF_HotKey_Competition";
	// ����
	// params : void
	static const char* g_strFunc_HotKey_Bank = "CF_HotKey_Bank";
	// Ŭ��â ȣ��
	// params : void
	static const char* g_strFunc_HotKey_Club = "CF_HotKey_Club";
	// Ŀ�´�Ƽ â ����
	// params : void
	static const char* g_strFunc_HotKey_Community = "CF_HotKey_Community";
	// DPS
	// params : void
	static const char* g_strFunc_HotKey_DPS = "CF_HotKey_DPS";
	// �ö��� ī�޶�
	// params : void
	static const char* g_strFunc_HotKey_FlyCamCtrl = "CF_HotKey_FlyCamCtrl";
	// ��Ȱ��Ϻ�
	// params : void
	static const char* g_strFunc_HotKey_StudentRecord = "CF_HotKey_StudentRecord";
	// ���� 
	// params : void
	static const char* g_strFunc_HotKey_Help = "CF_HotKey_Help";
	// ����Ʈ
	// params : void
	static const char* g_strFunc_HotKey_Quest = "CF_HotKey_Quest";
	// ť�ڽ�
	// params : void
	static const char* g_strFunc_HotKey_QBox = "CF_HotKey_QBox";

	// ����Ű ���� �Լ� ��
	//--------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------
	// ��� ȣ�� �Լ� �̸�

	// �������� �ھ� �ʱ�ȭ �Ϸ�
	// params : void
	static const char* g_strFunc_CreateCoreComplete = "CF_CreateCoreComplete";
	// ���� ����̽� (�ھ� �ʱ�ȭ�� �Ϸ�� �ں��͸� ȣ��)
	// params : int nScreenWidth, int nScreenHeight bool bResetUI
	static const char* g_strFunc_ResetDevice = "CF_ResetDevice";
	// ������Ʈ
	// params : float fElapsedTime
	static const char* g_strFunc_FrameMove = "CF_FrameMove";

	// ������ ���� ���
	// params : table tbItemHeader
	static const char* g_strFunc_GetItemTooltipHeader = "CF_GetItemTooltipHeader";
	// ������ ���� ����
	// params : table tbItemData
	static const char* g_strFunc_GetItemTooltip = "CF_GetItemTooltip";
	// ������ ���� ��ȯ ���� ����
	// params : void
	static const char* g_strFunc_GetItemTooltipTrade = "CF_GetItemTooltipTrade";
	// ��ų ���� ���
	// params : table tbSkillHeader
	static const char* g_strFunc_GetSkillTooltipHeader = "CF_GetSkillTooltipHeader";
	// ��ų ���� ����
	// params : table tbSkillData
	static const char* g_strFunc_GetSkillTooltip = "CF_GetSkillTooltip";
	// ���� �߰� ���� ����
	// params : void
	static const char* g_strFunc_GetTootipAddSlotCount = "CF_GetTooltipAddSlotCount";
	// ���� �߰� ���� ���ε�
	// params : void
	static const char* g_strFunc_GetTooltipAddSlotBinding = "CF_GetTooltipAddSlotBinding";
	// ���� ��ȯ ���� ����
	// params : void
	static const char* g_strFunc_GetTooltipTradeSlotCount = "CF_GetTooltipTradeSlotCount";
	// ���� ��ȯ ���� ���ε�
	// params : void
	static const char* g_strFunc_GetTooltipTradeSlotBinding = "CF_GetTooltipTradeSlotBinding";
	// ���� ���̵�
	// params : void
	static const char* g_strFunc_HideTooltipSlot = "CF_HideTooltipSlot";

	// �������� ������ �� ����
	// params : WORD wMID, WORD wSID, string strHeader, string strTootlip
	static const char* g_strFunc_ComparePutOnItemTooltip = "CF_ComparePutOnItemTooltip";

	// �α��� ȭ�� ����
	// params : void
	static const char* g_strFunc_EnterLoginStage = "CF_EnterLoginStage";
	// ĳ���� ���� ȭ�� ����
	// params : void
	static const char* g_strFunc_EnterCharSelectStage = "CF_EnterCharSelectStage";
	// In ���� ����
	// params : void
	static const char* g_strFunc_EnterGameStage = "CF_EnterGameStage";
	// ��� NPC ���� UI �ݱ�
	// params : void
	static const char* g_strFunc_CloseAllNpc = "CF_CloseAllNpc";

	// Ȧ�� ������ ������
	// params : int nHoldItemPlace, WORD wSlotPosX, WORD wSlotPosY, float fPosX, float fPosY, float fPosZ
	static const char* g_strFunc_HoldItemToField = "CF_HoldItemToField";
	// ��ħ ������ �и�
	// params : int nPosX, int nPosY, WORD wItemCount
	static const char* g_strFunc_SeparateOverlapItem = "CF_SeparateOverlapItem";


	static const char* g_strFunc_BasicGetCurrentLogID = "CF_BasicChat_GetCurrentLogID";
	static const char* g_strFunc_BasicGetCurrentChatType = "CF_BasicChat_GetCurrentChatType";
	// ä�� ��ũ �߰�
	// params : table tbLinkData, char* strLinkName
	static const char* g_strFunc_AddChatLink = "CF_AddChatLink";

	// �ߺ� ���� �α���
	// params : void
	static const char* g_strFunc_DuplicateLogin = "CF_DuplicateLogin";
	static const char* g_strFunc_CloseConnection = "CF_CloseConnection";
	// �������� �����ð� ����
	// params : void
	static const char* g_strFunc_LoginGainTime = "CF_LoginGainTime";

	// Ŭ���̾�Ʈ �ջ�.
	// params : void
	static const char* g_strFunc_Login_FailLoadClientVersion = "CF_Login_FailLoadClientVersion";
	// ������ �ٸ�.
	// params : void
	static const char* g_strFunc_Login_DifferentVersion = "CF_Login_DefferentVersion";
	// �α��� ���� �޽���
	// params : int nError
	static const char* g_strFunc_Login_FailLogin = "CF_Login_FailLogin";

	// ĳ���� ���� ����Ʈ �ʱ�ȭ
	// params : void
	static const char* g_strFunc_CharSelect_InitList = "CF_CharSelect_InitSelectCharList";
	// ĳ���� ���� ���� �޽���	
	// params : int nError
	static const char* g_strFunc_CharSelect_JoinFail = "CF_CharSelect_JoinFail";
	// ĳ���� ���� ����
	// params : void
	static const char* g_strFunc_CharSelect_SucceedDelChar = "CF_CharSelect_SucceedDelChar";
	// ĳ���� ���� ����
	// params : bool bClub
	static const char* g_strFunc_CharSelect_FailDelChar = "CF_CharSelect_FailDelChar";
	// ĳ���� ����
	// params : int nCharSelect, int nCharStart
	static const char* g_strFunc_CharSelect_SetSelecrCharIndex = "CF_CharSelect_SetSelectCharIndex";
	// ĳ���� ����Ŭ��
	// params : void
	static const char* g_strFunc_CharSelect_DClickJoin = "CF_CharSelect_DClickJoin";

	// ĳ���� ���� ���
	// params : int nResult
	static const char* g_strFunc_CharCreate_NewCharResult = "CF_CharCreate_NewCharResult";
	
	// ĳ���� �⺻���� ������Ʈ
	// params : void
	static const char* g_strFunc_BasicInfo_Update = "CF_BasicInfo_Update";
	// ���� ����Ʈ ���� ���
	// params : void
	static const char* g_strFunc_Character_ApplyStatPoint = "CF_CharInfo_ApplyStatPoint";
	// ��� ���� ������Ʈ
	// params : void
	static const char* g_strFunc_Character_UpdateSlot = "CF_CharInfo_UpdateSlot";
	// ��� ����
	// params : bool bUseArmSub
	static const char* g_strFunc_Character_ArmSwap = "CF_CharInfo_ArmSwap";
	// �Ҹ�ǰ ������ ���� ������Ʈ
	// params : int nSlot, WORD wTurnNum, int nType
	static const char* g_strFunc_Character_UpdatePutOnCount = "CF_CharInfo_UpdatePutOnCount";
	// �Ҹ�ǰ ��� ����
	// params : int nSlot
	static const char* g_strFunc_Character_RemovePutOnCount = "CF_CharInfo_RemovePutOnCount";

	// ������ ������Ʈ
	// params : LONGLONG llMoney
	static const char* g_strFunc_Inventory_UpdateMoney = "CF_Inventory_UpdateMoney";
	// ����Ʈ�� �Ӵ� ������Ʈ
	// params : LONGLONG llMoney
	static const char* g_strFunc_Inventory_UpdatePoint = "CF_Inventory_UpdatePoint";
	// �κ��丮 ���� ��ü ������Ʈ
	// params : void
	static const char* g_strFunc_Inventory_InvenAllSlotUpdate = "CF_Inventory_InvenAllSlotUpdate";
	// �κ��丮 ���� ������Ʈ
	// params : int nPosX, int nPosY
	static const char* g_strFunc_Inventory_InvenSlotUpdate = "CF_Inventory_InvenSlotUpdate";
	// �κ��丮 ���� ���� Ŭ��
	// params : string strName, int nPosX, int nPosY
	static const char* g_strFunc_Inventory_BankItemClick = "CF_Inventory_BankItemClick";
	// �κ��丮 ���� ��ü ���� ������Ʈ
	// parmas : void
	static const char* g_strFunc_Inventory_BankAllSlotUpdate = "CF_Inventory_BankAllSlotUpdate";
	// �κ��丮 ���� ���� ������Ʈ
	// parmas : int nPosX, int nPosY
	static const char* g_strFunc_Inventory_BankSlotUpdate = "CF_Inventory_BankSlotUpdate";
	// �κ��丮 ���ø���Ʈ ������ ��Ƽ ���� ���̱�
	// params : WORD wMID, WORD wSID, string strHeader, string strInfo
	static const char* g_strFunc_Inventory_WishItemShowTooltip = "CF_Inventory_WishItemShowTooltip";
	// �κ��丮 ���ø���Ʈ ���� ������Ʈ
	// params : int nPosX, int nPosY
	static const char* g_strFunc_Inventory_WishSlotUpdate = "CF_Inventory_WishSlotUpdate";
	// �κ��丮 ���� Ȯ��
	// params : void
	static const char* g_strFunc_Inventory_AddInvenLine = "CF_Inventory_AddInvenLine";
	// �κ��丮 ������ ���
	// params : int nError
	static const char* g_strFunc_InvenTrash_TrashResult = "CF_IvenTrash_TrashResult";
	// �κ��丮 ������ ����
	// params : void
	static const char* g_strFunc_InvenTrash_Open = "CF_InvenTrash_Open";
	// ������ ���ȳ�?
	static const char* g_strFunc_InvenTrash_IsOpen = "CF_InvenTrash_IsOpen";
	// �κ��丮 ���� �����Ѱ�?
	// params : void
	static const char* g_strFunc_IsInvenItemSort = "CF_IsInvenItemSort";
	// ������ �������ΰ�?
	// params : void
	static const char* g_strFunc_Inventory_IsItemDecompose = "CF_Inventory_IsItemDecompose";
	// ������ ���� Ȯ��
	// params : int nPosX, int nPosY
	static const char* g_strFunc_Inventory_ConfirmItemDecompose = "CF_Inventory_ConfirmItemDecompose";

	// ��ų ������ �ֱ� ��� ��ų ID
	// params : DWORD dwID
	static const char* g_strFunc_SQS_LastSkillID = "CF_SQS_LastSkillID";
	// ��ų ������ ������Ʈ
	// params : void
	static const char* g_strFunc_SQS_UpdateSlot = "CF_SQS_UpdateSlot";
	// ��ų ������ ������Ʈ
	// params : WORD wIndex
	static const char* g_strFunc_SQS_UpdateSlotIndex = "CF_SQS_UpdateSlotIndex";
	// ���� ��ų �� �ε���
	// params : void
	static const char* g_strFunc_SQS_CurrentTab = "CF_SQS_CurrentTab";
	// ��ų ������ �Ǵ� ����
	// params : void
	static const char* g_strFunc_SQS_TabSlotCount = "CF_SQS_TabSlotCount";
	// ��ų ������ ���� ���� ����
	// parmas : bool bMain
	static const char* g_strFunc_SQS_ArmSlotSwap = "CF_SQS_ArmSlotSwap";

	// ������ ������ ������Ʈ
	// params : void
	static const char* g_strFunc_IQS_UpdateSlot = "CF_IQS_UpdateSlot";
	// ������ ������ Ʈ���� ������Ʈ
	// params : int nIndex
	static const char* g_strFunc_IQS_UpdateTraySlot = "CF_IQS_UpdateTraySlot";

	// ä�� �޽��� 
	// params : table tbData
	static const char* g_strFunc_BasicChat_ReceiveMsg = "CF_BasicChat_ReceiveMsg";
	// ä�� ���ΰ�ħ
	static const char* g_strFuc_BasicChat_Refresh = "CF_BasicChat_Refresh";
	// �ӼӸ� ����
	// params : int nError, string strName
	static const char* g_strFunc_BasicChat_PrivateFail = "CF_BasicChat_PrivateFail";
	// ä�� ���
	// params : LONGLONG llLeftMinute
	static const char* g_strFunc_BasicChat_Block = "CF_BasicChat_Block";
	// ä�� �α�â�� ���� ĳ���� �̵� ���� ����
	static const char* g_strFunc_BasicChat_MovaleOnChatLog = "CF_BasicChat_MovalbeOnChatBG";
	// ä�� �ӼӸ� ��ȯ
	// params : string strName
	static const char* g_strFunc_BasicChat_SetWhisper = "CF_BasicChat_SetWhisper";
	// ������ ��ũ ��ġ ����
	// params : DWORD dwPos1, DWORD dwPos2
	static const char* g_strFunc_BasicChat_SwapItemLinkPos = "CF_BasicChat_SwapItemLinkPos";
	
	
	// params : table tbData
	static const char* g_strFunc_PrintLinkItemSystemMsg = "CF_PrintMsg_LinkItemSystemMsg";

    //! Ȯ��/������ ����Ʈ ���� ��� : sckim, Redmine #4544
	static const char* g_strFunc_PrintItemLinkAndReferLinkSystemMsg = "CF_PrintMsg_ItemLinkAndReferLinkSystemMsg";
	
	//baischat is focus in?
	static const char* g_strFunc_BasicChat_IsFocusIn = "CF_BasicChat_IsFocusIn";

	// �� �ؽ�Ʈ ����
	static const char* g_strFunc_ShowTip = "CF_TipMsgShow";
	// ������ ��ũ Ŭ��
	// params : int nMID, int nSID, char* strHeader, char* strInfo
	static const char* g_strFunc_ItemLinkInfo_Open = "CF_ItemLinkInfo_Open";
	// ��Ƽ ��ũ Ŭ��
	// params : table tbMemberList, bool bOnce, int nLinkIndex
	static const char* g_strFunc_PartyRecruit_Open = "CF_PartyRecruit_Open";
	// ��Ƽ ��ũ ���� ��û
	// params : DWORD dwGaeaID, int nClass, string strName, DWORD dwHashKey, DWORD dwTimeKey, DWORD dwMasterID
	static const char* g_strFunc_PartyRecruit_RequestRecv = "CF_PartyRecruit_RequestRecv";

	// ĳ���� ��ų ����Ʈ ������Ʈ
	// params : void
	static const char* g_strFunc_Skill_UpdateList = "CF_Skill_UpdateList";
	// ĳ���� ��ų ����Ʈ ������Ʈ
	// params : void
	static const char* g_strFunc_Skill_UpdatePoint = "CF_Skill_UpdatePoint";

	// ��Ƽ ����
	// params : void
	static const char* g_strFunc_Party_New = "CF_Party_New";
	// ��Ƽ ���� ������Ʈ (��Ƽ �ε��� �߰�/����/���� ���� ��)
	// params : void
	static const char* g_strFunc_Party_UpdateSlot = "CF_Party_UpdateSlot";
	// ��Ƽ �ػ�
	// params : void
	static const char* g_strFunc_Party_Dissolve = "CF_Party_Dissolve";
	// ��Ƽ�� ���̵�
	// params : DWORD dwIndex, DWORD dwMapID, DOWR dwGaeaMapID
	static const char* g_strFunc_Party_MembeMoveMap = "CF_Party_MemberMoveMap";
	// ��Ƽ�� ���� ������Ʈ
	// params : DWORD dwIndex, int nPosX, int nPosY, DWORD dwHPMax, DWORD dwHPNow, WORD wLevel
	static const char* g_strFunc_Party_UpdateMember = "CF_Party_UpdateMember";
	// ��Ƽ�� �̸� ����
	// params : DWORD dwIndex, char* strName
	static const char* g_strFunc_Party_MemberRename = "CF_Party_MemberRename";
	// ��Ƽ �ɼ� ����
	// params : int nItemOp, int nMoneyOp, int nItemFilterOp
	static const char* g_strFunc_Party_ChangeOption = "CF_PartyChangeOption";
	// ��Ƽ �ʴ� ����
	static const char* g_strFunc_Party_MsgInvite = "CF_PartyMsgInvite";
	// ��Ƽ�� ���� �߰�
	// params : DWORD dwGaeaID, DWORD dwSkillID, DWORD dwSlot
	static const char* g_strFunc_Party_AddBuff = "CF_Party_AddBuff";
	// ��Ƽ�� ���� ����
	// params : DWORD dwGaeaID, DWORD dwSkillID, DWORD dwSlot
	static const char* g_strFunc_Party_RemoveBuff = "CF_Party_RemoveBuff";
	// ��Ƽ�� ��� ���� ����
	// params : DWORD dwGaeaID
	static const char* g_strFunc_Party_RemoveAllBuff = "CF_Party_RemoveAllBuff";


	// NPC ��ȭ
	// param : void
	static const char* g_strFunc_NpcTalk_Open = "CF_NpcTalk_Open";
	// NPC ��ȭâ ������ �����ִ��� Ȯ��
	// param : void
	static const char* g_strFunc_NpcTalk_IsMarketTabOpen = "CF_NpcTalk_IsMarketTabOpen";
	// NPC ��ȸâ ���� Ȱ���� ������ �ε���
	// param : void
	static const char* g_strFunc_NpcTalk_GetActiveMarket = "CF_NpcTalk_GetActiveMarket";
	// Npc���� ������ �Ǹ�
	// params : bool bSale, bool bConfirm, string strItemName, int nPosX, int nPosY
	static const char* g_strFunc_NpcTalk_SellItem = "CF_NpcTalk_SellItem";
	// Npc ���� ������ ����
	// params : string strName, int nPosX, int nPosY, bool bPile
	static const char* g_strFunc_NpcTalk_BuyItem = "CF_NpcTalk_BuyItem";
	// Npc ���� īƮ�� ������ �߰�
	// params : string strName, WORD wMID, WORD wSID, int nPosX, int nPosY, 
	//			bool bPile, WORD wApplyNum, WORD wPileNum
	static const char* g_strFunc_NpcTalk_AddCart = "CF_NpcTalk_AddCart";
	// Npc ���� īƮ ������ ����
	// params : WORD wIndex
	static const char* g_strFunc_NpcTalk_RemoveCart = "CF_NpcTalk_RemoveCart";
	// Npc ����� �� ������Ʈ
	// params : void
	static const char* g_strFunc_NpcTalk_UpdateRebuy = "CF_NpcTalk_UpdateRebuy";
	// NPC ���� ������ ����
	// params : void
	static const char* g_strFunc_NpcTalk_OpenMarket = "CF_NpcTalk_OpenMarket";
	// Npc Ŭ�� â��
	// params : void
	static const char* g_strFunc_NpcTalk_CreateClub = "CF_NpcTalk_CreateClub";

	// �ý��� �޽��� ���
	// params : char* strMsg, DWORD dwColor
	static const char* g_strFunc_PrintSystemMsg = "CF_PrintMsg_SystemMsg";
	// �ý��� �޽��� �ڽ� ���
	// params : char* strMsg, DWORD dwColor
	static const char* g_strFunc_PrintSystemBox = "CF_PrintMsg_SystemBox";
	// �ý��� �޽��� & �ý��� �޽��� �ڽ� ���
	// params : char* strMsg, DWORD dwColor
	static const char* g_strFunc_PrintSystemMsgBox = "CF_PrintMsg_SystemMsgBox";
	// ä��â �Ϲ� �޽��� �߰�
	// params : char* strMsg, DWORD dwColor
	static const char* g_strFunc_AddString = "CF_PrintMsg_AddString";

	// ��ȣ �ۿ� �޴� ����
	static const char* g_strFunc_OpenInteractMenu = "CF_OpenInteractMenu";
	// ��� ��û UIȣ��
	static const char* g_strFunc_Conft = "CF_Open_ConftUI";
	// �ŷ�â UI ȣ��
	static const char* g_strFunc_Trade = "CF_Open_TradeUI";
	// �ŷ�â �ݱ�
	static const char* g_strFunc_TradeClose = "CF_Close_TradeUI";
	// ���� ��ݹ�ư ���
	static const char* g_strFunc_TradeTarButtonLock = "CF_Trade_SetTarButtonLock";
	// ���� �� �ؽ�Ʈ ����
	static const char* g_strFunc_TradeTarTextMoney = "CF_Trade_SetTarTextMoney";
	// ���� ���� ��ư ���º���
	static const char* g_strFunc_TradeTarButtonAgree = "CF_Trade_SetTarButtonAgree";
	// ���� ������Ʈ
	// params : WORD wPosX, WORD wPosY, bool bMySlot
	static const char* g_strFunc_Trade_UpdateSlot = "CF_Trade_UpdateSlot";
	// �ŷ� ��� �� �κ� ������ ������Ʈ
	// params : WORD wPosX, WORD wPosY, bool bEnable
	static const char* g_strFunc_Trade_UpdateInvenSlot = "CF_Trade_UpdateInvenSlot";
	// �� ��ݹ�ư ���º���
	static const char* g_strFunc_TradeMineButtonLock = "CF_Trade_SetMineButtonLock";
	// �� ���ι�ư ���º���
	static const char* g_strFunc_TradeMineButtonAgree = "CF_Trade_SetMineButtonAgree";
	// �ŷ�â ���� ���� �޽��� �ڽ� ȣ��
	static const char* g_strFunc_TradeTarMessageBox = "CF_Open_Trade_ReqMessageBox";
	// �ŷ�â �����ֳ�?
	static const char* g_strFunc_Trade_IsOpen = "CF_Trade_IsOpen";

	// �׷� ä�� ������� Ȯ��
	// params : int nIndex
	static const char* g_strFunc_GroupChat_IsRoomFree = "CF_GroupChat_IsRoomFree";
	// �׷� ä�� �� ����
	// params : int nIndex, DWORD dwKey, string strOwnerName
	static const char* g_strFunc_GroupChat_CreateRoom = "CF_GroupChat_CreateRoom";
	// �׷� ä�� �� ����
	// params : DWORD dwKey
	static const char* g_strFunc_GroupChat_DeleteRoom = "CF_GroupChat_DeleteRoom";
	// �׷� ä�� Ű ��������
	// params : int nIndex
	static const char* g_strFunc_GroupChat_GetKey = "CF_GroupChat_GetKey";
	// �׷� ä�� �ʴ� ����
	// params : string strName
	static const char* g_strFunc_GroupChat_InviteRecv = "CF_GroupChat_InviteRecv";
	// �׷� ä�� ��� ������Ʈ
	// params : DWORD dwKey, string strName, bool bJoin, bool bPrintMsg
	static const char* g_strFunc_GroupChat_UpdateMember = "CF_GroupChat_UpdateMember";
	// �׷� ä�� �޽��� ����
	// params : DWORD dwKey, string strName, string strMsg, table tbLinkItem
	static const char* g_strFunc_GroupChat_RecvMsg = "CF_GroupChat_RecvMsg";
	// �׷� ä�� Ȱ���� ä�ù� Ű ��������
	// params : void
	static const char* g_strFunc_GroupChat_GetEnableRoomKey = "CF_GroupChat_GetEnableRoomKey";
	// �׷� ä�� ��ũ ������ ������Ʈ
	// params : DWORD dwKey
	static const char* g_strFunc_GroupChat_UpdateLinkItem = "CF_GroupChat_UpdateLinkItem";
	
	// ��������â ����
	// params : void
	static const char* g_strFunc_ReferChar_Open = "CF_ReferChar_Open";
	// ��������â ĳ���� �̸� ����
	// params : string strName
	static const char* g_strFunc_ReferChar_SetName = "CF_ReferChar_SetName";
	// ��������â ĳ���� �̸� ���
	// params : void
	static const char* g_strFunc_ReferChar_GetName = "CF_ReferChar_GetName";

	// �����ʱ�ȭ ����
	// param : void
	static const char* g_strFunc_ProductReset_Open = "CF_ProductReset_Open";
	// ���� ���α׷����� ����
	// param : void
	static const char* g_strFunc_ProductStart = "CF_ProductStart";
	// ���� ��� ����;
	static const char* g_strFunc_ProductOngoingStart = "CF_ProductOngoingStart";
	// ���� ���α׷����� �ݱ�
	// param : void
	static const char* g_strFunc_ProductEnd = "CF_ProductEnd";
	// ���� ���ΰ�ħ
	// params : void
	static const char* g_strFunc_ProductRefresh = "CF_Product_Refresh";
	// ���� Ÿ��	 ������Ʈ
	// params : void
	static const char* g_strFunc_ProductUpdateType = "CF_Product_UpdateType";
	// ���� �Ϸ�
	static const char* g_strFunc_ProductSuccess = "CF_ProductSuccess";
	
	// ��â �ݱ�
	// param : void
	static const char* g_strFunc_Pet_CloseWindow = "CF_Pet_CloseWindow";
	// ��â ����
	// param : void
	static const char* g_strFunc_Pet_OpenWindow = "CF_Pet_OpenWindow";
	// ��â ������Ʈ
	// param : void
	static const char* g_strFunc_Pet_UpdateUI = "CF_Pet_UpdateUI";
	// �� �̸� ����
	// params : void
	static const char* g_strFunc_Pet_Rename = "CF_Pet_Rename";

	// �Ӹ� ��Ÿ�� ���� ī�� ���
	// param : void
	static const char* g_strFunc_StyleHairUse = "CF_StyleHairUse";
	// �Ӹ� ��Ÿ�� ���� ����
	// param : int nError, WORD wHair
	static const char* g_strFunc_StyleHairError = "CF_StyleHairError";

	// �� ��Ÿ�� ���� ī�� ���
	// param : void
	static const char* g_strFunc_StyleFaceUse = "CF_StyleFaceUse";
	// �� ��Ÿ�� ���� ����
	// param : int nError, WORD wFace
	static const char* g_strFunc_StyleFaceError = "CF_StyleFaceError";

	// ���� ���� ī�� ���
	// param : void
	static const char* g_strFunc_StyleGenderUse = "CF_StyleGenderUse";
	// ���� ���� ����
	// param : int nError
	static const char* g_strFunc_StyleGenderError = "CF_StyleGenderError";

	// �� ��Ÿ�� ���� ī�� ���
	// param : void
	static const char* g_strFunc_StylePetUse = "CF_StylePetUse";
	// �� ��Ÿ�� ���� ����
	// param : int nError, WORD wStyle, WORD wColor
	static const char* g_strFunc_StylePetError = "CF_StylePetError";
	
	// ����Ʈ�� īƮ �����۰���
	// params : void
	static const char* g_strFunc_PointShop_UpdateCart = "CF_PointShop_UpdateCart";
	// ����Ʈ�� īƮ ������ �� ����
	// params : void
	static const char* g_strFunc_PointShop_GetCartItemPrice = "CF_PointShop_GetCartItemPrice";
	// ����Ʈ�� UI ����
	// params : void
	static const char* g_strFunc_PointShop_Open = "CF_PointShop_Open";
	// ����Ʈ�� UI �ݱ�
	// params : void
	static const char* g_strFunc_PointShop_Close = "CF_PointShop_Close";
	
	// ���λ��� �˻� ����
	// params : void 
	static const char* g_strFunc_PrivateMarketSearch_Open = "CF_PrivateMarketSearch_Open";
	// ���λ��� �˻� ����Ʈ �ŷ� ������ �Ǹ� ���
	// params : int nResult, WORD wMID, WORD wSID, WORD wPosX, WORD wPosY
	static const char* g_strFunc_PrivateMarketSearch_RegisterItem = "CF_PrivateMarketSearch_RegisterItem";
	// ���λ��� �˻� 
	// params : void
	static const char* g_strFunc_PrivateMarketSearch_IsOpen = "CF_PrivateMarketSearch_IsOpen";
	// ���λ��� �˻� ������ ������Ʈ
	// params : void
	static const char* g_strFunc_PrivateMarketSearch_UpdateSearchPage = "CF_PrivateMarketSearch_UpdateSearchPage";
	// �˻� ���� �ڵ� ��ȯ
	// params : int nError, bool bUpdate
	static const char* g_strFunc_PrivateMarketSearch_Error = "CF_PrivateMarketSearch_Error";
	// ���λ��� �˻� �ŷ����� ������Ʈ
	// params : void
	static const char* g_strFunc_PrivateMarketSearch_UpdateLog = "CF_PrivateMarketSearch_UpdateLog";
	// ���� ���� �˻� ����Ʈ �ŷ� ����Ʈ ������Ʈ
	// params : void
	static const char* g_strFunc_PrivateMarketSearch_UpdatePointTrade = "CF_PrivateMarketSearch_UpdatePointTrade";
	// ���� ���� �˻� �˻� �ִ� ������ ������Ʈ
	// params : int nMaxPage
	static const char* g_strFunc_PrivateMarketSearch_UpdateSearchMaxPage = "CF_PrivateMarketSearch_UpdateSearchMaxPage";
	// ���� ���� �˻� ��ٱ��� �ִ� ������ ������Ʈ
	// params : int nMaxPage
	static const char* g_strFunc_PrivateMarketSearch_UpdateWishMaxPage = "CF_PrivateMarketSearch_UpdateWishMaxPage";

	// ���� �� ����
	// params : string strMapName, string strTexture, bool bInstantMap,
	//			DWORD dwServerID, DWORD dwClubID, string strClubName
	static const char* g_strFunc_Minimap_MoveActiveMap = "CF_Minimap_MoveActiveMap";

	// ������
	// ������ ����;
	static const char* g_strFunc_PostBox_Open = "CF_PostBox_Open";
	// ������ �ݱ�
	// params : void
	static const char* g_strFunc_PostBox_Close = "CF_PoseBox_Close";
	// ������ ���� �ֳ�?
	// params : void
	static const char* CF_PostBox_IsOpen = "CF_PostBox_IsOpen";
	// ���� ���� ��� �ޱ�Ϸ� �޽���;
	static const char* g_strFunc_PostBox_Msg_ReceivePost = "CF_PostBox_ReceivePost";
	// ���� ���� �Ϸ� �޽���;
	static const char* g_strFunc_PostBox_SelectedPostOpen = "CF_PostBox_SelectedPost";
	// ���� ���� ��� ������Ʈ
	static const char* g_strFunc_PostBox_ReceivePageUpdate = "CF_PostBox_ReceivePageUpdate";
	// ���� ��Ȳ ����Ʈ ������Ʈ
	// params : void
	static const char* g_strFunc_PostBox_HistoryPageUpdate = "CF_PostBox_HistoryPageUpdate";
	// ���� ��Ȳ ������ ��� ������Ʈ
	// params : void
	static const char* g_strFunc_PostBox_HistoryPageSendUpdate = "CF_PostBox_HistoryPageSendUpdate";
	// ���� ��Ȳ �ޱ� ��� ������Ʈ
	// params : void
	static const char* g_strFunc_PostBox_HistoryPageRecieveUpdate = "CF_PostBox_HistoryPageRecieveUpdate";
	// �������� ������ �巡�� �Ϸ�;
	static const char* g_strFunc_PostBox_SendPageAttachSlot = "CF_PostBox_SendPageAttachSlot";
	// �������� ���� ����;
	static const char* g_strFunc_PostBox_SendPageResetSlot = "CF_PostBox_SendPageResetSlot";
	// ���� ���� ���� ���� ������Ʈ
	static const char* g_strFunc_PostBox_AcceptPost = "CF_PostBox_AcceptPost";
	// ���� ������ �ޱ� - �κ� ���� ����
	// parmas : int nError
	static const char* g_strFunc_PostBox_ErrMsg = "CF_PostBox_ErrMsg";

	// ���� �ʱ�ȭ ����
	// param : table tbData, tbEvenlyStats, tbReturnItem
	static const char* g_strFunc_StatsReset_Open = "CF_StatsReset_Open";
	// ���� �ʱ�ȭ �����ֳ�?
	// params : void
	static const char* g_strFunc_StatsReset_IsOpen = "CF_StatsReset_IsOpen";
	// ���� �ʱ�ȭ ����
	// parmas : void
	static const char* g_strFunc_StatsReset_ReCalc = "CF_StatsReset_ReCalc";
	// ���� �ʱ�ȭ ���
	// params : int nError, WORD wStatNum, WORD wSkillNum, WORD wItemNum
	static const char* g_strFunc_StatsReset_Result = "CF_StatsReset_Result";

	// ��ü�� ĳ���� �˻� ���
	// params : table tbResult
	static const char* g_strFunc_LargeMap_SearchCharResult = "CF_LargeMap_SearchCharResult";
	// ��ü�� �˻��� ĳ���� ���� ����
	// params : int nLevel, int nSchool, int nClass, DWORD dwGaeaID, bool bParty,
	//			string strName, string strTitle, string strClubName
	static const char* g_strFunc_LargeMap_SearchCharDeatil = "CF_LargeMap_SearchCharDetail";
	// ��ü�� �˻��� ĳ���� ������ ǥ��
	// params : table tbMapPos
	static const char* g_strFunc_LargeMap_SearchCharMapPos = "CF_LargeMap_SearchCharMapPos";
	// ��ü�� ĳ���� �̵� ������ ����
	// params : void
	static const char* g_strFunc_LargeMap_HideMoveIcon = "CF_LargeMap_HideMoveIcon";
	
	// ������ �� ������ ���� �ؽ�Ʈ ���;
	// param������ SetMobDataInTable���� ������ ������ �������� SfLargemap_MakeTooltip_Mob�� �����ϰ� ���ȴ�; 
	// params : table
	static const char* g_strFunc_LargeMap_GetMobTooltip = "CF_LargeMap_GetMobTooltip";

	// ��Ȱ��Ϻ� ����
	// param : void
	static const char* g_strFunc_StudentRecord_Open = "CF_StudentRecord_Open";
	// ��Ȱ��Ϻ� �ݱ�
	// params : void
	static const char* g_strFunc_StudentRecord_Close = "CF_StudentRecord_Close";
	// Ư��Ȱ�� ������ ������Ʈ
	// params void
	static const char* g_strFunc_StudentRecord_Update_Act = "CF_StudentRecord_ActivityPage_Update";
	// Ư��Ȱ�� ���� �޼��� ���;
	static const char* g_strFunc_StudentRecord_ProgressMessege = "CF_StudentRecord_ProgressMessege_Update";
	// �⼮�� ������ ������Ʈ
	// params : void
	static const char* g_strFunc_StudentRecord_Update_Attendance = "CF_StudentRecord_AttendancePage_Update";
	// �⼮�� ���� ������Ʈ
	// params : void
	static const char* g_strFunc_StudentRecord_Update_AttendanceContents = "CF_StudentRecord_AttendancePage_UpdateContents";

	// Ż�� ���� ���� ī�� ���
	// params : string strName
	static const char* g_strFunc_VehicleColor_ItemUse = "CF_VehicleColor_ItemUse";
	// Ż�� ���� ���� UI �����ֳ�?
	// params : void
	static const char* g_strFunc_VehicleColor_IsOpen = "CF_VehicleColor_IsOpen";
	// Ż�� ���� ���� UI ����
	// params : void
	static const char* g_strFunc_VehicleColor_Close = "CF_VechicleColor_Close";
	// �� ���� ���� ī�� ���
	// params : void
	static const char* g_strFunc_PetColor_ItemUse = "CF_PetColor_ItemUse";
	// �ڽ�Ƭ ���� ���� ī�� ���
	// params : bool bRestrict
	static const char* g_strFunc_CostumeColor_ItemUse = "CF_CostumeColor_ItemUse";

	// ���� ���� �˸�(�ؽ���);
	// params : flaot fTime, int nState;
	static const char* g_strFunc_Toggle_NotifyCompetitionTexture = "CF_Toggle_Nofity_Competition_Texture";

	// CDM ���� ��� list clear;
	static const char* g_strFunc_Competition_CDM_Clear_List = "CF_Competition_CDM_Result_Clear_List";
	// CDM �� ���� ä���;
	// params : table Information of Column;
	static const char* g_strFunc_Competition_CDM_Add_List_Item = "CF_Competition_CDM_Result_Add_List_Item";	
	// CDM ���� ��� open;
	static const char* g_strFunc_Competition_CDM_Open = "CF_Competition_CDM_Result_Open";
	// CDM ���� ��� close;
	static const char* g_strFunc_Competition_CDM_Close = "CF_Competition_CDM_Result_Close";
	// CDM ��Ȳ�� ����;
	// param : table Information of Club-Ranking;
	static const char* g_strFunc_Competition_CDM_Status_Board_Update = "CF_Competition_CDM_Status_Board_Update";
	// CDM ��Ȳ�� ����;
	static const char* g_strFunc_Competition_CDM_Status_Board_Close = "CF_Competition_CDM_Status_Board_Close";
	// CDM ���� ����Ʈ
	// params : bool bVisible
	static const char* g_strFunc_CDM_ShowDoublePoint = "CF_CDM_ShowDoublePoint";

	// �̴� ���� : �ֻ���;
	// params : void
	static const char* g_strFunc_MiniGame_Dice_Open = "CF_MiniGame_Dice_Open";
	static const char* g_strFunc_MiniGame_Dice_Close = "CF_MiniGame_Dice_Close";
	static const char* g_strFunc_MiniGame_Dice_Turn_Stage = "CF_MiniGame_Dice_Turn_Stage";

	// Ŭ�� ���� ����
	// params : void
	static const char* g_strFunc_Club_NoticeChange = "CF_Club_NoticeChange";
	// Ŭ�� â�� ������ ����
	// params : DWORD dwChannel, WORD wPosX, WORD wPosY
	static const char* g_strFunc_Club_ItemChange = "CF_Club_StorageItem_Change";
	// Ŭ�� â�� �� ����
	// params : LONGLONG llIncomeMoney, LONGLONG llMoney
	static const char* g_strFunc_Club_UpdateMoney = "CF_Club_Storage_UpdateMoney";
	// Ŭ�� ���� ����
	// params : string strNickName
	static const char* g_strFunc_Club_ChangeNickName = "CF_Club_NickName_Change";
	// Ŭ�� ��� �̸� ����
	// params : void
	static const char* g_strFunc_Club_ChangeGradeName = "CF_Club_GradeName_Change";
	// Ŭ�� ��� ���� ����
	// params : void
	static const char* g_strFunc_Club_ChangeGradeAuth = "CF_Club_SetGrade_Auth";
	// Ŭ�� �ڵ� �߹� ����
	// params : void
	static const char* g_strFunc_Club_AutoBan = "CF_Club_AutoBan";
	// Ŭ�� â��� ����
	// params : void
	static const char* g_strFunc_Club_OpenStorage = "CF_Club_OpenStorage";
	// Ŭ�� ����/���� Ŭ�� ����Ʈ ������Ʈ
	// params : void
	static const char* g_strFunc_Club_UpdateAlliance = "CF_Club_UpdateAlliance";
	// Ŭ�� ��� ����Ʈ ������Ʈ
	// params : void
	static const char* g_strFunc_Club_UpdateMember = "CF_Club_UpdateMember";
	// Ŭ�� ���� ���� �޽���
	// params : string strClubName
	static const char* g_strFunc_Club_AuthorityMemberAsk = "CF_Club_AuthorityMemberAsk";
	// Ŭ�� �α� ������Ʈ
	// params : void
	static const char* g_strFunc_Club_UpdateLog = "CF_Club_UpdateLog";
	// Ŭ�� ���� ���� �޽���
	// params : DWORD dwChiefCharID, string strChiefCharName
	static const char* g_strFunc_Club_AllianceJoinAsk = "CF_Club_AllianceJoinAsk";
	// Ŭ�� ���� ���� �޽���
	// params : DWORD dwClubID, string strClubName, bool bAlliance
	static const char* g_strFunc_Club_ArmisticeAsk = "CF_Club_ArmisticeAsk";
	// Ŭ�� ��Ʋ ��û �޽���
	// params : DWORD dwMasterID, string strClubMaster, DWORD dwBattleTime, bool bAlliance
	static const char* g_strFunc_Club_BattleAsk = "CF_Club_BattleAsk";
	// Ŭ�� �ʴ�(�޴� ��)
	static const char* g_strFunc_Club_JoinAsk = "CF_Ask_Club_Join";
	// Ŭ�� �ػ�
	// params : bool bDissolution
	static const char* g_strFunc_Club_Dissolution = "CF_Club_Dissolution";
	// Ŭ�� ��� ������Ʈ
	// params : void
	static const char* g_strFunc_Club_UpdateRank = "CF_Club_UpdateRank";
	// Ŭ�� �̸� ������Ʈ
	// params : void
	static const char* g_strFunc_Club_UpdateName = "CF_Club_UpdateName";
	// Ŭ�� ���� �� ������Ʈ
	// params : void
	static const char* g_strFunc_Club_UpdateGuidanceMap = "CF_Club_UpdateGuidanceMap";
	// Ŭ�� ���� Ŭ�� ����Ʈ ������Ʈ
	static const char* g_strFunc_Club_UpdateBattleClubList = "CF_Club_UpdateBattleClubList";
	// Ŭ�� ���� Ŭ�� ����Ʈ ������Ʈ
	static const char* g_strFunc_Club_UpdateAllianceClubList = "CF_Club_UpdateAllianceClubList";
	// Ŭ�� ���� ������Ʈ 
	static const char* g_strFunc_Club_UpdateBattleRecords = "CF_Club_UpdateBattleRecords";

	// ĳ���� ��Ȱ ����
	// params : bool bResurrectionSkill, float fResurrectTime
	static const char* g_strFunc_Resurrection_ReceiveInfo = "CF_Resurrection_ReceiveInfo";
	// ��Ȱâ UI ����
	// params : void
	static const char* g_strFunc_Resurrection_Close = "CF_Resurrection_Close";
	
	// ��÷�� ������Ʈ
	// params : int nID
	static const char* g_strFunc_Lotto_WinManUpdate = "CF_Lotto_UpdateWinnerList";
	// ��÷ ��ȣ ������Ʈ
	// params : int nID, table tbNum
	static const char* g_strFunc_Lotto_WinNumUpdate = "CF_Lotto_UpdateWinNum";
	// ȸ�� ���� �ݾ� ������Ʈ
	// params : int nID, LONGLONG llMoney, bool bPrev
	static const char* g_strFunc_Lotto_AccumulateMoney = "CF_Lotto_AccumulateMoney";
	// ���� ���Ÿ���Ʈ ������Ʈ
	// params : int nID, bool bPrev
	static const char* g_strFunc_Lotto_BuyListUpdate = "CF_Lotto_UpdateBuyList";
	// ������ ����
	// params : int nID
	static const char* g_strFunc_Lotto_NextTurn = "CF_Lotto_NextTurn";
	// �ζ� ���� ����
	// params : int nID, int nState
	static const char* g_strFunc_Lotto_ChangeState = "CF_Lotto_ChangeState";

	//����Ʈ ����
	static const char* g_strFunc_Quest_Start = "CF_Quest_Start";
	//����Ʈ ��
	static const char* g_strFunc_Quest_End = "CF_Quest_End";
	//����Ʈ ����(�� ����)
	static const char* g_strFunc_Quest_ProgMobKill = "CF_Quest_ProGMobkill";
	//����Ʈ ����(����Ʈ ������ ����)
	static const char* g_strFunc_Quest_ProgQItem = "CF_Quest_ProGQItem";
	//����Ʈ ����(����Ʈ ������ ���)
	static const char* g_strFunc_Quest_ProgItemUse = "CF_Quest_ProGItemUse";
	//����Ʈ ����(���� ����Ʈ ������ ����)
	static const char* g_strFunc_Quest_ProgItemGrinding = "CF_Quest_ProGItemGrinding";
	//����Ʈ ����(���)
	static const char* g_strFunc_Quest_ProgConfting = "CF_Quest_ProGConfting";
	//����Ʈ ����(��ġ ����)
	static const char* g_strFunc_Quest_ProgReachZone = "CF_Quest_ProGReachZone";
	//����Ʈ ����(��� �ð�)
	static const char* g_strFunc_Quest_ProgTime = "CF_Quest_ProGTime";
	//����Ʈ ����(���� �ð� �ʰ�)
	static const char* g_strFunc_Quest_ProgTimeOver = "CF_Quest_ProGTimeOver";
	//����Ʈ ����(��� ����)
	static const char* g_strFunc_Quest_ProgNonDie = "CF_Quest_ProGNonDie";
	//����Ʈ ����(���� ����)
	static const char* g_strFunc_Quest_ProgLevel = "CF_Quest_ProGLevel";
	//����Ʈ ����(���� ��)
	static const char* g_strFunc_Quest_ProgLevelUp = "CF_Quest_ProGLevelUp";
	//����Ʈ ����(Npc ��ȭ)
	static const char* g_strFunc_Quest_ProgNpcTalk = "CF_Quest_ProGNpcTalk";
	//����Ʈ ����(�� �̵� ����)
	static const char* g_strFunc_Quest_ProgLeaveMap = "CF_Quest_ProGLeaveMap";
	//����Ʈ ����(����Ʈ ������ ����)
	static const char* g_strFunc_Quest_ProgItemInsert = "CF_Quest_ProGItemInsert";
	//����Ʈ ����(����Ʈ ������ ����)
	static const char* g_strFunc_Quest_ProgItemDelete = "CF_Quest_ProGItemDelete";
	//����Ʈ ����(����Ʈ ������ ���� ��ȭ)
	static const char* g_strFunc_Quest_ProgItemCount= "CF_Quest_ProGItemCount";
	//����Ʈ ����(����Ʈ ������ ����)
	static const char* g_strFunc_Quest_ProgItemPickUp = "CF_Quest_ProGItemPickUp";

	// ����Ʈ (UI ) ������Ʈ
	static const char* g_strFunc_Quest_Update = "CF_Quest_Update";


	// ����Ʈ (UI ) Ŭ���� 
	static const char* g_strFunc_Quest_Claer = "CF_Quest_Clear";

	//����Ʈ ����ܰ� ����
	static const char* g_strFunc_Quest_ProgChange = "CF_Quest_ProgChange";
	//����Ʈ �����߿��� ����
	static const char* g_strFunc_Quest_DelProg = "CF_Quest_DelProG";
	//����Ʈ �Ϸῡ�� ����
	static const char* g_strFunc_Quest_DelEnd = "CF_Quest_DelEnd";

	// ���� ī�� UI ����
	// params : -b
	static const char* g_strFunc_RebuildCard_Open = "CF_RebuildCard_Open";
	// ���� ī�� UI �ݱ�
	// params : void
	static const char* g_strFunc_RebuildCard_Close = "CF_RebuildCard_Close";
	// ���� ī�� ������ ���
	// params : WORD wPosX, WORD WPosY, table tbItemInfo
	static const char* g_strFunc_RebuildCard_RegistItem = "CF_RebuildCard_RegistItem";
	// ���� ī�� ���� ���
	// params : WORD wMID, WORD wSID, string strItemName, WORD wFixNum
	static const char* g_strFunc_RebuildCard_RegistStemp = "CF_RebuildCard_RegistStemp";
	// ���� ī�� ������ ���� ����
	// params : void
	static const char* g_strFunc_RebuildCard_RemoveItem = "CF_RebuildCard_RemoveItem";
	// ���� ī�� ������ ���� ����
	// params : void
	static const char* g_strFunc_RebuildCard_RemoveStemp = "CF_RebuildCard_RemoveStemp";
	// ���� ī�� ���� �ɼ� ����
	// params : table tbItemInfo, int nFixedOptionIndex
	static const char* g_strFunc_RebuildCard_Update = "CF_RebuildCard_Update";
	// ���� ī�� ���� ����
	// params : void
	static const char* g_strFunc_Update_FixdOption = "CF_RebuildCard_Update_FixdOption";
	// ���� ī�� ���� üũ����
	// params : void
	static const char* g_strFunc_RebuildCard_StampUncheck = "CF_RebuildCard_StampUncheck";
	// ���� Ȯ�� �� ��� �˾�
	// params : void
	static const char* g_strFunc_RebuildCard_FixPopupUI = "CF_RebuildCard_FixPopupUI";
	// ���� Ÿ�� ����
	// params : bool
	static const char* g_strFunc_RebuildCard_ChangedRebuildType = "CF_RebuildCard_ChangedRebuildType";
	// CTF ���� ������Ʈ
	// params : void
	static const char* g_strFunc_Competition_UpdateCTF = "CF_Competition_UpdateCTF";
	// CDM ��ũ ���� ����
	// param : table
	static const char* g_strFunc_Competition_CDMRankInfo = "CF_Competition_CDMRankInfo";
	// PVP �δ� ���� ������Ʈ
	// params : void
	static const char* g_strFunc_Competition_UpdatePVP = "CF_Competition_UpdatePVP";
	// �ܰ��� �δ� ���� ������Ʈ
	// params : void
	static const char* g_strFunc_Competition_UpdatePVE = "CF_Competition_UpdatePVE";
	// ���� UI : PvE�δ� ���� Ȯ�� ��û
	//params : bool bShow, DWORD dwMapID
	static const char* g_strFunc_Competition_PVE_AckEnternce= "CF_Competition_PVEAckEntrance";
	// ���� UI : PvE�δ� ���� Ȯ�� ��û ���
	//params : -n, -s
	static const char* g_strFunc_Competition_PVE_AckEnternceConfirm = "CF_Competition_PVE_AckEnternceConfirm";
	
	// ��ʸ�Ʈ ����ǥ : ��������
	// params : -l, -l, -dw
	static const char* g_strFunc_Tournament_BettingInfo = "CF_Tournament_BettingInfo";
	// ��ʸ�Ʈ ����ǥ : ���ñ׷� ����
	// params : -dw, -dw, -l, -dw, -f
	static const char* g_strFunc_Tournament_BettingGroupInfo = "CF_Tournament_BettingGroupInfo";

	// Ŀ�´�Ƽâ ģ���� ������Ʈ
	// params : void
	static const char* g_strFunc_Community_FriendTab_Update		= "CF_Community_FriendTab_Update";
	static const char* g_strFunc_RnCommunity_FriendTab_Update	= "CF_RnCommunity_FriendTab_Update";
	static const char* g_strFunc_RnCommunity_BlockTab_Update	= "CF_Community_BlockTab_Update";
	// ģ�� ��û
	// params : string strName, bool bNotify
	static const char* g_strFunc_Community_ReqFriend = "CF_Community_ReqFriend";

	static const char* g_strFunc_Community_AddGroup = "CF_Community_FriendGroupAdd";
	static const char* g_strFunc_Community_RemoveGroup = "CF_Community_FriendGroupDelete";
	static const char* g_strFunc_Community_AddFriend = "CF_Community_FriendAdd";
	static const char* g_strFunc_Community_RemoveFriend = "CF_Community_FriendDelete";
	// Param: -n, -n
	// ���� �׷�, �� �׷� ��ȣ;
	static const char* g_strFunc_Community_FriendSetGroup = "CF_Community_FriendSetGroup";
	static const char* g_strFunc_Community_FriendStateUpdate = "CF_Community_FriendStateUpdate";

	// DPS ������Ʈ
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


	// ������ ����
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

	// �ý� ī�� UI ����
	// params : VOID
	static const char* g_strFunc_TaxiCard_Open = "CF_TaxiCard_Open";
	// ���� ī�� UI �ݱ�
	// params : void
	static const char* g_strFunc_TaxiCard_Close = "CF_TaxiCard_Close";

	// ���� �޼��� �˸�(�޼��� �ڽ�X, ������ �ð� ���� ȭ�� ���ߴܿ� �������� ����);
	// param : -s, -n, -n, -n;
	static const char* g_strFunc_Toggle_Public_Notify_Message = "CF_Toggle_Public_Notify_Message";

	// ItemPreivew
	static const char* g_strFunc_ItemPreview_Open = "CF_ItemPreview_Open";
	static const char* g_strFunc_ItemPreview_IsOpen = "CF_ItemPreview_IsOpen";
	static const char* g_strFunc_ItemPreview_SetSlot = "CF_ItemPreview_SetSlot";

	// ��ų ��� ����
	// params : DWORD dwID
	static const char* g_strFunc_SkillReserve_SetSkill = "CF_SkillReserve_SetSkill";

	// ��ų â �ӽ� ��� �ݾ�
	static const char* g_strFunc_SkillUpMoney = "CF_Skill_GetSkillUpMoney";

	// ��ų â �ӽ� ����Ʈ
	static const char* g_strFunc_SkillUsingPoint = "CF_Skill_GetUsingPoint";

	// Ŀ�� Ÿ�� ���� (NONE)
	// params : void
	static const char* g_strFunc_CursorTargetInfo_SetNone = "CF_CursorTargetInfo_SetNone";
	// Ŀ�� Ÿ�� ���� (PC)
	// params : string strName, WORD wLevel, WORD wSchool, int emClassIndex, int nPartyState, 
	//			DWORD dwPartyID, DWORD dwClubID, string strClubName, bool bPK
	static const char* g_strFunc_CursorTargetInfo_SetPC = "CF_CursorTargetInfo_SetPC";
	// Ŀ�� Ÿ�� ���� (NPC)
	// params : string strName, table tbTalk, bool bQuestStart, bool bQuestStep
	static const char* g_strFunc_CursorTargetInfo_SetNPC = "CF_CursorTargetInfo_SetNPC";
	// Ŀ�� Ÿ�� ���� (Mob)
	// params : string strName, WORD wLevel, bool bNonDmg
	static const char* g_strFunc_CursorTargetInfo_SetMob = "CF_CursorTargetInfo_SetMob";
	// Ŀ�� Ÿ�� HP
	// params : DWORD dwNow, DWORD dwMax
	static const char* g_strFunc_CursorTargetInfo_SetHP = "CF_CursorTargetInfo_SetHP";
	
	// CTField ���� ���â ����
	// param : -n
	static const char* g_strFunc_CTFieldRewardResult_Open = "CF_CTFieldRewardResult_Open";

	// ����Ʈ �̵�â
	// params : table tbMapName
	static const char* g_strFunc_GateOpen = "CF_Gate_Open";
	// ����Ʈ �̵�â ����
	// params : void
	static const char* g_strFunc_GateClose = "CF_Gate_Close";

	// ���� ���
	// params : DWORD dwSlot, int nBuffType, DWORD dwSkillID, WORD wLevel, float fAge, float fLife, bool bExpired
	static const char* g_strFunc_Buff_Reg = "CF_Buff_Reg";
	// ���ö� ���� ��ø Ȯ��
	// params : bool bOverlap, string strPrevSkill, string strCurSkill, WORD wPosX, WORD wPosY
	static const char* g_strFunc_Buff_LunchBoxOverlap = "CF_Buff_LunchBoxOverlap";
	// ���� ���� �Ұ��� ����
	// params : string strSkillName
	static const char* g_strFunc_Buff_ImpossibleDelete = "CF_Buff_ImpossibleDelete";
	// ���� ���� Ȯ��
	// params : DWORD dwSkillID, string strSkillName, string strBinding
	static const char* g_strFunc_Buff_MsgDelete = "CF_Buff_MsgDelete";
	// ���� ���� �Ϸ�
	// params : DWORD dwSlot
	static const char* g_strFunc_Buff_Deleted = "CF_Buff_Deleted";
	// ���ö� ���� ����
	// params : DWORD dwIncreaseRate, DWORD dwTotalRate, DWORD dwAccumCount, DWORD dwMaxCount
	//			string strSkillName, string strDesc, table tbTime
	static const char* g_strFunc_Buff_RecvLunchBox = "CF_Buff_RecvLunchBox";
	// ����ġ ���ö� ���� ��ø Ƚ�� ����
	// params : DWORD dwTotalRate, DWORD dwAccumCount, DWORD dwMaxCount
	static const char* g_strFunc_Buff_SetExpAccume = "CF_Buff_SetExpAccum";

	// ����Ű ���� UI �����ֳ�?
	// params : void
	static const char* g_strFunc_HotKeyConfig_IsOpen = "CF_HotKeyConfig_IsOpen";

	// ť�ڽ� ��ư ��� ����
	// params : bool bEnable
	static const char* g_strFunc_QuickMenu_SetQBoxLock = "CF_QuickMenu_SetQBoxLock";
	// ť�ڽ� ��ư ��� ����
	// params : void
	static const char* g_strFunc_QuickMenu_GetQBoxLock = "CF_QuickMenu_GetQBoxLock";
	// �޸��� ���� ����
	// params : bool bRun
	static const char* g_strFunc_QuickMenu_SetRun = "CF_QuickMenu_SetRun";
	// Ż�� ����
	// prams : bool bEquip, WORD wMID, WORD wSID, int nFuel
	static const char* g_strFunc_QuickMenu_EquipVehicle = "CF_QuickMenu_EquipVehicle";
	// ��Ƽ ������ �й� ��ư Ȱ��ȭ;
	// params: bool - open/close
	static const char* g_strFunc_QuickMenu_OpenPartyTenderNotifyButton = "CF_QuickMenu_OpenPartyTenderNotifyButton";
	// ���� �ν��� ����;
	// params: 
	// 1- ���� ���� ���Ͻ� ���޴��� �˸������� �ν�Ʈ����;
	// 2- ���� �����ִ� �ν�Ʈ��;
	// 3- �ν�Ʈ �Ѿ�;
	static const char* g_strFunc_QuickMenu_SetWeaponBoostInfo = "CF_QuickMenu_SetWeaponBoostInfo";
	// ��ũ�� ���� ������Ʈ;
	static const char* g_strFunc_QuickMenu_MacroStateUpdate = "CF_QuickMenu_MacroStateUpdate";
	// ��ũ�� �ð� ����;
	// param : -f
	static const char* g_strFunc_QuickMenu_SetMacroTime = "CF_QuickMenu_SetMacroTime";
	
	// ��ȯ ��ũ�� ��ư ����
	static const char* g_strFun_ResetReMainButtons = "CF_Mo_ResetReMainButtons";

	// ���޴� UI �������°�?
	// return : bool
	static const char* g_strFunc_QuickMenu_IsMovedUI = "CF_QUICKMENU_ISMOVEDUI";

	// ���޴� UI ���������� ����
	// param : -b
	static const char* g_strFunc_QuickMenu_SetMovedUI = "QuickMenu_SetMovedUI";

	// ������ ä�� ��Ʈ ũ��
	static const char* g_strFunc_BasicChat_GetSize = "CF_BASICCHAT_GETSIZE";
	static const char* g_strFunc_BasicChat_SetSize = "CF_BASICCHAT_SETSIZE";
	
	// ť�ڽ� ����
	// params : int nType, float fTime
	static const char* g_strFunc_QBox_Take = "CF_QBox_Take";
	// ť�ڽ� ����
	// params : void
	static const char* g_strFunc_QBox_Reset = "CF_QBox_Reset";
	// 2�� ��� �Է�
	// params : void
	static const char* g_strFunc_SecondPassword_Input = "CF_SecondPassword_Input";
	// 2�� ��� �ű� ����
	// params : void
	static const char* g_strFunc_SecondPassword_New = "CF_SecondPassword_New";
	// 2�� ��� ���
	// params : int nError
	static const char* g_strFunc_SecondPassword_Result = "CF_SecondPassword_Result";

	// ���� UI ����
	// params : void
	static const char* g_strFunc_BusStation_Open = "CF_BusStation_Open";

	// �ö��� ī�޶� ���� �޽���
	// params : string strError
	static const char* g_strFunc_FlyCamCtrl_MsgError = "CF_FlyCamCtrl_MsgError";
	// �ö��� ī�޶� �̵� �ӵ� ����
	// params : float fVelo
	static const char* g_strFunc_FlyCamCtrl_SetMoveVelo = "CF_FlyCamCtrl_SetMoveVelo";
	// �ö��� ī�޶� ȸ�� �ӵ� ����
	// params : float fVelo
	static const char* g_strFunc_FlyCamCtrl_SetRotateVelo = "CF_FlyCamCtrl_SetRotateVelo";
	// �ö��� ī�޶� Ű������ �߰�
	// params : table tbKeyFrame
	static const char* g_strFunc_FlyCamCtrl_AddKeyFrame = "CF_FlyCamCtrl_AddKeyFrame";
	// �ö��� ī�޶� Ű������ ������ ��������
	// params : void
	static const char* g_strFunc_FlyCamCtrl_GetKeyFramePos = "CF_FlyCamCtrl_GetKeyFramePos";
	// �ö��� ī�޶� ���α׷����� ������Ʈ
	// params : int nCurrentKey
	static const char* g_strFunc_FlyCamCtrl_UpdateProgress = "CF_FlyCamCtrl_UpdateProgress";
	
	// ĳ���� �̸� ����
	// params : void
	static const char* g_strFunc_CharRename = "CF_CharRename";

	// Ŭ�� �̸� ����
	// params : void
	static const char* g_strFunc_ClubRename = "CF_ClubRename";

	// ���λ���
	// ���λ��� �̸����� �޽����ڽ� ����
	static const char* g_strFunc_PrivateMarket_TitleMsgBox_Open = "CF_PrivateMarket_TitleMsgBox_Open";
	static const char* g_strFunc_PrivateMarket_Close = "CF_PrivateMarket_Close";
	static const char* g_strFunc_PrivateMarket_IsOpen = "CF_PrivateMarket_IsOpen";
	static const char* g_strFunc_PrivateMarket_IsOpener = "CF_PrivateMarket_IsOpener";
	static const char* g_strFunc_PrivateMarket_RegItem = "CF_PrivateMarket_RegItem";
	static const char* g_strFunc_PrivateMarket_InsertItem = "CF_PrivateMarket_InsertItem";
	// ���� ���� ������ ������Ʈ
	// params : table tbItemList
	static const char* g_strFunc_PrivateMarket_Update = "CF_PrivateMarket_Update";
	// ���� ���� ��� ���������� Ȯ��
	// params : WORD wPosX, WORD wPosY
	//static const char* g_strFunc_PrivateMarket_IsBlock = "CF_PrivateMarket_IsBlock";

	// ���λ��� ��������
	static const char* g_strFunc_PrivateMarket_MsgTitleFb = "CF_PrivateMarket_MsgTitleFb";
	static const char* g_strFunc_PrivateMarket_MsgOpenFb = "CF_PrivateMarket_MsgOpenFb";
	static const char* g_strFunc_PrivateMarket_OpenFb = "CF_PrivateMarket_OpenFb";

	// Ÿ ĳ���� ���λ��� ����
	// params : DWORD dwID, string strName, string strTitle
	static const char* g_strFunc_PrivateMarket_ReqPMInfo = "CF_PrivateMarket_ReqPMInfo";

	// Ÿ ĳ���� ���λ��� �ݱ�
	static const char* g_strFunc_PrivateMarket_ReqPMClose = "CF_PrivateMarket_CloseWidget";

	// ���λ��� ��ǰ ����
	static const char* g_strFunc_PrivateMarket_ReqBuy = "CF_PrivateMarket_ReqBuy";
	static const char* g_strFunc_PrivateMarket_ReqBuyEx = "CF_PrivateMarket_ReqBuyEx";

	// ��Ŀ �����ֳ�?
	// params : void
	static const char* g_strFunc_Locker_IsOpen = "CF_Locker_IsOpen";
	// ��Ŀ ���� ī�� ��� ����
	// params : WORD wPosX, WORD wPosY
	static const char* g_strFunc_Locker_CanUseOpenCard = "CF_Locker_CanUseOpenCard";
	// ��Ŀ �� ������Ʈ
	// params  : LONGLONG llMoney
	static const char* g_strFunc_Locker_UpdateMoney = "CF_Locker_UpdateMoney";
	// ��Ŀ UI ����
	// params : void
	static const char* g_strFunc_Locker_Open = "CF_Locker_Open";
	// ��Ŀ ������ ������Ʈ
	// params : int nIndex
	static const char* g_strFunc_Locker_UpdateItem = "CF_Locker_UpdateItem";
	// ��Ŀ Ȯ�� ���� 
	// params : WORD wPosX, WORD wPosY (��Ŀ Ȯ�� ī�� �κ� ��ġ)
	static const char* g_strFunc_LockerExpand_Open = "CF_LockerExpand_Open";
	// ��Ŀ ���� ī�� ���
	// params : void
	static const char* g_strFunc_Locker_UseOpenCard = "CF_Locker_UseOpenCard";
	// ��Ŀ Ȯ�� ����, ��Ŀ ������Ʈ
	// params : void
	static const char* g_strFunc_Locker_Update = "CF_Locker_Update";

	// �����̾� ���� ����
	// params : void
	static const char* g_strFunc_UpdatePremium = "CF_UpdatePremium";

	// ������ �ڽ� ���
	// params : WORD wPosX, WORD wPosY, string strItemName
	static const char* g_strFunc_UseItemBox = "CF_UseItemBox";
	// ������ ����Ʈ ī��
	// params : WORD wPosX, WORD wPosY, string strItemName
	static const char* g_strFunc_UseItemPointCard = "CF_UseItemPointCard";

	// ���� ��� ������(�����ɼ��� �ִ�) Ȯ��
	static const char* g_strFunc_ProductNeedConfirm = "CF_ProductNeedConfirm";

	// ��� ���÷��� ���
	// params : int nIndex
	static const char* g_strFunc_ConftDisplay = "CF_ConftDisplay";

	// ������ ���� �޽��� �ڽ�
	// params : void
	static const char* g_strFunc_SetCommission = "CF_SetCommission";
	// NPC ����ġ ȸ��
	// params : DWORD dwNpcGlobalID, LONGLONG llReExp, LONGLONG llDecMoney
	static const char* g_strFunc_RecoveryNpcExp = "CF_RecoveryNpcExp";
	// �� ���� ���� ����ġ
	// params : void
	static const char* g_strFunc_RegenEntryFail = "CF_RegenEntryFail";

	// ���ʽ� Ÿ�� ����
	// params : bool bBusterTime, int nTime, int nRemainTime
	static const char* g_strFunc_BonusTime_StartEvent = "CF_BonusTime_StartEvent";
	// ���ʽ� Ÿ�� ��
	// params : void
	static const char* g_strFunc_BonusTime_EndEvent = "CF_BonusTime_EndEvent";

	// Ż�� ���� ������Ʈ
	// params : int nFuel
	static const char* g_strFunc_Vehicle_UpdateFuel = "CF_Vehicle_UpdateFuel";
	// Ż�� ���� ���ε� ID -> ���� Ÿ��
	// params : string strBinding
	static const char* g_strFunc_Vehicle_Binding2Parts = "CF_Vehicle_Binding2Parts";
	// Ż�� UI ������Ʈ
	// params : void
	static const char* g_strFunc_Vehicle_Update = "CF_Vehicle_Update";
	// Ż�� ����
	// params : void
	static const char* g_strFunc_Vehicle_Equip = "CF_Vehicle_Equip";
	// Ż�� UI ���� �ֳ�?
	// params : void
	static const char* g_strFunc_Vehicle_IsOpen = "CF_Vehicle_IsOpen";
	// Ż�� ��ǰ ���� ���̶���Ʈ
	// params : int nVehicleType, int nPartsType
	static const char* g_strFunc_Vehicle_SlotHighlight = "CF_Vehicle_SlotHighlight";

	// ����� UI ����
	// params : bool bFlag
	static const char* g_strFunc_CTFlag_Open = "CF_CTFlag_Open";
	// ����� Ÿ�̸� ����
	// params : float fTime
	static const char* g_strFunc_CTFlag_SetTime = "CF_CTFlag_SetTime";
	// ����� ��� ������ �̸� ����
	// params : bool bLeft, string strName, DWORD dwColor
	static const char* g_strFunc_CTFlag_SetName = "CF_CTFlag_SetName";
	// ����� ���� ����
	// params : bool bLeft, int nScore, DWORD dwColor
	static const char* g_strFunc_CTFlag_SetScore = "CF_CTFlag_SetScore";
	// ����� ������ ����
	// params : bool bLeft, string strXMLKey
	static const char* g_strFunc_CTFlag_SetIcon = "CF_CTFlag_SetIcon";
	// ����� �̹��� ����
	// params : bool bLeft, string strXMLKey, bool bBlink
	static const char* g_strFunc_CTFlag_SetImage = "CF_CTFlag_SetImage";
	// ����� �� ������Ʈ �ؽ�Ʈ ����
	// params : string strTeamObject, DWORD dwColor
	static const char* g_strFunc_CTFlag_SetObjectText = "CF_CTFlag_SetObjectText";

	// ��Ʈ ������ �̸����� ����
	// params : string strSetItemName, int nWoreCount, int nTotal, table tbItemList
	static const char* g_strFunc_SetItem_Open = "CF_SetItem_Open";

	// ��ų ĵ��;
	// Param : Void
	static const char* g_strFunc_ReservationSkillCancel_All = "SkillReservation_CancelAll";
	// ��ȣ���� UI�� ���� ž�� ��û�� ���� ���
	// Param : str - ��û�� �̸�
	static const char* g_strFunc_Modal_VehicleWith = "CF_Modal_VehicleWith";
	// Ż�Ϳ� ž����;
	// Param : bool - ������/������, bool - �ν���
	static const char* g_strFunc_RideVehicle = "CF_RideVehicle";
	// Ż�Ϳ��� ����;
	// Param : bool - ������/������
	static const char* g_strFunc_GetoffVehicle = "CF_GetoffVehicle";
	// ���� ��û�� �������Լ� �亯�� ��;
	// param1 : bool - ����/����;
	// param2 : dw - ����� ���� ���̵�;
	static const char* g_strFunc_VehicleReqTargetAnswer = "CF_VehicleReqTargetAnswer";
	static const char* g_strFunc_QuickMenuUpdateRideWithState = "CF_QuickMenu_UpdateRideWithState";

	// ���� ���� ������Ʈ
	// params : WORD wContinentID, WORD wCountryID
	static const char* g_strFunc_UpdateSelectCountry = "CF_UpdateSelectCountry";

	// ���÷��� ���� �̹��� ������ ID ���
	// params : DWORD dwCountryID
	static const char* g_strFunc_Display_GetCountryFrameID = "CF_Display_GetCountryFrameID";
	// ���籺�� ���� ������Ʈ
	// params : table tbRankList, int nType
	static const char* g_strFunc_CTI_UpdateRank = "CF_CTI_UpdateRank";
	// ���籺�� �� ���� ������Ʈ
	// params : int nRank, int nScore
	static const char* g_strFunc_CTI_UpdateMyRank = "CF_CTI_UpdateMyRank";
	// ���籺�� ������ ������Ʈ
	// params : WORD wCountryID, string strCountryName
	static const char* g_strFunc_CTI_UpdateVictoriousCountry = "CF_CTI_UpdateVictoriousCountry";
	// ���籺�� ���� ���� ������Ʈ
	// params : int nXMLIndex, int nProgressIndex, int nScore, float fTime
	static const char* g_strFunc_CTI_UpdateCountryProgressInfo = "CF_CTI_UpdateCountryProgressInfo";
	// ���籺�� ������
	// params : bool bVisible, float fTime
	static const char* g_strFunc_CTI_Auth = "CF_CTI_Auth";
	// ���籺�� ���� ���� ���� ����
	// params : int nError
	static const char* g_strFunc_CTI_BattleServerEntranceFail = "CF_CTI_BattleServerEntranceFail";
	// ���籺�� ���� ���� ���� ���
	// params : void
	static const char* g_strFunc_CTI_BattleServerEnterWait = "CF_CTI_BattleServerEnterWait";
	// ���籺�� ��� ����Ʈ Ÿ��Ʋ ����
	// params : table tbData
	static const char* g_strFunc_CTI_SetRewardTitle = "CF_CTI_SetRewardTitle";
	// ���籺�� ��� ����Ʈ ������
	// params : table tbData 
	static const char* g_strFunc_CTI_AddRewardData = "CF_CTI_AddRewardData";
	// ���籺�� ��� UI ����
	// params : int nOutTime
	static const char* g_strFunc_CTI_OpenReward = "CF_CTI_OpenReward";
	// ���� �ʱ�ȭ ī�� ���
	// params : bool bIsSelected, WORD wInvenPosX, WORD wInvenPosY
	static const char* g_strFunc_UseResetCountryCard = "CF_UseResetCountryCard";

	// ���籺�� �̴ϸ� ����ǰ, ������ ��ġ
	// params : table tbList
	static const char* g_strFunc_CTI_MinimapAuthPos = "CF_CTI_MinimapAuthPos";
	// ������ �̴ϸ� ������, ���ӱ� ��ġ
	// params : table tbList
	static const char* g_strFunc_Guidance_MinimapAuthPos = "CF_Guidance_MinimapAuthPos";

	// �ڽ�Ƭ �ɷ�ġ �ο� ��� Ȯ��
	// params : int nType, int nInvenPosX, int nInvenPosY
	static const char* g_strFunc_CostumeStat_Use = "CF_CostumeStat_Use";
	// �ڽ�Ƭ �ɷ�ġ �ο� ���� ����
	// params : int nError
	static const char* g_strFunc_CostumeStat_Error = "CF_CostumeStat_Error";
	// �ڽ�Ƭ �ɷ�ġ ����
	// params : WORD wPosX, WORD wPosY, WORD wHoldPosX, WORD wHoldPosY
	static const char* g_strFunc_CostumeStat_Release = "CF_CostumeStat_Release";

	// ������ ���� UI ����
	// params : void
	static const char* g_strFunc_ItemRepair_Open = "CF_ItemRepair_Open";
	// ������ ���� ������Ʈ
	// params : void
	static const char* g_strFUnc_ItemRepair_Update = "CF_ItemRepair_Update";

	// �ν��� ����
	// params : int nState
	static const char* g_strFunc_BoosterGauge_State = "CF_BoosterGauge_State";
	// �ν��� ���
	// params : bool bRide
	static const char* g_strFunc_BoosterGauge_Learn = "CF_BoosterGauge_Learn";

	// �Ͻð�ȭ �޼����ڽ�
	static const char* g_strFunc_TempEnchant = "CF_TempEnchant";

	// ĳ���� ����â ����(�̸�)
	static const char* g_strFunc_CharInfo_Update = "CF_CharInfo_Update";

	static const char* g_strFunc_OpenMsgBox = "CF_OpenMsgBox";

	// �ν��� ���� ���, ����;
	static const char* g_strFunc_GetItemDurabilityTooltipHeader = "CF_GetDurabilityTooltipHeader";
	static const char* g_strFunc_GetItemDurabilityTooltip = "CF_GetDurabilityTooltipInfo";

	// ��ȯ�� ���� on/off;
	// param : -b, -b;
	// Visible ����, ��� ����;
	static const char* g_strFunc_SetVisibleSummonState = "CF_SetVisibleSummonState";
	// ��ȯ�� ���� ������Ʈ;
	// param : -dwAction;
	static const char* g_strFunc_UpdateSummonAction = "CF_UpdateSummonAction";
	// ��Ƽ �й� ������ ����Ʈ ����;
	static const char* g_strFunc_PDD_Update = "CF_PDD_Update";
	// ��Ƽ �й�� ���õ� UI ��� ����;
	static const char* g_strFunc_PDU_Close = "CF_ClosePartyDistributionUI";

	// �췯 �÷� ���� ī�� ���
	// params : void
	static const char* g_strFunc_UseHairColorCard = "CF_HairColor_Use";

	// ��� �÷� ���� ������ �����ִ°�.
	static const char* g_strFunc_HairColor_IsOpen = "CF_HairColor_IsOpen";

	// ��� �̹��� �ѱ�/����
	// params : bool bShow
	static const char* g_strFunc_ShowWarning = "CF_ShowWarning";

	// ����� ��� Ÿ��Ʋ ���� ����
	// params : table tbTitle 
	static const char* g_strFunc_CTFlagResult_SetTitle = "CF_CTFlagResult_SetTitle";
	// ����� ��� ����Ʈ ���� �߰�
	// params : table tbInfo 
	static const char* g_strFunc_CTFlagResult_AddInfo = "CF_CTFlagResult_AddInfo";
	// ����� ��� ����
	// params : int nOutTime
	static const char* g_strFunc_CTFlagResult_Open = "CF_CTFlagResult_Open";

	// ������ ���â Ÿ��Ʋ
	// params : table tbTitle
	static const char* g_strFunc_GuidanceResult_SetTitle = "CF_GuidanceResult_SetTitle";
	// ������ ��� ����Ʈ ���� �߰�
	// params : table tbInfo
	static const char* g_strFunc_GuidancResult_AddInfo = "CF_GuidanceResult_AddInfo";
	// ������ ��� ����
	// params : int nOutTime
	static const char* g_strFunc_GuidanceResult_Open = "CF_GuidanceResult_Open";

	// �Ⱓ ���� Ȯ�� �޼��� �ڽ�;
	// params : word wHoldPosX, wHoldPosY, wInvenPosX, wInvenPosY
	static const char* g_strFunc_PeriodExConfirm = "CF_PeriodExConfirm";

	// �Ⱓ ���� �õ��� �Ⱓ Ȯ�� �޼��� �ڽ�;
	// params : word wHoldPosX, wHoldPosY, wInvenPosX, wInvenPosY
	static const char* g_strFunc_PeriodExReconfirmOpenMsgBox = "CF_PeriodExReconfirmOpenMsgBox";

	// �п� ���� �޽���
	// params : void
	static const char* g_strFunc_SchoolChangeOK = "CF_SchoolChangeOK";

	// HTML �� ��ġ, ũ��
	// params : void
	static const char* g_strFunc_GetHTMLViewSize = "CF_GetHTMLViewSize";

	// �⼮�� ���� �Ϸ� ���÷��� �ѱ�
	// params : string strInfo
	static const char* g_strFunc_AttendanceComplete_Show = "CF_AttendanceComplete_Show";

	// �δ� ������
	// parmas : int nOutTime
	static const char* g_strFunc_DungeonExit_Show = "CF_DungeonExit_Show";

	// ���� ������ Ǯ��
	// params : string strItemName, WORD wPosX, WORD wPosY
	static const char* g_strFunc_UnwrapItem = "CF_UnwrapItem";

	// �� ��Ų ī�� ���
	// params : DWORD dwItemID, WORD wPosX, WORD wPosY, WORD wHoldPosX, WORD wHoldPosY
	static const char* g_strFunc_UsePetSkinCard = "CF_UsePetSkinCard";

	// �κ��丮 ������ ��ġ ����
	// params : int nPosX1, int nPosY1, int nPosX2, int nPosY2
	static const char* g_strFunc_SwapInvenItemPos = "CF_SwapInvenItemPos";

	// �ڽ�Ƭ ���� ��� �Ⱓ ���� �޽���
	// params : int nPosX, int nPosY, int nHoldPosX, int nHoldPosY
	static const char* g_strFunc_CostumeDisguiseTimeCheck = "CF_CostumeDisguiseTimeCheck";

	// 
	static const char* g_strFunc_SelformBox_SetBoxID = "CF_SelformBox_SetBoxID";

	// ����ġ ĸ�� ������ ��� ���� Ȯ�� �˾�;
	static const char* g_strFunc_UseExpCapsuleItem = "CF_UseExpCapsuleItem";

	// ����ġ ���� ������ ��� ���� Ȯ�� �˾�;
	static const char* g_strFunc_UseExpCompressorItem = "CF_UseExpCompressorItem";

	// ĳ���� ���� Ȯ�� ������ ��� ���� Ȯ�� �˾�;
	static const char* g_strFunc_UseUnlockCharacterSlotItem = "CF_UseUnlockCharacterSlotItem";

	// �κ� ��� �޼��� �ڽ� �ݱ�;
	static const char* g_strFunc_LobbyWaitMsgBoxClose = "CF_LobbyWaitMsgBoxClose";

	// UI TimerMessageBox
	static const char* g_strFunc_UITimerMessageBox = "CF_UITimerMessageBox";

	// �� ����� ���� ���� ���
	static const char* g_strFunc_RanMobile_MsgRecognize = "CF_RanMobile_MsgRecognize";
	// �� ����� ���� ����
	static const char* g_strFunc_RanMobile_MsgInfo = "CF_RanMobile_MsgInfo";
	// �� ����� ���� ����
	static const char* g_strFunc_RanMobile_MsgInfoAll = "CF_RanMobile_MsgInfoAll";
	// �� ����� M����Ʈ ��û ���
	static const char* g_strFunc_RanMobile_MsgGetPoint = "CF_RanMobile_MsgGetPoint";
	// �� ����� �ð� ���� ���
	static const char* g_strFunc_RanMobile_MsgExtendTime = "CF_RanMobile_MsgExtendTime";
	// �� ����� ���� ��ȯ ���
	static const char* g_strFunc_RanMobile_MsgChangeItem = "CF_RanMobile_MsgChangeItem";
	// �� ����� ���� ���� ���
	static const char* g_strFunc_RanMobile_MsgSendJewel = "CF_RanMobile_MsgSendJewel";
	// �� ����� ���� ���� ���
	static const char* g_strFunc_RanMobile_MsgDailyReward = "CF_RanMobile_MsgDailyReward";
	// �� ����� ������ ���
	static const char* g_strFRunc_RanMobile_UseRecognizeItem = "CF_RanMobile_UseRecognizeItem";

	// ĳ���Ͱ� ���ظ� ���� ��� ȣ��
	static const char* g_strFunc_ReceiveDamage_Character = "CF_ReceiveDamage_Character";

	//----------------------------------------------------------------------------------
	// ���÷��� ����Ŭ��

	// �̸� ���÷��� --------------------------------------- 
	extern float g_fDef_Display_ShowNameDistanceRatio;				// �̸� ����� �Ÿ� ����
	extern std::string g_strDef_Display_NameMC;						// �̸� ���÷��� Linkage
	extern std::string g_strDef_Display_NameMC_tfName;				// �̸� �ؽ�Ʈ �ʵ� �ν��Ͻ�
	extern std::string g_strDef_Display_NameMC_mcNameBG;			// �̸� �ؽ�Ʈ ���
	extern std::string g_strDef_Display_NameMC_tfClub;				// Ŭ�� �ؽ�Ʈ �ʵ� �ν��Ͻ�
	extern std::string g_strDef_Display_NameMC_mcClubBG;			// Ŭ�� �ؽ�Ʈ ���
	extern std::string g_strDef_Display_NameMC_mcSchool;			// �п� ������ ����Ŭ�� �ν��Ͻ�
	extern std::string g_strDef_Display_NameMC_mcCountry;			// ���� �̹��� �ν��Ͻ�
	extern std::string g_strDef_Display_NameMC_mcParty;				// ��Ƽ ������ ����Ŭ�� �ν��Ͻ�
	extern std::string g_strDef_Display_NameMC_rtClubIcon;			// Ŭ�� ������ �����ؽ�ó �ν��Ͻ�
	extern std::string g_strDef_Display_NameMC_rtClubIconMC;		// Ŭ�� ������ �����ؽ�ó ����Ŭ�� ID
	extern std::string g_strDef_Display_NameMC_rtClubIconLink;		// Ŭ�� ������ ��Ʈ�� ID
	extern std::string g_strDef_Display_NameMC_barHP;				// HP ��
	extern int g_nDef_Display_NameMC_bgOffsetX;						// �̸� ���÷��� ��� ������ X
	extern int g_nDef_Display_NameMC_bgOffsetY;						// �̸� ���÷��� ��� ������ Y
	extern std::string g_strDef_Display_ItemNameMC;					// ������ �̸� ���÷��� Linkage

	// �п� ������ ������ ID
	extern std::string g_strDef_Display_NameMC_NoSchool;
	extern std::string g_strDef_Display_NameMC_SMSchool;
	extern std::string g_strDef_Display_NameMC_HASchool;
	extern std::string g_strDef_Display_NameMC_BHSchool;

	// ��Ƽ ������ ������ ID
	extern std::string g_strDef_Display_NameMC_NoParty;
	extern std::string g_strDef_Display_NameMC_PartyMaster;
	extern std::string g_strDef_Display_NameMC_PartyTarget;
	extern std::string g_strDef_Display_NameMC_PartyGeneral;

	// ����� ���÷��� ----------------------------------------
	extern std::string g_strDef_Display_HeadChatMC;		// ����� ���÷��� Linkage
	extern std::string g_strDef_Display_HeadChat_tf;	// �ؽ�Ʈ �ʵ� �ν��Ͻ�
	extern std::string g_strDef_Display_HeadChat_bg;	// ��� �ν��Ͻ�
	extern float g_fDef_Display_HeadChatLifeTime;		// ������ �ð�

	// HP ���÷��� --------------------------------------------
	extern std::string g_strDef_Display_HpRedMC;
	extern std::string g_strDef_Display_HpGreenMC;
	extern std::string g_strDef_Display_HpBlueMC;
	extern std::string g_strDef_Display_HpGreenSimpleMC;

	extern std::string g_strDef_Display_Hp_tf;
	extern std::string g_strDef_Display_Hp_bar;

	// ������ ���÷��� -----------------------------------------
	extern std::string g_strDef_Display_DmgMc;			// ������ ����Ŭ��
	extern float g_fDef_Display_DmgLifeTime;			// ������ ���� �ð�
	extern float g_fDef_Display_DmgRandOffsetRange;		// ������ ��ġ ���� ������ ����
	extern float g_fDef_Display_DmgHeadOffsetX;			// ������ ��ġ �Ӹ��κ��� ������ X
	extern float g_fDef_Display_DmgHeadOffsetY;			// ������ ��ġ �Ӹ��κ��� ������ Y

	// ���λ��� ���÷��� ----------------------------------------
	extern std::string g_strDef_Display_PrivateMarket;
	extern std::string g_strDef_Display_PrivateMarket_Button;
	extern std::string g_strDef_Display_PrivateMarket_TextPM;
	extern std::string g_strDef_Display_PrivateMarket_TextName;
	extern std::string g_strDef_Display_PrivateMarket_TextPMName;

	extern int g_nDisplayLimitCount;

	//----------------------------------------------------------------------------------

	extern bool g_bDef_WidgetCLoseToDeleteUseBatch;	// UI ���� ��� �ð� �ϰ� ���� ���?
	extern float g_fDef_WidgetCloseToDeleteTime;	// UI ���� �� ������ �ð�

	extern int g_nDef_SlotTooltipMaxWidth;			// ���� ���� Width �ִ밪
	extern int g_nDef_SlotTooltipMinWidth;			// ���� ���� Width �ּҰ�

	//----------------------------------------------------------------------------------

	extern DWORD g_dwDef_ItemSlotCountColor;		// ������ ���� ���� �÷�
	extern DWORD g_dwDef_ItemSlotMaxCountColor;		// ������ ���� �ִ� ���� �÷�

	//----------------------------------------------------------------------------------
	enum EMMSGBOX_TYPE	// MessageBox.lua�� �ִ� ���� ������� �����ؾ��Ѵ�;
	{
		SFMSG_BOX_TYPE_OK 		= 1,
		SFMSG_BOX_TYPE_CANCEL 	= 2,
		SFMSG_BOX_TYPE_OKCANCEL = 3,
		SFMSG_BOX_TYPE_INPUT 	= 4,
		SFMSG_BOX_TYPE_INFO 	= 5
	};

	// ���� ��ų Ÿ��
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

	// �κ� ������ �����ڵ�
	enum EMINVEN_TRASH
	{
		EMINVEN_TRASH_OK		= 0,	// ����
		EMINVEN_TRASH_NOREGIST	= 1,	// ��ϵ� ������ ����.
		EMINVEN_TRASH_NOINVEN	= 2,	// �κ��� �������� ����.
		EMINVEN_TRASH_MONEY		= 3,	// ���� ������.
		EMINVEN_TRASH_NOITEM	= 4,	// �������� ã�� �� ����.
	};

	// ��� ���÷��� �ε���
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

	// HTML �ۼ��� ���� ���� ���ڿ��� Ư�����ڰ� ���ٸ� HTML �±׷� ����
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

	// �÷��� ���ϱ�/����
	static DWORD FlagAdd( DWORD dwFlag, DWORD dwValue )
	{
		return dwFlag | dwValue;
	}
	static DWORD FlagSub( DWORD dwFlag, DWORD dwValue )
	{
		return dwFlag & ~dwValue;
	}

	// �Ϲ� �ؽ�Ʈ�� HTML �÷� �ؽ�Ʈ�� ����
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


	// ���� �÷�
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


	// ��ų ȿ�� ������ ���� Ÿ������ ��ȯ
	EMSKILL_BUFF_TYPE SkillFactSlotToType( DWORD dwSlot );

	// ������ ����
	std::string GetItemInfoHeader( const SITEMCUSTOM& sItemCustom );
	std::string GetItemInfo( const SITEMCUSTOM& sItemCustom, int nType = 0 );
	std::string GetDurabilityInfoHeader(const SITEMCUSTOM& sItemCustom, int nType);
	std::string GetDurabilityInfo(const SITEMCUSTOM& sItemCustom, int nType);
	std::string GenerateItemInfoHeader( WORD wMID, WORD wSID );
	std::string GenerateItemInfo( WORD wMID, WORD wSID );

	// ��ų ����
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

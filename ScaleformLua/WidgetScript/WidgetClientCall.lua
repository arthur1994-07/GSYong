  
-- 코어 초기화 완료
function CF_CreateCoreComplete()
	
	SetStageEvent( EVENT_KEY_UP, "Stage_KeyEvent", "keyCode;" )
	SetStageEvent( EVENT_KEY_DOWN, "Stage_KeyDownEvent", "keyCode;" )
	
	-- 툴팁 슬롯 ID 등록
	RegSlot( SLOT_TOOLTIP, SLOT_TOOLTIP_ID )
	RegSlot( SLOT_TOOLTIP, SLOT_TOOLTIP_ID_MULTISKILL )
	RegSlot( SLOT_TOOLTIP, SLOT_TOOLTIP_ID_LINKSKILL_0 )
	RegSlot( SLOT_TOOLTIP, SLOT_TOOLTIP_ID_LINKSKILL_1 )
	RegSlot( SLOT_TOOLTIP, SLOT_TOOLTIP_ID_LINKSKILL_2 )
	RegSlot( SLOT_TOOLTIP, SLOT_TOOLTIP_ID_DELAYSKILL )
	RegSlot( SLOT_TOOLTIP, SLOT_TOOLTIP_ID_BOOST )
	RegSlot( SLOT_TOOLTIP, SLOT_TOOLTIP_ID_DISGUISE )
	RegSlot( SLOT_TOOLTIP, SLOT_TOOLTIP_ID_ACTIVE )
	RegSlot( SLOT_TOOLTIP, SLOT_TOOLTIP_COMPARE_ID )

	-- 툴팁 추가 슬롯 ID 등록
	for i = 1, TOOLTIP_ADD_SLOT_COUNT, 1 do
	
		RegSlot( SLOT_ITEMBASE, SLOT_TOOLTIP_ADD_ITEM_ID .. tostring( i ) )
		RegSlot( SLOT_SKILLBASE, SLOT_TOOLTIP_ADD_SKILL_ID .. tostring( i ) )
		
		RegSlot( SLOT_ITEMBASE, SLOT_TOOLTIP_TRADE_ITEM_ID .. tostring( i ) )
		
	end
	
	-- 캐릭터 세부정보 슬롯 등록 
	for i = 0, 18, 1 do
		RegSlot( SLOT_CHARINFO, SLOT_CHARINFO_ID .. tostring( i ) )		
	end
	
	-- 인벤토리 슬롯 등록
	for i = 0, 59, 1 do
		
		RegSlot( SLOT_INVENTORY, SLOT_INVENTORY_ID .. tostring( i ) )
		RegSlot( SLOT_INVENTORY_BANK, SLOT_INVENTORY_BANK_ID .. tostring( i ) )
		RegSlot( SLOT_INVENTORY_WISHLIST, SLOT_INVENTORY_WISHLIST_ID .. tostring( i ) )
		
	end

	-- 인벤토리 휴지통 슬롯 등록
	RegSlot( SLOT_INVENTORY_TRASH, SLOT_INVENTORY_TRASH_ID )
	
	-- 스킬 퀵슬롯 등록
	RegSlot( SLOT_SQS, SLOT_SQS_LAST_ID  )
	for i = 0, SKILL_QUICK_SLOT_COUNT_TOTAL, 1 do
		RegSlot( SLOT_SQS, SLOT_SQS_ID .. tostring( i ) )
	end
	
	-- 아이템 퀵슬롯 등록
	RegSlot( SLOT_IQS, SLOT_IQS_LAST_ID )
	for i = 0, 5, 1 do
		RegSlot( SLOT_IQS, SLOT_IQS_ID .. tostring( i ) )
	end
	
	-- 아이템 링크 정보 슬롯 등록
	RegSlot( SLOT_ITEMLINKINFO, SLOT_ILI_ID )
	
	-- 아이템 링크 정보 추가 슬롯 등록
	for i = 1, ITEMLINKINFO_ADD_SLOT_TOTAL, 1 do
		RegSlot( SLOT_ITEMBASE, SLOT_ILI_ADD_ID .. tostring( i ) )
	end
	
	-- 스킬 슬롯 등록
	for i = 1, SKILL_SLOT_COUNT_TOTAL, 1 do
		RegSlot( SLOT_SKILL, SLOT_SKILL_ID .. tostring( i ) )
	end
	
	-- Npc 판매 아이템 슬롯 등록
	for i = 0, NPC_MARKET_SLOT_COUNT_TOTAL - 1, 1 do
		RegSlot( SLOT_NPCMARKET, SLOT_NPCMARKET_ID .. tostring( i ) )
	end
	
	-- Npc 추천 아이템 슬롯 등록
	for i = 0, NPC_MARKET_RECOMMEND_SLOT_COUNT - 1, 1 do
		RegSlot( SLOT_NPCMARKET, SLOT_NPCMARKET_RECOMMEND_ID .. tostring( i ) )
	end
	
	-- Npc 카트 아이템 슬롯 등록
	for i = 0, NPC_MARKET_CART_SLOT_COUNT - 1, 1 do
		RegSlot( SLOT_NPCCART, SLOT_NPCMARKET_CART_ID .. tostring( i ) )
	end
	
	-- Npc 재매입 아이템 슬롯 등록
	for i = 1, NPC_MARKET_REBUY_SLOT_COUNT, 1 do
		RegSlot( SLOT_ITEMBASE, SLOT_NPCMARKET_REBUY_ID .. tostring( i ) )
	end
	
	-- Npc 아이템 광고 슬롯 등록
	RegSlot( SLOT_ITEMBASE, SLOT_NPC_AD_ID )
	
	-- Pet 슬롯 등록
	for i = 1, 2, 1 do
		RegSlot( SLOT_SKILL, SLOT_PET_ID .. tostring( i ) )
		RegSlot( SLOT_PET, SLOT_PET_ITEM_ID .. tostring( i ) )
	end
	
	-- Pet 이미지 슬롯 등록
	RegSlot( SLOT_ITEMBASE, SLOT_PET_IMAGE_ID )
	
	-- Pet 스킬 슬롯 등록
	for i = 1, PET_SKILL_SLOT_COUNT_TOTAL, 1 do
		RegSlot( SLOT_SKILL, SLOT_PET_SKILL_ID .. tostring( i ) )
	end
	
	-- 탈것 이미지 슬롯 등록
	RegSlot( SLOT_ITEMBASE, SLOT_VEHICLE_IMAGE_ID )
	
	-- 제작 재료 스롯
	for i = 1, 5, 1 do
		RegSlot( SLOT_PRODUCT, SLOT_PRODUCT_MATERIAL_ID .. tostring( i ) )
	end
	
	-- 거래창 슬롯 (상대방 슬롯과 내 슬롯)
	for i = 1, TRADE_SLOT_TOTAL * 2, 1 do
		RegSlot( SLOT_TRADE, SLOT_TRADE_ID .. tostring( i ) )
	end
	
	-- 제작 슬롯
	RegSlot( SLOT_PRODUCT, SLOT_PRODUCT_ID )

	-- PointShop 아이템 슬롯 등록
	for i = 1, POINTSHOP_SLOT_COUNT_TOTAL, 1 do
		RegSlot( SLOT_POINTSHOP, SLOT_POINTSHOP_ITEM_ID .. tostring( i ) )
	end
	
	-- PointShop 카트 슬롯 등록
	for i = 1, POINTSHOP_CART_SLOT_COUNT_TOTAL, 1 do
		RegSlot( SLOT_POINTSHOP_CART, SLOT_POINTSHOP_CART_ID .. tostring( i ) )
	end
	
	-- 개인 상점 검색 아이템 슬롯 등록
	for i = 1, PRIVATE_MARKET_SEARCH_SLOT_TOTAL, 1 do
		RegSlot( SLOT_PRIVATEMARKETSEARCH, SLOT_PMS_SEARCH_ID .. tostring( i ) )
		RegSlot( SLOT_ITEMBASE, SLOT_PMS_CART_ID .. tostring( i ) )
		RegSlot( SLOT_ITEMBASE, SLOT_PMS_POINTRADE_ID .. tostring( i ) )
	end
	
	-- 개인 상점 검색 포인트 거래 아이템 등록 슬롯 등록
	RegSlot( SLOT_PMSPOINTTRADE, SLOT_PMS_POINTTRADE_REG_ID )
	
	-- 우편 슬롯
	for i = 1, POSTBOX_RECIEVEPAGE_ITEM_TOTAL, 1 do
		RegSlot( SLOT_POSTBOX, SLOT_POSTBOX_ID .. tostring( i ) )
	end
	-- 우편 보내기 슬롯
	RegSlot( SLOT_POSTBOX, SLOT_POSTBOX_SEND_ID )
	
	-- 우편 보조슬롯
	RegSlot( SLOT_POSTBOX_SUB, SLOT_POSTBOX_READPAGE_ID )
	RegSlot( SLOT_POSTBOX_SUB, SLOT_POSTBOX_PREVIEWPAGE_ID )
	
	-- 전체맵 몬스터 보상 아이템 슬롯
	for i = 1, LARGEMAP_MOBREWARD_SLOT_TOTAL, 1 do
		RegSlot( SLOT_ITEMBASE, SLOT_LARGEMAP_REWARDITEM_ID .. tostring( i ) )
	end
	
	-- 정보보기창 스킬 슬롯 등록
	for i = 1, SKILL_SLOT_COUNT_TOTAL, 1 do
		RegSlot( SLOT_REFERCHAR_SKILL, SLOT_REFERCHAR_SKILL_ID .. tostring( i ) )
	end
	
	-- 정보보기창 아이템 슬롯 등록
	for i = 0, 17, 1 do
		RegSlot(SLOT_REFERCHAR, SLOT_REFERCHAR_ID .. tostring(i) )
	end
	
	-- 클럽 창고 슬롯
	for index = 1, 144, 1 do
		RegSlot( SLOT_CLUBSTORAGE, SLOT_CLUBSTORAGE_ID .. tostring( index ) )
	end	
	
	-- 도움말 웹 뷰 ID 등록
	RegHtmlWebView( WEB_VIEW_ID_HELP )
	
	-- 출석부 슬롯
	for i = 0, STUDENTRECORD_ATTENDANCE_TASK_TOTAL-1, 1 do
		RegSlot( SLOT_ATTENDANCE_AWARD, SLOT_ATTENDANCE_AWARD_ID .. tostring( i ) )
	end

	-- 특별활동 판매아이템  슬롯
	for i = 1, ACTIVITY_SALES_ITEM_SLOT_MAX, 1 do
		RegSlot( SLOT_ACTIVITY_SALES, SLOT_ACTIVITY_SALES_ID .. tostring( i ) )
	end
	
	-- 미니 게임 보상 아이템 슬롯;
	for i = 0, 4, 1 do
		RegSlot( SLOT_MINIGAME_DICE_REWARD, SLOT_MINIGAME_DICE_REWARD_ID .. tostring( i ) )
	end
	
	-- 부활 UI 광고 슬롯
	RegSlot( SLOT_ITEMBASE, SLOT_RESURRECTION_AD_ID )
	
	-- 개조카드 아이템 슬롯
	RegSlot( SLOT_REBUILD_CARD_ITEM, SLOT_REBUILD_CARD_ITEM_ID )
	
	-- 개조카드 스템프 슬롯
	RegSlot( SLOT_REBUILD_CARD_STEMP, SLOT_REBUILD_CARD_STEMP_ID )
	
	-- 아이템 미리보기 슬롯
	for i = 1, ITEM_PREVIEW_SLOT_MAX, 1 do
		RegSlot( SLOT_ITEM_PREVIEW, SLOT_ITEM_PREVIEW_ID .. tostring( i ) )
	end	
	
	-- 퀘스트 보상 아이템 슬롯
	for i = 1, 8, 1 do
		RegSlot( SLOT_ITEMBASE, SLOT_QUEST_REWORDITEM_ID .. tostring(i))
	end
	
	-- 퀘스트 수집 아이템 슬롯
	for i = 1, 8, 1 do
		RegSlot( SLOT_ITEMBASE, SLOT_QUEST_COLLECTITEM_ID .. tostring(i))
	end
	
	-- 스킬 사용 예약 슬롯
	for i = 1, SKILL_RESERVE_SLOT_TOTAL, 1 do
		RegSlot( SLOT_SKILLBASE, SLOT_SKILL_RESERVE_ID .. tostring( i ) )
	end
	
	-- 일반 버프 슬롯
	for i = 1, BUFF_NORMAL_SLOT_TOTAL, 1 do
		RegSlot( SLOT_BUFF, SLOT_BUFF_NORMAL_ID .. tostring( i ) )
	end
	
	-- 도시락 버프 슬롯
	RegSlot( SLOT_BUFF, SLOT_BUFF_LUNCHBOX_ID )
	-- 경험치 도시락 버프 슬롯
	RegSlot( SLOT_BUFF, SLOT_BUFF_EXP_LUNCHBOX_ID )
	-- 추가 버프 슬롯
	RegSlot( SLOT_BUFF, SLOT_BUFF_ADD_ID )
	
	-- CTF 버프 슬롯(점령전 플레이중 버프)
	for i = 1, BUFF_CTF_SLOT_TOTAL, 1 do
		RegSlot( SLOT_BUFF, SLOT_BUFF_CTF_ID .. tostring( i ) )
	end
	
	-- 시스템 버프 슬롯(점령전 보상 버프)
	for i = 1, BUFF_SYSTEM_SLOT_TOTAL, 1 do
		RegSlot( SLOT_BUFF, SLOT_BUFF_SYSTEM_ID .. tostring( i ) )
	end
	
	-- 개인상점
	for i = 1, PIVATE_MARKET_SLOT_MAX, 1 do
		RegSlot( SLOT_PRIVATE_MARKET, SLOT_PRIVATE_MARKET_ID .. tostring( i ) )
	end
	
	-- 락커
	for i = 1, LOCKER_SLOT_TOTAL, 1 do
		RegSlot( SLOT_LOCKER, SLOT_LOCKER_ID .. tostring( i ) )
	end
	
	-- 탈것 부품 슬롯
	for i = 1, VEHICLE_SLOT_TOTAL, 1 do
		RegSlot( SLOT_VEHICLE, SLOT_VEHICLE_ID .. tostring( i ) )
	end
	
	-- 파티원 버프 슬롯
	for i = 1, PARTY_MAX, 1 do
	
		for j = 1, PARTY_MEMBER_BUFF_SLOT_TOTAL, 1 do
			RegSlot( SLOT_SKILLBASE, Party_MakeBuffSlotID( i - 1, j ) )
		end
	
	end
	
	-- 세트 아이템 슬롯
	for i = 1, SETITEM_SLOT_TOTAL, 1 do
		RegSlot( SLOT_SETITEM, SLOT_SETITEM_ID .. tostring( i ) )
	end
	
	-- CDM 보상 아이템 슬롯
	for i = 0, CDMREWARDITEM_SLOT_TOTAL-1, 1 do
		RegSlot( SLOT_ITEMBASE, SLOT_CDM_REWARDITEM..tostring(i) )
	end
	
	-- 멀티스킬 툴팁 슬롯 ID 등록
	RegSlot( SLOT_SKILLBASE, SLOT_MULTISKILLTOOLTIP_ID )
	-- 지연스킬 툴팁 슬롯 ID 등록
	RegSlot( SLOT_SKILLBASE, SLOT_DELAYSKILLTOOLTIP_ID )
	-- 링크스킬 툴팁 슬롯 ID 등록
	RegSlot( SLOT_SKILLBASE, SLOT_LINKSKILLTOOLTIP_ID )
	-- 스킬창 플러스버튼 툴팁 슬롯 ID 등록
	RegSlot( SLOT_SKILLBASE, SLOT_SKILLLVPLUSTOOLTIP_ID )
	
	-- 비교 툴팁 슬롯
	RegSlot( SLOT_ITEMBASE, SLOT_COMPARETOOLTIP_ID )
	
	-- 아이템 내구도 툴팁 슬롯
	RegSlot( SLOT_ITEMBASE, SLOT_ITEM_DURABILITY_ID )
	
	-- 아이템 스킬링크 툴팁 슬롯
	RegSlot( SLOT_SKILLBASE, SLOT_ITEM_SKILL_LINK_ID )
	
	-- 극강부 무기 슬롯
	RegSlot( SLOT_ITEMBASE, SLOT_EXTREMEWEAPON_ID )
	
	-- 아이템 수리 슬롯
	for i = 1, ITEMREPAIR_SLOT_TOTAL, 1 do
		RegSlot( SLOT_ITEMBASE, SLOT_ITEMREPAIR_ID .. tostring( i ) )
	end
	
	RegSlot( SLOT_SUMMONSTATE, SLOT_SUMMONSTATE_ID )
	
	-- 에뮬레이터면 게임 UI 로드하자
	if true == g_bIsEmulator then
		ToGameStage()
	end
	
	for i = 1, BUFFMINIPARTY_SLOT_TOTAL, 1 do
		RegSlot( SLOT_BUFF, SLOT_BUFFMINIPARTY_ID .. tostring( i ) )
	end
	
	for i = 1, PARTYDISTRIBUTION_SLOT_TOTAL, 1 do
		RegSlot( SLOT_PARTYDISTRIBUTION, SLOT_PARTYDISTRIBUTION_ID .. tostring( i ) )
	end
	
	for i = 1, CTFLAG_REWARD_SLOT_TOTAL, 1 do
		RegSlot( SLOT_ITEMBASE, SLOT_CTFLAG_REWARD .. tostring( i ) )
	end
	
	for i = 1, CDMRESULT_REWARD_SLOT_TOTAL, 1 do
		RegSlot( SLOT_ITEMBASE, SLOT_CDMRESULT_REWARD .. tostring( i ) )
	end
	
	RegSlot( SLOT_ITEMBASE, SLOT_LOTTO_WIN_ITEM_ID )
	
	for i = 1, LOTTO_WINMAN_LIST_SLOT_TOTAL, 1 do
		RegSlot( SLOT_ITEMBASE, SLOT_LOTTO_WIN_LIST_ITEM_ID .. tostring( i ) )
	end
	
	-- 선택형상자
	for i = 1, SELFORMBOX_SLOT_MAX, 1 do
		RegSlot( SLOT_ITEMBASE, SLOT_SELFORMBOX_ID .. tostring( i ) )
	end
end

-- 리셋 디바이스 ( 클라이언트 창 크기 수정, 최소화 후 복구 등 )
function CF_ResetDevice( nScreenWidth, nScreenHeight, bResetUI )
	
	ResetSize_CommandConsole( nScreenWidth )	
	Title_UIPosition( nScreenWidth, nScreenHeight )
	Letterbox_UIPosition( nScreenWidth, nScreenHeight )
	QBox_UIPosition( nScreenWidth )
	ConftDisp_UIPosition( nScreenWidth )
	VirtuarKeyBoard_Pos()
	CharCreate_ToolTipPos()
	CharCreateRotate_UpdatePos()
	BasicChat_UIPosition( nScreenWidth, nScreenHeight )
	EquipConsume_UIPosition()
	ServerChannelSelect_UIPosition()
	PartySlot_UIPosition()
	
	SummonState_CheckWeapon()
	ExtremeWeapon_CheckWeapon()
	
	ResetQuickMenuPostion( bResetUI )
	
end

-- 업데이트
function CF_FrameMove( fElapsedTime )

	FrameMove_Stage( fElapsedTime )

end

-- 로그인 화면 진입
function CF_EnterLoginStage()

	ToServerSelectStage()

end

-- 캐릭터 선택 화면 진입
function CF_EnterCharSelectStage()

	ToCharacterSelectStage()
	
	-- 캐릭터 데이터 수신 완료 대기를 위한 메세지박스 활성화;
	-- 로비스테이지에서 메세지박스는 오직 하나만 활성화될 수 있다;
	OpenMsgBox( MSG_BOX_TYPE_INFO, GetGameInternalText( "WAITSERVER_MESSAGE" ), 
		nil, nil, nil, nil, nil, nil )

end

-- 게임 화면 진입
function CF_EnterGameStage()

	ToGameStage()

end

-- NPC 관련 UI 닫기
function CF_CloseAllNpc()

	CloseAllNpc()

end

-- 아이템 슬롯 툴팁 헤더
-- 아이템 등급, 이름, 컬러, 요구 학원, 클래스, 코스튬 클래스, 최소 레벨, 최대 레벨, 힘, 민첩, 정신력, 체력, 근력
function CF_GetItemTooltipHeader( tbHeader )
	return ItemToolTip_Header( tbHeader )
end

-- 아이템 슬롯 툴팁
function CF_GetItemTooltip( tbItemData )
	return ItemToolTip_Insert( tbItemData )
end

-- 아이템 슬롯 툴팁 교환 상태 텍스트
function CF_GetItemTooltipTrade()

	return g_sItemToolTip_TradeTypeText

end

-- 스킬 슬롯 툴팁 헤더
function CF_GetSkillTooltipHeader( tbHeader )
	return SkillToolTipHeader( tbHeader )
end

-- 스킬 슬롯 툴팁
function CF_GetSkillTooltip( tbSkillData )
	-- 스킬 슬롯에 아이템 추가 슬롯이 출력되지 않도록 하기 위함
	g_nItemTooltip_AddSlotCount = 0
	g_tbItemTooltip_AddSlotItem = {}
	-- 스킬 슬롯에 아이템 교환 슬롯이 출력되지 않도록 하기 위함
	g_nItemTooltip_TradeSlotCount = 0
	g_sItemToolTip_TradeTypeText = ""
	local SkillBodyData = SkillToolTipBody( tbSkillData )
	return SkillBodyData
end

-- 툴팁 추가 슬롯 개수
function CF_GetTooltipAddSlotCount()

	return g_nItemTooltip_AddSlotCount;

end

-- 툴팁 추가 슬롯 바인딩 ID
function CF_GetTooltipAddSlotBinding()

	return SLOT_TOOLTIP_ADD_ITEM_ID
	
end

-- 툴팁 교환 슬롯 개수
function CF_GetTooltipTradeSlotCount()

	return g_nItemTooltip_TradeSlotCount

end

-- 툴팁 교환 슬롯 바인딩 ID
function CF_GetTooltipTradeSlotBinding()

	return SLOT_TOOLTIP_TRADE_ITEM_ID

end

-- 툴팁 하이드
function CF_HideTooltipSlot()

	CloseWidget( WIDGET_ID_COMPARETOOLTIP )
	
end

-- 착용중인 아이템 비교 툴팁
function CF_ComparePutOnItemTooltip( wMID, wSID, strHeader, strTooltip )

	CompareTooltip_SetInfo( wMID, wSID, strHeader, strTooltip )

end

-- 홀드 아이템 버리기
function CF_HoldItemToField( nHoldSlotPlace, wSlotPosX, wSlotPosY, fPosX, fPosY, fPosZ )
	GameMsg_HoldItemToField( nHoldSlotPlace, wSlotPosX, wSlotPosY, fPosX, fPosY, fPosZ )
end

-- 겹침 아이템 분리
function CF_SeparateOverlapItem( nPosX, nPosY, wItemCount )
	GameMsg_SeparateOverlapItem( nPosX, nPosY, wItemCount )
end

function CF_BasicChat_GetCurrentLogID()
	return BasicChat_GetCurrentLogID()
end

function CF_BasicChat_GetCurrentChatType()
	return BasicChat_GetCurrentChatType()
end

function CF_BASICCHAT_GETSIZE()

	return g_nBasicChat_TextSize
end

function CF_BASICCHAT_SETSIZE( nSize )
	g_nBasicChat_TextSize = nSize
end

-- 채팅 링크 추가
function CF_AddChatLink( tbLinkData, strLinkName )

	if true == IsOpenWidget( WIDGET_ID_BASICCHAT ) then
		BasicChat_AddChatLink( tbLinkData, strLinkName )
	end

end

-- 중복 계정 로그인
function CF_DuplicateLogin()
	GameMsg_DuplicateLogin()
end

-- 오프라인 누적 시간 부족
function CF_LoginGainTime()
	GameMsg_LoginGainTime()
end

-- 로그인 클라이언트 손상
function CF_Login_FailLoadClientVersion()
	Login_FailLoadClientVersion()
end

-- 로그인 버전 다름.
function CF_Login_DefferentVersion()
	Login_DefferentVersion()
end

-- 로그인 실패 메시지
function CF_Login_FailLogin( nError )
	Login_FailLogin( nError )
end

-- 캐릭터 리스트 초기화
function CF_CharSelect_InitSelectCharList()
	CharSelect_InitSelectCharList()
end

-- 캐릭터 접속 에러 메시지 
function CF_CharSelect_JoinFail( nError )
	CharSelect_JoinFail( nError )
end

-- 캐릭터 삭제 성공
function CF_CharSelect_SucceedDelChar()
	CharSelect_SucceedDelChar()
end

-- 캐릭터 삭제 실패
function CF_CharSelect_FailDelChar( bClub )
	CharSelect_FailDelChar( bClub )
end

-- 캐릭터 직접 선택
function CF_CharSelect_SetSelectCharIndex( nCharSelect, nCharStart )
	CharSelect_SetSelectCharIndex( nCharSelect, nCharStart )
end

-- 캐릭터 더블클릭으로 접속
function CF_CharSelect_DClickJoin()
	CharSelect_DClickJoin()
end

-- 캐릭터 생성 결과
function CF_CharCreate_NewCharResult( nResult )
	CharCreate_NewCharResult( nResult )
end

-- 캐릭터 기본정보 업데이트
function CF_BasicInfo_Update()
	BasicInfo_UpdateAllInfo()
end

-- 캐릭터 정보 스탯 적용 결과
function CF_CharInfo_ApplyStatPoint()
	CharInfo_ApplyStatPoint()
end

-- 캐릭터 정보 장비 슬롯 업데이트
function CF_CharInfo_UpdateSlot()

	CharInfo_UpdateSlot()
	-- 소모품 착용 여부 확인
	EquipConsume_Check()
	-- 극강부 무기 체크
	ExtremeWeapon_Check()
	-- 아이템 수리 업데이트
	ItemRepair_Update()
	-- 스킬 업데이트
	Skill_UpdateListAndListState()
	-- 마술부 소환수 체크
	SummonState_CheckWeapon()
	-- 극강부 무기 체크
	ExtremeWeapon_CheckWeapon()
	
end

-- 캐릭터 정보 장비 스왑
function CF_CharInfo_ArmSwap( bUseArmSub )

	CharInfo_ArmSwap( bUseArmSub )
	-- 소모품 착용 여부 확인
	EquipConsume_Check()
	-- 극강부 무기 체크
	ExtremeWeapon_Check()
	-- 극강부 무기 체크
	ExtremeWeapon_CheckWeapon()
	
end

-- 캐릭터 슬롯 아이템 개수 업데이트
function CF_CharInfo_UpdatePutOnCount( nSlot, nCount, nType )

	CharInfo_UpdatePutOnCount( nSlot, nCount )
	EquipConsume_Update( nCount, nType )
	
end

-- 캐릭터 장착 소모품 모두 소진
function CF_CharInfo_RemovePutOnCount( nSlot )

	CharInfo_UpdatePutOnCount( nSlot, 0 )
	EquipConsume_Update( 0, -1 )

end

-- 인벤토리 소지금 업데이트
function CF_Inventory_UpdateMoney( nMoney )

	Inventory_UpdateMoney( nMoney )
	NpcTalk_UpdateMoney( nMoney )
	Product_UpdateMoney( nMoney )
	PostBox_UpdateMoney( nMoney )
	Lotto_UpdateMoney()
	
end

-- 인벤토리 포인트샵 머니 업데이트
function CF_Inventory_UpdatePoint( nPoint )

	Inventory_UpdatePoint( nPoint )
	PointShop_SetLabelRanPoint( nPoint )
	Lotto_UpdateMoney()
	
end

-- 인벤토리 슬롯 전체 업데이트
function CF_Inventory_InvenAllSlotUpdate()

	Inventory_InvenAllSlotUpdate()
	IQS_UpdateSlot()
	IQS_UpdateTraySlot( nil )
	
end

-- 인벤토리 슬롯 업데이트
function CF_Inventory_InvenSlotUpdate( nPosX, nPosY )

	Inventory_InvenSlotUpdate( nPosX, nPosY )
	IQS_UpdateSlot()
	IQS_UpdateTraySlot( nil )
	
end

-- 인벤토리 은행 슬롯 클릭
function CF_Inventory_BankItemClick( strName, nPosX, nPosY )
	Inventory_BankItemClick( strName, nPosX, nPosY )
end

-- 인벤토리 은행 전체 슬롯 업데이트
function CF_Inventory_BankAllSlotUpdate()
	Inventory_BankAllSlotUpdate()
end

-- 인벤토리 은행 슬롯 업데이트
function CF_Inventory_BankSlotUpdate( nPosX, nPosY )
	Inventory_BankSlotUpdate( nPosX, nPosY )
end

-- 인벤토리 위시리스트 아이템 툴팁 보이기
function CF_Inventory_WishItemShowTooltip( wMID, wSID, strHeader, strInfo )
	MultiItemToolTipInfo_Open( wMID, wSID, strHeader, strInfo )
end

-- 인벤토리 위시리스트 슬롯 업데이트
function CF_Inventory_WishSlotUpdate( nPosX, nPosY )
	Inventory_WishSlotUpdate( nPosX, nPosY )
end

-- 인벤토리 슬롯 확장
function CF_Inventory_AddInvenLine()
	Inventory_AddInvenLine()
end

-- 아이템 버리기 서버 요청 결과
function CF_IvenTrash_TrashResult( nError )
	IvenTrash_TrashResult( nError )
end

-- 휴지통 열기
function CF_InvenTrash_Open()
	OpenWidget( WIDGET_ID_INVENTORY_TRASH )
end

-- 휴지통 열렸나?
function CF_InvenTrash_IsOpen()
	return IsOpenWidget( WIDGET_ID_INVENTORY_TRASH )
end

-- 인벤토리 정렬 가능한가?
function CF_IsInvenItemSort()
	return IsInvenItemSort()
end

-- 아이템 분해중인가?
function CF_Inventory_IsItemDecompose()
	return Inventory_IsItemDecompose()
end

-- 아이템 분해 확인
function CF_Inventory_ConfirmItemDecompose( nPosX, nPosY )
	Inventory_ConfirmItemDecompose( nPosX, nPosY )
end

-- 스킬 퀵슬롯 최근 사용한 스킬
function CF_SQS_LastSkillID( nID )
	SQS_LastSkillID( nID )
end

-- 스킬 퀵슬롯 슬롯 업데이트
function CF_SQS_UpdateSlot()
	SQS_UpdateSlot()
end

-- 스킬 퀵슬롯 슬롯 업데이트
function CF_SQS_UpdateSlotIndex( nIndex )
	SQS_UpdateSlotIndex( nIndex )
end

-- 현재 스킬 탭 인덱스
function CF_SQS_CurrentTab()
	return SQS_CurrentTab()
end

-- 스킬 퀵슬롯 탭당 개수
function CF_SQS_TabSlotCount()
	return SQS_TabSlotCount()
end

-- 스킬 퀵슬롯 무기 슬롯 스왑
function CF_SQS_ArmSlotSwap( bMain )
	SQS_ArmSlotSwap( bMain )
end

-- 아이템 퀵슬롯 슬롯 업데이트
function CF_IQS_UpdateSlot()
	IQS_UpdateSlot()
end

-- 아이템 퀵슬롯 트레이 슬롯 업데이트
function CF_IQS_UpdateTraySlot( nIndex )
	IQS_UpdateTraySlot( nIndex )
end

-- 채팅 메시지 수신
-- tbData = 
-- {
--		nType : 채팅 타입
--		strName : 메시지 보낸 사람
--		strMsg : 채팅 메시지
--		bSelf : 내가 보낸 메시지인가?
--		tbLinkDataList : 링크 정보 테이블
--		{
--			!!! nStart, nEnd 값은 메시지 앞에 메시지 보낸 사람 이름을 추가하기 때문에 텍스트 길이만큼 밀어줘야한다.
--			{
--				nLinkTypeIndex : 링크 타입
--				nStart : 링크 텍스트 시작 위치
--				nEnd : 링크 텍스트 끝 위치
--				dwColor : 링크 컬러
--			}
--			...
--		}
-- }

-- 베이직쳇 포커스 검사.
function CF_BasicChat_IsFocusIn()
	return BasicChat_IsFocusIn()
end

function CF_BasicChat_ReceiveMsg( tbData )
	BasicChat_ReceiveMsg( tbData )
end

-- 귓속말 실패
function CF_BasicChat_PrivateFail( nError, strName )
	BasicChat_PrivateFail( nError, strName )
end

-- 채팅 갱신
function CF_BasicChat_Refresh()
	BasicChat_Refresh();
end

-- 채팅 블락
function CF_BasicChat_Block( llLeftMinute )
	BasicChat_Block( llLeftMinute )
end

function CF_BasicChat_MovalbeOnChatBG( bEnable )
	BasicChat_MovalbeOnChatBG( bEnable )
end

-- 채팅 귓속말 전환
function CF_BasicChat_SetWhisper( strName )
	BasicChat_SetWhisper( strName )
end

-- 아이템 링크 위치 변경
function CF_BasicChat_SwapItemLinkPos( dwPos1, dwPos2 )
	BasicChat_SwapItemLinkPos( dwPos1, dwPos2 )
end

-- 아이템 링크 클릭
function CF_ItemLinkInfo_Open( nMID, nSID, nRealMID, nRealSID, strHeader, strInfo )
	ItemLinkInfo_Open( nMID, nSID, nRealMID, nRealSID, strHeader, strInfo )
end

-- 파티 링크 클릭
function CF_PartyRecruit_Open( tbMemberList, bOnce, nLinkIndex )
	PartyRecruit_Open( tbMemberList, bOnce, nLinkIndex )
end

-- 파티 링크 지원 요청 받음
function CF_PartyRecruit_RequestRecv( dwGaeaID, nClass, strName, dwHashKey, dwTimeKey, dwMasterID )
	PartyRecruit_RequestRecv( dwGaeaID, nClass, strName, dwHashKey, dwTimeKey, dwMasterID )
end

-- 캐릭터 스킬 리스트 업데이트
function CF_Skill_UpdateList()
	Skill_UpdateAllList()
end

-- 캐릭터 스킬 포인트 업데이트
function CF_Skill_UpdatePoint()
	Skill_UpdatePoint()
end

-- 파티 생성
function CF_Party_New()

	Party_UpdateList()
	PartySlot_NewParty()
	PartyInfo_UpdateList()
	
	Competition_UpdatePVE()
	
	CF_QuickMenu_Party( true )
end

-- 파티 슬롯 업데이트 
function CF_Party_UpdateSlot()

	Party_UpdateList()
	PartySlot_UpdateList()
	PartyInfo_UpdateList()
	
end

-- 파티 해산
function CF_Party_Dissolve()

	Party_Dissolve()
	PartySlot_Dissolve()
	PartyInfo_Dissolve()
	
	Competition_UpdatePVE()
	
	CF_QuickMenu_Party( false )
end

-- 파티원 맵이동
function CF_Party_MemberMoveMap( dwIndex, dwMapID, dwGaeaMapID )

	Party_MemberMoveMap( dwIndex, dwMapID, dwGaeaMapID )
	PartySlot_MemberMoveMap( dwIndex )
	PartyInfo_MemberMoveMap( dwIndex )
	
	LargeMap_PartyIconAddOrRemove( dwIndex )
	
end
	
-- 파티원 정보 업데이트
function CF_Party_UpdateMember( dwIndex, nPosX, nPosY, dwHPMax, dwHPNow, wLevel, tbPos )

	Party_UpdateMember( dwIndex, nPosX, nPosY, dwHPMax, dwHPNow, wLevel, tbPos )
	PartySlot_UpdateMember( dwIndex )
	PartyInfo_UpdateMember( dwIndex )
	
	Competition_UpdatePVE()
	LargeMap_UpdatePartyIcon( dwIndex )
	
end

-- 파티원 이름 변경
function CF_Party_MemberRename( dwIndex, strName )

	Party_MemberRename( dwIndex, strName )
	PartySlot_MemberRename( dwIndex )
	PartyInfo_MemberRename( dwIndex )
	
end

-- 파티 옵션 변경
function CF_PartyChangeOption( nItemOp, nMoneyOp, nItemFilterOp )
	PartyInfo_ChangeOption( nItemOp, nMoneyOp, nItemFilterOp )
end

-- 파티 초대 수신(원용)
function CF_PartyMsgInvite( strMasterName, dwPartyID, dwHashKey, dwMasterID, dwTimeKey, bExpedition, strPartyInfo )
	local tbPartyInfor = {}
	tbPartyInfor["dwPartyID"] = dwPartyID
	tbPartyInfor["dwHashKey"] = dwHashKey
	tbPartyInfor["dwMasterID"] = dwMasterID
	tbPartyInfor["dwTimeKey"] = dwTimeKey
	tbPartyInfor["strPartyInfo"] = strPartyInfo
	
	Insert_Invitation_Table(INVITATION_REPORT_PARTY, strMasterName, tbPartyInfor)
	
	Report_MessegeBox_Invitation(g_ReportPoint)
	
	--g_PartyMan:ReqPartyInviteAns( true, dwPartyID, dwHashKey, dwMasterID, dwTimeKey )

end

-- 파티원 버프 추가
function CF_Party_AddBuff( dwGaeaID, dwSkillID, dwSlot )
	Party_AddBuff( dwGaeaID, dwSkillID, dwSlot )
end

-- 파티원 버프 삭제
function CF_Party_RemoveBuff( dwGaeaID, dwSkillID, dwSlot )
	Party_RemoveBuff( dwGaeaID, dwSkillID, dwSlot )
end

-- 파티원 모든 버프 삭제
function CF_Party_RemoveAllBuff( dwGaeaID )
	Party_RemoveAllBuff( dwGaeaID )
end

-- NPC 대화
function CF_NpcTalk_Open( bOpen )
	NpcTalk_OpenTalk( bOpen )
end

-- Npc 대화창 열려있는지 확인
function CF_NpcTalk_IsMarketTabOpen()
	return NpcTalk_IsMarketTabOpen()
end

-- Npc 대화창 활성된 마켓탭 인덱스
function CF_NpcTalk_GetActiveMarket()
	return NpcTalk_GetActiveMarket()
end

-- NPC에게 아이템 팔기
function CF_NpcTalk_SellItem( bSale, bConfirm, strItemName, nPosX, nPosY )
	NpcTalk_SellItem( bSale, bConfirm, strItemName, nPosX, nPosY )
end

-- NPC 아이템 구매
function CF_NpcTalk_BuyItem( strItemName, nPosX, nPosY, bPile )
	NpcTalk_BuyItem( strItemName, nPosX, nPosY, bPile )
end

-- Npc 마켓 카트에 아이템 추가
function CF_NpcTalk_AddCart( strItemName, wMID, wSID, wPosX, wPosY, bPile, wApplyNum, wPileNum )
	NpcTalk_AddCart( strItemName, wMID, wSID, wPosX, wPosY, bPile, wApplyNum, wPileNum )
end

-- Npc 마켓 카트 아이템 제거
function CF_NpcTalk_RemoveCart( wIndex )
	NpcTalk_RemoveCart( wIndex )
end

-- Npc 마켓 재매입 탭 업데이트
function CF_NpcTalk_UpdateRebuy()
	NpcTalk_UpdateRebuyTab()
	NpcTalk_EnableRebuyBtn( true )
end

-- Npc 마켓 탭 열기
function CF_NpcTalk_OpenMarket()
	NpcTalk_OpenMarket()
end

-- Npc 클럽 창설
function CF_NpcTalk_CreateClub()
	NpcTalk_CreateClub()
end

function CF_TipMsgShow(bShow)
	TipMsg_Show(bShow)
end

-- 채팅창 시스템 메시지 출력
function CF_PrintMsg_SystemMsg( strText, nColor )
	BasicChat_PrintSystemMsg( CHAT_SYSTEM, strText, nColor )
end

-- 시스템 아이템링크 메세지
function CF_PrintMsg_LinkItemSystemMsg( tbData )
	BasicChat_PrintItemLinkSystemMsg( tbData )
end

-- 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
-- 시스템 Refer Link And Item Link message
function CF_PrintMsg_ItemLinkAndReferLinkSystemMsg( tbData )
	BasicChat_PrintItemLinkAndReferLinkSystemMsg( tbData )
end

-- 시스템 메시지 UI 출력
function CF_PrintMsg_SystemBox( strText, nColor )
	SystemMsg_PrintMsg( strText, nColor )
end

-- 채팅창 & 시스템 메시지 UI 출력
function CF_PrintMsg_SystemMsgBox( strText, nColor )
	BasicChat_PrintSystemMsg( CHAT_SYSTEM, strText, nColor )
	SystemMsg_PrintMsg( strText, nColor )
end

-- 채팅창 일반 메시지 추가
function CF_PrintMsg_AddString( strText, nColor )
	BasicChat_PrintSystemMsg( CHAT_NORMAL, strText, nColor )
end

-- 메세지 박스 출력;
function CF_OpenMsgBox(nMsgBoxType, strText)
	OpenMsgBox( nMsgBoxType, strText, Event_MsgBox_Close )
end

 -- 상호작용 메뉴 오픈
function CF_OpenInteractMenu( strName )

	InteractMenu_Open( strName )
	
end

--대련 신청 UI 
function CF_Open_ConftUI( TargetName_Str, Table_sOption )

	g_strInteractMenu_Name = TargetName_Str
	OPEN_ConftUI( Table_sOption )
	
end

-- 거래 승낙여부 메시지박스 호출
function CF_Open_Trade_ReqMessageBox( strCharName )
	Trade_RecvMsg( strCharName )	
end

-- 거래창 UI
function CF_Open_TradeUI()
	Open_Trade_UI()
end

-- 거래창 잠금 상대방 잠금버튼상태 변경
function CF_Trade_SetTarButtonLock(bLock)
	SetTarButtonLock(bLock)
end

-- 거래창 상대방 승인 버튼 상태 변경
function CF_Trade_SetTarButtonAgree(bAgree)
	SetTarButtonAgree(bAgree)
end

-- 거래창 상대방 돈 텍스트 변경
function CF_Trade_SetTarTextMoney(Tar_Money)
	SetTarTextMoney(Tar_Money)
end

-- 거래 슬롯 업데이트
function CF_Trade_UpdateSlot( nPosX, nPosY, bMySlot )
	Trade_UpdateSlot( nPosX, nPosY, bMySlot )
end

-- 거래 등록된 아이템 인벤토리 비활성
function CF_Trade_UpdateInvenSlot( nPosX, nPosY, bEnable )
	Trade_UpdateInvenSlot( nPosX, nPosY, bEnable )
end

-- 거래창 내 잠금 버튼 상태변경
function CF_Trade_SetMineButtonLock(bLock)
	SetMineButtonLock(bLock)
end

-- 거래창 내 승인 버튼 상태변경
function CF_Trade_SetMineButtonAgree(bAgree)
	SetMineButtonAgree(bAgree)
end

-- 거래창 닫기
function CF_Close_TradeUI()

	if true == IsOpenWidget(WIDGET_ID_TRADE) then
	
		Event_MsgBox_Close()
		CloseWidget(WIDGET_ID_TRADE)
		
	end
	
end

-- 거래창 열려있나?
function CF_Trade_IsOpen()
	return IsOpenWidget( WIDGET_ID_TRADE )
end

-- 그룹 채팅 빈방인가?
function CF_GroupChat_IsRoomFree( nIndex )
	return GroupChat_IsRoomFree( nIndex )
end

-- 그룹 채팅 방 개설
function CF_GroupChat_CreateRoom( nIndex, nKey, strOwnerName )
	GroupChat_CreateRoom( nIndex, nKey, strOwnerName )
end

-- 그룹 채팅 방 삭제
function CF_GroupChat_DeleteRoom( nKey )
	GroupChat_DeleteRoom( nKey )
end

-- 그룹 채팅 키 가져오기
function CF_GroupChat_GetKey( nIndex )
	return GroupChat_GetKey( nIndex )
end

-- 그룹 채팅 초대 수신
function CF_GroupChat_InviteRecv( strName )

	-- 알림창 테이블에 정보값 등록 및 메세지박스 출력
	Insert_Invitation_Table( INVITATION_REPORT_GROUPCHAT, strName, strName )
	Report_MessegeBox_Invitation( g_ReportPoint )
	
end

-- 그룹 채팅 멤버 업데이트
function CF_GroupChat_UpdateMember( nKey, strName, bJoin, bPrintMsg )
	GroupChat_UpdateMember( nKey, strName, bJoin, bPrintMsg )
end

-- 그립 채팅 메시지 수신
function CF_GroupChat_RecvMsg( nKey, strName, strMsg, tbLinkItem )
	GroupChat_RecvMsg( nKey, strName, strMsg, tbLinkItem )
end

-- 그룹 채팅 활성된 채팅방 키 가져오기
function CF_GroupChat_GetEnableRoomKey()
	return GroupChat_GetEnableRoomKey()
end

-- 그룹 채팅 아이템 링크 업데이트
function CF_GroupChat_UpdateLinkItem( nKey )
	GroupChat_UpdateLinkItem( nKey )
end

-- 정보보기창 열기
function CF_ReferChar_Open( strName )
	
	if true == IsOpenWidget( WIDGET_ID_REFERCHAR ) then
		CloseWidget( WIDGET_ID_REFERCHAR )
	end
	
	if true == IsOpenWidget( WIDGET_ID_REFERCHAR_SKILL ) then
		CloseWidget( WIDGET_ID_REFERCHAR_SKILL )
	end
	
	g_bReferChar_InitData = false
	g_bReferCharSkill_InitData = false
	
	ReferChar_SetName( strName )
	
	OpenWidget( WIDGET_ID_REFERCHAR )
	
end

-- 정보보기창 캐릭터 이름 설정
function CF_ReferChar_SetName( strName )
	ReferChar_SetName( strName )
end

-- 정보보기창 캐릭터 이름 얻기
function CF_ReferChar_GetName()
	return ReferChar_GetName()
end

-- 클럽창 공지 변경
function CF_Club_NoticeChange()
	Set_Club_Notice()
end

-- 클럽 창고 아이템 변경
function CF_Club_StorageItem_Change(Channel, PosX, PosY)
	Set_ClubSlot_Item(Channel, PosX, PosY)
end

-- 클럽창고 돈 업데이트
function CF_Club_Storage_UpdateMoney(IncomeMoney, StorageMoney)
	Set_Club_StorageUpdateMoney(IncomeMoney, StorageMoney)
end

-- 별명 변경
function CF_Club_NickName_Change(_NickName)
	Set_Club_NickName(_NickName)
end

-- 계급 이름 변경
function CF_Club_GradeName_Change()
	Set_Club_GradeName()
end

-- 계급별 권한 설정
function CF_Club_SetGrade_Auth()

	Set_Club_Auth()
	Club_UpdateRank()
	Club_UpdateMember()
	
end

-- 자동 추방 설정
function CF_Club_AutoBan()
	Set_Club_AutoBan()	
end

-- 클럽 창고로 열기
function CF_Club_OpenStorage()
	Club_OpenStorage()
end

-- 클럽 동맹/적대 클럽 리스트 업데이트
function CF_Club_UpdateAlliance()
	Ev_Club_BasicAllianceHostileInfo()
end

-- 클럽 멤버 리스트 업데이트
function CF_Club_UpdateMember()
	Club_UpdateMember()
end

-- 클럽 권한 위임 메시지
function CF_Club_AuthorityMemberAsk( strClubName )
	Club_AuthorityMemberAsk( strClubName )
end

-- 클럽 로그 업데이트
function CF_Club_UpdateLog()
	Club_UpdateLog()
end

-- 클럽 동맹 가입 메시지
function CF_Club_AllianceJoinAsk( dwChiefCharID, strChiefCharName )
	Club_AllianceJoinAsk( dwChiefCharID, strChiefCharName )
end

-- 클럽 휴전 제안 메시지
function CF_Club_ArmisticeAsk( dwClubID, strClubName, dwReqChar, bAlliance )
	Club_ArmisticeAsk( dwClubID, strClubName, dwReqChar, bAlliance )
end

-- 클럽 배틀 신청 메시지
function CF_Club_BattleAsk( dwMasterID, strClubMaster, dwBattleTime, bAlliance )
	Club_BattleAsk( dwMasterID, strClubMaster, dwBattleTime, bAlliance )
end

-- 클럽 초대 메시지 수신(원용)
function CF_Ask_Club_Join(_MasterName, _ClubName, _MasterID)
	--g_Client:ReqClubJoinAns 답장 함수(마스터ID, True/false)
	
	
	local dwTimeKey = "2015-03-05"
	
	local tbClubInfor = {}
	tbClubInfor["dwClubName"] = _ClubName
	tbClubInfor["dwMasterID"] = _MasterID
	
	Insert_Invitation_Table(INVITATION_REPORT_CLUB, _MasterName, tbClubInfor)
	
	Report_MessegeBox_Invitation(g_ReportPoint)
end

-- 클럽 해산
function CF_Club_Dissolution( bDissolution )

	Club_Dissolution( bDissolution )

end

-- 클랍 등급 업데이트
function CF_Club_UpdateRank()
	Club_UpdateRank()
end

-- 클럽 선도 맵 업데이트
function CF_Club_UpdateGuidanceMap()

	ClubCommisionArea_Update()
	Club_CheckCommisionArea()
	
end

function CF_Club_UpdateBattleClubList()

	Ev_Club_BasicAllianceHostileInfo()

end

function CF_Club_UpdateAllianceClubList()

	Ev_Club_BasicAllianceHostileInfo()

end

-- 클럽 전적 업데이트
function CF_Club_UpdateBattleRecords()

	Club_UpdateBattleRecords()

end

function CF_QUICKMENU_ISMOVEDUI()

	return QuickMenu_IsMovedUI()

end

function CF_QUICKMENU_SETMOVEDUI( bMovedUI )

	QuickMenu_SetMovedUI( bMovedUI )

end

-- 펫 소환(원용)
function CF_Pet_OpenWindow()

	g_Use_QuickMenu_Pet = true
	QuickMenu_ViewSet()
	
end

-- 펫 소환해제
function CF_Pet_CloseWindow()

	g_Use_QuickMenu_Pet = false
	QuickMenu_ViewSet()
	
	-- 펫 관련 UI 끄기
	Pet_CloseWindow()
	CloseWidget( WIDGET_ID_PETCOLOR )
	CloseWidget( WIDGET_ID_PETSTYLE )
	
end

-- 펫창 업데이트
function CF_Pet_UpdateUI()
	Pet_Update( false )
end

-- 펫 이름 변경
function CF_Pet_Rename()
	Pet_Rename()
end

-- 포인트샵 카트 업데이트
-- -1 : 입력받은 아이템 가격 0원
--  0 : 카트 리스트 풀
--  1 : 정상 동작
function CF_PointShop_UpdateCart( nError )
	PointShop_UpdateCart()
	PointShop_HandleError(nError)
end

-- 포인트샵 카트 아이템 총 가격
function CF_PointShop_GetCartItemPrice()
	return PointShop_GetCartItemPrice()
end

-- 포인트 샵 UI 열기
function CF_PointShop_Open()
	
	if true == g_bPointShop_ForceClosed then
	
		OpenWidget( WIDGET_ID_POINTSHOP )
		g_bPointShop_ForceClosed = false
		
	end
	
end

-- 포인트샵 UI 닫기
function CF_PointShop_Close()
	
	if true == IsOpenWidget( WIDGET_ID_POINTSHOP ) then
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "POINT_SHOP_CHANGE_DATA", 0 ), TEXT_COLOR_LIGHTSKYBLUE )
		CloseWidget( WIDGET_ID_POINTSHOP )
		
		g_bPointShop_ForceClosed = true
	
	end

end

-- 제작초기화 열기
function CF_ProductReset_Open( wAble, wPosX, wPosY )
	ProductReset_Open( wAble, wPosX, wPosY )
end

-- 제작하기
function CF_ProductStart( strItemName, nCount,fDelay )
	Product_EnableProductComponent(false)
	ProductProgress_ResetData()
	ProductProgress_SetData( strItemName, nCount, fDelay )
	if IsOpenWidget( WIDGET_ID_PRODUCT_PROGRESS ) == false then
		OpenWidget( WIDGET_ID_PRODUCT_PROGRESS )
	end
	
end

function CF_ProductOngoingStart()
	
	ProductProgress_SetData( strConfirmItemName, nConfirmCount, fConfirmDelay )
	ProductProgress_StartProduct()
	
end

-- 제작취소
function CF_ProductEnd()
	Product_EnableProductComponent(true)	
	ProductProgress_ResetData()
	
	CloseWidget( WIDGET_ID_PRODUCT_PROGRESS )
	
	if IsOpenWidget( WIDGET_ID_PRODUCT ) then
		SetString( WIDGET_ID_PRODUCT, "label", GetGameWordText( "PRODUCT_BUTTON", 0 ), "Goods.ProductionButton" )
		Product_Refresh()
	end
	
end

-- 제작 새로고침
function CF_Product_Refresh()
	if IsOpenWidget( WIDGET_ID_PRODUCT ) then
		Product_Refresh()
	end
end

-- 제작 타입 업데이트
function CF_Product_UpdateType()
	Product_UpdateType()
end

function CF_ProductSuccess()
	ProductSuccess()
end

-- 망각의 약 UI 열기
function CF_StatsReset_UIOpen()
	UIOpenStatsRest()
end

-- 망각의 약 UI 열려있나?
function CF_StatsReset_IsOpen()
	return IsOpenWidget( WIDGET_ID_STATS_RESET )
end

-- 망각의 약 스탯 초기화 재계산
function CF_StatsReset_ReCalc()
	StatsReset_ReCalc()
end

-- 망각의 약 스탯 초기화 결과
function CF_StatsReset_Result( nError, wStatNum, wSkillNum, wItemNum )
	StatsReset_Result( nError, wStatNum, wSkillNum, wItemNum )
end

-- 머리 스타일 변경 카드 사용
function CF_StyleHairUse()
	HSCC_UseItem()
end

-- 머리 색상 변경을 위젯이 열려있는가?
function CF_HairColor_IsOpen()
	return IsOpenWidget( WIDGET_ID_HAIRCOLOR )
end

-- 머리 스타일 변경 에러
function CF_StyleHairError( emFB, dwID )
	HSCC_UseResult( emFB, dwID )
end

-- 얼굴 스타일 변경 카드 사용
function CF_StyleFaceUse()
	FSCC_UseItem()
end

-- 얼굴 스타일 변경 에러
function CF_StyleFaceError( emFB, dwID )
	FSCC_UseResult( emFB, dwID )
end

-- 성별 변경 카드 사용
function CF_StyleGenderUse()
	GCC_UseItem()
end

-- 성별 변경 에러
function CF_StyleGenderError( emFB )
	GCC_UseResult( emFB )
end

-- 펫 스타일 변경 카드 사용
function CF_StylePetUse()
	PetStyle_UseItem()
end

-- 펫 스타일 변경 에러
function CF_StylePetError( emFB, wStyle, wColor )
	PetStyle_UseResult( emFB, wStyle, wColor )
end

-- 현재 맵 변경
function CF_Minimap_MoveActiveMap( strMapName, strMapTexture, bSecretMap, dwServerID, dwClubID, strClubName )
	Minimap_MoveActiveMap( strMapName, strMapTexture, bSecretMap, dwServerID, dwClubID, strClubName )
end

-- 우편함
-- 열기
function CF_PostBox_Open()
	if IsOpenWidget( WIDGET_ID_POSTBOX ) == false then
		OpenWidget( WIDGET_ID_POSTBOX )
	end
end

-- 우편함 닫기
function CF_PoseBox_Close()
	CloseWidget( WIDGET_ID_POSTBOX )
end

-- 우편함 열려있나?
function CF_PostBox_IsOpen()
	return IsOpenWidget( WIDGET_ID_POSTBOX )
end

-- 받은우편 목록 받기완료
function CF_PostBox_ReceivePost()
	Msg_PostBox_ReceivePagePost()
end

function CF_PostBox_SelectedPost( nSelected )
	Msg_PostBox_SelectedPost( nSelected )
end

function CF_PostBox_ReceivePageUpdate()
	Msg_PostBox_ReceivePageUpdate()
end

function CF_PostBox_SendPageAttachSlot()
	PostBox_SendPageAttachSlot()
	SetBool( WIDGET_ID_POSTBOX, "visible", false, "pageSend.staticSendNonItem" )
end

function CF_PostBox_SendPageResetSlot()
	PostBox_SendPageResetSlot()
	SetBool( WIDGET_ID_POSTBOX, "visible", true, "pageSend.staticSendNonItem" )
end

function CF_PostBox_HistoryPageUpdate()
	PostBox_HistoryPageUpdate()
end

function CF_PostBox_HistoryPageSendUpdate()
	PostBox_HistoryPageSendUpdate()
end

function CF_PostBox_HistoryPageRecieveUpdate()
	PostBox_HistoryPageRecieveUpdate()
end

-- 새로운 우편 받음( 새로운 우편 갯수 )
function CF_PostBox_AcceptPost( nNewPost )

	QuickMenu_ViewSet()
	PostBox_ReceivePage_UnReadLabel()
	
	if 0 ~= nNewPost then
		CF_PrintMsg_SystemMsg( GetGameInternalText( "POSTBOX_MSG_RECEIVE_NEW", 0 ), TEXT_COLOR_LIGHTSKYBLUE )
	end
	
end

-- 우편 에러 메시지
function CF_PostBox_ErrMsg( nError )
	PostBox_ErrMsg( nError )
end

-- 우편함 열려있나?
function CF_PostBox_IsOpen()
	return IsOpenWidget( WIDGET_ID_POSTBOX )
end

-- 스탯초기화
function CF_StatsReset_Open( tbData, tbEvenly, tbItem )
	StatsReset_Open( tbData, tbEvenly, tbItem )
end

-- 전체맵 캐릭터 검색 결과
function CF_LargeMap_SearchCharResult( tbResult )
	LargeMap_SearchCharResult( tbResult )
end

-- 전체맵 검색한 캐릭터 세부 정보
function CF_LargeMap_SearchCharDetail( nLevel, nSchool, nClass, dwGaeaID, bParty, strName, strTitle, strClubName )
	LargeMap_SearchCharDetail( nLevel, nSchool, nClass, dwGaeaID, bParty, strName, strTitle, strClubName )
end

-- 전체맵 검색한 캐릭터 지도에 표시
function CF_LargeMap_SearchCharMapPos( tbMapPos )
	LargeMap_SearchCharMapPos( tbMapPos )
end

-- 전체맵 캐릭터 이동 아이콘 끄기
function CF_LargeMap_HideMoveIcon()
	LargeMap_HideMoveIcon()
	MiniMap_HideMoveIcon()
end

-- 라지맵 몹 아이콘 툴팁
function CF_LargeMap_GetMobTooltip(tbMob)
	return LargeMap_GetMobTooltip(tbMob)
end

-- 탈것 색상 변경 카드 사용
function CF_VehicleColor_ItemUse( strName )
	VehicleColor_ItemUse( strName )
end

-- 탈것 색상 변경 UI 열려있나?
function CF_VehicleColor_IsOpen()
	return IsOpenWidget( WIDGET_ID_VEHICLECOLOR )
end

-- 탈것 색상 변경 UI 닫기
function CF_VechicleColor_Close()
	CloseWidget( WIDGET_ID_VEHICLECOLOR )
end

--개인상점검색창 열기
function CF_PrivateMarketSearch_Open()
	ToggleUI( WIDGET_ID_PRIVATE_MARKET_SEARCH )
end

--개인상점검색창 판매물품 등록
function CF_PrivateMarketSearch_RegisterItem( nResult, wMID, wSID, nPosX, nPosY, strItemName )
	PMS_RegSlotItem( nResult, wMID, wSID, nPosX, nPosY, strItemName )
end

-- 개인상점 열려있나?
function CF_PrivateMarketSearch_IsOpen()
	return IsOpenWidget( WIDGET_ID_PRIVATE_MARKET_SEARCH )
end

-- 개인상점 검색 페이지 업데이트
function CF_PrivateMarketSearch_UpdateSearchPage()
	PMS_SearchUpdate()
end

-- 개인상점 에러
function CF_PrivateMarketSearch_Error( nError, bUpdate )
	PMS_Error( nError, bUpdate )
end

-- 개인상점 검색 거래내역 업데이트
function CF_PrivateMarketSearch_UpdateLog()
	PMS_UpdateLog()
end

-- 개인 상점 검색 포인트 거래 리스트 업데이트
function CF_PrivateMarketSearch_UpdatePointTrade()
	PMS_UpdatePointTrade()
end

-- 개인 상점 검색 검색 최대 페이지 업데이트
function CF_PrivateMarketSearch_UpdateSearchMaxPage( nMaxPage )
	PMS_UpdateSearchMaxPage( nMaxPage )
end

-- 개인 상점 검색 장바구니 최대 페이지 업데이트
function CF_PrivateMarketSearch_UpdateWishMaxPage( nMaxPage )
	PMS_UpdateWishMaxPage( nMaxPage )
end

-- 펫 색상 변경 카드 사용
function CF_PetColor_ItemUse()
	PetColor_ItemUse()
end

-- 코스튬 색상 변경 카드 사용
function CF_CostumeColor_ItemUse( bRestrict )
	CostumeColor_ItemUse( bRestrict )
end

-- 생활기록부 열기
function CF_StudentRecord_Open()

	OpenWidget( WIDGET_ID_STUDENT_RECORD )
	
	-- !!
	-- 좋지 않다. 최초 접속 후 ToGameStage 호출될 시 아직
	-- 캐릭터 초기화가 안돼서 소모품 착용 확인이 안된다.
	-- 캐릭터 초기화 끝났는지 알 수 있는 뭔가가 필요할 듯.
	EquipConsume_Check()
	ExtremeWeapon_Check()
	
end

-- 생활 기록부 닫기
function CF_StudentRecord_Close()
	CloseWidget( WIDGET_ID_STUDENT_RECORD )
end

-- 특별활동 업데이트
function CF_StudentRecord_ActivityPage_Update()
	StudentRecord_ActivityPage_Update()
end

-- 특별활동 진행 메시지 보기(완성된 문자열, 현재 횟수, 목표 횟수)
function CF_StudentRecord_ProgressMessege_Update( strName, nNow, nMax )
	local ProgressStr = StringFormat( GetGameInternalText( "ACTIVITY_UPDATE_PROGRESS_PROGRESSDISPLAY", 0 ), strName, nNow, nMax )
	if g_StudentRecord_Activity_ProgressDisplayChecked == true then
		StudentRecord_CFProgressMessege( ProgressStr )
	end	
end

-- 출석부 업데이트
function CF_StudentRecord_AttendancePage_Update()
	StudentRecord_AttendancePage_Update()
end

function CF_StudentRecord_AttendancePage_UpdateContents()

	if IsOpenStudentRecord() == false then
		return
	end

	StudentRecord_AttendancePage_UpdateContents()
	StudentRecord_AttendancePage_UpdateRewardItem()
end

-- 캐릭터 부활 정보 수신
function CF_Resurrection_ReceiveInfo( bResurrectionSkill, bGeneralResurrection, bForcedResurrection, fResurrectTime )

	if ( true == bResurrectionSkill ) and ( true == bForcedResurrection ) then
		OpenForceResurrection_Skill()
	elseif ( true == bGeneralResurrection ) then
		Resurrection_ReceiveInfo( bResurrectionSkill, fResurrectTime )
	elseif ( true == bForcedResurrection ) then
		OpenForceResurrection_SetTime( fResurrectTime )
	end
	
end

-- 부활창 끄긱
function CF_Resurrection_Close()
	CloseWidget( WIDGET_ID_RESURRECTION )
end

-- 게임 비정상 종료시 출력될 메시지박스
function CF_CloseConnection(nTYPE)
	local strMsg = ""
	if nTYPE == 2 then
		strMsg = GetGameInternalText("PLAYINTERFACE_5", 0)
	elseif nTYPE == 1 then
		strMsg = GetGameInternalText("PLAYINTERFACE_8", 0)
	elseif nTYPE == 0 then
		strMsg = GetGameInternalText("PLAYINTERFACE_9", 0)
	end
	
	OpenMsgBox( MSG_BOX_TYPE_OK, strMsg, Event_MsgBox_CloseGame )
end

-- ///////////////////////////////////////////////////////////////////////////////////////////////
-- New Instance System 관련 UI //////////////////////////////////////////////////////////////////
-- ///////////////////////////////////////////////////////////////////////////////////////////////

-- Open InstanceSystem UI;
function CF_OpenInstanceSystemUI()

	if ( false == IsOpenWidget( WIDGET_ID_INSTANCE_SYSTEM_DISP ) ) then
	
		CloseInstanceSystemUI()
		
		OpenWidget( WIDGET_ID_INSTANCE_SYSTEM_DISP )
		
	end
	
end

-- 통곡의 빌딩 UI 호출
function CF_SetPVEEntranceState(nCur, nAll, nSec, bShow)
	CF_OpenInstanceSystemUI()
	
	PVESetEntranceState(nCur, nAll, nSec, bShow)
end
-- 
function CF_SetPVEStage(nStage, bShow)
	CF_OpenInstanceSystemUI()
	
	PVESetStage(nStage, bShow)
end

function CF_SetPVEMessage(szText, bShow)
	CF_OpenInstanceSystemUI()
	
	PVESetMessage(szText, bShow)
end

function CF_SetPVEMonsterIndicator(nCur, nAll, bShow)
	CF_OpenInstanceSystemUI()
	
	PVEMonsterIndicator(nCur, nAll, bShow)
end

-- 스테이지를 클리어 하였을때 실행되는 클콜 함수
function CF_SetPVEPositioningIndicator(nX, nY, nNow, nNeed, bShow)
	CF_OpenInstanceSystemUI()
	
	PVEPositioningIndicator(nX, nY, nNow, nNeed, bShow)
end

function CF_SetPVEStageIndicator(nStage, nEnd, nRange, nSec, bBlank, bShow)
	CF_OpenInstanceSystemUI()
	
	PVESetStageIndicator(nStage, nEnd, nSec, bBlank, bShow )
end

function CF_SetPVEStageState(nState, bShow)
	CF_OpenInstanceSystemUI()
	
	PVESetStageState(nState, bShow)
end

-- 제도전 메시지 박스 출력함수
function CF_SetPVERetry(llMoney, nTime, bShow)
	CF_OpenInstanceSystemUI()
	
	if true == bShow and 0 < nTime then
	
		local strMsg = string.format(GetGameInternalText("COMPETITION_PVE_RETRY_TEXT", 0), llMoney )
		OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsg, 
			Event_InfinityCooperation_Retray_MsgBox_btnOk, Event_InfinityCooperation_Retray_MsgBox_btnCancel,
			nil, nil, nTime, Event_InfinityCooperation_Retray_MsgBox_btnCancel )

	end
end

-- 통곡의 빌딩 결과창 출력함수
function CF_SetPVEStageResult( nTime )

	Event_MsgBox_Close()
	PVEReward_OpenWidget( nTime )
	
end

-- 재도전 UI, 인던을 실패했을 때 다시 할것인지 확인한다
function CF_SetNewInstanceRetry(llMoney, nTime, bShow)
	SetNewInstanceRetry( llMoney, nTime, bShow );
end

-- ///////////////////////////////////////////////////////////////////////////////////////////////
-- Matching System 관련 UI //////////////////////////////////////////////////////////////////////
-- ///////////////////////////////////////////////////////////////////////////////////////////////

-- 입장 확인 UI, 신청 후 입장을 정말 할것인지 다시한번 확인한다
function CF_MatchingConfirmJoin ( _bShow, _nTime )
	Matching_ConfirmJoin( _bShow, _nTime );
end

-- ///////////////////////////////////////////////////////////////////////////////////////////////
-- ///////////////////////////////////////////////////////////////////////////////////////////////
-- ///////////////////////////////////////////////////////////////////////////////////////////////



-- 개조카드 UI 오픈
function CF_RebuildCard_Open( bUseStemp, nRebuildCard_ItemType )
	return RebuildCard_Open( bUseStemp, nRebuildCard_ItemType )
end

-- 개조카드 UI 닫기
function CF_RebuildCard_Close()
	CloseWidget( WIDGET_ID_REBUILD_CARD )
end

-- 개조카드 아이템 슬롯 등록
function CF_RebuildCard_RegistItem( wPosX, wPosY, tbItemInfo )
	RebuildCard_RegistItem( wPosX, wPosY, tbItemInfo, nil )
	RebuildCard_UpdateInfo()
	
end

-- 개조카드 아이템 해제
function CF_RebuildCard_RemoveItem()
	RebuildCard_RemoveItem()
end

-- 개조카드 스템프 슬롯 등록
function CF_RebuildCard_RegistStemp( wMID, wSID, strName, nFixNum )
	RebuildCard_RegistStemp( wMID, wSID, strName, nFixNum )
end

-- 개조카드 아이템 해제
function CF_RebuildCard_RemoveStemp()
	RebuildCard_RemoveStemp()
end

-- 개조카드 랜덤 옵션 업데이트
function CF_RebuildCard_Update( tbItemInfo, tbCacheItemInfo, nFixedOptionIndex )
	RebuildCard_Update( tbItemInfo, tbCacheItemInfo, nFixedOptionIndex )
end

-- 개조카드 인장 갱신
function CF_RebuildCard_Update_FixdOption ( _nIndex )

	RebuildCard_Update_FixdOption( _nIndex )
	
end

-- 개조카드 인장 체크해제
function CF_RebuildCard_StampUncheck()
	RebuildCard_StampUncheck()
end

-- 개조 확정 및 취소 팝업
function CF_RebuildCard_FixPopupUI ()
	RebuildCard_FixPopupUI()
end

function CF_RebuildCard_ChangedRebuildType(bType)
	RebuildCard_ChangedRebuildType(bType)
	
end

-- 전장 PVP 정보 업데이트
function CF_Competition_UpdatePVP()
	Competition_UpdatePVP()
	Minimap_SetCTFVictorySchool()
	Minimpa_UpdateVictoryIconPosInfo()
end

-- 전장 CTF 정보 업데이트
function CF_Competition_UpdateCTF()
	Competition_UpdateCTF()
end

-- 전장 UI CDM 랭크 정보 업데이트
function CF_Competition_CDMRankInfo( tbRankInfo )
	Competition_CDMRankInfo( tbRankInfo )
end

-- 전장 UI PVE 정보 업데이트
function CF_Competition_UpdatePVE()
	Competition_UpdatePVE()
end

--전장 UI : PVE 입장 확인 메시지 박스 
function CF_Competition_PVEAckEntrance( bShow, dwMapID )
	Competition_PVEAckEntrance( bShow, dwMapID )
end

--전장 UI : PVE 입장 요청 결과
function CF_Competition_PVE_AckEnternceConfirm( nResult, strChaName )
end

-- 게이트 이동창
function CF_Gate_Open( tbMapName )

	if nil == tbMapName then
		return
	end
	
	-- 테이블 개수가 1개면 일반 게이트 이동
	if 1 == table.getn( tbMapName ) then
		GateMsg_Open( tbMapName[ 1 ] )
	
	-- 아니면 멀티 게이트 이동
	else
	
		MultiGateMsg_Open( tbMapName )
	
	end
	
end

-- 게이트 이동창 끄기
function CF_Gate_Close()

	GateMsg_Close()
	MultiGateMsg_Close()
	
end

-- 친구 탭 업데이트
function CF_RnCommunity_FriendTab_Update()
	
	Community_UpdateFriendList( false )
	
end

function CF_Community_BlockTab_Update()

	Community_UpdateFriendList( false )
	Community_UpdateBlock()

end

function CF_Community_FriendGroupAdd( nGroupNum )

	Community_UpdateCount()
	Community_UpdateFriendList( false )
	
end

function CF_Community_FriendGroupDelete( nGroupNum )

	Community_UpdateCount()
	Community_UpdateFriendList( false )

end

function CF_Community_FriendAdd( nGroupNum )

	Community_UpdateCount()
	Community_UpdateFriendList( false )
	
end

function CF_Community_FriendDelete( nGroupNum )

	Community_UpdateCount()
	Community_UpdateFriendList( false )
	Community_UpdateBlock()

end

function CF_Community_FriendSetGroup( wBeforeGroupNum, wNewGroupNum )

	Community_UpdateFriendList( false )

end

function CF_Community_FriendStateUpdate( nGroupNum )

	Community_UpdateState( nGroupNum )

end

-- 친구 요청 받음
function CF_Community_ReqFriend( strName, bNotify, tbMGuid )

	
	Insert_Invitation_Table(INVITATION_REPORT_FRIEND, strName, nil, tbMGuid )	
	Report_MessegeBox_Invitation(g_ReportPoint)
	
end

-- 슬롯 ID 리턴
function CF_Toggle_Nofity_Competition_Texture( nState )
	ToggleNotifyCompetitionTexture( nState )
end

function CF_MiniGame_Dice_Open()
	MiniGameDice_Open()
end	

function CF_MiniGame_Dice_Close()
	MiniGameDice_Close()
end	
-- 점령전 점령시 호출됨
function CF_DisplayProgressNotify(nProgress)
	DisplayProgressNotify(nProgress)
end

function CF_MiniGame_Dice_Turn_Stage()
	TurnStageMiniGameDice()
end
-- 점령전 점령시 호출, 입장시 호출됨
function CF_CTFAuthCountInit(nSMAuthCnt, nHAAuthCnt, nBHAuthCnt)
	CTFAuthCountInit(nSMAuthCnt, nHAAuthCnt, nBHAuthCnt)
end

function CF_DisplayProgressNotify_Ready(nRemainMinutes)
	DisplayProgressNotify_Ready(nRemainMinutes)
end

function CF_Competition_CDM_Result_Open(closetime)
	CDMResult_Open(closetime)
end
function CF_Competition_CDM_Result_Close()
	CDMResult_Close()
end
function CF_Competition_CDM_Result_Clear_List(_tableInformation)
	CDMResult_SetTitle(_tableInformation)
end
function CF_Competition_CDM_Result_Add_List_Item(_tableItem)
	CDMResult_AddInfo(_tableItem)
end

function CF_Toggle_Public_Notify_Message(stringOut, colorString, sizeString, nTime)
	TogglePublicNotifyMessage(stringOut, colorString, sizeString, nTime)
end

function CF_Competition_CDM_Status_Board_Update( _tableInformationBoard )
	CompetitionCDMStatusBoardUpdate( _tableInformationBoard )
end

function CF_Competition_CDM_Status_Board_Close()
	CompeititonCDMStatusBoardClose()
end

function CF_CDM_ShowDoublePoint( bVisible )
	CDMDoublePoint_Show( bVisible )
end

function CF_OpenCTFDisplayInfo()
	if IsOpenWidget(WIDGET_ID_CTF_DISPLAY_INFO) == false then
		OpenWidget(WIDGET_ID_CTF_DISPLAY_INFO)
	end
end

function CF_CloseCTFDisplayInfo()
	if g_bCTFieldHPDisplayOpen == true then
		CloseWidget(WIDGET_ID_CTF_DISPLAY_INFO)
		CloseWidget(WIDGET_ID_CTFIELD_HPDISPLAY)
	end
end

function CF_CTFieldHpDisplay_SetPos(nSM, nHA, nBH, PosX, PosY)
	CTFieldHpDisplay_SetAuthenticHP(nSM, nHA, nBH, PosX, PosY)
end

function CF_CTFieldHpDisplay_Close()
	CloseWidget(WIDGET_ID_CTFIELD_HPDISPLAY)
end

---
function CF_CTFieldDisplayInfo_AuthenticatorUpdate()
	LargeMap_SetAuthenticatorIcon(g_MyChar:GetCurrentMapID())
	Minimap_SetAuthenticatorIcon()
	CTFieldDisplayInfo_UpdateAuthenticatorStatus()
end

function CF_CTFieldDisplayInfo_SetCTFState(nState)
	CTFieldDisplayInfo_SetCTFState(nState)
end

function CF_CTFieldDisplayInfo_SetTimeSync(strTime)
	CTFieldDisplayInfo_SetTimeSync(strTime)
end
------

--[[
function CF_CTFDisplayInfo_SetAuthCtrlHPDisplay(nMID, nSID)
	CTFDisplayInfo_SetAuthCtrlHPDisplay(nMID, nSID)
end
]]--

function CF_CTFDisplayInfo_CloseWidget()
	if IsOpenWidget(WIDGET_ID_CTF_DISPLAY_INFO) == true then
		CloseWidget(WIDGET_ID_CTFIELD_HPDISPLAY)
	end
end


-- DPS 업데이트
function CF_DPS_Update()
	DPS_Update()
end

--택시카드 열기
function CF_TaxiCard_Open()
	local IsOpen = IsOpenWidget( WIDGET_ID_TAXI_CARD )
	if false == IsOpen then
		OpenWidget( WIDGET_ID_TAXI_CARD )
	end
	
	return IsOpen;
end

--택시카드 닫기
function CF_TaxiCard_Close()
	if true == IsOpenWidget( WIDGET_ID_TAXI_CARD ) then
		CloseWidget( WIDGET_ID_TAXI_CARD )
	end
end

function CF_ItemPreview_Open()
	ItemPreiview_Open()
end

function CF_ItemPreview_IsOpen()
	return IsOpenWidget( WIDGET_ID_ITEM_PREVIEW )
end

function CF_ItemPreview_SetSlot( tbItem )
	ItemPreview_SetSlot( tbItem )
end

-- 토너먼트 대진표 : 배팅정보
function CF_Tournament_BettingInfo( llMinBettingSize, llMaxBettingSize, dwScriptID )
	Tournament_SetBettingInfo( llMinBettingSize, llMaxBettingSize, dwScriptID )
end

-- 토너먼트 대진표 : 배팅그룹 정보
function CF_Tournament_BettingGroupInfo( dwUserDBID, dwBettingGroup, llBettingCount, dwLeageID, fBettingRatio )

	Tournament_SetBettingGroupInfo( dwUserDBID, dwBettingGroup, llBettingCount, dwLeageID, fBettingRatio )
end

-- 로또 당첨자 업데이트
function CF_Lotto_UpdateWinnerList( nSystemID )
	Lotto_UpdateWinnerList( nSystemID )
end

-- 로또 당첨 번호 업데이트
function CF_Lotto_UpdateWinNum( nSystemID, tbNum )
	Lotto_UpdateWinNum( nSystemID, tbNum )
end

-- 로또 회차 누적 금액
function CF_Lotto_AccumulateMoney( nSystemID, llMoney, bPrev )
	Lotto_AccumulateMoney( nSystemID, llMoney, bPrev )
end

-- 로또 구매 리스트 업데이트
function CF_Lotto_UpdateBuyList( nSystemID, bPrev )

	if false == bPrev then

		Lotto_UpdateBuyList( nSystemID )
		
	else
	
		LottoConfirm_Update( nSystemID )
	
	end
	
end

-- 로또 상태 업데이트
function CF_Lotto_ChangeState( nSystemID, nState )
	Lotto_ChangeState( nSystemID, nState )
end

-- 로또 다음턴 진행
function CF_Lotto_NextTurn( nSystemID )
	Lotto_NextTurn( nSystemID )
end

-- 퀘스트 시작
function CF_Quest_Start(QuestID)
	Quest_ListLoad_Run()
	Quest_ListLoad_End()
end
-- 퀘스트 끝(종료 퀘스트의 경우 ID를 이상하게 넘겨줌)d
function CF_Quest_End(QuestID)
	Quest_ChackBox_UnChack( QuestID )
end
-- 단계별 UI호출에 변경사항이 생겨서 필요한 데이터들을 가져오는 것은 추가해야함
-- 현재는 UI호출하는 단계만 나눠져 있음(해당하는 단계의 데이터를 가져오는 부분이 없음)
-- 퀘스트 진행(몹 제거)
function CF_Quest_ProGMobkill(QuestID)
	CF_Quest_Helper_DataSetting( QuestID )
	Quest_UpdateProc()
end
-- 퀘스트 진행(퀘스트 아이템 습득)
function CF_Quest_ProGQItem(QuestID)
	CF_Quest_Helper_DataSetting( QuestID )
	Quest_UpdateProc()
end
-- 퀘스트 진행(퀘스트 아이템 사용)
function CF_Quest_ProGItemUse(QuestID)
	CF_Quest_Helper_DataSetting( QuestID )
	Quest_UpdateProc()
end
-- 퀘스트 진행(연마 퀘스트 아이템 연마)
function CF_Quest_ProGItemGrinding(QuestID)
	CF_Quest_Helper_DataSetting( QuestID )
	Quest_UpdateProc()
end
-- 퀘스트 진행(대련)
function CF_Quest_ProGConfting(QuestID)
	CF_Quest_Helper_DataSetting( QuestID )
	Quest_UpdateProc()
end
-- 퀘스트 진행(위치 도달)
function CF_Quest_ProGReachZone(QuestID)
	CF_Quest_Helper_DataSetting( QuestID )
	Quest_UpdateProc()
end
-- 퀘스트 진행(경과 시간)
function CF_Quest_ProGTime(QuestID)
	CF_Quest_Helper_DataSetting( QuestID )
	Quest_UpdateProc()
end
-- 퀘스트 진행(제한 시간 초과)
function CF_Quest_ProGTimeOver(QuestID)
	CF_Quest_Helper_DataSetting( QuestID )
	Quest_UpdateProc()
end
-- 퀘스트 진행(사망 제한)
function CF_Quest_ProGNonDie(QuestID)
	CF_Quest_Helper_DataSetting( QuestID )
	Quest_UpdateProc()
end
-- 퀘스트 진행(레벨 도달)
function CF_Quest_ProGLevel(QuestID)
	CF_Quest_Helper_DataSetting( QuestID )
	Quest_UpdateProc()
end
-- 퀘스트 진행(레벨 업)
function CF_Quest_ProGLevelUp(QuestID)
	CF_Quest_Helper_DataSetting( QuestID )
	Quest_UpdateProc()
end
-- 퀘스트 진행(Npc 대화)
function CF_Quest_ProGNpcTalk(QuestID)
	CF_Quest_Helper_DataSetting( QuestID )
	Quest_UpdateProc()
end
-- 퀘스트 진행(맵 이동 제한)
function CF_Quest_ProGLeaveMap(QuestID)
	CF_Quest_Helper_DataSetting( QuestID )
	Quest_UpdateProc()
end
-- 퀘스트 진행(퀘스트 아이템 삽입)
function CF_Quest_ProGItemInsert(QuestID)
	CF_Quest_Helper_DataSetting( QuestID )
	Quest_UpdateProc()
end
-- 퀘스트 진행(퀘스트 아이템 삭제)
function CF_Quest_ProGItemDelete(QuestID)
	CF_Quest_Helper_DataSetting( QuestID )
	Quest_UpdateProc()
end
-- 퀘스트 진행(퀘스트 아이템 갯수 변화)
function CF_Quest_ProGItemCount(QuestID)
	CF_Quest_Helper_DataSetting( QuestID )
	Quest_UpdateProc()
end
-- 퀘스트 진행(퀘스트 아이템 줏음)
function CF_Quest_ProGItemPickUp(ItemID)
	--CF_Quest_Helper_DataSetting( ItemID )
	Quest_UpdateProc()
end
-- 퀘스트 진행단계 변경
function CF_Quest_ProgChange(QuestID)
	CF_Quest_Helper_DataSetting( QuestID )
	Quest_UpdateProc()
end
-- 퀘스트 진행중에서 삭제 
function CF_Quest_DelProG( QuestID )
	Quest_ChackBox_UnChack( QuestID )
	Quest_ListLoad_Run()
	Quest_ListLoad_End()
end
-- 퀘스트 완료에서 삭제 
function CF_Quest_DelEnd( QuestID )
	Quest_ListLoad_Run()
	Quest_ListLoad_End()
end

-- 퀘스트 UI 업데이트
function CF_Quest_Update()
	Quest_Clear()
end

-- 퀘스트 UI Claer
function CF_Quest_Clear()
	Quest_Clear()
end

-- 스킬 사용 예약
function CF_SkillReserve_SetSkill( dwID )
	SkillReserve_SetSkill( dwID )
end

function SkillReservation_CancelAll()
	SQS_SkillReservationCancelAll()
end

-- 커서 타겟 정보 (NONE)
function CF_CursorTargetInfo_SetNone()
	CursorTargetInfo_SetNone()
end

-- 커서 타겟 정보 (PC)
function CF_CursorTargetInfo_SetPC( strName, nLevel, nSchool, nClassIndex,
	nPartyState, dwPartyID, dwClubID, strClubName, bPkTime )
	
	CursorTargetInfo_SetPC( strName, nLevel, nSchool, nClassIndex, 
		nPartyState, dwPartyID, dwClubID, strClubName, bPkTime )
	
end

-- 커서 타겟 정보 (NPC)
function CF_CursorTargetInfo_SetNPC( strName, tbTalk, bQuestStart, bQuestStep )
	CursorTargetInfo_SetNPC( strName, tbTalk, bQuestStart, bQuestStep )
end

-- 커서 타겟 정보 (Mob)
function CF_CursorTargetInfo_SetMob( strName, nLevel, bNonDmg )
	CursorTargetInfo_SetMob( strName, nLevel, bNonDmg )
end

-- 커서 타겟 HP
function CF_CursorTargetInfo_SetHP( dwNow, dwMax )
	CursorTargetInfo_SetHP( dwNow, dwMax )
end

function CF_Modal_VehicleWith ( szOwnerName )
	Modal_VehicleWith ( szOwnerName )
end

-- CTField 보상 결과창 오픈
function CF_CTFieldRewardResult_Open( nCloseTime )
	CTFReward_OpenWidget(nCloseTime)
end

-- 버프 등록
function CF_Buff_Reg( dwSlot, nType, dwSkillID, strName, wLevel, fAge, fLife, bExpired )

	-- 일반 버프
	if 0 == nType then
	
		NormalBuff_Reg( dwSlot, dwSkillID, strName, wLevel, fAge, fLife )
		
	-- 추가 버프
	elseif 1 == nType then
	
		AddBuff_Reg( dwSlot, dwSkillID, wLevel, fAge, fLife )
	
	-- 도시락 버프
	elseif 2 == nType then
	
		LunchBoxBuff_Reg( dwSlot, dwSkillID, wLevel, fAge, fLife )
	
	-- 경험치 도시락 버프
	elseif 3 == nType then
	
		ExpLunchBoxBuff_Reg( dwSlot, dwSkillID, wLevel, fAge, fLife )
	
	-- 시스템 버프
	elseif 4 == nType then
	
		SystemBuff_Reg( dwSlot, dwSkillID, wLevel, fAge, fLife, bExpired )
	
	-- CTF 버프
	elseif 5 == nType then
	
		CtfBuff_Reg( dwSlot, dwSkillID, wLevel, fAge, fLife )
	
	end
	
	CharInfo_UpdateTerm()

end

-- 버프 중첩 확인
function CF_Buff_LunchBoxOverlap( bOverlap, strPrevSkill, strCurSkill, wPosX, wPosY )
	Buff_LunchBoxOverlap( bOverlap, strPrevSkill, strCurSkill, wPosX, wPosY )
end

-- 버프 삭제 불가능 지역
function CF_Buff_ImpossibleDelete( strSkillName )

	local str = string.format( GetGameInternalText( "LUNCHBOX_DISABLE_RELEASE", 0 ), strSkillName )
	CF_PrintMsg_SystemMsg( str, TEXT_COLOR_RED )

end

-- 버프 삭제 확인
function CF_Buff_MsgDelete( dwSkillID, strSkillName, strBinding )
	GameMsg_BuffDelete( dwSkillID, strSkillName, strBinding )
end

-- 버프 삭제 완료
function CF_Buff_Deleted( wSlotIndex )
	-- 클라이언트에서 모든 버프가 하나의 배열에 타입에 따라 인덱스로 나뉘어 있다.
	-- 타입별로 필요한 인덱스만큼 빼준다.
	
	-- 일반 버프
	if wSlotIndex < BUFF_NORMAL_SLOT_TOTAL then
	
		NormalBuff_Delete( wSlotIndex )
	
	-- 추가 버프
	elseif	wSlotIndex < ( BUFF_NORMAL_SLOT_TOTAL + BUFF_ADD_SLOT_TOTAL ) then
	
		AddBuff_Delete( wSlotIndex )
	
	-- 도시락 버프
	elseif	wSlotIndex < ( BUFF_NORMAL_SLOT_TOTAL + BUFF_ADD_SLOT_TOTAL + BUFF_LUNCHBOX_SLOT_TOTAL ) then
	
		LunchBoxBuff_Delete( wSlotIndex )
	
	-- 경험치 도시락 버프
	elseif	wSlotIndex < ( BUFF_NORMAL_SLOT_TOTAL + BUFF_ADD_SLOT_TOTAL + 
		BUFF_LUNCHBOX_SLOT_TOTAL + BUFF_EXP_LUNCHBOX_SLOT_TOTAL ) then
	
		ExpLunchBoxBuff_Delete( wSlotIndex )
	
	-- 시스템 버프
	elseif	wSlotIndex < ( BUFF_NORMAL_SLOT_TOTAL + BUFF_ADD_SLOT_TOTAL + 
		BUFF_LUNCHBOX_SLOT_TOTAL + BUFF_EXP_LUNCHBOX_SLOT_TOTAL + BUFF_SYSTEM_SLOT_TOTAL ) then
	
		SystemBuff_Delete( wSlotIndex )
	
	end
	
	CharInfo_UpdateTerm()

end

-- 도시락 버프 받음
-- 증가율, 총 증가율, 중첩 횟수, 총 중첩 횟수, 스킬 이름, 스킬 설명, 시간( [ 1 ] : 일 [ 2 ] : 시 [ 3 ] : 분 [ 4 ] : 초 )
function CF_Buff_RecvLunchBox( dwIncreaseRate, dwTotalRate, dwAccumCount, dwMaxCount,
	strSkillName, strDesc, tbTime, wSlotIndex )
	
	-- 일반 버프
	if wSlotIndex < BUFF_NORMAL_SLOT_TOTAL then
	-- 추가 버프
	elseif	wSlotIndex < ( BUFF_NORMAL_SLOT_TOTAL + BUFF_ADD_SLOT_TOTAL ) then
	-- 도시락 버프
	elseif	wSlotIndex < ( BUFF_NORMAL_SLOT_TOTAL + BUFF_ADD_SLOT_TOTAL + BUFF_LUNCHBOX_SLOT_TOTAL ) then
	
		LunchBoxBuff_DataSet( strSkillName, strDesc, tbTime )
	
	-- 경험치 도시락 버프
	elseif	wSlotIndex < ( BUFF_NORMAL_SLOT_TOTAL + BUFF_ADD_SLOT_TOTAL + 
		BUFF_LUNCHBOX_SLOT_TOTAL + BUFF_EXP_LUNCHBOX_SLOT_TOTAL ) then
		
		ExpLunchBoxBuff_DataSet( dwIncreaseRate, dwTotalRate, dwAccumCount, dwMaxCount, strSkillName, strDesc, tbTime )
		
	-- 시스템 버프
	elseif	wSlotIndex < ( BUFF_NORMAL_SLOT_TOTAL + BUFF_ADD_SLOT_TOTAL + 
		BUFF_LUNCHBOX_SLOT_TOTAL + BUFF_EXP_LUNCHBOX_SLOT_TOTAL + BUFF_SYSTEM_SLOT_TOTAL ) then
	end
	
end

-- 경험치 도시락 버프 중첩 횟수 설정
function CF_Buff_SetExpAccum( dwTotalRate, dwAccumCount, dwMaxCount )
	ExpLunchBoxBuff_SetAccum( dwTotalRate, dwAccumCount, dwMaxCount )
end

-- 단축키 설정 UI 열려있나?
function CF_HotKeyConfig_IsOpen()
	return IsOpenHotKeyCfg()
end

-- 큐박스 잠금 상태 설정
function CF_QuickMenu_SetQBoxLock( bEnable, bPartyMasterMsg )
	QuickMenu_SetQBoxLock( bEnable, bPartyMasterMsg )
end

-- 큐박스 잠금 상태 반환
function CF_QuickMenu_GetQBoxLock()
	return QuickMenu_GetQBoxLock()
end

-- 달리기 상태 설정
function CF_QuickMenu_SetRun( bRun )
	QuickMenu_SetRun( bRun )
end

-- 큐박스 습득
function CF_QBox_Take( nType, fTime )
	QBox_Take( nType )
	QuickMenu_QBox_Take( nType, fTime )
end

-- 탈것 장착
function CF_QuickMenu_EquipVehicle( bEquip, wMID, wSID, nFuel )	

	QuickMenu_ViewSet_Vehicle( bEquip, wMID, wSID, nFuel )
	QuickMenu_ViewSet()
	
end

-- 파티원간의 분배 가능한 아이템 드랍됨
function CF_QuickMenu_OpenPartyTenderNotifyButton(bNotify)
	QuickMenu_OpenPartyTenderNotifyButton(bNotify)
end

function CF_QuickMenu_SetWeaponBoostInfo(fDurabilityAlarmRemainPer, fDurability, fDurabilityMax)
	QuickMenu_SetWeaponBoostInfo(fDurabilityAlarmRemainPer, fDurability, fDurabilityMax)
end

-- 큐박스 리셋
function CF_QBox_Reset()
	QBox_Reset()
	QuickMenu_QBox_Reset()
end

-- 매크로 상태 갱신;
function CF_QuickMenu_MacroStateUpdate()
	-- QuickMenu, MacroOptionWindow Update
	QuickMenu_MacroViewSet()
	Mo_Update()
end

-- 매크로 시간 세팅;
function CF_QuickMenu_SetMacroTime(fTime)
	QuickMenu_SetMacroTime(fTime)
end

-- 귀환 메크로 버튼 초기화가
function CF_Mo_ResetReMainButtons()
	Mo_ResetReMainButtons()
end

-- 2차 비번 입력
function CF_SecondPassword_Input()
	SecondPassword_Open( 1 )
end

-- 2차 비번 신규 설정
function CF_SecondPassword_New()
	SecondPassword_Open( 2 )
end

-- 2차 비번 결과
function CF_SecondPassword_Result( nError )
	SecondPassword_Result( nError )
end

-- 버스 UI 오픈
function CF_BusStation_Open()
	OpenWidget( WIDGET_ID_BUS_STATION )
	
	
end

-- 플라잉 카메라 에러 메시지
function CF_FlyCamCtrl_MsgError( strError )
	FlyCamCtrl_MsgError( strError )
end

-- 플라잉 카메라 이동 속도 설정
function CF_FlyCamCtrl_SetMoveVelo( fVelo )
	FlyCamCtrl_SetMoveVelo( fVelo )
end

-- 플라잉 카메라 회전 속도 설정
function CF_FlyCamCtrl_SetRotateVelo( fVelo )
	FlyCamCtrl_SetRotateVelo( fVelo )
end

-- 플라잉 카메라 키프레임 추가
function CF_FlyCamCtrl_AddKeyFrame( tbKeyFrame )
	FlyCamCtrl_AddKeyFrame( tbKeyFrame )
end

-- 플라잉 카메라 키프레임 카메라 위치 가져오기
function CF_FlyCamCtrl_GetKeyFramePos()
	return FlyCamCtrl_GetKeyFramePos()
end

-- 플라잉 카메라 프로그래스바 업데이트
function CF_FlyCamCtrl_UpdateProgress( nCurrentKey )
	FlyCamCtrl_UpdateProgress( nCurrentKey )
end

-- 캐릭터 이름 변경
function CF_CharRename()
	GameMsg_CharRename()
end

-- 클럽 이름 변경
function CF_ClubRename()
	GameMsg_ClubRename()
end

-- 클럽 이름 업데이트
function CF_Club_UpdateName( strName )
	Club_UpdateName( strName )
end

function CF_PrivateMarket_MsgTitleFb( strTitle )
	PrivateMarket_MsgTitleFb( strTitle )
end

function CF_PrivateMarket_TitleMsgBox_Open()
	PrivateMarket_TitleMsgBox_Open()
end

function CF_PrivateMarket_Close()
	Ev_PrivateMarket_Close()
end

function CF_PrivateMarket_IsOpen()
	return PrivateMarket_IsOpen()
end

function CF_PrivateMarket_IsOpener()
	return PrivateMarket_IsOpener()
end


function CF_PrivateMarket_RegItem( bPile, nPosX, nPosY )
	PrivateMarketRegItemMsgBox_Init( bPile, nPosX, nPosY )
end

function CF_PrivateMarket_Update( tbItemList, bMe, nChaNum )

	PrivateMarket_Update( tbItemList, bMe, nChaNum )
	Inventory_DisplayPrivateMarketRegItem( false )
	
end

function CF_PrivateMarket_MsgOpenFb()
	PrivateMarket_MsgOpenFb()
end

function CF_PrivateMarket_ReqPMInfo( strName, strMarket, nChaNum )
	PrivateMarket_ReqPMInfo( strName, strMarket, nChaNum )
end

function CF_PrivateMarket_ReqBuy( nPosX, nPosY, strItem )
	PrivateMarket_ReqBuy( nPosX, nPosY, strItem )
end

function CF_PrivateMarket_ReqBuyEx( nPosX, nPosY )
	PrivateMarket_ReqBuyEx( nPosX, nPosY )
end

function CF_PrivateMarket_CloseWidget()
	CloseWidget( WIDGET_ID_PRIVATE_MARKET )
end

-- 락커 열려있나?
function CF_Locker_IsOpen()
	return IsOpenWidget( WIDGET_ID_LOCKER )
end

-- 락커 연결 카드 사용 가능
function CF_Locker_CanUseOpenCard( nPosX, nPosY )
	Locker_CanUseOpenCard( nPosX, nPosY )
end

-- 락커 돈 업데이트
function CF_Locker_UpdateMoney( nMoney )
	Locker_UpdateMoney( nMoney )
end

-- 락커 UI 열기
function CF_Locker_Open( dwNpcID )
	Locker_OpenFromNPC( dwNpcID )
end

-- 락커 아이템 업데이트
function CF_Locker_UpdateItem( nIndex )
	Locker_UpdateItem( nIndex )
end

-- 락커 확장 열기
function CF_LockerExpand_Open( wPosX, wPosY )
	LockerExpand_Open( wPosX, wPosY )
end

-- 락커 연결 카드 사용
function CF_Locker_UseOpenCard()
	CloseAllNpc()
	Locker_UseOpenCard()
end

-- 락커 확장 성공, 락커 업데이트
function CF_Locker_Update()
	Locker_Update()
end

-- 프리미엄 상태 변경됨
function CF_UpdatePremium()
	Locker_Premium_Use()
end

-- 아이템 박스 사용
function CF_UseItemBox( wPosX, wPosY, strItemName )
	GameMsg_UseItemBox( wPosX, wPosY, strItemName )
end

-- 일시강화
function CF_TempEnchant( wPosX, wPosY )
	GameMsg_TempEnchant( wPosX, wPosY )
end

-- 아이템 포인트 카드 사용
function CF_UseItemPointCard( wPosX, wPosY, strItemName )
	GameMsg_UseItemPointCard( wPosX, wPosY, strItemName )
end

-- 제작 재료 아이템(랜덤옵션이 있는) 확인
function CF_ProductNeedConfirm(strItemName, nCount, fDelay)
	SetProductNeedConfirm(strItemName, nCount, fDelay)
	OpenWidget(WIDGET_ID_PRODUCTCONFIRM)
end

-- 대련 디스플레이
function CF_ConftDisplay( nIndex )
	ConftDisplay( nIndex )
end

-- 수수료 설정 메시지
function CF_SetCommission()
	GameMsg_SetCommission()
end

-- NPC 경험치 회복
function CF_RecoveryNpcExp( dwNpcGlobalID, llReExp, llDecMoney )
	GameMsg_RecoveryNpcExp( dwNpcGlobalID, llReExp, llDecMoney )
end

-- 맵 진입 조건 불일치
function CF_RegenEntryFail()
	GameMsg_RegenEntryFail()
end

-- 보너스 타임 시작
function CF_BonusTime_StartEvent( bEventBuster, nTime, nRemainTime, fExpGain, fItemGain )
	BonusTime_StartEvent( bEventBuster, nTime, nRemainTime, fExpGain, fItemGain )
end

-- 보너스 타임 끝
function CF_BonusTime_EndEvent()
	CloseWidget( WIDGET_ID_BONUSTIME )
end

-- 탈것 연료 업데이트
function CF_Vehicle_UpdateFuel( nFuel )
	Vehicle_SetFuel( nFuel )
	QuickMenu_vehicle_Fuel_Gauge()
end

-- 탈것 슬롯 바인딩 ID -> 파츠 타입
function CF_Vehicle_Binding2Parts( strBinding )
	return Vehicle_Binding2Parts( strBinding )
end

-- 탈것 슬롯 업데이트
function CF_Vehicle_Update()
	Vehicle_Update()
end

-- 탈것 장착
function CF_Vehicle_Equip()

	Vehicle_Equip()
	
end

-- 탈것 UI 열려 있나?
function CF_Vehicle_IsOpen()
	return IsOpenWidget( WIDGET_ID_VEHICLE )
end

-- 탈것 부품 슬롯 하이라이트
function CF_Vehicle_SlotHighlight( nVehicleType, nPartsType )
	Vehicle_SlotHighlight( nVehicleType, nPartsType )
end

-- 탈것에 탑승되면 콜됨
function CF_RideVehicle( bHost, bBooster )

	CF_QuickMenu_RideVehicle( bHost )
	
	if true == bHost then
	
		BoosterGauge_RideVehicle( bBooster )
		CharInfo_UpdateDetail()
		
	end
	
	InteractMenu_EnableBike( true )
	
	if false == bHost then
	
		g_fNotifyVehicle = 6
		InsertFrameMove( FrameMove_NotifyVehicle )
	
	end
	
end

-- 탈것에서 내리게되면 콜됨
function CF_GetoffVehicle( bHost )

	CF_QuickMenu_GetoffVehicle( bHost )
	BoosterGauge_RideVehicle( false )
	
	if true == bHost then
	
		CharInfo_UpdateDetail()
		
	end
	
	InteractMenu_EnableBike( false )
	RemoveFrameMove( FrameMove_NotifyVehicle )
	
end

function FrameMove_NotifyVehicle( dt )

	g_fNotifyVehicle = g_fNotifyVehicle + dt
	if 6 < g_fNotifyVehicle then
	
		g_fNotifyVehicle = 0
		CF_PrintMsg_SystemBox( GetGameInternalText( "BIKE_MESSAGE_PASSENGER", 0 ), TEXT_COLOR_RED )
	
	end

end

function CF_Skill_GetUsingPoint()
	return Skill_GetUsingPoint();
end

function CF_Skill_GetSkillUpMoney()
	return g_nSkill_SkillUpMoney;
end

-- 동승 요청한 유저에게서 답변이오면 콜됨
function CF_VehicleReqTargetAnswer( bAnswer, dwID )
	CF_QuickMenu_VehicleReqTargetAnswer( bAnswer )
end
-- 깃발전/토너먼트 현황판 UI 오픈
function CF_CTFlag_Open( bFlag )

	if true == bFlag then
	
		OpenWidget( WIDGET_ID_CTFLAG_DASHBOARD )
	
	else
	
		CloseCTFlagDashboard()
		CloseWidget( WIDGET_ID_CTFLAG_DASHBOARD )
	
	end
	
end

-- 깃발전/토너먼트 타이머 설정
function CF_CTFlag_SetTime( fTime )
	CTFlag_SetTime( fTime )
end

-- 깃발전/토너먼트 이름 설정
function CF_CTFlag_SetName( bLeft, strName, dwColor )
	CTFlag_SetName( bLeft, strName, dwColor )
end

-- 깃발전/토너먼트 점수 설정
function CF_CTFlag_SetScore( bLeft, nScore, dwColor )
	CTFlag_SetScore( bLeft, nScore, dwColor )
end

-- 깃발전/토너먼트 아이콘 설정
function CF_CTFlag_SetIcon( bLeft, strXMLKey )
end

-- 깃발전/토너먼트 이미지 설정
function CF_CTFlag_SetImage( bLeft, strXMLKey, bBlink )
	CTFlag_SetImage( bLeft, bBlink )
end

-- 깃발전/토너먼트 팀 오브젝트 텍스트 설정
function CF_CTFlag_SetObjectText( strTeamObjectText, dwColor )
	CTFlag_SetObjectText( strTeamObjectText, dwColor )
end

-- 세트 아이템 미리보기 오픈
function CF_SetItem_Open( strSetItemName, nWoreCount, nTotal, tbItemList )
	SetItem_Open( strSetItemName, nWoreCount, nTotal, tbItemList )
end

-- 국가 선택 업데이트
function CF_UpdateSelectCountry( wContinentID, wCountryID )
	CountrySelect_Update( wCountryID )
end

-- 디스플레이 국가 이미지 프레임 ID
function CF_Display_GetCountryFrameID( dwCountryID )
	return Display_GetCountryFrameID( dwCountryID )
end

-- 난사군도 국가 순위 업데이트
function CF_CTI_UpdateRank( tbRankList, nType )
	CountryRanking_Update( tbRankList, nType )
end

-- 난사군도 국가 내 순위 업데이트
function CF_CTI_UpdateMyRank( nRank, nPoint )
	CountryRanking_UpdateMy( nRank, nPoint )
end

-- 난사군도 승전국 업데이트
function CF_CTI_UpdateVictoriousCountry( wCountryID, strCountryName )
	Minimap_SetVictoriousCountry( wCountryID, strCountryName )
	Minimpa_UpdateVictoryIconPosInfo()
end

-- 난사군도 진행 정보 업데이트
function CF_CTI_UpdateCountryProgressInfo( nXMLIndex, nProgressIndex, nScore, fTime, nType )
	CountryProgressInfo_Update( nXMLIndex, nProgressIndex, nScore, fTime, nType )
end

-- 난사군도 인증기
function CF_CTI_Auth( bVisible, fTime )
	CountryAuthProgress_Open( bVisible, fTime )
end

-- 난사군도 전장 서버 입장 실패
function CF_CTI_BattleServerEntranceFail( nError )
	CountryServerEntrance_FailEnter( nError )
end

-- 난사군도 전장 서버 입장 대기
function CF_CTI_BattleServerEnterWait()
	CountryServerEntrance_EnterWait()
end

-- 난사군도 결과 리스트 타이틀 데이터
function CF_CTI_SetRewardTitle( tbData )
	CountryBattleReward_RecvTitle( tbData )
end

-- 난사군도 결과 리스트 데이터 추가
function CF_CTI_AddRewardData( tbData )
	CountryBattleReward_RecvList( tbData )
end

-- 난사군도 결과 리스트 오픈
function CF_CTI_OpenReward( fOutTime )
	CountryBattleReward_Open( fOutTime )
end

-- 국가 초기화 카드 사용
function CF_UseResetCountryCard( bIsNotSelected, wInvenPosX, wInvenPosY )
	GameMsg_UseResetCountryCard( bIsNotSelected, wInvenPosX, wInvenPosY )
end

-- 난사군도 미니맵 보급품, 인증기 위치
function CF_CTI_MinimapAuthPos( tbList )

	Minimap_SetCTIAuth( tbList )
	LargeMap_SetCTIAuth( tbList )

end

-- 선도전 미니맵 인증기 가속기 위치
function CF_Guidance_MinimapAuthPos( tbList )

	Minimap_SetGuidanceAuth( tbList )
	LargeMap_SetGuidanceAuth( tbList )

end

-- 코스튬 능력치 부여 사용 확인
function CF_CostumeStat_Use( nType, nInvenPosX, nInvenPosY )
	CostumeStat_Use( nType, nInvenPosX, nInvenPosY )
end

-- 코스튬 능력치 부여 관련 에러
function CF_CostumeStat_Error( nError )
	CostumeStat_Error( nError )
end

-- 코스튬 능력치 해제
function CF_CostumeStat_Release( wPosX, wPosY, wHoldPosX, wHoldPosY )
	CostumeStat_Release( wPosX, wPosY, wHoldPosX, wHoldPosY )
end

-- 아이템 수리 UI 오픈
function CF_ItemRepair_Open()
	OpenWidget( WIDGET_ID_ITEMREPAIR )
end

-- 아이템 수리 업데이트
function CF_ItemRepair_Update()
	ItemRepair_Update()
end

-- 부스터 상태
function CF_BoosterGauge_State( nState )
	BoosterGauge_State( nState )
end

-- 부스터 배움
function CF_BoosterGauge_Learn( bRide )
	BoosterGauge_Learn( bRide )
end

function CF_SetVisibleSummonState(bVisible, bHire)
	SummonState_SetVisible(bVisible, bHire)
end

function CF_UpdateSummonAction( dwAction, bForceUpdate )
	SummonState_UpdateSummonAction( dwAction, bForceUpdate )
end

function CF_PDD_Update()
	PDD_Update()
end

function CF_ClosePartyDistributionUI()
	ClosePartyDistributionUI()
end

function CF_OpenForceResurrection(fTime)
	OpenForceResurrection_SetTime(fTime)
end

function CF_GetDurabilityTooltipHeader( tbItemData )
	return GetItemHeadDurability( tbItemData )
end

function CF_GetDurabilityTooltipInfo( tbItemData )
	return GetItemDurability( tbItemData )
end

function CF_CharInfo_Update()
	if true == IsLoadWidget( WIDGET_ID_CHARINFO ) then
		CharInfo_Update()
	end
end

-- 헤러 컬러 변경 카드 사용
function CF_HairColor_Use()

	HairColor_ItemUse()

end

-- 경고 이미지 켜기/끄기
function CF_ShowWarning( bShow )
	Warning_Show( bShow )
end

-- 깃발전 결과 타이틀 정보 설정
function CF_CTFlagResult_SetTitle( tbTitle )
	CTFlagResult_SetTitle( tbTitle )
end

-- 깃발전 결과 리스트 정보 추가
function CF_CTFlagResult_AddInfo( tbInfo )
	CTFlagResult_AddInfo( tbInfo )
end

-- 깃발전 결과 오픈
function CF_CTFlagResult_Open( nOutTime )
	CTFlagResult_Open( nOutTime )
end

-- 선도전 결과 타이틀 정보 설정
function CF_GuidanceResult_SetTitle( tbTitle )
	GuidanceResult_SetTitle( tbTitle )
end

-- 선도전 결과 리스트 정보 추가
function CF_GuidanceResult_AddInfo( tbInfo )
	GuidanceResult_AddInfo( tbInfo )
end

-- 선도전 결과 오픈
function CF_GuidanceResult_Open( nOutTime )
	GuidanceResult_Open( nOutTime )
end

-- 기능 연장
function CF_PeriodExConfirm(wPE_HoldPosX, wPE_HoldPosY, wPE_InvenPosX, wPE_InvenPosY, strText)
	PeriodExConfirm(wPE_HoldPosX, wPE_HoldPosY, wPE_InvenPosX, wPE_InvenPosY, strText)
end

-- 재확인 메세지 박스 출력;
function CF_PeriodExReconfirmOpenMsgBox(wPE_HoldPosX, wPE_HoldPosY, wPE_InvenPosX, wPE_InvenPosY)
	PeriodExReconfirmOpenMsgBox( wPE_HoldPosX, wPE_HoldPosY, wPE_InvenPosX, wPE_InvenPosY )
end

-- 학원 변경 메시지
function CF_SchoolChangeOK()
	GameMsg_SchoolChangeOK()
end

-- HTML 뷰 위치, 크기
function CF_GetHTMLViewSize()

	local tbSize = {}
	
	local tb = Help_GetHTMLViewSize()
	if nil ~= tb then
		table.insert( tbSize, tb )
	end
	
	return tbSize

end

-- 출석부 과제 완료 디스플레이 켜기
function CF_AttendanceComplete_Show( strInfo )
	AttendanceComplete_Show( strInfo )
end

-- 인던 나가기
function CF_DungeonExit_Show( nOutTime )
	DungeonExit_Show( nOutTime )
end

-- 포장 아이템 풀기
function CF_UnwrapItem( strItemName, wPosX, wPosY )
	GameMsg_UnwrapItem( strItemName, wPosX, wPosY )
end

-- 펫 스킨 카드 사용
function CF_UsePetSkinCard( dwItemID, wPosX, wPosY, wHoldPosX, wHoldPosY )
	PetSkin_UseCard( dwItemID, wPosX, wPosY, wHoldPosX, wHoldPosY )
end

-- 인벤토리 아이템 위치 변경
function CF_SwapInvenItemPos( nPosX1, nPosY1, nPosX2, nPosY2 )

	Locker_SwapItem( nPosX1, nPosY1, nPosX2, nPosY2 )

end

-- 코스튬 씌울 경우 기간 연장 메시지
function CF_CostumeDisguiseTimeCheck( nPosX, nPosY, nHoldPosX, nHoldPosY )
	GameMsg_CostumeDisguiseTimeCheck( nPosX, nPosY, nHoldPosX, nHoldPosY )
end

function CF_SelformBox_SetBoxID( nMid, nSid )
	SelformBox_SetBoxID( nMid, nSid )
end

-- 경험치 캡슐 아이템 사용
function CF_UseExpCapsuleItem ( wInvenPositionX, wInvenPositionY )

	GameMsg_UseExpCapsuleItem( wInvenPositionX, wInvenPositionY )
	
end

-- 경험치 압축 아이템 사용
function CF_UseExpCompressorItem ( wInvenPositionX, wInvenPositionY )

	GameMsg_UseExpCompressorItem( wInvenPositionX, wInvenPositionY )
	
end

-- 캐릭터 슬롯 확장 아이템 사용
function CF_UseUnlockCharacterSlotItem ( wInvenPositionX, wInvenPositionY )

	GameMsg_UseUnlockCharacterSlotItem( wInvenPositionX, wInvenPositionY )

end

-- 캐릭터 데이터 수신 완료 대기를 위한 메세지박스 비활성화
-- 로비스테이지에서 메세지박스는 오직 하나만 활성화될 수 있다;
function CF_LobbyWaitMsgBoxClose()
	Event_MsgBox_Close()
end

-- UI TimerMessageBox
function CF_UITimerMessageBox ( _bShow, _strContents, _nTime, _tableData )

	GameMsg_UITimerMessageBox( _bShow, _strContents, _nTime, _tableData )
	
end

-- 란 모바일 계정 연동 결과
function CF_RanMobile_MsgRecognize( nResult )
	RanMobile_MsgRecognize( nResult )
end

-- 란 모바일 계정 정보
function CF_RanMobile_MsgInfo( nResult )
	RanMobile_MsgInfo( nResult )
end

-- 란 모바일 계정 정보
function CF_RanMobile_MsgInfoAll( nResult )
	CharSelect_RecvRanMobileInfo()
end

-- 란 모바일 M포인트 요청 결과
function CF_RanMobile_MsgGetPoint( nResult )
	RanMobile_MsgGetPoint( nResult )
end

-- 란 모바일 시간 연장 결과
function CF_RanMobile_MsgExtendTime( nResult )
	RanMobile_MsgExtendTime( nResult )
end

-- 란 모바일 보상 교환 결과
function CF_RanMobile_MsgChangeItem( nResult )
	RanMobile_MsgChangeItem( nResult )
end

-- 란 모바일 보석 전송 결과
function CF_RanMobile_MsgSendJewel( nResult )
	RanMobile_MsgSendJewel( nResult )
end

-- 란 모바일 일일 보상 결과
function CF_RanMobile_MsgDailyReward( nResult )
	RanMobile_MsgDailyReward( nResult )
end

-- 란 모바일 연동권 사용
function CF_RanMobile_UseRecognizeItem()
	RanMobile_UseRecognizeItem()
end

-- 캐릭터가 피해를 받을 경우 호출된다
function CF_ReceiveDamage_Character ()

	-- String ProgressBar 가 캐릭터슬롯 변경 타입일 경우에 처리한다
	if STRING_PROGRESSBAR_TYPE_CHANGE_CHARSLOT == g_STRING_PROGRESS_BAR.GetUIType() then
	
		-- String ProgressBar 를 초기화한다
		g_STRING_PROGRESS_BAR.Reset()
	
		-- 캐릭터 정보를 갱신한다
		CharInfo_Update()
		
		-- 메시지를 출력한다
		CF_PrintMsg_SystemMsg(
			GetGameInternalText( "MSG_CHARSLOT_CHANGE_RECEIVEDAMAGE", 0 ),
			TEXT_COLOR_RED )
		
	end
	
end

----------------------------------------------------------------------------------------------------------
-- 단축키

-- UI 하나씩 닫기
function CF_HotKey_EscWidgetClose()
	return EscWidgetClose()
end

-- 채팅 매크로 단축키 입력
function CF_HotKey_AddChatMacro( strText )
	BasicChat_AddChatMacro( strText )
end

-- 인벤토리 창 열기
function CF_HotKey_Inventory()
	Inventory_HotKeyInven()
end

-- 은행 창 열기
function CF_HotKey_Bank()
	Inventory_HotKeyBank()
end

-- 캐릭터 창 열기
function CF_HotKey_Character()
	ToggleUI( WIDGET_ID_CHARINFO )
end

-- 탈것 창 열기
function CF_HotKey_Vehicle()
	HotkeyVehicle()
end

-- 스킬 창 열기
function CF_HotKey_Skill()
	ToggleUI( WIDGET_ID_SKILL )
end

-- 채팅 매크로 창 열기
function CF_HotKey_ChatMacro()
	ToggleUI( WIDGET_ID_CHATMACRO )
end

-- 스킬 퀵슬롯 그룹
function CF_HotKey_SQSGroup( nIndex )
	SQS_SetGroup( nIndex + 1 )
end

-- 스킬 퀵슬롯 트레이
function CF_HotKey_SQSTray()

	if 1 == g_InputMan:GetCurrentControlType() then
	
		if  CHARCLASS_IDX_EXTREAM_M == g_MyChar.nClassIndex or
			CHARCLASS_IDX_EXTREAM_W ==  g_MyChar.nClassIndex then
			return
		end
	
	end

	Ev_SQS_Collapse()
	
end

-- 파티 창 열기
function CF_HotKey_Party()
	ToggleUI( WIDGET_ID_PARTYINFO )
end

-- 펫 창 열기
function CF_HotKey_Pet()

	-- 펫이 소환된 상태가 아니면 열지 않는다.
	if false == g_Pet:IsVALID() then
		return
	end

	ToggleUI( WIDGET_ID_PET )
	
end

-- 제작 창 열기
function CF_HotKey_Product()
	ToggleUI( WIDGET_ID_PRODUCT )
end

-- 포인트샾 창 열기
function CF_HotKey_PointShop()
	ToggleUI( WIDGET_ID_POINTSHOP )
end

--개인상점검색 창 열기
function CF_HotKey_PrivateMarketSearch()
	ToggleUI( WIDGET_ID_PRIVATE_MARKET_SEARCH )
end

-- 클럽창 열기
function CF_HotKey_Club()
	ToggleUI( WIDGET_ID_CLUB )
end

-- 시스템 메뉴창
function CF_HotKey_System()
	ToggleUI( WIDGET_ID_ESC )
end

-- 메시지 박스 끄기
function CF_HotKey_CancelMsgBox()

	if true == ReportMsgBox_ESC() then
		return true
	end

	-- 메시지 박스 닫기
	if true == MsgBox_HotKey_ESC() then
		return true
	end
	
	return false

end

-- 엔터키 확인 (메시지박스 확인, 채팅창 포커스 등)
function CF_HotKey_Confirm()

	-- 메시지 박스 확인
	if true == MsgBox_HotKey_Enter() then
		return
	end
	
	if true == ReportMsgBox_Enter() then
		return
	end
	
	-- 개조카드 엔터키를 이용하여 사용
	if true == Ev_RebuildCard_KeyUp() then
		return
	end
		
	-- 그룹 채팅 포커스
	if true == GroupChat_SetFocus() then
		return
	end

	-- 채팅창 포커스는 우선순위가 제일 낮다. 이후 추가되는건 위에서 검사한다.
	if true == IsOpenWidget( WIDGET_ID_BASICCHAT ) then
		BasicChat_SetFocus()
	end

end

-- 전체맵
function CF_HotKey_LargeMap()
	ToggleUI( WIDGET_ID_LARGEMAP )
end

--전장 UI
function CF_HotKey_Competition()
	ToggleUI( WIDGET_ID_COMPETITION )
end

function CF_HotKey_Community()
	ToggleUI( WIDGET_ID_COMMUNITY )
end

-- DPS
function CF_HotKey_DPS()
	ToggleUI( WIDGET_ID_DPS )
end

-- 플라잉 카메라
function CF_HotKey_FlyCamCtrl()
	ToggleUI( WIDGET_ID_FLYCAMCTRL )
end

-- 생활 기록부
function CF_HotKey_StudentRecord()
	ToggleUI( WIDGET_ID_STUDENT_RECORD )
end

-- 도움말
function CF_HotKey_Help()
	ToggleUI( WIDGET_ID_HELP )
end

-- 퀘스트 
function CF_HotKey_Quest()
	ToggleUI( WIDGET_ID_QUEST )
end

-- 큐박스
function CF_HotKey_QBox()
	QuickMenu_ToggleQBox()
end

-- 단축키 관련 함수 끝
----------------------------------------------------------------------------------------------------------
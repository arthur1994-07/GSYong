
-- 아이템 버리기 메시지 박스
function GameMsg_HoldItemToField( nHoldSlotPlace, wSlotPosX, wSlotPosY, fPosX, fPosY, fPosZ )

	local tbData = 
	{
		nHoldSlotPlace,
		wSlotPosX, 
		wSlotPosY, 
		fPosX, 
		fPosY, 
		fPosZ
	}
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText( "MODAL_REQ_HOLD_TO_FILED", 0 ),
		Ev_GameMsg_HoldItemToField, Event_MsgBox_Close, tbData )

end

-- 아이템 버리기 메시지 박스 이벤트
function Ev_GameMsg_HoldItemToField( tbData )

	local nIndex = Inventory_SlotPosToIndex( tbData[ 2 ], tbData[ 3 ] )
	local strSlotID = SLOT_INVENTORY_ID .. tostring( nIndex )

	g_Msg:HoldItemToField( strSlotID, tbData[ 1 ], tbData[ 2 ], tbData[ 3 ], 
		tbData[ 4 ], tbData[ 5 ], tbData[ 6 ] )
		
	Event_MsgBox_Close()
	
end

-- 겹침 아이템 분리 메시지 박스
function GameMsg_SeparateOverlapItem( nPosX, nPosY, wItemCount )

	local tbData = { nPosX, nPosY, wItemCount }
	
	OpenMsgBox( MSG_BOX_TYPE_INPUT, GetGameInternalText( "SPLIT_ITEM", 0 ),
		Ev_GameMsg_SeparateOverlapItem, Event_MsgBox_Close, tbData )
	SetString( WIDGET_ID_MESSAGEBOX, "Restrict", "number", "input" )

end

-- 겹침 아이템 분리 메시지 박스 이벤트
function Ev_GameMsg_SeparateOverlapItem( tbData )

	local nCount = tonumber( GetMsgBoxInput() )
	if nil == nCount then
		return
	end
	
	if tbData[ 3 ] <= nCount then
		return
	end
	
	g_Msg:ReqInvenSplit( tbData[ 1 ], tbData[ 2 ], nCount )
	Event_MsgBox_Close()
	
end

-- 중복 계정 로그인
function GameMsg_DuplicateLogin()

	OpenMsgBox( MSG_BOX_TYPE_OK, GetGameInternalText( "DUPLICATELOGIN_CLOSEGAME", 0 ), Event_MsgBox_CloseGame )

end

-- 오프라인 누적 시간 부족
function GameMsg_LoginGainTime()

	OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "CHINA_LOGIN_GAINTIME_ERROR", 0 ), Event_MsgBox_Close )

end

-- 캐릭터 이름 변경
function GameMsg_CharRename()

	OpenMsgBox( MSG_BOX_TYPE_INPUT, GetGameInternalText( "INVEN_CHAR_RENAME", 0 ), 
		Ev_GameMsg_CharRenameOK, Event_MsgBox_Close )
		
	SetInt( WIDGET_ID_MESSAGEBOX, "maxByte", 16, "msgBoxInput.input" )

end

-- 캐릭터 이름 변경 확인
function Ev_GameMsg_CharRenameOK()

	local strName = GetMsgBoxInput()
	
	if string.len(strName) > 16 then
		return ;
	end
	
	-- 서버로 이름 변경 요청
	g_Msg:ReqInvenRename( strName )

	Event_MsgBox_Close()

end

-- 클럽 이름 변경
function GameMsg_ClubRename()

	OpenMsgBox( MSG_BOX_TYPE_INPUT, GetGameInternalText( "INVEN_CLUB_RENAME", 0 ), 
		Ev_GameMsg_ClubRenameOK, Event_MsgBox_Close )

end

-- 클럽 이름 변경 확인
function Ev_GameMsg_ClubRenameOK()

	local strName = GetMsgBoxInput()
	-- 서버로 클럽 이름 변경 요청
	g_Msg:ReqInvenClubRename( strName )

	Event_MsgBox_Close()

end

-- 일시강화
function GameMsg_TempEnchant( wPosX, wPosY )

	local tbData = { wPosX, wPosY }

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText( "TEMP_ENCHANT_EXIST", 0 ), 
		Ev_GameMsg_TempEnchant, Event_MsgBox_Close, tbData )

end

-- 일시강화
function Ev_GameMsg_TempEnchant( tbData )
	
	g_Msg:ReqTLGrinding( tbData[ 1 ], tbData[ 2 ] )
	Event_MsgBox_Close()

end

-- 아이템 박스 사용
function GameMsg_UseItemBox( wPosX, wPosY, strItemName )

	if g_PrivateMarket:IsRegInven( wPosX, wPosY ) == true then
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMPMARKET_REGITEM_FB_REGITEM" ), TEXT_COLOR_RED )
		return
	end

	local str = string.format( "[%s] %s", strItemName, GetGameInternalText( "RNINVEN_ITEM_BOX_OPEN", 0 ) )
	local tbData = { wPosX, wPosY }

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_GameMsg_UseItemBoxPointCard, Event_MsgBox_Close, tbData )

end

-- 아이템 포인트 카드 사용
function GameMsg_UseItemPointCard( wPosX, wPosY, strItemName )

	local str = string.format( GetGameInternalText( "RNINVEN_ITEM_USE", 0 ), strItemName )
	local tbData = { wPosX, wPosY }

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_GameMsg_UseItemBoxPointCard, Event_MsgBox_Close, tbData )

end

-- 아이템 박스/포인트 카드 사용 확인
function Ev_GameMsg_UseItemBoxPointCard( tbData )

	g_Msg:ReqInvenDrug( tbData[ 1 ], tbData[ 2 ] )
	
	Event_MsgBox_Close()

end

-- 수수료 설정
function GameMsg_SetCommission()

	local str = GetGameInternalText( "MODAL_COMMISSION", 0 )
	
	OpenMsgBox( MSG_BOX_TYPE_INPUT_ONLYNUM, str, Ev_GameMsg_SetCommission, Event_MsgBox_Close )

end

-- 수수료 설정 확인
function Ev_GameMsg_SetCommission()

	local fRate = tonumber( GetMsgBoxInput() )
	
	g_Msg:ReqGuidCommission( fRate )
	Event_MsgBox_Close()

end

-- NPC 경험치 회복
function GameMsg_RecoveryNpcExp( dwNpcGlobalID, llReExp, llDecMoney )

	local str = StringFormat( GetGameInternalText( "MODAL_RECOVERY_NPC_EXP", 0 ), llReExp, llDecMoney )
	local tbData = { dwNpcGlobalID }

	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_GameMsg_RecoveryNpcExp, Event_MsgBox_Close, tbData )

end

-- NPC 경험치 회복 확인
function Ev_GameMsg_RecoveryNpcExp( tbData )

	g_Msg:ReqRecoveryExpNpc( tbData[ 1 ] )
	Event_MsgBox_Close()

end

-- 맵 진입 조건 불일치
function GameMsg_RegenEntryFail()

	OpenMsgBox( MSG_BOX_TYPE_OK, GetGameExternalText( "REGEN_ENTRY_FAIL", 0 ), Event_MsgBox_Close )

end

-- 버프 삭제 확인
function GameMsg_BuffDelete( dwSkillID, strSkillName, strBinding )

	local str = string.format( GetGameInternalText( "SKILLFACT_ASK_RESET", 0 ), strSkillName )
	
	local tbData = { dwSkillID, strBinding }
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_GameMsg_BuffDelete, Event_MsgBox_Close, tbData )

end

-- 버프 삭제 확인 OK
function Ev_GameMsg_BuffDelete( tbData )

	Event_MsgBox_Close()
	
	local nID = tonumber( strID )

	-- 일반 버프
	for i = 1, BUFF_NORMAL_SLOT_TOTAL, 1 do
	
		if tbData[ 2 ] == SLOT_BUFF_NORMAL_ID ..tostring( i ) then
		
			if nil ~= g_nBuffNormal_SlotPos[ i ] then
			
				g_Msg:ReqSkillHoldReset( tbData[ 1 ], g_nBuffNormal_SlotPos[ i ] )
			
			end
			
			return
			
		end
	
	end
	
	-- 추가 버프
	if tbData[ 2 ] == SLOT_BUFF_ADD_ID then
	
		g_Msg:ReqSkillHoldReset( tbData[ 1 ], g_nBuffAdd_SlotPos )
		return
		
	end
	
	-- 도시락 버프
	if tbData[ 2 ] == SLOT_BUFF_LUNCHBOX_ID then
		
		g_Msg:ReqSkillHoldReset( tbData[ 1 ], g_nBuffLunchBox_SlotPos )
		return
		
	end
	
	-- 경험치 도시락 버프
	if tbData[ 2 ] == SLOT_BUFF_EXP_LUNCHBOX_ID then
	
		g_Msg:ReqSkillHoldReset( tbData[ 1 ], g_nBuffExpLunchBox_SlotPos )
		return
		
	end
	
	-- 시스템 버프
	for i = 1, BUFF_SYSTEM_SLOT_TOTAL, 1 do
	
		if tbData[ 2 ] == SLOT_BUFF_SYSTEM_ID ..tostring( i ) then
			
			if nil ~= g_tbSystemBuff_SlotPos[ i ] then
			
				g_Msg:ReqSkillHoldReset( tbData[ 1 ], g_tbSystemBuff_SlotPos[ i ] )
			
			end

			return
			
		end
	
	end

end

-- 국가 초기화 카드 사용
function GameMsg_UseResetCountryCard( bIsNotSelected, wInvenPosX, wInvenPosY )

	-- 아직 선택된 국가 없음
	if true == bIsNotSelected then
	
		local str = GetGameInternalText( "USE_ITEM_COUNTRY_CHANGE_INFO", 0 )
		OpenMsgBox( MSG_BOX_TYPE_OK, str, Event_MsgBox_Close, Event_MsgBox_Close )
	
	-- 선택된 국가 있음, 초기화 여부 묻기
	else
	
		local str = GetGameInternalText( "CONFIRM_ITEM_COUNTRY_CHANGE_INFO", 0 )
		local tbData = { wInvenPosX, wInvenPosY }
		
		OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_GameMsg_UseResetCountryCardOK, Event_MsgBox_Close, tbData )
	
	end

end

-- 국가 초기화 카드 사용 확인
function Ev_GameMsg_UseResetCountryCardOK( tbData )

	Event_MsgBox_Close()
	
	g_Msg:ReqResetCountry( tbData[ 1 ], tbData[ 2 ] )

end

-- 학원 변경 메시지
function GameMsg_SchoolChangeOK()
	
	local str = GetGameInternalText( "MODAL_SCHOOL_CHANGE_END", 0 )
	
	OpenMsgBox( MSG_BOX_TYPE_OK, str, EscMenu_ServerSelectTimerMsg, EscMenu_ServerSelectTimerMsg )
	
end

-- 포장 아이템 풀기
function GameMsg_UnwrapItem( strItemName, wPosX, wPosY )

	if g_PrivateMarket:IsRegInven( wPosX, wPosY ) == true then
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMPMARKET_REGITEM_FB_REGITEM" ), TEXT_COLOR_RED )
		return
	end

	local tbData = { wPosX, wPosY }
	local str = string.format( "[%s] %s", strItemName, GetGameInternalText( "RNINVEN_ITEM_BOX_OPEN", 0 ) )
	
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_GameMsg_UnwrapItem, Event_MsgBox_Close, tbData )

end

-- 포장 아이템 풀기 확인
function Ev_GameMsg_UnwrapItem( tbData )

	g_Msg:ReqInvenDrug( tbData[ 1 ], tbData[ 2 ] )
	Event_MsgBox_Close()

end

-- 코스튬 씌울 경우 기간 연장 메시지
function GameMsg_CostumeDisguiseTimeCheck( nPosX, nPosY, nHoldPosX, nHoldPosY )

	-- 코스튬 아이템인지 검사 
	if g_MyChar:IsDisguiseInvenItem( nPosX, nPosY) == true then
		return 
	end

	local tbData = { nPosX, nPosY, nHoldPosX, nHoldPosY }
	local str = GetGameInternalText( "ITEM_PERIODEX_CHECK_EXPIRATIONDATE", 0 )
	
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_GameMsg_CostumeDisguiseTimeCheck, Event_MsgBox_Close, tbData )

end

-- 코스튬 씌울 경우 기간 연장 메시지 확인
function Ev_GameMsg_CostumeDisguiseTimeCheck( tbData )

	Event_MsgBox_Close()
	
	g_MyChar:UseDisguiseInvenItem( tbData[ 1 ], tbData[ 2 ], tbData[ 3 ], tbData[ 4 ] )

end

-- 경험치 캡슐 아이템 사용 확인 팝업
function GameMsg_UseExpCapsuleItem ( wInvenPosX, wInvenPosY )

	local str = GetGameInternalText( "EXP_ITEM_CAPSULE_USE", 0 )
	local tbData = { wInvenPosX, wInvenPosY }
		
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_GameMsg_OK_UseExpCapsuleItem, Event_MsgBox_Close, tbData )

end

function Ev_GameMsg_OK_UseExpCapsuleItem ( tbData )

	Event_MsgBox_Close()
	
	g_Msg:ReqUseExpCapsuleItem( tbData[ 1 ], tbData[ 2 ] )
	
end

-- 경험치 압축 아이템 사용 확인 팝업
function GameMsg_UseExpCompressorItem ( wInvenPosX, wInvenPosY )

	local str = GetGameInternalText( "EXP_ITEM_COMPRESSOR_USE", 0 )
	local tbData = { wInvenPosX, wInvenPosY }
		
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_GameMsg_OK_UseExpCompressorItem, Event_MsgBox_Close, tbData )

end

function Ev_GameMsg_OK_UseExpCompressorItem ( tbData )

	Event_MsgBox_Close()
	
	g_Msg:ReqUseExpCompressorItem( tbData[ 1 ], tbData[ 2 ] )
	
end

-- 캐릭터 슬롯 확장 아이템 사용 확인 팝업;
function GameMsg_UseUnlockCharacterSlotItem ( wInvenPosX, wInvenPosY )

	local str = GetGameInternalText( "TEXT_CONFIRM_UNLOCK_CHARACTER_SLOT_ITEM_MESSAGE", 0 )
	local tbData = { wInvenPosX, wInvenPosY }
		
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, str, Ev_GameMsg_OK_UseUnlockCharacterSlotItem, Event_MsgBox_Close, tbData )
	
end

function Ev_GameMsg_OK_UseUnlockCharacterSlotItem ( tbData )

	Event_MsgBox_Close()
	
	g_Msg:ReqUseUnlockCharacterSlotItem( tbData[ 1 ], tbData[ 2 ] )
	
end

-- TimerMessageBox ( Instance System )

function GameMsg_UITimerMessageBox ( _bShow, _strContents, _nTime, _tableData )

	if ( _bShow ) then
	
		OpenMsgBox (
			MSG_BOX_TYPE_OKCANCEL,
			_strContents,
			Ev_GameMsg_OK_UITimerMessageBox,
			Ev_GameMsg_Cancel_UITimerMessageBox,
			_tableData,
			_tableData,
			_nTime,
			Ev_GameMsg_TimerOver_UITimerMessageBox,
			_tableData )
		
	else
	
		Event_MsgBox_Close()
		
	end
	
end

function Ev_GameMsg_OK_UITimerMessageBox ( _tableData )

	g_Msg:SendCustomMessage( _tableData[ 1 ], _tableData[ 2 ], _tableData[ 3 ], _tableData[ 4 ], 0, 0 ) 
	
	Event_MsgBox_Close()
	
end

function Ev_GameMsg_Cancel_UITimerMessageBox ( _tableData )

	g_Msg:SendCustomMessage( _tableData[ 1 ], _tableData[ 2 ], _tableData[ 3 ], _tableData[ 4 ], 1, 0 ) 
	
	Event_MsgBox_Close()

end

function Ev_GameMsg_TimerOver_UITimerMessageBox ( _tableData )

	g_Msg:SendCustomMessage( _tableData[ 1 ], _tableData[ 2 ], _tableData[ 3 ], _tableData[ 4 ], 2, 0 ) 
	
	Event_MsgBox_Close()

end
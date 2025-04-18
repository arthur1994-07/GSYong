
g_nHSCC_Index = 0
g_nHSCC_MaxIndex = 0
g_nHSCC_Previous = 0

-- 머리 스타일 변경 카드 UI
function LoadHairStyleChangeCard()

	SetEvent( WIDGET_ID_CHANGE_CARD_STYLE_HAIR, EVENT_WIDGET_CLOSE, "CloseHairStyleChangeCard", "", "" )

	-- 왼쪽 버튼	
	SetEvent( WIDGET_ID_CHANGE_CARD_STYLE_HAIR, EVENT_BUTTON_CLICK, "Ev_HSCC_btnPrev", "", "btnPrev" )
	
	-- 오른쪽 버튼
	SetEvent( WIDGET_ID_CHANGE_CARD_STYLE_HAIR, EVENT_BUTTON_CLICK, "Ev_HSCC_btnNext", "", "btnNext" )
	
	-- 확인 버튼
	local btnOk = GetGameWordText( "MODAL_BUTTON", 0 )
	SetString( WIDGET_ID_CHANGE_CARD_STYLE_HAIR, "label", btnOk, "btnOk" )
	
	-- 이벤트
	SetEvent( WIDGET_ID_CHANGE_CARD_STYLE_HAIR, EVENT_BUTTON_CLICK, "Ev_HSCC_btnOK", "", "btnOk" )
	
	-- 취소 버튼
	local btnCancel = GetGameWordText( "MODAL_BUTTON", 1 )
	SetString( WIDGET_ID_CHANGE_CARD_STYLE_HAIR, "label", btnCancel, "btnCancel" )
	-- 이벤트
	SetEvent( WIDGET_ID_CHANGE_CARD_STYLE_HAIR, EVENT_BUTTON_CLICK, "Ev_HSCC_btnCancel", "", "btnCancel" )

end

-- 헤어 스타일 변경창 오픈
function OpenHairStyleChangeCard()

	g_nHSCC_Index = 0
	g_nHSCC_MaxIndex = 0

	-- 캐릭터 클래스, 캐릭터 성별, 캐릭터 머리스타일 불러오기
	HSCC_InitData()
	
	SetBool( WIDGET_ID_CHANGE_CARD_STYLE_HAIR, "visible", false, "faceMan" )
	SetBool( WIDGET_ID_CHANGE_CARD_STYLE_HAIR, "visible", false, "faceWoman" )
	
	HSCC_UpdateImg()
	HSCC_UpdateText()

end

function CloseHairStyleChangeCard()

	g_MyChar:HairStyleChange( g_nHSCC_Previous )

end

-- UI 열기 - 머리 스타일 체인지 카드 아이템 사용
function HSCC_UseItem()

	-- UI 가 닫혀있으면은 UI를 연다
	if false == IsOpenWidget( WIDGET_ID_CHANGE_CARD_STYLE_HAIR ) then
		OpenWidget( WIDGET_ID_CHANGE_CARD_STYLE_HAIR )
	end

end

-- 캐릭터 머리 스타일 데이터 초기화
-- 캐릭터 클래스, 캐릭터 성별, 캐릭터 머리 스타일 불러오기
function HSCC_InitData()

	local emClass = g_MyChar.m_emClass
	local UserClassType = GetCharClassFlagIndex( emClass )

	-- 모든 캐릭터 클래스 별 상수 머리스타일 번호 적용
	g_nHSCC_MaxIndex = FSCC_ClassToHairMaxIndex( UserClassType )

	g_nHSCC_Index = g_MyChar.m_wHair
	g_nHSCC_Previous = g_nHSCC_Index

end

-- 모든 캐릭터 클래스 별 상수 머리 스타일 번호 읽어오기
function FSCC_ClassToHairMaxIndex( UserClassType )
	
	if CHARCLASS_IDX_FIGHTER_M == UserClassType then		-- 0
		return g_CONST_CHARCLASS0.dwHAIRNUM
	elseif CHARCLASS_IDX_ARMS_M == UserClassType then		-- 1
		return g_CONST_CHARCLASS1.dwHAIRNUM
	elseif CHARCLASS_IDX_ARCHER_W == UserClassType then		-- 2
		return g_CONST_CHARCLASS2.dwHAIRNUM
	elseif CHARCLASS_IDX_SPIRIT_W == UserClassType then		-- 3
		return g_CONST_CHARCLASS3.dwHAIRNUM
	elseif CHARCLASS_IDX_EXTREAM_M == UserClassType then	-- 4
		return g_CONST_CHARCLASS4.dwHAIRNUM
	elseif CHARCLASS_IDX_EXTREAM_W == UserClassType then	-- 5
		return g_CONST_CHARCLASS5.dwHAIRNUM
	elseif CHARCLASS_IDX_FIGHTER_W == UserClassType then	-- 6
		return g_CONST_CHARCLASS6.dwHAIRNUM
	elseif CHARCLASS_IDX_ARMS_W == UserClassType then		-- 7
		return g_CONST_CHARCLASS7.dwHAIRNUM
	elseif CHARCLASS_IDX_ARCHER_M == UserClassType then		-- 8
		return g_CONST_CHARCLASS8.dwHAIRNUM
	elseif CHARCLASS_IDX_SPIRIT_M == UserClassType then		-- 9
		return g_CONST_CHARCLASS9.dwHAIRNUM
	elseif CHARCLASS_IDX_SCIENTIST_M == UserClassType then	-- 10
		return g_CONST_CHARCLASS10.dwHAIRNUM
	elseif CHARCLASS_IDX_SCIENTIST_W == UserClassType then	-- 11
		return g_CONST_CHARCLASS11.dwHAIRNUM
	elseif CHARCLASS_IDX_ASSASSIN_M == UserClassType then	-- 12
		return g_CONST_CHARCLASS12.dwHAIRNUM
	elseif CHARCLASS_IDX_ASSASSIN_W == UserClassType then	-- 13
		return g_CONST_CHARCLASS13.dwHAIRNUM
	elseif CHARCLASS_IDX_TRICKER_M == UserClassType then	-- 14
		return g_CONST_CHARCLASS14.dwHAIRNUM
	elseif CHARCLASS_IDX_TRICKER_W == UserClassType then	-- 15
		return g_CONST_CHARCLASS15.dwHAIRNUM
	elseif CHARCLASS_IDX_ETC_M == UserClassType then		-- 16
		return g_CONST_CHARCLASS16.dwHAIRNUM
	elseif CHARCLASS_IDX_ETC_W == UserClassType then		-- 17
		return g_CONST_CHARCLASS17.dwHAIRNUM
	elseif CHARCLASS_IDX_ACTOR_M == UserClassType then		-- 16
		return g_CONST_CHARCLASS18.dwHAIRNUM
	elseif CHARCLASS_IDX_ACTOR_W == UserClassType then		-- 17
		return g_CONST_CHARCLASS19.dwHAIRNUM
	elseif CHARCLASS_IDX_NUM_ETC == UserClassType then		-- 18
		return g_CONST_CHARCLASS20.dwHAIRNUM
	end

end

-- 머리 스타일 데이터 클릭시 텍스트 업데이트
function HSCC_UpdateText()
		
	local strHairName = GetGameInternalText( "HAIRSTYLE_NAME", g_nHSCC_Index )
	SetString( WIDGET_ID_CHANGE_CARD_STYLE_HAIR, "text", strHairName, "labelStyleName" )
	
end

-- 성공 / 실패 메세지 출력
function HSCC_UseResult( emFB, dwID )

	-- 일반오류.
	if 0 == emFB then
	
		g_MyChar:HairStyleChange( g_nHSCC_Index )
		
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMINVEN_HAIR_CHANGE_FB_FAIL", 0 ), TEXT_COLOR_RED )
		
	-- 성공
	elseif 1 == emFB then
	
		g_nHSCC_Index = dwID
		g_MyChar.m_wHair = dwID
		g_MyChar:UpdateSuit( 1, 0 )
		
		-- 머리 스타일 변경
		g_MyChar:HairStyleChange( g_nHSCC_Index )
		
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMINVEN_HAIR_CHANGE_FB_OK", 0 ), TEXT_COLOR_PALEGREEN )
		
	-- 아이템이 없음
	elseif 2 == emFB then
	
		g_MyChar:HairStyleChange( g_nHSCC_Index )
		
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMINVEN_HAIR_CHANGE_FB_NOITEM", 0 ), TEXT_COLOR_RED )
		
	-- 사용 불가능 아이템
	elseif 3 == emFB then
	
		g_MyChar:HairStyleChange( g_nHSCC_Index )
		
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMINVEN_HAIR_CHANGE_FB_BADITEM", 0 ), TEXT_COLOR_RED )
		
	-- 캐릭터 직업이 틀려서 사용 불가능 아이템
	elseif 4 == emFB then
	
		g_MyChar:HairStyleChange( g_nHSCC_Index )
		
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMINVEN_HAIR_CHANGE_FB_BADCLASS", 0 ), TEXT_COLOR_RED )
		
	-- 쿨타임 실패
	elseif 5 == emFB then
	
		g_MyChar:HairStyleChange( g_nHSCC_Index )

	end
	
	CloseWidget( WIDGET_ID_CHANGE_CARD_STYLE_HAIR )

end

-- 헤어 이미지 변경
function HSCC_UpdateImg()

	local strInst = ""
	if 0 == g_MyChar.m_wSex then
	
		strInst = "hairWoman"		
		SetBool( WIDGET_ID_CHANGE_CARD_STYLE_HAIR, "visible", false, "hairMan" )
		
	else
	
		strInst = "hairMan"
		SetBool( WIDGET_ID_CHANGE_CARD_STYLE_HAIR, "visible", false, "hairWoman" )
		
	end
	
	SetBool( WIDGET_ID_CHANGE_CARD_STYLE_HAIR, "visible", true, strInst )
	CallFunction( WIDGET_ID_CHANGE_CARD_STYLE_HAIR, "gotoAndStop", tostring( g_nHSCC_Index + 1 ), strInst )
	
	g_MyChar:HairStyleChange( g_nHSCC_Index )

end

--------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 왼쪽 버튼 이벤트
function Ev_HSCC_btnPrev()
		
	g_nHSCC_Index = g_nHSCC_Index - 1	
	
	if	g_nHSCC_Index < 0 then
		g_nHSCC_Index = 0
	end
	
	HSCC_UpdateImg()
	HSCC_UpdateText()

end

-- 오른쪽 버튼 이벤트
function Ev_HSCC_btnNext()

	g_nHSCC_Index = g_nHSCC_Index + 1

	if	g_nHSCC_Index >= g_nHSCC_MaxIndex then
		g_nHSCC_Index = g_nHSCC_MaxIndex - 1
	end
	
	HSCC_UpdateImg()
	HSCC_UpdateText()

end

-- 확인 버튼 이벤트
function Ev_HSCC_btnOK()
	
	g_Msg:ReqInvenHairStyleChange( g_nHSCC_Index )
	
	-- UI 닫기
	CloseWidget( WIDGET_ID_CHANGE_CARD_STYLE_HAIR )

end

-- 취소 버튼 이벤트
function Ev_HSCC_btnCancel()
	
	-- UI 닫기
	CloseWidget( WIDGET_ID_CHANGE_CARD_STYLE_HAIR )

end
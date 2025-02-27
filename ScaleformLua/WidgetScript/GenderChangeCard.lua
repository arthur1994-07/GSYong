
-- 머리 스타일
g_nGCC_HairIndex = 0
g_nGCC_HairMaxIndex = 0
g_nGCC_HairPrevious = 0

-- 얼굴 스타일
g_nGCC_FaceIndex = 0
g_nGCC_FaceMaxIndex = 0
g_nGCC_FacePrevious = 0

g_nGCC_SEX_W = 0
g_nGCC_SEX_M = 1

-- 성별 전환 카드 - 확인 클릭시 팝업 UI
function LoadGenderChangeCard()

	-- 머리 버튼
	SetEvent( WIDGET_ID_CHANGE_CARD_GENDER, EVENT_BUTTON_CLICK, "Ev_GCC_btnHairPrev", "", "btnHairPrev" )
	SetEvent( WIDGET_ID_CHANGE_CARD_GENDER, EVENT_BUTTON_CLICK, "Ev_GCC_btnHairNext", "", "btnHairNext" )
	
	-- 얼굴 버튼
	SetEvent( WIDGET_ID_CHANGE_CARD_GENDER, EVENT_BUTTON_CLICK, "Ev_GCC_btnFacePrev", "", "btnFacePrev" )
	SetEvent( WIDGET_ID_CHANGE_CARD_GENDER, EVENT_BUTTON_CLICK, "Ev_GCC_btnFaceNext", "", "btnFaceNext" )
	
	-- 확인 버튼		HAIRSTYLECARD_OK.
	local btnOk = GetGameWordText( "MODAL_BUTTON", 0 )
	SetString( WIDGET_ID_CHANGE_CARD_GENDER, "label", btnOk, "btnOk" )	
	SetEvent( WIDGET_ID_CHANGE_CARD_GENDER, EVENT_BUTTON_CLICK, "Ev_GCC_btnOK", "", "btnOk" )
	
	-- 취소 버튼		HAIRSTYLECARD_CANCEL
	local btnCancel = GetGameWordText( "MODAL_BUTTON", 1 )
	SetString( WIDGET_ID_CHANGE_CARD_GENDER, "label", btnCancel, "btnCancel" )
	SetEvent( WIDGET_ID_CHANGE_CARD_GENDER, EVENT_BUTTON_CLICK, "Ev_GCC_btnCancel", "", "btnCancel" )
	
end

-- 성별 변경창 오픈
function OpenGenderChangeCard()

	g_nGCC_HairIndex = 0
	g_nGCC_HairMaxIndex = 0
	
	g_nGCC_FaceIndex = 0
	g_nGCC_FaceMaxIndex = 0
	
	-- 캐릭터 성별 전환 데이터 초기화
	-- 캐릭터 클래스, 캐릭터 성별, 캐릭터 머리스타일 불러오기
	GCC_InitData()
	
	-- 머리 스타일 모양 데이터 설정
	GCC_UpdateHairText()
	GCC_UpdateHairImg()
	
	-- 얼굴 스타일 모양 데이터 설정
	GCC_UpdateFaceText()
	GCC_UpdateFaceImg()

end

-- 메세지 박스 열기 - 성별 전환 카드 아이템 사용
function GCC_UseItem( StrTarName )

	-- 성별 변경 카드를 사용하시겠습니까?	MODAL_GENDER_CHANGE
	local strMsgBox = GetGameInternalText( "MODAL_GENDER_CHANGE", 0 )
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, strMsgBox, GCC_OpenUI, Event_MsgBox_Close )

end

-- 메세지 박스 예 클릭 후 UI
function GCC_OpenUI()

	-- UI 가 닫혀있으면은 UI를 연다
	if	false == IsOpenWidget( WIDGET_ID_CHANGE_CARD_GENDER ) then
		Event_MsgBox_Close()
		OpenWidget( WIDGET_ID_CHANGE_CARD_GENDER )
	end
	
end

-- 캐릭터 머리 스타일 데이터 초기화
-- 캐릭터 클래스, 캐릭터 성별, 캐릭터 머리스타일 불러오기
function GCC_InitData()
	
	-- 내 캐릭터 클래스
	local emClass = g_MyChar.m_emClass

	-- UI 에 보여주기 위해 임시 성전환
	-- m_nSex = ( m_nSex == g_nGCC_SEX_W ) ? g_nGCC_SEX_M : g_nGCC_SEX_W;
	if m_nSex == g_nGCC_SEX_W  then
		m_nSex = g_nGCC_SEX_M
	elseif m_nSex ~= g_nGCC_SEX_W then -- m_nSex == g_nGCC_SEX_M
		m_nSex = g_nGCC_SEX_W
	end

	emClass = GetCharClassToSexFlag( emClass )
	
	local UserClassType = GetCharClassFlagIndex( emClass )
	g_nGCC_HairMaxIndex = GCC_ClassToMaxHair( UserClassType )
	g_nGCC_FaceMaxIndex = GCC_ClassToMaxFace( UserClassType )

	g_nGCC_HairIndex = 0;
	g_nGCC_FaceIndex = 0;
	
end

-- 모든 캐릭터 클래스 별 상수 머리 스타일 번호 읽어오기
function GCC_ClassToMaxHair( UserClassType )
	
	if CHARCLASS_IDX_FIGHTER_M == UserClassType then		-- 0
		return g_CONST_CHARCLASS0.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_ARMS_M == UserClassType then		-- 1
		return g_CONST_CHARCLASS1.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_ARCHER_W == UserClassType then		-- 2
		return g_CONST_CHARCLASS2.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_SPIRIT_W == UserClassType then		-- 3
		return g_CONST_CHARCLASS3.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_EXTREAM_M == UserClassType then	-- 4
		return g_CONST_CHARCLASS4.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_EXTREAM_W == UserClassType then	-- 5
		return g_CONST_CHARCLASS5.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_FIGHTER_W == UserClassType then	-- 6
		return g_CONST_CHARCLASS6.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_ARMS_W == UserClassType then		-- 7
		return g_CONST_CHARCLASS7.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_ARCHER_M == UserClassType then		-- 8
		return g_CONST_CHARCLASS8.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_SPIRIT_M == UserClassType then		-- 9
		return g_CONST_CHARCLASS9.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_SCIENTIST_M == UserClassType then	-- 10
		return g_CONST_CHARCLASS10.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_SCIENTIST_W == UserClassType then	-- 11
		return g_CONST_CHARCLASS11.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_ASSASSIN_M == UserClassType then	-- 12
		return g_CONST_CHARCLASS12.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_ASSASSIN_W == UserClassType then	-- 13
		return g_CONST_CHARCLASS13.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_TRICKER_M == UserClassType then	-- 14
		return g_CONST_CHARCLASS14.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_TRICKER_W == UserClassType then	-- 15
		return g_CONST_CHARCLASS15.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_ETC_M == UserClassType then		-- 16
		return g_CONST_CHARCLASS16.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_ETC_W == UserClassType then		-- 17
		return g_CONST_CHARCLASS17.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_ACTOR_M == UserClassType then		-- 16
		return g_CONST_CHARCLASS18.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_ACTOR_W == UserClassType then		-- 17
		return g_CONST_CHARCLASS19.dwHAIRNUM_SELECT
	elseif CHARCLASS_IDX_NUM_ETC == UserClassType then		-- 18
		return g_CONST_CHARCLASS20.dwHAIRNUM_SELECT
	end

end

-- 모든 캐릭터 클래스 별 상수 얼굴 스타일 번호 읽어오기
function GCC_ClassToMaxFace( UserClassType )
	
	if CHARCLASS_IDX_FIGHTER_M == UserClassType then		-- 0
		return g_CONST_CHARCLASS0.dwHEADNUM
	elseif CHARCLASS_IDX_ARMS_M == UserClassType then		-- 1
		return g_CONST_CHARCLASS1.dwHEADNUM
	elseif CHARCLASS_IDX_ARCHER_W == UserClassType then		-- 2
		return g_CONST_CHARCLASS2.dwHEADNUM
	elseif CHARCLASS_IDX_SPIRIT_W == UserClassType then		-- 3
		return g_CONST_CHARCLASS3.dwHEADNUM
	elseif CHARCLASS_IDX_EXTREAM_M == UserClassType then	-- 4
		return g_CONST_CHARCLASS4.dwHEADNUM
	elseif CHARCLASS_IDX_EXTREAM_W == UserClassType then	-- 5
		return g_CONST_CHARCLASS5.dwHEADNUM
	elseif CHARCLASS_IDX_FIGHTER_W == UserClassType then	-- 6
		return g_CONST_CHARCLASS6.dwHEADNUM
	elseif CHARCLASS_IDX_ARMS_W == UserClassType then		-- 7
		return g_CONST_CHARCLASS7.dwHEADNUM
	elseif CHARCLASS_IDX_ARCHER_M == UserClassType then		-- 8
		return g_CONST_CHARCLASS8.dwHEADNUM
	elseif CHARCLASS_IDX_SPIRIT_M == UserClassType then		-- 9
		return g_CONST_CHARCLASS9.dwHEADNUM
	elseif CHARCLASS_IDX_SCIENTIST_M == UserClassType then	-- 10
		return g_CONST_CHARCLASS10.dwHEADNUM
	elseif CHARCLASS_IDX_SCIENTIST_W == UserClassType then	-- 11
		return g_CONST_CHARCLASS11.dwHEADNUM
	elseif CHARCLASS_IDX_ASSASSIN_M == UserClassType then	-- 12
		return g_CONST_CHARCLASS12.dwHEADNUM
	elseif CHARCLASS_IDX_ASSASSIN_W == UserClassType then	-- 13
		return g_CONST_CHARCLASS13.dwHEADNUM
	elseif CHARCLASS_IDX_TRICKER_M == UserClassType then	-- 14
		return g_CONST_CHARCLASS14.dwHEADNUM
	elseif CHARCLASS_IDX_TRICKER_W == UserClassType then	-- 15
		return g_CONST_CHARCLASS15.dwHEADNUM
	elseif CHARCLASS_IDX_ETC_M == UserClassType then		-- 16
		return g_CONST_CHARCLASS16.dwHEADNUM
	elseif CHARCLASS_IDX_ETC_W == UserClassType then		-- 17
		return g_CONST_CHARCLASS17.dwHEADNUM
	elseif CHARCLASS_IDX_ACTOR_M == UserClassType then		-- 16
		return g_CONST_CHARCLASS18.dwHEADNUM
	elseif CHARCLASS_IDX_ACTOR_W == UserClassType then		-- 17
		return g_CONST_CHARCLASS19.dwHEADNUM
	elseif CHARCLASS_IDX_NUM_ETC == UserClassType then		-- 18
		return g_CONST_CHARCLASS20.dwHEADNUM
	end

end

-- 머리 스타일 데이터 설정
function GCC_UpdateHairText()
	
	local strHairName = GetGameInternalText( "HAIRSTYLE_NAME", g_nGCC_HairIndex )
	SetString( WIDGET_ID_CHANGE_CARD_GENDER, "text", strHairName, "labelHair" )
	
end

-- 얼굴 스타일 데이터 설정
function GCC_UpdateFaceText()
	
	local strFaceName = GetGameInternalText( "FACESTYLE_NAME", g_nGCC_FaceIndex )
	SetString( WIDGET_ID_CHANGE_CARD_GENDER, "text", strFaceName, "labelFace" )
	
end

-- 머리 이미지 업데이트
function GCC_UpdateHairImg()

	local strInst = ""
	if 0 ~= g_MyChar.m_wSex then
	
		strInst = "hairWoman"		
		SetBool( WIDGET_ID_CHANGE_CARD_GENDER, "visible", false, "hairMan" )
		
	else
	
		strInst = "hairMan"
		SetBool( WIDGET_ID_CHANGE_CARD_GENDER, "visible", false, "hairWoman" )
		
	end
	
	SetBool( WIDGET_ID_CHANGE_CARD_GENDER, "visible", true, strInst )
	CallFunction( WIDGET_ID_CHANGE_CARD_GENDER, "gotoAndStop", tostring( g_nGCC_HairIndex + 1 ), strInst )

end

-- 얼굴 이미지 업데이트
function GCC_UpdateFaceImg()

	local strInst = ""
	if 0 ~= g_MyChar.m_wSex then
	
		strInst = "faceWoman"		
		SetBool( WIDGET_ID_CHANGE_CARD_GENDER, "visible", false, "faceMan" )
		
	else
	
		strInst = "faceMan"
		SetBool( WIDGET_ID_CHANGE_CARD_GENDER, "visible", false, "faceWoman" )
		
	end
	
	SetBool( WIDGET_ID_CHANGE_CARD_GENDER, "visible", true, strInst )
	CallFunction( WIDGET_ID_CHANGE_CARD_GENDER, "gotoAndStop", tostring( g_nGCC_FaceIndex + 1 ), strInst )

end

-- 결과 메세지 출력
function GCC_UseResult( emFB )

	-- 일반 오류
	if 0 == emFB then
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMINVEN_GENDER_CHANGE_FB_FAIL", 0 ), TEXT_COLOR_RED )
	
	-- 성공
	elseif 1 == emFB then
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMINVEN_GENDER_CHANGE_FB_OK", 0 ), TEXT_COLOR_RED )
		
		OpenMsgBox( MSG_BOX_TYPE_OK, GetGameInternalText( "MODAL_GENDER_CHANGE_END", 0 ), Ev_GCC_Complete )
	
	-- 아이템이 없음
	elseif 2 == emFB then
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMINVEN_GENDER_CHANGE_FB_NOITEM", 0 ), TEXT_COLOR_RED )
	
	-- 사용 불가능 아이템
	elseif 3 == emFB then
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMINVEN_GENDER_CHANGE_FB_ITEMTYPE", 0 ), TEXT_COLOR_RED )
	
	-- 캐릭터 직업이 틀려서 사용 불가능
	elseif 4 == emFB then
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMINVEN_GENDER_CHANGE_FB_BADCLASS", 0 ), TEXT_COLOR_RED )
	
	-- 잘못된 데이터
	elseif 5 == emFB then
	
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMINVEN_GENDER_CHANGE_FB_NOTVALUE", 0 ), TEXT_COLOR_RED )
		
	end
	
	CloseWidget( WIDGET_ID_CHANGE_CARD_GENDER )
	
end

------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 머리 왼쪽 버튼 이벤트
function Ev_GCC_btnHairPrev()

	g_nGCC_HairIndex = g_nGCC_HairIndex - 1	
	
	if	g_nGCC_HairIndex < 0 then
		g_nGCC_HairIndex = 0
	end
	
	GCC_UpdateHairText()
	GCC_UpdateHairImg()
	
end

-- 머리 오른쪽 버튼 이벤트
function Ev_GCC_btnHairNext()
	
	g_nGCC_HairIndex = g_nGCC_HairIndex + 1
	
	if	g_nGCC_HairIndex >= g_nGCC_HairMaxIndex then
		g_nGCC_HairIndex = g_nGCC_HairMaxIndex - 1
	end
	
	GCC_UpdateHairText()
	GCC_UpdateHairImg()
	
end

-- 얼굴 왼쪽 버튼 이벤트
function Ev_GCC_btnFacePrev()
	
	g_nGCC_FaceIndex = g_nGCC_FaceIndex - 1	
	
	if	g_nGCC_FaceIndex < 0 then
		g_nGCC_FaceIndex = 0
	end
	
	GCC_UpdateFaceText()
	GCC_UpdateFaceImg()
	
end

-- 얼굴 오른쪽 버튼 이벤트
function Ev_GCC_btnFaceNext()
	
	g_nGCC_FaceIndex = g_nGCC_FaceIndex + 1
	
	if	g_nGCC_FaceIndex >= g_nGCC_FaceMaxIndex then
		g_nGCC_FaceIndex = g_nGCC_FaceMaxIndex - 1
	end
	
	GCC_UpdateFaceText()
	GCC_UpdateFaceImg()
	
end

-- 확인 버튼 이벤트
function Ev_GCC_btnOK()
	
	g_Msg:ReqInvenGenderChange( g_nGCC_FaceIndex, g_nGCC_HairIndex )
	
	-- UI 닫기
	CloseWidget( WIDGET_ID_CHANGE_CARD_GENDER )
	
end 

-- 취소 버튼 이벤트
function Ev_GCC_btnCancel()
	
	-- UI 닫기
	CloseWidget( WIDGET_ID_CHANGE_CARD_GENDER )
	
end

-- 성별 변경 완료 메시지
function Ev_GCC_Complete()

	-- 서버 변경 타이머 메시지
	EscMenu_ServerSelectTimerMsg()

end

g_nFSCC_Index = 0
g_nFSCC_MaxIndex = 0
g_nFSCC_Previous = 0

-- 얼굴 스타일 변경 카드 UI
function LoadFaceStyleChangeCard()

	SetEvent( WIDGET_ID_CHANGE_CARD_STYLE_FACE, EVENT_WIDGET_CLOSE, "CloseFaceStyleChangeCard", "", "" )
	
	-- 왼쪽 버튼	
	SetEvent( WIDGET_ID_CHANGE_CARD_STYLE_FACE, EVENT_BUTTON_CLICK, "Ev_FSCC_btnPrev", "", "btnPrev" )
	
	-- 오른쪽 버튼
	SetEvent( WIDGET_ID_CHANGE_CARD_STYLE_FACE, EVENT_BUTTON_CLICK, "Ev_FSCC_btnNext", "", "btnNext" )
	
	-- 확인 버튼		FaceSTYLECARD_OK.
	local btnOk = GetGameWordText( "MODAL_BUTTON", 0 )
	SetString( WIDGET_ID_CHANGE_CARD_STYLE_FACE, "label", btnOk, "btnOk" )
	
	-- 이벤트
	SetEvent( WIDGET_ID_CHANGE_CARD_STYLE_FACE, EVENT_BUTTON_CLICK, "Ev_FSCC_btnOK", "", "btnOk" )
	
	-- 취소 버튼		FaceSTYLECARD_CANCEL
	local btnCancel = GetGameWordText( "MODAL_BUTTON", 1 )
	SetString( WIDGET_ID_CHANGE_CARD_STYLE_FACE, "label", btnCancel, "btnCancel" )
	
	-- 이벤트
	SetEvent( WIDGET_ID_CHANGE_CARD_STYLE_FACE, EVENT_BUTTON_CLICK, "Ev_FSCC_btnClose", "", "btnCancel" )
	
end

-- 얼굴 변경창 오픈
function OpenFaceStyleChangeCard()

	g_nFSCC_Index = 0
	g_nFSCC_MaxIndex = 0

	-- 캐릭터 머리 스타일 데이터 초기화
	-- 캐릭터 클래스, 캐릭터 성별, 캐릭터 머리스타일 불러오기
	FSCC_InitData()
	
	SetBool( WIDGET_ID_CHANGE_CARD_STYLE_FACE, "visible", false, "hairMan" )
	SetBool( WIDGET_ID_CHANGE_CARD_STYLE_FACE, "visible", false, "hairWoman" )
	
	FSCC_UpdateImg()
	FSCC_UpdateText()

end

function CloseFaceStyleChangeCard()

	g_MyChar:FaceStyleChange( g_nFSCC_Previous )

end

-- UI 열기 - 얼굴 스타일 체인지 카드 아이템 사용
function FSCC_UseItem()

	-- UI 가 닫혀있으면은 UI를 연다
	if	false == IsOpenWidget( WIDGET_ID_CHANGE_CARD_STYLE_FACE ) then
		OpenWidget( WIDGET_ID_CHANGE_CARD_STYLE_FACE )
	end

end

-- 캐릭터 얼굴 스타일 데이터 초기화
-- 캐릭터 클래스, 캐릭터 성별, 캐릭터 얼굴 스타일 불러오기
function FSCC_InitData()

	local emClass = g_MyChar.m_emClass
	local UserClassType = GetCharClassFlagIndex( emClass )

	-- 모든 캐릭터 클래스 별 상수 얼굴 스타일 번호 적용
	g_nFSCC_MaxIndex = FSCC_ClassToFaceMaxIndex( UserClassType )
		
	g_nFSCC_Index = g_MyChar.m_wFace
	g_nFSCC_Previous = g_nFSCC_Index

end

-- 모든 캐릭터 클래스 별 상수 얼굴 스타일 번호 읽어오기
function FSCC_ClassToFaceMaxIndex( UserClassType )

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

-- 얼굴 스타일 데이터 클릭시 텍스트 업데이트
function FSCC_UpdateText()
	
	local strFaceName = GetGameInternalText( "FACESTYLE_NAME", g_nFSCC_Index )
	SetString( WIDGET_ID_CHANGE_CARD_STYLE_FACE, "text", strFaceName, "labelStyleName" )

end

-- 성공 / 실패 메세지 출력
function FSCC_UseResult( emFB, dwID )

	-- 일반오류.
	if 0 == emFB then
	
		g_MyChar:FaceStyleChange( g_nFSCC_Index )
		
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMINVEN_FACE_CHANGE_FB_FAIL", 0 ), TEXT_COLOR_RED )
		
	-- 성공
	elseif 1 == emFB then
	
		g_nFSCC_Index = dwID
		g_MyChar.m_wFace = g_nFSCC_Index
		g_MyChar:UpdateSuit( 0, 1 )
		
		-- 얼굴 스타일 변경
		g_MyChar:FaceStyleChange( g_nFSCC_Index )
		
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMINVEN_FACE_CHANGE_FB_OK", 0 ), TEXT_COLOR_PALEGREEN )
		
	-- 아이템이 없음
	elseif 2 == emFB then
	
		g_MyChar:FaceStyleChange( g_nFSCC_Index )
		
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMINVEN_FACE_CHANGE_FB_NOITEM", 0 ), TEXT_COLOR_RED )
		
	-- 사용 불가능 아이템
	elseif 3 == emFB then
	
		g_MyChar:FaceStyleChange( g_nFSCC_Index )
		
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMINVEN_FACE_CHANGE_FB_BADITEM", 0 ), TEXT_COLOR_RED )
		
	-- 캐릭터 직업이 틀려서 사용 불가능 아이템
	elseif 4 == emFB then
	
		g_MyChar:FaceStyleChange( g_nFSCC_Index )
		
		CF_PrintMsg_SystemMsg( GetGameInternalText( "EMINVEN_FACE_CHANGE_FB_BADCLASS", 0 ), TEXT_COLOR_RED )
		
	-- 쿨타임 실패
	elseif 5 == emFB then
	
		g_MyChar:FaceStyleChange( g_nFSCC_Index )
		
	end
	
	CloseWidget( WIDGET_ID_CHANGE_CARD_STYLE_FACE )

end

function FSCC_UpdateImg()

	local strInst = ""
	if 0 == g_MyChar.m_wSex then
	
		strInst = "faceWoman"		
		SetBool( WIDGET_ID_CHANGE_CARD_STYLE_FACE, "visible", false, "faceMan" )
		
	else
	
		strInst = "faceMan"
		SetBool( WIDGET_ID_CHANGE_CARD_STYLE_FACE, "visible", false, "faceWoman" )
		
	end
	
	SetBool( WIDGET_ID_CHANGE_CARD_STYLE_FACE, "visible", true, strInst )
	CallFunction( WIDGET_ID_CHANGE_CARD_STYLE_FACE, "gotoAndStop", tostring( g_nFSCC_Index + 1 ), strInst )
	
	g_MyChar:FaceStyleChange( g_nFSCC_Index )

end

------------------------------------------------------------------------------------------------------------------------
-- 이벤트 

-- 왼쪽 버튼 이벤트
function Ev_FSCC_btnPrev()
	
	g_nFSCC_Index = g_nFSCC_Index - 1	
	
	if	g_nFSCC_Index < 0 then
		g_nFSCC_Index = 0
	end
	
	FSCC_UpdateImg()
	FSCC_UpdateText()

end

-- 오른쪽 버튼 이벤트
function Ev_FSCC_btnNext()

	g_nFSCC_Index = g_nFSCC_Index + 1

	if	g_nFSCC_Index >= g_nFSCC_MaxIndex then
		g_nFSCC_Index = g_nFSCC_MaxIndex - 1
	end
	
	FSCC_UpdateImg()
	FSCC_UpdateText()

end

-- 확인 버튼 이벤트
function Ev_FSCC_btnOK()
	
	g_Msg:ReqInvenFaceStyleChange( g_nFSCC_Index )
	
	-- UI 닫기
	CloseWidget( WIDGET_ID_CHANGE_CARD_STYLE_FACE )

end

-- 취소 버튼 이벤트
function Ev_FSCC_btnClose()
	
	-- UI 닫기
	CloseWidget( WIDGET_ID_CHANGE_CARD_STYLE_FACE )

end
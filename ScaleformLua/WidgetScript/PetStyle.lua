
PETSTYLE_HUSKY		= 0
PETSTYLE_TIGER		= 2
PETSTYLE_BEAR		= 3
PETSTYLE_OWL 		= 4
PETSTYLE_BUDGERIGAR = 5
PETSTYLE_MONKEY 	= 8
PETSTYLE_FAIRY 		= 9
PETSTYLE_CAT 		= 10
PETSTYLE_LEOPARD 	= 11
PETSTYLE_DINOSAUR 	= 12

-- 펫 스타일 이미지 프레임 이름
g_strPetStyle_FrameName = ""

-- 현재 펫 스타일 최대 인덱스
g_nPetStyle_MaxIndex = 0
-- 현재 펫 스타일 인덱스
g_nPetStyle_CurIndex = 0

-- 펫 스타일 변경 카드 UI
function LoadPetStyle()

	SetEvent( WIDGET_ID_PETSTYLE, EVENT_WIDGET_CLOSE, "ClosePetStyle", "", "" )
	SetBool( WIDGET_ID_PETSTYLE, "visible", false, "closeBtn" )
	
	SetBool( WIDGET_ID_PETSTYLE, "visible", false, "faceMan" )
	SetBool( WIDGET_ID_PETSTYLE, "visible", false, "faceWoman" )
	
	SetBool( WIDGET_ID_PETSTYLE, "visible", false, "hairMan" )
	SetBool( WIDGET_ID_PETSTYLE, "visible", false, "hairWoman" )
	
	-- 왼쪽 버튼	
	SetEvent( WIDGET_ID_PETSTYLE, EVENT_BUTTON_CLICK, "Ev_PetStyle_btnPrev", "", "btnPrev" )
	-- 오른쪽 버튼
	SetEvent( WIDGET_ID_PETSTYLE, EVENT_BUTTON_CLICK, "Ev_PetStyle_btnNext", "", "btnNext" )
	
	-- 확인 버튼
	local str = GetGameWordText( "MODAL_BUTTON", 0 )
	SetString( WIDGET_ID_PETSTYLE, "label", str, "btnOk" )
	SetEvent( WIDGET_ID_PETSTYLE, EVENT_BUTTON_CLICK, "Ev_PetStyle_btnOK", "", "btnOk" )
	
	-- 취소 버튼
	str = GetGameWordText( "MODAL_BUTTON", 1 )
	SetString( WIDGET_ID_PETSTYLE, "label", str, "btnCancel" )
	SetEvent( WIDGET_ID_PETSTYLE, EVENT_BUTTON_CLICK, "Ev_PetStyle_btnCancel", "", "btnCancel" )

end

-- 펫 스타일 변경창 오픈
function OpenPetStyle()

	g_nPetStyle_CurIndex = g_Pet.m_wStyle
	
	PetStyle_SetInfo()
	
end

-- 닫기
function ClosePetStyle()

	-- 스타일 원래대로 돌리기
	g_Pet:StyleChange( g_Pet.m_wStyle )

end

-- 펫 스타일 정보 설정
function PetStyle_SetInfo()

	-- 현재 펫 스타일 이미지 설정
	CallFunction( WIDGET_ID_PETSTYLE, "gotoAndStop", 
		g_strPetStyle_FrameName .. tostring( g_nPetStyle_CurIndex + 1 ), "PetTypeImage" )
		
	-- 스타일 이름
	local str = GetGameInternalText( "PETSTYLECARD_NAME_A", g_nPetStyle_CurIndex )
	SetString( WIDGET_ID_PETSTYLE, "text", str, "labelStyleName" )

end

----------------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- UI 열기 - 펫 스타일 체인지 카드 아이템 사용
function PetStyle_UseItem()

	if true == IsOpenWidget( WIDGET_ID_PETSTYLE ) then
		return
	end

	-- 현재 소환된 펫 타입의 번호를 리턴(텍스트를 리턴하고 싶을 경우 g_Pet:GetTypeString() 사용)
	local nPetType = g_Pet:GetType()
	
	-- 현재 펫 스타일 최대 인덱스
	g_nPetStyle_MaxIndex = GetPetStyleNum( nPetType )
	if 0 == g_nPetStyle_MaxIndex then
		return
	end
	
	if nPetType == PETSTYLE_HUSKY then
		g_strPetStyle_FrameName = "husky"
	elseif nPetType == PETSTYLE_TIGER then
		g_strPetStyle_FrameName = "tiger"
	elseif nPetType == PETSTYLE_BEAR then
		g_strPetStyle_FrameName = "bear"
	elseif nPetType == PETSTYLE_OWL then
		g_strPetStyle_FrameName = "Owl"
	elseif nPetType == PETSTYLE_BUDGERIGAR then
		g_strPetStyle_FrameName = "Budgerigar"
	elseif nPetType == PETSTYLE_MONKEY then
		g_strPetStyle_FrameName = "Monkey"
	elseif nPetType == PETSTYLE_FAIRY then
		g_strPetStyle_FrameName = "Fairy"
	elseif nPetType == PETSTYLE_CAT then
		g_strPetStyle_FrameName = "Cat"
	elseif nPetType == PETSTYLE_LEOPARD then
		g_strPetStyle_FrameName = "Leopard"
	elseif nPetType == PETSTYLE_DINOSAUR then
		g_strPetStyle_FrameName = "Dinosaur"
	end
	
	OpenWidget( WIDGET_ID_PETSTYLE )
	
end

-- 성공 / 실패 메세지 출력
function PetStyle_UseResult( emFB, wStyle, wColor )

	-- 일반오류.
	if 0 == emFB then
	
		--!! 채팅창 메시지 출력
		
	-- 성공
	elseif 1 == emFB or 2 == emFB then
	
		--!! 채팅창 메시지 출력
		
	-- 아이템이 없음
	elseif 3 == emFB then
	
		--!! 채팅창 메시지 출력
		
	-- 사용 불가능 아이템
	elseif 4 == emFB then
	
		--!! 채팅창 메시지 출력
		
	-- 쿨타임 실패
	elseif 5 == emFB then
	
		--!! 채팅창 메시지 출력
		
	-- 스킨팩 사용중
	elseif 6 == emFB then
	
		--!! 채팅창 메시지 출력
		
	end

end

----------------------------------------------------------------------------------------------------------------
-- 이벤트

-- 왼쪽 버튼 이벤트
function Ev_PetStyle_btnPrev()
	
	g_nPetStyle_CurIndex = g_nPetStyle_CurIndex - 1
	if g_nPetStyle_CurIndex <= 0 then
		g_nPetStyle_CurIndex = 0
	end
	
	PetStyle_SetInfo()

	-- 펫 스타일 변경
	g_Pet:StyleChange( g_nPetStyle_CurIndex )

end

-- 오른쪽 버튼 이벤트
function Ev_PetStyle_btnNext()

	g_nPetStyle_CurIndex = g_nPetStyle_CurIndex + 1
	if g_nPetStyle_MaxIndex <= g_nPetStyle_CurIndex then
		g_nPetStyle_CurIndex = g_nPetStyle_MaxIndex - 1
	end
	
	PetStyle_SetInfo()

	-- 펫 스타일 변경
	g_Pet:StyleChange( g_nPetStyle_CurIndex )
	
end

-- 확인 버튼 이벤트
function Ev_PetStyle_btnOK()
	
	-- 서버에 스타일 변경 요청
	g_Msg:ReqChangeStylePet( g_nPetStyle_CurIndex )
	
	-- UI 닫기
	CloseWidget( WIDGET_ID_PETSTYLE )

end

-- 취소 버튼 이벤트
function Ev_PetStyle_btnCancel()
	
	-- UI 닫기
	CloseWidget( WIDGET_ID_PETSTYLE )

end
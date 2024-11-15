
g_Guide_List_Size = 22			-- 가이드 리스트 크기(gameword.xml 파일에 GUIDE_LIST 부분의 최종 index 번호와 동일해야 한다.)
g_Guide_List = { }				-- 가이드 리스트 텍스트를 담을 테이블 변수
g_funcGuide_EscKey = nil		-- ESC키 이벤트

---------------------가이드용 코드----------------------
GUIDE_STRING_GRADE 	= 6
GUIDE_STRING_PET 	= 7
GUIDE_STRING_SRC 	= 8

--------------------------------------------------------
-------------가이드용 코드 Index Max번호----------------(해당 숫자는 XML 설명에서 Index가 한줄 추가되거나 삭제되면 변경 되어야 된다.)
GUIDE_INDEX_GRADE 	= 6
GUIDE_INDEX_PET 	= 20
GUIDE_INDEX_SRC 	= 8

--------------------------------------------------------

-- 실행파일을 실행하여 가이드창을 처음 열떄
function LoadGuide()
	Guide_Intit()
	Guide_ListUpdate()
	Open_Guide()
end
-- 가이드창을 열때
function Open_Guide()
	SetDataComplex( WIDGET_ID_GUIDE, "label;depth;likeFolder", g_Guide_List, "GuideList", false )		-- 가이드 리스트 출력
	SetEvent( WIDGET_ID_GUIDE, EVENT_LIST_ITEM_CLICK, "Guide_PrintData", "index", "GuideList" )		-- 가이드 리스트 클릭시 이벤트
end
-- 퀘스트 기본 레이아웃 셋팅
function Guide_Intit()
	g_funcGuide_EscKey = Event_Guide_Close
	SetEvent( WIDGET_ID_GUIDE, EVENT_KEY_UP, "Ev_Guide_KeyUp", "keyCode", "" )	-- 키보드 이벤트
	
	SetString( WIDGET_ID_GUIDE, "title", GetGameWordText( "ACTIVITY_HELP_TEXT_TITLE", 0 ), "" )
	-- 영상/텍스트 타이틀 이름 설정
	SetString( WIDGET_ID_GUIDE, "text", GetGameWordText( "ACTIVITY_HELP_TEXT_TITLE", 0 ), "MovieTitle" )	-- 영상부분
	SetString( WIDGET_ID_GUIDE, "text", GetGameWordText( "ACTIVITY_HELP_TEXT_TITLE", 0 ), "BodyTitle" )	-- 텍스트 부분
	-- 완료/포기 버튼 설정
	--SetString( WIDGET_ID_QUEST, "label", GetGameWordText( "QUEST_MAIN_BUTTON", 3 ), "CompleteButton" )
	--SetBool( WIDGET_ID_QUEST, "visible", false, "CompleteButton" )
	--SetString( WIDGET_ID_QUEST, "label", GetGameWordText( "QUEST_MAIN_BUTTON", 0 ), "GiveUpButton" )
	--SetBool( WIDGET_ID_QUEST, "visible", false, "GiveUpButton" )
end
-- 가이드 리스트 생성
function Guide_ListUpdate()
	for i = 1, g_Guide_List_Size, 1 do
		local GuideList = { }
		GuideList[1] = GetGameWordText( "GUIDE_LIST", i - 1 )	-- gameword에 index 는 0부터 시작하므로 i에 -1 해준다.(label 부분)
		GuideList[2] = 0										-- depth
		GuideList[3] = false
		table.insert(g_Guide_List, GuideList)					-- 테이블 구조로 넣어줘야 출력이 된다.
	end
end

function Guide_PrintData( nIndex )
	SetString( WIDGET_ID_GUIDE, "text", "", "BodyTextField" )	-- 설명란 텍스트 초기화
	if GUIDE_STRING_PET == nIndex then
		Guide_Print_Pet()
	end
	if GUIDE_STRING_GRADE == nIndex then
		Guide_Print_Grade()
	end
end
-- 재련 방법 설명
function Guide_Print_Grade() 
	
	OpenMsgBox(MSG_BOX_TYPE_OKCANCEL, GetGameInternalText( "GUIDE_VIEW_PET", 21 ), Guide_Print_Pet, Guide_Print_Pet )
--	local PetData = ""
--	for i = 0, GUIDE_INDEX_PET, 1 do
--		PetData = PetData .. GetGameInternalText( "GUIDE_VIEW_PET", i ) .. "\n"
--	end
--	SetString( WIDGET_ID_GUIDE, "text", PetData, "BodyTextField" )
end
------ 펫 사용법 설명
function Guide_Print_Pet()
	local PetData = ""
	for i = 0, GUIDE_INDEX_PET, 1 do
		PetData = PetData .. GetGameInternalText( "GUIDE_VIEW_PET", i ) .. "\n"
	end
	SetString( WIDGET_ID_GUIDE, "text", PetData, "BodyTextField" )
end



-- 키보드 이벤트 (단축키 체크는 캐릭터 접속까지 끝나야한다. 게임 외부에서 처리를 위해 이벤트로 처리해주자.)
function Ev_Guide_KeyUp( nKeyCode )
	-- 알림창이 visible off 되어 있으면 On 시킨다.
	if false == GetBool( WIDGET_ID_GUIDE, "visible", "" ) then
		g_funcGuide_EscKey = Event_Guide_Close		-- ESC키를 눌렀을 경우 창을 종료하기 위한 문
	end
	
	if KEYCODE_ESCAPE == nKeyCode then
		Quest_HotKey_ESC()
	end
end
-- 닫기 이벤트
function Event_Guide_Close()
	CloseWidget(WIDGET_ID_GUIDE)		
end
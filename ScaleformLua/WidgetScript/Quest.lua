g_nQuest_SelectTep = 0			-- 현재 선택한 페이지
g_nQuest_Chack_MaxNum = 5		-- 퀘스트 진행상황 보기 등록 최대 개수
g_nQuest_Chack_ChackNum = 0		-- 현재 등록된 퀘스트 진행상황 보기 개수
g_Quest_Run_Area = { }			-- 내가 가진 진행중 퀘스트 정보를 담을 테이블 변수
g_Quest_End_Area = { }			-- 내가 가진 완료 퀘스트 정보를 담을 테이블 변수

g_Quest_Run_IDtoArea = { }		-- 각 ID가 속한 지역 라벨명을 저장하기 위한 변수

g_nQuest_OpenTreeID = -1		-- 퀘스트창 오픈 후 펼칠 트리 퀘스트 ID
g__Quest_Chack_True = 1			-- 퀘스트 체크를 true 처리한다.
g__Quest_Chack_False = 0		-- 퀘스트 체크를 false 처리한다.

g_Quest_Reword_Item = 0			-- 퀘스트 보상 아이템 슬롯에 아이템이 등록되어있는지 확인하기 위한 on/off 용 함수(on = 1, off = 0)
g_Quest_Collect_Item = 0		-- 퀘스트 수집 아이템 슬롯에 아이템이 등록되어있는지 확인하기 위한 on/off 용 함수(on = 1, off = 0)

g_funcQuest_EscKey = nil		-- ESC키 이벤트

g_nScrollPos = 0 				-- 스크롤 인덱스

function LoadQuest()

	SetEvent(WIDGET_ID_QUEST, EVENT_WIDGET_CLOSE, "Ev_Quest_Close", "", "")
	
	Quest_Intit()
	Quest_ListLoad_Run()
	Quest_ListLoad_End()
	
end

function OpenQuest()

	if -1 ~= g_nQuest_OpenTreeID then
	
		Quest_ListOpen_Run( g_nQuest_OpenTreeID )
		g_nQuest_OpenTreeID = -1
	
	end

	--Quest_Intit()
	--Quest_ListLoad_Run()
	--Quest_ListLoad_End()
	
	
end

-- 퀘스트창 종료시 발생시킬 이벤트(변수 초기화용)
function Ev_Quest_Close()

	g_Quest_Run_Area = { }			
	g_Quest_End_Area = { }	
	
	-- 퀘스트 창이 종료되면 퀘스트 도우미의 라디오 버튼 포커스를 제거한다.
	for i = 1, g_nQuest_Chack_ChackNum, 1 do
	
		SetBool(WIDGET_ID_QUEST_HELPER, "selected", false, "TitleRadioButton" .. tostring(i))
		
	end
	
	-- 퀘스트 도우미를 통한 퀘스트 UI 토글이 가능하도록 하기 위함
	g_QuestHelper_Focus = -1
	
end

-- 퀘스트 기본 레이아웃 셋팅
function Quest_Intit()

	g_funcQuest_EscKey = Event_Quest_Close
	SetEvent( WIDGET_ID_QUEST, EVENT_KEY_UP, "Ev_Quest_KeyUp", "keyCode", "" )	-- 키보드 이벤트
	
	SetString( WIDGET_ID_QUEST, "title", GetGameWordText( "QUEST_WINDOW_STATIC", 0 ), "" )
	-- 퀘스트 진행상황 보기 등록 기본 셋팅(string.format 은 %d 를 출력하기 위한 명령어, StringFormat 은 %1% 를 출력하기 위한 명령어)
	local ChackQuestNum = string.format(GetGameInternalText("QUEST_HELPER_COUNT", 0) , g_nQuest_Chack_ChackNum, g_nQuest_Chack_MaxNum)
	SetString( WIDGET_ID_QUEST, "text", ChackQuestNum, "QuestRegistration" )
	-- 진행중인 퀘스트 각 항목 타이틀 이름 설정
	SetString( WIDGET_ID_QUEST, "text", GetGameInternalText( "QUEST_LIST_COLLECT_ITEM", 0 ), "QuestAction.RunningQuest.CollectionTitle" )	-- 임무
	SetString( WIDGET_ID_QUEST, "text", GetGameWordText( "POSTBOX_PAGE_SEND_TEXTBOX", 3 ), "QuestAction.RunningQuest.ContentTitle" )		-- 내용
	SetString( WIDGET_ID_QUEST, "text", GetGameWordText( "QUEST_LIST_BUTTON", 2 ), "QuestAction.RunningQuest.MinutelyTitle" )				-- 자세히
	-- 종료된 퀘스트 각 항목 타이틀 이름 설정
	SetString( WIDGET_ID_QUEST, "text", GetGameWordText( "QUEST_LIST_RESULT", 0 ), "QuestAction.CompleteQuest.ResultTitle" )				-- 수행결과
	SetString( WIDGET_ID_QUEST, "text", GetGameWordText( "POSTBOX_PAGE_SEND_TEXTBOX", 3 ), "QuestAction.CompleteQuest.ContentTitle" )		-- 내용
	SetString( WIDGET_ID_QUEST, "text", GetGameWordText( "QUEST_LIST_BUTTON", 2 ), "QuestAction.CompleteQuest.MinutelyTitle" )				-- 자세히
	-- 보상 아이템 타이틀 이름 설정
	SetString( WIDGET_ID_QUEST, "text", GetGameWordText( "QUEST_GIFT", 9 ), "RewordTitl" )	-- 퀘스트 보상
	-- 완료/포기 버튼 설정
	SetString( WIDGET_ID_QUEST, "label", GetGameWordText( "QUEST_MAIN_BUTTON", 3 ), "CompleteButton" )
	SetBool( WIDGET_ID_QUEST, "visible", false, "CompleteButton" )
	SetString( WIDGET_ID_QUEST, "label", GetGameWordText( "QUEST_MAIN_BUTTON", 0 ), "GiveUpButton" )
	SetBool( WIDGET_ID_QUEST, "visible", false, "GiveUpButton" )
	
	-- 보상 아이템 슬롯 바인딩(연결)
	Quest_RewordSlot_Binding()
	-- 수집 아이템 슬롯 바인딩(연결)
	Quest_CollectSlot_Binding()
	QuestTep()
	Quest_UpdateTep()
	
end
-- 퀘스트 보상 아이템 슬롯 바인딩
function Quest_RewordSlot_Binding() 
	local Route = "RewardItem"
	local SumRoute = ""
	for i = 1, 8, 1 do
		local SlotID = SLOT_QUEST_REWORDITEM_ID .. i
		SumRoute = Route .. tostring(i)
		SetString(WIDGET_ID_QUEST, "binding", SlotID, SumRoute)
	end
end
-- 퀘스트 수집 아이템 슬롯 바인딩
function Quest_CollectSlot_Binding() 
	local Route = "QuestAction.RunningQuest.QuestItem"
	local SumRoute = ""
	for i = 1, 8, 1 do
		local SlotID = SLOT_QUEST_COLLECTITEM_ID .. i
		SumRoute = Route .. tostring(i)
		SetString(WIDGET_ID_QUEST, "binding", SlotID, SumRoute)
	end
end
-- 진행중인 퀘스트 리스트 데이터 셋팅
function Quest_ListLoad_Run()

	if false == IsLoadWidget( WIDGET_ID_QUEST ) then
		return
	end

	g_Quest_Run_Area = g_MyChar:GetQuestListProc()	-- 현재 내가 가진 퀘스트 정보를 가져온다.
	local AreaData = g_Quest_Run_Area["Area"]		-- 가져온 퀘스트 정보중 지역 이름을 테이블로 가져온다.
	local AreaSize = table.getn(AreaData)		-- 가져온 지역 테이블의 개수를 구한다.(내가 가진 퀘스트 지역 개수)
	local tbQuest								-- 퀘스트 이름 및 아이디 값을 담을 임시변수
	local QuestSize								-- 가져온 퀘스트 테이블의 개수를 구한다.(내가 가진 퀘스트 개수)
	local tbAreaList = { }						-- 퀘스트 지역 및 퀘스트 리스트의 트리 테이블을 담을 테이블 변수
	g_Quest_Run_IDtoArea = {}	-- 지역별 라벨 데이터 초기화
	
	for i = 1, AreaSize, 1 do
		local QuestData = { }		-- 퀘스트 리스트의 트리 테이블을 담을 테이블 변수
		local Quest_Run_tbAreaName = g_Quest_Run_Area[AreaData[i]]	-- 지역이름으로 각각의 퀘스트를 테이블에 저장한다.
		QuestSize = table.getn(Quest_Run_tbAreaName)				-- 퀘스트의 총 개수를 측정한다.
		for j = 1, QuestSize, 1 do
			tbQuest = Quest_Run_tbAreaName[j]						-- 퀘스트 이름과 ID 를 tbQuest 변수에 담아준다.
			if nil ~= tbQuest[1] then
				local tbInputQuest = { }
				tbInputQuest[1] = tbQuest[1]			-- labal
				tbInputQuest[2] = 1						-- depth
				tbInputQuest[3] = tbQuest[2]			-- Quest id
				if true == tbQuest[3] then
					tbInputQuest[4] = 1
				elseif false == tbQuest[3] then
					tbInputQuest[4] = 0
				end
				--tbInputQuest[4] = tostring(tbQuest[3])	-- 체크 여부
				tbInputQuest[5] = { }					-- leaf
				tbInputQuest[6] = false
				table.insert(QuestData, tbInputQuest)
				
				local tbIDtoArea = {}		-- 퀘스트 ID 별 지역을 삽입하기 위한 변수 생성
				tbIDtoArea[1] = tbQuest[2]	-- 퀘스트 ID 삽입
				tbIDtoArea[2] = AreaData[i]	-- 퀘스트 지역 라벨 삽입
				table.insert(g_Quest_Run_IDtoArea, tbIDtoArea)
			end
		end
		
		local tbQuestList = { }
		tbQuestList[1] = AreaData[i]-- labal
		tbQuestList[2] = 0			-- depth
		tbQuestList[3] = -1			-- id(무조건 -1)
		tbQuestList[4] = -1			-- 체크 여부(무조건 -1)
		tbQuestList[5] = QuestData	-- leaf
		tbQuestList[6] = false		-- leaf 데이터가 없어도 폴더처럼 사용?
		table.insert(tbAreaList, tbQuestList)
	end
	
	local strParam = CreateParam( "label", "depth", "id", "checked", "leaf", "likeFolder" )
	
	-- tbAreaList 내용을 QuestList 에 출력
	SetDataComplex( WIDGET_ID_QUEST, strParam, tbAreaList, "QuestList_run", false )
	-- QuestList의 리스트 아이템을 클릭시 클릭 이벤트
	SetEvent( WIDGET_ID_QUEST, EVENT_LIST_ITEM_CLICK, "Quest_DataLoad_Run", "index", "QuestList_run" )
	-- QuestList의 체크박스를 클릭시 클릭 이벤트
	SetEvent( WIDGET_ID_QUEST, EVENT_TREE_ITEM_CHECK, "Quest_QuestList_Chack", "index", "QuestList_run" )
	-- 스크롤 이벤트
	SetEvent( WIDGET_ID_QUEST, EVENT_SCROLL, "Ev_Proc_Quest_List_Scroll", "", "QuestList_run" )	
	
end
-- 종료된 퀘스트 리스트 데이터 셋팅
function Quest_ListLoad_End()

	if false == IsLoadWidget( WIDGET_ID_QUEST ) then
		return
	end

	g_Quest_End_Area = g_MyChar:GetQuestListEnd()	-- 현재 내가 가진 퀘스트 정보를 가져온다.
	local AreaData = g_Quest_End_Area["Area"]		-- 가져온 퀘스트 정보중 지역 이름을 테이블로 가져온다.
	local AreaSize = table.getn(AreaData)			-- 가져온 지역 테이블의 개수를 구한다.(내가 가진 퀘스트 지역 개수)
	local tbQuest									-- 퀘스트 이름 및 아이디 값을 담을 임시변수
	local QuestSize									-- 가져온 퀘스트 테이블의 개수를 구한다.(내가 가진 퀘스트 개수)
	local tbAreaList = { }							-- 퀘스트 지역 및 퀘스트 리스트의 트리 테이블을 담을 테이블 변수
	
	for i = 1, AreaSize, 1 do
		local QuestData = { }		-- 퀘스트 리스트의 트리 테이블을 담을 테이블 변수
		local Quest_End_tbAreaName = g_Quest_End_Area[AreaData[i]]	-- 지역이름으로 각각 퀘스트를 테이블에 저장한다.
		QuestSize = table.getn(Quest_End_tbAreaName)				-- 퀘스트의 총 개수를 측정한다.
		
		for j = 1, QuestSize, 1 do
			tbQuest = Quest_End_tbAreaName[j]		-- 퀘스트 이름과 ID 를 tbQuest 변수에 담아준다.
			if nil ~= tbQuest[1] then
				local tbInputQuest = { }			-- 퀘스트 트리(잎) 테이블 구축용 변수
				local Quest_CashData = g_MyChar:GettbQuestEnd( tbQuest[2] )	-- 임시 변수를 선언하여 해당 퀘스트 ID 의 데이터값 담기
				local Quest_Data = Quest_CashData["Data"]					-- 담긴 임시 데이터값의 테이블을 Quest_Data 에 담기
				local Quest_Success = g_MyChar:GetQuestComplete( tbQuest[2], false )
				
				if true == Quest_Success then
					tbInputQuest[1] = HTMLImage( "QUEST_WINDOW_LIST_CHILD_DONE_LUXES", 12, 12 ) .. tbQuest[1]	-- 이미지 + labal(각 잎(leaf)의(각 퀘스트) 이름)
				elseif false == Quest_Success then
					tbInputQuest[1] = HTMLImage( "QUEST_WINDOW_LIST_CHILD_FAIL_LUXES", 12, 12 ) .. tbQuest[1]	-- 이미지 + labal(각 잎(leaf)의(각 퀘스트) 이름)
				end
				
				tbInputQuest[2] = 1						-- depth(최상위 바로 아래일 경우 1)
				tbInputQuest[3] = tbQuest[2]			-- Quest id
				tbInputQuest[4] = { }					-- leaf
				tbInputQuest[5] = false
				table.insert(QuestData, tbInputQuest)	-- 생성된 잎(leaf)들을 하나로 묶어 준다.
			end
		end
		
		local tbQuestList = { }
		tbQuestList[1] = AreaData[i]-- labal(최 상위 루트(지역 정보) 이름)
		tbQuestList[2] = 0			-- depth(최상위 루트일 경우 0)
		tbQuestList[3] = -1			-- id(무조건 -1)
		tbQuestList[4] = QuestData	-- leaf(묶인 잎(leaf)들을 담은 테이블)
		tbQuestList[5] = false		-- leaf 데이터가 없어도 폴더처럼 사용?
		table.insert(tbAreaList, tbQuestList)	-- 생성된 최상위 루트 정보를 하나로 묶어 준다.
	end
	
	local strParam = CreateParam( "label", "depth", "id", "leaf", "likeFolder" )
	
	-- tbAreaList 내용을 QuestList 에 출력
	SetDataComplex( WIDGET_ID_QUEST, strParam, tbAreaList, "QuestList_end", false )
	-- QuestList의 리스트 아이템을 클릭시 클릭 이벤트
	SetEvent( WIDGET_ID_QUEST, EVENT_LIST_ITEM_CLICK, "Quest_DataLoad_End", "index", "QuestList_end" )
	-- 스크롤 이벤트
	SetEvent( WIDGET_ID_QUEST, EVENT_SCROLL, "Ev_End_Quest_List_Scroll", "", "QuestList_end" )	
end

-- 퀘스트  리스트 스크롤
function Ev_Proc_Quest_List_Scroll()
	g_nScrollPos = GetInt( WIDGET_ID_QUEST, "scrollPosition", "QuestList_run" )
	--Quest_UpdateTep()
end
function Ev_End_Quest_List_Scroll()
	g_nScrollPos = GetInt( WIDGET_ID_QUEST, "scrollPosition", "QuestList_end" )
	--Quest_UpdateTep()	
end

-- 진행중 퀘스트 데이터 셋팅
function Quest_DataLoad_Run( nIndex )	
	local nQuestID			-- 선택한 퀘스트 ID 를 담을 변수
	local QuestData = { }	-- 선택한 퀘스트 데이터를 담을 테이블 변수
	local Quest_Data		-- 퀘스트 데이터 테이블을 받을 테이블 변수(Content, Detail, Expd, Money, Element, LifePoint, SkillPoint, StatPoint)
	local Quest_Content		-- 퀘스트 내용 텍스트를 담을 변수
	local Quest_Detail		-- 퀘스트 자세히 텍스트를 담을 변수
	
	-- (StepTitle, Condition1, Condition2, NPCtalk, QItem, Kill, ItemUse, ItemGrinding, LevelUp, Conft, NpcGuard, ReachZone, Defence)
	local Quest_Cond			-- 퀘스트 스텝 테이블을 받을 테이블 변수
	local Quest_CondSize		-- 퀘스트 스텝 테이블수를 받을 변수
	local Quest_StepTitle = ""	-- 퀘스트 스텝 타이틀값을 받을 변수
	local NowStep = 0			-- 현재 스텝 인덱스 번호를 받을 변수
	local Quest_StapDetail = ""	-- 퀘스트 스텝 자세히 값을 담을 변수
	local Quest_Qitem = { }		-- 수집 아이템 MID를 담을 변수
	
	nQuestID = GetDataInt( WIDGET_ID_QUEST, nIndex, "id", "QuestList_run" )	-- QuestList 의 해당 nIndex 리스트 아이템에 등록된 ID 값을 가져온다.
	
	CF_Quest_Helper_QuestSelect( nQuestID )
	
	if -1 ~= nQuestID then	-- 퀘스트 ID 가 -1이면 지역 트리를 클릭한것이기 때문에 퀘스트 ID 가 -1이면 저장하지 않는다.
		QuestData = g_MyChar:GettbQuestProc( nQuestID )
		Quest_Data = QuestData["Data"]			-- 퀘스트 데이터 내용(테이블)을 Quest_Data 변수에 저장한다.
		
		-- [shhan][2016.09.19] 비어있는 QuestData 정보가 넘어올 수 있다. ( 실제 crash 가 발생되서 추가함. )
		if nil == Quest_Data then
			return
		end
	
		Quest_Content = Quest_Data["Content"]		-- 퀘스트 내용 텍스트를 Quest_Content 변수에 저장한다.
		Quest_Detail = Quest_Data["Detail"]			-- 퀘스트 자세히 텍스트를 Quest_Detail 변수에 저장한다.
		
		Quest_Cond = QuestData["Cond"]				-- 퀘스트 스텝 내용(테이블)을 Quest_Cond 변수에 저장한다.
		Quest_CondSize = table.getn(Quest_Cond)		-- 스텝 개수를 구한다.(해당 스텝 개수만큼 for문을 돌리기 위함)
		
		Quest_Reword( QuestData )					-- 퀘스트 보상 출력
		NowStep = g_MyChar:GetQuestStep( nQuestID )	-- 현재 스텝 번호를 저장한다.
		
		-- 스텝 데이터 셋팅을 위한 for문
		for i = 1, Quest_CondSize, 1 do
			local Quest_CashData_Step = Quest_Cond[i]	-- Quest_CashData_Step 는 퀘스트와 관련된 임시 데이터를 담기 위한 임시변수이다.
			
			-- 아래 if 문은 각 스텝 상태별(완료/진행중/미진행) 텍스트 색상 및 아이콘을 설정하기 위한 if문이다.
			if i < NowStep + 1 then			-- 완료한 스텝
				Quest_CashData_Step["StepTitle"] = HTMLFontColor(Quest_CashData_Step["StepTitle"], HTML_TEXT_COLOR_GRAY)
				Quest_StepTitle = Quest_StepTitle .. HTMLImage( "QUEST_CHECK", 10, 10 ) .. Quest_CashData_Step["StepTitle"] .. "\n"
			elseif i == NowStep + 1 then	-- 진행중인 스텝
				Quest_CashData_Step["StepTitle"] = HTMLFontColor(Quest_CashData_Step["StepTitle"], HTML_TEXT_COLOR_GOLD)
				Quest_StepTitle = Quest_StepTitle .. HTMLImage( "QUEST_WINDOW_LIST_CHILD_DONE_LUXES", 12, 12 ) .. Quest_CashData_Step["StepTitle"] .. "\n"
			else							--미진행 스텝
				Quest_StepTitle = Quest_StepTitle .. HTMLImage( "QUEST_CHECK", 10, 10 ) .. Quest_CashData_Step["StepTitle"] .. "\n"
			end		
			
			if Quest_CashData_Step["Condition1"] ~= nil then
				local strCondition = Quest_CashData_Step["Condition1"]
				strCondition = strCondition:replace("\n", "")
				Quest_StepTitle = Quest_StepTitle .. HTMLFontColor ( strCondition, HTML_TEXT_COLOR_ORANGE ) .. "\n"
			end
			
			if Quest_CashData_Step["Condition2"] ~= nil then
				local strCondition = Quest_CashData_Step["Condition2"]
				strCondition = strCondition:replace("\n", "")
				Quest_StepTitle = Quest_StepTitle .. HTMLFontColor ( strCondition, HTML_TEXT_COLOR_ORANGE ) .. "\n"
			end
		end
		
		-- 아래 if 문은 각 스텝별 자세히 내용 및 수집아이템을 담기/출력하기 위한 if문이다.
		local Quest_CashData_Detail = Quest_Cond[ NowStep + 1 ]	-- Quest_CashData_Detail 는 자세히와 관련된 임시 데이터를 담기 위한 임시변수이다.
		
		-- 수집아이템 슬롯은 무조건 한번 비워준다.
		Quest_Collect_Slot_Delet()
		
		if nil ~= Quest_CashData_Detail["NPCtalk"] then
			Quest_StapDetail = Quest_CashData_Detail["NPCtalk"]		-- NPC 에게 대화걸기 스텝
		elseif nil ~= Quest_CashData_Detail["QItem"] then
						
			local QItemID = Quest_CashData_Detail["QItemID"]			-- 아이템 수집 아이콘 보여주기
			local QItemSize = table.getn( QItemID )
			-- 수집 아이템 보여주기
			if 0 < QItemSize then						-- 수집아이템이 있을경우 수집아이템 슬롯에 아이템을 등록한다.
				for i = 1, QItemSize, 1 do				-- 수집아이템의 개수만큼 반복
					if 0 ~= QItemID then				-- 혹시 수집 아이템이 없지 않는지 최종검사.
						local ITemID = QItemID[i]		-- 수집 아이템[i 번째] ID 등록
						local SlotID = SLOT_QUEST_COLLECTITEM_ID .. i			-- 슬롯 ID[i 번째] 등록
						SetSlot_Item( SlotID, ITemID[1], ITemID[2], ITemID[4] )	-- 슬롯에 아이템 등록(1이 MID, 2가 SID, 3이 필요개수, 4가 현재개수 이다.)
					end
				end
				g_Quest_Collect_Item = 1				-- 슬롯에 아이템이 등록되었으므로 슬롯 상태를 on 상태로 변경한다.
			end
			-- 퀘스트 텍스트 출력
			local ITemID = QItemID[1]
			Quest_StapDetail = Quest_CashData_Detail["QItem"] .. "[" .. tostring(ITemID[4]) .. "/" .. tostring(ITemID[3]) .. "]"	-- 아이템 수집 텍스트(수집한개수/총개수)
			
			--Quest_QitemNum = 
		elseif nil ~= Quest_CashData_Detail["Kill"] then
			Quest_StapDetail = Quest_CashData_Detail["Kill"]			-- 몬스터 퇴치
		elseif nil ~= Quest_CashData_Detail["ItemUse"] then
			Quest_StapDetail = Quest_CashData_Detail["ItemUse"]		-- 아이템 사용
		elseif nil ~= Quest_CashData_Detail["ItemGrinding"] then
			Quest_StapDetail = Quest_CashData_Detail["ItemGrinding"]	-- 아이템 연마
		elseif nil ~= Quest_CashData_Detail["LevelUp"] then
			Quest_StapDetail = Quest_CashData_Detail["LevelUp"]		-- 캐릭터 레벨업
		elseif nil ~= Quest_CashData_Detail["Conft"] then
			Quest_StapDetail = Quest_CashData_Detail["Conft"]			-- 대련
		elseif nil ~= Quest_CashData_Detail["NpcGuard"] then
			Quest_StapDetail = Quest_CashData_Detail["NpcGuard"]		-- NPC 보호
		elseif nil ~= Quest_CashData_Detail["ReachZone"] then
			Quest_StapDetail = Quest_CashData_Detail["ReachZone"]		-- 특정 위치 도달
		elseif nil ~= Quest_CashData_Detail["Defence"] then
			Quest_StapDetail = Quest_CashData_Detail["Defence"]		-- 어느 위치 방어
		end
		
		if Quest_StapDetail ~= "" then
			
			Quest_StapDetail = HTMLFontColor(Quest_StapDetail, HTML_TEXT_COLOR_GOLD)	-- 퀘스트 자세히 * 적혀있는 항목 색상 변경(금색)
			Quest_Detail = Quest_StapDetail .. "\n" .. Quest_Detail						-- * 적혀있는 항목과 기존 자세히 항목을 연결
			
		end
		
		-- 퀘스트 완료 버튼 생성 및 클릭이벤트 생성(퀘스트 ID 가 -1이 들어오면 안된다.)
		if true == g_MyChar:GetQuestCompleteCheck( nQuestID ) then
			SetBool( WIDGET_ID_QUEST, "visible", true, "CompleteButton" )
			SetEvent( WIDGET_ID_QUEST, EVENT_BUTTON_CLICK, "Quest_Complete", "", "CompleteButton", nQuestID )
		else			
			SetBool( WIDGET_ID_QUEST, "visible", false, "CompleteButton" )
		end
		-- 퀘스트 포기 버튼 생성 및 클릭이벤트 생성(퀘스트 ID 가 -1이 들어오면 안된다.)
		if true == g_MyChar:GetQuestGiveUp( nQuestID ) then
		
			SetBool( WIDGET_ID_QUEST, "visible", true, "GiveUpButton" )
			SetEvent( WIDGET_ID_QUEST, EVENT_BUTTON_CLICK, "Quest_GiveUp", "", "GiveUpButton", nQuestID )		-- 포기 버튼 클릭 이벤트
			
		else
		
			SetBool( WIDGET_ID_QUEST, "visible", false, "GiveUpButton" )
		
		end
		
		SetString( WIDGET_ID_QUEST, "text", Quest_Content, "QuestAction.RunningQuest.ContentText" )	-- 퀘스트 내용 텍스트 출력
		SetString( WIDGET_ID_QUEST, "htmlText", Quest_Detail, "QuestAction.RunningQuest.DetailText" )	-- 자세히 텍스트 출력
		SetString( WIDGET_ID_QUEST, "htmlText", Quest_StepTitle, "QuestAction.RunningQuest.StapText" )	-- 스텝 텍스트 출력
	end
end
-- 종료된 퀘스트 데이터 셋팅
function Quest_DataLoad_End( nIndex )
	local nQuestID			-- 선택한 퀘스트 ID 를 담을 변수
	local QuestData = { }	-- 선택한 퀘스트 데이터를 담을 테이블 변수
	local Quest_Data		-- 퀘스트 데이터 테이블을 받을 테이블 변수(Content, Detail, Exp, Money, Element, LifePoint, SkillPoint, StatPoint)
	local Quest_Result		-- 퀘스트 수행결과 텍스트를 담을 변수
	local Quest_Content		-- 퀘스트 내용 텍스트를 담을 변수
	local Quest_Detail		-- 퀘스트 자세히 텍스트를 담을 변수
	local Quest_DataSize	-- 퀘스트 데이터(스텝) 테이블수를 받을 변수
	-- (StepTitle, Condition1, Condition2, NPCtalk, QItem, Kill, ItemUse, ItemGrinding, LevelUp, Conft, NpcGuard, ReachZone, Defence)
	local Quest_StepTitle = ""	-- 퀘스트 스텝 타이틀값을 받을 변수
	
	local Quest_Cond			-- 퀘스트 스텝 테이블을 받을 테이블 변수
	local Quest_CondSize		-- 퀘스트 스텝 테이블수를 받을 변수
	
	local Quest_StapDetail
	local Quest_QitemID = { }		-- 수집 아이템 ID를 담을 변수
	local Quest_QitemNum = { }		-- 수집 아이템 개수를 담을 변수
	
	nQuestID = GetDataInt( WIDGET_ID_QUEST, nIndex, "id", "QuestList_end" )	-- QuestList 의 리스트 아이템에 등록된 ID 값을 가져온다.
	
	if -1 ~= nQuestID then	-- 퀘스트 
		QuestData = g_MyChar:GettbQuestEnd( nQuestID )
		Quest_Data = QuestData["Data"]
		
		-- [shhan][2016.09.19] 비어있는 QuestData 정보가 넘어올 수 있다.
		if nil == Quest_Data then
			return
		end
		
		Quest_Reword( QuestData )		-- 퀘스트 보상 출력
		
		Quest_Result = Quest_Data["Result"]
		Quest_Content = Quest_Data["Content"]
		Quest_Detail = Quest_Data["Detail"]

		Quest_Cond = QuestData["Data"]
		
		local Quest_CashData = Quest_Data["StepTitle"]	-- Quest_CashData 는 퀘스트와 관련된 임시 데이터를 담기 위한 임시변수이다.
		Quest_DataSize = table.getn(Quest_CashData)
		for i = 1, Quest_DataSize, 1 do
			Quest_StepTitle = Quest_StepTitle .. HTMLImage( "QUEST_CHECK", 10, 10 ) .. Quest_CashData[i] .. "\n"						
		end
		Quest_StepTitle = HTMLFontColor(Quest_StepTitle, HTML_TEXT_COLOR_GRAY)
		
		SetString( WIDGET_ID_QUEST, "text", Quest_Result, "QuestAction.CompleteQuest.ResultText" )			-- 수행결과 텍스트 출력
		SetString( WIDGET_ID_QUEST, "text", Quest_Content, "QuestAction.CompleteQuest.ContentText" )		-- 퀘스트 내용 텍스트 출력
		SetString( WIDGET_ID_QUEST, "htmlText", Quest_Detail, "QuestAction.CompleteQuest.DetailText" )		-- 자세히 텍스트 출력
		SetString( WIDGET_ID_QUEST, "htmlText", Quest_StepTitle, "QuestAction.CompleteQuest.StapText" )	-- 스텝 텍스트 출력
		SetEvent( WIDGET_ID_QUEST, EVENT_BUTTON_CLICK, "Quest_Complete", "", "CompleteButton", nQuestID )		-- 완료 버튼 클릭 이벤트
	end
	
end
-- 퀘스트 보상 부분 데이터 셋팅
function Quest_Reword( Q_Data )
	local Quest_Data = Q_Data["Data"]
	local Reword_Item = Q_Data["Item"]					-- 아이템
	local Reword_Item_Size	= table.getn(Reword_Item)	-- 아이템 개수
	local Reword_Skill = Q_Data["Skill"]				-- 스킬
	local Reword_Skill_Size = table.getn(Reword_Skill)	-- 스킬 개수
	local Quest_Reword_Data = ""						-- 퀘스트 보상 데이터 텍스트 값을 담을 변수
	local Quest_Exp = Quest_Data["Exp"]					-- 경험치
	local Quest_Money = Quest_Data["Money"]				-- 게임머니
	local Quest_Element = Quest_Data["Element"]			-- 성향
	local Quest_LifePoint = Quest_Data["LifePoint"]		-- 생활점수
	local Quest_SkillPoint = Quest_Data["SkillPoint"]	-- 스킬포인트
	local Quest_StatPoint = Quest_Data["StatPoint"]		-- 스텟포인트


	if 0 ~= Quest_Exp then
		Quest_Reword_Data = Quest_Reword_Data .. GetGameWordText( "QUEST_GIFT", 0 ) .. " : " .. Quest_Exp .. "    "
	end
	if 0 ~= Quest_Money then
		Quest_Reword_Data = Quest_Reword_Data .. GetGameWordText( "QUEST_GIFT", 1 ) .. " : " .. Quest_Money .. "    "
	end
	if 0 ~= Quest_Element then
		Quest_Reword_Data = Quest_Reword_Data .. GetGameWordText( "QUEST_GIFT", 2 ) .. " : " .. Quest_Element .. "    "
	end
	if 0 ~= Quest_LifePoint then
		Quest_Reword_Data = Quest_Reword_Data .. GetGameWordText( "QUEST_GIFT", 3 ) .. " : " .. Quest_LifePoint .. "    "
	end
	if 0 ~= Quest_SkillPoint then
		Quest_Reword_Data = Quest_Reword_Data .. GetGameWordText( "QUEST_GIFT", 5 ) .. " : " .. Quest_SkillPoint .. "    "
	end
	if 0 ~= Quest_StatPoint then
		Quest_Reword_Data = Quest_Reword_Data .. GetGameWordText( "QUEST_GIFT", 6 ) .. " : " .. Quest_StatPoint .. "    "
	end
	
	if 0 < Reword_Skill_Size then
		for i = 1, Reword_Skill_Size, 1 do
			if 0 ~= Reword_Skill then
				Quest_Reword_Data = Quest_Reword_Data .. GetGameWordText( "QUEST_GIFT", 8 ) .. " : " .. Reword_Skill[i] .. "    "
			end
		end
	end
	
	-- 보상 아이템 슬롯을 한번 비운후 다시 채운다.
	Quest_Reword_Slot_Delet()
	
	-- 보상 아이템 보여주기
	if 0 < Reword_Item_Size then				-- 보상아이템이 있을경우 보상아이템 슬롯에 아이템을 등록한다.
		for i = 1, Reword_Item_Size, 1 do		-- 보상아이템의 개수만큼 반복
			if 0 ~= Reword_Item then			-- 혹시 보상 아이템이 없지 않는지 최종검사.
				local ITemID = Reword_Item[i]	-- 보상 아이템[i 번째] ID 등록
				local SlotID = SLOT_QUEST_REWORDITEM_ID .. i	-- 슬롯 ID[i 번째] 등록
				if ITemID[3] == 0 then
					SetSlot_Item( SlotID, ITemID[1], ITemID[2], 1 )	-- 슬롯에 아이템 등록
				else
					SetSlot_Item( SlotID, ITemID[1], ITemID[2], ITemID[3] )	-- 슬롯에 아이템 등록
				end
			end
		end
		g_Quest_Reword_Item = 1				-- 슬롯에 아이템이 등록되었으므로 슬롯 상태를 on 상태로 변경한다.
	end

	SetString( WIDGET_ID_QUEST, "htmlText", Quest_Reword_Data, "RewordTextField" )
end
-- 보상 아이템 슬롯 비우기
function Quest_Reword_Slot_Delet()
	if 1 == g_Quest_Reword_Item then	-- 슬롯에 아이템이 등록 되어있는 상태일 경우만 슬롯을 비워준다.
		for i = 1, 8, 1 do
			local SlotID = SLOT_QUEST_REWORDITEM_ID .. i
			ResetSlot( SlotID )			-- 슬롯에 등록된 아이템 삭제
		end
		g_Quest_Reword_Item = 0			-- 슬롯을 비웠으므로 슬롯상태를 off 상태로 변경한다.
	end
end
-- 수집 아이템 슬롯 비우기
function Quest_Collect_Slot_Delet()
	if 1 == g_Quest_Collect_Item then	-- 슬롯에 아이템이 등록 되어있는 상태일 경우만 슬롯을 비워준다.
		for i = 1, 8, 1 do
			local SlotID = SLOT_QUEST_COLLECTITEM_ID .. i
			ResetSlot( SlotID )			-- 슬롯에 등록된 아이템 삭제
		end
		g_Quest_Collect_Item = 0			-- 슬롯을 비웠으므로 슬롯상태를 off 상태로 변경한다.
	end
end
-- 퀘스트창 텝 설정
function QuestTep()
	
-- 탭 데이터 생성
	local strTabData = ""
	for i = 0, 1, 1 do
		strTabData = strTabData .. GetGameWordText( "QUEST_LIST_BUTTON", i ) .. ";"
	end
-- 탭 설정	
	SetData( WIDGET_ID_QUEST, strTabData, "QuestTabPage", false )
	SetInt( WIDGET_ID_QUEST, "selectedIndex", g_nQuest_SelectTep, "QuestTabPage" )
	SetEvent( WIDGET_ID_QUEST, EVENT_INDEX_CLICK_CHANGE, "Ev_Quest_TabPage", "index;", "QuestTabPage" )

	
end
-- 페이지 업데이트
function Quest_UpdateTep()
	
		-- 완료/포기 버튼 설정
	SetString( WIDGET_ID_QUEST, "label", GetGameWordText( "QUEST_MAIN_BUTTON", 3 ), "CompleteButton" )
	SetBool( WIDGET_ID_QUEST, "visible", false, "CompleteButton" )
	SetString( WIDGET_ID_QUEST, "label", GetGameWordText( "QUEST_MAIN_BUTTON", 0 ), "GiveUpButton" )
	SetBool( WIDGET_ID_QUEST, "visible", false, "GiveUpButton" )
	
	
	-- 진행중인 퀘스트 보이기
	if 0 == g_nQuest_SelectTep then
		SetBool( WIDGET_ID_QUEST, "visible", true, "QuestAction.RunningQuest" )
		SetBool( WIDGET_ID_QUEST, "visible", true, "QuestList_run" )
		SetBool( WIDGET_ID_QUEST, "visible", false, "QuestAction.CompleteQuest" )
		SetBool( WIDGET_ID_QUEST, "visible", false, "QuestList_end" )
		
		Quest_Collect_Slot_Delet()
		Quest_Reword_Slot_Delet()
		Quest_Cleen_Run( "Proc" )
	-- 완료 퀘스트 보이기
	elseif 1 == g_nQuest_SelectTep then
		SetBool( WIDGET_ID_QUEST, "visible", false, "QuestAction.RunningQuest" )
		SetBool( WIDGET_ID_QUEST, "visible", false, "QuestList_run" )
		SetBool( WIDGET_ID_QUEST, "visible", true, "QuestAction.CompleteQuest" )
		SetBool( WIDGET_ID_QUEST, "visible", true, "QuestList_end" )
		
		Quest_Collect_Slot_Delet()
		Quest_Reword_Slot_Delet()
		Quest_Cleen_Run( "End" )
	end
end
-- 퀘스트 리스트에서 체크박스 체크시 들어올 함수
function Quest_QuestList_Chack( nIndex )
	local nQuestID			-- 선택한 퀘스트 ID 를 담을 변수
	local ChackBox			-- 체크 여부를 담을 변수
	local nIdx = nIndex + g_nScrollPos -- 스크롤 되되는경우 index가 달라진다
	
	--nQuestID = GetDataInt( WIDGET_ID_QUEST, nIndex, "id", "QuestList_run" )	-- QuestList 의 리스트 아이템에 등록된 ID 값을 가져온다.
	--ChackBox = GetDataBool( WIDGET_ID_QUEST, nIndex, "checked", "QuestList_run" ) -- 선택한 인덱스 항목의 체크박스가 체크되었는지 여부 저장
	nQuestID = GetDataInt( WIDGET_ID_QUEST, nIdx, "id", "QuestList_run" )	-- QuestList 의 리스트 아이템에 등록된 ID 값을 가져온다.
	ChackBox = GetDataBool( WIDGET_ID_QUEST, nIdx, "checked", "QuestList_run" ) -- 선택한 인덱스 항목의 체크박스가 체크되었는지 여부 저장
	
	local NoticeQuest = GetDataString( WIDGET_ID_QUEST, nIdx, "label", "QuestList_run" )
	if true == ChackBox then		-- 체크박스에 체크가 되어 있을경우
		if g_nQuest_Chack_MaxNum > g_nQuest_Chack_ChackNum then	-- 최대 개수를 넘지 않게 하기 위해서
			g_nQuest_Chack_ChackNum = g_nQuest_Chack_ChackNum + 1	-- 체크가 되었기 때문에 퀘스트 등록개수 1 증가
			local ChackQuestNum = string.format(GetGameInternalText("QUEST_HELPER_COUNT", 0) , g_nQuest_Chack_ChackNum, g_nQuest_Chack_MaxNum)
			SetString( WIDGET_ID_QUEST, "text", ChackQuestNum, "QuestRegistration" )	-- 체크사항이 변동하였으므로 텍스트 갱신
			if false == IsOpenWidget(WIDGET_ID_QUEST_HELPER) then
				OpenWidget(WIDGET_ID_QUEST_HELPER)
			end
			Quest_Helper_Chack( nQuestID )		-- 퀘스트 헬프창 체크 실행
			g_MyChar:SetQuestHelpCheck( nQuestID, true)
			NoticeQuest = StringFormat( GetGameInternalText("QUEST_HELPER_ADD", 0), NoticeQuest)
		elseif g_nQuest_Chack_MaxNum == g_nQuest_Chack_ChackNum then	-- 최대 개수이상 체크 할 수 없도록 하는 부분
			CallFunction( WIDGET_ID_QUEST, "DoCheck", tostring(nIdx) .. ";" .. tostring(g__Quest_Chack_False), "QuestList_run" )
			BasicChat_PrintSystemMsg(CHAT_SYSTEM,GetGameInternalText("QUEST_HELPER_ERROR", 0),TEXT_COLOR_RED)
			return
		end
	elseif false == ChackBox then	-- 체크박스에 체크가 되어 있지 않을경우
		Quest_ChackBox_NotChack( nQuestID )
		NoticeQuest = StringFormat(GetGameInternalText("QUEST_HELPER_DEL", 0), NoticeQuest )
	end
	BasicChat_PrintSystemMsg(CHAT_SYSTEM,NoticeQuest,TEXT_COLOR_LIGHTSKYBLUE) 
end
-- 체크박스에 체크가 되어 있지 않을 경우에 사용될 함수
function Quest_ChackBox_NotChack( nQuestID )
	if 0 < g_nQuest_Chack_ChackNum then	-- 등록 개수가 0이하가 되지 않게 하기 위해서
		g_MyChar:SetQuestHelpCheck( nQuestID, false)
		g_nQuest_Chack_ChackNum = g_nQuest_Chack_ChackNum - 1	-- 체크 해제이므로 퀘스트 등록개수 1 감소
		Quest_Helper_UnChack( nQuestID )		-- 퀘스트 헬프창 체크해제 실행
		local ChackQuestNum = string.format(GetGameInternalText("QUEST_HELPER_COUNT", 0) , g_nQuest_Chack_ChackNum, g_nQuest_Chack_MaxNum)
		SetString( WIDGET_ID_QUEST, "text", ChackQuestNum, "QuestRegistration" )	-- 체크사항이 변동하였으므로 텍스트 갱신
	end
end
-- 퀘스트가 체크되어 있는지 확인
function Quest_ChackBox_UnChack( QuestID )
	
	for i = 1, g_nQuest_Chack_ChackNum, 1 do
		if QuestID == g_QuestHelper_QuestID[i] then
			Quest_ChackBox_NotChack( QuestID )
			Quest_Cleen_Run( "Proc" )
		end
	end

end
-- 탭 인덱스 변경
function Ev_Quest_TabPage( nIndex )

	g_nQuest_SelectTep = nIndex
	Quest_UpdateTep()

end
-- 완료 버튼 클릭
function Quest_Complete( nQuestID )
	g_Msg:ReqQuestComplete( nQuestID )
end
-- 포기 버튼 클릭
function Quest_GiveUp( nQuestID )

	local tbData = { nQuestID }
	OpenMsgBox( MSG_BOX_TYPE_OKCANCEL, GetGameInternalText( "QUEST_GIVEUP_CONFIRM" ),
		Quest_GiveUp_OK, Quest_GiveUp_CANCEL, tbData )

end
-- 포기 확인 창에서 확인 클릭
function Quest_GiveUp_OK( tbData )

	g_Msg:ReqQuestGiveUp( tbData[ 1 ] )
	Event_MsgBox_Close()
	
end
-- 포기 확인 창에서 취소 클릭
function Quest_GiveUp_CANCEL()
	Event_MsgBox_Close()
end
-- 퀘스트 클린작업
function Quest_Cleen_Run( ProcOrEnd )
	if "Proc" == ProcOrEnd then			-- 진행중
		SetString( WIDGET_ID_QUEST, "text", "", "QuestAction.RunningQuest.ContentText" )	-- 내용란 지우기
		SetString( WIDGET_ID_QUEST, "text", "", "QuestAction.RunningQuest.DetailText" )	-- 자세히 란 지우기
		SetString( WIDGET_ID_QUEST, "text", "", "QuestAction.RunningQuest.StapText" )		-- 스텝 란 지우기
		SetString( WIDGET_ID_QUEST, "text", "", "RewordTextField" )						-- 보상 란 지우기
		
	elseif "End" == ProcOrEnd then		-- 종료된
		SetString( WIDGET_ID_QUEST, "text", "", "QuestAction.CompleteQuest.ResultText" )	-- 수행결과란 지우기
		SetString( WIDGET_ID_QUEST, "text", "", "QuestAction.CompleteQuest.ContentText" )	-- 내용란 지우기
		SetString( WIDGET_ID_QUEST, "text", "", "QuestAction.CompleteQuest.DetailText" )	-- 자세히 란 지우기
		SetString( WIDGET_ID_QUEST, "text", "", "RewordTextField" )						-- 보상 란 지우기
		
	end
end

-- 퀘스트 도우미에서 퀘스트 창을 열었을 경우 데이터 셋팅
function Quest_ListOpen_Run( nQuestID )

	if false == IsOpenWidget( WIDGET_ID_QUEST ) then
	
		g_nQuest_OpenTreeID = nQuestID
		OpenWidget( WIDGET_ID_QUEST )
		return
	
	end
	
	if 0 ~= g_nQuest_SelectTep then
	
		SetInt( WIDGET_ID_QUEST, "selectedIndex", 0, "QuestTabPage" )
	
	end

	-- 가져온 지역 테이블의 개수를 구한다.(내가 가진 퀘스트 지역 개수)
	local AreaSize = table.getn(g_Quest_Run_IDtoArea)
	
	for i = 1, AreaSize, 1 do
	
		-- 저장 되어 있던 i 번째의 퀘스트ID 와 지역명 테이블을 가져온다.
		local IDtoArea = g_Quest_Run_IDtoArea[i]
		
		-- 받아온 퀘스트ID 와 저장되어 있던 퀘스트ID 를 비교하여 같다면 실행
		if nQuestID == IDtoArea[1] then
		
			-- 지역명 비교이므로 0부터 새로 시작
			for j = 0, AreaSize, 1 do
			
				-- QuestList 의 해당 index(j) 리스트 아이템에 등록된 label 값을 가져와 비교한다.
				if IDtoArea[2] == GetDataString( WIDGET_ID_QUEST, j, "label", "QuestList_run" ) then
				
					-- 해당 index(j) 트리를 펼친다.(true 가 펼치기/false 가 접기)
					local strParam = CreateParam( j, true )
					CallFunction( WIDGET_ID_QUEST, "DoExpand", strParam, "QuestList_run" )
					
					local nDataProviderSize = GetInt( WIDGET_ID_QUEST, "length", "QuestList_run.dataProvider" )
					
					-- 받아온 퀘스트의 선택 처리 및 정보 출력을 위하여 index값을 구한다.
					local nIndex = Quest_Count_Index( nQuestID, nDataProviderSize )
					if -1 == nIndex then
						return
					end
					
					-- 퀘스트 정보를 출력
					Quest_DataLoad_Run( nIndex )
					-- 퀘스트 리스트 렌더러 선택처리
					SetInt( WIDGET_ID_QUEST, "selectedIndex", nIndex, "QuestList_run" )
					
					return
					
				end
				
			end
			
		end
		
	end
	
end

-- 퀘스트 리스트에서 원하는 퀘스트의 Index번호를 가져오기 위한 반복문함수
function Quest_Count_Index( nQuestID, AreaSize )

	for i = 1, AreaSize, 1 do
	
		-- QuestList 의 해당 nIndex 리스트 아이템에 등록된 ID 값을 가져온다.
		if nQuestID == GetDataInt( WIDGET_ID_QUEST, i, "id", "QuestList_run" ) then
		
			return i
			
		end
		
	end
	
	return -1
	
end

-- 퀘스트창 ESC키 단축키
function Quest_HotKey_ESC()
	-- 퀘스트창이 켜져 있지 않을 경우 리턴한다.
	if false == IsOpenWidget( WIDGET_ID_QUEST ) then
		return
	end
	-- 퀘스트창이 visible off 상태일 경우 리턴한다.
	if false == GetBool( WIDGET_ID_QUEST, "visible", "" ) then
		return false
	else
		-- ESC키 단축키 이벤트 호출
		if nil ~= g_funcQuest_EscKey then
			g_funcQuest_EscKey()				-- 여기서 위의 Quest_Intit 함수에서 
		end										-- g_funcQuest_EscKey = Event_Quest_Close 를
												-- 해주었기 때문에 Event_Quest_Close 가 실행된다.
		return true
	end
end
-- 키보드 이벤트 (단축키 체크는 캐릭터 접속까지 끝나야한다. 게임 외부에서 처리를 위해 이벤트로 처리해주자.)
function Ev_Quest_KeyUp( nKeyCode )
	-- 알림창이 visible off 되어 있으면 On 시킨다.
	if false == GetBool( WIDGET_ID_QUEST, "visible", "" ) then
		g_funcQuest_EscKey = Event_Quest_Close		-- ESC키를 눌렀을 경우 창을 종료하기 위한 문
	end
	
	if KEYCODE_ESCAPE == nKeyCode then
		Quest_HotKey_ESC()
	end
end
-- 닫기 이벤트
function Event_Quest_Close()
	CloseWidget(WIDGET_ID_QUEST)
	
	g_funcReportBox_EnterKey = nil
	g_funcQuest_EscKey = nil
		
end

function Quest_UpdateProc()

	if false == IsOpenWidget( WIDGET_ID_QUEST ) then
		return
	end

	local nIndex = GetInt( WIDGET_ID_QUEST, "selectedIndex", "QuestList_run" )
	Quest_DataLoad_Run( nIndex )

end

function Quest_Clear() 
	Quest_Intit()
	Quest_ListLoad_Run()
	Quest_ListLoad_End()
end
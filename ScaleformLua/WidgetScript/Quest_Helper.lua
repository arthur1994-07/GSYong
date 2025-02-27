--g_nQuest_Chack_MaxNum = 5		-- 퀘스트 진행상황 보기 등록 최대 개수
--g_nQuest_Chack_ChackNum = 0		-- 현재 등록된 퀘스트 진행상황 보기 개수

g_QuestHelper_QuestID = { }			-- 인덱스별 퀘스트 ID 를 담기위한 테이블
g_QuestHelper_Detail_Data = { }		-- 인덱스별 자세히 데이터를 담기위한 테이블
g_QuestHelper_StepTitle_Data = { }	-- 인덱스별 스텝 제목 데이터를 담기위한 테이블
g_QuestHelper_Focus = -1

function LoadQuestHelper()

	g_nQuest_Chack_ChackNum = 0
	Quest_Helper_Intit()
	
end

-- 퀴스트 도우미 기본셋팅
function Quest_Helper_Intit( )

	for i = 1, g_nQuest_Chack_MaxNum, 1 do
		SetBool(WIDGET_ID_QUEST_HELPER, "visible", false, "DetailText" .. tostring(i))			-- 접속시 안보이는 상태로 해놓기 위함(자세히 텍스트)
		HitTestEnable( WIDGET_ID_QUEST_HELPER, false, "DetailText" .. tostring(i) )						-- 텍스트 구간은 마우스 클릭 이벤트 설정을 금지하기 위함(자세히)
		SetBool(WIDGET_ID_QUEST_HELPER, "visible", false, "StepTitleText" .. tostring(i))		-- 접속시 안보이는 상태로 해놓기 위함(스텝 제목 텍스트)
		HitTestEnable( WIDGET_ID_QUEST_HELPER, false, "StepTitleText" .. tostring(i) )					-- 텍스트 구간은 마우스 클릭 이벤트 설정을 금지하기 위함(스텝 제목)
		SetBool(WIDGET_ID_QUEST_HELPER, "visible", false, "TitleRadioButton" .. tostring(i))	-- 접속시 안보이는 상태로 해놓기 위함(스텝 제목 라디오 버튼)
		
		SetEvent( WIDGET_ID_QUEST_HELPER, EVENT_BUTTON_CLICK, "Quest_Helper_TitleRadioButton", "", "TitleRadioButton" .. tostring(i) , tostring(i) )	-- 클릭 이벤트 설정(마지막에 tostring(i) 가 index 역할을 한다.)
	end
	HitTestEnable( WIDGET_ID_QUEST_HELPER, false, "BackGround" )	-- 틀을 잡아주는 배경 구간의 마우스 이벤트를 금지하기 위함(지면 클릭)(true 는 해당 UI 클릭/false 는 지면클릭)
	Quest_Helper_DataSetting()
	
end

-- 퀘스트 스텝 제목 라디오 버튼 클릭이벤트 설정
function Quest_Helper_TitleRadioButton( strIndex )

	local nIndex = tonumber( strIndex )
	for i = 1, g_nQuest_Chack_MaxNum, 1 do
	
		if i ~= nIndex then
			SetBool( WIDGET_ID_QUEST_HELPER, "selected", false, "TitleRadioButton" .. i )
		end
	
	end

	if g_QuestHelper_Focus ~= nIndex then		
	
		g_QuestHelper_Focus = nIndex
		Quest_ListOpen_Run( g_QuestHelper_QuestID[ nIndex ] )
		
	elseif g_QuestHelper_Focus == nIndex then
	
		if true == IsOpenWidget( WIDGET_ID_QUEST ) then
			CloseWidget(WIDGET_ID_QUEST)
		end
		
	end
end

-- 퀘스트 도우미 창에 출력할 데이터 셋팅을 한다.(체크되어있는 항목이 있는지 검사)
function Quest_Helper_DataSetting( )
	local Quest_Area = g_MyChar:GetQuestListProc()	-- 현재 내가 가진 퀘스트 정보를 가져온다.
	local AreaData = Quest_Area["Area"]				-- 가져온 퀘스트 정보중 지역 이름을 테이블로 가져온다.
	local AreaSize = table.getn(AreaData)		-- 가져온 지역 테이블의 개수를 구한다.(내가 가진 퀘스트 지역 개수)
	local Quest_tbArea_Name
	
	local bHasQuest = false
	if 0 < AreaSize then
	
		for i = 1, AreaSize, 1 do
		
			local QuestData = { }		-- 퀘스트 리스트의 트리 테이블을 담을 테이블 변수
			local QuestSize
			
			Quest_tbArea_Name = Quest_Area[AreaData[i]]	-- 지역이름으로 각각 퀘스트를 테이블에 저장한다.
			QuestSize = table.getn(Quest_tbArea_Name)		-- 퀘스트의 총 개수를 측정한다.
			
			for j = 1, QuestSize, 1 do
			
				tbQuest = Quest_tbArea_Name[j]		-- 퀘스트 이름과 ID 를 tbQuest 변수에 담아준다.
				
				if true == tbQuest[3] then
				
					bHasQuest = true
				
					local tbInputQuest = { }
					
					if g_nQuest_Chack_ChackNum < g_nQuest_Chack_MaxNum then
					
						g_nQuest_Chack_ChackNum = g_nQuest_Chack_ChackNum + 1
						Quest_Helper_Chack( tbQuest[2] )
						g_QuestHelper_QuestID[ g_nQuest_Chack_ChackNum ] = tbQuest[2]
						
					elseif g_nQuest_Chack_ChackNum == g_nQuest_Chack_MaxNum then
					
						Quest_Helper_Chack( tbQuest[2] )
						g_QuestHelper_QuestID[ g_nQuest_Chack_ChackNum ] = tbQuest[2]
						
					end		
					
				end
				
			end
			
		end

	end
	
	SetBool( WIDGET_ID_QUEST_HELPER, "visible", bHasQuest, "" )
	
end

-- 체크가 되어 있는 항목의 데이터를 셋팅한다.
function Quest_Helper_Chack( nQuestID )
	local QuestData = { }	-- 선택한 퀘스트 데이터를 담을 테이블 변수
	local Quest_Cond		-- 퀘스트 데이터 테이블을 받을 테이블 변수(Content, Detail, Exp, Money, Element, LifePoint, SkillPoint, StatPoint)
	local Quest_StapDetail	-- 퀘스트 자세히 내용을 담을 변수
	
	QuestData = g_MyChar:GettbQuestProcInfo( nQuestID )	-- 현재 스텝의 데이터 내용을 가져온다.
	Quest_Cond = QuestData["Cond"]						-- 퀘스트 스텝 내용(테이블)을 Quest_Cond 변수에 저장한다.
	Quest_StapDetail = Quest_Helper_DetailData( Quest_Cond )
		
	-- lua_tinker::table GettbQuestProcInfo( DWORD dwQuestID)
	
	g_QuestHelper_QuestID[ g_nQuest_Chack_ChackNum ] = nQuestID							-- 체크 해재시를 대비한 강 Index 별 ID 등록
	g_QuestHelper_Detail_Data[ g_nQuest_Chack_ChackNum ] = Quest_StapDetail				-- Index 별 스텝 자세히 데이터 등록을 위한 데이터 입력
	g_QuestHelper_StepTitle_Data[ g_nQuest_Chack_ChackNum ] = Quest_Cond["StepTitle"]	-- Index 별 스텝 타이틀 데이터 등록을 위한 데이터 입력
	
	Quest_Helper_OutputData()	--등록된 퀘스트 출력
	
end
-- 자세히 데이터를 셋팅하기 위한 분류 함수
function Quest_Helper_DetailData( Quest_Cond )
	local Quest_StapDetail	-- 퀘스트 자세히 내용을 담을 변수
	if nil ~= Quest_Cond["NPCtalk"] then
		Quest_StapDetail = Quest_Cond["NPCtalk"]		-- NPC 에게 대화걸기 스텝
	elseif nil ~= Quest_Cond["QItem"] then
		local QItemID = Quest_Cond["QItemID"]			-- 아이템 수집 아이콘 보여주기
		local ITemID = QItemID[1]
		Quest_StapDetail = Quest_Cond["QItem"] .. "[" .. tostring(ITemID[4]) .. "/" .. tostring(ITemID[3]) .. "]"	-- 아이템 수집[수집한개수/목표개수]
	elseif nil ~= Quest_Cond["Kill"] then
		Quest_StapDetail = Quest_Cond["Kill"]			-- 몬스터 퇴치
	elseif nil ~= Quest_Cond["ItemUse"] then
		Quest_StapDetail = Quest_Cond["ItemUse"]		-- 아이템 사용
	elseif nil ~= Quest_Cond["ItemGrinding"] then
		Quest_StapDetail = Quest_Cond["ItemGrinding"]	-- 아이템 연마
	elseif nil ~= Quest_Cond["LevelUp"] then
		Quest_StapDetail = Quest_Cond["LevelUp"]		-- 캐릭터 레벨업
	elseif nil ~= Quest_Cond["Conft"] then
		Quest_StapDetail = Quest_Cond["Conft"]			-- 대련
	elseif nil ~= Quest_Cond["NpcGuard"] then
		Quest_StapDetail = Quest_Cond["NpcGuard"]		-- NPC 보호
	elseif nil ~= Quest_Cond["ReachZone"] then
		Quest_StapDetail = Quest_Cond["ReachZone"]		-- 특정 위치 도달
	elseif nil ~= Quest_Cond["Defence"] then
		Quest_StapDetail = Quest_Cond["Defence"]		-- 어느 위치 방어
	end
	
	return Quest_StapDetail
end
-- 퀘스트 체크 해제시 해당 내용을 퀘스트 도우미 창에서 삭제하기 위한 함수
function Quest_Helper_UnChack( nQuestID )
	local UnChack = 0
	for i = 1, g_nQuest_Chack_ChackNum, 1 do
		if nQuestID == g_QuestHelper_QuestID[i] then
			for j = i, g_nQuest_Chack_ChackNum, 1 do
				g_QuestHelper_QuestID[j] = g_QuestHelper_QuestID[j + 1]
				g_QuestHelper_Detail_Data[j] = g_QuestHelper_Detail_Data[j + 1]	-- 퀘스트 자세히 내용 테이블 빈공간 채우기
				g_QuestHelper_StepTitle_Data[j] = g_QuestHelper_StepTitle_Data[j + 1]	-- 퀘스트 스텝 제목 내용 테이블 빈공간 채우기
			end
		end
	end
	Quest_Helper_OutputData( UnChack )	--등록된 퀘스트 출력(체크 해제 이벤트라는 것을 넣어줌)
	
end
-- 퀘스트 도우미창 텍스트를 출력하기 위한 함수
function Quest_Helper_OutputData( UnChack )

	if 0 == g_nQuest_Chack_ChackNum then
		SetBool( WIDGET_ID_QUEST_HELPER, "visible", false, "" )
	else
		SetBool( WIDGET_ID_QUEST_HELPER, "visible", true, "" )
	end

	for i = 1, g_nQuest_Chack_ChackNum, 1 do	-- 등록된 퀘스트 수에 따른 순차적 입력
		
		SetBool(WIDGET_ID_QUEST_HELPER, "visible", true, "DetailText" .. tostring(i))			-- 체크 된 항목수만큼 visible on 시킴(자세히 텍스트)
		SetBool(WIDGET_ID_QUEST_HELPER, "visible", true, "StepTitleText" .. tostring(i))		-- 체크 된 항목수만큼 visible on 시킴(스텝 제목 텍스트)
		SetBool(WIDGET_ID_QUEST_HELPER, "visible", true, "TitleRadioButton" .. tostring(i))	-- 체크 된 항목수만큼 visible on 시킴(스텝 제목 라디오 버튼)
		
		local strDetail_Data = g_QuestHelper_Detail_Data[i]
		if strDetail_Data == nil then
			strDetail_Data = ""
		end
		
		local strStepTile_Data = g_QuestHelper_StepTitle_Data[i]
		if strStepTile_Data == nil then
			strStepTile_Data = ""
		end
		
		SetString( WIDGET_ID_QUEST_HELPER, "text", strDetail_Data, "DetailText" .. tostring(i) )			-- 퀘스트 자세히 텍스트 출력
		SetString( WIDGET_ID_QUEST_HELPER, "text", strStepTile_Data, "StepTitleText" .. tostring(i) )	-- 퀘스트 스텝 제목 텍스트 출력
	end
	
	if 0 == UnChack then	-- 체크 해제시에만 실행
		for i = g_nQuest_Chack_ChackNum + 1, g_nQuest_Chack_MaxNum, 1 do	-- 등록된 퀘스트 수에 따른 순차적 입력
			SetBool(WIDGET_ID_QUEST_HELPER, "visible", false, "DetailText" .. tostring(i))			-- 체크 해제된 항목수만큼 visible off 시킴(자세히 텍스트)
			SetBool(WIDGET_ID_QUEST_HELPER, "visible", false, "StepTitleText" .. tostring(i))		-- 체크 해제된 항목수만큼 visible off 시킴(스텝 제목 텍스트)
			SetBool(WIDGET_ID_QUEST_HELPER, "visible", false, "TitleRadioButton" .. tostring(i))	-- 체크 해제된 항목수만큼 visible off 시킴(스텝 제목 라디오 버튼)
		end
	end
	
	
end
-- 클라이언트 콜이 발생시 해당 내용을 갱신하기 위한 함수
function CF_Quest_Helper_DataSetting( nQuestID )	
	local QuestData = g_MyChar:GettbQuestProcInfo( nQuestID )	-- 현재 스텝의 데이터 내용을 가져온다.
	local Quest_Cond = QuestData["Cond"]						-- 퀘스트 스텝 내용(테이블)을 Quest_Cond 변수에 저장한다.
	if Quest_Cond == nil then
		return
	end
	local Quest_StepTitle = Quest_Cond["StepTitle"]					-- 퀘스트 스텝 제목 내용을 담을 변수
	local Quest_StapDetail = Quest_Helper_DetailData( Quest_Cond )	-- 퀘스트 자세히 내용을 담을 변수

	for i = 1, g_nQuest_Chack_ChackNum, 1 do
		if nQuestID == g_QuestHelper_QuestID[i] then
			g_QuestHelper_StepTitle_Data[i] = Quest_StepTitle
			g_QuestHelper_Detail_Data[i] = Quest_StapDetail
			SetString( WIDGET_ID_QUEST_HELPER, "text", g_QuestHelper_StepTitle_Data[i], "StepTitleText" .. tostring(i) )	-- 퀘스트 스텝 제목 텍스트 출력
			SetString( WIDGET_ID_QUEST_HELPER, "text", g_QuestHelper_Detail_Data[i], "DetailText" .. tostring(i) )			-- 퀘스트 자세히 텍스트 출력
		end
	end
end
-- 퀘스트 창에서 항목 선택시 해당 퀘스트에 대한 라디오 버튼이 선택상태가 되도록 하기위한 함수
function CF_Quest_Helper_QuestSelect( nQuestID )

	for i = 1, g_nQuest_Chack_ChackNum, 1 do
	
		if nQuestID == g_QuestHelper_QuestID[i] then
			SetBool( WIDGET_ID_QUEST_HELPER, "selected", true, "TitleRadioButton" .. tostring(i) )
		else
			SetBool( WIDGET_ID_QUEST_HELPER, "selected", false, "TitleRadioButton" .. tostring(i) )
		end
	end
end
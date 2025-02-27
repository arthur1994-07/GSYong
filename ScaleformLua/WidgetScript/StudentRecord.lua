
STUDENTRECORD_ATTENDANCE_TASK_TOTAL	= 4
STUDENTRECORD_ATTENDANCE_MAX_DAYS = 7
STUDENTRECORD_ACTIVITY_LIST_MAX = 6
STUDENTRECORD_ACTIVITY_GRADE_MAX = 3
STUDENTRECORD_ACTIVITY_CATEGORY_MAX = 6

g_StudentRecord_Activity_Select_Tab = 0
g_StudentRecord_Activity_Select_Grade = 0
g_StudentRecord_Activity_Select_Category = 0

g_StudentRecord_Activity_ProgressDisplayChecked = false
g_IsOpenStudentRecord = false

function IsOpenStudentRecord()
	return g_IsOpenStudentRecord
end

function LoadStudentRecord()

	StudentRecord_Intit()
	
end

function OpenStudentRecord()

	g_IsOpenStudentRecord = true
	
	StudentRecord_AttendancePage_Update()
	StudentRecord_ActivityPage_Update()
	
end

function CloseStudentRecord()

	g_IsOpenStudentRecord = false
	
end

function StudentRecord_Intit()
	
	-- UI 닫기 이벤트
	SetEvent( WIDGET_ID_STUDENT_RECORD, EVENT_WIDGET_CLOSE, "CloseStudentRecord", "", "" )

	if g_UserFeatures:IsUsingAttendance() == false then
		SetBool(WIDGET_ID_STUDENT_RECORD, "visible", false, "AttendanceTab" )
		Ev_StudentRecord_ActivityTab()
	else
		SetBool( WIDGET_ID_STUDENT_RECORD, "visible", false, "ActivityPage" )
		Ev_StudentRecord_AttendanceTab()		
	end
	
	SetEvent( WIDGET_ID_STUDENT_RECORD, EVENT_MOUSE_DOWN, "Ev_StudentRecord_AttendanceTab", "", "AttendanceTab" )
	SetEvent( WIDGET_ID_STUDENT_RECORD, EVENT_MOUSE_DOWN, "Ev_StudentRecord_ActivityTab", "", "ActivityTab" )
	SetString( WIDGET_ID_STUDENT_RECORD, "label", GetGameWordText( "ATTENDANCE_BOOK", 0 ), "AttendanceTab" )
	SetString( WIDGET_ID_STUDENT_RECORD, "label", GetGameWordText( "SPECIAL_ACTIVITY_WINDOW", 0 ), "ActivityTab" )
	
	SetEvent( WIDGET_ID_STUDENT_RECORD, EVENT_SCROLL, "Ev_StudentRecord_Activity_Scroll", "", "ActivityPage.list.list" )

	-- 보상 지급 완료 텍스트
	local str = HTMLFontColor( GetGameWordText( "ATTENDANCE_TASK_TEXT", 1 ), HTML_TEXT_COLOR_PALEGREEN )
	for i = 0, 3, 1 do
		SetString( WIDGET_ID_STUDENT_RECORD, "htmlText", str, "AttendancePage.Task.labelComplete" .. i )
	end

	local tbCategoryList = {}
	local tbScrollList = {}
	local tbGrade = {}
	
	-- 상급 특별활동 트리
	for i = 0, STUDENTRECORD_ACTIVITY_CATEGORY_MAX - 1, 1 do
	
		local tbActivityType = { GetGameWordText( "SPEC_ACT_REPORT_TYPE", i ), 1, 2, i, {}, false }
		table.insert( tbCategoryList, tbActivityType )
		
	end
	
	tbGrade = { GetGameWordText( "SPEC_ACT_REPORT_GRADE", 0 ), 0, 2, -1, tbCategoryList, false }
	table.insert( tbScrollList, tbGrade )
	
	-- 중급 특별활동 트리
	tbCategoryList = {}
	for i = 0, STUDENTRECORD_ACTIVITY_CATEGORY_MAX - 1, 1 do
	
		local tbActivityType = { GetGameWordText( "SPEC_ACT_REPORT_TYPE", i ), 1, 1, i, {}, false }
		table.insert( tbCategoryList, tbActivityType )
		
	end
	tbGrade = { GetGameWordText("SPEC_ACT_REPORT_GRADE", 1), 0, 1, -1, tbCategoryList, false }
	table.insert( tbScrollList, tbGrade )
	
	-- 하급 특별활동 트리
	tbCategoryList = {}
	for i = 0, STUDENTRECORD_ACTIVITY_CATEGORY_MAX - 1, 1 do
	
		local tbActivityType = { GetGameWordText( "SPEC_ACT_REPORT_TYPE", i ), 1, 0, i, {}, false }
		table.insert( tbCategoryList, tbActivityType )
		
	end
	tbGrade = { GetGameWordText( "SPEC_ACT_REPORT_GRADE", 2 ), 0, 0, -1, tbCategoryList, false }
	table.insert( tbScrollList, tbGrade )
	
	SetString( WIDGET_ID_STUDENT_RECORD, "text", 
		GetGameInternalText( "SPEC_ACT_PROGRESSDISPAY_TEXTBOX" ), "ActivityPage.ProgressDisplay_text" )
	
	local strParam = CreateParam( "label", "depth", "grade", "category", "leaf", "likeFolder" )
	SetDataComplex( WIDGET_ID_STUDENT_RECORD, strParam, tbScrollList, "ActivityPage.scrollList", false )
	-- 특별활동 진행상황 보기 체크박스 클릭
	SetEvent( WIDGET_ID_STUDENT_RECORD, EVENT_MOUSE_CLICK, "Ev_StudentRecord_Activity_ProgressDisplayClick", "", "ActivityPage.ProgressDisplay" )
	-- 특별활동 분류목록(상급/중급/하급)클릭
	SetEvent( WIDGET_ID_STUDENT_RECORD, EVENT_LIST_ITEM_CLICK, "Ev_StudentRecord_Activity_CategoryClick", "index", "ActivityPage.scrollList" )
	
	SetEvent( WIDGET_ID_STUDENT_RECORD, EVENT_BUTTON_CLICK, "Ev_StudentRecord_ActivityListTab0", "", "ActivityPage.ListButton0" )
	SetEvent( WIDGET_ID_STUDENT_RECORD, EVENT_BUTTON_CLICK, "Ev_StudentRecord_ActivityListTab1", "", "ActivityPage.ListButton1" )
	SetEvent( WIDGET_ID_STUDENT_RECORD, EVENT_BUTTON_CLICK, "Ev_StudentRecord_ActivityListTab2", "", "ActivityPage.ListButton2" )
	SetEvent( WIDGET_ID_STUDENT_RECORD, EVENT_BUTTON_CLICK, "Ev_StudentRecord_ActivityListTab3", "", "ActivityPage.ListButton3" )
	
	SetEvent( WIDGET_ID_STUDENT_RECORD, EVENT_BUTTON_CLICK, "Ev_StudentRecord_SalesItem0", "", "ActivityPage.SalesItemButton0" )
	SetEvent( WIDGET_ID_STUDENT_RECORD, EVENT_BUTTON_CLICK, "Ev_StudentRecord_SalesItem1", "", "ActivityPage.SalesItemButton1" )
	SetEvent( WIDGET_ID_STUDENT_RECORD, EVENT_BUTTON_CLICK, "Ev_StudentRecord_SalesItem2", "", "ActivityPage.SalesItemButton2" )
	
	SetEvent( WIDGET_ID_STUDENT_RECORD, EVENT_BUTTON_CLICK, "Ev_StudentRecord_HelpOver" , "", "ActivityPage.btnHelp" )
	
	SetString( WIDGET_ID_STUDENT_RECORD, "label", GetGameWordText( "SPEC_ACT_LIST_BUTTON", 0 ), "ActivityPage.ListButton0" )
	SetString( WIDGET_ID_STUDENT_RECORD, "label", GetGameWordText( "SPEC_ACT_LIST_BUTTON", 1 ), "ActivityPage.ListButton1" )
	SetString( WIDGET_ID_STUDENT_RECORD, "label", GetGameWordText( "SPEC_ACT_LIST_BUTTON", 2 ), "ActivityPage.ListButton2" )
	SetString( WIDGET_ID_STUDENT_RECORD, "label", GetGameWordText( "SPEC_ACT_LIST_BUTTON", 3 ), "ActivityPage.ListButton3" )
	SetString( WIDGET_ID_STUDENT_RECORD, "text", GetGameWordText( "SPEC_ACT_WINDOW_RESULT", 0 ), "ActivityPage.CompletedCountTitle" )
	SetString( WIDGET_ID_STUDENT_RECORD, "label", "ITEM", "ActivityPage.SalesItemButton0" )
	SetString( WIDGET_ID_STUDENT_RECORD, "label", "ITEM", "ActivityPage.SalesItemButton1" )
	SetString( WIDGET_ID_STUDENT_RECORD, "label", "ITEM", "ActivityPage.SalesItemButton2" )
	
	SetString( WIDGET_ID_STUDENT_RECORD, "text", GetGameWordText( "SPEC_ACT_PAGE_TEXT", 0 ), "ActivityPage.list.TitletextField1" )
	SetString( WIDGET_ID_STUDENT_RECORD, "text", GetGameWordText( "SPEC_ACT_PAGE_TEXT", 1 ), "ActivityPage.list.TitletextField2" )
	SetString( WIDGET_ID_STUDENT_RECORD, "text", GetGameWordText( "COMPETITION_CTF_BATTLE_STATIC", 3 ), "ActivityPage.list.TitletextField3" )
	SetString( WIDGET_ID_STUDENT_RECORD, "text", GetGameWordText( "SPEC_ACT_PAGE_TEXT", 2 ), "ActivityPage.list.TitletextField4" )
		
end

-- 특별활동 분류목록(상급/중급/하급)클릭
function Ev_StudentRecord_Activity_CategoryClick( nIndex )

	local nGrade = GetDataInt( WIDGET_ID_STUDENT_RECORD, nIndex, "grade", "ActivityPage.scrollList" )
	local nCategory = GetDataInt( WIDGET_ID_STUDENT_RECORD, nIndex, "category", "ActivityPage.scrollList" )
	
	-- 스크롤 위치 초기화
	SetInt( WIDGET_ID_STUDENT_RECORD, "scrollPosition", 0, "ActivityPage.list.list" )
	-- 텝 위치 초기화
	Ev_StudentRecord_ActivityListTab0()
	
	if nCategory < 0  or STUDENTRECORD_ACTIVITY_CATEGORY_MAX <= nCategory then
		return
	end
	
	g_StudentRecord_Activity_Select_Tab = 0
	g_StudentRecord_Activity_Select_Grade = nGrade
	g_StudentRecord_Activity_Select_Category = nCategory
	
	Ev_StudentRecord_ActivityListUpdate()
	
end

-- 특별활동 진행상황 보기 체크박스 클릭
function Ev_StudentRecord_Activity_ProgressDisplayClick( )
	local bSelect = GetBool( WIDGET_ID_STUDENT_RECORD, "selected", "ActivityPage.ProgressDisplay" )
	
	if bSelect == false then
		g_StudentRecord_Activity_ProgressDisplayChecked = true
	elseif bSelect == true then
		g_StudentRecord_Activity_ProgressDisplayChecked = false
	end
end

function StudentRecord_AttendancePage_Update()

	if true == IsOpenStudentRecord() and g_UserFeatures:IsUsingAttendance() then
	
		StudentRecord_AttendancePage_UpdateContents()
		StudentRecord_AttendancePage_UpdateDayList()
		StudentRecord_AttendancePage_UpdateRewardItem()
		
	end
	
end

function StudentRecord_ActivityPage_Update()

	if true == IsOpenStudentRecord() then
	
		local nCountHigh = StringFormat( GetGameWordText("SPEC_ACT_WINDOW_RESULT", 1), g_Activity:GetActivityCompletedCount(2) )
		local nPointHigh = StringFormat( GetGameWordText("SPEC_ACT_WINDOW_RESULT", 2), g_Activity:GetActivityPoint(2) )
		local nCountMedium = StringFormat( GetGameWordText("SPEC_ACT_WINDOW_RESULT", 3), g_Activity:GetActivityCompletedCount(1) )
		local nPointMedium = StringFormat( GetGameWordText("SPEC_ACT_WINDOW_RESULT", 4), g_Activity:GetActivityPoint(1) )
		local nCountLow = StringFormat( GetGameWordText("SPEC_ACT_WINDOW_RESULT", 5), g_Activity:GetActivityCompletedCount(0) )
		local nPointLow = StringFormat( GetGameWordText("SPEC_ACT_WINDOW_RESULT", 6), g_Activity:GetActivityPoint(0) )

		SetString( WIDGET_ID_STUDENT_RECORD, "text", nCountHigh, "ActivityPage.CompletedCount2" )
		SetString( WIDGET_ID_STUDENT_RECORD, "text", nCountMedium, "ActivityPage.CompletedCount1" )
		SetString( WIDGET_ID_STUDENT_RECORD, "text", nCountLow, "ActivityPage.CompletedCount0" )
		SetString( WIDGET_ID_STUDENT_RECORD, "text", nPointHigh, "ActivityPage.ActivityPoint2" )
		SetString( WIDGET_ID_STUDENT_RECORD, "text", nPointMedium, "ActivityPage.ActivityPoint1" )
		SetString( WIDGET_ID_STUDENT_RECORD, "text", nPointLow, "ActivityPage.ActivityPoint0" )
		
		Ev_StudentRecord_ActivityListUpdate()

	end
end

function StudentRecord_AttendancePage_UpdateDayList()

	if IsOpenStudentRecord() == false then
		return
	end
	
	local nDays = g_Attendance:GetProgressDays()	
	for i = 0, nDays, 1 do
		local strInst = "AttendancePage.Day"..tostring(i)
		CallFunction( WIDGET_ID_STUDENT_RECORD, "gotoAndStop", "complete", strInst )
		SetString( WIDGET_ID_STUDENT_RECORD, "text", "DAY " .. tostring( i + 1 ), strInst .. ".textField" )
	end
	
	CallFunction( WIDGET_ID_STUDENT_RECORD, "gotoAndStop", "today", "AttendancePage.Day".. tostring(nDays) )
	SetString( WIDGET_ID_STUDENT_RECORD, "text", "DAY " .. tostring( nDays + 1 ), "AttendancePage.Day".. tostring(nDays) .. ".textField" )
	
	for i = nDays+1, STUDENTRECORD_ATTENDANCE_MAX_DAYS-1, 1 do
		local strInst = "AttendancePage.Day"..tostring(i)
		CallFunction( WIDGET_ID_STUDENT_RECORD, "gotoAndStop", "default", strInst )
		SetString( WIDGET_ID_STUDENT_RECORD, "text", "DAY " .. tostring( i + 1 ), strInst .. ".textField" )
	end
	
	local nCalendar = g_Attendance:GetCalendar()
	SetString( WIDGET_ID_STUDENT_RECORD, "text", tostring(nCalendar), "AttendancePage.textCalendar" )
	
end

function StudentRecord_AttendancePage_UpdateContents()

	if IsOpenStudentRecord() == false then
		return
	end

	for i = 0, STUDENTRECORD_ATTENDANCE_TASK_TOTAL - 1, 1 do
	
		local strInst 			= "AttendancePage.Task.Task" .. i
		local strInstCaption1 	= strInst .. "_Caption"
		local strInstCaption2 	= strInstCaption1 .. ".textField2"
		local strInstProgress 	= strInst .."_Progress"
		local strInstAward 		= strInst .."_Award"
		local strInstSlot 		= strInst .."_Slot"
		local strInstButton		= strInst .."_Button"
		
		local strCaption 	= GetGameWordText("ATTENDANCE_TASK_TEXT", 2)
		local strCaption1 	= g_Attendance:AttendanceUIGetStringCaption_1(i) .. " "
		local strCaption2 	= g_Attendance:AttendanceUIGetStringCaption_2(i)
		local strProgress 	= g_Attendance:AttendanceUIGetStringProgress(i)
		local strAword		= GetGameWordText("ATTENDANCE_AWARD_TEXT", 0)
		local strPoint		= ""
		local tbPoint 		= g_Attendance:GetPointTableInTask(i)
		
		if tbPoint[1] ~= 0 then
			strPoint = StringFormat( GetGameWordText( "ATTENDANCE_AWARD_TEXT", 1 ), tbPoint[1] ).. "  "
		end
		if tbPoint[2] ~= 0 then
			strPoint = strPoint .. StringFormat( GetGameWordText( "ATTENDANCE_AWARD_TEXT", 2 ), tbPoint[2] ).. "  "
		end
		if tbPoint[3] ~= 0 then
			local strMoney = CurrencyFormat( tbPoint[3] )
			strPoint = strPoint .. StringFormat( GetGameWordText( "ATTENDANCE_AWARD_TEXT", 3 ), strMoney ).. "  "
		end
		
		local emState 	= g_Attendance:AttendanceUIGetState(i)
		
		strCaption = StringFormat( strCaption, i+1 )
		if emState == 1 then
		
			strProgress = StringFormat( GetGameWordText("ATTENDANCE_TASK_TEXT", 3), 1 )
			strProgress = HTMLFontColor( strProgress, "#FF820A" )
			strPoint = HTMLFontColor( strPoint, HTML_TEXT_COLOR_GRAY )
			strAword = HTMLFontColor( strAword, HTML_TEXT_COLOR_GRAY )
			strCaption = HTMLFontColor( strCaption, HTML_TEXT_COLOR_GRAY )
			strCaption1 = HTMLFontColor( strCaption1, HTML_TEXT_COLOR_GRAY )
			strCaption2 = HTMLFontColor( strCaption2, HTML_TEXT_COLOR_GRAY )
			SetString( WIDGET_ID_STUDENT_RECORD, "htmlText", strCaption, strInstCaption1 )
			SetString( WIDGET_ID_STUDENT_RECORD, "htmlText", strCaption1..strCaption2, strInstCaption2 )

			strProgress = strProgress .."\n".. strPoint
			
		else
		
			strCaption = HTMLFontColor( strCaption, "#FF820A" )
			strCaption1 = HTMLFontColor( strCaption1, HTML_TEXT_COLOR_WHITE )
			strCaption2 = HTMLFontColor( strCaption2, HTML_TEXT_COLOR_LIGHTSKYBLUE )
			SetString( WIDGET_ID_STUDENT_RECORD, "htmlText", strCaption, strInstCaption1 )
			SetString( WIDGET_ID_STUDENT_RECORD, "htmlText", strCaption1..strCaption2, strInstCaption2 )
			
		end		
		
		SetBool( WIDGET_ID_STUDENT_RECORD, "visible", false, "AttendancePage.Task.labelComplete" .. i )
		SetBool( WIDGET_ID_STUDENT_RECORD, "visible", false, strInstButton )
		
		if emState == 0 then
		
			strProgress = HTMLFontColor( strProgress, HTML_TEXT_COLOR_WHITE )
			strProgress = strProgress .."\n".. strPoint
			
		elseif emState == 2 then
		
			if g_Attendance:AttendanceIsReceivedAwardItem(i) then
			
				SetBool( WIDGET_ID_STUDENT_RECORD, "visible", false, strInstButton )
				strPoint = HTMLFontColor( strPoint, HTML_TEXT_COLOR_GRAY )
				strAword = HTMLFontColor( strAword, HTML_TEXT_COLOR_GRAY )
				
				SetBool( WIDGET_ID_STUDENT_RECORD, "visible", true, "AttendancePage.Task.labelComplete" .. i )
				
			else
			
				SetBool( WIDGET_ID_STUDENT_RECORD, "visible", true, strInstButton )
				strPoint = HTMLFontColor( strPoint, HTML_TEXT_COLOR_WHITE )
				
				SetBool( WIDGET_ID_STUDENT_RECORD, "visible", false, "AttendancePage.Task.labelComplete" .. i )
				
			end
			
			strProgress = HTMLFontColor( strProgress, HTML_TEXT_COLOR_LIGHTSKYBLUE )			
			strProgress = strProgress .."\n".. strPoint
			
		else
		
			strProgress = HTMLFontColor( strProgress, "#FF820A" )
			
		end
		
		SetString( WIDGET_ID_STUDENT_RECORD, "htmlText", strProgress, strInstProgress )
		SetString( WIDGET_ID_STUDENT_RECORD, "htmlText", strAword, strInstAward )
		SetString( WIDGET_ID_STUDENT_RECORD, "label", GetGameWordText( "ATTENDANCE_TASK_TEXT", 0 ), strInstButton )
		
		SetEvent( WIDGET_ID_STUDENT_RECORD, EVENT_BUTTON_CLICK, "Ev_StudentRecord_AwardReceive"..tostring(i), "", strInstButton )
		
	end
	
end

function StudentRecord_AttendancePage_UpdateRewardItem()

	for i = 0, STUDENTRECORD_ATTENDANCE_TASK_TOTAL - 1, 1 do
	
		local strInst = "AttendancePage.Task.Task" .. i .. "_Slot"
		
		local emState = g_Attendance:AttendanceUIGetState( i )
		local dwAwardID = g_Attendance:AttendanceUIGetAwardItem( i )
		local bReceive = g_Attendance:AttendanceIsReceivedAwardItem( i )
		
		local nSlotItemCount = 1
		if true == bReceive and emState == 2 then
			nSlotItemCount = 0
		end
		
		SetString( WIDGET_ID_STUDENT_RECORD, "binding", SLOT_ATTENDANCE_AWARD_ID .. tostring( i ), strInst )
		SetSlot_Item_DWORD( SLOT_ATTENDANCE_AWARD_ID .. tostring( i ) , dwAwardID, nSlotItemCount )
		
	end

end

function Ev_StudentRecord_AttendanceTab()

	SetBool( WIDGET_ID_STUDENT_RECORD, "visible", true, "AttendancePage" )
	SetBool( WIDGET_ID_STUDENT_RECORD, "selected", true, "AttendanceTab" )
	SetBool( WIDGET_ID_STUDENT_RECORD, "visible", false, "ActivityPage" )
	SetBool( WIDGET_ID_STUDENT_RECORD, "selected", false, "ActivityTab" )
	
end

function Ev_StudentRecord_ActivityTab()

	SetBool( WIDGET_ID_STUDENT_RECORD, "visible", false, "AttendancePage" )
	SetBool( WIDGET_ID_STUDENT_RECORD, "selected", false, "AttendanceTab" )
	SetBool( WIDGET_ID_STUDENT_RECORD, "visible", true, "ActivityPage" )
	SetBool( WIDGET_ID_STUDENT_RECORD, "selected", true, "ActivityTab" )

	Ev_StudentRecord_ActivityListTab0()
	
end

function Ev_StudentRecord_Activity_Scroll()

	if g_StudentRecord_tbActivity == nil then
		return
	end

	local nOffset = GetInt( WIDGET_ID_STUDENT_RECORD, "scrollPosition", "ActivityPage.list.list" )
	
	for i = 1, STUDENTRECORD_ACTIVITY_LIST_MAX, 1 do
	
		local strItemInst = "ActivityPage.list.item" .. tostring( i )
		local nIndex = i + nOffset
		
		local tbActivity = g_StudentRecord_tbActivity[ nIndex ]
		if nil ~= tbActivity then
		
			SetBool( WIDGET_ID_STUDENT_RECORD, "visible", true, strItemInst .. ".rewardPoint" )
		
			-- 보상 타이틀이 없으면 보상 타이틀 아이콘 visible off
			if "" ~= tbActivity[ 6 ] then
			
				SetBool( WIDGET_ID_STUDENT_RECORD, "visible", true, strItemInst .. ".rewardTitle" )
			
			else
			
				SetBool( WIDGET_ID_STUDENT_RECORD, "visible", false, strItemInst .. ".rewardTitle" )
			
			end
			
		else
		
			SetBool( WIDGET_ID_STUDENT_RECORD, "visible", false, strItemInst .. ".rewardTitle" )
			SetBool( WIDGET_ID_STUDENT_RECORD, "visible", false, strItemInst .. ".rewardPoint" )
		
		end
		
	end
	
end

function StudentRecord_Open()

	if IsOpenStudentRecord() == false then
		OpenWidget( WIDGET_ID_STUDENT_RECORD )
	end
	
end

function Ev_StudentRecord_AwardReceive0()

	g_Msg:ReqAttendanceTakeReward(0)
	
end

function Ev_StudentRecord_AwardReceive1()

	g_Msg:ReqAttendanceTakeReward(1)
	
end

function Ev_StudentRecord_AwardReceive2()

	g_Msg:ReqAttendanceTakeReward(2)
	
end

function Ev_StudentRecord_AwardReceive3()

	g_Msg:ReqAttendanceTakeReward(3)
	
end

function StudentRecord_ActivityList_ResetButton()

	SetBool(WIDGET_ID_STUDENT_RECORD, "selected", false, "ActivityPage.ListButton0")
	SetBool(WIDGET_ID_STUDENT_RECORD, "selected", false, "ActivityPage.ListButton1")
	SetBool(WIDGET_ID_STUDENT_RECORD, "selected", false, "ActivityPage.ListButton2")
	SetBool(WIDGET_ID_STUDENT_RECORD, "selected", false, "ActivityPage.ListButton3")
	
end

function Ev_StudentRecord_ActivityListTab0()
	-- 스크롤 위치 초기화
	SetInt( WIDGET_ID_STUDENT_RECORD, "scrollPosition", 0, "ActivityPage.list.list" )
	
	StudentRecord_ActivityList_ResetButton()	
	g_StudentRecord_Activity_Select_Tab = 0
	
	SetBool(WIDGET_ID_STUDENT_RECORD, "selected", true, "ActivityPage.ListButton0")
	Ev_StudentRecord_ActivityListUpdate()
	
end

function Ev_StudentRecord_ActivityListTab1()
	-- 스크롤 위치 초기화
	SetInt( WIDGET_ID_STUDENT_RECORD, "scrollPosition", 0, "ActivityPage.list.list" )
	
	StudentRecord_ActivityList_ResetButton()	
	g_StudentRecord_Activity_Select_Tab = 1
	
	SetBool(WIDGET_ID_STUDENT_RECORD, "selected", true, "ActivityPage.ListButton1")
	Ev_StudentRecord_ActivityListUpdate()
	
end

function Ev_StudentRecord_ActivityListTab2()
	-- 스크롤 위치 초기화
	SetInt( WIDGET_ID_STUDENT_RECORD, "scrollPosition", 0, "ActivityPage.list.list" )
	
	StudentRecord_ActivityList_ResetButton()
	g_StudentRecord_Activity_Select_Tab = 2
	
	SetBool(WIDGET_ID_STUDENT_RECORD, "selected", true, "ActivityPage.ListButton2")
	Ev_StudentRecord_ActivityListUpdate()
	
end

function Ev_StudentRecord_ActivityListTab3()
	-- 스크롤 위치 초기화
	SetInt( WIDGET_ID_STUDENT_RECORD, "scrollPosition", 0, "ActivityPage.list.list" )
	
	StudentRecord_ActivityList_ResetButton()
	g_StudentRecord_Activity_Select_Tab = 3
	
	SetBool(WIDGET_ID_STUDENT_RECORD, "selected", true, "ActivityPage.ListButton3")
	Ev_StudentRecord_ActivityListUpdate()
	
end

function Ev_StudentRecord_SalesItem0()

	ActivitySalesItem_Open(0)
	
end

function Ev_StudentRecord_SalesItem1()

	ActivitySalesItem_Open(1)
	
end

function Ev_StudentRecord_SalesItem2()

	ActivitySalesItem_Open(2)
	
end

function Ev_StudentRecord_HelpOver()	

	local strMsg = string.format( "%s\n%s\n%s\n%s\n%s",
		GetGameInternalText( "ACTIVITY_HELP_TEXT", 0 ),
		GetGameInternalText( "ACTIVITY_HELP_TEXT", 1 ),
		GetGameInternalText( "ACTIVITY_HELP_TEXT", 2 ),
		GetGameInternalText( "ACTIVITY_HELP_TEXT", 3 ),
		GetGameInternalText( "ACTIVITY_HELP_TEXT", 4 ) )
	
	HelpTooltip_SetText( strMsg )
	
end

function Ev_StudentRecord_ActivityListUpdate()

	g_StudentRecord_tbActivity = g_Activity:GetActivityList( 
		g_StudentRecord_Activity_Select_Tab, g_StudentRecord_Activity_Select_Grade, g_StudentRecord_Activity_Select_Category )
		
	local tbList = {}
	for value in pairs( g_StudentRecord_tbActivity ) do
	
		local tbActivity = g_StudentRecord_tbActivity[ value ]
		
		local strProgress = ""
		local strOrder = ""
		
		-- 일반
		if 0 == tbActivity[ 1 ] then
		
			-- 레벨 달성
			if 0 == tbActivity[ 3 ] then
			
				strProgress = tostring( tbActivity[ 8 ] ) .. GetGameWordText( "ACTIVITY_STATUS_TEXT", 3 )
			
			-- 그 외
			else
			
				strProgress = tostring( tbActivity[ 8 ] ) .. GetGameWordText( "ACTIVITY_STATUS_TEXT", 4 )
			
			end
		
		-- 진행중
		elseif 1 == tbActivity[ 1 ] then
		
			-- 레벨 달성
			if 0 == tbActivity[ 3 ] then
			
				strProgress = tostring( tbActivity[ 8 ] ) .. GetGameWordText( "ACTIVITY_STATUS_TEXT", 3 )
			
			-- 그 외
			elseif 0 ~= tbActivity[ 8 ] then
			
				strProgress = tostring( tbActivity[ 8 ] ) .. GetGameWordText( "ACTIVITY_STATUS_TEXT", 4 )
			
			end
			
		-- 진행불가
		elseif 2 == tbActivity[ 1 ] then
		
			strProgress = GetGameWordText( "ACTIVITY_STATUS_TEXT", 2 )
		
		-- 완료
		elseif 3 == tbActivity[ 1 ] then
		
			strProgress = GetGameWordText( "ACTIVITY_STATUS_TEXT", 1 )
		
		end
		
		-- All
		if 0 == tbActivity[ 2 ] then
		
			strOrder = GetGameWordText( "SPEC_ACT_SLOT_TEXT", 0 )
		
		-- 선착순
		else
		
			strOrder = tostring( tbActivity[ 2 ] )
		
		end
		
		-- 타이틀
		local strTitle = "[ " .. tbActivity[ 4 ] .. " ]"
		
		-- 보상 포인트
		local strRewardPoint = tbActivity[ 7 ]
		
		-- 보상 타이틀
		local strRewardTitle = tbActivity[ 6 ]
		if 0 == string.len( strRewardTitle ) then
			strRewardTitle = " "
		end
		
		local tbData =
		{
			tbActivity[ 1 ], 	-- 타입
			tbActivity[ 5 ],	-- 내용 삽입
			strOrder,			-- 선착순
			strTitle,			-- 타이틀
			strProgress,		-- 진행상태
			strRewardTitle,		-- 보상 타이틀
			strRewardPoint,		-- 보상 포인트
		}
		table.insert( tbList, tbData )
	
	end
	
	if 0 == table.getn( tbList ) then
	
		RemoveData( WIDGET_ID_STUDENT_RECORD, 0, "ActivityPage.list.list", true )
	
	else
	
		local strParam = CreateParam( "type", "label1", "label2", "label3", "label4", "label5", "label6" )
		SetDataEx( WIDGET_ID_STUDENT_RECORD, strParam, tbList, "ActivityPage.list.list", false )
	
	end
	
	Ev_StudentRecord_Activity_Scroll()
	
end



---------------클라이언트 콜----------------
-- 특별활동 진행보기 정보값 전달받음
function StudentRecord_CFProgressMessege( ProgressStr )
	BasicChat_PrintSystemMsg( CHAT_SYSTEM, ProgressStr, TEXT_COLOR_GREEN )
end
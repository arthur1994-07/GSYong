
g_bDPS_IsOpen = false

-- DPS 창 로드 완료
function LoadDPS()

	-- 이벤트 설정
	SetEvent( WIDGET_ID_DPS, EVENT_LIST_INDEX_CHANGE, "Ev_DPS_SelectListChar", "index", "listChar" )
	SetEvent( WIDGET_ID_DPS, EVENT_WIDGET_CLOSE, "CloseDPS", "", "" )
	
	for i = 0, 4, 1 do
	
		local strIndex = tostring( i )
		SetEvent( WIDGET_ID_DPS, EVENT_BUTTON_CLICK, "Ev_DPS_btnRemoveClick" .. strIndex, "", "btnRemove" .. strIndex )
	
	end

	-- DPS 캐릭터 리스트 업데이트
	DPS_UpdateCharList()
	-- 스킬 리스트 초기화
	DPS_UpdateSkillList( -1 )

end

function OpenDPS()

	g_bDPS_IsOpen = true

end

function CloseDPS()

	g_bDPS_IsOpen = false

end

-- DPS 캐릭터 리스트 업데이트
function DPS_UpdateCharList()

	local tbRecordList = g_DmgRecordMan:GetRecord()
	
	local tbDataProvider = {}
	for value in pairs( tbRecordList ) do
	
		local tbRecord = tbRecordList[ value ]
		
		local strSkill = ""
		if 	-1 == tbRecord[ "MID" ] and 
			-1 == tbRecord[ "SID" ] and 
			-1 == tbRecord[ "nSkillLevel" ] then
			
			strSkill = "--/--(-)"
			
		else
		
			strSkill = string.format( "%d/%d(%d)", tbRecord[ "MID" ], tbRecord[ "SID" ], tbRecord[ "nSkillLevel" ] )
			
		end
		
		local tbData = 
		{
			tbRecord[ "strName" ],
			tostring( tbRecord[ "nDamage" ] ),
			string.format( "%d:%02d", tbRecord[ "nMinute" ], tbRecord[ "nSecond" ] ),
			string.format( "%.2f", tbRecord[ "fDPS" ] ),
			strSkill
		}
		table.insert( tbDataProvider, tbData )
	
	end
	
	local strParam = CreateParam( "label1", "label2", "label3", "label4", "label5" )
	SetDataEx( WIDGET_ID_DPS, strParam, tbDataProvider, "listChar" )

end

-- DPS 스킬 리스트 업데이트
function DPS_UpdateSkillList( nIndex )

	local tbDetailList = g_DmgRecordMan:GetDetail( nIndex )
	
	local tbDataProvider = {}
	for value in pairs( tbDetailList ) do
	
		local tbDetail = tbDetailList[ value ]
		
		local tbData = 
		{
			tbDetail[ "strName" ],
			tostring( tbDetail[ "nDamage" ] ),
			tostring( tbDetail[ "nUseCount" ] ),
			tostring( tbDetail[ "nDPS" ] ),
			string.format( "%3d%%", tbDetail[ "nRate" ] )
		}
		
		table.insert( tbDataProvider, tbData )
	
	end
	
	if 0 ~= table.getn( tbDetailList ) then
	
		local strParam = CreateParam( "label1", "label2", "label3", "label4", "label5" )
		SetDataEx( WIDGET_ID_DPS, strParam, tbDataProvider, "listSkill" )
		
	else
	
		RemoveData( WIDGET_ID_DPS, 0, "listSkill", true )
		
	end

end

-- DPS 리스트 초기화
function DPS_ResetRecord( nIndex )

	g_DmgRecordMan:ResetRecord( nIndex )
	
	-- 리스트 업데이트 해주고 선택 초기화
	DPS_UpdateCharList()
	SetInt( WIDGET_ID_DPS, "selectedIndex", -1, "listChar" )

end

-- DPS 리스트 제거
function DPS_RemoveRecord( nIndex )

	g_DmgRecordMan:RemoveRecord( nIndex )
	
	-- 리스트 업데이트 해주고 선택 초기화
	DPS_UpdateCharList()
	SetInt( WIDGET_ID_DPS, "selectedIndex", -1, "listChar" )

end

-- DPS 리스트 제거/초기화
function DPS_ChooseResetRemove( nIndex )

	-- 0번은 내 캐릭터니까 초기화만
	if 0 == nIndex then
		DPS_ResetRecord( nIndex )
	-- 그 외 삭제
	else
		DPS_RemoveRecord( nIndex )
	end

end

---------------------------------------------------------------------------------------------------------
-- 클라이언트 콜

-- DPS 리스트 업데이트
function DPS_Update()

	if false == g_bDPS_IsOpen then
		return
	end
	
	-- 캐릭터 리스트 업데이트
	DPS_UpdateCharList()
	
	local nSelect = GetInt( WIDGET_ID_DPS, "selectedIndex", "listChar" )
	if -1 ~= nSelect then
		DPS_UpdateSkillList( nSelect )
	end

end

---------------------------------------------------------------------------------------------------------
-- 이벤트

-- 캐릭터 리스트 선택
function Ev_DPS_SelectListChar( nIndex )

	DPS_UpdateSkillList( nIndex )

end

-- 제거 버튼 클릭
function Ev_DPS_btnRemoveClick0()

	local nScrollPos = GetInt( WIDGET_ID_DPS, "scrollPosition", "listChar" )
	DPS_ChooseResetRemove( nScrollPos )

end

function Ev_DPS_btnRemoveClick1()

	local nScrollPos = GetInt( WIDGET_ID_DPS, "scrollPosition", "listChar" )
	DPS_ChooseResetRemove( nScrollPos + 1 )

end

function Ev_DPS_btnRemoveClick2()

	local nScrollPos = GetInt( WIDGET_ID_DPS, "scrollPosition", "listChar" )
	DPS_ChooseResetRemove( nScrollPos + 2 )

end

function Ev_DPS_btnRemoveClick3()

	local nScrollPos = GetInt( WIDGET_ID_DPS, "scrollPosition", "listChar" )
	DPS_ChooseResetRemove( nScrollPos + 3 )

end

function Ev_DPS_btnRemoveClick4()

	local nScrollPos = GetInt( WIDGET_ID_DPS, "scrollPosition", "listChar" )
	DPS_ChooseResetRemove( nScrollPos + 4 )

end
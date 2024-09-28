
--(1, buff skill id)
--(2, buff age)
--(3, buff life)
--(4, buff skill level)
g_tbBMP = {}
g_fBMP_ElapsedTime = 0.0

-- 매번 불러오지 않기 위해 캐싱해둠;
g_nBMPSlotWidth = 0
g_nBMPSlotHeight = 0

function LoadBuffMiniParty()
	-- 슬롯 바인딩, 기본값 세팅	
	local tbProp, tbInst, tbData, tbType = {}, {}, {}, {}
	for i = 1, BUFFMINIPARTY_SLOT_TOTAL, 1 do	
		local strSlot = "slotTimer" .. tostring( i )
		
		PropertyArraySetting(tbProp, "visible", "tooltipEnabled", "maximum", "binding")
		PropertyArraySetting(tbInst, strSlot, strSlot..".slot", strSlot..".bar", strSlot..".slot")
		PropertyArraySetting(tbData, false, false, 100, SLOT_BUFFMINIPARTY_ID .. tostring( i ))
		PropertyArraySetting(tbType, "-b", "-b", "-n", "-s")
		
	end
	SetArray(WIDGET_ID_BUFFMINIPARTY, tbProp, tbData, tbInst, tbType)
	-- 종료 이벤트
	SetEvent( WIDGET_ID_BUFFMINIPARTY, EVENT_WIDGET_CLOSE, "BMP_Close", "", "" )	
	
	g_nBMPSlotWidth = GetInt(WIDGET_ID_BUFFMINIPARTY, "width", "slotTimer1")
	g_nBMPSlotHeight= GetInt(WIDGET_ID_BUFFMINIPARTY, "height","slotTimer1")
	
end

-- 슬롯들 안보이게함;
function BMP_SlotVisibleSetting()
	local tbProp, tbInst, tbData, tbType = {}, {}, {}, {}	
	local nSize = table.getn(g_tbBMP)
	for i = 1, nSize, 1 do
		local strSlot = "slotTimer" .. tostring( i )
		table.insert(tbProp, "visible")
		table.insert(tbInst, strSlot)
		table.insert(tbData, true)
		table.insert(tbType, "-b")
		SetSlot_SkillLevel( SLOT_BUFFMINIPARTY_ID .. tostring( i ), g_tbBMP[i][1], g_tbBMP[i][4] )
	end
	
	for i = nSize+1, BUFFMINIPARTY_SLOT_TOTAL, 1 do	
		local strSlot = "slotTimer" .. tostring( i )
		table.insert(tbProp, "visible")
		table.insert(tbInst, strSlot)
		table.insert(tbData, false)
		table.insert(tbType, "-b")
	end
	SetArray(WIDGET_ID_BUFFMINIPARTY, tbProp, tbData, tbInst, tbType)
end

function OpenBuffMiniParty()
	
	local MousePos = GetMousePos()	--팝업 버튼 커서 위치에서 생성 설정
	local nPosX = MousePos["x"] + 10
	local nPosY = MousePos["y"] + 10
	
	local tbProp, tbInst, tbData, tbType = {}, {}, {}, {}
	PropertyArraySetting(tbProp, "x", "y")
	PropertyArraySetting(tbInst, "", "")
	PropertyArraySetting(tbData, nPosX, nPosY)
	PropertyArraySetting(tbType, "-n", "-n")
	
	SetArray(WIDGET_ID_BUFFMINIPARTY, tbProp, tbData, tbInst, tbType)
	InsertFrameMove(BMP_FrameMove)
	
	BMP_SlotVisibleSetting()
	BMP_BackgroundSetting()
end

function BMP_Close()
	RemoveFrameMove(BMP_FrameMove)
end

function BMP_SetPartyMemberInfo(tbMember)
	g_Client:SfReqPartyMemberGetSkillFactInfo( tbMember[ "dwGaeaID" ] )
end

function MsgPartyMemberSkillFactInfo(tbBuffInfo)
	g_tbBMP = {}
	g_tbBMP = tbBuffInfo
	BMP_SlotVisibleSetting()
	BMP_BackgroundSetting()
	OpenWidget(WIDGET_ID_BUFFMINIPARTY)
end

function BMP_FrameMove(fElapsedTime)
	
	g_fBMP_ElapsedTime = g_fBMP_ElapsedTime + fElapsedTime
	if 0.1 > g_fBMP_ElapsedTime then
		return
	end
	
	local tbProp, tbInst, tbData, tbType = {}, {}, {}, {}	
	-- 버프 갱신 시간을 매번 클라이언트에서 받아오지않는다;
	-- 변화가 있다면 루아에 갱신요청하게하자;
	local nBuffSize = table.getn(g_tbBMP)
	if nBuffSize == 0 or g_tbBMP == nil then
		g_fBMP_ElapsedTime = 0.0
		return 
	end
	
	local i = 1
	while i <= nBuffSize do

		g_tbBMP[i][2] = g_tbBMP[i][2] - g_fBMP_ElapsedTime
		local fAge = g_tbBMP[i][2]
		
		-- 시간이 다되었다면 끈다;
		if fAge <= 0.0 then
			-- 한칸식 땡길고 아이콘 갱신한다;
			for j = i, nBuffSize-1, 1 do
				g_tbBMP[j] = g_tbBMP[j+1]
				SetSlot_SkillLevel( SLOT_BUFFMINIPARTY_ID .. tostring( j ), g_tbBMP[j][1], g_tbBMP[j][4] )
			end
			
			table.insert(tbProp, "visible")
			table.insert(tbInst, "slotTimer" .. tostring(nBuffSize) )
			table.insert(tbData, false)
			table.insert(tbType, "-b")
			table.remove(g_tbBMP, nBuffSize)
			nBuffSize = nBuffSize - 1
			BMP_BackgroundSetting()
			continue
		end
		
		local fLife = g_tbBMP[i][3]
		local fRatio = fAge / fLife * 100
		
		table.insert(tbProp, "value")
		table.insert(tbInst, "slotTimer".. tostring( i ).. ".bar")
		table.insert(tbData, fRatio)
		table.insert(tbType, "-f")
		
		i = i + 1
	end
	
	SetArray(WIDGET_ID_BUFFMINIPARTY, tbProp, tbData, tbInst, tbType)
	if nBuffSize == 0 then
		CloseWidget(WIDGET_ID_BUFFMINIPARTY)	
	end
	g_fBMP_ElapsedTime = 0.0
end

function BMP_BackgroundSetting()
	local nBuffSize = table.getn(g_tbBMP)
	if nBuffSize == 0 then
		SetBool(WIDGET_ID_BUFFMINIPARTY, "visible", false, "mcBackGround")
	end
	
	-- 한줄당 7개
	local nXSize = ((nBuffSize%7) * g_nBMPSlotWidth) + 10	-- 10->Padding
	local nYSize = (math.floor(nBuffSize/7+1) * g_nBMPSlotHeight) + 10	-- 10->Padding
	local tbProp, tbInst, tbData, tbType = {}, {}, {}, {}
	PropertyArraySetting(tbProp, "width", "height", "visible")
	PropertyArraySetting(tbInst, "mcBackGround", "mcBackGround", "mcBackGround")
	PropertyArraySetting(tbData, nXSize, nYSize, true)
	PropertyArraySetting(tbType, "-n", "-n", "-b")
	SetArray(WIDGET_ID_BUFFMINIPARTY, tbProp, tbData, tbInst, tbType)
end



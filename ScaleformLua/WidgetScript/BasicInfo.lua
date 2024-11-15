
g_tbBasicInfo_PrevHP = {}
g_tbBasicInfo_PrevMP = {}
g_tbBasicInfo_PrevSP = {}
g_tbBasicInfo_PrevEXP = {}
g_tbBasicInfo_PrevCP = {}

g_fBasicInfo_ElspTime = 0
g_bBasicInfo_IsOpen = false

-- 드래그 관련 저장 값
g_nBasicInfo_DragMouseX = 0
g_nBasicInfo_DragMouseY = 0
g_nBasicInfo_DragPosX = 0
g_nBasicInfo_DragPosY = 0

-- 캐릭터 기본정보 UI 로드 완료
function LoadBasicInfo()

	SetEvent( WIDGET_ID_BASICINFO, EVENT_WIDGET_CLOSE, "CloseBasicInfo", "", "" )

	-- HP 텍스트 설정
	SetString( WIDGET_ID_BASICINFO, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 6 ), 
		"infoHP.tfTitle" )
	-- MP 텍스트 설정
	SetString( WIDGET_ID_BASICINFO, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 7 ), 
		"infoMP.tfTitle" )
	-- SP 텍스트 설정
	SetString( WIDGET_ID_BASICINFO, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 8 ), 
		"infoSP.tfTitle" )
	-- EXP 텍스트 설정
	SetString( WIDGET_ID_BASICINFO, "text", GetGameWordText( "SELECT_CHARACTER_STATIC", 0 ), 
		"infoEXP.tfTitle" )
	-- CP 텍스트 설정
	SetString( WIDGET_ID_BASICINFO, "text", GetGameWordText( "CHARACTER_STATUS_STATIC", 13 ), 
		"infoCP.tfTitle" )
		
	-- HP 바 설정
	SetInt( WIDGET_ID_BASICINFO, "minimum", 0, "infoHP.barValue" )
	SetInt( WIDGET_ID_BASICINFO, "maximum", 100, "infoHP.barValue" )
	
	-- MP 바 설정
	SetInt( WIDGET_ID_BASICINFO, "minimum", 0, "infoMP.barValue" )
	SetInt( WIDGET_ID_BASICINFO, "maximum", 100, "infoMP.barValue" )
	
	-- SP 바 설정
	SetInt( WIDGET_ID_BASICINFO, "minimum", 0, "infoSP.barValue" )
	SetInt( WIDGET_ID_BASICINFO, "maximum", 100, "infoSP.barValue" )
	
	-- EXP 바 설정
	SetInt( WIDGET_ID_BASICINFO, "minimum", 0, "infoEXP.barValue" )
	SetInt( WIDGET_ID_BASICINFO, "maximum", 100, "infoEXP.barValue" )
	
	-- CP 바 설정
	SetInt( WIDGET_ID_BASICINFO, "minimum", 0, "infoCP.barValue" )
	SetInt( WIDGET_ID_BASICINFO, "maximum", 100, "infoCP.barValue" )
	
	
	SetBool( WIDGET_ID_BASICINFO, "mouseEnabled", false, "infoHP.tfValue" )
	SetBool( WIDGET_ID_BASICINFO, "mouseEnabled", false, "infoMP.tfValue" )
	SetBool( WIDGET_ID_BASICINFO, "mouseEnabled", false, "infoSP.tfValue" )
	SetBool( WIDGET_ID_BASICINFO, "mouseEnabled", false, "infoEXP.tfValue" )
	SetBool( WIDGET_ID_BASICINFO, "mouseEnabled", false, "infoCP.tfValue" )
	
	-- 드래그 시작 이벤트
	SetEvent( WIDGET_ID_BASICINFO, EVENT_MOUSE_DOWN, "Ev_BasicInfo_DragStart", "stageX;stageY", "" )
	
end

function OpenBasicInfo()

	g_bBasicInfo_IsOpen = true

	g_tbBasicInfo_PrevHP = {}
	g_tbBasicInfo_PrevMP = {}
	g_tbBasicInfo_PrevSP = {}
	g_tbBasicInfo_PrevEXP = {}
	g_tbBasicInfo_PrevCP = {}
	
	InsertFrameMove( BasicInfo_FrameMove )

end

function CloseBasicInfo()

	g_bBasicInfo_IsOpen = false

	g_tbBasicInfo_PrevHP = {}
	g_tbBasicInfo_PrevMP = {}
	g_tbBasicInfo_PrevSP = {}
	g_tbBasicInfo_PrevEXP = {}
	g_tbBasicInfo_PrevCP = {}

end

function BasicInfo_FrameMove( fElspTime )

	g_fBasicInfo_ElspTime = g_fBasicInfo_ElspTime + fElspTime
	
	if 0.1 < g_fBasicInfo_ElspTime then
	
		g_fBasicInfo_ElspTime = 0
		
		BasicInfo_UpdateAllInfo()
	
	end

end

function BasicInfo_UpdateAllInfo()

	if false == g_bBasicInfo_IsOpen then
		return
	end
	
	BasicInfo_UpdateHP()
	BasicInfo_UpdateMP()
	BasicInfo_UpdateSP()
	BasicInfo_UpdateEXP()
	BasicInfo_UpdateCP()
	BasicInfo_UpdateLevel()

end

function BasicInfo_UpdateHP()

	local nNow = g_MyChar:GetHP()
	local nMax = g_MyChar:GetMaxHP()
	if 	nNow == g_tbBasicInfo_PrevHP[ 1 ] and
		nMax == g_tbBasicInfo_PrevHP[ 2 ] then
		return
	end

	SetDouble( WIDGET_ID_BASICINFO, "value", BasicInfo_CalcRatio( nNow, nMax ), "infoHP.barValue" )
	SetString( WIDGET_ID_BASICINFO, "text", tostring( nNow ), "infoHP.tfValue" )
	
	SetString( WIDGET_ID_BASICINFO, "TooltipText", 
		tostring( nNow ) .. "/" .. tostring( nMax ), "infoHP.barValue" )
		
	g_tbBasicInfo_PrevHP[ 1 ] = nNow
	g_tbBasicInfo_PrevHP[ 2 ] = nMax

end

function BasicInfo_UpdateMP()

	local nNow = g_MyChar:GetMP()
	local nMax = g_MyChar:GetMaxMP()
	if 	nNow == g_tbBasicInfo_PrevMP[ 1 ] and
		nMax == g_tbBasicInfo_PrevMP[ 2 ] then
		return
	end

	SetDouble( WIDGET_ID_BASICINFO, "value", BasicInfo_CalcRatio( nNow, nMax ), "infoMP.barValue" )
	SetString( WIDGET_ID_BASICINFO, "text", tostring( nNow ), "infoMP.tfValue" )
	
	SetString( WIDGET_ID_BASICINFO, "TooltipText", 
		tostring( nNow ) .. "/" .. tostring( nMax ), "infoMP.barValue" )
		
	g_tbBasicInfo_PrevMP[ 1 ] = nNow
	g_tbBasicInfo_PrevMP[ 2 ] = nMax
	
end

function BasicInfo_UpdateSP()

	local nNow = g_MyChar:GetSP()
	local nMax = g_MyChar:GetMaxSP()
	if 	nNow == g_tbBasicInfo_PrevSP[ 1 ] and
		nMax == g_tbBasicInfo_PrevSP[ 2 ] then
		return
	end

	SetDouble( WIDGET_ID_BASICINFO, "value", BasicInfo_CalcRatio( nNow, nMax ), "infoSP.barValue" )
	SetString( WIDGET_ID_BASICINFO, "text", tostring( nNow ), "infoSP.tfValue" )
	
	SetString( WIDGET_ID_BASICINFO, "TooltipText", 
		tostring( nNow ) .. "/" .. tostring( nMax ), "infoSP.barValue" )
		
	g_tbBasicInfo_PrevSP[ 1 ] = nNow
	g_tbBasicInfo_PrevSP[ 2 ] = nMax
	
end

function BasicInfo_UpdateEXP()

	local nNow = g_MyChar:GetEXP()
	local nMax = g_MyChar:GetMaxEXP()
	if 	nNow == g_tbBasicInfo_PrevEXP[ 1 ] and
		nMax == g_tbBasicInfo_PrevEXP[ 2 ] then
		return
	end
	
	if nNow > nMax then
		nNow = nMax
	end	
	
	local fExpRatio = BasicInfo_CalcRatio( nNow, nMax )
	local strExp = string.format( "%.2f", fExpRatio ) .. "%"
	
	SetDouble( WIDGET_ID_BASICINFO, "value", fExpRatio, "infoEXP.barValue" )
	SetString( WIDGET_ID_BASICINFO, "text", strExp, "infoEXP.tfValue" )
	
	SetString( WIDGET_ID_BASICINFO, "TooltipText", 
		CurrencyFormat( nNow ) .. "/" .. CurrencyFormat( nMax ), "infoEXP.barValue" )
		
	g_tbBasicInfo_PrevEXP[ 1 ] = nNow
	g_tbBasicInfo_PrevEXP[ 2 ] = nMax
	
end

function BasicInfo_UpdateCP()

	local nNow = g_MyChar:GetCP()
	local nMax = g_MyChar:GetMaxCP()
	if 	nNow == g_tbBasicInfo_PrevCP[ 1 ] and
		nMax == g_tbBasicInfo_PrevCP[ 2 ] then
		return
	end

	SetDouble( WIDGET_ID_BASICINFO, "value", BasicInfo_CalcRatio( nNow, nMax ), "infoCP.barValue" )
	SetString( WIDGET_ID_BASICINFO, "text", tostring( nNow ), "infoCP.tfValue" )
	
	SetString( WIDGET_ID_BASICINFO, "TooltipText", 
		tostring( nNow ) .. "/" .. tostring( nMax ), "infoCP.barValue" )
		
	g_tbBasicInfo_PrevCP[ 1 ] = nNow
	g_tbBasicInfo_PrevCP[ 2 ] = nMax
	
end

function BasicInfo_UpdateLevel()

	local strLevel = "Lv <font color=\"" .. HTML_TEXT_COLOR_WHITE .. "\">" .. tostring( g_MyChar:GetLevel() ) .. "</font>"	
	SetString( WIDGET_ID_BASICINFO, "htmlText", strLevel, "infoEXP.tflv" )   --htmlText

end

function BasicInfo_CalcRatio( nNow, nMax )

	return nNow / nMax * 100
	
end

--------------------------------------------------------------------------------------------------------
-- 이벤트

-- 드래그 시작 이벤트
function Ev_BasicInfo_DragStart( nStageX, nStageY )

	InsertStageMouseMove( Ev_BasicInfo_Dragging )
	InsertStageMouseUp( Ev_BasicInfo_DragEnd )
	
	g_nBasicInfo_DragMouseX = nStageX
	g_nBasicInfo_DragMouseY = nStageY
	
	g_nBasicInfo_DragPosX = GetInt( WIDGET_ID_BASICINFO, "x", "" )
	g_nBasicInfo_DragPosY = GetInt( WIDGET_ID_BASICINFO, "y", "" )
	
end

-- 드래그 이동
function Ev_BasicInfo_Dragging( nStageX, nStageY )

	local nOffsetX = nStageX - g_nBasicInfo_DragMouseX
	local nOffsetY = nStageY - g_nBasicInfo_DragMouseY

	local tb = SnapCheck( WIDGET_ID_BASICINFO, g_nBasicInfo_DragPosX + nOffsetX, g_nBasicInfo_DragPosY + nOffsetY )
	
	local nNewX = tb[ 1 ]
	local nNewY = tb[ 2 ]
	
	local nScreenWidth = GetScreenWidth()
	local nScreenHeight = GetScreenHeight()
	
	local nWidth = GetInt( WIDGET_ID_BASICINFO, "width", "" )
	local nHeight = GetInt( WIDGET_ID_BASICINFO, "height", "" )
	
	if 0 > nNewX then
		nNewX = 0
	end
	
	if 0 > nNewY then
		nNewY = 0
	end
	
	if nNewX + nWidth > nScreenWidth then
		nNewX = nScreenWidth - nWidth
	end
	
	if nNewY + nHeight > nScreenHeight then
		nNewY = nScreenHeight - nHeight
	end
	
	SetInt( WIDGET_ID_BASICINFO, "x", nNewX, "" )
	SetInt( WIDGET_ID_BASICINFO, "y", nNewY, "" )

end

-- 드래그 끝
function Ev_BasicInfo_DragEnd( nStageX, nStageY )

	RemoveStageMouseMove( Ev_BasicInfo_Dragging )
	RemoveStageMouseUp( Ev_BasicInfo_DragEnd )

end
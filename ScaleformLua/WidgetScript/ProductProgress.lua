
g_ProductProgress_nProductCount 		= 0		-- 총 제작 개수
g_ProductProgress_nCompleteCount 		= 0		-- 완료된 제작 개수
g_ProductProgress_fDelay				= 0		-- 개당 제작 걸리는 시간
g_ProductProgress_nCompleteTimer 		= 0		-- 완료 개수 타이머
g_ProductProgress_fFps					= 0.1	-- 완료 개수 타이머 간격
g_ProductProgress_strItemName			= ""	-- 아이템 이름


g_nProductProgress_DragMouseX = 0	-- 드래그 관련 저장 값
g_nProductProgress_DragMouseY = 0
g_nProductProgress_DragPosX = 0
g_nProductProgress_DragPosY = 0

g_bProductNeedConfirm = false
g_bProductStart = true

-- 재확인시 사용할 백업용
strConfirmItemName	= ""
nConfirmCount		= 0
fConfirmDelay		= 0


function LoadProductProgress()

	SetEvent( WIDGET_ID_PRODUCT_PROGRESS, EVENT_MOUSE_DOWN, "Ev_ProductProgress_DragStart", "stageX;stageY", "" )

end

function OpenProductProgress()

	-- 제작시간이 짧은 경우 서버 통신, 처리시간에 의해 UI가 오픈되기전에 종료 메세지를 던질 수 있다;
	-- 이때 Open이 이루어지면 버그가 발생한다;
	if g_ProductProgress_nProductCount == 0 or g_ProductProgress_fDelay == 0 then
		CloseWidget( WIDGET_ID_PRODUCT_PROGRESS )
		return
	end
	ProductProgress_StartProduct()
	
end

-- 드래그 시작 이벤트
function Ev_ProductProgress_DragStart( nStageX, nStageY )

	InsertStageMouseMove( Ev_ProductProgress_Dragging )
	InsertStageMouseUp( Ev_ProductProgress_DragEnd )
	
	g_nProductProgress_DragMouseX = nStageX
	g_nProductProgress_DragMouseY = nStageY
	
	g_nProductProgress_DragPosX = GetInt( WIDGET_ID_PRODUCT_PROGRESS, "x", "" )
	g_nProductProgress_DragPosY = GetInt( WIDGET_ID_PRODUCT_PROGRESS, "y", "" )
	
end

-- 드래그 이동
function Ev_ProductProgress_Dragging( nStageX, nStageY )

	local nOffsetX = 0
	local nOffsetY = 0
	nOffsetX = nStageX - g_nProductProgress_DragMouseX
	nOffsetY = nStageY - g_nProductProgress_DragMouseY
	
	local nNewX = 0
	local nNewY = 0
	nNewX, nNewY = PreventOutsideDrag( WIDGET_ID_PRODUCT_PROGRESS,
		g_nProductProgress_DragPosX + nOffsetX, g_nProductProgress_DragPosY + nOffsetY, 
		0, 0, 0, 0 )
		
	SetInt( WIDGET_ID_PRODUCT_PROGRESS, "x", nNewX, "" )
	SetInt( WIDGET_ID_PRODUCT_PROGRESS, "y", nNewY, "" )

end

-- 드래그 끝
function Ev_ProductProgress_DragEnd( nStageX, nStageY )

	RemoveStageMouseMove( Ev_ProductProgress_Dragging )
	RemoveStageMouseUp( Ev_ProductProgress_DragEnd )

end

function ProductProgress_ResetData()
	
	g_ProductProgress_nProductCount 		= 0
	g_ProductProgress_nCompleteCount		= 0
	g_ProductProgress_nCompleteTimer		= 0
	g_ProductProgress_fDelay				= 0

	g_ProductProgress_strItemName			= ""
	g_bProductNeedConfirm = false
	g_bProductStart = true
	--RemoveFrameMove( Ev_ProductProgress_FrameMove )
	
end

function ProductProgress_SetData( strItemName, nCount, fDelay )

	g_ProductProgress_nProductCount 	= nCount
	g_ProductProgress_fDelay 			= fDelay
	g_ProductProgress_strItemName		= strItemName
end

function ProductProgress_StartProduct()
	
	strConfirmItemName	= g_ProductProgress_strItemName 
	nConfirmCount		= g_ProductProgress_nProductCount
	fConfirmDelay		= g_ProductProgress_fDelay

	if IsOpenWidget(WIDGET_ID_PRODUCT_PROGRESS) == false then
		OpenWidget(WIDGET_ID_PRODUCT_PROGRESS)
	end
	local strProductName = g_ProductProgress_strItemName .. " " .. GetGameInternalText("PRODUCT_PROGRESS_TEXT")
	local strProduct = tostring(g_ProductProgress_nCompleteCount) .. " / " .. tostring( g_ProductProgress_nProductCount )
	local nValue = g_ProductProgress_nCompleteCount/g_ProductProgress_nProductCount * 100
	
	SetString( WIDGET_ID_PRODUCT_PROGRESS, "text", strProductName, "productName" )
	SetString( WIDGET_ID_PRODUCT_PROGRESS, "text", strProduct, "productCount" )
	--ProductProgress_CompleteCountTimer()
	
	if g_bProductNeedConfirm == false then
	
		if g_bProductStart == true then 
			SetInt( WIDGET_ID_PRODUCT_PROGRESS, "maximum", 100,"progressBar" )
			SetInt( WIDGET_ID_PRODUCT_PROGRESS, "value", nValue, "progressBar" )
	
			ProductProgress_SetProgressAnim()
			g_bProductStart = false
		end
		
		ProductProgress_CompleteCountTimer()
		SetString( WIDGET_ID_PRODUCT, "label", GetGameWordText( "PRODUCT_BUTTON", 1 ), "Goods.ProductionButton" )
	end
	

end

-- 프로그래스 바 애니메이션 설정
function ProductProgress_SetProgressAnim()
--	if false == IsOpenWidget(WIDGET_ID_PRODUCT_PROGRESS) then
--		return
--	end

	local tbQuickSet = 
	{
		time = ((g_ProductProgress_nProductCount-g_ProductProgress_nCompleteCount) * g_ProductProgress_fDelay * 1000 ),
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = ""
	}
	
	SetTween( WIDGET_ID_PRODUCT_PROGRESS, tbQuickSet, "value", "100", "progressBar" )
	
end

-- 제작 완료 카운트 타이머
function ProductProgress_CompleteCountTimer()

end

-- 제작 완료 카운트 타이머 이벤트
function Ev_ProductProgress_CompleteCountTimer()
end

function ProductSuccess()
	
	-- 개수 카운팅을 위해 하나씩 트윈을 사용하는것은 서버에서 계산과 동기화가 너무 안맞다;
	g_ProductProgress_nCompleteCount = g_ProductProgress_nCompleteCount + 1

	-- 완료 개수 텍스트 설정
	local strProduct = tostring( g_ProductProgress_nCompleteCount ) .. " / " .. 
		tostring( g_ProductProgress_nProductCount )
	SetString( WIDGET_ID_PRODUCT_PROGRESS, "text", strProduct,"productCount" )
		
end

function SetProductNeedConfirm( strItemName, nCount, fDelay )
	SetString( WIDGET_ID_PRODUCT, "label", GetGameWordText( "PRODUCT_BUTTON", 1 ), "Goods.ProductionButton" )
	g_bProductNeedConfirm = true

	strConfirmItemName	= strItemName
	nConfirmCount		= nCount
	fConfirmDelay			= fDelay

	RemoveTween ( WIDGET_ID_PRODUCT_PROGRESS, "" )
	if false == IsOpenWidget(WIDGET_ID_PRODUCT_PROGRESS) then
		return
	end
	RemoveTween ( WIDGET_ID_PRODUCT_PROGRESS, "progressBar" )
	
	ProductProgress_StartProduct()
end

function ResetProductNeedConfirm()
	g_bProductNeedConfirm = false
end

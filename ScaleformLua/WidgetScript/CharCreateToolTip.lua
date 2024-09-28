
function LoadCharCreateToolTip()

	-- 팁 visible off
	SetBool( WIDGET_ID_CHARCREATETOOLTIP, "visible", false, "schoolTip" )
	SetBool( WIDGET_ID_CHARCREATETOOLTIP, "visible", false, "classTip" )

end

-- UI 위치 잡기
function CharCreate_ToolTipPos()

	if false == IsOpenWidget( WIDGET_ID_CHARCREATETOOLTIP ) then
		return
	end

	if true == GetBool( WIDGET_ID_CHARCREATETOOLTIP, "visible", "schoolTip" ) then
	
		-- 학원창
		local nValue = GetInt( WIDGET_ID_CHARCREATE, "x", "" ) - 
			GetInt( WIDGET_ID_CHARCREATETOOLTIP, "width", "" )
		SetInt( WIDGET_ID_CHARCREATETOOLTIP, "x", nValue, "" )
		
		nValue = GetInt( WIDGET_ID_CHARCREATE, "y", "" )
		SetInt( WIDGET_ID_CHARCREATETOOLTIP, "y", nValue, "" )
		
	end
	
end

-- 캐릭터 생성창 툴팁 켜기( nType = 1 : 학원, 2 : 클레스 )
function Ev_CharCreate_ToolTip_Open( nType, strImgParam, strDec )

	local strInst = ""
	local strTitle = ""
	
	if nType == 1 then
	
		strInst = "schoolTip"	
		strTitle = GetGameWordText( "FULL_ACADEMY_NAME", g_CharCreate.m_nSelectSchool )
		
	elseif nType == 2 then
	
		strInst = "classTip"
		strTitle = GetCharClassName( g_CharCreate.m_nSelectClass )
		
	end
	
	SetBool( WIDGET_ID_CHARCREATETOOLTIP, "visible", true, strInst )
	SetString( WIDGET_ID_CHARCREATETOOLTIP, "text", strTitle, strInst .. ".tfTitle" )
	SetString( WIDGET_ID_CHARCREATETOOLTIP, "text", strDec, strInst .. ".tfTip" )
	
	CharCreate_ToolTipPos()
	
end

-- 캐릭터 생성창 툴팁 끄기( nType = 1 : 학원, 2 : 클레스 )
function Ev_CharCreate_ToolTip_Close( nType )

	if nType == 1 then
	
		SetBool( WIDGET_ID_CHARCREATETOOLTIP, "visible", false, "schoolTip" )
		
	elseif nType == 2 then
	
		SetBool( WIDGET_ID_CHARCREATETOOLTIP, "visible", false, "classTip" )
		
	end
	
end
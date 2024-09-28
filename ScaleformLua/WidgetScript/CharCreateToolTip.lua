
function LoadCharCreateToolTip()

	-- �� visible off
	SetBool( WIDGET_ID_CHARCREATETOOLTIP, "visible", false, "schoolTip" )
	SetBool( WIDGET_ID_CHARCREATETOOLTIP, "visible", false, "classTip" )

end

-- UI ��ġ ���
function CharCreate_ToolTipPos()

	if false == IsOpenWidget( WIDGET_ID_CHARCREATETOOLTIP ) then
		return
	end

	if true == GetBool( WIDGET_ID_CHARCREATETOOLTIP, "visible", "schoolTip" ) then
	
		-- �п�â
		local nValue = GetInt( WIDGET_ID_CHARCREATE, "x", "" ) - 
			GetInt( WIDGET_ID_CHARCREATETOOLTIP, "width", "" )
		SetInt( WIDGET_ID_CHARCREATETOOLTIP, "x", nValue, "" )
		
		nValue = GetInt( WIDGET_ID_CHARCREATE, "y", "" )
		SetInt( WIDGET_ID_CHARCREATETOOLTIP, "y", nValue, "" )
		
	end
	
end

-- ĳ���� ����â ���� �ѱ�( nType = 1 : �п�, 2 : Ŭ���� )
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

-- ĳ���� ����â ���� ����( nType = 1 : �п�, 2 : Ŭ���� )
function Ev_CharCreate_ToolTip_Close( nType )

	if nType == 1 then
	
		SetBool( WIDGET_ID_CHARCREATETOOLTIP, "visible", false, "schoolTip" )
		
	elseif nType == 2 then
	
		SetBool( WIDGET_ID_CHARCREATETOOLTIP, "visible", false, "classTip" )
		
	end
	
end
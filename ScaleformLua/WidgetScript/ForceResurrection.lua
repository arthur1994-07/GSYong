
g_funcFR_TimerEvent = nil		-- Ÿ�̸� ������ ȣ��� �Լ�
g_fRessurectionTime = 0

-- ������ �޽��� �ڽ��� �̿��ؼ� �ѷ��ش�;
function LoadForceResurrection()
end

function OpenForceResurrection()
	FR_SetTimer( g_fRessurectionTime )
end

function OpenForceResurrection_SetTime(fTime)
	if false == IsOpenWidget( WIDGET_ID_FORCERESSURECTION ) then
		g_fRessurectionTime = fTime
		OpenWidget( WIDGET_ID_FORCERESSURECTION )
	else
		FR_SetTimer( fTime )
	end
end

-- ������Ȱ���¿����� ��ų��Ȱ
function OpenForceResurrection_Skill()

	OpenMsgBox(
		MSG_BOX_TYPE_OKCANCEL,
		GetGameInternalText( "ForceRebirthSkill", 0 ),
		Ev_Resurrection_ForceAndSkill,
		Event_MsgBox_Close )
		
end

-- Ÿ�̸� ����
function FR_SetTimer( fTime )

	if nil == fTime or "number" ~= type( fTime ) then
		g_fRessurectionTime = 0
		return
	else
		g_fRessurectionTime = fTime
	end
	
	-- Ÿ�̸� ������
	if 0 < g_fRessurectionTime then
	
		-- ���α׷��� �� ���̱�
		SetBool( WIDGET_ID_FORCERESSURECTION, "visible", true, "MB_progressBar" )
		
		SetInt( WIDGET_ID_FORCERESSURECTION, "value", 100, "MB_progressBar.progressbar" )
		
		local tbQuickSet = 
		{
			time = g_fRessurectionTime * 1000,
			delay = 0,
			ease = EASE_NONE,
			loop = false,
			onComplete = ""
		}
		SetTween( WIDGET_ID_FORCERESSURECTION, tbQuickSet, "value", "0", "MB_progressBar.progressbar" )
		
		-- Ÿ�̸� �ؽ�Ʈ ����
		local strTime = tostring( math.ceil( g_fRessurectionTime ) ) .. GetGameWordText( "WAITSERVER_TIMELEFT_UNIT", 0 )
		SetString( WIDGET_ID_FORCERESSURECTION, "text", strTime, "MB_progressBar.tfTimer" )
		
		strTime = StringFormat( GetGameInternalText( "NOTIFY_FORCE_REBIRTH" ), math.ceil( g_fRessurectionTime ) )
		SetString( WIDGET_ID_FORCERESSURECTION, "text", strTime, "textField" )
		-- �ʴ� Ÿ�̸� ����
		FR_SetSecondTimer()
		
	-- Ÿ�̸� ����
	else	
		-- ���α׷��� �����
		SetBool( WIDGET_ID_FORCERESSURECTION, "visible", false, "MB_progressBar" )
		
	end	
end

-- �ʴ� Ÿ�̸� ����
function FR_SetSecondTimer()

	local tbQuickSet = 
	{
		time = 1000,
		delay = 0,
		ease = EASE_NONE,
		loop = false,
		onComplete = "Ev_FR_SecTimer"
	}
	SetTween( WIDGET_ID_FORCERESSURECTION, tbQuickSet, "", "", "" )

end

function Ev_FR_SecTimer()
	-- 1�� ����
	g_fRessurectionTime = g_fRessurectionTime - 1

	if 0 <= g_fRessurectionTime then
	
		local strTime = tostring( math.ceil( g_fRessurectionTime ) ) .. GetGameWordText( "WAITSERVER_TIMELEFT_UNIT", 0 )
		SetString( WIDGET_ID_FORCERESSURECTION, "text", strTime, "MB_progressBar.tfTimer" )
		
		strTime = StringFormat( GetGameInternalText( "NOTIFY_FORCE_REBIRTH" ), math.ceil( g_fRessurectionTime ) )
		SetString( WIDGET_ID_FORCERESSURECTION, "text", strTime, "textField" )
		
		FR_SetSecondTimer()
	
	else
	
		g_fRessurectionTime = 0
		CloseWidget(WIDGET_ID_FORCERESSURECTION)
	
		-- Ÿ�̸� �̺�Ʈ�� ��ϵǾ� �ִٸ� �ش� �̺�Ʈ ȣ��
		if nil ~= g_funcFR_TimerEvent then
			g_funcFR_TimerEvent()
		end
	
	end
end

-- ������Ȱ �� ��ų��Ȱ�϶��� �̺�Ʈ
function Ev_Resurrection_ForceAndSkill()

	g_Msg:ResurrectSkill()
	CloseWidget( WIDGET_ID_FORCERESSURECTION )
	
	Event_MsgBox_Close()
	
end
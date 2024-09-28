
--------------------------------------------------------------------------------
STRING_PROGRESSBAR_TYPE_NONE				= 0
STRING_PROGRESSBAR_TYPE_CHANGE_CHARSLOT		= 1

--------------------------------------------------------------------------------
-- String ProgressBar
StringProgressBar = {}
StringProgressBar.new = function ( )

	----------------------------------------------------------------------------
	-- String ProgressBar instance
	local instance = {}
	
	
	----------------------------------------------------------------------------
	-- Private Variable
	local m_nAllTime = 0
	local m_nTime = 0
	local m_strText = ""
	
	local m_nUIType = STRING_PROGRESSBAR_TYPE_NONE
	
	local m_functionCompleteTimer = FunctionTrigger.new()
	
	
	----------------------------------------------------------------------------
	-- Private Function List
	local SetTimer = function ( )
	
		local tbQuickSet = 
		{
			time = 1000,
			delay = 0,
			ease = EASE_NONE,
			loop = false,
			onComplete = "Ev_StringProgressBar_Timer"
		}
		SetTween( WIDGET_ID_STRING_PROGRESSBAR, tbQuickSet, "", "", "" )
	
	end
	
	local CompleteTimer = function ( )
	
		CloseWidget( WIDGET_ID_STRING_PROGRESSBAR )
		
		m_functionCompleteTimer.ExecuteFunction()
		
		instance.Reset()
		
	end
	
	local StartAnimation_ProgressBar = function ( )
	
		local tbQuickSet = 
		{
			time = m_nAllTime * 1000,
			delay = 0,
			ease = EASE_NONE,
			loop = false,
			onComplete = ""
		}
		
		SetTween( WIDGET_ID_STRING_PROGRESSBAR, tbQuickSet, "value", "100", "progressBar" )
		
	end
	
	
	----------------------------------------------------------------------------
	-- Function List
	instance.Initialize = function ( _nTime, _strText, _nUIType )
	
		instance.Reset()
	
		m_nAllTime = _nTime
		m_nTime = _nTime
		m_strText = _strText
		
		m_nUIType = _nUIType
		
	end
	
	instance.Reset = function ( )
	
		if true == IsOpenWidget( WIDGET_ID_STRING_PROGRESSBAR ) then
		
			CloseWidget( WIDGET_ID_STRING_PROGRESSBAR )
			
		end
		
		if STRING_PROGRESSBAR_TYPE_CHANGE_CHARSLOT == m_nUIType and
			true == IsOpenWidget( WIDGET_ID_CHARINFO ) then
		
			CharInfo_Update()
			
		end
		
		m_nAllTime = 0
		m_nTime = 0
		m_strText = ""
		
		m_nUIType = STRING_PROGRESSBAR_TYPE_NONE
		
		-- Initialize Trigger
		m_functionCompleteTimer = nil
		m_functionCompleteTimer = FunctionTrigger.new()
		
		-- Initialize Animation
		SetInt( WIDGET_ID_STRING_PROGRESSBAR, "value", 0, "progressBar" )
		
	end
	
	instance.Open = function ( )
	
		if false == IsOpenWidget( WIDGET_ID_STRING_PROGRESSBAR ) then
		
			OpenWidget( WIDGET_ID_STRING_PROGRESSBAR )
			
		end
	
	end
	
	instance.CompleteOpen = function ( )
	
		SetString( WIDGET_ID_STRING_PROGRESSBAR, "text", m_strText.." : "..tostring( m_nTime ), "skill_type_countdown" )
		SetTimer()
		
		StartAnimation_ProgressBar()
		
	end
	
	instance.Update = function ( )
	
		if false == IsOpenWidget( WIDGET_ID_STRING_PROGRESSBAR ) then
		
			instance.Reset()
			return
			
		end
		
		if false == g_MyChar:IsIdle() then
			
			instance.Reset()
			return
			
		end
		
		m_nTime = m_nTime - 1
		
		SetString( WIDGET_ID_STRING_PROGRESSBAR, "text", m_strText.." : "..tostring( m_nTime ), "skill_type_countdown" )
		
		if ( 0 == m_nTime ) then
		
			CompleteTimer()
		
		else
		
			SetTimer()
			
		end
		
	end
	
	instance.SetTrigger_Function = function ( _func )
		
		m_functionCompleteTimer.SetFunction( _func )
		
	end
	
	instance.SetTrigger_InsertValue = function ( _value )
	
		m_functionCompleteTimer.InsertValue( _value )
		
	end
	
	instance.GetUIType = function ( )
	
		return m_nUIType
		
	end
	
	----------------------------------------------------------------------------
	
	return instance
	
end


--------------------------------------------------------------------------------
-- 앞으로 StringProgressBar 를 이용할 때 사용하면 된다
g_STRING_PROGRESS_BAR = StringProgressBar.new()


--------------------------------------------------------------------------------
-- Callback Function : Load StringProgressBar
function LoadStringProgressBar ( )
	
end


--------------------------------------------------------------------------------
-- Callback Function : Open StringProgressBar
function OpenStringProgressBar ( )

	g_STRING_PROGRESS_BAR.CompleteOpen()
	
end


--------------------------------------------------------------------------------
-- Event : Timer ( per second )
function Ev_StringProgressBar_Timer ( )

	g_STRING_PROGRESS_BAR.Update()
	
end


--------------------------------------------------------------------------------
#pragma once

#include "../Util/UIGroupHelper.h"

#include "../../../RanLogic/Country/GLCountryDefine.h"

class COuterInterface;
class EngineDeviceMan;

class CBasicTextBox;

// 대륙 리스트의 Property;
class CSelectContinentListProperty : public CUIGroupHelper
{
public:
	enum PROPERTY_STATE
	{
		ESTATE_INACTIVE,
		ESTATE_ACTIVE
	};

	enum
	{
		EPROPERTY_CLIECK_EVENT = NO_ID + 1
	};

	struct SPROPERTY_INFO
	{
		Country::CONTINENT_ID dwContinentID;
		std::string strContinentName;
	};

public:
	CSelectContinentListProperty ( COuterInterface* pInterface
		, EngineDeviceMan* pEngineDevice );
	virtual ~CSelectContinentListProperty ();

public:
	virtual void CreateSubControl();

protected:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

protected:
	virtual void SetPropertyState ( PROPERTY_STATE state );

public:
	void SetPropertyInfo ( const SPROPERTY_INFO& sPropertInfo );

public:
	inline const SPROPERTY_INFO& GetPropertyInfo () const { return m_sPropertInfo; }

protected:
	COuterInterface* m_pInterface;

	CBasicTextBox* m_pContinentNameTextBox;
	SPROPERTY_INFO m_sPropertInfo;

	PROPERTY_STATE m_state;
};
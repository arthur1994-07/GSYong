#pragma once
//
//mjeon.activities
//
#include <set>

#include "./ActivityBase.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../Character/GLCharData.h"

//
// 차후에 .ini나 .xml로 별도 관리해도 된다.
//
enum ACTIVITY_VERIFY_RANGE
{
	ACTIVITYMID_MIN	= 0,
	ACTIVITYMID_MAX	= 65535,

	ACTIVITYSID_MIN	= 0,
	ACTIVITYSID_MAX	= 65535,

	TARGETMID_MIN	= 0,
	TARGETMID_MAX	= 65535,

	TARGETSID_MIN	= 0,
	TARGETSID_MAX	= 65535,

	REQUIRED_MIN	= 0,
	REQUIRED_MAX	= 65535,

	CLASS_MIN		= ACTIVITY_CLASS_LOW,
	CLASS_MAX		= ACTIVITY_CLASS_HIGH,

	NOTIFY_MIN		= ACTIVITY_NOTIFY_NONE,
	NOTIFY_MAX		= ACTIVITY_NOTIFY_CLUBANDAROUND,

	BYORDER_MIN		= 0,
	BYORDER_MAX		= 65535,

	REWARDPOINT_MIN	= 0,
	REWARDPOINT_MAX	= 65535,

	VALIDATE_MIN	= 0,
	VALIDATE_MAX	= 1,

	REWARDTITLE_ID_MAXLENGTH	= EM_TITLEID_MAXLENGTH,
	TITLE_ID_MAXLENGTH			= EM_TITLEID_MAXLENGTH,
	DESC_ID_MAXLENGTH			= EM_TITLEID_MAXLENGTH,

    REWARDTITLE_MAXLENGTH	    = EM_TITLE_MAXLENGTH,
    TITLE_MAXLENGTH			    = 64,
    DESC_MAXLENGTH			    = 256,
};

typedef std::vector<std::string>			VECISSUES;
typedef std::vector<std::string>::iterator	ITER_VECISSUES;

typedef std::set<DWORD>						SETIDS;
typedef std::set<DWORD>::iterator			ITER_SETIDS;


class ActivityVerifier
{
public:
	ActivityVerifier();
	virtual ~ActivityVerifier();

	BOOL Verify(std::string &xml, ENUM_XML_ENCODING_TYPE encoded = XML_ENCODING_UTF8);

	
	BOOL IsVerifiedOK()
	{
		return m_bVerifiedOK;
	}

	VECISSUES& GetIssues()
	{
		return m_vecIssues;
	}

	void GetIssues(std::string &strIssues);
    void Clear();


private:	

	void Issue(std::string &strIssue);

	//
	// do not use following methods directly because there could be some mis-use of them.
	//  just verify with full-xml-string with Verify() method.
	//
	BOOL VerifyMID(int nValue)
	{
		return VerifyNumeric(nValue, ACTIVITYMID_MIN, ACTIVITYMID_MAX, std::string("MID"));
	}

	BOOL VerifySID(int nValue)
	{
		return VerifyNumeric(nValue, ACTIVITYSID_MIN, ACTIVITYSID_MAX, std::string("SID"));
	}

	BOOL VerifyTargetMID(int nValue)
	{
		return VerifyNumeric(nValue, TARGETMID_MIN, TARGETMID_MAX, std::string("TargetMID"));
	}

	BOOL VerifyTargetSID(int nValue)
	{
		return VerifyNumeric(nValue, TARGETSID_MIN, TARGETSID_MAX, std::string("TargetSID"));
	}

	BOOL VerifyRequired(int nValue)
	{
		return VerifyNumeric(nValue, REQUIRED_MIN, REQUIRED_MAX, std::string("Required"));
	}

	BOOL VerifyClass(int nValue)
	{
		return VerifyNumeric(nValue, CLASS_MIN, CLASS_MAX, std::string("Class"));
	}

	BOOL VerifyNotify(int nValue)
	{
		return VerifyNumeric(nValue, NOTIFY_MIN, NOTIFY_MAX, std::string("Notify"));
	}

	BOOL VerifyByOrder(int nValue)
	{
		return VerifyNumeric(nValue, BYORDER_MIN, BYORDER_MAX, std::string("ByOrder"));
	}

	BOOL VerifyRewardPoint(int nValue)
	{
		return VerifyNumeric(nValue, REWARDPOINT_MIN, REWARDPOINT_MAX, std::string("RewardPoint"));
	}

	BOOL VerifyValidate(int nValue)
	{
		return VerifyNumeric(nValue, VALIDATE_MIN, VALIDATE_MAX, std::string("Validate"));
	}

	BOOL VerifyRewardTitle(std::string &strValue)
	{
		return VerifyStringLength(strValue, REWARDTITLE_MAXLENGTH, std::string("RewardTitle"));
	}

	BOOL VerifyTitle(std::string &strValue)
	{
		return VerifyStringLength(strValue, TITLE_MAXLENGTH, std::string("Title"));
	}

	BOOL VerifyDesc(std::string &strValue)
	{
		return VerifyStringLength(strValue, DESC_MAXLENGTH, std::string("Desc"));
	}

    BOOL VerifyRewardTitleID(std::string &strValue)
    {
        return VerifyStringLength(strValue, REWARDTITLE_ID_MAXLENGTH, std::string("RewardTitle ID"));
    }

    BOOL VerifyTitleID(std::string &strValue)
    {
        return VerifyStringLength(strValue, TITLE_ID_MAXLENGTH, std::string("Title ID"));
    }

    BOOL VerifyDescID(std::string &strValue)
    {
        return VerifyStringLength(strValue, DESC_ID_MAXLENGTH, std::string("Desc ID"));
    }
	
	//
	//core-methods for VerifyXXX methods
	//
	BOOL VerifyNumeric(int nValue, int nMin, int nMax, std::string &hint);
	BOOL VerifyStringLength(std::string &strValue, int nMaxLength, std::string &hint);
	
	BOOL VerifyDuplicatedID(DWORD dwID);
	BOOL VerifyDuplicatedTitle(std::string strTitle);


private:

    SNATIVEID       m_sActID;
	BOOL			m_bVerifiedOK;	//there's no problem
	
	std::vector<std::string>	m_vecIssues;
	
	std::set<DWORD>				m_setIDs;
	std::set<std::string>		m_setTitles;
};
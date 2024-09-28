#pragma once

//--------------------------------------------------------------------------------------------//

#include "../../SigmaCore/Xml/2.5.3/ticpp.h"

//--------------------------------------------------------------------------------------------//

#include "UIInfoCfg_TypeTraits.h"

//--------------------------------------------------------------------------------------------//

#include <string>

//--------------------------------------------------------------------------------------------//

class CUIInfoCfg;

namespace NSUIINFOCFG
{
    class CONTROL
    {
        friend class CUIInfoCfg;

    private:
        ticpp::Element m_ELEMENT;
        DWORD          m_dwVERSION;
		bool		   m_bINIT;

    public:
        const bool  Empty  () { return !(m_bINIT); }
        const DWORD Version() { return m_dwVERSION;   }

    public:
        template <typename T>         const bool GetValue     ( const std::string& strID, T& OutValue );
        template <typename T>         const bool GetValueAttr ( const std::string& strID, const std::string& strAttr, T& OutValue );
        template <typename _ATTRLIST> const bool GetValueAttr ( const std::string& strID, _ATTRLIST& OutValue );

    public:
        template <typename T>         const bool SetValue     ( const std::string& strID, const T& InValue );
        template <typename T>         const bool SetValueAttr ( const std::string& strID, const std::string& strAttr, const T& InValue );
        template <typename _ATTRLIST> const bool SetValueAttr ( const std::string& strID, const _ATTRLIST& InValue );

    private:
        template <typename _ATTRLIST, int N> const bool _GetValueAttr ( const std::string& strID, _ATTRLIST& OutValue,      NType<N> );
        template <typename _ATTRLIST, int N> const bool _GetValueAttr ( const std::string& strID, _ATTRLIST& OutValue,      NType<0> );
        template <typename _ATTRLIST, int N> const bool _SetValueAttr ( const std::string& strID, const _ATTRLIST& InValue, NType<N> );
        template <typename _ATTRLIST, int N> const bool _SetValueAttr ( const std::string& strID, const _ATTRLIST& InValue, NType<0> );

	private:
		void writeLogError( const std::string& strError );

    private:
        CONTROL ( const ticpp::Element* _pELEMENT, const DWORD _dwVERSION=0 )
			: m_bINIT(false)
        {
			if ( _pELEMENT )
			{
				m_ELEMENT   = (*_pELEMENT);
				m_dwVERSION = _dwVERSION; 
				m_bINIT		= true;
			}
        }
    };
};

//--------------------------------------------------------------------------------------------//

class IUIInfoCfg
{

public:
    IUIInfoCfg ();
    virtual ~IUIInfoCfg () = 0;

public:
    virtual NSUIINFOCFG::CONTROL LoadControlInfo ( const std::string& strID )                          = 0;
    virtual NSUIINFOCFG::CONTROL SaveControlInfo ( const std::string& strID, const DWORD dwVersion=0 ) = 0;

public:
    static IUIInfoCfg* GetInstance ();

};

//--------------------------------------------------------------------------------------------//

namespace NSUIINFOCFG
{
    static NSUIINFOCFG::CONTROL LOAD_CONTROL ( const std::string& strID )                          { return IUIInfoCfg::GetInstance()->LoadControlInfo(strID);            }
    static NSUIINFOCFG::CONTROL SAVE_CONTROL ( const std::string& strID, const DWORD dwVersion=0 ) { return IUIInfoCfg::GetInstance()->SaveControlInfo(strID, dwVersion); }
};


//--------------------------------------------------------------------------------------------//

#include "UIInfoCfg_Interface.inl"

//--------------------------------------------------------------------------------------------//

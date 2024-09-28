
#pragma once

template <typename T>         
const bool NSUIINFOCFG::CONTROL::GetValue ( const std::string& strID, T& OutValue )
{
    if ( Empty() )
    {
        return false;
    }

    try {
        ticpp::Element* pElem = (ticpp::Element*) m_ELEMENT.FirstChild( strID );
        pElem->GetAttribute( "VALUE", &OutValue );
    } 
    catch (ticpp::Exception& ex)
    {
        // Report the error
        writeLogError(ex.what());
        writeLogError(
            sc::string::format(
                "NSUIINFOCFG::CONTROL::::GetValue () - ID:%1% failed", strID));
        return false;
    }

    return true;
}

template <typename T>         
const bool NSUIINFOCFG::CONTROL::GetValueAttr ( const std::string& strID, const std::string& strAttr, T& OutValue )
{
    if ( Empty() )
    {
        return false;
    }

    try {
        ticpp::Element* pElem = (ticpp::Element*) m_ELEMENT.FirstChild( strID );
        pElem->GetAttribute( strAttr, &OutValue );
    } 
    catch (ticpp::Exception& ex)
    {
        // Report the error
        writeLogError(ex.what());
        writeLogError(
            sc::string::format(
                "NSUIINFOCFG::CONTROL::::GetValueAttr () - ID:%1% Attr:%2% failed",
                strID, strAttr));
        return false;
    }

    return true;
}

template <typename _ATTRLIST> 
const bool NSUIINFOCFG::CONTROL::GetValueAttr ( const std::string& strID, _ATTRLIST& OutValue )
{
    return _GetValueAttr<_ATTRLIST, _ATTRLIST::Length> ( strID, OutValue, NSUIINFOCFG::NType<_ATTRLIST::Length>() );   
}

template <typename _ATTRLIST, int N> 
const bool NSUIINFOCFG::CONTROL::_GetValueAttr ( const std::string& strID, _ATTRLIST& OutValue, NSUIINFOCFG::NType<N> )
{
    _GetValueAttr<_ATTRLIST, N-1> ( strID, OutValue, NSUIINFOCFG::NType<N-1>() );

    typedef typename _ATTRLIST::ELEMENT<N-1>::type _ATTRTYPE;
    _ATTRTYPE& cAttr = OutValue.GetAttr<N-1, _ATTRTYPE>();
    return GetValueAttr ( strID, cAttr.GetID(), cAttr.GetValue() );
}

template <typename _ATTRLIST, int N>        
const bool NSUIINFOCFG::CONTROL::_GetValueAttr ( const std::string& strID, _ATTRLIST& OutValue, NSUIINFOCFG::NType<0> )
{
    return true;
}

template <typename T>         
const bool NSUIINFOCFG::CONTROL::SetValue ( const std::string& strID, const T& InValue )
{
    if ( Empty() )
    {
        return false;
    }

    try {
        ticpp::Element* pElem = (ticpp::Element*) m_ELEMENT.FirstChild( strID, false );

		ticpp::Element cElem(strID);

        if ( !pElem )
        {
            pElem = (ticpp::Element*) m_ELEMENT.LinkEndChild( &cElem );
        }

        pElem->SetAttribute( "VALUE", InValue );
    } 
    catch (ticpp::Exception& ex)
    {
        // Report the error
        writeLogError(ex.what());
        writeLogError(
            sc::string::format(
                "NSUIINFOCFG::CONTROL::SetValue () - ID:%1% failed", strID));        
        return false;
    }

    return true;
}

template <typename T>         
const bool NSUIINFOCFG::CONTROL::SetValueAttr ( const std::string& strID, const std::string& strAttr, const T& InValue )
{
    if ( Empty() )
    {
        return false;
    }

    try {
        ticpp::Element* pElem = (ticpp::Element*) m_ELEMENT.FirstChild( strID, false );

		ticpp::Element cElem(strID);

        if ( !pElem )
        {
            pElem = (ticpp::Element*) m_ELEMENT.LinkEndChild( &cElem );
        }

        pElem->SetAttribute( strAttr, InValue );
    } 
    catch (ticpp::Exception& ex)
    {
        // Report the error
        writeLogError(ex.what());
        writeLogError(
            sc::string::format(
                "NSUIINFOCFG::CONTROL::SetValueAttr() - ID:%1% Attr:%2% failed",
                strID,
                strAttr));
        return false;
    }

    return true;
}

template <typename _ATTRLIST> 
const bool NSUIINFOCFG::CONTROL::SetValueAttr ( const std::string& strID, const _ATTRLIST& InValue )
{
    return _SetValueAttr<_ATTRLIST, _ATTRLIST::Length> ( strID, InValue, NSUIINFOCFG::NType<_ATTRLIST::Length>() );   
}

template <typename _ATTRLIST, int N> 
const bool NSUIINFOCFG::CONTROL::_SetValueAttr ( const std::string& strID, const _ATTRLIST& InValue, NSUIINFOCFG::NType<N> )
{
    _SetValueAttr<_ATTRLIST, N-1> ( strID, InValue, NSUIINFOCFG::NType<N-1>() );

    typedef typename const _ATTRLIST::ELEMENT<N-1>::type _ATTRTYPE;
    _ATTRTYPE& cAttr = InValue.GetAttr<N-1, _ATTRTYPE>();
    return SetValueAttr ( strID, cAttr.GetcID(), cAttr.GetcValue() );
}

template <typename _ATTRLIST, int N>        
const bool NSUIINFOCFG::CONTROL::_SetValueAttr ( const std::string& strID, const _ATTRLIST& InValue, NSUIINFOCFG::NType<0> )
{
    return true;
}


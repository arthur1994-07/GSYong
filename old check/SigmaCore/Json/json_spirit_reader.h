#ifndef JASON_SPIRIT_READER
#define JASON_SPIRIT_READER

/* Copyright (c) 2007-2009 John W Wilkinson

   json spirit version 4.00

   This source code can be used for any purpose as long as
   this comment is retained. */

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "json_spirit_value.h"
#include <iostream>

namespace json_spirit
{
    // An Error_position exception is thrown by the "read_or_throw" functions below on finding an error.
    // Note the "read_or_throw" functions are around 3 times slower than the standard functions "read" 
    // functions that return a bool.
    //
    struct Error_position
    {
        Error_position();
        Error_position( unsigned int line, unsigned int column, const std::string& reason );
        bool operator==( const Error_position& lhs ) const;
        unsigned int line_;
        unsigned int column_;
        std::string reason_;
    };

    // functions to reads a JSON values

    bool read( const std::string& s, Value& value );
    bool read( std::istream&  is,    Value& value );
    bool read( std::string::const_iterator& begin, std::string::const_iterator end, Value& value );

    void read_or_throw( const std::string& s, Value& value );  
    void read_or_throw( std::istream&  is,    Value& value );
    void read_or_throw( std::string::const_iterator& begin, std::string::const_iterator end, Value& value );

#ifndef BOOST_NO_STD_WSTRING

    bool read( const std::wstring& s, wValue& value );
    bool read( std::wistream&  is,    wValue& value );
    bool read( std::wstring::const_iterator& begin, std::wstring::const_iterator end, wValue& value );    

    void read_or_throw( const std::wstring& s, wValue& value );
    void read_or_throw( std::wistream&  is,    wValue& value );
    void read_or_throw( std::wstring::const_iterator& begin, std::wstring::const_iterator end, wValue& value );

#endif

    bool read( const std::string& s, mValue& value );
    bool read( std::istream&  is,    mValue& value );
    bool read( std::string::const_iterator& begin, std::string::const_iterator end, mValue& value );

    void read_or_throw( const std::string& s, mValue& value );  
    void read_or_throw( std::istream&  is,    mValue& value );
    void read_or_throw( std::string::const_iterator& begin, std::string::const_iterator end, mValue& value );

#ifndef BOOST_NO_STD_WSTRING

    bool read( const std::wstring& s, wmValue& value );
    bool read( std::wistream&  is,    wmValue& value );
    bool read( std::wstring::const_iterator& begin, std::wstring::const_iterator end, wmValue& value );    

    void read_or_throw( const std::wstring& s, wmValue& value );
    void read_or_throw( std::wistream&  is,    wmValue& value );
    void read_or_throw( std::wstring::const_iterator& begin, std::wstring::const_iterator end, wmValue& value );

#endif
}

#endif

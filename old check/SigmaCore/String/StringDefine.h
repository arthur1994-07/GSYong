#ifndef SC_STRING_DEFINE_H_
#define SC_STRING_DEFINE_H_

#pragma once
namespace sc {
    namespace string {

    enum EM_CHAR_NATION {
        CHAR_NUMBER   = 0,
        CHAR_ENGLISH  = 1,
        CHAR_HANGUL   = 2,
        CHAR_JAPAN    = 3,
        CHAR_CHINA    = 4,
        CHAR_THAI     = 5,
        CHAR_SYMBOL   = 100,

        CHAR_SPACE    = 0x0020, // ' ' space
        CHAR_EXMARK   = 0x0021, // '!' exclamation mark
        CHAR_QTMARK   = 0x0022, // '"' quotation mark
        CHAR_DOT      = 0x002E, // '.' dot
        CHAR_LOWLINE  = 0x005F, // '_' low line

        CHAR_UNKNOW   = 255
    };

    enum UUID_RESULT
    {
        UUID_ERROR         = -1,
        UUID_SUCCESS       =  0,
        UUID_LENGTH        = 37, //! { } 미포함
        UUID_LENGTH_BRACES = 39, //! { } 포함
    };

    } // namespace string
} // namespace sc
#endif // SC_STRING_DEFINE_H_
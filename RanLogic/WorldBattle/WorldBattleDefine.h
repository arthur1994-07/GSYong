#ifndef _RAN_LOGIC_WORLD_BATTLE_DEFINE_H_
#define _RAN_LOGIC_WORLD_BATTLE_DEFINE_H_

#pragma once

#include <string>

//! namespace world battle 2012-02-08 jgkim
namespace wb
{
    //! 국가
    //! http://en.wikipedia.org/wiki/ISO_3166-1
    enum EM_COUNTRY
    {
        UNKNOWN_COUNTRY                         =   0,
        AFGHANISTAN                             =   4,
        ALAND_ISLANDS                           = 248,
        ALBANIA                                 =   8,
        ALGERIA                                 =  12,
        AMERICAN_SAMOA                          =  16,
        ANDORRA                                 =  20,
        ANGOLA                                  =  24,
        ANGUILLA                                = 660,
        ANTARCTICA                              =  10,
        ANTIGUA_AND_BARBUDA                     =  28,
        ARGENTINA                               =  32,
        ARMENIA                                 =  51,
        ARUBA                                   = 533,
        AUSTRALIA                               =  36,
        AUSTRIA                                 =  40,
        AZERBAIJAN                              =  31,
        BAHAMAS                                 =  44,
        BAHRAIN                                 =  48,
        BANGLADESH                              =  50,
        BARBADOS                                =  52,
        BELARUS                                 = 112,
        BELGIUM                                 =  56,
        BELIZE                                  =  84,
        BENIN                                   = 204,
        BERMUDA                                 =  60,
        BHUTAN                                  =  64,
        BOLIVIA_PLURINATIONAL_STATE_OF          =  68,
        BONAIRE_SINT_EUSTATIUS_AND_SABA         = 535,
        BOSNIA_AND_HERZEGOVINA                  =  70,
        BOTSWANA                                =  72,
        BOUVET_ISLAND                           =  74,
        BRAZIL                                  =  76,
        BRITISH_INDIAN_OCEAN_TERRITORY          =  86,
        BRUNEI_DARUSSALAM                       =  96,
        BULGARIA                                = 100,
        BURKINA_FASO                            = 854,
        BURUNDI                                 = 108,
        CAMBODIA                                = 116,
        CAMEROON                                = 120,
        CANADA                                  = 124,
        CAPE_VERDE                              = 132,
        CAYMAN_ISLANDS                          = 136,
        CENTRAL_AFRICAN_REPUBLIC                = 140,
        CHAD                                    = 148,
        CHILE                                   = 152,
        CHINA                                   = 156,
        CHRISTMAS_ISLAND                        = 162,
        COCOS_KEELING_ISLANDS                   = 166,
        COLOMBIA                                = 170,
        COMOROS                                 = 174,
        CONGO                                   = 178,
        CONGO_THE_DEMOCRATIC_REPUBLIC_OF_THE    = 180,
        COOK_ISLANDS                            = 184,
        COSTA_RICA                              = 188,
        COTE_D_IVOIRE                           = 384,
        CROATIA                                 = 191,
        CUBA                                    = 192,
        CURACAO                                 = 531,
        CYPRUS                                  = 196,
        CZECH_REPUBLIC                          = 203,
        DENMARK                                 = 208,
        DJIBOUTI                                = 262,
        DOMINICA                                = 212,
        DOMINICAN_REPUBLIC                      = 214,
        ECUADOR                                 = 218,
        EGYPT                                   = 818,
        EL_SALVADOR                             = 222,
        EQUATORIAL_GUINEA                       = 226,
        ERITREA                                 = 232,
        ESTONIA                                 = 233,
        ETHIOPIA                                = 231,
        FALKLAND_ISLANDS_MALVINAS               = 238,
        FAROE_ISLANDS                           = 234,
        FIJI                                    = 242,
        FINLAND                                 = 246,
        FRANCE                                  = 250,
        FRENCH_GUIANA                           = 254,
        FRENCH_POLYNESIA                        = 258,
        FRENCH_SOUTHERN_TERRITORIES             = 260,
        GABON                                   = 266,
        GAMBIA                                  = 270,
        GEORGIA                                 = 268,
        GERMANY                                 = 276,
        GHANA                                   = 288,
        GIBRALTAR                               = 292,
        GREECE                                  = 300,
        GREENLAND                               = 304,
        GRENADA                                 = 308,
        GUADELOUPE                              = 312,
        GUAM                                    = 316,
        GUATEMALA                               = 320,
        GUERNSEY                                = 831,
        GUINEA                                  = 324,
        GUINEA_BISSAU                           = 624,
        GUYANA                                  = 328,
        HAITI                                   = 332,
        HEARD_ISLAND_AND_MCDONALD_ISLANDS       = 334,
        HOLY_SEE_VATICAN_CITY_STATE             = 336,
        HONDURAS                                = 340,
        HONG_KONG                               = 344,
        HUNGARY                                 = 348,
        ICELAND                                 = 352,
        INDIA                                   = 356,
        INDONESIA                               = 360,
        IRAN_ISLAMIC_REPUBLIC_OF                = 364,
        IRAQ                                    = 368,
        IRELAND                                 = 372,
        ISLE_OF_MAN                             = 833,
        ISRAEL                                  = 376,
        ITALY                                   = 380,
        JAMAICA                                 = 388,
        JAPAN                                   = 392,
        JERSEY                                  = 832,
        JORDAN                                  = 400,
        KAZAKHSTAN                              = 398,
        KENYA                                   = 404,
        KIRIBATI                                = 296,
        KOREA_DEMOCRATIC_PEOPLE_S_REPUBLIC_OF   = 408,
        KOREA_REPUBLIC_OF                       = 410,
        KUWAIT                                  = 414,
        KYRGYZSTAN                              = 417,
        LAO_PEOPLE_S_DEMOCRATIC_REPUBLIC        = 418,
        LATVIA                                  = 428,
        LEBANON                                 = 422,
        LESOTHO                                 = 426,
        LIBERIA                                 = 430,
        LIBYA                                   = 434,
        LIECHTENSTEIN                           = 438,
        LITHUANIA                               = 440,
        LUXEMBOURG                              = 442,
        MACAO                                   = 446,
        MACEDONIA_THE_FORMER_YUGOSLAV_REPUBLIC_OF = 807,
        MADAGASCAR                              = 450,
        MALAWI                                  = 454,
        MALAYSIA                                = 458,
        MALDIVES                                = 462,
        MALI                                    = 466,
        MALTA                                   = 470,
        MARSHALL_ISLANDS                        = 584,
        MARTINIQUE                              = 474,
        MAURITANIA                              = 478,
        MAURITIUS                               = 480,
        MAYOTTE                                 = 175,
        MEXICO                                  = 484,
        MICRONESIA_FEDERATED_STATES_OF          = 583,
        MOLDOVA_REPUBLIC_OF                     = 498,
        MONACO                                  = 492,
        MONGOLIA                                = 496,
        MONTENEGRO                              = 499,
        MONTSERRAT                              = 500,
        MOROCCO                                 = 504,
        MOZAMBIQUE                              = 508,
        MYANMAR                                 = 104,
        NAMIBIA                                 = 516,
        NAURU                                   = 520,
        NEPAL                                   = 524,
        NETHERLANDS                             = 528,
        NEW_CALEDONIA                           = 540,
        NEW_ZEALAND                             = 554,
        NICARAGUA                               = 558,
        NIGER                                   = 562,
        NIGERIA                                 = 566,
        NIUE                                    = 570,
        NORFOLK_ISLAND                          = 574,
        NORTHERN_MARIANA_ISLANDS                = 580,
        NORWAY                                  = 578,
        OMAN                                    = 512,
        PAKISTAN                                = 586,
        PALAU                                   = 585,
        PALESTINIAN_TERRITORY_OCCUPIED          = 275,
        PANAMA                                  = 591,
        PAPUA_NEW_GUINEA                        = 598,
        PARAGUAY                                = 600,
        PERU                                    = 604,
        PHILIPPINES                             = 608,
        PITCAIRN                                = 612,
        POLAND                                  = 616,
        PORTUGAL                                = 620,
        PUERTO_RICO                             = 630,
        QATAR                                   = 634,
        REUNION                                 = 638,
        ROMANIA                                 = 642,
        RUSSIAN_FEDERATION                      = 643,
        RWANDA                                  = 646,
        SAINT_BARTHELEMY                        = 652,
        SAINT_HELENA_ASCENSION_AND_TRISTAN_DA_CUNHA = 654,
        SAINT_KITTS_AND_NEVIS                   = 659,
        SAINT_LUCIA                             = 662,
        SAINT_MARTIN_FRENCH_PART                = 663,
        SAINT_PIERRE_AND_MIQUELON               = 666,
        SAINT_VINCENT_AND_THE_GRENADINES        = 670,
        SAMOA                                   = 882,
        SAN_MARINO                              = 674,
        SAO_TOME_AND_PRINCIPE                   = 678,
        SAUDI_ARABIA                            = 682,
        SENEGAL                                 = 686,
        SERBIA                                  = 688,
        SEYCHELLES                              = 690,
        SIERRA_LEONE                            = 694,
        SINGAPORE                               = 702,
        SINT_MAARTEN_DUTCH_PART                 = 534,
        SLOVAKIA                                = 703,
        SLOVENIA                                = 705,
        SOLOMON_ISLANDS                         = 90,
        SOMALIA                                 = 706,
        SOUTH_AFRICA                            = 710,
        SOUTH_GEORGIA_AND_THE_SOUTH_SANDWICH_ISLANDS = 239,
        SOUTH_SUDAN                             = 728,
        SPAIN                                   = 724,
        SRI_LANKA                               = 144,
        SUDAN                                   = 729,
        SURINAME                                = 740,
        SVALBARD_AND_JAN_MAYEN                  = 744,
        SWAZILAND                               = 748,
        SWEDEN                                  = 752,
        SWITZERLAND                             = 756,
        SYRIAN_ARAB_REPUBLIC                    = 760,
        TAIWAN_PROVINCE_OF_CHINA                = 158,
        TAJIKISTAN                              = 762,
        TANZANIA_UNITED_REPUBLIC_OF             = 834,
        THAILAND                                = 764,
        TIMOR_LESTE                             = 626,
        TOGO                                    = 768,
        TOKELAU                                 = 772,
        TONGA                                   = 776,
        TRINIDAD_AND_TOBAGO                     = 780,
        TUNISIA                                 = 788,
        TURKEY                                  = 792,
        TURKMENISTAN                            = 795,
        TURKS_AND_CAICOS_ISLANDS                = 796,
        TUVALU                                  = 798,
        UGANDA                                  = 800,
        UKRAINE                                 = 804,
        UNITED_ARAB_EMIRATES                    = 784,
        UNITED_KINGDOM                          = 826,
        UNITED_STATES                           = 840,
        UNITED_STATES_MINOR_OUTLYING_ISLANDS    = 581,
        URUGUAY                                 = 858,
        UZBEKISTAN                              = 860,
        VANUATU                                 = 548,
        VENEZUELA_BOLIVARIAN_REPUBLIC_OF        = 862,
        VIETNAM                                 = 704,
        VIRGIN_ISLANDS_BRITISH                  =  92,
        VIRGIN_ISLANDS_US                       = 850,
        WALLIS_AND_FUTUNA                       = 876,
        WESTERN_SAHARA                          = 732,
        YEMEN                                   = 887,
        ZAMBIA                                  = 894,
        ZIMBABWE                                = 716,
    };
    
    struct TEXTURE_POSITION
    {
        WORD m_Left;
        WORD m_Top;
        WORD m_Right;
        WORD m_Bottom;

        TEXTURE_POSITION()
            : m_Left(0)
            , m_Top(0)
            , m_Right(0)
            , m_Bottom(0)
        {
        }
    };

    struct FLAG_TEXTURE_POSITION
    {
        std::string m_TextureName; //! 국기 이미지 파일
        WORD m_Left; //! 좌표
        WORD m_Top;
        WORD m_Right;
        WORD m_Bottom;

        FLAG_TEXTURE_POSITION()
            : m_Left(0)
            , m_Top(0)
            , m_Right(0)
            , m_Bottom(0)
        {
        }
    };

    //! See http://en.wikipedia.org/wiki/ISO_3166-1
    struct COUNTRY_INFO
    {
        EM_COUNTRY m_Code; //! 국가코드
        WORD m_ClubCount; //! 클럽숫자. 서버에서 정보를 받기 전 까지는 0 이다.
        std::string m_FullName; //! Country full name
        std::string m_Alpha2Code; //! See http://en.wikipedia.org/wiki/ISO_3166-1
        std::string m_Alpha3Code; //! See http://en.wikipedia.org/wiki/ISO_3166-1
        std::string m_Desc; //! Country description
        std::string m_Anthem; //! 국가. ex) kor.ogg
                
        FLAG_TEXTURE_POSITION m_Pos; //! Texture Position

        COUNTRY_INFO()
            : m_Code(UNKNOWN_COUNTRY)
            , m_ClubCount(0)
        {
        }
    };

    //! 국가/국가별 클럽수
    struct COUNTRY_CLUB_COUNT_INFO
    {
        EM_COUNTRY m_Country; //! 국가
        WORD m_ClubCount; //! 국가별 클럽수

        COUNTRY_CLUB_COUNT_INFO(EM_COUNTRY Country, WORD ClubCount)
            : m_Country(Country)
            , m_ClubCount(ClubCount)
        {
        }
    };

    struct CLUB_INFO_BASE
    {
        EM_COUNTRY m_Country;
        DWORD m_ClubDbNum;
        DWORD m_ClubRank;
        DWORD m_MemberNum;
        int m_MasterLevel;
        __time64_t m_OrganizeDate; //! 25598 select CONVERT(int, CONVERT(datetime, '1970-02-01 00:00:00'))
        std::string m_ClubName;
        std::string m_MasterName;
        std::string m_ClubNotice;

        CLUB_INFO_BASE(
            EM_COUNTRY CountryCode,
            DWORD ClubDbNum,
            DWORD ClubRank,
            DWORD MemberNum,
            int MasterLevel,
            __time64_t OrganizeDate,
            const std::string& ClubName,
            const std::string& MasterName,
            const std::string& ClubNotice)
            : m_Country(CountryCode)
            , m_ClubDbNum(ClubDbNum)
            , m_ClubRank(ClubRank)
            , m_MemberNum(MemberNum)
            , m_MasterLevel(MasterLevel)
            , m_OrganizeDate(OrganizeDate)
            , m_ClubName(ClubName)
            , m_MasterName(MasterName)
            , m_ClubNotice(ClubNotice)
        {
        }
    };

    namespace search
    {
        enum EM_ORDER_TYPE
        {
            CLUB_NAME_SEARCH,
            CLUB_RANK,
            CLUB_MASTER_LEVEL,
            CLUB_MASTER_NAME,
            CLUB_MEMBER_NUM,
        };

        enum EM_ORDER_ASC_DSC
        {
            ASC, //! 낮은차순
            DSC, //! 높은차순
        };

        class OrderBy
        {
        public:
            EM_ORDER_TYPE m_Type;
            EM_ORDER_ASC_DSC m_AscDsc;

            OrderBy()
                : m_Type(CLUB_NAME_SEARCH)
                , m_AscDsc(ASC)
            {
            }
        };
    } // namespace search

} // namespace wb

#endif // _RAN_LOGIC_WORLD_BATTLE_DEFINE_H_
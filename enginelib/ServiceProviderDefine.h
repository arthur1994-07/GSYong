#ifndef _SERVICE_PROVIDER_DEFINE_H_
#define _SERVICE_PROVIDER_DEFINE_H_

#pragma once

//! 서비스 제공업자(수정시 WidgetDef.lua파일도 같이 수정)
enum EMSERVICE_PROVIDER
{
    SP_OFFICE_TEST  =  0, //! Korea(Mincoms), 내부서버
    SP_KOREA        =  1, //! Korea(Daum game)(Korea), IDC 서버, Web Start // KR_PARAM
    SP_TAIWAN       =  2, //! Taiwan                  // TW_PARAM
    SP_CHINA        =  3, //! China                   // CH_PARAM
    SP_JAPAN        =  4, //! Japan                   // JP_PARAM
    SP_PHILIPPINES  =  5, //! Philippines             // PH_PARAM
    SP_THAILAND     =  6, //! Thailand                // TH_PARAM
    SP_GLOBAL       =  7, //! Global Service (GSP)    // GS_PARAM
    SP_MALAYSIA     =  8, //! Malaysia                // MY_PARAM
    SP_GS		    =  9, //! Global Service          // GS_PARAM
    SP_INDONESIA    = 10, //! Indonesia               // ID_PARAM
    SP_MALAYSIA_EN  = 11, //! Malaysia English        // MYE_PARAM
    SP_VIETNAM      = 12, //! Vietnam                 // VN_PARAM
    SP_HONGKONG     = 13, //! HongKong                // HK_PARAM
    SP_KOREA_TEST   = 14, //! Korea Test Server, IDC, Web Start // KRT_PARAM
    SP_WORLD_BATTLE = 15, //! World Battle, 2012-02-08 jgkim
	SP_EU           = 16, //! EU, Games Masters
	SP_US           = 17, //! US, GameSamba
    SP_TOTAL_NUM    = 18,
};

namespace ENGINE
{
    extern EMSERVICE_PROVIDER emServiceProvider;
}

#endif // _SERVICE_PROVIDER_DEFINE_H_
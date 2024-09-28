#ifndef _GL_ITEM_DRUG_H_
#define _GL_ITEM_DRUG_H_

#pragma once

#include "./GLItemDef.h"

namespace sc
{
    class BaseStream;
    class SerialFile;
}

namespace ITEM
{
    struct SDRUG_100
    {
        BOOL		bInstance;
        EMITEM_DRUG	emDrug;
        WORD		wPileNum;

        BOOL		bRatio;

        union
        {
            struct { WORD	wCureVolume; };
            struct { WORD	wArrowNum; };
        };

        DWORD		dwCureDISORDER;
    };

    struct SDRUG_101	//	데이터 포맷 변경 진행
    {
        __time64_t	tTIME_LMT;		//	시한부 아이템일 경우 유효 기간.
        BOOL		bInstance;		//	소모성 여부.
        EMITEM_DRUG	emDrug;			//	약품 타입.
        WORD		wPileNum;		//	최대겹침량. ( old : wApplyNum )

        BOOL		bRatio;			//	비율 많큼 치료.

        union
        {
            struct { WORD	wCureVolume; };	//	치료 용량. ( 일반 약품일 경우. )
            struct { WORD	wArrowNum; };	//	화살 한묶음양. ( 화살일 경우. )
        };

        DWORD		dwCureDISORDER;	//	"DISORDER" 치료 목록.
    };

	//	약품 류, 소모성 관련 데이터.
	struct SDRUG_102
	{
		enum { VERSION = 0x0102 };

		EMITEM_DRUG	emDrug;			//! 약품 타입.
		BOOL		bInstance;		//! 소모성 여부.
		__time64_t	tTIME_LMT;		//! 시한부 아이템일 경우 유효 기간.
		BOOL		bRatio;			//! 비율 많큼 치료.
		WORD		wPileNum;		//! 최대겹침량 (old:wApplyNum)

		union
		{
			struct { WORD wArrowNum; };	  //! 화살 한묶음양(화살일 경우)
			struct { WORD wCureVolume; }; //! 치료 용량(일반 약품일 경우)
			struct { WORD PointCharge; }; //! Point 카드일경우 Point 충전량
			struct { WORD wStage; };	  //! 무한의 재단 인던 스테이지
			struct { WORD wLv; };		  //! 무한의 재단 인던 스테이지 난이도
		};

		DWORD dwCureDISORDER; //! "DISORDER" 치료 목록.
	};

	struct SDRUG_103
	{
		enum { VERSION = 0x0103 };

		EMITEM_DRUG	emDrug;			//! 약품 타입.
		BOOL		bInstance;		//! 소모성 여부.
		__time64_t	tTIME_LMT;		//! 시한부 아이템일 경우 유효 기간.
		BOOL		bRatio;			//! 비율 많큼 치료.
		WORD		wPileNum;		//! 최대겹침량 (old:wApplyNum)

		union
		{
			struct { DWORD dwArrowNum; };	  //! 화살 한묶음양(화살일 경우)
			struct { DWORD dwCureVolume; }; //! 치료 용량(일반 약품일 경우)
			struct { DWORD dwPointCharge; }; //! Point 카드일경우 Point 충전량
			struct { DWORD dwStage; };	  //! 무한의 재단 인던 스테이지
			struct { DWORD dwLv; };		  //! 무한의 재단 인던 스테이지 난이도
		};

		DWORD dwCureDISORDER; //! "DISORDER" 치료 목록.
	};

	struct SDRUG_104
	{
		enum { VERSION = 0x0104 };

		EMITEM_DRUG	emDrug;			//! 약품 타입.
		BOOL		bInstance;		//! 소모성 여부.
		__time64_t	tTIME_LMT;		//! 시한부 아이템일 경우 유효 기간.
		BOOL		bRatio;			//! 비율 많큼 치료.
		WORD		wPileNum;		//! 최대겹침량 (old:wApplyNum)

		union
		{
			struct { DWORD dwArrowNum; };	  //! 화살 한묶음양(화살일 경우)
			struct { DWORD dwCureVolume; }; //! 치료 용량(일반 약품일 경우)
			struct { DWORD dwPointCharge; }; //! Point 카드일경우 Point 충전량
			struct { DWORD dwStage; };	  //! 무한의 재단 인던 스테이지
			struct { DWORD dwLv; };		  //! 무한의 재단 인던 스테이지 난이도
		};

		DWORD dwCureDISORDER; //! "DISORDER" 치료 목록.
		__time64_t tDuration; // 지속시간
	};

    //	약품 류, 소모성 관련 데이터.
    struct SDRUG
    {
        enum { VERSION = 0x0105 };

        EMITEM_DRUG	emDrug;			//! 약품 타입.
        BOOL		bInstance;		//! 소모성 여부.
        __time64_t	tTIME_LMT;		//! 시한부 아이템일 경우 유효 기간.
        BOOL		bRatio;			//! 비율 많큼 치료.
        WORD		wPileNum;		//! 최대겹침량 (old:wApplyNum)

        union
        {
            struct { DWORD dwArrowNum; };	  //! 화살 한묶음양(화살일 경우)
            struct { DWORD dwCureVolume; }; //! 치료 용량(일반 약품일 경우)
            struct { DWORD dwPointCharge; }; //! Point 카드일경우 Point 충전량
			struct { DWORD dwStage; };	  //! 무한의 재단 인던 스테이지
			struct { DWORD dwLv; };		  //! 무한의 재단 인던 스테이지 난이도
			struct { DWORD dwCompressPrecent; };		  //! 경험치 압축 및 캡슐 
        };

        DWORD	   dwCureDISORDER; //! "DISORDER" 치료 목록.
		__time64_t tDuration;	   // 지속시간
		DWORD	   dwERList;       // 효과해제물약 일때만 사용하는 효과해제 리스트

        SDRUG ()
            : tTIME_LMT(0)
            , bInstance(FALSE)
            , emDrug(ITEM_DRUG_NUNE)
            , wPileNum(1)

            , bRatio(FALSE)
            , dwCureVolume(0)
            , dwArrowNum(0)


            , dwCureDISORDER(NULL)
			, tDuration(0)
			, dwERList(0)
        {
        }

        void Assign ( SDRUG_100 &sDRUG );
        void Assign ( SDRUG_101 &sDRUG );
		void Assign ( SDRUG_102 &sDRUG );
		void Assign ( SDRUG_103 &sDRUG );
		void Assign ( SDRUG_104 &sDRUG );

        static VOID SaveCsvHead ( std::fstream &SFile );
        VOID SaveCsv ( std::fstream &SFile );
        VOID LoadCsv ( CStringArray &StrArray, int& iCsvCur );

        //! DISORDER 치료 목록.
        inline DWORD GetCureDisOrder() const { return dwCureDISORDER; }

        //!	약품 타입
        inline EMITEM_DRUG GetType() const { return emDrug; }

        //! 소모성 여부
        inline BOOL GetInstance() const { return bInstance; }

        ////! 시한부 아이템일 경우 유효 기간
        //inline __time64_t GetTimeLimit() const { return tTIME_LMT; }

        //! 비율 많큼 치료.
        inline BOOL GetRatio() const { return bRatio; }

        //!	최대겹침량. (old:wApplyNum)
        inline DWORD GetPileNum() const { return wPileNum; }

        //! 화살 한묶음양(화살일 경우)
        inline DWORD GetArrowNum() const { return dwArrowNum; }

        //! 치료 용량(일반 약품일 경우)
        inline DWORD GetCureVolumn() const { return dwCureVolume; }

        //! Point 카드일경우 Point 충전량
        inline DWORD GetPointCharge() const { return dwPointCharge; }

		inline DWORD GetCompressPrecent() const { return dwCompressPrecent; }
		inline void SetCompressPrecent( DWORD percent ) { dwCompressPrecent = percent; }

    private:
        /************************************************************************/
        // 다음 중 함수가 필요하다면 SITEMCUSTOM.h을 참고하라.
        // .\GLItemCustom.h

		// 시한부 아이템일 경우 유효 기간.
		bool IsTimeLimited() const;
		bool IsTimeDuration() const;

        // SITEMCUSTOM에서의 접근을 허가한다.
        friend struct SITEMCUSTOM;

    };

} // namespace ITEM
#endif // _GL_ITEM_DRUG_H_
#ifndef _GL_ITEM_BASIC_H_
#define _GL_ITEM_BASIC_H_

#pragma once

#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../Character/GLCharDefine.h"
#include "./GLItemDef.h"

namespace sc 
{    
    class BaseStream;
    class SerialFile;
    class TxtFile;
}

namespace ITEM
{

    struct SBASIC_100
    {
        SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )

        char		szName[ITEM_SZNAME];		//	아이템의 이름.	
        EMITEMLEVEL	emLevel;					//	아이템 가치 등급.

        DWORD		dwFlags;					//	Trade 속성.
        DWORD		dwPrice;					//	아이템 가격.

        EMITEM_TYPE	emItemType;					//	아이템 종류.

        //	착용 조건.
        EMBRIGHT	emReqBright;				//	캐릭 속성. ( 광/암 )
        DWORD		dwReqCharClass;				//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
        WORD		wReqLevel;					//	요구 Level.
        WORD		wReqPA;						//	요구 격투치.
        WORD		wReqSA;						//	요구 사격치.
        SCHARSTATS	sReqStats;					//	요구 케릭터 Stats 수치.

        WORD		wInvenSizeX;				//	인벤토리 사이즈.
        WORD		wInvenSizeY;				//	인벤토리 사이즈.

        char		szFieldFile[MAX_PATH];				//	바닥 형상 파일.
        char		szInventoryFile[MAX_PATH];			//	인벤토리 형상 파일.
        char		szWearingFIle[GLCI_NUM][MAX_PATH];	//	형상 파일.
        char		szComment[ITEM_SZCOMMENT];			//	아이템에 대한 간단한 설명.
    };

    struct SBASIC_101
    {
        SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )

        char		szName[ITEM_SZNAME];		//	아이템의 이름.	
        EMITEMLEVEL	emLevel;					//	아이템 가치 등급.

        DWORD		dwFlags;					//	Trade 속성.
        DWORD		dwPrice;					//	아이템 가격.

        EMITEM_TYPE	emItemType;					//	아이템 종류.

        //	착용 조건.
        EMBRIGHT	emReqBright;				//	캐릭 속성. ( 광/암 )
        DWORD		dwReqCharClass;				//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
        WORD		wReqLevel;					//	요구 Level.
        WORD		wReqPA;						//	요구 격투치.
        WORD		wReqSA;						//	요구 사격치.
        SCHARSTATS	sReqStats;					//	요구 케릭터 Stats 수치.

        WORD		wInvenSizeX;				//	인벤토리 사이즈.
        WORD		wInvenSizeY;				//	인벤토리 사이즈.

        char		szSelfBodyEffect[MAX_PATH];	//	자기 자신의 몸에 붓는 이팩트.
        char		szTargBodyEffect[MAX_PATH];	//	목표 유닛의 몸에 붓는 이팩트.
        char		szTargetEffect[MAX_PATH];	//	목표 지향 이팩트.

        char		szFieldFile[MAX_PATH];				//	바닥 형상 파일.
        char		szInventoryFile[MAX_PATH];			//	인벤토리 형상 파일.
        char		szWearingFIle[GLCI_NUM][MAX_PATH];	//	형상 파일.
        char		szComment[ITEM_SZCOMMENT];			//	아이템에 대한 간단한 설명.
    };

    struct SBASIC_102
    {
        SNATIVEID	sNativeID;					//	고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )

        char		szName[ITEM_SZNAME];		//	아이템의 이름.	
        EMITEMLEVEL	emLevel;					//	아이템 가치 등급.

        DWORD		dwFlags;					//	Trade 속성.
        DWORD		dwPrice;					//	아이템 가격.

        EMITEM_TYPE	emItemType;					//	아이템 종류.

        //	착용 조건.
        EMBRIGHT	emReqBright;				//	캐릭 속성. ( 광/암 )
        DWORD		dwReqCharClass;				//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
        WORD		wReqLevel;					//	요구 Level.
        WORD		wReqPA;						//	요구 격투치.
        WORD		wReqSA;						//	요구 사격치.
        SCHARSTATS	sReqStats;					//	요구 케릭터 Stats 수치.

        WORD		wInvenSizeX;				//	인벤토리 사이즈.
        WORD		wInvenSizeY;				//	인벤토리 사이즈.

        SNATIVEID	sICONID;					//	아이콘 인덱스.

        char		szSelfBodyEffect[MAX_PATH];	//	자기 자신의 몸에 붓는 이팩트.
        char		szTargBodyEffect[MAX_PATH];	//	목표 유닛의 몸에 붓는 이팩트.
        char		szTargetEffect[MAX_PATH];	//	목표 지향 이팩트.

        char		szFieldFile[MAX_PATH];				//	바닥 형상 파일.
        char		szInventoryFile[MAX_PATH];			//	인벤토리 형상 파일.
        char		szWearingFIle[GLCI_NUM][MAX_PATH];	//	형상 파일.
        char		szComment[ITEM_SZCOMMENT];			//	아이템에 대한 간단한 설명.
    };

    struct SBASIC
    {
        enum { VERSION = 0x0130 };

        SNATIVEID	sNativeID;					// 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
        SNATIVEID	sGroupID;					// ???

        std::string	strName;					// 아이템의 이름.	
        EMITEMLEVEL	emLevel;					// 아이템 가치 등급.

        WORD		wGradeAttack;				// 공격 개조 등급
        WORD		wGradeDefense;				// 방어 개조 등급

        float		fExpMultiple;				// 경험치 개조 등급

        WORD		wReserved1;					// 사용 안함
        WORD		wReserved2;					// 사용 안함
        WORD		wReserved3;					// 사용 안함
        WORD		wReserved4;					// 사용 안함
        WORD		wReserved5;					// 사용 안함

        
    private:
        // 이 플레그는 직접 건드리면 안된다.
        // 건드리고 싶을땐  GLItemCustom.h ( SITEMCUSTOM )을 먼저 참고하라.
        DWORD		dwFlags;				    // Trade 속성.

	public :
        int			nRPTrade;                  // RanPoint (RP) Trade 가능 회수

        LONGLONG		dwBuyPrice;					// 아이템 구입 가격.
        LONGLONG		dwSellPrice;				// 아이템 판매 가격

        EMITEM_TYPE	emItemType;					// 아이템 종류.

        //	착용 조건.
        EMBRIGHT	emReqBright;				//	캐릭 속성. ( 광/암 )
        DWORD		dwReqCharClass;				//	착용할 수 있는 직업들. ( EMCHARCLASS Flags )
        DWORD		dwReqSchool;				//	착용할 수 있는 학원. 
        WORD		wReqLevelDW;				//	요구 Level 하한선
        WORD		wReqLevelUP;				//  요구 Level 상한선
        WORD		wReqPA;						//	요구 격투치.
        WORD		wReqSA;						//	요구 사격치.
        SCHARSTATS	sReqStats;					//	요구 케릭터 Stats 수치.

        SNATIVEID	sICONID;					//	아이콘 인덱스.

        std::string		strSelfBodyEffect;		//	자기 자신의 몸에 붓는 이팩트.
        std::string		strTargBodyEffect;		//	목표 유닛의 몸에 붓는 이팩트.
        std::string		strTargetEffect;		//	목표 지향 이팩트.
		std::string		strGeneralEffect;		//	전신 이펙트;

        std::string		strFieldFile;				//	바닥 형상 파일.
        std::string		strInventoryFile;			//	인벤토리 형상 파일.

		std::string		strArrInventoryFile[GLCI_NUM_ACTOR];//	직업별 인벤토리 형상 파일.
        std::string		strWearingFile[GLCI_NUM_ACTOR];		//	형상 파일.
        std::string		strWearingFileEx[GLCI_NUM_ACTOR];	//	형상 파일.

        std::string		strComment;					//	아이템에 대한 간단한 설명.

        // PET
        std::string		strPetWearingFile;			//  팻의 형상파일. 

        SNATIVEID		sSubID;						// mid/sid ( 각종 MID/SID ) 
        WORD			wPosX;						// X좌표
        WORD			wPosY;						// Y좌표

        DWORD			dwCoolTime;					// 아이템 사용 쿨타임
        EMCOOL_TYPE		emCoolType;					// 쿨타임 타입

        bool			bSearch;					// 아이템 검색 가능 유무.		

        int             emReqActivityType;          //  필요 요구활동
        WORD            wReqActPointDW;             //  요구 포인트 최소 
        WORD            wReqActPointUP;             //  요구 포인트 최대

		DWORD			dwReqContribution;			// 요구 기여도

		// 툴을 제외하고 클라, 서버일 경우 검색 속도를 위해 다음과 같은 추가작업을 함;
		// 기간 연장 아이템인 경우를 제외하고;
		// SITEM->sBasicOp.strItemPeriodExtension에 기간 연장 리스트를 등록해둔다;
		// 원래는 기간 연장 아이템만 사용한다;
		// 설명 추가 [9/7/2016 gbgim];
		// 기간 연장 가능 아이템인지 단순 판별하기 위함이다;
		// 인게임에서 로드시 중복이건 상관없이 속도를 위해 넣어둔다;
		std::string		strItemPeriodExtension;		// 아이템 기간 연정 설정;

        SBASIC () 
            : sNativeID(false)
            , sGroupID(false)

            , emLevel(EMITEM_LEVEL_NORMAL)

            , wGradeAttack(0)
            , wGradeDefense(0)

            , fExpMultiple(1.0f)

            , wReserved1(0)
            , wReserved2(0)
            , wReserved3(0)
            , wReserved4(0)
            , wReserved5(0)

            , dwFlags(TRADE_ALL)
            , nRPTrade(0)

            , dwBuyPrice(1)
            , dwSellPrice(1)

            , emItemType(ITEM_SUIT)
            , emReqBright(BRIGHT_BOTH)
            , dwReqCharClass(GLCC_NONE)
            , dwReqSchool(GLSCHOOL_ALL)
            , wReqLevelDW(0)
            , wReqLevelUP(0)
            , wReqPA(0)
            , wReqSA(0)
            , sICONID(0,0)
            , sSubID( false )
            , wPosX( 0 )
            , wPosY( 0 )
            , dwCoolTime( 0 )
            , emCoolType( EMCOOL_ITEMID )
            , bSearch(true)
            , emReqActivityType( 0 ) 
            , wReqActPointDW( 0 ) 
            , wReqActPointUP( 0 )
			, dwReqContribution( 0 )
        {
        }

        void Assign ( SBASIC_100 &sOld );
        void Assign ( SBASIC_101 &sOld );
        void Assign ( SBASIC_102 &sOld );

        bool LOAD_103 ( sc::BaseStream &SFile );
        bool LOAD_104 ( sc::BaseStream &SFIle );
        bool LOAD_105 ( sc::BaseStream &SFile );
        bool LOAD_106 ( sc::BaseStream &SFile );
        bool LOAD_107 ( sc::BaseStream &SFile );
        bool LOAD_108 ( sc::BaseStream &SFile );
        bool LOAD_109 ( sc::BaseStream &SFile );
        bool LOAD_110 ( sc::BaseStream &SFile );
        bool LOAD_111 ( sc::BaseStream &SFile );
        bool LOAD_112 ( sc::BaseStream &SFile );
        bool LOAD_113 ( sc::BaseStream &SFile );
        bool LOAD_114 ( sc::BaseStream &SFile );	//	데이터 포맷 변경 진행
        bool LOAD_115 ( sc::BaseStream &SFile );	//	과학부 추가
        bool LOAD_116 ( sc::BaseStream &SFile );
        bool LOAD_117 ( sc::BaseStream &SFile );
        bool LOAD_118 ( sc::BaseStream &SFile );
		bool LOAD_119 ( sc::BaseStream &SFile );
		bool LOAD_120 ( sc::BaseStream &SFile );
		bool LOAD_121 ( sc::BaseStream &SFile );
        bool LOAD_122 ( sc::BaseStream &SFile );
		bool LOAD_123 ( sc::BaseStream &SFile );
		bool LOAD_124 ( sc::BaseStream &SFile );
		bool LOAD_125 ( sc::BaseStream &SFile );
		bool LOAD_126 ( sc::BaseStream &SFile );
		bool LOAD_127 ( sc::BaseStream &SFile );
        bool LOAD_128 ( sc::BaseStream& SFile );
		bool LOAD_129 ( sc::BaseStream& SFile );

		bool LOAD(sc::BaseStream& SFile);
        bool SAVE(sc::SerialFile& SFile);
        bool SaveLua(sc::TxtFile& LuaFile);

        static VOID SaveCsvHead ( std::fstream &SFile );
        VOID SaveCsv ( std::fstream &SFile );
        VOID LoadCsv ( CStringArray &StrArray, int& iCsvCur );

        SBASIC& operator = ( const SBASIC& rvalue );

        //! 아이템 종류
        inline EMITEM_TYPE Type() const { return emItemType; }

        //! 고유 ID. ( 아이템 설정 최초 생성시에 부여되는 고유 ID )
        inline SNATIVEID GetNativeID() const { return sNativeID; }


		inline int RanPointTradeCount() const 
		{ 
			if( nRPTrade < 0 )
				return 0; 

			return nRPTrade; 
		}
		inline bool IsCoolTime() const { return ( dwCoolTime != 0 ); }

		// 아이템 등급;
		inline const EMITEMLEVEL GetItemLevel ( void ) const { return emLevel; }

		// 원본
		//inline bool IsSALAE() const { return (dwFlags&TRADE_SALE) != NULL; }
		//inline bool IsEXCHANGE() const { return (dwFlags&TRADE_EXCHANGE) != NULL; }
		//inline bool IsTHROW() const { return (dwFlags&TRADE_THROW)!=NULL; }
		//inline bool IsEVENT() const { return (dwFlags&TRADE_EVENT_SGL)!=NULL; }
		//inline bool IsRestrict() const	{ return (dwFlags&ITEM_RESTRICT)!=NULL; }
		//inline bool IsDISGUISE() const	{ return (dwFlags&ITEM_DISGUISE)!=NULL; }
		//inline bool IsCHANNEL_ALL() const { return (dwFlags&ITEM_CHANNEL_ALL)!=NULL; }
		//inline bool IsGarbage() const  { return (dwFlags&TRADE_GARBAGE)!=NULL; }
		//inline bool IsTIMELMT() const { return (dwFlags&ITEM_TIMELMT) != NULL; }


    private:
        // SITEMCUSTOM에서의 접근을 허가한다.
        friend struct SITEMCUSTOM;

        // 툴에서 사용하기 위함
        friend class CItemParent;
	};

} // namespace ITEM

#endif // _GL_ITEM_BASIC_H_
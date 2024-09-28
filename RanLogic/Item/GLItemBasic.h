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
        SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )

        char		szName[ITEM_SZNAME];		//	�������� �̸�.	
        EMITEMLEVEL	emLevel;					//	������ ��ġ ���.

        DWORD		dwFlags;					//	Trade �Ӽ�.
        DWORD		dwPrice;					//	������ ����.

        EMITEM_TYPE	emItemType;					//	������ ����.

        //	���� ����.
        EMBRIGHT	emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
        DWORD		dwReqCharClass;				//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
        WORD		wReqLevel;					//	�䱸 Level.
        WORD		wReqPA;						//	�䱸 ����ġ.
        WORD		wReqSA;						//	�䱸 ���ġ.
        SCHARSTATS	sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

        WORD		wInvenSizeX;				//	�κ��丮 ������.
        WORD		wInvenSizeY;				//	�κ��丮 ������.

        char		szFieldFile[MAX_PATH];				//	�ٴ� ���� ����.
        char		szInventoryFile[MAX_PATH];			//	�κ��丮 ���� ����.
        char		szWearingFIle[GLCI_NUM][MAX_PATH];	//	���� ����.
        char		szComment[ITEM_SZCOMMENT];			//	�����ۿ� ���� ������ ����.
    };

    struct SBASIC_101
    {
        SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )

        char		szName[ITEM_SZNAME];		//	�������� �̸�.	
        EMITEMLEVEL	emLevel;					//	������ ��ġ ���.

        DWORD		dwFlags;					//	Trade �Ӽ�.
        DWORD		dwPrice;					//	������ ����.

        EMITEM_TYPE	emItemType;					//	������ ����.

        //	���� ����.
        EMBRIGHT	emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
        DWORD		dwReqCharClass;				//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
        WORD		wReqLevel;					//	�䱸 Level.
        WORD		wReqPA;						//	�䱸 ����ġ.
        WORD		wReqSA;						//	�䱸 ���ġ.
        SCHARSTATS	sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

        WORD		wInvenSizeX;				//	�κ��丮 ������.
        WORD		wInvenSizeY;				//	�κ��丮 ������.

        char		szSelfBodyEffect[MAX_PATH];	//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
        char		szTargBodyEffect[MAX_PATH];	//	��ǥ ������ ���� �״� ����Ʈ.
        char		szTargetEffect[MAX_PATH];	//	��ǥ ���� ����Ʈ.

        char		szFieldFile[MAX_PATH];				//	�ٴ� ���� ����.
        char		szInventoryFile[MAX_PATH];			//	�κ��丮 ���� ����.
        char		szWearingFIle[GLCI_NUM][MAX_PATH];	//	���� ����.
        char		szComment[ITEM_SZCOMMENT];			//	�����ۿ� ���� ������ ����.
    };

    struct SBASIC_102
    {
        SNATIVEID	sNativeID;					//	���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )

        char		szName[ITEM_SZNAME];		//	�������� �̸�.	
        EMITEMLEVEL	emLevel;					//	������ ��ġ ���.

        DWORD		dwFlags;					//	Trade �Ӽ�.
        DWORD		dwPrice;					//	������ ����.

        EMITEM_TYPE	emItemType;					//	������ ����.

        //	���� ����.
        EMBRIGHT	emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
        DWORD		dwReqCharClass;				//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
        WORD		wReqLevel;					//	�䱸 Level.
        WORD		wReqPA;						//	�䱸 ����ġ.
        WORD		wReqSA;						//	�䱸 ���ġ.
        SCHARSTATS	sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

        WORD		wInvenSizeX;				//	�κ��丮 ������.
        WORD		wInvenSizeY;				//	�κ��丮 ������.

        SNATIVEID	sICONID;					//	������ �ε���.

        char		szSelfBodyEffect[MAX_PATH];	//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
        char		szTargBodyEffect[MAX_PATH];	//	��ǥ ������ ���� �״� ����Ʈ.
        char		szTargetEffect[MAX_PATH];	//	��ǥ ���� ����Ʈ.

        char		szFieldFile[MAX_PATH];				//	�ٴ� ���� ����.
        char		szInventoryFile[MAX_PATH];			//	�κ��丮 ���� ����.
        char		szWearingFIle[GLCI_NUM][MAX_PATH];	//	���� ����.
        char		szComment[ITEM_SZCOMMENT];			//	�����ۿ� ���� ������ ����.
    };

    struct SBASIC
    {
        enum { VERSION = 0x0130 };

        SNATIVEID	sNativeID;					// ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
        SNATIVEID	sGroupID;					// ???

        std::string	strName;					// �������� �̸�.	
        EMITEMLEVEL	emLevel;					// ������ ��ġ ���.

        WORD		wGradeAttack;				// ���� ���� ���
        WORD		wGradeDefense;				// ��� ���� ���

        float		fExpMultiple;				// ����ġ ���� ���

        WORD		wReserved1;					// ��� ����
        WORD		wReserved2;					// ��� ����
        WORD		wReserved3;					// ��� ����
        WORD		wReserved4;					// ��� ����
        WORD		wReserved5;					// ��� ����

        
    private:
        // �� �÷��״� ���� �ǵ帮�� �ȵȴ�.
        // �ǵ帮�� ������  GLItemCustom.h ( SITEMCUSTOM )�� ���� �����϶�.
        DWORD		dwFlags;				    // Trade �Ӽ�.

	public :
        int			nRPTrade;                  // RanPoint (RP) Trade ���� ȸ��

        LONGLONG		dwBuyPrice;					// ������ ���� ����.
        LONGLONG		dwSellPrice;				// ������ �Ǹ� ����

        EMITEM_TYPE	emItemType;					// ������ ����.

        //	���� ����.
        EMBRIGHT	emReqBright;				//	ĳ�� �Ӽ�. ( ��/�� )
        DWORD		dwReqCharClass;				//	������ �� �ִ� ������. ( EMCHARCLASS Flags )
        DWORD		dwReqSchool;				//	������ �� �ִ� �п�. 
        WORD		wReqLevelDW;				//	�䱸 Level ���Ѽ�
        WORD		wReqLevelUP;				//  �䱸 Level ���Ѽ�
        WORD		wReqPA;						//	�䱸 ����ġ.
        WORD		wReqSA;						//	�䱸 ���ġ.
        SCHARSTATS	sReqStats;					//	�䱸 �ɸ��� Stats ��ġ.

        SNATIVEID	sICONID;					//	������ �ε���.

        std::string		strSelfBodyEffect;		//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
        std::string		strTargBodyEffect;		//	��ǥ ������ ���� �״� ����Ʈ.
        std::string		strTargetEffect;		//	��ǥ ���� ����Ʈ.
		std::string		strGeneralEffect;		//	���� ����Ʈ;

        std::string		strFieldFile;				//	�ٴ� ���� ����.
        std::string		strInventoryFile;			//	�κ��丮 ���� ����.

		std::string		strArrInventoryFile[GLCI_NUM_ACTOR];//	������ �κ��丮 ���� ����.
        std::string		strWearingFile[GLCI_NUM_ACTOR];		//	���� ����.
        std::string		strWearingFileEx[GLCI_NUM_ACTOR];	//	���� ����.

        std::string		strComment;					//	�����ۿ� ���� ������ ����.

        // PET
        std::string		strPetWearingFile;			//  ���� ��������. 

        SNATIVEID		sSubID;						// mid/sid ( ���� MID/SID ) 
        WORD			wPosX;						// X��ǥ
        WORD			wPosY;						// Y��ǥ

        DWORD			dwCoolTime;					// ������ ��� ��Ÿ��
        EMCOOL_TYPE		emCoolType;					// ��Ÿ�� Ÿ��

        bool			bSearch;					// ������ �˻� ���� ����.		

        int             emReqActivityType;          //  �ʿ� �䱸Ȱ��
        WORD            wReqActPointDW;             //  �䱸 ����Ʈ �ּ� 
        WORD            wReqActPointUP;             //  �䱸 ����Ʈ �ִ�

		DWORD			dwReqContribution;			// �䱸 �⿩��

		// ���� �����ϰ� Ŭ��, ������ ��� �˻� �ӵ��� ���� ������ ���� �߰��۾��� ��;
		// �Ⱓ ���� �������� ��츦 �����ϰ�;
		// SITEM->sBasicOp.strItemPeriodExtension�� �Ⱓ ���� ����Ʈ�� ����صд�;
		// ������ �Ⱓ ���� �����۸� ����Ѵ�;
		// ���� �߰� [9/7/2016 gbgim];
		// �Ⱓ ���� ���� ���������� �ܼ� �Ǻ��ϱ� �����̴�;
		// �ΰ��ӿ��� �ε�� �ߺ��̰� ������� �ӵ��� ���� �־�д�;
		std::string		strItemPeriodExtension;		// ������ �Ⱓ ���� ����;

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
        bool LOAD_114 ( sc::BaseStream &SFile );	//	������ ���� ���� ����
        bool LOAD_115 ( sc::BaseStream &SFile );	//	���к� �߰�
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

        //! ������ ����
        inline EMITEM_TYPE Type() const { return emItemType; }

        //! ���� ID. ( ������ ���� ���� �����ÿ� �ο��Ǵ� ���� ID )
        inline SNATIVEID GetNativeID() const { return sNativeID; }


		inline int RanPointTradeCount() const 
		{ 
			if( nRPTrade < 0 )
				return 0; 

			return nRPTrade; 
		}
		inline bool IsCoolTime() const { return ( dwCoolTime != 0 ); }

		// ������ ���;
		inline const EMITEMLEVEL GetItemLevel ( void ) const { return emLevel; }

		// ����
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
        // SITEMCUSTOM������ ������ �㰡�Ѵ�.
        friend struct SITEMCUSTOM;

        // ������ ����ϱ� ����
        friend class CItemParent;
	};

} // namespace ITEM

#endif // _GL_ITEM_BASIC_H_
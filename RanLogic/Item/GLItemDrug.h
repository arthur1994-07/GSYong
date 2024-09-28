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

    struct SDRUG_101	//	������ ���� ���� ����
    {
        __time64_t	tTIME_LMT;		//	���Ѻ� �������� ��� ��ȿ �Ⱓ.
        BOOL		bInstance;		//	�Ҹ� ����.
        EMITEM_DRUG	emDrug;			//	��ǰ Ÿ��.
        WORD		wPileNum;		//	�ִ��ħ��. ( old : wApplyNum )

        BOOL		bRatio;			//	���� ��ŭ ġ��.

        union
        {
            struct { WORD	wCureVolume; };	//	ġ�� �뷮. ( �Ϲ� ��ǰ�� ���. )
            struct { WORD	wArrowNum; };	//	ȭ�� �ѹ�����. ( ȭ���� ���. )
        };

        DWORD		dwCureDISORDER;	//	"DISORDER" ġ�� ���.
    };

	//	��ǰ ��, �Ҹ� ���� ������.
	struct SDRUG_102
	{
		enum { VERSION = 0x0102 };

		EMITEM_DRUG	emDrug;			//! ��ǰ Ÿ��.
		BOOL		bInstance;		//! �Ҹ� ����.
		__time64_t	tTIME_LMT;		//! ���Ѻ� �������� ��� ��ȿ �Ⱓ.
		BOOL		bRatio;			//! ���� ��ŭ ġ��.
		WORD		wPileNum;		//! �ִ��ħ�� (old:wApplyNum)

		union
		{
			struct { WORD wArrowNum; };	  //! ȭ�� �ѹ�����(ȭ���� ���)
			struct { WORD wCureVolume; }; //! ġ�� �뷮(�Ϲ� ��ǰ�� ���)
			struct { WORD PointCharge; }; //! Point ī���ϰ�� Point ������
			struct { WORD wStage; };	  //! ������ ��� �δ� ��������
			struct { WORD wLv; };		  //! ������ ��� �δ� �������� ���̵�
		};

		DWORD dwCureDISORDER; //! "DISORDER" ġ�� ���.
	};

	struct SDRUG_103
	{
		enum { VERSION = 0x0103 };

		EMITEM_DRUG	emDrug;			//! ��ǰ Ÿ��.
		BOOL		bInstance;		//! �Ҹ� ����.
		__time64_t	tTIME_LMT;		//! ���Ѻ� �������� ��� ��ȿ �Ⱓ.
		BOOL		bRatio;			//! ���� ��ŭ ġ��.
		WORD		wPileNum;		//! �ִ��ħ�� (old:wApplyNum)

		union
		{
			struct { DWORD dwArrowNum; };	  //! ȭ�� �ѹ�����(ȭ���� ���)
			struct { DWORD dwCureVolume; }; //! ġ�� �뷮(�Ϲ� ��ǰ�� ���)
			struct { DWORD dwPointCharge; }; //! Point ī���ϰ�� Point ������
			struct { DWORD dwStage; };	  //! ������ ��� �δ� ��������
			struct { DWORD dwLv; };		  //! ������ ��� �δ� �������� ���̵�
		};

		DWORD dwCureDISORDER; //! "DISORDER" ġ�� ���.
	};

	struct SDRUG_104
	{
		enum { VERSION = 0x0104 };

		EMITEM_DRUG	emDrug;			//! ��ǰ Ÿ��.
		BOOL		bInstance;		//! �Ҹ� ����.
		__time64_t	tTIME_LMT;		//! ���Ѻ� �������� ��� ��ȿ �Ⱓ.
		BOOL		bRatio;			//! ���� ��ŭ ġ��.
		WORD		wPileNum;		//! �ִ��ħ�� (old:wApplyNum)

		union
		{
			struct { DWORD dwArrowNum; };	  //! ȭ�� �ѹ�����(ȭ���� ���)
			struct { DWORD dwCureVolume; }; //! ġ�� �뷮(�Ϲ� ��ǰ�� ���)
			struct { DWORD dwPointCharge; }; //! Point ī���ϰ�� Point ������
			struct { DWORD dwStage; };	  //! ������ ��� �δ� ��������
			struct { DWORD dwLv; };		  //! ������ ��� �δ� �������� ���̵�
		};

		DWORD dwCureDISORDER; //! "DISORDER" ġ�� ���.
		__time64_t tDuration; // ���ӽð�
	};

    //	��ǰ ��, �Ҹ� ���� ������.
    struct SDRUG
    {
        enum { VERSION = 0x0105 };

        EMITEM_DRUG	emDrug;			//! ��ǰ Ÿ��.
        BOOL		bInstance;		//! �Ҹ� ����.
        __time64_t	tTIME_LMT;		//! ���Ѻ� �������� ��� ��ȿ �Ⱓ.
        BOOL		bRatio;			//! ���� ��ŭ ġ��.
        WORD		wPileNum;		//! �ִ��ħ�� (old:wApplyNum)

        union
        {
            struct { DWORD dwArrowNum; };	  //! ȭ�� �ѹ�����(ȭ���� ���)
            struct { DWORD dwCureVolume; }; //! ġ�� �뷮(�Ϲ� ��ǰ�� ���)
            struct { DWORD dwPointCharge; }; //! Point ī���ϰ�� Point ������
			struct { DWORD dwStage; };	  //! ������ ��� �δ� ��������
			struct { DWORD dwLv; };		  //! ������ ��� �δ� �������� ���̵�
			struct { DWORD dwCompressPrecent; };		  //! ����ġ ���� �� ĸ�� 
        };

        DWORD	   dwCureDISORDER; //! "DISORDER" ġ�� ���.
		__time64_t tDuration;	   // ���ӽð�
		DWORD	   dwERList;       // ȿ���������� �϶��� ����ϴ� ȿ������ ����Ʈ

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

        //! DISORDER ġ�� ���.
        inline DWORD GetCureDisOrder() const { return dwCureDISORDER; }

        //!	��ǰ Ÿ��
        inline EMITEM_DRUG GetType() const { return emDrug; }

        //! �Ҹ� ����
        inline BOOL GetInstance() const { return bInstance; }

        ////! ���Ѻ� �������� ��� ��ȿ �Ⱓ
        //inline __time64_t GetTimeLimit() const { return tTIME_LMT; }

        //! ���� ��ŭ ġ��.
        inline BOOL GetRatio() const { return bRatio; }

        //!	�ִ��ħ��. (old:wApplyNum)
        inline DWORD GetPileNum() const { return wPileNum; }

        //! ȭ�� �ѹ�����(ȭ���� ���)
        inline DWORD GetArrowNum() const { return dwArrowNum; }

        //! ġ�� �뷮(�Ϲ� ��ǰ�� ���)
        inline DWORD GetCureVolumn() const { return dwCureVolume; }

        //! Point ī���ϰ�� Point ������
        inline DWORD GetPointCharge() const { return dwPointCharge; }

		inline DWORD GetCompressPrecent() const { return dwCompressPrecent; }
		inline void SetCompressPrecent( DWORD percent ) { dwCompressPrecent = percent; }

    private:
        /************************************************************************/
        // ���� �� �Լ��� �ʿ��ϴٸ� SITEMCUSTOM.h�� �����϶�.
        // .\GLItemCustom.h

		// ���Ѻ� �������� ��� ��ȿ �Ⱓ.
		bool IsTimeLimited() const;
		bool IsTimeDuration() const;

        // SITEMCUSTOM������ ������ �㰡�Ѵ�.
        friend struct SITEMCUSTOM;

    };

} // namespace ITEM
#endif // _GL_ITEM_DRUG_H_
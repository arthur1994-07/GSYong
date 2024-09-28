#pragma once

#include <vector>
#include "../../EngineLib/G-Logic/TypeDefine.h"

//-----------------------------
//  [2/8/2013 hsshin]
// ���� �ý��� ������
//-----------------------------

namespace sc 
{
	class BaseStream;
	class SerialFile;
}

struct SProductItem
{
	SNATIVEID sNID;
	BYTE nNum;

	SProductItem();
	void RESET();
};

namespace Product
{
	enum
	{
 		ETypeHeadGear = 0,			    // ����
 		ETypeUpper,					    // ����
 		ETypeLower,					    // ����
 		ETypeHand,					    // �尩
 		ETypeFoot,					    // �Ź�
 		ETypeHandheld,				    // ����
 		ETypeNeck_Earring,			    // ����� + �Ͱ���
 		ETypeWrist_WaistBand,		    // ���� + ��Ʈ
		ETypeFinger_Accessories,	    // ���� + ��ű�
		ETypeEtc,					    // ��Ÿ + VEHICLE;
		ETypeRandomBox,				    // �����ڽ�
		ETypeCostume,				    // �ڽ�Ƭ EFindEtc == 13

		ETypeTotal,					    // ��� (�Ǹ������� �־���� �ִ� �о߼��� �����ϴµ� ����)
	};
	enum
	{
		ITEM_PRODUCT_MAX_MATERIAL = 5,	//���۾������� �ִ� ��᰹��
	};
    const static int PRODUCT_NOT_USE_REQ = 65535;    //�ʿ� ����,���õ� ȹ�����ġ ��Ȱ��ȭ ��
}

namespace COMMENT
{
    extern std::string PRODUCT_TYPE_NAME[Product::ETypeTotal];
}

struct SProductRecipe
{
	enum
	{
		VERSION = 0x0102,	            //  [2/8/2013 hsshin] ���õ� �ý��� �߰��� ���� ���� 1.01 -> 1.02
	};
	DWORD           dwKey;			    //���չ� ����Ű

	SProductItem    sMeterialItem[ Product::ITEM_PRODUCT_MAX_MATERIAL ];
	SProductItem    sResultItem;

	DWORD           dwRate;			    //���� ���� Ȯ�� 
	DWORD           dwPrice;			//���� ����
	FLOAT           fDelaySec;		    //���� �ð�

	BOOL            bNeedLearn;		            
	BOOL            bRandomOption;		//�����ɼ�

	BYTE            nGradeAttack;
	BYTE            nGradeDefense;

	//  [2/8/2013 hsshin] ���õ� �ý����߰�
	WORD            wLevel;				//�ʿ䷹��
	WORD            wWs;				//�ʿ���õ�
	WORD            wWsp;				//ȹ����õ�
	WORD            wWspPer;			//���õ�ȹ��Ȯ��

	WORD            eProductType;		//���չ� ���ۺо�

    SProductRecipe();

	BOOL            SaveFile( sc::SerialFile& SFile );
	BOOL            LoadFile( sc::BaseStream& SFile );
	BOOL            LoadFile102( sc::BaseStream& SFile );   //���õ� �ý��� �߰� �������� �о����
	BOOL            LoadFile101( sc::BaseStream& SFile );
	BOOL            LoadFile100( sc::BaseStream& SFile );

	static void     SaveCsvHead( std::fstream &SFile );
	void            SaveCsv( std::fstream &SFile );
	void            LoadCsv( CStringArray &StrArray );
};

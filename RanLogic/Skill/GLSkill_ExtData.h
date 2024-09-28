#pragma once

#include "../Item/GLItemDef.h"
#include "../Character/GLCharDefine.h"

#include "./GLSkillDef.h"

namespace SKILL
{
	struct SEXT_DATA_100
	{
		EMANI_MAINTYPE		emANIMTYPE;						//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;						//	��ų ���� ĳ������ ���ϸ��̼� ����.		

		EMEFFECTIME			emTARG;
		char				szTARG[MAX_SZFILE];				//	��ǥ ���� ����Ʈ.

		EMEFFECTIME			emSELFZONE01;
		char				szSELFZONE01[MAX_SZFILE];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		char				szTARGZONE01[MAX_SZFILE];		//	��ǥ ��ġ ����Ʈ.
		EMEFFECTIME			emTARGZONE02;
		char				szTARGZONE02[MAX_SZFILE];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		char				szSELFBODY[MAX_SZFILE];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		EMEFFECTIME			emTARGBODY01;
		char				szTARGBODY01[MAX_SZFILE];		//	��ǥ ������ ���� �״� ����Ʈ.
		EMEFFECTIME			emTARGBODY02;
		char				szTARGBODY02[MAX_SZFILE];		//	��ǥ ������ ���� �״� ����Ʈ.

		SNATIVEID			sICONINDEX;						//	������ �ε���.
		char				szICONFILE[MAX_SZFILE];			//	������ �ؽ��� ���� �̸�.

		char				szCOMMENTS[MAX_SZCOMMENT];		//	��ų�� ���� ������ ����.
	};

	struct SEXT_DATA_101
	{
		EMANI_MAINTYPE		emANIMTYPE;						//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;						//	��ų ���� ĳ������ ���ϸ��̼� ����.		

		EMEFFECTIME			emTARG;
		char				szTARG[MAX_SZFILE];				//	��ǥ ���� ����Ʈ.

		EMEFFECTIME			emSELFZONE01;
		char				szSELFZONE01[MAX_SZFILE];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE02;
		char				szSELFZONE02[MAX_SZFILE];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		char				szTARGZONE01[MAX_SZFILE];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE02;
		char				szTARGZONE02[MAX_SZFILE];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		char				szSELFBODY[MAX_SZFILE];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		EMEFFECTIME			emTARGBODY01;
		char				szTARGBODY01[MAX_SZFILE];		//	��ǥ ������ ���� �״� ����Ʈ.
		EMEFFECTIME			emTARGBODY02;
		char				szTARGBODY02[MAX_SZFILE];		//	��ǥ ������ ���� �״� ����Ʈ.

		SNATIVEID			sICONINDEX;						//	������ �ε���.
		char				szICONFILE[MAX_SZFILE];			//	������ �ؽ��� ���� �̸�.

		char				szCOMMENTS[MAX_SZCOMMENT];		//	��ų�� ���� ������ ����.
	};

	struct SEXT_DATA_102
	{
		EMANI_MAINTYPE		emANIMTYPE;						//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;						//	��ų ���� ĳ������ ���ϸ��̼� ����.		

		EMEFFECTIME			emTARG;
		EMEFFECTPOS			emTARG_POSA;
		EMEFFECTPOS			emTARG_POSB;
		char				szTARG[MAX_SZFILE];				//	��ǥ ���� ����Ʈ.

		EMEFFECTIME			emSELFZONE01;
		EMEFFECTPOS			emSELFZONE01_POS;
		char				szSELFZONE01[MAX_SZFILE];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE02;
		EMEFFECTPOS			emSELFZONE02_POS;
		char				szSELFZONE02[MAX_SZFILE];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		EMEFFECTPOS			emTARGZONE01_POS;
		char				szTARGZONE01[MAX_SZFILE];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE02;
		EMEFFECTPOS			emTARGZONE02_POS;
		char				szTARGZONE02[MAX_SZFILE];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		char				szSELFBODY[MAX_SZFILE];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
		EMEFFECTIME			emTARGBODY01;
		char				szTARGBODY01[MAX_SZFILE];		//	��ǥ ������ ���� �״� ����Ʈ.
		EMEFFECTIME			emTARGBODY02;
		char				szTARGBODY02[MAX_SZFILE];		//	��ǥ ������ ���� �״� ����Ʈ.

		SNATIVEID			sICONINDEX;						//	������ �ε���.
		char				szICONFILE[MAX_SZFILE];			//	������ �ؽ��� ���� �̸�.

		char				szCOMMENTS[MAX_SZCOMMENT];		//	��ų�� ���� ������ ����.
	};

	struct SEXT_DATA_103
	{
		EMANI_MAINTYPE		emANIMTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		

		EMEFFECTIME			emTARG;
		EMEFFECTPOS			emTARG_POSA;
		EMEFFECTPOS			emTARG_POSB;
		std::string			strTARG[EMELEMENT_OLDMAX];				//	��ǥ ���� ����Ʈ.

		EMEFFECTIME			emSELFZONE01;
		EMEFFECTPOS			emSELFZONE01_POS;
		std::string			strSELFZONE01[EMELEMENT_OLDMAX];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE02;
		EMEFFECTPOS			emSELFZONE02_POS;
		std::string			strSELFZONE02[EMELEMENT_OLDMAX];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		EMEFFECTPOS			emTARGZONE01_POS;
		std::string			strTARGZONE01[EMELEMENT_OLDMAX];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE02;
		EMEFFECTPOS			emTARGZONE02_POS;
		std::string			strTARGZONE02[EMELEMENT_OLDMAX];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		std::string			strSELFBODY[EMELEMENT_OLDMAX];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY01;
		std::string			strTARGBODY01[EMELEMENT_OLDMAX];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY02;
		std::string			strTARGBODY02[EMELEMENT_OLDMAX];		//	��ǥ ������ ���� �״� ����Ʈ.

		std::string			strHOLDOUT;								//	��� ����Ʈ.

		SNATIVEID			sICONINDEX;								//	������ �ε���.
		std::string			strICONFILE;							//	������ �ؽ��� ���� �̸�.

		std::string			strCOMMENTS;							//	��ų�� ���� ������ ����.

		BOOL LOAD ( sc::BaseStream &SFile );
	};

	struct SEXT_DATA_104
	{
		EMANI_MAINTYPE		emANIMTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		

		float				fDELAY4DAMAGE;							//	����� �߻� ������.
		EMEFFECTIME			emTARG;
		EMEFFECTPOS			emTARG_POSA;
		EMEFFECTPOS			emTARG_POSB;
		std::string			strTARG[EMELEMENT_OLDMAX];				//	��ǥ ���� ����Ʈ.

		EMEFFECTIME			emSELFZONE01;
		EMEFFECTPOS			emSELFZONE01_POS;
		std::string			strSELFZONE01[EMELEMENT_OLDMAX];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE02;
		EMEFFECTPOS			emSELFZONE02_POS;
		std::string			strSELFZONE02[EMELEMENT_OLDMAX];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		EMEFFECTPOS			emTARGZONE01_POS;
		std::string			strTARGZONE01[EMELEMENT_OLDMAX];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE02;
		EMEFFECTPOS			emTARGZONE02_POS;
		std::string			strTARGZONE02[EMELEMENT_OLDMAX];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		std::string			strSELFBODY[EMELEMENT_OLDMAX];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY01;
		std::string			strTARGBODY01[EMELEMENT_OLDMAX];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY02;
		std::string			strTARGBODY02[EMELEMENT_OLDMAX];		//	��ǥ ������ ���� �״� ����Ʈ.

		std::string			strHOLDOUT;								//	��� ����Ʈ.

		SNATIVEID			sICONINDEX;								//	������ �ε���.
		std::string			strICONFILE;							//	������ �ؽ��� ���� �̸�.

		std::string			strCOMMENTS;							//	��ų�� ���� ������ ����.

		BOOL LOAD ( sc::BaseStream &SFile );
	};

	struct SEXT_DATA_105
	{
		EMANI_MAINTYPE		emANIMTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		

		float				fDELAY4DAMAGE;							//	����� �߻� ������.
		EMEFFECTIME			emTARG;
		EMEFFECTPOS			emTARG_POSA;
		EMEFFECTPOS			emTARG_POSB;
		bool				bTARG_ONE;								//	��ǥ ���� ����Ʈ ���� ����.
		std::string			strTARG[EMELEMENT_OLDMAX];				//	��ǥ ���� ����Ʈ.

		EMEFFECTIME			emSELFZONE01;
		EMEFFECTPOS			emSELFZONE01_POS;
		std::string			strSELFZONE01[EMELEMENT_OLDMAX];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE02;
		EMEFFECTPOS			emSELFZONE02_POS;
		std::string			strSELFZONE02[EMELEMENT_OLDMAX];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		EMEFFECTPOS			emTARGZONE01_POS;
		std::string			strTARGZONE01[EMELEMENT_OLDMAX];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE02;
		EMEFFECTPOS			emTARGZONE02_POS;
		std::string			strTARGZONE02[EMELEMENT_OLDMAX];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		std::string			strSELFBODY[EMELEMENT_OLDMAX];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY01;
		std::string			strTARGBODY01[EMELEMENT_OLDMAX];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY02;
		std::string			strTARGBODY02[EMELEMENT_OLDMAX];		//	��ǥ ������ ���� �״� ����Ʈ.

		std::string			strHOLDOUT;								//	��� ����Ʈ.

		SNATIVEID			sICONINDEX;								//	������ �ε���.
		std::string			strICONFILE;							//	������ �ؽ��� ���� �̸�.

		std::string			strCOMMENTS;							//	��ų�� ���� ������ ����.

		BOOL LOAD ( sc::BaseStream &SFile );

	};

	struct SEXT_DATA_106	//	������ ���� ���� ����
	{
		EMANI_MAINTYPE		emANIMTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		

		float				fDELAY4DAMAGE;							//	����� �߻� ������.
		EMEFFECTIME			emTARG;
		EMEFFECTPOS			emTARG_POSA;
		EMEFFECTPOS			emTARG_POSB;
		bool				bTARG_ONE;								//	��ǥ ���� ����Ʈ ���� ����.
		std::string			strTARG[EMELEMENT_MAXNUM];				//	��ǥ ���� ����Ʈ.

		EMEFFECTIME			emSELFZONE01;
		EMEFFECTPOS			emSELFZONE01_POS;
		std::string			strSELFZONE01[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE02;
		EMEFFECTPOS			emSELFZONE02_POS;
		std::string			strSELFZONE02[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		EMEFFECTPOS			emTARGZONE01_POS;
		std::string			strTARGZONE01[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE02;
		EMEFFECTPOS			emTARGZONE02_POS;
		std::string			strTARGZONE02[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		std::string			strSELFBODY[EMELEMENT_MAXNUM];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY01;
		std::string			strTARGBODY01[EMELEMENT_MAXNUM];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY02;
		std::string			strTARGBODY02[EMELEMENT_MAXNUM];		//	��ǥ ������ ���� �״� ����Ʈ.

		std::string			strHOLDOUT;								//	��� ����Ʈ.

		SNATIVEID			sICONINDEX;								//	������ �ε���.
		std::string			strICONFILE;							//	������ �ؽ��� ���� �̸�.

		std::string			strCOMMENTS;							//	��ų�� ���� ������ ����.

		BOOL LOAD ( sc::BaseStream &SFile );

	};

	struct SEXT_DATA_107
	{
		EMANI_MAINTYPE		emANIMTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		

		SNATIVEID			sICONINDEX;								//	������ �ε���.
		std::string			strICONFILE;							//	������ �ؽ��� ���� �̸�.

		std::string			strCOMMENTS;							//	��ų�� ���� ������ ����.

		EMEFFECTIME			emTARG;
		EMEFFECTPOS			emTARG_POSA;
		EMEFFECTPOS			emTARG_POSB;
		float				fDELAY4DAMAGE;							//	����� �߻� ������.

		std::string			strHOLDOUT;								//	��� ����Ʈ.

		EMEFFECTIME			emSELFZONE01;
		EMEFFECTPOS			emSELFZONE01_POS;
		std::string			strSELFZONE01[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE02;
		EMEFFECTPOS			emSELFZONE02_POS;
		std::string			strSELFZONE02[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		bool				bTARG_ONE;								//	��ǥ ���� ����Ʈ ���� ����.
		std::string			strTARG[EMELEMENT_MAXNUM];				//	��ǥ ���� ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		EMEFFECTPOS			emTARGZONE01_POS;
		std::string			strTARGZONE01[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE02;
		EMEFFECTPOS			emTARGZONE02_POS;
		std::string			strTARGZONE02[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGBODY01;
		std::string			strTARGBODY01[EMELEMENT_MAXNUM];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY02;
		std::string			strTARGBODY02[EMELEMENT_MAXNUM];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		std::string			strSELFBODY[EMELEMENT_MAXNUM];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.	

		BOOL LOAD ( sc::BaseStream &SFile );
	};

	struct SEXT_DATA_108
	{
		enum { VERSION = 0x0108 };

		EMANI_MAINTYPE		emANIMTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.
		EMANI_MAINTYPE		emANIVehicleMTYPE;						//	��ų ���� ����ũ �ִϸ��̼� ����

		SNATIVEID			sICONINDEX;								//	������ �ε���.
		std::string			strICONFILE;							//	������ �ؽ��� ���� �̸�.

		std::string			strCOMMENTS;							//	��ų�� ���� ������ ����.

		EMEFFECTIME			emTARG;
		EMEFFECTPOS			emTARG_POSA;
		EMEFFECTPOS			emTARG_POSB;
		float				fDELAY4DAMAGE;							//	����� �߻� ������.

		std::string			strHOLDOUT;								//	��� ����Ʈ.

		EMEFFECTIME			emSELFZONE01;
		EMEFFECTPOS			emSELFZONE01_POS;
		std::string			strSELFZONE01[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE02;
		EMEFFECTPOS			emSELFZONE02_POS;
		std::string			strSELFZONE02[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		bool				bTARG_ONE;								//	��ǥ ���� ����Ʈ ���� ����.
		std::string			strTARG[EMELEMENT_MAXNUM];				//	��ǥ ���� ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		EMEFFECTPOS			emTARGZONE01_POS;
		std::string			strTARGZONE01[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE02;
		EMEFFECTPOS			emTARGZONE02_POS;
		std::string			strTARGZONE02[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGBODY01;
		std::string			strTARGBODY01[EMELEMENT_MAXNUM];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY02;
		std::string			strTARGBODY02[EMELEMENT_MAXNUM];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		std::string			strSELFBODY[EMELEMENT_MAXNUM];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.		

		BOOL LOAD ( sc::BaseStream &SFile );
	};

	struct SEXT_DATA_109
	{
		enum { VERSION = 0x0109 };

		EMANI_MAINTYPE		emANIMTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.
		EMANI_MAINTYPE		emANIVehicleMTYPE;						//	��ų ���� ����ũ �ִϸ��̼� ����

		SNATIVEID			sICONINDEX;								//	������ �ε���.
		std::string			strICONFILE;							//	������ �ؽ��� ���� �̸�.

		std::string			strCOMMENTS;							//	��ų�� ���� ������ ����.

		EMEFFECTIME			emTARG;
		EMEFFECTPOS			emTARG_POSA;
		EMEFFECTPOS			emTARG_POSB;
		float				fDELAY4DAMAGE;							//	����� �߻� ������.

		std::string			strHOLDOUT;								//	��� ����Ʈ.

		EMEFFECTIME			emSELFZONE01;
		EMEFFECTPOS			emSELFZONE01_POS;
		std::string			strSELFZONE01[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE02;
		EMEFFECTPOS			emSELFZONE02_POS;
		std::string			strSELFZONE02[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE03;
		EMEFFECTPOS			emSELFZONE03_POS;
		std::string			strSELFZONE03[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		bool				bTARG_ONE;								//	��ǥ ���� ����Ʈ ���� ����.
		std::string			strTARG[EMELEMENT_MAXNUM];				//	��ǥ ���� ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		EMEFFECTPOS			emTARGZONE01_POS;
		std::string			strTARGZONE01[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE02;
		EMEFFECTPOS			emTARGZONE02_POS;
		std::string			strTARGZONE02[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE03;
		EMEFFECTPOS			emTARGZONE03_POS;
		std::string			strTARGZONE03[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGBODY01;
		std::string			strTARGBODY01[EMELEMENT_MAXNUM];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY02;
		std::string			strTARGBODY02[EMELEMENT_MAXNUM];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		std::string			strSELFBODY[EMELEMENT_MAXNUM];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.		

		SNATIVEID			idMultiTargetSkill;
		int					nMultiTatgetEffectFrame;				

		SEXT_DATA_109 (void) :
		emANIMTYPE(AN_SKILL),
			emANISTYPE(AN_SUB_00),
			emANIVehicleMTYPE( AN_VEHICLE_SKILL_A ),

			fDELAY4DAMAGE(0.0f),

			emTARG_POSA(EMPOS_GROUND),
			emTARG_POSB(EMPOS_GROUND),
			bTARG_ONE(false),

			emSELFZONE01_POS(EMPOS_GROUND),
			emSELFZONE02_POS(EMPOS_GROUND),
			emSELFZONE03_POS	(EMPOS_GROUND),			
			emTARGZONE01_POS(EMPOS_GROUND),
			emTARGZONE02_POS(EMPOS_GROUND),
			emTARGZONE03_POS(EMPOS_GROUND),

			emTARG(EMTIME_IMPACT),
			emSELFZONE01(EMTIME_FIRST),
			emSELFZONE02(EMTIME_IMPACT),
			emSELFZONE03(EMTIME_EFFECT), 
			emTARGZONE01(EMTIME_TARG_OVR),
			emTARGZONE02(EMTIME_PASSIVE),			
			emTARGZONE03(EMTIME_EFFECT),

			emSELFBODY(EMTIME_FIRST),
			emTARGBODY01(EMTIME_TARG_OVR),
			emTARGBODY02(EMTIME_PASSIVE),
			nMultiTatgetEffectFrame(0),				

			sICONINDEX(0,0)
		{
			idMultiTargetSkill = NATIVEID_NULL();
		}		

		BOOL LOAD ( sc::BaseStream &SFile );		
	};

	struct SEXT_DATA_110
	{
		enum { VERSION = 0x0110 };

		EMANI_MAINTYPE		emANIMTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.
		EMANI_MAINTYPE		emANIVehicleMTYPE;						//	��ų ���� ����ũ �ִϸ��̼� ����

		SNATIVEID			sICONINDEX;								//	������ �ε���.
		std::string			strICONFILE;							//	������ �ؽ��� ���� �̸�.

		std::string			strCOMMENTS;							//	��ų�� ���� ������ ����.

		EMEFFECTIME			emTARG;
		EMEFFECTPOS			emTARG_POSA;
		EMEFFECTPOS			emTARG_POSB;
		float				fDELAY4DAMAGE;							//	����� �߻� ������.

		std::string			strHOLDOUT;								//	��� ����Ʈ.

		EMEFFECTIME			emSELFZONE01;
		EMEFFECTPOS			emSELFZONE01_POS;
		std::string			strSELFZONE01[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE02;
		EMEFFECTPOS			emSELFZONE02_POS;
		std::string			strSELFZONE02[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE03;
		EMEFFECTPOS			emSELFZONE03_POS;
		std::string			strSELFZONE03[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		bool				bTARG_ONE;								//	��ǥ ���� ����Ʈ ���� ����.
		std::string			strTARG[EMELEMENT_MAXNUM];				//	��ǥ ���� ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		EMEFFECTPOS			emTARGZONE01_POS;
		std::string			strTARGZONE01[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE02;
		EMEFFECTPOS			emTARGZONE02_POS;
		std::string			strTARGZONE02[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE03;
		EMEFFECTPOS			emTARGZONE03_POS;
		std::string			strTARGZONE03[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGBODY01;
		std::string			strTARGBODY01[EMELEMENT_MAXNUM];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY02;
		std::string			strTARGBODY02[EMELEMENT_MAXNUM];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		std::string			strSELFBODY[EMELEMENT_MAXNUM];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.		

		SNATIVEID			idMultiTargetSkill;
		int					nMultiTatgetEffectFrame;

		SEXT_DATA_110 (void) :
		emANIMTYPE(AN_SKILL),
			emANISTYPE(AN_SUB_00),
			emANIVehicleMTYPE( AN_VEHICLE_SKILL_A ),

			fDELAY4DAMAGE(0.0f),

			emTARG_POSA(EMPOS_GROUND),
			emTARG_POSB(EMPOS_GROUND),
			bTARG_ONE(false),

			emSELFZONE01_POS(EMPOS_GROUND),
			emSELFZONE02_POS(EMPOS_GROUND),
			emSELFZONE03_POS	(EMPOS_GROUND),			
			emTARGZONE01_POS(EMPOS_GROUND),
			emTARGZONE02_POS(EMPOS_GROUND),
			emTARGZONE03_POS(EMPOS_GROUND),

			emTARG(EMTIME_IMPACT),
			emSELFZONE01(EMTIME_FIRST),
			emSELFZONE02(EMTIME_IMPACT),
			emSELFZONE03(EMTIME_EFFECT), 
			emTARGZONE01(EMTIME_TARG_OVR),
			emTARGZONE02(EMTIME_PASSIVE),			
			emTARGZONE03(EMTIME_EFFECT),

			emSELFBODY(EMTIME_FIRST),
			emTARGBODY01(EMTIME_TARG_OVR),
			emTARGBODY02(EMTIME_PASSIVE),
			nMultiTatgetEffectFrame(0),				

			sICONINDEX(0,0)
		{
			idMultiTargetSkill = NATIVEID_NULL();
		}

		BOOL LOAD ( sc::BaseStream &SFile );
	};

	struct SEXT_DATA_111
	{
		enum { VERSION = 0x0111 };

		EMANI_MAINTYPE		emANIMTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.
		EMANI_MAINTYPE		emANIVehicleMTYPE;						//	��ų ���� ����ũ �ִϸ��̼� ����

		SNATIVEID			sICONINDEX;								//	������ �ε���.
		std::string			strICONFILE;							//	������ �ؽ��� ���� �̸�.

		std::string			strCOMMENTS;							//	��ų�� ���� ������ ����.

		EMEFFECTIME			emTARG;
		EMEFFECTPOS			emTARG_POSA;
		EMEFFECTPOS			emTARG_POSB;
		float				fDELAY4DAMAGE;							//	����� �߻� ������.

		std::string			strHOLDOUT;								//	��� ����Ʈ.

		EMEFFECTIME			emSELFZONE01;
		EMEFFECTPOS			emSELFZONE01_POS;
		std::string			strSELFZONE01[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE02;
		EMEFFECTPOS			emSELFZONE02_POS;
		std::string			strSELFZONE02[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE03;
		EMEFFECTPOS			emSELFZONE03_POS;
		std::string			strSELFZONE03[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		bool				bTARG_ONE;								//	��ǥ ���� ����Ʈ ���� ����.
		std::string			strTARG[EMELEMENT_MAXNUM];				//	��ǥ ���� ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		EMEFFECTPOS			emTARGZONE01_POS;
		std::string			strTARGZONE01[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE02;
		EMEFFECTPOS			emTARGZONE02_POS;
		std::string			strTARGZONE02[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE03;
		EMEFFECTPOS			emTARGZONE03_POS;
		std::string			strTARGZONE03[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGBODY01;
		std::string			strTARGBODY01[EMELEMENT_MAXNUM];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY02;
		std::string			strTARGBODY02[EMELEMENT_MAXNUM];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		std::string			strSELFBODY[EMELEMENT_MAXNUM];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.		

		SNATIVEID			idMultiTargetSkill;
		int					nMultiTatgetEffectFrame;

		SNATIVEID			idDisguiseSkill;

		SEXT_DATA_111 (void) :
		emANIMTYPE(AN_SKILL),
			emANISTYPE(AN_SUB_00),
			emANIVehicleMTYPE( AN_VEHICLE_SKILL_A ),

			fDELAY4DAMAGE(0.0f),

			emTARG_POSA(EMPOS_GROUND),
			emTARG_POSB(EMPOS_GROUND),
			bTARG_ONE(false),

			emSELFZONE01_POS(EMPOS_GROUND),
			emSELFZONE02_POS(EMPOS_GROUND),
			emSELFZONE03_POS	(EMPOS_GROUND),			
			emTARGZONE01_POS(EMPOS_GROUND),
			emTARGZONE02_POS(EMPOS_GROUND),
			emTARGZONE03_POS(EMPOS_GROUND),

			emTARG(EMTIME_IMPACT),
			emSELFZONE01(EMTIME_FIRST),
			emSELFZONE02(EMTIME_IMPACT),
			emSELFZONE03(EMTIME_EFFECT), 
			emTARGZONE01(EMTIME_TARG_OVR),
			emTARGZONE02(EMTIME_PASSIVE),			
			emTARGZONE03(EMTIME_EFFECT),

			emSELFBODY(EMTIME_FIRST),
			emTARGBODY01(EMTIME_TARG_OVR),
			emTARGBODY02(EMTIME_PASSIVE),
			nMultiTatgetEffectFrame(0),				

			sICONINDEX(0,0)
		{
			idMultiTargetSkill = NATIVEID_NULL();
			idDisguiseSkill	   = NATIVEID_NULL();
		}

		BOOL LOAD ( sc::BaseStream &SFile );
	};

	struct SEXT_DATA_112
	{
		enum { VERSION = 0x0112 };

		EMANI_MAINTYPE		emANIMTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.		
		EMANI_SUBTYPE		emANISTYPE;								//	��ų ���� ĳ������ ���ϸ��̼� ����.
		EMANI_MAINTYPE		emANIVehicleMTYPE;						//	��ų ���� ����ũ �ִϸ��̼� ����

		SNATIVEID			sICONINDEX;								//	������ �ε���.
		std::string			strICONFILE;							//	������ �ؽ��� ���� �̸�.

		std::string			strCOMMENTS;							//	��ų�� ���� ������ ����.

		EMEFFECTIME			emTARG;
		EMEFFECTPOS			emTARG_POSA;
		EMEFFECTPOS			emTARG_POSB;
		float				fDELAY4DAMAGE;							//	����� �߻� ������.

		std::string			strHOLDOUT;								//	��� ����Ʈ.

		EMEFFECTIME			emSELFZONE01;
		EMEFFECTPOS			emSELFZONE01_POS;
		std::string			strSELFZONE01[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE02;
		EMEFFECTPOS			emSELFZONE02_POS;
		std::string			strSELFZONE02[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		EMEFFECTIME			emSELFZONE03;
		EMEFFECTPOS			emSELFZONE03_POS;
		std::string			strSELFZONE03[EMELEMENT_MAXNUM];		//	�ڱ� ��ġ ����Ʈ.

		bool				bTARG_ONE;								//	��ǥ ���� ����Ʈ ���� ����.
		std::string			strTARG[EMELEMENT_MAXNUM];				//	��ǥ ���� ����Ʈ.

		EMEFFECTIME			emTARGZONE01;
		EMEFFECTPOS			emTARGZONE01_POS;
		std::string			strTARGZONE01[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE02;
		EMEFFECTPOS			emTARGZONE02_POS;
		std::string			strTARGZONE02[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGZONE03;
		EMEFFECTPOS			emTARGZONE03_POS;
		std::string			strTARGZONE03[EMELEMENT_MAXNUM];		//	��ǥ ��ġ ����Ʈ.

		EMEFFECTIME			emTARGBODY01;
		std::string			strTARGBODY01[EMELEMENT_MAXNUM];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emTARGBODY02;
		std::string			strTARGBODY02[EMELEMENT_MAXNUM];		//	��ǥ ������ ���� �״� ����Ʈ.

		EMEFFECTIME			emSELFBODY;
		std::string			strSELFBODY[EMELEMENT_MAXNUM];			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.		

		SNATIVEID			idMultiTargetSkill;
		int					nMultiTatgetEffectFrame;

		SNATIVEID			idDisguiseSkill;

		inline void LIMIT ( EMELEMENT &emElement )
		{
			GASSERT(emElement<EMELEMENT_MAXNUM2);
			if (emElement==EMELEMENT_MAXNUM) emElement=EMELEMENT_SPIRIT;
		}

		bool VALIDTARG ( EMELEMENT emElement )					{  LIMIT(emElement); return strTARG[emElement].length()!=0; }

		const char* GETTARG ( EMELEMENT emElement )				{  LIMIT(emElement); return strTARG[emElement].c_str(); }
		const char* GETSELFZONE01 ( EMELEMENT emElement )		{  LIMIT(emElement); return strSELFZONE01[emElement].c_str(); }
		const char* GETSELFZONE02 ( EMELEMENT emElement )		{  LIMIT(emElement); return strSELFZONE02[emElement].c_str(); }
		const char* GETSELFZONE03 ( EMELEMENT emElement )		{  LIMIT(emElement); return strSELFZONE03[emElement].c_str(); }

		const char* GETTARGZONE01 ( EMELEMENT emElement )		{  LIMIT(emElement); return strTARGZONE01[emElement].c_str(); }
		const char* GETTARGZONE02 ( EMELEMENT emElement )		{  LIMIT(emElement); return strTARGZONE02[emElement].c_str(); }
		const char* GETTARGZONE03 ( EMELEMENT emElement )		{  LIMIT(emElement); return strTARGZONE03[emElement].c_str(); }		

		const char* GETSELFBODY ( EMELEMENT emElement )			{  LIMIT(emElement); return strSELFBODY[emElement].c_str(); }

		const char* GETTARGBODY01 ( EMELEMENT emElement )		{  LIMIT(emElement); return strTARGBODY01[emElement].c_str(); }
		const char* GETTARGBODY02 ( EMELEMENT emElement )		{  LIMIT(emElement); return strTARGBODY02[emElement].c_str(); }				

		SEXT_DATA_112 (void);

		void Assign ( const SEXT_DATA_100 &OldData );
		void Assign ( const SEXT_DATA_101 &OldData );
		void Assign ( const SEXT_DATA_102 &OldData );
		void Assign ( const SEXT_DATA_103 &OldData );
		void Assign ( const SEXT_DATA_104 &OldData );
		void Assign ( const SEXT_DATA_105 &OldData );
		void Assign ( const SEXT_DATA_106 &OldData );
		void Assign ( const SEXT_DATA_107 &OldData );
		void Assign ( const SEXT_DATA_108 &OldData );
		void Assign ( const SEXT_DATA_109 &OldData );
		void Assign ( const SEXT_DATA_110 &OldData );
		void Assign ( const SEXT_DATA_111 &OldData );

		BOOL SAVE ( sc::SerialFile &SFile );
		BOOL LOAD ( sc::BaseStream &SFile );

		static VOID SaveCsvHead ( std::fstream &SFile );
		VOID SaveCsv ( std::fstream &SFile );
		VOID LoadCsv ( CStringArray &StrArray );

		SEXT_DATA_112& operator= ( const SEXT_DATA_112 &value );
	};

	struct SEXT_DATA : public SEXT_DATA_112
	{
		// �̰����� ���� ���� ���� �� ��;
		// ���� ���� �� ������ ���� �ҷ����� �� ���� ����;
		SEXT_DATA(){;}		
	}; //struct SEXT_DATA
};
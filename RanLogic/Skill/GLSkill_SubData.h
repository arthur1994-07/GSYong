#pragma once

namespace SKILL
{
	struct SSPEC_100
	{
		float	fVAR1;
		float	fVAR2;
		DWORD	dwFLAG;

		SSPEC_100 ();
	};

	struct SSPEC_101
	{
		float		fVAR1;
		float		fVAR2;
		DWORD		dwFLAG;
		SNATIVEID	dwNativeID;

		SSPEC_101 ();
		void Assign( SSPEC_100 &sOldData );
	};

	struct SSPEC_102
	{
		union {
			float		fVAR1;
			int			nVAR1;
		};

		union {
			float		fVAR2;
			int			nVAR2;
		};

		float		fRate; // ���� ȿ�� ����. by luxes.
		float		fRate2; // ���� ȿ�� ����. by luxes.
		DWORD		dwFLAG;
		SNATIVEID	dwNativeID;

		SSPEC_102 ();
	};

	struct SSPEC
	{
		union {
			float		fVAR1;
			int			nVAR1;
		};

		union {
			float		fVAR2;
			int			nVAR2;
		};

		float		fRate; // ���� ȿ�� ����. by luxes.
		float		fRate2; // ���� ȿ�� ����. by luxes.
		DWORD		dwFLAG;
		SNATIVEID	dwNativeID;
		SNATIVEID	dwLinkID;

		SSPEC ();

		void Assign( SSPEC_100 &sOldData );
		void Assign( SSPEC_101 &sOldData );
		void Assign( SSPEC_102 &sOldData );
	};

	struct SIMPACT_ADDON_LEVEL
	{
		//	�ΰ� ȿ�� ����.
		EMIMPACT_ADDON		emADDON;					//	�ΰ�ȿ�� ����.
		float				fADDON_VAR;					//	��ȭ ��ġ.	
		float				fRate; // ���� ȿ�� ����. by luxes.

		SIMPACT_ADDON_LEVEL();
	};	

	struct SSPEC_ADDON_LEVEL
	{
		//	Ư����� ����.
		EMSPEC_ADDON		emSPEC;						//	Ư��.
		SKILL::SSPEC		sSPEC;						//	Ư�� ��� ������.

		SSPEC_ADDON_LEVEL();
		SSPEC_ADDON_LEVEL(const EMSPEC_ADDON eAddon );
		bool operator()(SSPEC_ADDON_LEVEL& key) const;
	};

	struct SKILLDELAY
	{
		float fDelayTime;
		float fMAXDelayTime;
		float fDefultMaxDelayTime;
		SKILLDELAY();
	};

	typedef std::map<DWORD, SKILLDELAY> DELAY_MAP;
	typedef DELAY_MAP::iterator	              DELAY_MAP_ITER;
};
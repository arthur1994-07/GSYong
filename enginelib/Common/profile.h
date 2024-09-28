// "Portions Copyright (C) Steve Rabin, 2000"
//	Edit by Jung DH, 2002.
//	modify by Jung DH, 2005.
//
#pragma once

#include <map>

struct SMSG_PROFILE
{
	int	  nMsgType;
	DWORD dwCount;
	DWORD dwSize;

	SMSG_PROFILE()
	{
		nMsgType = 0;
		dwCount = 0;
		dwSize = 0;
	}
};

typedef	std::map<DWORD,SMSG_PROFILE>	SMSG_PROFILE_MAP;
typedef	SMSG_PROFILE_MAP::iterator		SMSG_PROFILE_MAP_ITER;


class CPROFILE
{
public:
	enum { NUM_PROFILE_SAMPLES = 90, };

	struct PROFILESAMPLE
	{
		char			szName[256];			//Name of sample

		bool			bValid;					//Whether this data is valid
		unsigned int	iProfileInstances;		//# of times ProfileBegin called
		int				iOpenProfiles;			//# of times ProfileBegin w/o ProfileEnd
		unsigned long	fStartTime;				//The current open profile start time
		unsigned long	fAccumulator;			//All samples this frame added together
		unsigned long	fChildrenSampleTime;	//Time taken by all children
		unsigned int	iNumParents;			//Number of profile parents

		PROFILESAMPLE () 
			: bValid(false)
			, iProfileInstances(0)
			, iOpenProfiles(0)
			, fStartTime(0)
			, fAccumulator(0)
			, fChildrenSampleTime(0)
			, iNumParents(0)
		{
			memset( szName, 0, sizeof(char)*256 );
		}
	};

	struct PROFILESAMLEHISTORY
	{
		char	szName[256];				//Name of the sample

		bool	bValid;						//Whether the data is valid
		float	fAve;						//Average time per frame (percentage)
		float	fMin;						//Minimum time per frame (percentage)
		float	fMax;						//Maximum time per frame (percentage)

		PROFILESAMLEHISTORY () 
			: bValid(false)
			, fAve(0)
			, fMin(0)
			, fMax(0)
		{
			memset( szName, 0, sizeof(char)*256 );
		}
	};

protected:
	PROFILESAMPLE		m_samples[NUM_PROFILE_SAMPLES];
	PROFILESAMLEHISTORY	m_history[NUM_PROFILE_SAMPLES];

	SMSG_PROFILE_MAP	m_mapMsgProFile;
	
	unsigned long		m_startProfile;
	unsigned long		m_endProfile;

protected:
	void StoreInHistory ( char* name, float percent );
	void GetFromHistory ( char* name, float* ave, float* min, float* max );

public:
	CPROFILE ();
	~CPROFILE ();

public:
    void Init ();
	void Begin ( char* name );
	void End ( char* name );

	void BlockStart ();
	void BlockEnd ();
	void DumpOutputToView ( int _OUTCHANNEL );
	void DumpOutputToFile ();
	void DumpOutputToNon ();

	void MsgCount( int nType, DWORD dwSize );
	void MsgCount( LPVOID pBuffer );
};


class CPROFILE_EX
{
public:
	enum { NUM_PROFILE_NETWORK = 5000, NUM_PRINT_NETWORK_PROFILE = 600000 };

	// 해당 기능은 ThreadSafe 하지 않음 !!
	// 서버 Hang을 찾기 위해서 추가 따라서 기본 기능만 있음
	// 차후 필요시 기능 추가
	struct PROFILENETWORK
	{
		bool			bUse;	
		unsigned long	fCurrentStartTime;				
		unsigned long	fLastEndTime;				
		unsigned long	fCounter;				// 현재 사용 안함

		PROFILENETWORK () 
			: bUse(false)
			, fCurrentStartTime(0)
			, fLastEndTime(0)
			, fCounter(0)
		{
		}
	};

	// 해당 기능은 ThreadSafe 하지 않음 !!
	// 서버 Hang을 찾기 위해서 추가 따라서 기본 기능만 있음
	// 차후 필요시 기능 추가
	struct PROFILEFRAMEMOVE
	{
		bool			bUse;	
		unsigned long	fStartTime;				
		unsigned long	fEndTime;				

		PROFILEFRAMEMOVE () 
			: bUse(false)
			, fStartTime(0)
			, fEndTime(0)
		{
		}
	};

protected:
	PROFILENETWORK		m_network[NUM_PROFILE_NETWORK];
	PROFILEFRAMEMOVE	m_framemove;

public:
	CPROFILE_EX ();
	~CPROFILE_EX ();

public:
    void Init ();

	void BeginNetworkProfile(int nMsgType);
	void EndNetworkProfile(int nMsgType);
	void BeginFrameMoveProfile();
	void EndFrameMoveProfile();
	void TraceProfile();

private:
	void InitNetworkProfiler();
	void InitFrameMoveProfiler();

	void TraceNetworkHang();
	void TraceFrameMoveHang();
};

extern CPROFILE	g_profile_1;
extern CPROFILE	g_profile_2;
extern CPROFILE_EX	g_ExProfile;
extern BOOL		g_bProfile;
extern BOOL		g_bProfileEx;

//----------------------------------------------------------------------------------
#define PROFILE_ON 0
//----------------------------------------------------------------------------------
#if defined(PROFILE_ON)
	#define PROFILE_INIT()				g_profile_1.Init()
#else
	#define PROFILE_INIT()				((void)0)
#endif

#if defined(PROFILE_ON)
	#define PROFILE_BEGIN(a)			{ g_bProfile ? g_profile_1.Begin(#a) : (void)0; }
#else
	#define PROFILE_BEGIN(a)			((void)0)
#endif

#if defined(PROFILE_ON)
	#define PROFILE_END(a)				{ g_bProfile ? g_profile_1.End(#a) : (void)0; }
#else
	#define PROFILE_END(a)				((void)0)
#endif

#if defined(PROFILE_ON)
	#define PROFILE_DUMPOUTPUT()		{ g_bProfile ? g_profile_1.DumpOutputToView(2) : (void)0; }
#else
	#define PROFILE_DUMPOUTPUT()		((void)0)
#endif

#if defined(PROFILE_ON)
	#define PROFILE_DUMPOUTPUT_FILE()	{ g_bProfile ? g_profile_1.DumpOutputToFile() : (void)0; }
#else
	#define PROFILE_DUMPOUTPUT_FILE()	((void)0)
#endif

#if defined(PROFILE_ON)
	#define PROFILE_BLOCKSTART()		{ g_bProfile ? g_profile_1.BlockStart() : (void)0; }
#else
	#define PROFILE_BLOCKSTART()		((void)0)
#endif

#if defined(PROFILE_ON)
	#define PROFILE_BLOCKEND()			{ g_bProfile ? g_profile_1.BlockEnd() : (void)0; }
#else
	#define PROFILE_BLOCKEND()			((void)0)
#endif

#if defined(PROFILE_ON)
	//#define PROFILE_MSG_COUNT_RECV(a,b)			{ g_bProfile ? g_profile_1.MsgCount(a,b) : (void)0; }
    #define PROFILE_MSG_COUNT_RECV(a,b)		((void)0)
#else
	#define PROFILE_MSG_COUNT_RECV(a,b)		((void)0)
#endif

#if defined(PROFILE_ON)
	//#define PROFILE_MSG_COUNT_SEND(a)		{ g_bProfile ? g_profile_1.MsgCount(a) : (void)0; }
    #define PROFILE_MSG_COUNT_SEND(a)		((void)0)
#else
	#define PROFILE_MSG_COUNT_SEND(a)		((void)0)
#endif



//----------------------------------------------------------------------------------
#if defined(PROFILE_ON)
	#define PROFILE_INIT2()				g_profile_2.Init()
#else
	#define PROFILE_INIT2()				((void)0)
#endif

#if defined(PROFILE_ON)
	#define PROFILE_BEGIN2(a)			{ g_bProfile ? g_profile_2.Begin(#a) : (void)0; }
#else
	#define PROFILE_BEGIN2(a)			((void)0)
#endif

#if defined(PROFILE_ON)
	#define PROFILE_END2(a)				{ g_bProfile ? g_profile_2.End(#a) : (void)0; }
#else
	#define PROFILE_END2(a)				((void)0)
#endif

#if defined(PROFILE_ON)
	#define PROFILE_DUMPOUTPUT2()		{ g_bProfile ? g_profile_2.DumpOutputToView(3) : (void)0; }
#else
	#define PROFILE_DUMPOUTPUT2()		((void)0)
#endif

#if defined(PROFILE_ON)
	#define PROFILE_DUMPOUTPUT_FILE2()	{ g_bProfile ? g_profile_2.DumpOutputToFile() : (void)0; }
#else
	#define PROFILE_DUMPOUTPUT_FILE2()	((void)0)
#endif

#if defined(PROFILE_ON)
	#define PROFILE_BLOCKSTART2()		{ g_bProfile ? g_profile_2.BlockStart() : (void)0; }
#else
	#define PROFILE_BLOCKSTART2()		((void)0)
#endif

#if defined(PROFILE_ON)
	#define PROFILE_BLOCKEND2()			{ g_bProfile ? g_profile_2.BlockEnd() : (void)0; }
#else
	#define PROFILE_BLOCKEND2()			((void)0)
#endif


#if defined(PROFILE_ON)
	#define PROFILE_MSG_COUNT2_RECV(a,b)			{ g_bProfile ? g_profile_2.MsgCount(a,b) : (void)0; }
#else
	#define PROFILE_MSG_COUNT2_RECV(a,b)		((void)0)
#endif

#if defined(PROFILE_ON)
	#define PROFILE_MSG_COUNT2_SEND(a)		{ g_bProfile ? g_profile_1.MsgCount(a) : (void)0; }
#else
	#define PROFILE_MSG_COUNT2_SEND(a)		((void)0)
#endif



#define PROFILE_EX_INIT()				g_ExProfile.Init()
#define PROFILE_EX_BEGIN(a)				{ g_bProfileEx ? g_ExProfile.BeginNetworkProfile(a) : (void)0; }
#define PROFILE_EX_END(a)				{ g_bProfileEx ? g_ExProfile.EndNetworkProfile(a) : (void)0; }
#define PROFILE_EX_BEGIN()				{ g_bProfileEx ? g_ExProfile.BeginFrameMoveProfile() : (void)0; }
#define PROFILE_EX_END()				{ g_bProfileEx ? g_ExProfile.EndFrameMoveProfile() : (void)0; }
#define PROFILE_EX_TRACE()				{ g_bProfileEx ? g_ExProfile.TraceProfile() : (void)0; }
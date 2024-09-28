#ifndef DATABASE_TABLE_H_
#define DATABASE_TABLE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//
//struct FolderList
//{
//	int nIdx;
//	CString strName;
//};
//
//struct FileList
//{
//	int nIdx;
//	int nVer;
//    int nFtp;
//	CString strFile;
//	CString strDate;
//	CString strDesc;
//};
//
//struct CompressFile
//{
//	CString strSrc; // Full Path
//	CString strTarget; // Full TargetPath
//	CString strFileName; // File Name
//    CString strPurePath;
//	int		nDir;
//};
//
//struct FullFileList
//{
//    int nIdx;
//    CString strFolder;
//    CString strFileName;
//    int     nVersion;
//};
//
//struct RanFileList
//{ 
//    char szFileName[64];
//    char szSubPath[128];
//    int nVersion;
//};

struct UserInfo
{
	int UserNum;
	CString UserName;
	CString UserID;
	CString UserPass;
	CString UserPass2;
	CString CreateDate;
	CString LastLoginDate;
	int UserType;
	int UserLoginState;
	int UserAvailable;
	int SGNum;
	int SvrNum;
	CString ChaName;
};

#ifndef LONGLONG
#define LONGLONG __int64
#endif

struct CharInfo
{
	int ChaNum;
	int UserNum;
	CString ChaName;
	int ChaTribe;
	int ChaClass;

	int ChaFace;
	int ChaHair;
	int ChaSchool;
	int ChaLiving;

	int SGNum;

	int ChaLevel;	
	int ChaPower;
	int ChaStrong;
	int ChaStrength;
	int ChaSpirit;
	int ChaDex;
	int ChaIntel;
	int ChaStRemain;

	LONGLONG ChaMoney;
	LONGLONG ChaExp;

	int ChaHP;
	int ChaMP;

	int ChaStartMap;
	int ChaStartGate;
	float ChaPosX;
	float ChaPosY;
	float ChaPosZ;

	int GuNum;

	int ChaBright;
	int ChaAttackP;
	int ChaDefenseP;
	int ChaFightA;
	int ChaShootA;
	int ChaSP;
	int ChaPK;
	int ChaSkillPoint;

	int ChaOnline;
	int ChaDeleted;

	/*
	[ChaSkills] [image] NULL ,
	[ChaSkillSlot] [image] NULL ,
	[ChaPutOnItems] [image] NULL ,
	[ChaInven] [image] NULL ,
	*/
};

struct AllCharNum
{
	int ChaNum;
	int UserNum;
	CString ChaName;
};

struct viewServerInfo
{
	int nSGNum;
	int nSvrNum;
	int nSvrType;
	CString strSGName;
	CString strOdbcName;
	CString strOdbcUserID;
	CString strOdbcPassword;
	CString strName;
};

struct viewClubInfo
{
	int nGuNum;
	int nChaNum;	
	int nGuRank;   
	int nGuMarkVer;
	int nGuExpire;
	LONGLONG llGuMoney;
	LONGLONG llGuIncomeMoney;
	__time64_t GuMakeTime;
	__time64_t GuExpireTime;
	CString strChaName;
	CString strGuName;
	CString strGuNotice;
};

#endif // DATABASE_TABLE_H_
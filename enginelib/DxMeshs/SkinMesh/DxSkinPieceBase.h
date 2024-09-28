#pragma once

class DxSkinPieceBase
{
public:
	enum SKIN_PIECE_TYPE
	{
		SKIN_PIECE		= 0,
		SKIN_ATT_BONE_LINK	= 1,
	};
	SKIN_PIECE_TYPE m_emSkinPieceType;

public:
	virtual const TCHAR* GetFileName()	const = 0;
	virtual DWORD GetSlot()				const = 0;

public:
	virtual const TCHAR* GetBoneName()	const	{ return NULL; }
	virtual DWORD GetVehicleSlot()		const	{ return 0; }

public:
	DxSkinPieceBase(void);
	virtual ~DxSkinPieceBase(void);
};
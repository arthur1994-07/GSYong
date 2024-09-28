#pragma once

class DxBackupTarget
{
public:
    enum
    {
        TARGET  = 0x0001,
        DEPTH   = 0x0002,

        ALL     = TARGET | DEPTH
    };

public:
    LPDIRECT3DDEVICEQ m_pd3dDevice;
    WORD m_wFlags;
	LPDIRECT3DSURFACEQ m_pTarget;
	LPDIRECT3DSURFACEQ m_pDepth;

public:
	DxBackupTarget();
    ~DxBackupTarget();

public:
    bool Backup( LPDIRECT3DDEVICEQ pd3dDevice, WORD wFlags = ALL );
	void Restore();
};
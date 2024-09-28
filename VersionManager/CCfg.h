//! History
//! 2002.05.30 jgkim Create//
//! Copyright (c) MIN COMMUNICATIONS, INC.

#ifndef _VERSION_MAN_CONFIG_H_
#define _VERSION_MAN_CONFIG_H_

#pragma once

#include "IVersionManConfig.h"

class VersionManCfg : public IVersionManConfig
{	
public:
	VersionManCfg();
	VersionManCfg(const char* filename);
	virtual ~VersionManCfg();

public:	
    void		SetDefault(void);
    int			Process(char* sLine);
    virtual bool Load(const char* filename) override;
    CString		GetAppPath();
};

#endif // _VERSION_MAN_CONFIG_H_
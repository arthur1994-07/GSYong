#ifndef SC_LOG_DATABASE_H_
#define SC_LOG_DATABASE_H_

#pragma once

#include "./LogBase.h"

namespace sc
{
/**
 * \ingroup sc
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 * MFC\n
 *
 * \version 1.0
 * first version
 *
 * \date 2007.03.14
 *
 * \author Jgkim
 *
 * \history
 * 2007.03.14 Jgkim Create
  *
 * \par license
 * Copyright (c) Min Communications. All rights reserved.
 * 
 * \todo 
 *
 * \bug 
 *
 */
class LogDatabase : public LogBase
{
public:
	LogDatabase(void);
	virtual ~LogDatabase(void);
};

}; // namespace sc

#endif //
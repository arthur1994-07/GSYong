/* xmalloc.c -- malloc with out of memory checking
   Copyright (C) 1990, 91, 92, 93, 94, 95, 96, 99 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "stdafx.h"

#pragma warning( disable : 4996 )
#pragma warning( disable : 4005 )
#pragma warning( disable : 4244 )

//#if HAVE_CONFIG_H
# include "config.h"
//#endif

// TODO better use #define in header file?!
#include <string.h>
#include <stdlib.h>
void *xmalloc (size_t n) {return malloc(n);}
void *xcalloc (size_t n, size_t s) {return calloc(n,s);}
void *xrealloc (void *p, size_t n) {return realloc(p,n);}
char *xstrdup (const char *p) {return strdup(p);}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning( default : 4244 )
#pragma warning( disable : 4005 )
#pragma warning( default : 4996 )

/***********************************************************************
        Copyright (c) 2003 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

/*
  intUtils.c

  Ron S.
  2 May 1996

  int conversion functions.

  */

#include "rvstdio.h"
#include "intutils.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
  Desc: Check if data is a number.
  */
RvBool
isNumber(IN RvChar *data) /* null terminated string. */
{
  RvChar *p = data;

  if (!data) return RV_FALSE;

  while (*p)
  {
    if (!isdigit((RvInt)*p)) return RV_FALSE;
    p++;
  }
  return RV_TRUE;
}



#ifdef __cplusplus
}
#endif


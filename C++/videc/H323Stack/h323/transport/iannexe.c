/***********************************************************************
        Copyright (c) 2002 RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Ltd.. No part of this document may be reproduced in any
form whatsoever without written prior approval by RADVISION Ltd..

RADVISION Ltd. reserve the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
***********************************************************************/

#include "rvinternal.h"
#include "annexe.h"
#include "iannexe.h"

#ifdef __cplusplus
extern "C" {
#endif


/*lint -save -e416 -e415  Likely creation of out-of-bounds pointer */


#ifdef RV_DEBUG
    /* safe typecast functions (only in DEBUG mode) */
    tAnnexE*        AsAnnexE( HANNEXE hannexe ) { return (tAnnexE*)hannexe; }
    HANNEXE         AsHANNEXE( tAnnexE* pannexe ) { return (HANNEXE)pannexe; }
#endif

void
hton16( RvUint16 h, RvUint8* ptr ) {
    ptr[1] = (RvUint8)(h & 0xff);
    h >>= 8;
    ptr[0] = (RvUint8)(h & 0xff);
}

void
hton24( RvUint32 h, RvUint8* ptr ) {
    ptr[2] = (RvUint8)(h & 0xff);
    h >>= 8;
    ptr[1] = (RvUint8)(h & 0xff);
    h >>= 8;
    ptr[0] = (RvUint8)(h & 0xff);
}

void
hton32( RvUint32 h, RvUint8* ptr ) {
    ptr[3] = (RvUint8)(h & 0xff);
    h >>= 8;
    ptr[2] = (RvUint8)(h & 0xff);
    h >>= 8;
    ptr[1] = (RvUint8)(h & 0xff);
    h >>= 8;
    ptr[0] = (RvUint8)(h & 0xff);
}

RvUint16
ntoh16( RvUint8* ptr ) {
    RvUint16 res = *ptr++;
    res <<= 8;
    res |= *ptr;
    return res;
}

RvUint32
ntoh24( RvUint8* ptr ) {
    RvUint32 res = *ptr++;
    res = (res << 8) + *ptr++;
    return (res << 8) + *ptr;
}

RvUint32
ntoh32( RvUint8* ptr ) {
    RvUint32 res = *ptr++;
    res = (res << 8) + *ptr++;
    res = (res << 8) + *ptr++;
    return (res << 8) + *ptr;
}


/*lint -restore */

#ifdef __cplusplus
}
#endif /* __cplusplus*/

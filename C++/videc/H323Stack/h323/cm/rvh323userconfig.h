/***********************************************************************
Filename   : rvh323usrconfig.h
Description: let the user to put his own choices
************************************************************************
        Copyright (c) 2001 RADVISION Inc. and RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Inc. and RADVISION Ltd.. No part of this document may be
reproduced in any form whatsoever without written prior approval by
RADVISION Inc. or RADVISION Ltd..

RADVISION Inc. and RADVISION Ltd. reserve the right to revise this
publication and make changes without obligation to notify any person of
such revisions or changes.
***********************************************************************/

#ifndef RVH323_USRCONFIG_H
#define RVH323_USRCONFIG_H

#undef RV_H245_USE_HIGH_AVAILABILITY
#define RV_H245_USE_HIGH_AVAILABILITY   RV_YES

#undef RV_H245_LEAN_H223
#define RV_H245_LEAN_H223               RV_NO

#undef RV_ASN1_LEAN_3G324M
#define RV_ASN1_LEAN_3G324M             RV_NO

#undef RV_ASN1_CODER_USE_Q931
#define RV_ASN1_CODER_USE_Q931			RV_YES


#undef RV_ASN1_CODER_USE_H450
#define RV_ASN1_CODER_USE_H450			RV_YES

/* definitions for supporting H.225 or H.223 */
#define RV_H245_SUPPORT_H225_PARAMS RV_YES
#define RV_H245_SUPPORT_H223_PARAMS RV_NO

/* H.245 definitions for capability set and for capability descriptors arrays */
#define RV_H245_CAP_SET_SIZE 100
#define RV_H245_CAP_DESC_SIZE 100

#endif /* RVH323_USRCONFIG_H */

/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.10.00.23 07-02-03 (ddk-b12)" */
/********************************************************************/
/* _saa7115.c file                                                      */
/********************************************************************/
#include "_tvp5150.h"
 
/* Default values as sugested at TVP5150AM1 datasheet */
const struct i2c_reg_value tvp5150_init_default[] = {
         { /* 0x00 */
                 TVP5150_VD_IN_SRC_SEL_1,0x00
         },
         { /* 0x01 */
                 TVP5150_ANAL_CHL_CTL,0x15
         },
         { /* 0x02 */
                 TVP5150_OP_MODE_CTL,0x00
         },
         { /* 0x03 */
                 TVP5150_MISC_CTL,0x01
         },
         { /* 0x06 */
                 TVP5150_COLOR_KIL_THSH_CTL,0x10
         },
         { /* 0x07 */
                 TVP5150_LUMA_PROC_CTL_1,0x60
         },
         { /* 0x08 */
                 TVP5150_LUMA_PROC_CTL_2,0x00
         },
         { /* 0x09 */
                 TVP5150_BRIGHT_CTL,0x80
         },
         { /* 0x0a */
                 TVP5150_SATURATION_CTL,0x80
         },
         { /* 0x0b */
                 TVP5150_HUE_CTL,0x00
         },
         { /* 0x0c */
                 TVP5150_CONTRAST_CTL,0x80
         },
         { /* 0x0d */
                 TVP5150_DATA_RATE_SEL,0x47
         },
         { /* 0x0e */
                 TVP5150_LUMA_PROC_CTL_3,0x00
         },
         { /* 0x0f */
                 TVP5150_CONF_SHARED_PIN,0x08
         },
         { /* 0x11 */
                 TVP5150_ACT_VD_CROP_ST_MSB,0x00
         },
         { /* 0x12 */
                 TVP5150_ACT_VD_CROP_ST_LSB,0x00
         },
         { /* 0x13 */
                 TVP5150_ACT_VD_CROP_STP_MSB,0x00
         },
         { /* 0x14 */
                 TVP5150_ACT_VD_CROP_STP_LSB,0x00
         },
         { /* 0x15 */
                 TVP5150_GENLOCK,0x01
         },
         { /* 0x16 */
                 TVP5150_HORIZ_SYNC_START,0x80
         },
         { /* 0x18 */
                 TVP5150_VERT_BLANKING_START,0x00
         },
         { /* 0x19 */
                 TVP5150_VERT_BLANKING_STOP,0x00
         },
         { /* 0x1a */
                 TVP5150_CHROMA_PROC_CTL_1,0x0c
         },
         { /* 0x1b */
                 TVP5150_CHROMA_PROC_CTL_2,0x14
         },
         { /* 0x1c */
                 TVP5150_INT_RESET_REG_B,0x00
         },
         { /* 0x1d */
                 TVP5150_INT_ENABLE_REG_B,0x00
         },
         { /* 0x1e */
                 TVP5150_INTT_CONFIG_REG_B,0x00
         },
         { /* 0x28 */
                 TVP5150_VIDEO_STD,0x00
         },
         { /* 0x2e */
                 TVP5150_MACROVISION_ON_CTR,0x0f
         },
         { /* 0x2f */
                 TVP5150_MACROVISION_OFF_CTR,0x01
         },
         { /* 0xbb */
                 TVP5150_TELETEXT_FIL_ENA,0x00
         },
         { /* 0xc0 */
                 TVP5150_INT_STATUS_REG_A,0x00
         },
         { /* 0xc1 */
                 TVP5150_INT_ENABLE_REG_A,0x00
         },
         { /* 0xc2 */
                 TVP5150_INT_CONF,0x04
         },
         { /* 0xc8 */
                 TVP5150_FIFO_INT_THRESHOLD,0x80
         },
         { /* 0xc9 */
                 TVP5150_FIFO_RESET,0x00
         },
         { /* 0xca */
                 TVP5150_LINE_NUMBER_INT,0x00
         },
         { /* 0xcb */
                 TVP5150_PIX_ALIGN_REG_LOW,0x4e
         },
         { /* 0xcc */
                 TVP5150_PIX_ALIGN_REG_HIGH,0x00
         },
         { /* 0xcd */
                 TVP5150_FIFO_OUT_CTRL,0x01
         },
         { /* 0xcf */
                 TVP5150_FULL_FIELD_ENA,0x00
         },
         { /* 0xd0 */
                 TVP5150_LINE_MODE_INI,0x00
         },
         { /* 0xfc */
                 TVP5150_FULL_FIELD_MODE_REG,0x7f
         },
         { /* end of data */
                 0xff,0xff
         }
 };

const struct i2c_reg_value tvp5150_init_enable[] = {
         { /* 0x02 */
                 TVP5150_OP_MODE_CTL,0x40 //0x00
         },
        {     /* Activate YCrCb output 0x9 or 0xd ? */
                TVP5150_MISC_CTL, 0xEF//0x6f
        },
		 { /* 0x07 */
                 TVP5150_LUMA_PROC_CTL_1, 0x20 //x60
         },
        {     /* Default format: 0x47. For 4:2:2: 0x40 */
                TVP5150_DATA_RATE_SEL, 0x07//0x47
        },
        {
                TVP5150_CONF_SHARED_PIN, 0
        },
         { /* 0x12 */
                 TVP5150_ACT_VD_CROP_ST_LSB,0x04 //0x00
         },
        {     /* Activates video std autodetection for all standards */
                TVP5150_AUTOSW_MSK, 0x00
        },

        {
                TVP5150_CHROMA_PROC_CTL_1, 0x0C
        },
        {
                TVP5150_CHROMA_PROC_CTL_2, /*0x14*/ 0x54
        },
        {     /* Non documented, but initialized on WinTV USB2 */
                0x27, 0x20
        },
         { /* 0x2e */
                 TVP5150_MACROVISION_ON_CTR,0x15//0x1f
         },
        { /* !!0xc1 */
                 TVP5150_INT_ENABLE_REG_A,0x40
        },
		{ /* !!0xc2 */
                 TVP5150_INT_CONF,0x07
        },
        {
                0xff,0xff
        }
};

/********************************************************************/
/* end of file                                                      */
/********************************************************************/


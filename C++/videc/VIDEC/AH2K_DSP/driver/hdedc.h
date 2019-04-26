
/*
define EDC command
*/
#include <edc.h>

#define EDC_EXT_INIT		(EDC_USER + 0x01)
#define EDC_EXT_GETINPUT	(EDC_USER + 0x02)
#define EDC_EXT_GETRES		(EDC_USER + 0x03)
#define EDC_EXT_GETSTATUS	(EDC_USER + 0x04)
#define EDC_EXT_CONFIG		(EDC_USER + 0x05)
#define EDC_EXT_RESET		(EDC_USER + 0x06)
#define EDC_EXT_SETBRIGHT   (EDC_USER + 0x07)

#define VPORT_CMD_EDC(edc)	(VPORT_CMD_EDC_BASE + EDC_EXT_##edc)

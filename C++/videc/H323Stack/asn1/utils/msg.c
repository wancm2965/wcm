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

#include "rvtypes.h"
#include <stdlib.h>
#include "rvcbase.h"
#include "rvloglistener.h"
#include "rvthread.h"
#include "rvstdio.h"
#include "rvtm.h"
#include "rvclock.h"
#include "strutils.h"
#include "h32xLog.h"
#include "msg.h"


#if (RV_LOGMASK != RV_LOGLEVEL_NONE)

#ifdef __cplusplus
extern "C" {
#endif


#define CONF_FILE  "rvtele.ini"
#define LOG_FILE   "rvtsp.log"

static void (*stackNotify)(IN char *line,...) = NULL;

static char msConfFileName[128] = CONF_FILE; /* configuration file name */
static char msLogFileName[128]  = "";  /* log file name */

static int msgDebugLevel = -1;
static int fileSize = -1;
static int numOfFiles = -1;
static int flushFile = -1;

static RvLogListener fileListener;
static RvBool logfiled = RV_FALSE;

static RvLogListener termListener;
static RvBool logtermed = RV_FALSE;

static RvLogListener debListener;
static RvBool logdebed = RV_FALSE;

/* Source used to indicate changes in logging filters */
static RvLogSource loggerSource;

/* Indication if the application has a listener set */
static RvBool applicationListenerOn = RV_FALSE;

static RvUint32 msCalls = 0;


/************************** Assisting Functions **************************/

static int parseSupservLine(
    IN  char line[],
    OUT char ** param,
    OUT char ** value
)
{
    int cToInd=0, cFromInd=0;

    *param = *value = NULL;

    while(line[cFromInd] != '\0')
    {
        if((isspace((int) line[cFromInd])) || (line[cFromInd] == '='))
        {
            cFromInd++;
            continue;
        }
        if(*param == NULL)
        {
            /* found the param. we start copying the param to the beginning of the line,
            ending with '\0' */
            *param = &line[cToInd];

            while((line[cFromInd] != '\0') && !(isspace((int)line[cFromInd])) && (line[cFromInd] != '='))
            {
                line[cToInd] = (char) tolower((int)line[cFromInd]);
                cToInd++;
                cFromInd++;
            }
            if(line[cFromInd] != '\0') cFromInd++;
            line[cToInd] = '\0';
            cToInd++;
        }
        else if(*value == NULL)
        {
            /* found the value. we start copying the param to the beginning of the line,
            ending with '\0' */
            *value = &line[cToInd];

            while((line[cFromInd] != '\0') && !(isspace((int)line[cFromInd])))
            {
                line[cToInd] = line[cFromInd];
                cToInd++;
                cFromInd++;
            }
            line[cToInd] = '\0';
            break;
        }
    }
    if((*param == NULL) || (*value == NULL))
    {
        return RV_ERROR_UNKNOWN;
    }
    return RV_OK;
}


static char * lowerString(char * string)
{
    int ind;
    for(ind=0; string[ind] != '\0'; ind++)
        string[ind] = (char)tolower((int) string[ind]);
    return string;
}


static int parseInsertLine(
    IN  const RvChar line[],
    OUT char logName[128],
    OUT RvLogMessageType * logMask
)
{
    int cFromInd=0;
    RvBool noFilters = RV_TRUE;

    /* if user stated only the name, log everything */
    *logMask = (RvLogMessageType) 0;
    logName[0] = '\0';

    while(line[cFromInd] != '\0')
    {
        if((isspace((int) line[cFromInd])) || (line[cFromInd] == '='))
        {
            cFromInd++;
            continue;
        }
        if(logName[0] == '\0')
        {
            int cToInd=0;
            /* found the log name. we start copying the param to the logName, ending with '\0' */
            while((line[cFromInd] != '\0') && !(isspace((int)line[cFromInd])) &&
                  (line[cFromInd] != '=') && (cToInd < 127))
            {
                logName[cToInd] = line[cFromInd];
                cToInd++;
                cFromInd++;
            }
            logName[cToInd] = '\0';
        }
        else
        {
            noFilters = RV_FALSE;

            /* we add each log filter to the mask */
            if(line[cFromInd] == '0')
            {
                *logMask = (RvLogMessageType) 0;
                return RV_OK;
            }
            if ((isdigit((RvInt)line[cFromInd]) && (line[cFromInd] != '0')) ||
                (line[cFromInd] == '*'))
            {
                *logMask = RV_LOGLEVEL_ALL;
                return RV_OK;
            }
            if(!strncmp_ci(&line[cFromInd], "EXCEP", 5))
            {
                *logMask |= RV_LOGLEVEL_EXCEP;
                cFromInd += 5;
                continue;
            }
            if(!strncmp_ci(&line[cFromInd], "ERROR", 5))
            {
                *logMask |= RV_LOGLEVEL_ERROR;
                cFromInd += 5;
                continue;
            }
            if(!strncmp_ci(&line[cFromInd], "WARN", 4))
            {
                *logMask |= RV_LOGLEVEL_WARNING;
                cFromInd += 4;
                continue;
            }
            if(!strncmp_ci(&line[cFromInd], "INFO", 4))
            {
                *logMask |= RV_LOGLEVEL_INFO;
                cFromInd += 4;
                continue;
            }
            if(!strncmp_ci(&line[cFromInd], "DEBUG", 5))
            {
                *logMask |= RV_LOGLEVEL_DEBUG;
                cFromInd += 5;
                continue;
            }
            if(!strncmp_ci(&line[cFromInd], "ENTER", 5))
            {
                *logMask |= RV_LOGLEVEL_ENTER;
                cFromInd += 5;
                continue;
            }
            if(!strncmp_ci(&line[cFromInd], "LEAVE", 5))
            {
                *logMask |= RV_LOGLEVEL_LEAVE;
                cFromInd += 5;
                continue;
            }
            if(!strncmp_ci(&line[cFromInd], "SYNC", 4))
            {
                *logMask |= RV_LOGLEVEL_SYNC;
                cFromInd += 4;
                continue;
            }
            /* didn't match anything - skip this word */
            while((line[cFromInd] != '\0') && !(isspace((int) line[cFromInd])))
                cFromInd++;
        }
    }
    if(logName[0] == '\0')
        return RV_ERROR_UNKNOWN;

    if(noFilters)
        *logMask = RV_LOGLEVEL_ALL;

    return RV_OK;
}


/************************** Read File Functions **************************/

RvBool rvteleSepserv(FILE** fdp, char line[]);
RvBool rvteleInsertIntoLog(FILE** fdp, char line[]);


/* Open log configuration parse it and start the listeners */
RVAPI void RVCALLCONV msOpen(void)
{
    FILE *fd = NULL;
    char line[200];

    msCalls++;

    if (msCalls <= 1)
        /* initialization */
    {
        RvCBaseInit();
        RvLogSetGlobalMask(H32xLogGet(), 0);
        RvLogSourceConstruct(H32xLogGet(), &loggerSource, "LOGINFO", "Log filtering information");
        RvLogSourceSetMask(&loggerSource, RV_LOGLEVEL_ALL);
    }

    if (msConfFileName[0])
    {
        fd = RvFopen(msConfFileName, "r");
#if (RV_OS_TYPE == RV_OS_TYPE_WIN32)
        if(fd == NULL)
        {
            char winPath[64];
            char fullPath[128];

            /* Windows only code - we can use _snprintf() directly */
            GetWindowsDirectory(winPath, 64);
            _snprintf(fullPath, sizeof(fullPath), "%s\\%s", winPath, msConfFileName);
            fd = RvFopen(fullPath, "r");
        }
#endif
    }

    if(fd != NULL)
    {
        RvSize_t len;

        while (RvFgets(line, sizeof(line), fd))
        {
            len = strlen(line);
            if (len < 2) continue; /* ignore empty lines */

            if (line[0] == '#') continue; /* ignore remarks */

            if (line[0] == '[')
            {
                RvBool retry;

                line[len-1] = 0; /* remove \n */

                do
                {
                    retry = RV_FALSE;

                    if(!strcmp(line, "[supserve]"))
                    {
                        retry = rvteleSepserv(&fd, line);
                    }

                    if(!strcmp(line, "[insertIntoLog]"))
                    {
                        retry = rvteleInsertIntoLog(&fd, line);
                    }

                    if(!strcmp(line, "[insertIntoFile]"))
                    {
                        retry = rvteleInsertIntoLog(&fd, line);
                    }
                } while(retry);
            }
        }
        (void)RvFclose(fd);
    }
}


/* Close log listeners */
RVAPI void RVCALLCONV msClose(void)
{
    msCalls--;
    if (msCalls > 0)
        return;

    if (logfiled)
    {
        RvLogListenerDestructLogfile(&fileListener);
        logfiled = RV_FALSE;
    }
    if (logtermed)
    {
        RvLogListenerDestructTerminal(&termListener);
        logtermed = RV_FALSE;
    }
    if (logdebed)
    {
        RvLogListenerDestructDebug(&debListener);
        logdebed = RV_FALSE;
    }

    RvLogSourceDestruct(&loggerSource);

    applicationListenerOn = RV_FALSE;

    RvCBaseEnd();
}


RvBool rvteleSepserv(FILE** fdp, char line[])
{
    char *param, *value;
    RvBool useFile=RV_FALSE, useTerm=RV_FALSE, useDeb=RV_FALSE;
    RvBool newHeader = RV_FALSE;
    RvSize_t len;

    while(RvFgets(line, 200, *fdp))
    {
        len = strlen(line);
        if (len < 2) continue; /* ignore empty lines */

        if (line[0] == '#') continue; /* ignore remarks */

        line[len-1] = 0; /* remove \n */

        if(line[0] == '[')
        {
            newHeader = RV_TRUE;
            break; /* exit on new header */
        }

        if(parseSupservLine(line, &param, &value) != RV_OK)
            continue;

        if(!strcmp(param, "deblevel"))
        {
            /* set debug level */
            if(msgDebugLevel == -1)
                msgDebugLevel = atoi(value);
            continue;
        }

        if(!strcmp(param, "msgterm"))
        {
            if((!strcmp(lowerString(value), "on")) || (atoi(value) > 0))
            {
                /* create window listener */
                useTerm = RV_TRUE;
                continue;
            }
        }

        if(!strcmp(param, "msgfile"))
        {
            if((!strcmp(lowerString(value), "on")) || (atoi(value) > 0))
            {
                /* create file listener */
                useFile = RV_TRUE;
                continue;
            }
        }

        if(!strcmp(param, "msgdeb"))
        {
            if((!strcmp(lowerString(value), "on")) || (atoi(value) > 0))
            {
                /* create debugger listener */
                useDeb = RV_TRUE;
                continue;
            }
        }

        if(!strcmp(param, "flushfile"))
        {
            /* set buffer size */
            if(flushFile == -1)
                flushFile = atoi(value);
            continue;
        }

        if(!strcmp(param, "filesize") || !strcmp(param, "buffersize"))
        {
            /* set buffer size */
            if(fileSize == -1)
                fileSize = atoi(value);
            continue;
        }

        if(!strcmp(param, "numoffiles"))
        {
            /* set number of files */
            if(numOfFiles == -1)
                numOfFiles = atoi(value);
            continue;
        }

        if(!strcmp(param, "filename"))
        {
            /* set file name */
            if(msLogFileName[0] == '\0')
                strncpy(msLogFileName, value, sizeof(msLogFileName));
            continue;
        }
    }

    /* set defaults if they were not set */
    if(msLogFileName[0] == '\0')
        strcpy(msLogFileName, LOG_FILE);
    if(numOfFiles == -1)
        numOfFiles = 1;
    if(fileSize == -1)
        fileSize = 0;
    if(flushFile == -1)
        flushFile = 0;
    if(msgDebugLevel == -1)
        msgDebugLevel = 0;

    if(useFile && !logfiled)
        if(RvLogListenerConstructLogfile(&fileListener, H32xLogGet(), msLogFileName, numOfFiles, fileSize, flushFile != 0) == RV_OK)
            logfiled = RV_TRUE;

    if(useTerm && !logtermed)
        if(RvLogListenerConstructTerminal(&termListener, H32xLogGet(), 0) == RV_OK)
            logtermed = RV_TRUE;

    if(useDeb && !logdebed)
        if(RvLogListenerConstructDebug(&debListener, H32xLogGet()) == RV_OK)
            logdebed = RV_TRUE;

    msSetDebugLevel(msgDebugLevel);

    return newHeader;
}


RvBool rvteleInsertIntoLog(FILE** fdp, char line[])
{
    char logName[128];
    RvLogMessageType logMask;
    RvSize_t len;

    while(RvFgets(line, 200, *fdp))
    {
        len = strlen(line);
        if (len < 2) continue; /* ignore empty lines */

        if(line[0] == '#') continue; /* ignore remarks */

        line[len-1] = 0; /* remove \n */

        if (line[0] == '[')
        {
            return RV_TRUE; /* exit on new header */
        }

        if(parseInsertLine(line, logName, &logMask) != RV_OK)
            continue;

        RvLogSourceSetMask(&loggerSource, RV_LOGLEVEL_ALL);
        RvLogInfo(&loggerSource, (&loggerSource, "%s", line));

        if(logName[0] == '*')
        {
            RvLogSetGlobalMask(H32xLogGet(), logMask);
        }
        else
        {
            RvLogSource logSource;
            RvLogSourceConstruct(H32xLogGet(), &logSource, logName, "");
            RvLogSourceSetMask(&logSource, logMask);
        }
    }

    return RV_FALSE;
}



/****************************** internal listener *****************************/


/********************************************************************************************
 * rvMsgPrintListener
 *
 * purpose : Callback that is executed whenever a message has to be logged somewhere.
 *           This is used to get log information to the application's callbacks.
 * input   : logRecord  - Information related with the logged message
 *           userData   - User related data, given when the printing function was set
 * output  : None
 * return  : None
 * remarks : The log record holds a pointer to the message to log. This message has
 *           a set amount of bytes before it allocated but unused to allow better
 *           formatting of messages. That amount is RV_LOG_RESERVED_BYTES.
 ********************************************************************************************/
static void RVCALLCONV rvMsgPrintListener(
    IN RvLogRecord* logRecord,
    IN void*        userData)
{
    RvChar* buf;
    RvChar* threadName;
    RvTime t;
    RvTm tm;
    const RvChar* mtypeStr;
    RvUint32 len;
    void (*notify)(IN char *line,...) = stackNotify;

    RV_UNUSED_ARG(userData);

    if (notify == NULL)
        return;

    /* Find the message type string */
    switch (RvLogRecordGetMessageType(logRecord))
    {
        case RV_LOGID_EXCEP:   mtypeStr = "EXCEP"; break;
        case RV_LOGID_ERROR:   mtypeStr = "ERROR"; break;
        case RV_LOGID_WARNING: mtypeStr = "WARN "; break;
        case RV_LOGID_INFO :   mtypeStr = "INFO "; break;
        case RV_LOGID_DEBUG:   mtypeStr = "DEBUG"; break;
        case RV_LOGID_ENTER:   mtypeStr = "ENTER"; break;
        case RV_LOGID_LEAVE:   mtypeStr = "LEAVE"; break;
        case RV_LOGID_SYNC:    mtypeStr = "SYNC "; break;
        default:
            return;
    }

    /* Move back in the text - there's free space there for our formatting */
    buf = (RvChar *)RvLogRecordGetText(logRecord) - 38;

    /* Find out the thread's name */
    threadName = RvLogRecordGetThread(logRecord);

    /* Get the time of the log */
    RvClockGet(NULL, &t);
    RvTmConstructUtc(&t, NULL, &tm);

    /* Format the additional information into the reserved space in the beginning of the buffer */
    /* 8+1+2+1+2+1+2+1+10+2+5+3 = */
    RvSprintf(buf, "%8.8s %2.2u:%2.2u:%2.2u %-10.10s: %s -",
        threadName,
        RvTmGetHour(&tm), RvTmGetMin(&tm), RvTmGetSec(&tm),
        RvLogSourceGetName(RvLogRecordGetSource(logRecord)),
        mtypeStr);

    /* We're done with time information */
    RvTimeDestruct(&t);
    RvTmDestruct(&tm);

    /* This one is for the addition of the actual string after the formatted header */
    buf[37] = ' ';

    /* Add a newline in the end and calculate the size of message */
    len = strlen(buf);
    buf[len] = '\0';

    /* Now that we finally have a formatted buffer we can work with - notify the application */
    notify("%s", buf);
}


/****************************** ms functions *****************************/

/* Add module to debug printing list */
RVAPI int RVCALLCONV msAdd(IN const RvChar *moduleName)
{
    char logName[128];
    RvLogMessageType logMask;
    RvLogSource tmpSource;
    RvStatus status;

    status = parseInsertLine(moduleName, logName, &logMask);
    if (status == RV_OK)
    {
        RvLogSourceSetMask(&loggerSource, RV_LOGLEVEL_ALL);
        RvLogInfo(&loggerSource, (&loggerSource, "%s", moduleName));

        if(logName[0] == '*')
        {
            status = RvLogSetGlobalMask(H32xLogGet(), logMask);
        }
        else
        {
            /* See if we the source we requested already exists */
            status = RvLogGetSourceByName(H32xLogGet(), logName, &tmpSource);
            if (status != RV_OK)
            {
                /* It doesn't - we create it. This might cause us a "resource leak" of log sources,
                   but we're better off this way than crashing on too many calls to msDelete(). */
                status = RvLogSourceConstruct(H32xLogGet(), &tmpSource, logName, "");
            }

            if (status == RV_OK)
                status = RvLogSourceSetMask(&tmpSource, logMask);
        }
    }

    return status;
}

/* Delete module from debug printing list */
RVAPI int RVCALLCONV msDelete(IN const RvChar *moduleName)
{
    RvLogSource tmpSource;
    RvStatus status;

    status = RvLogGetSourceByName(H32xLogGet(), moduleName, &tmpSource);
    if (status == RV_OK)
    {
        RvLogSourceSetMask(&loggerSource, RV_LOGLEVEL_ALL);
        RvLogInfo(&loggerSource, (&loggerSource, "Removing trace on %s", moduleName));

        status = RvLogSourceSetMask(&tmpSource, RV_LOGLEVEL_NONE);
    }
    return status;
}

/* Delete all modules from debug printing list */
RVAPI int RVCALLCONV msDeleteAll(void)
{
    RvLogSourceSetMask(&loggerSource, RV_LOGLEVEL_ALL);
    RvLogInfo(&loggerSource, (&loggerSource, "Removing all traces"));
    return RvLogSetGlobalMask(H32xLogGet(), RV_LOGLEVEL_NONE);
}

/* Set the configuration file name. */
RVAPI void RVCALLCONV msFile(IN const RvChar *name)
{
    strncpy(msConfFileName, name, sizeof(msConfFileName));
    msConfFileName[sizeof(msConfFileName)-1] = '\0';
}

/* Get the debug level */
RVAPI int RVCALLCONV msGetDebugLevel(void)
{
    return msgDebugLevel;
}

/* Set the log output file name.  */
RVAPI void RVCALLCONV msLogFile(IN const RvChar *name)
{
    strncpy(msLogFileName, name, sizeof(msLogFileName));
    msLogFileName[sizeof(msLogFileName)-1] = '\0';
}

/* Get the current log output file name.  */
RVAPI RvStatus RVCALLCONV msGetLogFilename(IN RvUint32 nameLength, OUT RvChar *name)
{
    if (logfiled == RV_FALSE)
        return RV_ERROR_NOTSUPPORTED; /* No file... */

    return RvLogListenerLogfileGetCurrentFilename(&fileListener, nameLength, name);
}

/* Set the debug level */
RVAPI int RVCALLCONV msSetDebugLevel(IN int newDebugLevel)
{
    /* Make sure we silence the log for debug level 0 only */
    if (newDebugLevel == 0)
    {
        RvLogInfo(&loggerSource, (&loggerSource, "Debug level: %d", newDebugLevel));
        RvLogSetLevel(H32xLogGet(), 0);
    }
    else
    {
        if (newDebugLevel >= 3)
            RvLogSetLevel(H32xLogGet(), 2);
        else
            RvLogSetLevel(H32xLogGet(), 1);
        RvLogInfo(&loggerSource, (&loggerSource, "Debug level: %d", newDebugLevel));
    }

    return(msgDebugLevel = newDebugLevel);
}

/* Set function to be called when logging messages are being written */
RVAPI void RVCALLCONV msSetStackNotify(void (*sN)(IN char *line,...))
{
    if ((applicationListenerOn == RV_FALSE) && (sN != NULL))
    {
        /* We have to set a dedicated listener for the application on the log */
        RvLogRegisterListener(H32xLogGet(), rvMsgPrintListener, NULL);
    }
    else if (applicationListenerOn == RV_TRUE)
    {
        /* We have to remove a dedicated listener */
        if (stackNotify != NULL)
            RvLogUnregisterListener(H32xLogGet(), rvMsgPrintListener);
    }

    /* Set the listener function */
    applicationListenerOn = (sN != NULL);
    stackNotify = sN;
}

/* Add sink */
RVAPI int RVCALLCONV msSinkAdd(IN const RvChar *sinkName)
{
    char firstName[64], secondName[64];
    int numOfNames; /* number of separated strings in sinkName */

    numOfNames=RvSscanf(sinkName, "%64s %64s", firstName, secondName);

    if(!strcmp(firstName, "file"))
    {
        /* see if we got two names */
        if (numOfNames>1)
            msLogFile(secondName); /* register new logfile name */

        /* if we already have a log file, close before opening a new one */
        if (logfiled)
        {
            RvLogListenerDestructLogfile(&fileListener); /* close current logfile */
            logfiled = RV_FALSE;
        }

        /* Open the new file */
        if(RvLogListenerConstructLogfile(&fileListener, H32xLogGet(), msLogFileName, numOfFiles, fileSize, flushFile != 0) == RV_OK)
            logfiled = RV_TRUE;
    }

    /* if we don't have a terminal output, open one */
    if(!strcmp(firstName, "terminal") && !logtermed)
    {
        if(RvLogListenerConstructTerminal(&termListener, H32xLogGet(), 0) == RV_OK)
            logtermed = RV_TRUE;
    }

    /* if we don't have a debugger output, open one */
    if(!strcmp(firstName, "logger") && !logdebed)
    {
        if(RvLogListenerConstructDebug(&debListener, H32xLogGet()) == RV_OK)
            logdebed = RV_TRUE;
    }

    return RV_TRUE;
}

/* Delete sink */
RVAPI int RVCALLCONV msSinkDelete(IN const RvChar *sinkName)
{
    if(!strcmp(sinkName, "file") && logfiled)
    {
        RvLogListenerDestructLogfile(&fileListener);
        logfiled = RV_FALSE;
    }
    if(!strcmp(sinkName, "terminal") && logtermed)
    {
        RvLogListenerDestructTerminal(&termListener);
        logtermed = RV_FALSE;
    }
    if(!strcmp(sinkName, "logger") && logdebed)
    {
        RvLogListenerDestructDebug(&debListener);
        logdebed = RV_FALSE;
    }
    return RV_OK;
}



#else  /* (RV_LOGMASK) */

int RV_dummyMsg = 0;

#endif  /* (RV_LOGMASK) */

#ifdef __cplusplus
}
#endif

//VIDECFREHeader.h
#ifndef __VIDECFREAPI_HEADER_H__
#define __VIDECFREAPI_HEADER_H__

//#include "nedmalloc.h"

#ifdef VIDECFRE_EXPORT
#define VIDECFRE_API _declspec(dllexport)
#elif VIDECFRE_DLL
#define VIDECFRE_API _declspec(dllimport)
#else
#define VIDECFRE_API
#endif

//log level  1--99 core infomation
//log level  1 core error 
//log level  2 core exception return
//log level  99  core other
//log level  100--999  module information
//log level  100  thread
//log level  101  soft encoder
//log level  102  soft decoder
//log level  103  hardware encoder
//log level  104  hardware decoder
//log level  105  directshow display
//log level  106  d3d display
//log level  107  capture
//log level  108  call statck 
//log level  999  module other..

#define   logcoreerror  1  
#define   logcoreexcep 2  
#define   logcoreother  99  
#define   logthread  100 
#define   logsoftenc  101 
#define   logsoftdec   102 
#define   loghardenc  103  
#define   logharddec   104  
#define   logdsw   105  
#define   logd3d    106  
#define   logcapture  107  
#define   logcallstack  108  
#define   logmodother  999 


//DebugOutput(logcoreerror ,"new pointer:%p,func:%s,Line:%d\r\n",   obj , __FUNCTION__,__LINE__ );

#define  NEWRECORD( obj , statement ) \
	obj = NULL ; \
	try \
	{ \
	obj = statement ; \
	} \
	catch (...) \
	{ \
	DebugOutput(logcoreerror ,"new exception, file:%s,func:%s,Line:%d\r\n",   __FILE__,__FUNCTION__,__LINE__ ); \
	obj = NULL; \
	}




#define  NEWRECORD3( objtype , obj , statement ) \
	objtype obj = NULL ; \
	try \
	{ \
	obj = statement ; \
	} \
	catch (...) \
	{ \
	DebugOutput(logcoreerror ,"new exception, file:%s,func:%s,Line:%d\r\n",   __FILE__,__FUNCTION__,__LINE__ ); \
	obj = NULL; \
	}



#define  NEWRECORDret3( objtype , obj , statement ) \
	objtype obj = NULL ; \
	try \
	{ \
	obj = statement ; \
	} \
	catch (...) \
	{ \
	DebugOutput(logcoreerror ,"new exception, file:%s,func:%s,Line:%d\r\n",   __FILE__,__FUNCTION__,__LINE__ ); \
	obj = NULL; \
	} \
	return obj;






#define  NULLReturnRet( obj , ret ) \
	if (obj == NULL) \
	{ \
	DebugOutput( logcoreerror ,"Error. New Failed. FUNC:%s,Line:%d \n" , ( __FUNCTION__ ) , ( __LINE__  )  ); \
	return ret; \
}

#define  NULLReturn( obj ) \
	if (obj == NULL) \
	{ \
	DebugOutput( logcoreerror ,"Error. New Failed. FUNC:%s,Line:%d \n" , ( __FUNCTION__ ) , ( __LINE__  )  ); \
	return ; \
}


#define  NULLRecord( obj ) \
	if (obj == NULL) \
	{ \
	DebugOutput( logcoreerror ,"Error. New Failed. FUNC:%s,Line:%d \n" , ( __FUNCTION__ ) , ( __LINE__  )  ); \
}





//#define  MALLOCMSG(size) \
//	FreMalloc( ( malloc(size) ), ( size ) , ( __FILE__ ) , ( __FUNCTION__ ) , ( __LINE__  ) )  
//
//
//
//VIDECFRE_API void * FreMalloc(void * p,size_t size,char * fileName,char * func,int nLine);



#define  MALLOCMSG(size) \
	FreMalloc(  ( size ) , ( __FILE__ ) , ( __FUNCTION__ ) , ( __LINE__  ) )  

#define  FREEMSG( p ) \
	FreFree(  ( p ) , ( __FILE__ ) , ( __FUNCTION__ ) , ( __LINE__  ) ) ; \
	p = NULL 


VIDECFRE_API void * FreMalloc( size_t size,char * fileName,char * func,int nLine);
VIDECFRE_API void  FreFree(void * p,char * fileName,char * func,int nLine);

VIDECFRE_API void CreateVPool(int nSize,int nThreads);
VIDECFRE_API void DestroyVPool(void);

VIDECFRE_API void CreateIPCPool(void);


#define  CALLOCMSG(no , size ) \
	FreCalloc(  ( no ) , ( size ) , ( __FILE__ ) , ( __FUNCTION__ ) , ( __LINE__  ) )  

#define  REALLOCMSG( po , size ) \
	FreRealloc(  ( po ) , ( size ) , ( __FILE__ ) , ( __FUNCTION__ ) , ( __LINE__  ) )  

VIDECFRE_API void * FreCalloc(  size_t no, size_t size,char * fileName,char * func,int nLine);
VIDECFRE_API void * FreRealloc( void * po, size_t size, char * fileName,char * func,int nLine);



VIDECFRE_API void  DebugOutput(int nLevel, char* msg);

VIDECFRE_API void DebugOutput(char* msg,char * header =NULL);

VIDECFRE_API void DebugOutput(const char* fmt,...);

VIDECFRE_API void DebugOutput(int nLevel,const char* fmt,...);







#include "nedmalloc.h"

#define  NEWMALLOC( obj , objType,statement ) \
	obj = NULL ; \
	try \
{ \
	obj = nedalloc::New <objType> statement ; \
} \
	catch (...) \
{ \
	DebugOutput(logcoreerror ,"new malloc exception, file:%s,func:%s,Line:%d\r\n",   __FILE__,__FUNCTION__,__LINE__ ); \
	obj = NULL; \
}


#define  NEWMALLOC3( objtype , obj , otype , statement ) \
	objtype obj = NULL ; \
	try \
{ \
	obj = nedalloc::New <otype> statement ; \
} \
	catch (...) \
{ \
	DebugOutput(logcoreerror ,"new exception, file:%s,func:%s,Line:%d\r\n",   __FILE__,__FUNCTION__,__LINE__ ); \
	obj = NULL; \
}



#define  NEWMALLOCret3( objtype , obj , otype , statement  ) \
	objtype obj = NULL ; \
	try \
{ \
	obj = nedalloc::New <otype> statement ; \
} \
	catch (...) \
{ \
	DebugOutput(logcoreerror ,"new exception, file:%s,func:%s,Line:%d\r\n",   __FILE__,__FUNCTION__,__LINE__ ); \
	obj = NULL; \
} \
	return obj;


#define  DELETEFREE( obj ) \
	try \
{ \
	if(obj)  nedalloc::Delete (obj ) ; \
} \
	catch (...) \
{ \
	DebugOutput(logcoreerror ,"Delete free exception, file:%s,func:%s,Line:%d\r\n",   __FILE__,__FUNCTION__,__LINE__ ); \
	obj = NULL; \
} \
	obj = NULL ;



#endif
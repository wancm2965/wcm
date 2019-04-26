#pragma once


#ifdef PLCORE_EXPORTS
#define PluginCore_API __declspec(dllexport)
#else
#define PluginCore_API __declspec(dllimport)	
#endif


#if !defined(PluginCore_API)
#define PluginCore_API
#endif


#ifndef PLCORE_EXPORTS
#ifdef _DEBUG
#pragma comment(lib,"PLCore.lib")
#else
#pragma comment(lib,"PLCore.lib")
#endif
#endif

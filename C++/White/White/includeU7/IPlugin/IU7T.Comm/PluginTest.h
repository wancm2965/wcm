#ifndef _INTERFACE_PLUGIN_INNER_TEST_H_
#define _INTERFACE_PLUGIN_INNER_TEST_H_

///测试插件间以来的开关(需要的话撤销此行注释)
//#define _PLUGIN_INNER_TEST_

#ifdef _PLUGIN_INNER_TEST_
#define PLUGIN_PATH_DEPEND		("D:\\depend.ini")		//文件保存插件的 依赖关系
//#define PLUGIN_PATH_DEPENDED	("D:\\depended.ini")		//文件保存插件的 被依赖关系
#endif

#endif
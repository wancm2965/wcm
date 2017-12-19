//
//  pugiwriter.h
//  TestNetec6.3
//
//  Created by Eric on 13-3-15.
//  Copyright (c) 2013年 Eric. All rights reserved.
//

#ifndef __TestNetec6_3__pugiwriter__
#define __TestNetec6_3__pugiwriter__

#include "KBASE/KBASE_Export.h"
#include "pugixml.hpp"


KBASE_API std::string node_to_string(pugi::xml_node node);

KBASE_API char* node_to_buffer(pugi::xml_node node, char* buffer, size_t size);

KBASE_API char* node_to_buffer_heap(pugi::xml_node node);

#endif /* defined(__TestNetec6_3__pugiwriter__) */

/*
 *      Copyright (C) 2010-2012 Hendrik Leppkes
 *      http://www.1f0.de
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// Based on the SampleParser Template by GDCL
// --------------------------------------------------------------------------------
// Copyright (c) GDCL 2004. All Rights Reserved. 
// You are free to re-use this as the basis for your own filter development,
// provided you retain this copyright notice in the source.
// http://www.gdcl.co.uk
// --------------------------------------------------------------------------------

#include "stdafx.h"

// Initialize the GUIDs
#include <InitGuid.h>

#include <qnetwork.h>
#include "LAVSplitter.h"
#include "moreuuids.h"

#include "IGraphRebuildDelegate.h"


// --- COM factory table and registration code --------------

const AMOVIESETUP_MEDIATYPE 
  sudMediaTypes[] = {
    { &MEDIATYPE_Stream, &MEDIASUBTYPE_NULL },
};

const AMOVIESETUP_PIN sudOutputPins[] = 
{
  {
    L"Output",            // pin name
      FALSE,              // is rendered?    
      TRUE,               // is output?
      FALSE,              // zero instances allowed?
      TRUE,               // many instances allowed?
      &CLSID_NULL,        // connects to filter (for bridge pins)
      NULL,               // connects to pin (for bridge pins)
      0,                  // count of registered media types
      NULL                // list of registered media types
  },
  {
    L"Input",             // pin name
      FALSE,              // is rendered?    
      FALSE,              // is output?
      FALSE,              // zero instances allowed?
      FALSE,              // many instances allowed?
      &CLSID_NULL,        // connects to filter (for bridge pins)
      NULL,               // connects to pin (for bridge pins)
      1,                  // count of registered media types
      &sudMediaTypes[0]   // list of registered media types
  }
};

const AMOVIESETUP_FILTER sudFilterReg =
{
  &__uuidof(CLAVSplitter),        // filter clsid
  L"LAV Splitter",                // filter name
  MERIT_PREFERRED + 4,            // merit
  2,                              // count of registered pins
  sudOutputPins,                  // list of pins to register
  CLSID_LegacyAmFilterCategory
};

const AMOVIESETUP_FILTER sudFilterRegSource =
{
  &__uuidof(CLAVSplitterSource),  // filter clsid
  L"LAV Splitter Source",         // filter name
  MERIT_PREFERRED + 4,            // merit
  1,                              // count of registered pins
  sudOutputPins,                  // list of pins to register
  CLSID_LegacyAmFilterCategory
};

// --- COM factory table and registration code --------------

// DirectShow base class COM factory requires this table, 
// declaring all the COM objects in this DLL
CFactoryTemplate g_Templates[] = {
  // one entry for each CoCreate-able object
  {
    sudFilterReg.strName,
      sudFilterReg.clsID,
      CreateInstance<CLAVSplitter>,
      NULL,
      &sudFilterReg
  },
  {
    sudFilterRegSource.strName,
      sudFilterRegSource.clsID,
      CreateInstance<CLAVSplitterSource>,
      NULL,
      &sudFilterRegSource
  },
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);


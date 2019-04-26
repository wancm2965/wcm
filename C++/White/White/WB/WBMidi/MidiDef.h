#pragma once
#include <GdiPlus.h>
#pragma comment(lib,"gdiplus.lib")
using namespace Gdiplus;
#include "MidiNote.h"

class CMidiNote;
 enum Accid
 {
 	None,
 	Sharp,
 	Flat,
 	Natural
 };
 enum Clef
 {
 	Treble,
 	Bass
 };
 enum SymbolType
 {
 	ACCIDSYMBOL,
 	BARSYMBOL,
 	BLANKSYMBOL,
 	CHORDSYMBOL,
 	CLEFSYMBOL,
 	RESTSYMBOL
 };
 enum NoteDuration
 {
 	ThirtySecond,
 	Sixteenth,
 	Triplet,
 	Eighth,
 	DottedEighth,
 	Quarter,
 	DottedQuarter,
 	Half,
 	DottedHalf,
 	Whole
 };
 enum StemDir
 {
 	Up,
 	Down
 };

  bool CompareNote(CMidiNote*x,CMidiNote*y);

  
  bool SortNotes( CMidiNote*x,CMidiNote*y );
 
  
  Bitmap* DrawtrableClefImage();
  
  Bitmap* DrawbassClefImage();
 
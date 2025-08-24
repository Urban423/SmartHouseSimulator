#pragma once
#include "FileManager.h"

struct offsetSubTable { 
	uint32 scalerType;
	uint16 numTables;
	uint16 searchRange;
	uint16 entrySelector;
	uint16 rangeShift;
};

struct tableDirectory {
	uint32 checkSum;
	uint32 offset;
	uint32 length;
};

struct headTable {
	fixed 			version;
	fixed 			fontRevision;
	uint32 			checkSumAdjustment;
	uint32 			magicNumber;
	uint16 			flags;
	uint16 			unitsPerEm;
	longDateTime	created;
	longDateTime	modified;
	fword			xMin;
	fword			yMin;
	fword			xMax;
	fword			yMax;
	uint16			macStyle;
	uint16			lowestRecPPEM;
	uint16			fontDirectionHint;
	uint16			indexToLocFormat;
	uint16			glyphDataFormat;
};

struct cmapHead {
	uint16 version;
	uint16 numberSubtables;
};

struct cmapSubtable {
	uint16 platformID;
	uint16 platformSpecificID;
	uint32 offset;
};

struct cmapFormat4 {
	uint16 length;
	uint16 language;
	uint16 segCount;
	uint16 searhRange;
	uint16 entrySelector;
	uint16 rangeShift;
	uint16* endCode;
	uint16 reservedPad;
	uint16* startCode;
	uint16* idDelta;
	uint16* idRangeOffset;
	uint16* glyfIndexArray;
};

struct cmapTable {
	cmapHead head;
	cmapSubtable* subTables;
	uint16 format;
	cmapFormat4 format4;
};

struct maxpTable {
	fixed version;
	uint16 numGlyphs;
	uint16 maxPoints;
	uint16 maxContours;
	uint16 maxComponentPoints;
	uint16 maxZones;
	uint16 twillightZone;
	uint16 maxStorage;
	uint16 maxFunctionDefs;
	uint16 maxInstructionDefs;
	uint16 maxStackElements;
	uint16 maxSizeOfInstructions;
	uint16 maxComponentElements;
	uint16 maxComponentDepth;
};

struct locaTable {
	uint32* offsets;
	uint32  size;
};







constexpr uint32 FourBytesToInverseInt32(const char* string) {
	return string[0] | (string[1] << 8) | (string[2] << 16) | (string[3] << 24);
}

constexpr uint32 tags[] = {
	FourBytesToInverseInt32("cmap"),
	FourBytesToInverseInt32("glyf"),
	FourBytesToInverseInt32("head"),
	FourBytesToInverseInt32("hhea"),
	FourBytesToInverseInt32("hmtx"),
	FourBytesToInverseInt32("loca"),
	FourBytesToInverseInt32("maxp"),
	FourBytesToInverseInt32("name"),
	FourBytesToInverseInt32("post"),
};

constexpr uint32 getIndex(uint32 tag) {
	switch(tag) {
		case(tags[0]): {return 0;}
		case(tags[1]): {return 1;}
		case(tags[2]): {return 2;}
		case(tags[3]): {return 3;}
		case(tags[4]): {return 4;}
		case(tags[5]): {return 5;}
		case(tags[6]): {return 6;}
		case(tags[7]): {return 7;}
		case(tags[8]): {return 8;}
	};
	return -1;
}
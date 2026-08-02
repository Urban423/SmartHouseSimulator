#pragma once
#include "stdlibInclude.h"
#include "Rect.h"
#include "Image.h"

using fixed = int32_t;
using longDateTime = int64_t;
using fword = int16_t;

struct offsetSubTable { 
	uint32_t scalerType;
	uint16_t numTables;
	uint16_t searchRange;
	uint16_t entrySelector;
	uint16_t rangeShift;
};

struct tableDirectory {
	uint32_t tag;
	uint32_t checkSum;
	uint32_t offset;
	uint32_t length;
};

struct headTable {
	fixed 			version;
	fixed 			fontRevision;
	uint32_t 		checkSumAdjustment;
	uint32_t 		magicNumber;
	uint16_t 		flags;
	uint16_t 		unitsPerEm;
	longDateTime	created;
	longDateTime	modified;
	fword			xMin;
	fword			yMin;
	fword			xMax;
	fword			yMax;
	uint16_t		macStyle;
	uint16_t		lowestRecPPEM;
	uint16_t		fontDirectionHint;
	uint16_t		indexToLocFormat;
	uint16_t		glyphDataFormat;
};

struct cmapSubtable {
	uint16_t platformID;
	uint16_t encodingID;
	uint32_t offset;
};

struct cmapTable {
    uint16_t version;
    uint16_t numTables;
    cmapSubtable* records;
};

struct cmapFormat4 {
    uint16_t 	format;
    uint16_t 	length;
    uint16_t 	language;
	uint16_t 	segCount;
    uint16_t 	searchRange;
    uint16_t 	entrySelector;
    uint16_t 	rangeShift;
    uint16_t* 	endCode;
    uint16_t 	reservedPad;
    uint16_t* 	startCode;
    int16_t* 	idDelta;
    uint16_t* 	idRangeOffset;
    uint16_t* 	glyphIdArray;
};

struct maxpTable {
	fixed version;
	uint16_t numGlyphs;
	uint16_t maxPoints;
	uint16_t maxContours;
	uint16_t maxComponentPoints;
	uint16_t maxZones;
	uint16_t twillightZone;
	uint16_t maxStorage;
	uint16_t maxFunctionDefs;
	uint16_t maxInstructionDefs;
	uint16_t maxStackElements;
	uint16_t maxSizeOfInstructions;
	uint16_t maxComponentElements;
	uint16_t maxComponentDepth;
};

struct locaTable {
	uint32_t  size;
	uint32_t* offsets;
};

struct Edge {
    Vector2 a;
    Vector2 b;
    Vector2 control;
    bool bezier;
    char rgbState;
};

struct Glyph {
    int16_t xMin;
    int16_t yMin;
    int16_t xMax;
    int16_t yMax;

	Rect uv;
    std::vector<uint16_t> contourEnds;
    std::vector<Vector2> points;
    std::vector<bool> onCurve;

	inline Rect getUV() { return uv; }
	inline int16_t width() { return xMax - xMin; }
	inline int16_t height() { return yMax - yMin; }
};

class TTFAtlas {
public:
	int  getCharacterIndex(int character);
	inline Glyph& getGlyph(int glyphIndex) { return glyphs[glyphIndex]; }
	inline size_t getGlyphsSize() { return maxp.numGlyphs; }
	inline float getCharacterWidth(char character) {
		int characterIndex = getCharacterIndex(character);
		if(characterIndex == -1) return 0;
		return glyphs[characterIndex].width();
	}
	inline Rect getBoundingBox() { return { (float)head.xMin, (float)head.yMin, (float)head.xMax, (float)head.yMax }; }
	 int toTexture();
private:
	char drawGlyph(Layer& layer, Glyph& glyph, float scale, float padding);
public:
	headTable head;
	maxpTable maxp;
	cmapTable cmap;
	cmapFormat4 format;
	std::vector<Glyph> glyphs;
};
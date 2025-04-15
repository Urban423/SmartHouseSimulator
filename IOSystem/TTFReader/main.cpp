#include <stdio.h>
#include "FileManager.h"
#include "TTFStructures.h"


int readOffsetTable(CFile* file, offsetSubTable& oTable) {
	oTable.scalerType 		= inverseRead32(file);
	oTable.numTables 		= inverseRead16(file);
	oTable.searchRange 		= inverseRead16(file);
	oTable.entrySelector 	= inverseRead16(file);
	oTable.rangeShift 		= inverseRead16(file);
	
	//printf("offsetSubTable\n\tscalerType = %d\n\tnumTables = %d\n\tsearchRange = %d\n\tentrySelector = %d\n\trangeShift = %d\n", oTable.scalerType, oTable.numTables, oTable.searchRange, oTable.entrySelector,  oTable.rangeShift);
	if(!(oTable.scalerType == 0x00010000 || oTable.scalerType == 0x74727565)) { 
		return -1;
	}
	return 0;
}







int readTableDirectory(CFile* file, tableDirectory* dTables) {
	uint32 tag;
	readFile(&tag, sizeof(uint32), file);
	//printf("%c%c%c%c\n", (tag >> 0) & 0xFF, (tag >> 8) & 0xFF,(tag >> 16) & 0xFF,(tag >> 24) & 0xFF);
	tag = getIndex(tag);
	if(tag == -1) { seekFile(file, sizeof(uint32) * 3, SEEK_CUR); return -1;}
	readFile(&dTables[tag].checkSum, sizeof(uint32), file);
	dTables[tag].offset 		= inverseRead32(file);
	dTables[tag].length 		= inverseRead32(file);
	//printf("TableDirectory\n\ttag = %d\n\tcheckSum = %d\n\toffset = %d\n\tlength = %d\n", tag, dTables[tag].checkSum, dTables[tag].offset, dTables[tag].length);
	return 0;
}





char readHeadTable(CFile* file, void* data) {
	headTable hTable;
	readFile(&hTable.version, sizeof(fixed), file);
	if(hTable.version != 0x00000100) { return -1; }
	hTable.fontRevision 		= inverseRead32(file);
	hTable.checkSumAdjustment 	= inverseRead32(file);
	readFile(&hTable.magicNumber, sizeof(fixed), file);
	if(hTable.magicNumber != 0xF53C0F5F) {return -1;}
	hTable.flags = inverseRead16(file);
	hTable.unitsPerEm = inverseRead16(file);
	readFile(&hTable.created , sizeof(longDateTime), file);
	readFile(&hTable.modified, sizeof(longDateTime), file);
	hTable.xMin = inverseRead16(file);
	hTable.yMin = inverseRead16(file);
	hTable.xMax = inverseRead16(file);
	hTable.yMax = inverseRead16(file);
	hTable.macStyle 			= inverseRead16(file);
	hTable.lowestRecPPEM 		= inverseRead16(file);
	hTable.fontDirectionHint 	= inverseRead16(file);
	hTable.indexToLocFormat		= inverseRead16(file);
	hTable.glyphDataFormat		= inverseRead16(file);
	printf("HeadTable:\n\tversion = 1\n\tfontRevision = %d\n\tcheckSumAdjustment = %d\n\tmagicNumber = 0xF53C0F5F\n\tflags = %d\n\txMin = %d\n\tyMin = %d\n\txMax = %d\n\tyMax = %d\n\tmacStyle = %d\n\tlowestRecPPEM = %d\n\tfontDirectionHint = %d\n\tindexToLocFormat = %d\n\tglyphDataFormat = %d\n", hTable.fontRevision, hTable.checkSumAdjustment, hTable.flags, hTable.xMin, hTable.yMin, hTable.xMax, hTable.yMax, hTable.macStyle, hTable.lowestRecPPEM, hTable.fontDirectionHint, hTable.indexToLocFormat, hTable.glyphDataFormat);
	memcpy(data, &hTable, sizeof(headTable));
	return 0;
}

cmapFormat4 readFormat4(CFile* file) {
	cmapFormat4 format;
	format.length = inverseRead16(file);
	format.language = inverseRead16(file);
	format.segCount = inverseRead16(file) >> 1;
	format.searhRange = inverseRead16(file);
	format.entrySelector = inverseRead16(file);
	format.rangeShift = inverseRead16(file);
	format.endCode = new uint16[format.segCount];
	for(int i = 0; i < format.segCount; ++i) {
		format.endCode[i] = inverseRead16(file);
	}
	format.reservedPad = inverseRead16(file);
	format.startCode = new uint16[format.segCount];
	for(int i = 0; i < format.segCount; ++i) {
		format.startCode[i] = inverseRead16(file);
	}
	format.idDelta = new uint16[format.segCount];
	for(int i = 0; i < format.segCount; ++i) {
		format.idDelta[i] = inverseRead16(file);
	}
	int adressArraySize = (format.length - format.segCount * 8 - 16) >> 1;
	format.idRangeOffset = new uint16[format.segCount + adressArraySize];
	for(int i = 0; i < format.segCount + adressArraySize; ++i) {
		format.idRangeOffset[i] = inverseRead16(file);
	}
	return format;
}

void readCmapTable(CFile* file, void* data) {
	cmapTable cmap;
	char* ptr = file->getPtr();
	readFile(&cmap.head.version, sizeof(uint16), file);
	if(cmap.head.version != 0) { return; }
	cmap.head.numberSubtables = inverseRead16(file);
	cmap.subTables = new cmapSubtable[cmap.head.numberSubtables];
	int index = 0;
	for(int i = 0; i < cmap.head.numberSubtables; ++i) {
		cmap.subTables[i].platformID 			= inverseRead16(file);
		cmap.subTables[i].platformSpecificID 	= inverseRead16(file);
		cmap.subTables[i].offset 				= inverseRead32(file);
		if(cmap.subTables[i].platformID == 3 && cmap.subTables[i].platformSpecificID == 1) {
			index = i;
		}
	}
	seekFile(file, cmap.subTables[index].offset - (file->getPtr() - ptr), SEEK_CUR);
	
	cmap.format  = inverseRead16(file);
	cmap.format4 = readFormat4(file);
	//printf("cmap Table:\n\tversion = %d\n\tnumberSubtables = %d\n", cmap.head.version, cmap.head.numberSubtables);for(int i = 0; i < cmap.head.numberSubtables; ++i) {printf("\tcmap Subtable:\n\t\tplatformID = %d\n\t\tplatformSpecificID = %d\n\t\toffset = %d\n", cmap.subTables[i].platformID, cmap.subTables[i].platformSpecificID, cmap.subTables[i].offset);}if(cmap.format == 4) {printf("\tFormat4\n\t\tlength = %d\n\t\tlanguage = %d\n\t\tsegCount = %d\n\t\tsearhRange = %d\n\t\tentrySelector = %d\n\t\trangeShift = %d\n", cmap.format4.length, cmap.format4.language, cmap.format4.segCount, cmap.format4.searhRange, cmap.format4.entrySelector, cmap.format4.rangeShift);}	
	memcpy(data, &cmap, sizeof(cmapTable));
}

int searchCharacterIndex(int character, cmapTable& cmap) {
	if(cmap.format == 4) {
		int index = 0;
		for(; index < cmap.format4.segCount; ++index) { 
			if(cmap.format4.startCode[index] <= character && character <= cmap.format4.endCode[index]) {
				break;
			}
		}
		if(index == cmap.format4.segCount) { return -1;}
		int glyphIndex;
		if(cmap.format4.idRangeOffset[index] == 0) {
			glyphIndex = (character + cmap.format4.idDelta[index]) & 0xFFFF;
		}
		else {
			glyphIndex =  *(&cmap.format4.idRangeOffset[index] + (cmap.format4.idRangeOffset[index] >> 1) + (character - cmap.format4.startCode[index]));
		}
		
		return glyphIndex;
	}
	return -1;
}


void readMaxpTable(CFile* file, void* data) {
	maxpTable maxp;
	readFile(&maxp.version, sizeof(fixed), file);
	maxp.numGlyphs = inverseRead16(file);
	maxp.maxPoints = inverseRead16(file);
	maxp.maxContours = inverseRead16(file);
	maxp.maxComponentPoints = inverseRead16(file);
	maxp.maxZones = inverseRead16(file);
	maxp.twillightZone = inverseRead16(file);
	maxp.maxStorage = inverseRead16(file);
	maxp.maxFunctionDefs = inverseRead16(file);
	maxp.maxInstructionDefs = inverseRead16(file);
	maxp.maxStackElements = inverseRead16(file);
	maxp.maxSizeOfInstructions = inverseRead16(file);
	maxp.maxComponentElements = inverseRead16(file);
	maxp.maxComponentDepth = inverseRead16(file);
	//printf("maxpTable:\n\tversion = %d\n\tnumGlyphs = %d\n\tmaxPoints = %d\n\t", maxp.version, maxp.numGlyphs, maxp.maxPoints, maxp.version, maxp.version, maxp.version, maxp.version, maxp.version, maxp.version, maxp.version, maxp.version, maxp.version, maxp.version, maxp.version);
	memcpy(data, &maxp, sizeof(maxpTable));
}

void readLocaTable(CFile* file, void* data, bool indexToLocFormat, uint32 size) {
	locaTable loca;
	loca.size = size;
	loca.offsets = new uint32[size];
	if(indexToLocFormat) {
		for(uint32 i = 0; i < size; ++i) {
			loca.offsets[i] = inverseRead32(file);
		}
	}
	else {
		for(uint32 i = 0; i < size; ++i) {
			loca.offsets[i] = (uint32)inverseRead16(file);
		}

	}
	memcpy(data, &loca, sizeof(locaTable));
}



int* drawCharacter(int character, const char* filepath) {
	offsetSubTable oTable;
	tableDirectory dTables[sizeof(tags) / sizeof(uint32)];
	headTable head;
	cmapTable cmap;
	maxpTable maxp;
	locaTable loca;
	
	CFile* f = openFile(filepath);
	if(readOffsetTable(f, oTable)) return nullptr;
	for(int i = 0; i < oTable.numTables; ++i) {
		if(readTableDirectory(f, dTables)) {continue;}
	}
	
	
	CFile copy = *f;
	seekFile(&copy, dTables[getIndex(FourBytesToInverseInt32("head"))].offset, SEEK_SET);
	readHeadTable(&copy, &head);
	
	
	
	seekFile(&copy, dTables[getIndex(FourBytesToInverseInt32("cmap"))].offset, SEEK_SET);
	readCmapTable(&copy, &cmap);
	
	
	seekFile(&copy, dTables[getIndex(FourBytesToInverseInt32("maxp"))].offset, SEEK_SET);
	readMaxpTable(&copy, &maxp);
	
	seekFile(&copy, dTables[getIndex(FourBytesToInverseInt32("loca"))].offset, SEEK_SET);
	readLocaTable(&copy, &loca, head.indexToLocFormat, maxp.numGlyphs);
	
	
	int index  = searchCharacterIndex(character, cmap);
	int offset = loca.offsets[index];
	
	seekFile(&copy, dTables[getIndex(FourBytesToInverseInt32("glyf"))].offset, SEEK_SET);
	
	
	printf("character: %d\nindex: %d\noffset: %d\n", character, index, offset);
	
	closeFile(f);
	return nullptr;
}


int main() {
	drawCharacter(193, "MyriadWeb.ttf");
	return 0;
}
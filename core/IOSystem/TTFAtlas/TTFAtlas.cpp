#include "TTFAtlas.h"
#include "../IOSystem.h"


offsetSubTable readOffsetTable(CFile& file) {
	return {
		file.read<uint32_t>(),
		file.read<uint16_t>(),
		file.read<uint16_t>(),
		file.read<uint16_t>(),
		file.read<uint16_t>()
	};
}

constexpr uint32_t MakeTag(const char* text) {
    return (uint32_t(text[0]) << 24) | (uint32_t(text[1]) << 16) | (uint32_t(text[2]) << 8) | uint32_t(text[3]);
}

size_t findTableIndex(const std::vector<tableDirectory>& tables, uint32_t tag) {
    for (size_t i = 0; i < tables.size(); i++) {
        if (tables[i].tag == tag) return i;
    }
    return SIZE_MAX;
}

tableDirectory readTableDirectory(CFile& file) {
	return {
		file.read<uint32_t>(),
		file.read<uint32_t>(),
		file.read<uint32_t>(),
		file.read<uint32_t>()
	};
}

headTable readHeadTable(CFile& file) {
    return {
        file.read<fixed>(),
        file.read<fixed>(),
        file.read<uint32_t>(),
        file.read<uint32_t>(),
        file.read<uint16_t>(),
        file.read<uint16_t>(),
        file.read<longDateTime>(),
        file.read<longDateTime>(),
        file.read<fword>(),
        file.read<fword>(),
        file.read<fword>(),
        file.read<fword>(),
        file.read<uint16_t>(),
        file.read<uint16_t>(),
        file.read<uint16_t>(),
        file.read<uint16_t>(),
        file.read<uint16_t>()
    };
}

cmapTable readCmapTable(CFile& file) {
    cmapTable cmap;
    cmap.version = file.read<uint16_t>();
    cmap.numTables = file.read<uint16_t>();
    cmap.records = new cmapSubtable[cmap.numTables];
    for (int i = 0; i < cmap.numTables; i++) {
        cmap.records[i].platformID = file.read<uint16_t>();
        cmap.records[i].encodingID = file.read<uint16_t>();
        cmap.records[i].offset = file.read<uint32_t>();
    }
    return cmap;
}


cmapFormat4 readFormat4(CFile& file) {
	cmapFormat4 format{};
    format.format = file.read<uint16_t>();
    format.length = file.read<uint16_t>();
    format.language = file.read<uint16_t>();
    format.segCount = file.read<uint16_t>() / 2;
    format.searchRange = file.read<uint16_t>();
    format.entrySelector = file.read<uint16_t>();
    format.rangeShift = file.read<uint16_t>();
    format.endCode = new uint16_t[format.segCount];
    for(int i = 0; i < format.segCount; i++) format.endCode[i] = file.read<uint16_t>();
    format.reservedPad = file.read<uint16_t>();
    format.startCode = new uint16_t[format.segCount];
    for(int i = 0; i < format.segCount; i++) format.startCode[i] = file.read<uint16_t>();
    format.idDelta = new int16_t[format.segCount];
    for(int i = 0; i < format.segCount; i++) format.idDelta[i] = file.read<int16_t>();
    format.idRangeOffset = new uint16_t[format.segCount];
    for(int i = 0; i < format.segCount; i++) format.idRangeOffset[i] = file.read<uint16_t>();
    int glyphCount = (format.length - (16 + format.segCount * 8)) / 2;
    format.glyphIdArray = new uint16_t[glyphCount];
    for(int i = 0; i < glyphCount; i++) format.glyphIdArray[i] = file.read<uint16_t>();
    return format;
}

maxpTable readMaxpTable(CFile& file) {
    return {
        file.read<fixed>(),        // version
        file.read<uint16_t>(),     // numGlyphs
        file.read<uint16_t>(),     // maxPoints
        file.read<uint16_t>(),     // maxContours
        file.read<uint16_t>(),     // maxComponentPoints
        file.read<uint16_t>(),     // maxZones
        file.read<uint16_t>(),     // twillightZone
        file.read<uint16_t>(),     // maxStorage
        file.read<uint16_t>(),     // maxFunctionDefs
        file.read<uint16_t>(),     // maxInstructionDefs
        file.read<uint16_t>(),     // maxStackElements
        file.read<uint16_t>(),     // maxSizeOfInstructions
        file.read<uint16_t>(),     // maxComponentElements
        file.read<uint16_t>()      // maxComponentDepth
    };
}

locaTable readLocaTable(CFile& file, uint16_t indexToLocFormat, uint32_t size) {
	locaTable loca;
	loca.size = size + 1;
	loca.offsets = new uint32_t[loca.size];
    if (indexToLocFormat == 0) {
        for (uint32_t i = 0; i < loca.size; i++) {
            loca.offsets[i] = file.read<uint16_t>() * 2;
        }
    }
    else {
        for (uint32_t i = 0; i < loca.size; i++) {
            loca.offsets[i] = file.read<uint32_t>();
        }
    }
    return loca;
}


Glyph readGlypth(CFile& file) {
	Glyph glyph;
    int16_t numberOfContours = file.read<int16_t>();
    glyph.xMin = file.read<int16_t>();
    glyph.yMin = file.read<int16_t>();
    glyph.xMax = file.read<int16_t>();
    glyph.yMax = file.read<int16_t>();
	if(numberOfContours <= 0) return glyph;

	glyph.contourEnds.resize(numberOfContours);
    for(int16_t i = 0; i < numberOfContours; i++) {
        glyph.contourEnds[i] = file.read<uint16_t>();
    }

	uint16_t instructionLength = file.read<uint16_t>();
    for(int i = 0; i < instructionLength; i++) {
        file.read<uint8_t>();
    }

	uint16_t numPoints = glyph.contourEnds.back() + 1;
    std::vector<uint8_t> flags(numPoints);
	for(int i = 0; i < numPoints; i++) {
        uint8_t flag = file.read<uint8_t>();
        flags[i] = flag; 
		if (flag & 0x08) {
            uint8_t repeat = file.read<uint8_t>();
            for(int j = 0; j < repeat; j++) flags[++i] = flag;
        }
    }

	glyph.points.resize(numPoints);
    glyph.onCurve.resize(numPoints);
	int16_t x = 0;
    for(int i = 0; i < numPoints; i++) {
        uint8_t flag = flags[i];
        if(flag & 0x02) {
            uint8_t dx = file.read<uint8_t>();
            if(flag & 0x10) x += dx;
            else x -= dx;
        }
        else {
            if(!(flag & 0x10)) x += file.read<int16_t>();
        }
        glyph.points[i].x = x;
    }

    int16_t y = 0;
    for(int i = 0; i < numPoints; i++) {
        uint8_t flag = flags[i];
        if(flag & 0x04) {
            uint8_t dy = file.read<uint8_t>();
            if(flag & 0x20) y += dy;
            else y -= dy;
        }
        else {
            if(!(flag & 0x20)) y += file.read<int16_t>();
        }
        glyph.points[i].y = y;
        glyph.onCurve[i] = flags[i] & 0x01;
    }

	return glyph;
}


bool IOSystem::readTTF(TTFAtlas& out, const char* filename) {
	CFile f = openCFile(filename);
	if(f.isEmpty()) return false;
	f.setEndian(Endian::Big);

	offsetSubTable oTable = readOffsetTable(f);
	// printf("scalerType = 0x%08X\nnumTables = %d\n", oTable.scalerType, oTable.numTables);
	if(!(oTable.scalerType == 0x00010000 || oTable.scalerType == 0x74727565)) { 
		return false;
	}

	std::vector<tableDirectory> dTables(oTable.numTables);
	for(auto& dtable : dTables) {
		dtable = readTableDirectory(f);
		// printf("TableDirectory\n\ttag = %c%c%c%c\n\tcheckSum = %d\n\toffset = %d\n\tlength = %d\n",  
		// 	(dtable.tag >> 24) & 0xFF, (dtable.tag >> 16) & 0xFF, (dtable.tag >> 8) & 0xFF, dtable.tag & 0xFF, 
		// 	dtable.checkSum, 
		// 	dtable.offset, 
		// 	dtable.length);
	}
	
	seekCFile(f, dTables[findTableIndex(dTables, MakeTag("head"))].offset, SEEK_SET);
	out.head = readHeadTable(f);
	// printf("HeadTable:\n\tversion = 0x%08X\n\tfontRevision = %f\n\tcheckSumAdjustment = %d\n\tmagicNumber = 0x%08X\n\tflags = %d\n\txMin = %d\n\tyMin = %d\n\txMax = %d\n\tyMax = %d\n\tmacStyle = %d\n\tlowestRecPPEM = %d\n\tfontDirectionHint = %d\n\tindexToLocFormat = %d\n\tglyphDataFormat = %d\n", 
	// 	head.version,
	// 	head.fontRevision / 65536.0f, 
	// 	head.checkSumAdjustment, 
	// 	head.magicNumber, 
	// 	head.flags, 
	// 	head.xMin, 
	// 	head.yMin, 
	// 	head.xMax, 
	// 	head.yMax, 
	// 	head.macStyle, 
	// 	head.lowestRecPPEM,
	// 	head.fontDirectionHint, 
	// 	head.indexToLocFormat, 
	// 	head.glyphDataFormat);
	if(out.head.magicNumber != 0x5F0F3CF5) return false;
	if(out.head.version != 0x00010000) return false;


	uint32_t cmapOffset = dTables[findTableIndex(dTables, MakeTag("cmap"))].offset;
	seekCFile(f, cmapOffset, SEEK_SET);
	out.cmap = readCmapTable(f);
	if(out.cmap.version != 0) return false;
	// printf("cmap Table:\n\tversion = %d\n\tnumberSubtables = %d\n", out.cmap.version, out.cmap.numTables);
	// for(int i = 0; i < out.cmap.numTables; ++i) {
	// 	printf("\tcmap Subtable:\n\t\tplatformID = %d\n\t\tplatformSpecificID = %d\n\t\toffset = %d\n", 
	// 		out.cmap.records[i].platformID, 
	// 		out.cmap.records[i].encodingID, 
	// 		out.cmap.records[i].offset);
	// }


	cmapSubtable* selected = nullptr;
	for (int i = 0; i < out.cmap.numTables; i++) {
		auto& rec = out.cmap.records[i];
		if (rec.platformID == 3 && rec.encodingID == 1) {
			selected = &rec;
			break;
		}
	}
	if (!selected) return false;
	seekCFile(f, cmapOffset + selected->offset, SEEK_SET);
	out.format = readFormat4(f);
	// printf("\tFormat4\n\t\tlength = %d\n\t\tlanguage = %d\n\t\tsegCount = %d\n\t\tsearhRange = %d\n\t\tentrySelector = %d\n\t\trangeShift = %d\n", 
	// 	out.format.length, 
	// 	out.format.language, 
	// 	out.format.segCount, 
	// 	out.format.searchRange, 
	// 	out.format.entrySelector, 
	// 	out.format.rangeShift
	// );


	uint32_t maxpffset = dTables[findTableIndex(dTables, MakeTag("maxp"))].offset;
	seekCFile(f, maxpffset, SEEK_SET);
	out.maxp = readMaxpTable(f);
	// printf("maxpTable:\n\tversion = %d\n\tnumGlyphs = %d\n\tmaxPoints = %d\n\tmaxContours = %d\n\tmaxComponentPoints = %d\n\tmaxZones = %d\n\ttwillightZone = %d\n\tmaxStorage = %d\n\tmaxFunctionDefs = %d\n\tmaxInstructionDefs = %d\n\tmaxStackElements = %d\n\tmaxSizeOfInstructions = %d\n\tmaxComponentElements = %d\n\tmaxComponentDepth = %d\n",
	// 	out.maxp.version,
	// 	out.maxp.numGlyphs,
	// 	out.maxp.maxPoints,
	// 	out.maxp.maxContours,
	// 	out.maxp.maxComponentPoints,
	// 	out.maxp.maxZones,
	// 	out.maxp.twillightZone,
	// 	out.maxp.maxStorage,
	// 	out.maxp.maxFunctionDefs,
	// 	out.maxp.maxInstructionDefs,
	// 	out.maxp.maxStackElements,
	// 	out.maxp.maxSizeOfInstructions,
	// 	out.maxp.maxComponentElements,
	// 	out.maxp.maxComponentDepth
	// );
	
	uint32_t locaffset = dTables[findTableIndex(dTables, MakeTag("loca"))].offset;
	seekCFile(f, locaffset, SEEK_SET);
	locaTable loca = readLocaTable(f, out.head.indexToLocFormat, out.maxp.numGlyphs);


	out.glyphs.resize(out.maxp.numGlyphs);
	size_t glyfIndex = findTableIndex(dTables, MakeTag("glyf"));
	uint32_t glyfOffset = dTables[glyfIndex].offset;
	for(uint16_t i = 0; i < out.maxp.numGlyphs; i++) {
		uint32_t start = loca.offsets[i];
		uint32_t end = loca.offsets[i + 1];
		if(end == start) continue;
		seekCFile(f, glyfOffset + start, SEEK_SET);
		out.glyphs[i] = readGlypth(f);
	}
	return true;
}





int TTFAtlas::getCharacterIndex(int character) {
	if(format.format == 4) {
		int index = 0;
		for(; index < format.segCount; ++index) { 
			if(format.startCode[index] <= character && character <= format.endCode[index]) {
				break;
			}
		}
		if(index == format.segCount) { return -1;}
		int glyphIndex;
		if(format.idRangeOffset[index] == 0) {
			glyphIndex = (character + format.idDelta[index]) & 0xFFFF;
		}
		else {
			glyphIndex =  *(format.idRangeOffset + (format.idRangeOffset[index] >> 1));
			// if(glyphIndex != 0) glyphIndex = (glyphIndex + format.idDelta[index]) & 0xFFFF;
		}
		
		return glyphIndex;
	}
	return -1;
}




char TTFAtlas::drawGlyph(Layer& layer, Glyph& glyph, float scale, float padding) {
	Vector2 glyphOffset{-glyph.xMin, -glyph.yMin };
	Vector2 offset{padding, padding};

	std::vector<Edge> edges;
	int contourStart = 0;
	for(size_t c = 0; c < glyph.contourEnds.size(); c++) {
		int contourEnd = glyph.contourEnds[c];
		char rgbState = 0;
		for(int i = contourStart; i <= contourEnd; i++) {
			int next = (i == contourEnd) ? contourStart : i + 1;
			int next2 = (next == contourEnd) ? contourStart : next + 1;

			Vector2 p0 = (glyph.points[i] + glyphOffset) * scale + offset;
			Vector2 p1 = (glyph.points[next] + glyphOffset) * scale + offset;
			Vector2 p2 = (glyph.points[next2] + glyphOffset) * scale + offset;

			char state = (glyph.onCurve[i] << 2) | (glyph.onCurve[next] << 1) | (glyph.onCurve[next2] << 0);
			switch(state) {
				case(6): { }
				case(7): {
					edges.push_back({ p0, p1, {}, false, rgbState });
					break;
				}

				case(0): {
					edges.push_back({ (p0 + p1) * 0.5, (p1 + p2) * 0.5f, p1, true, rgbState });
					break;
				}
				case(1): {
					edges.push_back({ (p0 + p1) * 0.5f, p2, p1, true, rgbState });
					break;
				}
				case(4): {
					edges.push_back({ p0, (p1 + p2) * 0.5f, p1, true, rgbState });
					break;
				}
				case(5): {
					edges.push_back({ p0, p2, p1, true, rgbState });
					break;
				}
			}
			if(i > contourStart && state != 2 && state != 3) {
				Edge& prevEdge = edges[edges.size() - 2];
				Edge& edge = edges.back();
				Vector2 d0 = (prevEdge.b - prevEdge.a).normalized();
				Vector2 d1 = (edge.b - edge.a).normalized();

				float dot = Vector2::Dot(d0, d1);
				bool sharp = dot < cosf(30 * (Math::PI / 180.0f));
				if (sharp) {
					rgbState++;
					if (rgbState > 2) rgbState = 0;
					edge.rgbState = rgbState;
				}
			}
        }
		if(edges.back().rgbState == 0) {
			edges.back().rgbState = 1;
		}
        contourStart = contourEnd + 1;
    }


	int height = layer.getHeight();
	int width = layer.getWidth();
	int index = 0;
	for(int y = 0; y < height; ++y) {
		for(int x = 0; x < width; ++x) {
			Vector2 p{ (float)x + 0.5f, (float)y + 0.5f };
			float minDist[3] = { FLT_MAX, FLT_MAX, FLT_MAX };
			int counter = 0;
			for(auto& e : edges) {
				float d2;
				if(0) {
					if(intersectBezierRay(p, e.a, e.control, e.b)) counter++;
					d2 = distanceToBezierSquared(p, e.a, e.control, e.b);
				}
				else {
					if(intersectRay(p, e.a, e.b)) counter++;
					d2 = distanceToLineSquared(p, e.a, e.b);
				}

				if(minDist[e.rgbState] > d2) minDist[e.rgbState] = d2;
			}

			float pxRange = 5;
			bool inside = counter & 0x1;
			float rgb[3];
			float sign = inside ? 1.0f : -1.0f;
			for(int i = 0; i < 3; ++i) {
				float d = sqrtf(minDist[i]) * sign;
				rgb[i] = Math::clamp( d / pxRange * 0.5f + 0.5f, 0.0f, 1.0f );
			}
			layer.getImage()[index++] = Color(rgb[0], rgb[1], rgb[2]).ToINT();
		}
	}
	return 0;
}



int TTFAtlas::toTexture() {	
	size_t glyphsSize = getGlyphsSize();
	Rect bbox = getBoundingBox();

	int sizer = 64;
	int ImageWidth = 1024;
	int ImageHeight = 1024;
	int fontSize = sizer;
	int numInARow = ImageWidth / fontSize;

	Image image(ImageWidth, ImageHeight);
	Layer *layers = new Layer[glyphsSize];
	float padding = 3;
	int startPosX = 0;
	int startPosY = 0;
	for(size_t i = 0; i < glyphsSize; ++i) {
		Glyph& glyph = getGlyph(i);
		Vector2 available{ fontSize - padding * 2.0f, fontSize - padding * 2.0f };
		Vector2 glyphSize{ glyph.width(), glyph.height() };
		char mainAxis = glyphSize.y > glyphSize.x;
		if(glyphSize[mainAxis] < 0.001f) continue;

		
		float scale = available[mainAxis] / glyphSize[mainAxis];
		glyphSize *= scale;
		float paddingX2 = padding * 2.0f;
		float layerWidth  = paddingX2 + glyphSize[0];
		float layerHeight = paddingX2 + glyphSize[1];
		if(startPosX + layerWidth >= ImageWidth) {
			startPosX = 0;
			startPosY += fontSize;
		}
		layers[i].init(startPosX, startPosY, layerWidth, layerHeight);

		drawGlyph(layers[i], glyph, scale, padding);
		glyph.uv = { 
			(float)startPosX / ImageWidth,
			(float)startPosY / ImageHeight,
			(float)(startPosX + layerWidth) / ImageWidth, 
			(float)(startPosY + layerHeight) / ImageHeight };
		image.addLayer(&layers[i]);
		startPosX += layerWidth;
	}
	return image.convertToTexture();
	//printf("glyphIndex: %d\n", getCharacterIndex('M'));
}
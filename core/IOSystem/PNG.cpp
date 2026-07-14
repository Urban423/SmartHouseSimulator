#include "IOSystem.h"
#include <zlib.h>

uint32_t readBE32(CFile& f) {
    uint8_t b[4];
    readCFile(b, 4, f);
    return (uint32_t(b[0]) << 24) |
           (uint32_t(b[1]) << 16) |
           (uint32_t(b[2]) << 8)  |
            uint32_t(b[3]);
}

constexpr uint32_t FOURCC(const char* s) {
    return (uint32_t)(uint8_t)s[0] << 24 |
           (uint32_t)(uint8_t)s[1] << 16 |
           (uint32_t)(uint8_t)s[2] << 8  |
           (uint32_t)(uint8_t)s[3];
}

inline int paeth(int a, int b, int c) {
    int p = a + b - c;
    int pa = abs(p - a);
    int pb = abs(p - b);
    int pc = abs(p - c);
    if (pa <= pb && pa <= pc) return a;
    if (pb <= pc) return b;
    return c;
}







struct IHDR {
	uint32_t width;
	uint32_t height;
	uint8_t  bitDepth;
	uint8_t  colorType;
	uint8_t  compressionMethod;
	uint8_t  filterMethod;
	uint8_t  interlaceMethod;
};

bool IOSystem::readPNG(TextureStruct& out, const char* filename) {
	CFile f = openCFile(filename);
	if(f.isEmpty()) return false;

    bool isReading = true;
    uint64_t signature;
    IHDR ihdr;
    std::vector<uint8_t> idat;
    std::vector<uint32_t> palette;

    readCFile(&signature, sizeof(uint64_t), f);
    // printf("%016llX\n", signature);
    if(signature != 0x0A1A0A0D474E5089ULL) return false;
    while(isReading) {
        uint32_t chunkSize = readBE32(f);
        uint32_t chunkType = readBE32(f);
        switch(chunkType) {
            case(FOURCC("IHDR")): {
                ihdr.width = readBE32(f);
                ihdr.height = readBE32(f);
                readCFile(&ihdr.bitDepth, sizeof(uint8_t), f);
                readCFile(&ihdr.colorType, sizeof(uint8_t), f);
                readCFile(&ihdr.compressionMethod, sizeof(uint8_t), f);
                readCFile(&ihdr.filterMethod, sizeof(uint8_t), f);
                readCFile(&ihdr.interlaceMethod, sizeof(uint8_t), f);
                break;
            }

            case(FOURCC("IDAT")): {
                size_t oldSize = idat.size();
                idat.resize(oldSize + chunkSize);
                readCFile(idat.data() + oldSize, chunkSize, f);
                break;
            }

            case(FOURCC("PLTE")): {
                if (chunkSize % 3 != 0) return false;

                size_t colorCount = chunkSize / 3;
                palette.resize(colorCount);
                for (size_t i = 0; i < colorCount; ++i) {
                    uint8_t r, g, b;
                    readCFile(&r, 1, f);
                    readCFile(&g, 1, f);
                    readCFile(&b, 1, f);

                    palette[i] = (255u << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8)  | uint32_t(b);
                }
                break;
            }
            
            case(FOURCC("IEND")): {
                isReading = false;
                break;
            }

            default: {
                seekCFile(f, chunkSize, SEEK_CUR);
            }
        }
        uint32_t CRC = readBE32(f);
        // if (!checkCRC(...)) return false;
    }

    int channels;
    switch (ihdr.colorType) {
        case 0: channels = 1; break; // Grayscale
        case 2: channels = 3; break; // RGB
        case 3: channels = 1; break; // Indexed
        case 4: channels = 2; break; // Gray + Alpha
        case 6: channels = 4; break; // RGBA
        default: return false;
    }
    
    int bitsPerPixel = channels * ihdr.bitDepth;
    size_t rowSize = (ihdr.width * bitsPerPixel + 7) / 8;
    size_t decompressedSize = (rowSize + 1) * ihdr.height;

    std::vector<uint8_t> imageData(decompressedSize);
    uLongf destLen = imageData.size();
    if (uncompress(imageData.data(), &destLen, idat.data(), idat.size()) != Z_OK) return false;


    int bytesPerPixel = (bitsPerPixel + 7) / 8;
    std::vector<uint8_t> pixels(rowSize * ihdr.height);
    for (uint32_t y = 0; y < ihdr.height; ++y) {
        const uint8_t* src = imageData.data() + y * (rowSize + 1);
        uint8_t* dst = pixels.data() + y * rowSize;
        uint8_t filter = src[0];
        src++;
        switch (filter) {
            case 0: { // None 
                memcpy(dst, src, rowSize);
                break;
            }

            case 1: { // Sub
                for (size_t x = 0; x < rowSize; ++x) {
                    uint8_t left = (x >= (size_t)bytesPerPixel) ? dst[x - bytesPerPixel] : 0;
                    dst[x] = src[x] + left;
                }
                break;
            }

            case 2: { // Up
                const uint8_t* prev = (y > 0) ? pixels.data() + (y - 1) * rowSize: nullptr;
                for (size_t x = 0; x < rowSize; ++x) {
                    uint8_t up = prev ? prev[x] : 0;
                    dst[x] = src[x] + up;
                }
                break;
            }

            case 3: { // Average
                const uint8_t* prev = (y > 0) ? pixels.data() + (y - 1) * rowSize : nullptr;
                for (size_t x = 0; x < rowSize; ++x) {
                    uint8_t left = (x >= (size_t)bytesPerPixel) ? dst[x - bytesPerPixel] : 0;
                    uint8_t up = prev ? prev[x] : 0;
                    dst[x] = src[x] + ((left + up) >> 1);
                }
                break;
            }

            case 4: { // Paeth
                const uint8_t* prev = (y > 0) ? pixels.data() + (y - 1) * rowSize : nullptr;
                for (size_t x = 0; x < rowSize; ++x) {
                    uint8_t left = (x >= (size_t)bytesPerPixel) ? dst[x - bytesPerPixel] : 0;
                    uint8_t up = prev ? prev[x] : 0;
                    uint8_t upLeft = (prev && x >= (size_t)bytesPerPixel) ? prev[x - bytesPerPixel] : 0;
                    dst[x] = src[x] + paeth(left, up, upLeft);
                }
                break;
            }
            default: return false;
        }
    }


    out.width = ihdr.width;
    out.height = ihdr.height;
    int pixelsCount = out.width * out.height;
    out.pixels = new int[pixelsCount];

    const uint8_t* src = pixels.data();
    switch (ihdr.colorType) {
        case 6: { // RGBA 
            for (uint32_t y = 0; y < out.height; ++y) {
                uint32_t dstY = out.height - 1 - y;
                for (uint32_t x = 0; x < out.width; ++x) {
                    uint8_t r = *src++;
                    uint8_t g = *src++;
                    uint8_t b = *src++;
                    uint8_t a = *src++;
                    out.pixels[dstY * out.width + x] = (a << 24) | (b << 16) | (g << 8) | r;
                }
            }
            break;
        }

        case 2: { // RGB
            for (uint32_t y = 0; y < out.height; ++y) {
                uint32_t dstY = out.height - 1 - y;
                for (uint32_t x = 0; x < out.width; ++x) {
                    uint8_t r = *src++;
                    uint8_t g = *src++;
                    uint8_t b = *src++;
                    out.pixels[dstY * out.width + x] = (255 << 24) | (b << 16) | (g << 8) | r;
                }
            }
            break;
        }

        case 0: { // Grayscale
            for (uint32_t y = 0; y < out.height; ++y) {
                uint32_t dstY = out.height - 1 - y;
                for (uint32_t x = 0; x < out.width; ++x) {
                    uint8_t l = *src++;
                    out.pixels[dstY * out.width + x] = (255 << 24) | (l << 16) | (l << 8) | l;
                }
            }
            break;
        }

        case 4: { // Grayscale + Alpha
            for (uint32_t y = 0; y < out.height; ++y) {
                uint32_t dstY = out.height - 1 - y;
                for (uint32_t x = 0; x < out.width; ++x) {
                    uint8_t l = *src++;
                    uint8_t a = *src++;
                    out.pixels[dstY * out.width + x] = (a << 24) | (l << 16) | (l << 8) | l;
                }
            }
            break;
        }

        case 3: { // Indexed
            for (uint32_t y = 0; y < out.height; ++y) {
                uint32_t dstY = out.height - 1 - y;
                for (uint32_t x = 0; x < out.width; ++x) {
                    uint8_t index = *src++;
                    if (index >= palette.size()) return false;
                    out.pixels[dstY * out.width + x] = palette[index];
                }
            }
            break;
        }

        default: {
            printf("34\n");
            delete[] out.pixels;
            out.pixels = nullptr;
            return false;
        }
    }

    return true;
}
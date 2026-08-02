#pragma once
#include <string.h>
#include <cstdint>

template<typename T>
constexpr T byteswap(T value)
{
    if constexpr (sizeof(T) == 1)
        return value;
    else if constexpr (sizeof(T) == 2)
        return __builtin_bswap16(value);
    else if constexpr (sizeof(T) == 4)
        return __builtin_bswap32(value);
    else if constexpr (sizeof(T) == 8)
        return __builtin_bswap64(value);
}

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

enum class Endian {
    Little,
    Big
};

//files
class CFile {
	friend CFile createCFile();
    friend void writeCFile(const void* data, int size, CFile& file);
    friend void saveCFile(const char* name, CFile& file);

	friend char readCFile(void* value, int value_size, CFile& file);
    friend void seekCFile(CFile& file, int offset, int origin);
public:
	CFile();
	CFile(char* ptr, int size);
	~CFile();
	
	template<class T>
	void read(T& value) {
		readCFile(&value, sizeof(T), *this);
		if constexpr (sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8) {
			if (endian != Endian::Little) value = byteswap(value);
		}
	}

	template<class T>
	T read() { T value; read(value); return value; }

	inline void setEndian(Endian newEndian) { endian = newEndian; }
	inline char* getPtr() { return pointer; }
	inline bool isEmpty() { return pointer == nullptr; };
private:
	char* start = nullptr;
	char* pointer = nullptr;
	unsigned int size = 0;
    unsigned int capacity = 0;
	Endian endian = Endian::Little;
};


CFile openCFile(const char* name);
char readCFile(void* value, int value_size, CFile& file);
void seekCFile(CFile& file, int offset, int origin);

CFile createCFile();
void writeCFile(const void* data, int size, CFile& file);
void saveCFile(const char* name, CFile& file);
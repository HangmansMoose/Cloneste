#pragma once

#include <cstdio>
// This is to get malloc
#include <cstdlib>

// This is to get memset
#include <string.h>

// File Timestamps
#include <sys/stat.h>


// #############################################################################
//                           Defines
// #############################################################################

#ifdef _WIN32
#define DEBUG_BREAK() __debugbreak()
#define EXPORT_FN __declspec(dllexport)
#elif __linux__
#define DEBUG_BREAK() __builtin_debugtrap()
#define EXPORT_FN
#elif __APPLE__
#define DEBUG_BREAK() __builtin_trap()
#define EXPORT_FN
#endif

// #############################################################################
//                           Logging
// #############################################################################
enum TextColor
{  
  TEXT_COLOR_BLACK,
  TEXT_COLOR_RED,
  TEXT_COLOR_GREEN,
  TEXT_COLOR_YELLOW,
  TEXT_COLOR_BLUE,
  TEXT_COLOR_MAGENTA,
  TEXT_COLOR_CYAN,
  TEXT_COLOR_WHITE,
  TEXT_COLOR_BRIGHT_BLACK,
  TEXT_COLOR_BRIGHT_RED,
  TEXT_COLOR_BRIGHT_GREEN,
  TEXT_COLOR_BRIGHT_YELLOW,
  TEXT_COLOR_BRIGHT_BLUE,
  TEXT_COLOR_BRIGHT_MAGENTA,
  TEXT_COLOR_BRIGHT_CYAN,
  TEXT_COLOR_BRIGHT_WHITE,
  TEXT_COLOR_COUNT
};

template <typename ...Args>
void _log(char* prefix, char* msg, TextColor textColor, Args... args)
{
    // These values need to align with the enum if you want it to work as
    // expected
  static char* TextColorTable[TEXT_COLOR_COUNT] = 
  {    
    "\x1b[30m", // TEXT_COLOR_BLACK
    "\x1b[31m", // TEXT_COLOR_RED
    "\x1b[32m", // TEXT_COLOR_GREEN
    "\x1b[33m", // TEXT_COLOR_YELLOW
    "\x1b[34m", // TEXT_COLOR_BLUE
    "\x1b[35m", // TEXT_COLOR_MAGENTA
    "\x1b[36m", // TEXT_COLOR_CYAN
    "\x1b[37m", // TEXT_COLOR_WHITE
    "\x1b[90m", // TEXT_COLOR_BRIGHT_BLACK
    "\x1b[91m", // TEXT_COLOR_BRIGHT_RED
    "\x1b[92m", // TEXT_COLOR_BRIGHT_GREEN
    "\x1b[93m", // TEXT_COLOR_BRIGHT_YELLOW
    "\x1b[94m", // TEXT_COLOR_BRIGHT_BLUE
    "\x1b[95m", // TEXT_COLOR_BRIGHT_MAGENTA
    "\x1b[96m", // TEXT_COLOR_BRIGHT_CYAN
    "\x1b[97m", // TEXT_COLOR_BRIGHT_WHITE
  };

  char formatBuffer[8192] = {};
  sprintf(formatBuffer, "%s %s %s \033[0m", TextColorTable[textColor], prefix, msg);

  char textBuffer[8912] = {};
  sprintf(textBuffer, formatBuffer, args...);

  puts(textBuffer);
}

#define SM_TRACE(msg, ...) _log("TRACE: ", msg, TEXT_COLOR_GREEN, ##__VA_ARGS__);
#define SM_WARN(msg, ...) _log("WARN: ", msg, TEXT_COLOR_YELLOW, ##__VA_ARGS__);
#define SM_ERROR(msg, ...) _log("ERROR: ", msg, TEXT_COLOR_RED, ##__VA_ARGS__);

#define SM_ASSERT(x, msg, ...)    \
{                                 \
  if(!(x))                        \
  {                               \
    SM_ERROR(msg, ##__VA_ARGS__); \
    DEBUG_BREAK();                \
    SM_ERROR("Assertion HIT!")    \
  }                               \
}

// #############################################################################
//                           Array
// #############################################################################

template<typename T, int N>
struct Array
{
  static constexpr int maxElements = N;
  int count = 0;
  T elements[N];

  T& operator[](int idx)
  {
    SM_ASSERT(idx >= 0, "idx negative!");
    SM_ASSERT(idx < count, "Idx out of bounds!");
    return elements[idx];
  }

  int add(T element)
  {
    SM_ASSERT(count < maxElements, "Array Full!");
    elements[count] = element;
    return count++;
  }

  void remove_idx_and_swap(int idx)
  {
    SM_ASSERT(idx >= 0, "idx negative!");
    SM_ASSERT(idx < count, "idx out of bounds!");
    elements[idx] = elements[--count];
  }

  void clear()
  {
    count = 0;
  }

  bool is_full()
  {
    return count == N;
  }
};

// #############################################################################
//                           Bump Allocator
// #############################################################################

// A bump allocator is an implementation of an arena, also referred to as a linear allocator.
// It works by pre-allocating a large block of memory and allocates regions from it simply
// by 'bumping' a pointer forward, rather than allocating and deallocating
struct BumpAllocator
{
  size_t capacity;
  size_t used;
  char* memory;
};

BumpAllocator make_bump_allocator(size_t size)
{
  BumpAllocator ba = {};
  
  ba.memory = (char*)malloc(size);
  if(ba.memory)
  {
    ba.capacity = size;
    memset(ba.memory, 0, size); // Sets the memory to 0
  }
  else
  {
    SM_ASSERT(false, "Failed to allocate Memory!");
  }

  return ba;
}

char* bump_alloc(BumpAllocator* bumpAllocator, size_t size)
{
  char* result = nullptr;

  // The aligned size is logically AND'd with NOT 7
  size_t allignedSize = (size + 7) & ~ 7; // This makes sure the first 4 bits are 0 
  if(bumpAllocator->used + allignedSize <= bumpAllocator->capacity)
  {
    result = bumpAllocator->memory + bumpAllocator->used;
    bumpAllocator->used += allignedSize;
  }
  else
  {
    SM_ASSERT(false, "BumpAllocator is full");
  }

  return result;
}

//#######################################################################
//                          File I/O
//#######################################################################
long long get_timestamp(char* file)
{
  // stat() fills out the provided stat structure with information about the queried file
  /*   
  struct stat
    {
        _dev_t         st_dev;      // Device ID of device containing file
        _ino_t         st_ino;      // File serial number (nix inode)
        unsigned short st_mode;     // mode of file
        short          st_nlink;    // Number of hard links to file
        short          st_uid;      // User ID of file
        short          st_gid;      // Group ID of file
        _dev_t         st_rdev;     // Device Id (if file is character or block special)
        _off_t         st_size;     // Size in bytes of file (or lenght in bytes of the pathname for a symlink)
        time_t         st_atime;    // Time of last access
        time_t         st_mtime;    // Time of last data modification
        time_t         st_ctime;    // Time of last status change
    };*/ 
  struct stat file_stat = {};
  stat(file, &file_stat);
  return file_stat.st_mtime;
}

// Proves a file exists by attempting to open it
bool file_exists(char* filePath)
{
  SM_ASSERT(filePath, "No filePath supplied!");

  auto file = std::fopen(filePath, "rb");
  if(!file)
  {
    return false;
  }
  fclose(file);

  return true;
}

long get_file_size(const char* filePath)
{
    SM_ASSERT(filePath, "No filePath supplied!");

    long fileSize = 0;
    auto file = std::fopen(filePath, "rb");

    if(!file)
    {
        SM_ERROR("Failed opening file: %s", filePath);
        return false;
    }
    
    // This seeks to the end of the file, then ftell gives us the current offset
    // in bytes then the second fseek returns the file pointer back to the beginning
    // of the file
    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    fclose(file);

    return fileSize;
}

/*
* Reads a file into a supplied buffer. We manage our own
* memory and therefore want more control over where it 
* is allocated
*/
char* read_file(const char* filePath, int* fileSize, char* buffer)
{
  SM_ASSERT(filePath, "No filePath supplied!");
  SM_ASSERT(fileSize, "No fileSize supplied!");
  SM_ASSERT(buffer, "No buffer supplied!");

  *fileSize = 0;
  auto file = fopen(filePath, "rb");
  if(!file)
  {
    SM_ERROR("Failed opening File: %s", filePath);
    return nullptr;
  }

  fseek(file, 0, SEEK_END);
  *fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);

  memset(buffer, 0, *fileSize + 1);
  fread(buffer, sizeof(char), *fileSize, file);

  fclose(file);

  return buffer;
}

char* read_file(const char* filePath, int* fileSize, BumpAllocator* bumpAllocator)
{
  char* file = nullptr;
  long fileSize2 = get_file_size(filePath);

  if(fileSize2)
  {
    char* buffer = bump_alloc(bumpAllocator, fileSize2 + 1);

    file = read_file(filePath, fileSize, buffer);
  }

  return file; 
}

void write_file(const char* filePath, char* buffer, int size)
{
  SM_ASSERT(filePath, "No filePath supplied!");
  SM_ASSERT(buffer, "No buffer supplied!");
  auto file = fopen(filePath, "wb");
  if(!file)
  {
    SM_ERROR("Failed opening File: %s", filePath);
    return;
  }

  fwrite(buffer, sizeof(char), size, file);
  fclose(file);
}

bool copy_file(const char* fileName, const char* outputName, char* buffer)
{
  int fileSize = 0;
  char* data = read_file(fileName, &fileSize, buffer);

  auto outputFile = fopen(outputName, "wb");
  if(!outputFile)
  {
    SM_ERROR("Failed opening File: %s", outputName);
    return false;
  }

  int result = fwrite(data, sizeof(char), fileSize, outputFile);
  if(!result)
  {
    SM_ERROR("Failed opening File: %s", outputName);
    return false;
  }
  
  fclose(outputFile);

  return true;
}

bool copy_file(const char* fileName, const char* outputName, BumpAllocator* bumpAllocator)
{
  char* file = 0;
  long fileSize2 = get_file_size(fileName);

  if(fileSize2)
  {
    char* buffer = bump_alloc(bumpAllocator, fileSize2 + 1);

    return copy_file(fileName, outputName, buffer);
  }

  return false;
}

//#######################################################################
//                          Normal Colors
//#######################################################################
//constexpr Vec4 COLOR_WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
//constexpr Vec4 COLOR_RED = {1.0f, 0.0f, 0.0f, 1.0f};
//constexpr Vec4 COLOR_GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
//constexpr Vec4 COLOR_BLUE = {0.0f, 0.0f, 1.0f, 1.0f};
//constexpr Vec4 COLOR_YELLOW = {1.0f, 1.0f, 0.0f, 1.0f};
//constexpr Vec4 COLOR_BLACK = {0.0f, 0.0f, 0.0f, 1.0};



















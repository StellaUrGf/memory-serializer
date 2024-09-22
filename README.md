## memory-serializer
The memory-serializer is a comprehensive, header-only C++ library designed to provide a suite of utilities, data structures, and helper functions to simplify and enhance your C++ development experience. This library includes utilities for memory management, serialization, string formatting, and more.

## Features:
- Header-only: Integrate the library by simply including a header file.
- Memory Management: Efficient handling of dynamic memory includes setting and getting various types.
- Serialization: Save and load the state of memory buffers to and from files.
- String Formatting: Flexible and type-safe string formatting.
- Heap Management: Custom heap allocation and deallocation utilities.

 ## Installation:
- include header into your project.
- define macros ``__USING_SERIALIZER__`` before including the header.

. Example:
```cpp
#define __USING_SERIALIZER__
#include <serializer.h>

using namespace utils;

int main(int argc, char* argv[])
{
    
    return EXIT_SUCCESS;
}
```

## Usage:
- Basic example of using ``wmemory_t`` classes.
```cpp
using namespace utils;

int main(int argc, char* argv[])
{
    wmemory_t* buffer = new wmemory_t(0x4);
    if (!buffer->is_valid())
        delete buffer

    buffer->setInt(9);
    delete buffer;

    return EXIT_SUCCESS;
}
```
- Error handling and Exception.
```cpp
using namespace utils;

int main(int argc, char *argv[])
{
    try {

        wmemory_t* buffer = new wmemory_t(0x1);
        if (!buffer->is_valid())
            delete buffer;

        // resize the buffer you want to write
        if (buffer->lens() + sizeof(int) > buffer->size())
            buffer->resize(buffer->size() + sizeof(int));

        buffer->setInt(9);
        delete buffer;

    } catch (std::exception& e) {
        std::cout << utils::format("wmemory_t::exception -> {}\n", e.what());
    }
    return EXIT_SUCCESS;
}
```
## Input/Output (I/O) Utilities
The io namespace provides functions for serializing and deserializing memory buffers to and from files, which is giving in the example already.
- ``void serialize(wmemory_t *buffer, const char *filename)``: Serialize memory buffer to a file.
- ``void deserialize(wmemory_t *buffer, const char *filename)``: Deserialize memory buffer from a file.

# Example:
- Serialize data into binary file
```cpp
using namespace utils;

int main(int argc, char *argv[])
{
    wmemory_t* buffer = new wmemory_t(0x1);
    if (!buffer->is_valid())
        delete buffer;
    buffer->setBytes(0xA);
    utils::io::serialize(buffer, "test.bin");
    delete buffer;
    
    return EXIT_SUCCESS;
}
```
- Deserialize data from binary file
```cpp
using namespace utils;

int main(int argc, char *argv[])
{
    wmemory_t buffer = _STD nullptr_t();
    utils::io::deserialize(&buffer, "test.bin");
    if (buffer.is_valid()) {
       const char bytes = buffer.get_bytes();
        std::cout << "bytes: " << bytes << std::endl;
    }
    return EXIT_SUCCESS;
}
```

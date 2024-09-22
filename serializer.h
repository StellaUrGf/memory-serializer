#ifndef SERIALIZER_H
#define SERIALIZER_H
#if _MSC_VER
#include <io.h>
#include <stdint.h>
#elif defined(__MINGW32__) || defined(__MINGW64__)
#include <unistd.h>
#include <cstdint>
#define _STD ::std::
#endif
#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <fstream>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <concepts>
#include <type_traits>
#include <tuple>
#include <new>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <functional>
#ifdef __USING_SERIALIZER__
namespace utils {
    // initialize variant for support data-type
    using variant = std::variant<std::string, std::string_view, char, uint8_t, short, uint16_t, int, uint32_t, long long
        , uint64_t, bool, float, double>;
    // class to define support variant type for short used and easy to understand
    // at this point you can just use enum, but i prefer class because it look better
    class support_t {
    public:
        static constexpr int variant_str = 0;
        static constexpr int variant_strview = 1;
        static constexpr int variant_int8 = 2;
        static constexpr int variant_uint8 = 3;
        static constexpr int variant_int16 = 4;
        static constexpr int variant_uint16 = 5;
        static constexpr int variant_int32 = 6;
        static constexpr int variant_uint32 = 7;
        static constexpr int variant_int64 = 8;
        static constexpr int variant_uint64 = 9;
        static constexpr int variant_bool = 10;
        static constexpr int variant_float = 11;
        static constexpr int variant_double = 12;
    };

    class wmemory_t {
    public:
        wmemory_t(const std::nullptr_t &) {
            m_size = 0x00, m_lens = 0x00;
            buffer.clear();
        }

        /**
       * Constructs a `wmemory_t` object with the specified size.
       *
       * @param size The size to reserve for the internal buffer. Must be greater than zero.
       *
       * @throws std::invalid_argument If the size is less than or equal to zero.
       *
       * @return A newly constructed `wmemory_t` object with reserved size.
       */
        wmemory_t(const uintmax_t &size) {
            if (size <= 0x000)
                throw std::invalid_argument("size must be greater than zero");
            buffer.reserve(size);
            m_size = size, m_lens = 0x00;
            _STD memset(buffer.data(), 0x0, m_size);
        }

        /**
         * Constructs a `wmemory_t` object from the provided vector of bytes.
         *
         * @param con A vector of `uint8_t` representing the initial content to be used
         *            for constructing the internal buffer.
         *
         * @return A newly constructed `wmemory_t` object initialized with the contents
         *         of the provided vector.
         */
        wmemory_t(const std::vector<uint8_t> &con) {
            buffer.reserve(con.size());
            m_size = con.size(), m_lens = 0x00;
            _STD memset(buffer.data(), 0x0, m_size);
        }

        /**
         * Constructs a `wmemory_t` object with the given data and size.
         *
         * @param data A pointer to the initial data to be used for constructing the internal buffer.
         *             Must not be null.
         * @param size The size of the data to reserve for the internal buffer. Must be greater than zero.
         *
         * @throws std::invalid_argument If the data pointer is null or the size is less than or equal to zero.
         *
         * @return A newly constructed `wmemory_t` object initialized with the provided data and size.
         */
        wmemory_t(uint8_t *data, const uintmax_t &size) {
            if (data != nullptr && size >= 1) {
                buffer.reserve(size);
                _STD memcpy(buffer.data(), data, size);
                m_size = size, m_lens = 0x00;
            } else throw std::invalid_argument("data is null or size is negative");
        }

        wmemory_t(const wmemory_t &next) {
            if (next.m_size != 0x00 && next.buffer.data()) {
                buffer.clear();
                buffer = next.buffer;
                m_size = next.m_size, m_lens = next.m_lens;
            }
        }

        void resize(const uintmax_t &size) {
            buffer.resize(size);
        }

        void reserve(const uintmax_t &size) {
            buffer.reserve(size);
        }

        void reserve(uint8_t *data, const uintmax_t &size) {
            if (data != nullptr && size >= 1) {
                buffer.reserve(size);
                _STD memcpy(buffer.data(), data, size);
                m_size = size, m_lens = 0x00;
            } else throw std::invalid_argument("data is null or size is negative");
        }

        /**
         * Cleans up the internal buffer and resets its size and length indicators.
         *
         * This function clears the internal buffer if it contains any data,
         * and then sets the length and size indicators to zero.
         *
         * @note This function is marked as `noexcept` and guarantees not to throw any exceptions.
         */
        constexpr void cleanup() noexcept {
            if (buffer.data()) {
                buffer.clear();
                m_lens = 0x00, m_size = 0x00;
            }
        }

        /**
         * Checks if there is enough space in the internal buffer to accommodate additional data.
         *
         * @param size The size of the data to be checked against the remaining space in the buffer.
         *             It should be a positive integer.
         *
         * @return `true` if there is enough space in the buffer, `false` otherwise.
         */
        constexpr bool is_enough(const uintmax_t &size) const {
            return m_lens + size <= m_size;
        }

        /**
         * Increases the internal length by the specified size.
         *
         * @param size The size to add to the internal length. Must be greater than zero.
         */
        void skip(const uintmax_t &size) {
            if (size <= 0x000)
                return;
            m_lens += size;
        }

        constexpr auto setBytes(const char &value) noexcept -> void {
            insert(value);
        }

        constexpr auto setShort(const short &value) noexcept -> void {
            insert(value);
        }

        constexpr auto setInt(const int &value) noexcept -> void {
            insert(value);
        }

        constexpr auto setLong(const long long &value) noexcept -> void {
            insert(value);
        }

        constexpr auto setFloat(const float &value) noexcept -> void {
            insert(value);
        }

        constexpr auto setDouble(const double &value) noexcept -> void {
            insert(value);
        }

        constexpr auto setUBytes(const uint8_t &value) noexcept -> void {
            insert(value);
        }

        constexpr auto setUShort(const uint16_t &value) noexcept -> void {
            insert(value);
        }

        constexpr auto setUInt(const uint32_t &value) noexcept -> void {
            insert(value);
        }

        constexpr auto setULong(const uint64_t &value) noexcept -> void {
            insert(value);
        }

        constexpr auto setString(const std::string_view &value) noexcept -> void {
            insert(value);
        }

        constexpr auto setStringView(const std::string_view &value) noexcept -> void {
            insert(value);
        }

        constexpr auto setBool(const bool &value) noexcept -> void {
            insert(value);
        }

    private:
        /**
         * Retrieves a value of type `_typename` from the internal buffer.
         *
         * The method decodes different types from the internal buffer based on
         * the type held by the variant. It handles cases of `std::string_view`,
         * `std::string`, and other primitive types specified by the variant.
         *
         * Note: If an invalid variant type is passed through, it might show an error
         * at compile time. Ensure that the variant type is supported.
         *
         * @throws std::bad_variant_access If the variant holds an invalid type.
         *
         * @return The value of type `_typename` retrieved from the buffer.
         */
        template<class _typename>
        constexpr _typename get() noexcept(false) {
            _typename value;
            const variant &var = value;
            if (var.index() == std::variant_npos)
                throw std::bad_variant_access();
            else if (var.index() == support_t::variant_strview) {
                const size_t size = *(size_t *) &buffer.data()[m_lens];
                m_lens += sizeof(size_t);
                value = std::string_view((char *) buffer.data() + m_lens, size);
                m_lens += size;
            } else if (var.index() == support_t::variant_str) {
                const size_t size = *(size_t *) &buffer.data()[m_lens];
                m_lens += sizeof(size_t);
                value = std::string((char *) buffer.data() + m_lens, size);
                m_lens += size;
            } else {
                _STD memcpy(&value, buffer.data() + m_lens, sizeof(_typename));
                m_lens += sizeof(_typename);
            }
            return value;
        }

    public:
        /**
         * Retrieves a string from the internal buffer.
         *
         * This method reads a size value from the buffer, then reads and returns
         * a string of that size starting from the current buffer position.
         *
         * @return A string extracted from the buffer. Returns an empty string if the buffer size is zero.
         *
         * @exception noexcept This function is declared noexcept and will not throw exceptions.
         */
        const std::string get_string() noexcept(true) {
            if (m_size != 0x00) {
                size_t size = *(size_t *) &buffer.data()[m_lens];
                m_lens += sizeof(size_t);
                std::string value = std::string((char *) buffer.data() + m_lens, size);
                m_lens += size;
                return value;
            }
            return std::string("");
        }

        /**
         * Retrieves a string view from the internal buffer.
         *
         * This function extracts a string view from the internal buffer starting at the current position
         * specified by `m_lens`. The length of the string is determined by reading a `size_t` value from the buffer.
         * `m_lens` is then incremented by the size of the string and returns the string view.
         *
         * @return A `std::string_view` object representing the extracted string. If the internal size `m_size`
         * is zero, an empty string view is returned.
         *
         * @note This function is marked as `noexcept(true)`, which means it is guaranteed not to throw exceptions.
         */
        const std::string_view get_string_view() noexcept(true) {
            if (m_size != 0x00) {
                size_t size = *(size_t *) &buffer.data()[m_lens];
                m_lens += sizeof(size_t);
                std::string_view value = std::string_view((char *) buffer.data() + m_lens, size);
                m_lens += size;
                return value;
            }
            return std::string_view("");
        }

        /**
         * Retrieves the next byte from the buffer.
         *
         * This function extracts a single byte from the buffer if the buffer is not empty.
         * It advances the internal position index after reading the byte.
         *
         * @return The next byte from the buffer, or -1 if the buffer is empty.
         *
         * @note The function is marked as `noexcept(true)`, indicating it does not throw exceptions.
         */
        const char get_bytes() noexcept(true) {
            if (m_size != 0x00) {
                const char value = *(char *) &buffer.data()[m_lens];
                m_lens += sizeof(char);
                return value;
            }
            return -1;
        }

        /**
         * Retrieves a `short` value from the internal buffer if available.
         *
         * If the buffer is not empty, extracts a `short` value from the current position
         * and updates the position accordingly.
         *
         * @return The extracted `short` value, or -1 if the buffer is empty.
         */
        const short get_short() noexcept(true) {
            if (m_size != 0x00) {
                const short value = *(short *) &buffer.data()[m_lens];
                m_lens += sizeof(short);
                return value;
            }
            return -1;
        }

        /**
         * Retrieves an integer from the internal buffer.
         *
         * @return The next integer from the buffer if available, otherwise returns -1.
         *
         * This method is marked noexcept and guarantees not to throw exceptions.
         *
         * The method reads an integer from the current buffer position indicated by `m_lens`,
         * increments `m_lens` by the size of an integer, and returns the extracted value.
         * If `m_size` is zero, indicating that no data is available, the method returns -1.
         */
        const int get_int() noexcept(true) {
            if (m_size != 0x00) {
                const int value = *(int *) &buffer.data()[m_lens];
                m_lens += sizeof(int);
                return value;
            }
            return -1;
        }

        /**
         * Retrieves a long value from the buffer.
         *
         * This function reads a long value from the buffer starting at the current
         * position (`m_lens`), updates the current position, and returns the value.
         * If the buffer is empty (i.e., `m_size` is zero), it returns -1.
         *
         * @return The long value read from the buffer, or -1 if the buffer is empty.
         */
        const long get_long() noexcept(true) {
            if (m_size != 0x00) {
                const long value = *(long *) &buffer.data()[m_lens];
                m_lens += sizeof(long);
                return value;
            }
            return -1;
        }

        /**
         * Retrieves a `long long` value from the buffer if available.
         *
         * This method fetches a `long long` value from the current position within the buffer if the buffer size is non-zero.
         *
         * @return The retrieved `long long` value from the buffer, or -1 if the buffer size is zero.
         */
        const long long get_llong() noexcept(true) {
            if (m_size != 0x00) {
                const long long value = *(long long *) &buffer.data()[m_lens];
                m_lens += sizeof(long long);
                return value;
            }
            return -1;
        }

        /**
         * Retrieves a 16-bit unsigned short from the buffer.
         *
         * @return The 16-bit unsigned short value extracted from the buffer.
         *         If the buffer is empty (m_size is 0), returns `-1`.
         *
         * @throws None This function is marked noexcept(true).
         */
        const uint16_t get_ushort() noexcept(true) {
            if (m_size != 0x00) {
                const uint16_t value = *(uint16_t *) &buffer.data()[m_lens];
                m_lens += sizeof(uint16_t);
                return value;
            }
            return -1;
        }

        /**
         * Retrieves a uint32_t value from the internal buffer.
         *
         * @return The next uint32_t value from the buffer if available; otherwise, returns -1.
         *
         * @throws noexcept This function is guaranteed not to throw any exceptions.
         */
        const uint32_t get_uint() noexcept(true) {
            if (m_size != 0x00) {
                const uint32_t value = *(uint32_t *) &buffer.data()[m_lens];
                m_lens += sizeof(uint32_t);
                return value;
            }
            return -1;
        }

        /**
         * Retrieves a uint64_t value from the buffer.
         *
         * This function extracts and returns a 64-bit unsigned integer from the
         * buffer starting at the current lens position and increments the lens
         * by the size of the uint64_t. If the size is zero, it returns -1.
         *
         * @return uint64_t The extracted 64-bit unsigned integer from the buffer.
         * Returns -1 if the current size is zero.
         */
        const uint64_t get_uint64() noexcept(true) {
            if (m_size != 0x00) {
                const uint64_t value = *(uint64_t *) &buffer.data()[m_lens];
                m_lens += sizeof(uint64_t);
                return value;
            }
            return -1;
        }

        /**
         * Retrieves a boolean value from the internal buffer.
         *
         * @return The boolean value extracted from the buffer if the size is non-zero, otherwise false.
         */
        const bool get_bool() noexcept(true) {
            if (m_size != 0x00) {
                const bool value = *(bool *) &buffer.data()[m_lens];
                m_lens += sizeof(bool);
                return value;
            }
            return false;
        }

        /**
         * Retrieves a floating-point value from the internal buffer.
         *
         * This method reads a `float` from the current position in the buffer,
         * advances the position by the size of a `float`, and returns the read value.
         *
         * @return The `float` value from the buffer if the size is non-zero; otherwise, returns `0.0f`.
         */
        const float get_float() noexcept(true) {
            if (m_size != 0x00) {
                const float value = *(float *) &buffer.data()[m_lens];
                m_lens += sizeof(float);
                return value;
            }
            return 0.0f;
        }

        /**
         * Retrieves a double-precision floating-point number from the buffer.
         *
         * This method extracts a double value from the internal buffer at the current
         * buffer position, updates the position, and returns the extracted value.
         * If the buffer is empty, it returns 0.0.
         *
         * @return The double value extracted from the buffer, or 0.0 if the buffer is empty.
         */
        const double get_double() noexcept(true) {
            if (m_size != 0x00) {
                const double value = *(double *) &buffer.data()[m_lens];
                m_lens += sizeof(double);
                return value;
            }
            return 0.0;
        }

        using iterator = uint8_t *;

        iterator begin() noexcept {
            return buffer.data();
        }

        iterator end() noexcept {
            return buffer.data() + m_lens;
        }

        /**
         * Validates the internal buffer to ensure it contains valid data.
         *
         * This function checks the consistency and integrity of the internal buffer,
         * ensuring that the buffer size and current position are within valid ranges.
         *
         * @return `true` if the internal buffer data is valid, `false` otherwise.
         */
        constexpr bool is_valid() const noexcept {
            // Ensure the buffer is not null, the size is greater than zero,
            return buffer.data() != nullptr && m_size != 0x00;
        }

    private:
        /**
         * Inserts a value into the internal buffer, managing different possible types.
         *
         * @param value The value to insert into the buffer. It must be a variant that
         *              can hold supported types including `std::string`, `std::string_view`,
         *              and other primitive types as defined in the variant type.
         *
         * @throws std::invalid_argument If the variant holds an invalid type.
         * @throws std::runtime_error If the buffer size is exceeded during insertion.
         */
        template<class _typename>
        constexpr void insert(const _typename &value) {
            const variant &var = value;
            if (var.index() == std::variant_npos)
                throw std::invalid_argument("invalid variant access");
            else if (var.index() == support_t::variant_str) {
                const std::string str = std::get<std::string>(var);
                if (m_lens + str.size() > m_size)
                    throw std::runtime_error("maximum buffer size exceeded");
                const size_t lens = str.size();
                _STD memcpy(buffer.data() + m_lens, &lens, sizeof(size_t));
                m_lens += sizeof(size_t);
                _STD memcpy(buffer.data() + m_lens, str.c_str(), lens);
                m_lens += lens;
            } else if (var.index() == support_t::variant_strview) {
                const std::string_view str = std::get<std::string_view>(var);
                if (m_lens + str.size() > m_size)
                    throw std::runtime_error("maximum buffer size exceeded");
                const size_t lens = str.size();
                _STD memcpy(buffer.data() + m_lens, &lens, sizeof(size_t));
                m_lens += sizeof(size_t);
                _STD memcpy(buffer.data() + m_lens, str.data(), lens);
                m_lens += lens;
            } else {
                if (m_lens + sizeof(_typename) > m_size)
                    throw std::runtime_error("maxium buffer size exceeded");
                const _typename object = std::get<_typename>(var);
                std::memcpy(buffer.data() + m_lens, &object, sizeof(_typename));
                m_lens += sizeof(_typename);
            }
        }

    public:
        constexpr uint8_t *data() noexcept {
            return buffer.data();
        }

        constexpr uintmax_t size() noexcept { return m_size; }
        constexpr uintmax_t lens() noexcept { return m_lens; }

    private:
        std::vector<uint8_t> buffer; // main data to store value
    private:
        uintmax_t m_size = 0x00; // size of memory allocation / reallocation
        uintmax_t m_lens = 0x00; // tracker of memory position
    };

    namespace io {
        void serialize(wmemory_t *buffer, const char *filename) {
            FILE *file = fopen(filename, "wb");
            fwrite(buffer->data(), 1, buffer->lens(), file);
            fflush(file);
            fclose(file);
        }

        /**
         * Deserializes data from a binary file and stores it into the specified `wmemory_t` buffer.
         *
         * @param buffer A pointer to the `wmemory_t` object where the deserialized data will be stored.
         * @param filename The name of the binary file to read the data from.
         *
         * @throws std::runtime_error If the file cannot be opened.
         */
        void deserialize(wmemory_t *buffer, const char *filename) {
            std::ifstream file(filename, std::ios::binary);
            if (file.is_open()) {
                file.seekg(0x0, std::ios::end);
                std::streamsize size = file.tellg();
                file.seekg(0x0, std::ios::beg);
                uint8_t *data = (uint8_t *) _STD malloc(size);
                file.read(reinterpret_cast<char *>(data), size);
                file.close();
                buffer->reserve(data, size);
                std::free(data);
            } else throw std::runtime_error("failed to open file");
        }
    }

    // Compile-time check for C++ version; if not C++20 or later, emit an error.
#if __cplusplus < 201703L
    #error "C++17 or later is required for this string format function."
#else
    namespace detail {
        inline void format_helper(std::stringstream &ss, const std::string &format) {
            ss << format; // Base case: no more placeholders
        }

        template<typename T, typename... Args>
        void format_helper(std::stringstream &ss, const std::string &format, T value, Args... args) {
            size_t pos = format.find("{}");
            if (pos == std::string::npos) {
                throw std::invalid_argument("Too many arguments provided to format string.");
            }
            ss << format.substr(0, pos) << value;
            format_helper(ss, format.substr(pos + 2), args...);
        }
    }

    template<typename... Args>
    std::string format(const std::string &fmt, Args... args) {
        std::stringstream ss;
        detail::format_helper(ss, fmt, args...);
        return ss.str();
    }
#endif

    template<typename IntegerType>
    std::string group_digit(IntegerType number, char separator = ',') {
        // Buffer to hold the resulting string
        char buffer[50];
        // Buffer to hold the temporary number
        char temp[50];

        // Handle potential negative number
        bool is_negative = (number < 0);
        if (is_negative) {
            number = -number;
        }

        int idx = 0;
        int len = 0;

        // Convert number to string in reverse
        do {
            temp[len++] = '0' + number % 10;
            number /= 10;
        } while (number > 0);

        // Now insert separators while copying temp to buffer in proper order
        int group_count = 0;
        for (int i = 0; i < len; ++i) {
            if (group_count == 3) {
                buffer[idx++] = separator;
                group_count = 0;
            }
            buffer[idx++] = temp[len - 1 - i];
            ++group_count;
        }

        // Add minus sign if the number was negative
        if (is_negative) {
            buffer[idx++] = '-';
        }

        // Reverse the buffer to get the final string
        for (int i = 0; i < idx / 2; ++i) {
            std::swap(buffer[i], buffer[idx - 1 - i]);
        }

        // Null-terminate the string
        buffer[idx] = '\0';

        return std::string(buffer);
    }

    constexpr size_t HEAP_SIZE = 1024 * 1024; // 1 MB
    constexpr size_t ALIGNMENT = 8;

    // Align utility
    inline size_t align(size_t size) {
        return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
    }

    // Block header structure
    struct BlockHeader {
        size_t size;
        bool is_free;
        BlockHeader *next;
    };

    // Static memory pool
    alignas(ALIGNMENT) uint8_t heap[HEAP_SIZE];
    BlockHeader *free_list = reinterpret_cast<BlockHeader *>(heap);

    // Initialize the free list
    void initialize() {
        // Securely initialize heap memory to avoid undefined behavior
        std::memset(heap, 0, HEAP_SIZE);

        free_list->size = HEAP_SIZE - sizeof(BlockHeader);
        free_list->is_free = true;
        free_list->next = nullptr;
    }

    /**
     * Allocates a block of memory of the specified size.
     *
     * @param size The size of the memory block to allocate. Must be greater than zero.
     *
     * @return A pointer to the allocated memory block. Returns `nullptr` if the size is zero or if no suitable free block is found.
     */
    void *alloc_(size_t size) {
        static bool initialized = false;
        if (!initialized) {
            initialize();
            initialized = true;
        }

        if (size == 0) {
            return nullptr;
        }

        size = align(size);

        BlockHeader *current = free_list;
        BlockHeader *previous = nullptr;

        // Find a free block of sufficient size
        while (current != nullptr) {
            if (current->is_free && current->size >= size) {
                // Split the block if it's larger than needed
                if (current->size >= size + sizeof(BlockHeader) + ALIGNMENT) {
                    BlockHeader *new_block = reinterpret_cast<BlockHeader *>(
                        reinterpret_cast<uint8_t *>(current) + sizeof(BlockHeader) + size);
                    new_block->size = current->size - size - sizeof(BlockHeader);
                    new_block->is_free = true;
                    new_block->next = current->next;

                    current->size = size;
                    current->next = new_block;
                }

                current->is_free = false;
                return reinterpret_cast<void *>(reinterpret_cast<uint8_t *>(current) + sizeof(BlockHeader));
            }

            previous = current;
            current = current->next;
        }
        return nullptr;
    }

    /**
     * Frees the memory block pointed to by `ptr` and marks it as free,
     * allowing it to be reused in future memory allocations.
     *
     * @param ptr Pointer to the memory block to be freed. If `ptr` is `nullptr`, the function does nothing.
     */
    void free_(void *ptr) {
        if (ptr == nullptr) {
            return;
        }

        BlockHeader *block = reinterpret_cast<BlockHeader *>(
            reinterpret_cast<uint8_t *>(ptr) - sizeof(BlockHeader));
        block->is_free = true;

        // Coalesce contiguous free blocks
        BlockHeader *current = free_list;
        while (current != nullptr) {
            BlockHeader *next = current->next;
            if (next != nullptr && next->is_free) {
                // Combine current and next blocks
                current->size += next->size + sizeof(BlockHeader);
                current->next = next->next;
            } else {
                current = next;
            }
        }
    }
}
#endif
#endif //SERIALIZER_H

























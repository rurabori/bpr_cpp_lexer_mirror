#ifndef CTLE_FILE
#define CTLE_FILE

#include "range.h"

#include <filesystem>
#include <exception>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace ctle {

    template<typename CharT>
    class basic_file {
        int m_fd{0};
        utils::range<CharT> m_data; // default initialized by itself.
    public:
        using char_t = CharT;
        using iterator_t = const char_t*;
        /**
         * @brief Construct a file, opens the file, throws if cannot open or cannot map.
         * 
         * @param path the path to the file.
         */
        basic_file(const std::filesystem::path& path);
        /**
         * @brief Construct an empty file object. Needs initialize to be called.
         * 
         */
        basic_file() noexcept = default;

        ~basic_file();
        /**
         * @brief initializes the object. Should mimick the constructor argument wise.
         * 
         * @param path the path to file.
         * @return true if success, false otherwise.
         */
        bool initialize(const std::filesystem::path& path) noexcept;
        /**
         * @brief creates a file and initializes it.
         * 
         * @tparam Args variadic.
         * @param args to pass to initialize.
         * @return std::unique_ptr<basic_file> if success, false otherwise. 
         */
        template<typename... Args>
        static std::unique_ptr<basic_file> create(Args&&... args);

        // accessors
        const CharT* begin() const noexcept { return m_data.data; }

        const CharT* end() const noexcept { return m_data.data + m_data.size; }

        size_t size() const noexcept { return m_data.size; }
    private:
        bool open_file(const std::filesystem::path& path) noexcept;
        bool map_memory() noexcept;
    };

    // implementation

    template <typename CharT>
    basic_file<CharT>::basic_file(const std::filesystem::path& path) {
        if (!open_file(path))
            throw std::runtime_error("File could not be opened.");
        
        if (!map_memory())
            throw std::runtime_error("File could not be mapped.");
    }

    template <typename CharT>
    basic_file<CharT>::~basic_file() {
        if (m_data.data) munmap(m_data.data, m_data.size);
        close(m_fd); // we dont care if fd is bad.
    }

    template <typename CharT>
    bool basic_file<CharT>::initialize(const std::filesystem::path& path) noexcept {
        return open_file(path) && map_memory();
    }

    template<typename CharT>
    template<typename... Args>
    std::unique_ptr<basic_file<CharT>> basic_file<CharT>::create(Args&&... args){ 
        auto retval = std::make_unique<basic_file>(std::forward<Args>(args)...);
        return (retval->initialize(std::forward<Args>(args)...)) ? std::move(retval) : nullptr;
    }

    template <typename CharT>
    bool basic_file<CharT>::open_file(const std::filesystem::path& path) noexcept {
        m_fd = open(path.c_str(), O_RDONLY); 
        if (!m_fd) return false;

        std::error_code err;
        // get size in CharT. (file_size reports in bytes).
        m_data.size = std::filesystem::file_size(path, err) / sizeof(CharT);

        return !(err);
    }

    template <typename CharT>
    bool basic_file<CharT>::map_memory() noexcept {
        return (m_data.data = (CharT*)mmap(NULL, m_data.size, PROT_READ, MAP_PRIVATE | MAP_FILE | MAP_POPULATE, m_fd, 0)) != nullptr;
    }
}

#endif

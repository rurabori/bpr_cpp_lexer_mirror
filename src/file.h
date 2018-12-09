#ifndef CTLE_FILE
#define CTLE_FILE
#include <filesystem>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <exception>

namespace ctle {
    template<typename CharT>
    class basic_file {
    public:
        using char_type = CharT;
        basic_file(const std::filesystem::path& path);
        ~basic_file();
        const CharT* begin() const noexcept { return m_data; }
        const CharT* end() const noexcept { return m_data + m_size; }
        // mmap reports size one bigger than I expect...
        size_t size() const noexcept { return m_size - 1; }
    private:
        bool open_file(const std::filesystem::path& path) noexcept;

        int m_fd;
        size_t m_size;
        CharT* m_data;
        size_t m_offset{0};
    };

    template <typename CharT>
    basic_file<CharT>::basic_file(const std::filesystem::path& path) {
        if (!open_file(path))
            throw std::runtime_error("File could not be opened.");
        
        m_data = (CharT*)mmap(NULL, m_size, PROT_READ, MAP_PRIVATE | MAP_FILE | MAP_POPULATE, m_fd, 0);
        if (!m_data)
            throw std::runtime_error("File could not be mapped.");
    }

    template <typename CharT>
    basic_file<CharT>::~basic_file() {
        munmap(m_data, m_size);
        close(m_fd);
    }

    template <typename CharT>
    bool basic_file<CharT>::open_file(const std::filesystem::path& path) noexcept {
        m_fd = open(path.c_str(), O_RDONLY); 
        if (!m_fd)
            return false;

        struct stat s;
        int status = fstat(m_fd, &s);
        if (status)
            return false;

        m_size = s.st_size;
        return true;
    }
}

#endif
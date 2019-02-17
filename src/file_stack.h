#ifndef CTLE_FILE_STACK
#define CTLE_FILE_STACK
#include "file.h"
#include <vector>
#include <iostream>

namespace ctle {

    template <typename FileType>
    class basic_file_stack
    {
        using element_t = std::pair<std::unique_ptr<FileType>, size_t>;
        std::vector<element_t> m_files;
    public:
        using file_t = FileType;
        using char_t = typename file_t::char_t;
        using iterator_t = typename file_t::iterator_t;

        struct file_range {
            iterator_t begin{};
            iterator_t end{};
        };

        /**
         * @brief tries to add a file to the stack, if it fails, nothing changes stack_wise.
         * 
         * @tparam Args
         * @param constructor_args forwarded to FileType creator.
         * @return true if success false otherwise.
         */
        template <typename... Args>
        bool push(Args&&... constructor_args) { 
            auto new_file = FileType::create(std::forward<Args>(constructor_args)...);

            if (!new_file) return false;
            m_files.emplace_back(std::move(new_file), 0);
            
            return true;
        }
        /**
         * @brief stores the current position in the file on top of the stack.
         * 
         * @param current_position an iterator, that MUST belong to the file at the top of the stack.
         */
        void store(iterator_t current_position) {
            if (!m_files.empty()) {
                auto& current = m_files.back();
                current.second = std::distance(current.first->begin(), current_position);            
            }
        }
        /**
         * @brief checks if stack is empty.
         */
        bool empty() const noexcept { return m_files.empty(); }
        /**
         * @brief removes a file from the stack.
         */
        void pop() { m_files.pop_back(); }
        /**
         * @brief returns the file on top of the stack, in the state that it was last stored (if offset was stored).
         * 
         * @return file_range, if stack is empty, an empty range is returned.
         */
        file_range top() { 
            if (m_files.empty()) return file_range{};

            auto& current = m_files.back();

            return file_range{current.first->begin() + current.second, current.first->end()};
        }

    };

}
#endif

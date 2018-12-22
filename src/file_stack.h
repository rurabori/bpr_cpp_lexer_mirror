#ifndef CTLE_FILE_STACK
#define CTLE_FILE_STACK
#include "file.h"
#include "stack_iterator.h"
#include <vector>

namespace ctle {

    template <typename file_type>
    class basic_file_stack
    {
    public:
        using char_type = typename file_type::char_type;
        using iterator = stack_iterator<basic_file_stack<file_type>>;
        
        template <typename... Args>
        bool push(Args&&... constructor_args) { m_files.push_back(std::make_unique<file_type>(std::forward<Args>(constructor_args)...)); return true; }
        iterator begin() noexcept { return iterator(this, m_files.size() - 1); }
        iterator end() noexcept { return iterator(this, -1); }



        void pop() { m_files.pop_back(); }
        file_type* top() { return m_files.back().get(); }
        file_type* operator[](size_t idx) const { 
            return m_files[idx].get(); 
        }
    private:
        std::vector<std::unique_ptr<file_type>> m_files;
    };

}
#endif

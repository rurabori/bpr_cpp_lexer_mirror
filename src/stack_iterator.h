#ifndef CTLE_STACK_ITERATOR
#define CTLE_STACK_ITERATOR

namespace ctle {
    template <typename stack>
    class stack_iterator {
    public:
        using difference_type = ptrdiff_t;
        using value_type = typename stack::char_type;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = std::bidirectional_iterator_tag;

        stack_iterator() = default;
        stack_iterator(stack* parent, int64_t file_index = 0) : m_parent(parent), m_file_index(file_index) {}
        
        stack_iterator& operator=(const stack_iterator& other) {
            m_parent = other.m_parent;
            m_file_offset = other.m_file_offset;
            m_file_index = other.m_file_index;
            return *this;
        }

        stack_iterator& operator++() {
            ++m_file_offset;
            if (m_file_offset > parent()[m_file_index]->size()) {
                 m_file_offset = 0;
                 --m_file_index;
            }

            return *this;
        }
        
        stack_iterator& operator--() {
            if (!m_file_offset) {
                ++m_file_index;
                m_file_offset = parent()[m_file_index]->size();
            } else {
                --m_file_offset;
            }

            return *this;
        }

        stack_iterator& operator+=(int n) {
            if (n > 0) while(n--) ++(*this);
            else while (n++) --(*this);

            return *this;
        }

        stack_iterator& operator-=(int n) {
            return operator+=(-1*n);
        }

        stack_iterator operator+(int n) {
            stack_iterator retval{*this};
            return retval += n;
        }

        stack_iterator operator-(int n) {
            return operator-(-1*n);
        }

        const value_type& operator*() const noexcept {
            if (!m_parent)
                return *"";
            return *(parent()[m_file_index]->begin() + m_file_offset);
        }


        friend bool operator==(const stack_iterator& lhs, const stack_iterator& rhs) {
            return (lhs.m_file_index == rhs.m_file_index && lhs.m_file_offset == rhs.m_file_offset);
        }

        friend bool operator!=(const stack_iterator& lhs, const stack_iterator& rhs) {
            return !(lhs == rhs);
        }
    private:
        stack& parent() const noexcept { return *m_parent; }
        stack* m_parent{nullptr};
        int64_t m_file_offset{0};
        int64_t m_file_index{-1}; // END BY DEFAULT
    };
}
#endif // CTLE_STACK_ITERATOR
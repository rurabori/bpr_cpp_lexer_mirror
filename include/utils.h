#ifndef CTLE_UTILS
#define CTLE_UTILS
#include "states.h"
#include "callable.h"

#include <ctll/list.hpp>
#include <ctll/fixed_string.hpp>
#include <ctll/utilities.hpp>
#include <array>

namespace ctle {
/**
 * @brief the operator used in fold expression for filters.
 *
 * @tparam Start the rules that have been matched until this one.
 * @tparam Next a list with the next rule or an empty list if this rule doesn't belong
 * @param start ... same as TP
 * @param next ... same as TP
 * @return the list of rules filtered so far.
 */
template<typename... Start, typename Next>
constexpr auto operator+(ctll::list<Start...> start, Next next) {
    return ctll::concat(start, next);
}
/**
 * @brief std::find marked as non-constexpr by my compiler, workaround.
 *
 * @tparam array of elements to be searched.
 * @param is_in the element to search for
 * @return true if contains else false.
 */
template<std::array array>
constexpr bool contains(auto is_in) {
    for (auto x : array)
        if (static_cast<int>(x) == static_cast<int>(is_in)) return true;

    return false;
}

namespace detail {
    // glorified std::apply which allows for varargs before|| THE VARARGS ARE APPLIED BEFORE TUPLE
    template<callable Callable, typename... Args, typename Tuple, size_t... idx>
    static constexpr CTLL_FORCE_INLINE auto apply_tuple(Tuple&& tuple, std::index_sequence<idx...>,
                                                        Args&&... args) {
        return Callable(std::forward<Args>(args)..., std::get<idx>(tuple)...);
    }
    // yes this only swaps pair right now, not needed for anything else atm.
    template<typename Ty>
    static constexpr void swap(Ty&& a, Ty&& b) {
        auto tmp = std::move(a);
        a.first = b.first;
        a.second = b.second;
        b.first = tmp.first;
        b.second = tmp.second;
    }
} // namespace detail

// glorified std::apply which allows for varargs before || THE VARARGS ARE APPLIED BEFORE TUPLE
template<callable Callable, size_t NArgs, typename... Args, typename Tuple>
static constexpr CTLL_FORCE_INLINE auto apply_tuple(Tuple&& tuple, Args&&... args) {
    return detail::apply_tuple<Callable>(std::move(tuple), std::make_index_sequence<NArgs>(),
                                         std::forward<Args>(args)...);
}

// just a bubble sort, not expecting long sequences of states.
template<typename Indexable, typename Comparator>
static constexpr auto sort(Indexable& to_sort, Comparator&& compare) {
    for (auto i = 0; i < to_sort.size(); ++i) {
        for (auto j = 0; j < to_sort.size() - i - 1; ++j) {
            if (!compare(to_sort[j], to_sort[j + 1])) detail::swap(to_sort[j], to_sort[j + 1]);
        }
    }
}

template<size_t X, size_t Y>
static constexpr bool operator==(ctll::fixed_string<X> first, ctll::fixed_string<Y> second) {
    if (X != Y) return false;
    for (size_t i = 0; i < X; ++i)
        if (first[i] != second[i]) return false;
    return true;
}

template<ctll::fixed_string... input>
static constexpr auto concat() {
    static_assert(sizeof...(input), "Concatenating no strings makes no sense.");
    using char_t = decltype((input[0], ...));

    char_t  buffer[(1 + ... + input.size())] = {};
    char_t* dest = &buffer[0];

    const auto loop_body = [&dest](const auto& str) {
        for (auto c : str) *(dest++) = c;
    };

    (loop_body(input), ...);

    return ctll::fixed_string(buffer);
}

namespace capture {
    /**
     * @brief Used in a fold expression to count captures in a regex.
     *
     */
    class count
    {
        size_t m_count;

    public:
        /**
         * @brief Constructor.
         *
         */
        constexpr count(size_t i = 1) : m_count{i} {}
        /**
         * @brief binop used in fold expression.
         *
         * @param other another counter.
         * @return the one with more captures.
         */
        constexpr count operator|(count other) { return (m_count < other.m_count) ? other : *this; }
        /**
         * @brief user defined conversion
         *
         */
        constexpr operator size_t() { return m_count; }
    };

    /**
     * @brief Gets the number of captures in a regex.
     *
     * @tparam Rule the rule containing the regex.
     * @tparam IBegin begin iterator.
     * @tparam IEnd end iterator
     * @return number of captures.
     */
    template<typename Rule, typename IBegin, typename IEnd>
    static constexpr size_t get_num_captures() {
        return std::tuple_size_v<decltype(Rule::match(IBegin{}, IEnd{}))>;
    }
    /**
     * @brief returns the maximum number of captures of any rule.
     *
     * @tparam IBegin begin iterator.
     * @tparam IEnd end iterator.
     * @tparam Rule argpack of rules.
     * @return size_t the maximum number of captures of any rule.
     */
    template<typename IBegin, typename IEnd, typename... Rule>
    static constexpr size_t max(ctll::list<Rule...>) {
        return (count{} | ... | count{get_num_captures<Rule, IBegin, IEnd>()});
    }
} // namespace capture
/**
 * @brief a small struct encapsulating the input to a lexer.
 *
 * @tparam IteratorT the iterator type.
 */
template<typename IteratorT>
struct input_range
{
    IteratorT begin;
    IteratorT end;
};
} // namespace ctle
#endif // CTLE_UTILS
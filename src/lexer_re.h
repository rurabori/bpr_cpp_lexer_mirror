#ifndef CTLE_LEXER_RE
#define CTLE_LEXER_RE
#include "ctre/wrapper.hpp"

namespace ctle {


    // copied from CTRE, just doesn't assert end of string as we need to match only on start on string(or rather search).
    template <typename Iterator, typename EndIterator, typename Pattern> 
    constexpr inline auto match_start(const Iterator begin, const EndIterator end, Pattern pattern) noexcept {
        using namespace ctre;
        using return_type = decltype(regex_results(std::declval<Iterator>(), find_captures(pattern)));
        return evaluate(begin, begin, end, return_type{}, ctll::list<start_mark, Pattern, end_mark, accept>());
    }

    template <typename RE>
    struct regular_expression : public ctre::regular_expression<RE>
    {
        constexpr CTRE_FORCE_INLINE regular_expression() noexcept : ctre::regular_expression<RE>() { };
        constexpr CTRE_FORCE_INLINE regular_expression(RE) noexcept : ctre::regular_expression<RE>() { };

        template <typename IteratorBegin, typename IteratorEnd> constexpr CTRE_FORCE_INLINE static auto match_relaxed(IteratorBegin begin, IteratorEnd end) noexcept {
            return match_start(begin, end, RE());
        }        
    };

    template <typename RE> regular_expression(RE) -> regular_expression<RE>;
}

#endif // CTLE_LEXER_RE
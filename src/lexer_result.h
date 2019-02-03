#ifndef CTLE_LEXER_RESULT
#define CTLE_LEXER_RESULT

#include <variant>

namespace ctle {
    /**
     * @brief holds a result of match operation
     * 
     * @tparam ResultVariant std::variant of all possible result types for a lexer.
     * @tparam ActionSignature signature of an "action function" provided by lexer.
     */
    template <typename ResultVariant, typename ActionSignature>
    struct lexer_result {
        ResultVariant   match;
        ActionSignature action{nullptr};
        /**
         * @brief returns a view of held match.
         */
        auto to_view() const noexcept {
            return std::visit([](const auto& val){ return val.to_view(); }, match);
        }
        /**
         * @brief returns length of a match.
         */
        auto length() const noexcept {
            return to_view().length();
        }
        /**
         * @brief used in fold expression by lexer.
         * 
         * @param other other rule.
         * @return lexer_result the rule which matched more text.
         */
        lexer_result operator|(lexer_result other) const noexcept {
            return (length() < other.length()) ? other : *this;
        }
    };
} // namespace ctle

#endif // CTLE_LEXER_RESULT
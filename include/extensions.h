#ifndef CTLE_DERIVES_FROM
#define CTLE_DERIVES_FROM
namespace ctle {
/**
 * @brief A wrapper for the extensions to allow multiple of them.
 *
 * @tparam Derivation All of the extensions to be used in a lexer, which must take exactly one
 * template parameter. They need not be derived from ctle::extension.
 */
template<template<typename> typename... Derivation>
struct extensions
{
    /**
     * @brief actual structure handling CRTP.
     *
     * @tparam Ty The type of lexer.
     */
    template<typename Ty>
    struct inner : Derivation<Ty>...
    {};
};
/**
 * @brief A base class all extensions to the lexer should use as base if they need to use the lexer
 * interface itself. If not this can be ommited.
 *
 * @tparam LexerT the type of the lexer in which the extension is used (CRTP).
 */
template<typename LexerT>
class extension
{
protected:
    // accessors for const/non-const methods/calls.
    LexerT&       lexer() noexcept { return reinterpret_cast<LexerT&>(*this); }
    const LexerT& lexer() const noexcept { return reinterpret_cast<const LexerT&>(*this); }
};

} // namespace ctle
#endif // CTLE_DERIVES_FROM

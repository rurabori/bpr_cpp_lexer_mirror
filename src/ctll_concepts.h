#ifndef CTLL_CONCEPTS
#define CTLL_CONCEPTS
#include <filesystem>

template <typename Lexer>
concept bool LexerInterface = requires(Lexer lexer) {
	{ lexer.add_file(std::filesystem::path("test")) } -> bool;
    { lexer.set_state(0) } -> bool;
    typename Lexer::string_view_t;
};

template <typename Functor, typename Lexer>
concept bool LexingAction = requires(Functor fctor, Lexer lexer) {
    { fctor(std::string_view(), lexer) } -> std::string_view;
};
#endif // CTLL_CONCEPTS

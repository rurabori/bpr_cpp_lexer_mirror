#ifndef CTLE_ACTIONS
#define CTLE_ACTIONS

#include "ctle_concepts.h"

#include <optional>

/**
 *	An action in this lexer is just a class/struct with a static method named execute. 
 *	Hence this macro, which lets us define the simple, non-templated cases somewhat easier.
 *  The reason it is a class/struct is, that we cannot pass templated functions as template template parameters,
 * 	but classes with templated methods are OK.
 * 
 */

#define create_action(name, return_type, definition) struct name { static return_type execute definition }

namespace ctle::default_actions {
    

	create_action(echo, 
		void, (LexerInterface& b, auto&&... all) {
            (std::cout << ... << all.to_view()) << '\n';
        }
	);

    template<typename ReturnType>
	class eof {
	public:
		static std::optional<ReturnType> execute(LexerInterface& lexer) {
			std::optional<ReturnType> retval{};

			if (!lexer.pop_file())
				retval.emplace(ReturnType::eof);

			return retval;
		}
	};

    template<typename ReturnType>
	class no_match {
	public:
		static ReturnType execute(LexerInterface& lexer) {
			return ReturnType::no_match;
		}
	};
} // namespace ctle
#endif //CTLE_ACTIONS
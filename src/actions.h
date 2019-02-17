#ifndef CTLE_ACTIONS
#define CTLE_ACTIONS

#include "ctle_concepts.h"

#include <optional>

/**
 *	An action in this lexer is just a class/struct with defined operator(). 
 *	Hence this macro, which lets us define the simple, non-templated cases somewhat easier.
 *  The reason it is a class is, that we cannot pass functions as template template parameters,
 * 	but classes with templated methods are OK.
 * 
 */

#define create_action(name, return_type, definition) struct name { return_type operator() definition }

namespace ctle::default_actions {
    

	create_action(echo, 
		void, (LexerInterface& b, auto&&... all) {
            (std::cout << ... << all.to_view()) << '\n';
        }
	);

    template<typename ReturnType>
	class eof {
	public:
		std::optional<ReturnType> operator()(LexerInterface& lexer) {
			std::optional<ReturnType> retval{};

			if (!lexer.pop_file())
				retval.emplace(ReturnType::eof);

			return retval;
		}
	};

    template<typename ReturnType>
	class no_match {
	public:
		ReturnType operator()(LexerInterface& lexer) {
			return ReturnType::no_match;
		}
	};
} // namespace ctle
#endif //CTLE_ACTIONS
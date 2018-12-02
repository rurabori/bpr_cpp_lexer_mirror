#ifndef CTLE_LEXER
#define CTLE_LEXER

#include "lexer_rule.h"
#include "ctre.hpp"
#include "utils.h"
#include "ctll/list.hpp"
#include <iostream>


template <typename Tokens, typename rules = ctll::list<>>
class lexer
{
	enum class internal_states;
public:

	constexpr auto match(std::string_view to_match) {
		return match(to_match.begin(), to_match.end(), rules());
	}
	
	template <typename IBegin, typename IEnd>
	auto match (IBegin begin, IEnd end) {
		while (begin != end) {
			
			auto match_res = match_inner(begin, end);
			if (match_res){
				std::advance(begin, m_current_match.length());
				if (m_state == internal_states::ignore)
					continue;
				std::cout << "." << m_current_match << "." << std::endl;
			} else {
				std::cout << "empty" << std::endl;
				break;
			}
		}

		return std::pair{".", Tokens::unknown};
	}

	template <typename IBegin, typename IEnd>
	auto CTRE_FORCE_INLINE match_inner (IBegin begin, IEnd end) {
		
		set_state("", static_cast<Tokens>(0), internal_states::normal);
		return match(begin, end, rules());
	}

	template <typename IBegin,typename IEnd>
	constexpr bool CTRE_FORCE_INLINE match(IBegin begin, IEnd end,ctll::list<>) noexcept {			
		return false;
	}

	template <typename IBegin,typename IEnd, typename rule, typename... others>
	constexpr bool CTRE_FORCE_INLINE match(IBegin begin, IEnd end,ctll::list<rule, others...>) noexcept {			
		auto r = rule();
		auto matched = r.match(begin, end);
		
		if (matched.length() > m_current_match.length()) {
			internal_states state;
			if constexpr (std::is_same<typename rule::token_type, nullptr_t>())
				state = internal_states::ignore;
			else
				state = internal_states::normal;

			set_state(matched, static_cast<Tokens>(r.get_token()), state);
		}

		return (matched.length() != 0 || match(begin, end, ctll::list<others...>()));
	}	


private:
	constexpr CTRE_FORCE_INLINE void set_state(std::string_view match, Tokens token, internal_states state) {
		m_current_match = match;
		m_current_token = token;
		m_state = state;
	}

	enum class internal_states {
		normal,
		ignore
	};

	std::string_view 	m_current_match;
	Tokens 				m_current_token;
	internal_states 	m_state{internal_states::normal};

};
#endif // CTLE_LEXER
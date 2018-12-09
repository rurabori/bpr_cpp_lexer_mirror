#ifndef CTLE_LEXER
#define CTLE_LEXER

#include "lexer_rule.h"
#include "ctre.hpp"
#include "utils.h"
#include "ctll/list.hpp"
#include <iostream>
#include "ctll_concepts.h"

#include "file_stack.h"

template <typename Tokens, typename rules = ctll::list<>>
class lexer
{
	enum class internal_states;
public:
	using sw_type = std::basic_string_view<char>;
	using stack_type = ctle::basic_file_stack<ctle::basic_file<char>>;

	auto lex() {
		while (m_begin != m_end) {
			auto match_res = match_inner(m_begin, m_end);
			if (match_res) {
				if (m_state != internal_states::ignore)
					return std::pair{m_current_match, m_current_token};
			} else {
				break;
			}
		}

		return std::pair{std::string_view(), Tokens::unknown};
	}

	template <typename IBegin, typename IEnd>
	auto CTRE_FORCE_INLINE match_inner (IBegin begin, IEnd end) {
		
		set_state("", static_cast<Tokens>(0), internal_states::normal);
		return match(begin, end, rules());
	}

	template <typename matched_rule = nullptr_t, typename IBegin,typename IEnd>
	bool CTRE_FORCE_INLINE match(IBegin begin, IEnd end,ctll::list<>) noexcept {
		
		// if we get here we already have our match so we move our iterator and then execute a function if user provided any.
		std::advance(m_begin, m_current_match.length());
		// the constexpr if did a doo doo while evaluating && ... so that's why the nested ifs. not pretty. works tho.
		if constexpr (!std::is_same<matched_rule, std::nullptr_t>()) {
			using action_t = typename matched_rule::action;
			if constexpr(!std::is_same<action_t, std::nullptr_t>()) {
				static_assert(LexingAction<action_t, decltype(*this)>, "Function signature does not match.");
				m_text = action_t()(m_current_match, *this);	
			} else {
				m_text = m_current_match;
			}
		}

		return false;
	}

	template <typename matched_rule = nullptr_t, typename IBegin,typename IEnd, typename rule, typename... others>
	bool CTRE_FORCE_INLINE match(IBegin begin, IEnd end,ctll::list<rule, others...>) noexcept {			
		auto r = rule();
		auto matched = r.match(begin, end);
		bool next = false;

		if (matched.length() > m_current_match.length()) {
			internal_states state;
			if constexpr (std::is_same<typename rule::token_type, nullptr_t>())
				state = internal_states::ignore;
			else
				state = internal_states::normal;

			set_state(matched, static_cast<Tokens>(r.get_token()), state);
			next = match<rule>(begin, end, ctll::list<others...>());
		} else {
			next = match<matched_rule>(begin, end, ctll::list<others...>());
		}
		
		return (matched.length() != 0 || next);
	}	

	template <typename... Args>
	bool add_file(Args&&... c_args) {
		auto retval = m_stack.push(std::forward<Args>(c_args)...);
		if (!retval)
			return false;
		m_begin = m_stack.begin();
		m_end = m_stack.end();
		return true;
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

	stack_type m_stack{};
	stack_type::iterator m_begin{};
	stack_type::iterator m_end{};


	std::string m_text{100};
};



#endif // CTLE_LEXER
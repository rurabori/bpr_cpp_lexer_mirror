#ifndef CTLE_LEXER
#define CTLE_LEXER

#include "lexer_rule.h"
#include "ctre.hpp"
#include "utils.h"
#include "ctll/list.hpp"
#include <iostream>
#include <optional>
#include "ctll_concepts.h"
#include "wise_enum.h"
#include "file_stack.h"
#include <stack>

template <typename Id, typename Function>
struct c_pair{
	Id first;
	Function second;
};

template <	typename ReturnType,
			typename rules = ctll::list<>, 
			typename States = nullptr_t, 
			std::array StateDefinitions = {}> // because we need to know if they're exclusive etc.
class lexer
{
public:
	using state_t = States;
	using string_view_t = typename std::basic_string_view<char>;

private:
	using stack_t = ctle::basic_file_stack<ctle::basic_file<char>>;
	using file_iterator_t = typename stack_t::iterator;
	using match_t = bool;
	using match_signature = match_t (lexer::*)();
	using state_function_pair = c_pair<int, match_signature>;
	
	std::array<state_function_pair, StateDefinitions.size() + 1> m_state_functions{state_function_pair{0, nullptr}};
	std::string_view 	m_current_match;

	// file_stack
	stack_t m_stack{};
	file_iterator_t m_begin{};
	file_iterator_t m_end{};
	

	std::optional<ReturnType> m_return;


	match_t (lexer::* match_filtered)(){nullptr};	
	std::string_view m_text;


public:
	constexpr lexer() {
		static_assert(wise_enum::is_wise_enum_v<States>, "Not a wise choice my friend, use WISE_ENUM");
		fill_state_functions(std::make_index_sequence<StateDefinitions.size()>());	
		set_state(state_initial);
	}
	
	constexpr bool set_state(int state) {
		for (auto [s, f] : m_state_functions) {
			if (s == state) {
				match_filtered = f;
				return true;
			}
		}
		
		return false;
	}


	constexpr bool set_state(state_t state) {
		return set_state(static_cast<int>(state));
	}

	auto lex() {
		
		while (m_begin != m_end) {
			if (!(this->*match_filtered)()) {
				break;
			}
		}
		
		return std::pair{std::string_view(), ReturnType::unknown};
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
	template<auto index = 0> 
	constexpr void fsf_impl() {
		constexpr auto new_index = index + 1;
		static_assert(static_cast<int>(StateDefinitions[index].value) >= state_reserved, "All states must begin at state_reserved.");
		m_state_functions[new_index] = state_function_pair{static_cast<int>(StateDefinitions[index].value), &lexer::match_impl<decltype(filter<StateDefinitions[index].value>(rules()))>};
	}

	template <size_t... idx>
	constexpr void fill_state_functions(std::index_sequence<idx...>) {
		m_state_functions[0] = {state_initial, &lexer::match_impl<decltype(filter_initial<StateDefinitions>(rules()))>};
		(fsf_impl<idx>() , ...);
	}

	template <typename filtered_rules>
	match_t match_impl() {
		m_current_match = "";
		m_return.reset();
		return match(m_begin, m_end, filtered_rules());
	}

	template <typename matched_rule = std::nullptr_t, typename IBegin,typename IEnd>
	match_t CTRE_FORCE_INLINE match(IBegin begin, IEnd end,ctll::list<>) noexcept {
		// if we get here we already have our match so we move our iterator and then execute a function if user provided any.
		if constexpr (!std::is_same<matched_rule, std::nullptr_t>()) {
			std::advance(m_begin, m_current_match.length());
			// the constexpr if did a doo doo while evaluating && ... so that's why the nested ifs. not pretty. works tho.
			if constexpr(matched_rule::has_action()) {
				matched_rule::do_action(*this);
				m_text = m_current_match;
			} else {
				m_text = m_current_match;
			}
			return true;
		} else {
			return false;
		}
	}

	template <typename matched_rule = nullptr_t, typename IBegin,typename IEnd, typename rule, typename... others>
	match_t CTRE_FORCE_INLINE match(IBegin begin, IEnd end,ctll::list<rule, others...>) noexcept {			
		auto matched = rule::match(begin, end);
		bool next = false;

		if (matched.length() > m_current_match.length()) {
			m_current_match = matched;
			next = match<rule>(begin, end, ctll::list<others...>());
		} else {
			next = match<matched_rule>(begin, end, ctll::list<others...>());
		}
		
		return (matched.length() != 0 || next);
	}	
};

#endif // CTLE_LEXER
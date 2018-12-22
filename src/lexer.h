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
	using return_t = ReturnType;

	using action_signature = std::optional<ReturnType>(*)(lexer&);
	
private:
	using stack_t = ctle::basic_file_stack<ctle::basic_file<char>>;
	using file_iterator_t = typename stack_t::iterator;
	using match_t = std::optional<ReturnType>;
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
		return std::pair{"", (this->*match_filtered)()};		
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
		return match(filtered_rules());
	}

	template <typename... rule_list>
	match_t CTRE_FORCE_INLINE match(ctll::list<rule_list...>) noexcept {			

		while (m_begin != m_end) {
			auto [result, action] = (lexer_result<lexer, action_signature>{"", nullptr} | ... | rule_list::template match2<lexer>(m_begin, m_end));
			if (!result.size())
				break;
			
			std::advance(m_begin, result.size());

			if (action) {
				if (auto retval = action(*this); retval)
					return retval;
			}
		}

		return ReturnType::unknown;
	}	
};

#endif // CTLE_LEXER
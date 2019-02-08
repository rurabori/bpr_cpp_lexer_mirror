#ifndef CTLE_TYPE_FILTER
#define CTLE_TYPE_FILTER

#include "utils.h"

namespace ctle {
    namespace detail {
        /**
         * @brief just decides if two types are the same, if not returns the second one.
         * 
         * @tparam first type
         * @tparam second type
         * @return an empty list or a list with the second type only. 
         */
        template<typename first, typename second>
        static constexpr auto filter_elem() {
            if constexpr (!std::is_same_v<first, second>) {
                return ctll::list<second>{};
            } else {
                return ctll::list<>{};
            }
        }
        /**
         * @brief deletes all occurences of type from the list.
         * 
         * @tparam Type to be removed.
         * @tparam Types the list to remove from.
         * @return the list without Type occuring in it.
         */
        template<typename Type, typename... Types>
        static constexpr auto remove_type() {
            return (ctll::list<>{} + ... + filter_elem<Type, Types>());
        }
        /**
         * @brief this is where the filtering of non_unique types really happens.
         * 
         * @tparam Results the results so far.
         * @tparam Types the remaining types to be processed.
         * @tparam Type the current type being processed.
         * @param input the pair of proccessed/unproccessed types.
         * @return a pair of (unique_types, remaining_types).
         */
        using ctle::operator+;
        template <typename... Results, typename... Types, typename Type>
        constexpr auto operator+(std::pair<ctll::list<Results...>, ctll::list<Types...>> input, ctll::list<Type>) {
            // if no more types to judge, return what we have.
            if constexpr (!sizeof...(Types))
                return input;
            // take first type from remaining.
            using front = decltype(ctll::front(input.second));
            // check if it's the same as the one being judged.
            if constexpr (std::is_same_v<front, Type>) {
                // if same, add to the results and remove all other occurences from remaining types.
                return std::pair<ctll::list<Type, Results...>, decltype(detail::remove_type<Type, Types...>())>{};
            } else {
                // else return what we had, because the type at the front will come in some next iteration.
                return input;		
            }
        }
        /**
         * @brief takes a list of types, returns the list with only the unique rules
         * 
         * @tparam types the rules
         * @return the list with only the unique types inside
         */
        template<typename... Types>
        static constexpr auto remove_duplicates(ctll::list<Types...>) {
            return (std::pair<ctll::list<>, ctll::list<Types...>>{} + ... + ctll::list<Types>{}).first;
        }
    } // detail
  
    /**
     * @brief gets all unique types from a list of types and returns them in the desired container.
     * 
     * @tparam OutputType a template template parameter. a container into which the result is output.
     * @tparam List the rules to be "deduplicated".
     */
    template<template <typename> typename OutputType, typename List>
    using unique_from_list_to_type_t = list_to_type_t<OutputType, decltype(detail::remove_duplicates(std::declval<List>()))>;
} // ctle


#endif //CTLE_TYPE_FILTER
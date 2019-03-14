#ifndef CTLE_CALLABLE
#define CTLE_CALLABLE

namespace ctle {
	/**
	 * @brief 	A wrapper for a constexpr constructible callable object (this way we can accept lambdas).
	 * 			Meaning operator() is specified.
	 * 
	 * @tparam Ty (the type of object if nullptr_t it's an empty callable).
	 */
	template<typename Ty = nullptr_t>
	class callable {
		Ty m_callable;
	public:
		/**
		 * @brief checks if this callable is empty.
		 */
		static constexpr bool empty() { return std::is_same_v<Ty, std::nullptr_t>; }
		/**
		 * @brief constructs a callable from a callable object.
		 */
		constexpr callable(Ty call) : m_callable{std::move(call)} {}
		/**
		 * @brief copy constructor.
		 */
		constexpr callable(const callable& other) : callable{other.m_callable} {}
		/**
		 * @brief a wrapper which calls operator() of stored object.
		 * 
		 * @tparam Args forwarded argpack.
		 * @tparam std::enable_if_t<!empty()> SFINAE 
		 * @param args forwarded argpack.
		 * @return auto the return of stored object operator().
		 */
		template<typename... Args, typename = std::enable_if_t<!empty()>>
		auto operator()(Args&&... args) const { return m_callable(std::forward<Args>(args)...); }
	};
	// deduction guidelines.
	template<typename Ty> callable(Ty) -> callable<Ty>;
	template<typename Ty> callable(const callable<Ty>&) -> callable<Ty>;
	// represents an empty callable (if you don't want to specify an action for a rule fx.).
	static constexpr auto empty_callable = callable(nullptr);

	namespace callable_utils {
		/**
		 * @brief Get the default object.
		 * 
		 * @tparam New the compared.
		 * @tparam Default the default.
		 * @return constexpr auto  (if callable is empty returns the default otherwise returns new.
		 */
		template<callable New, callable Default>
		static constexpr auto get_default() { 
			static_assert(!Default.empty(), "Default cannot be an empty callable.");
			if constexpr (New.empty())
				return Default;
			else
				return New;
		}
	}
} // ctle

#endif // CTLE_CALLABLE
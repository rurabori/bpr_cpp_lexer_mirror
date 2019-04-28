#ifndef CTLE_RANGE
#define CTLE_RANGE

namespace ctle::utils {
/**
 * @brief	A small struct providing something of an "array_view".
 *
 * @tparam	T	Generic type parameter.
 */
template<typename T>
struct range
{
    T*     data{nullptr};
    size_t size{0};

    T* begin() noexcept { return data; }

    T* end() noexcept { return data + size; }

    const T* begin() const noexcept { return data; }

    const T* end() const noexcept { return data + size; }
};
} // namespace ctle::utils

#endif // CTLE_RANGE

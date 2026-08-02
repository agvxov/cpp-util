/* Swap-Pop vector, an std::vector variant,
 *  that on erase it swaps the target element with the last one and pops.
 * This results in significantly better erase performance,
 *  but ruins ordering.
 * This pattern is often useful in gamedev.
 */
#ifndef SWAPPOPP_VECTOR_HPP
#define SWAPPOPP_VECTOR_HPP

#include <vector>

template <typename T, typename Allocator = std::allocator<T>>
class swappop_vector : public std::vector<T, Allocator> {
  public:
    using std::vector<T, Allocator>::vector;

    typename std::vector<T, Allocator>::iterator erase(typename std::vector<T, Allocator>::iterator pos) {
        auto last = this->end() - 1;
        if (pos != last) {
            *pos = std::move(*last);
        }
        this->pop_back();
        return pos;
    }

    void erase_at(size_t index) {
        auto it = this->begin() + static_cast<std::ptrdiff_t>(index);
        erase(it);
    }
};

#endif

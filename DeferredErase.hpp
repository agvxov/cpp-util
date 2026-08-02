/* The following header is part of CPP-Util version c91b.
 * The upstream can be found at: https://github.com/agvxov/cpp-util
 * It is in the Public Domain.
 */
/* Deferred Erase is a class that helps you
 *  to delete elements from a container "during" iteration.
 * You mark() elements for deletion and they will be when
 *  DeferredErase goes out of scope or execute is invoked,
 */
#ifndef DEFERRED_ERASE_HPP
#define DEFERRED_ERASE_HPP

#include <vector>
#include <array>
#include <string>
#include <deque>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <utility>

namespace detail {
    template<typename T, typename A>
    std::true_type contiguous_test(const std::vector<T, A>*);

    template<typename T, std::size_t N>
    std::true_type contiguous_test(const std::array<T, N>*);

    template<typename T, typename Traits, typename A>
    std::true_type contiguous_test(const std::basic_string<T, Traits, A>*);

    std::false_type contiguous_test(const void*);
}

template<typename Container>
struct is_contiguous_container
: decltype(detail::contiguous_test(std::declval<Container*>())) {};

template<typename Container>
inline constexpr
bool is_contiguous_container_v = is_contiguous_container<Container>::value;

template<typename Container>
class DeferredErase {
  public:
    using iterator   = typename Container::iterator;
    using size_type  = typename Container::size_type;
    using value_type = typename Container::value_type;

    explicit DeferredErase(Container & c) : container_(c) {}

    ~DeferredErase() {
        execute();
    }

    // Mark an element for erasure by iterator. Requires random-access iterators.
    void mark(iterator it) {
        static_assert(
            std::is_same_v<
                typename std::iterator_traits<iterator>::iterator_category,
                std::random_access_iterator_tag
            >,
            "mark(iterator) requires a random-access container (e.g. std::vector)"
        );
        mark(static_cast<size_type>(std::distance(container_.begin(), it)));
    }

    // Mark an element for erasure by index.
    void mark(size_type index) {
        pending_.push_back(index);
    }

    // Mark by reference to an element (e.g. from `for (auto & p : container)`).
    // Requires a contiguous container (std::vector, std::array, std::basic_string,
    // or anything derived from them) so that pointer arithmetic recovers the index.
    void mark(const value_type& elem) {
        static_assert(
            is_contiguous_container_v<Container>,
            "mark(element) requires a contiguous container (vector/array/string-like); "
            "use mark(iterator) or mark(index) instead"
        );
        mark(static_cast<size_type>(&elem - &(*container_.begin())));
    }

    // Erase all marked elements. Safe to call multiple times / with no marks.
    void execute(void) {
        if (pending_.empty()) return;

        std::sort(pending_.begin(), pending_.end());
        pending_.erase(std::unique(pending_.begin(), pending_.end()), pending_.end());

        for (const auto& [first, last] : to_ranges(pending_)) {
            container_.erase(
                container_.begin() + static_cast<std::ptrdiff_t>(first),
                container_.begin() + static_cast<std::ptrdiff_t>(last + 1)
            );
        }
        pending_.clear();
    }

  private:
    // Collapses sorted, deduplicated indices into inclusive [first, last] ranges,
    // returned in reverse order so callers can erase back-to-front without
    // invalidating earlier indices.
    static std::vector<std::pair<size_type, size_type>> to_ranges(const std::vector<size_type>& sorted_indices) {
        std::vector<std::pair<size_type, size_type>> ranges;
        size_type range_start = sorted_indices.front();
        size_type range_end   = range_start;

        for (size_type i = 1; i < sorted_indices.size(); ++i) {
            if (sorted_indices[i] == range_end + 1) {
                range_end = sorted_indices[i];
            } else {
                ranges.emplace_back(range_start, range_end);
                range_start = range_end = sorted_indices[i];
            }
        }
        ranges.emplace_back(range_start, range_end);

        std::reverse(ranges.begin(), ranges.end());
        return ranges;
    }

    Container& container_;
    std::vector<size_type> pending_;
};

#endif

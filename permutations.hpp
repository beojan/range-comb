// -*-C++-*-
#ifndef __RCPERMUTATIONS_HPP_
#define __RCPERMUTATIONS_HPP_
#include <utility>
#include <vector>

#include <range/v3/algorithm/permutation.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/view/facade.hpp>
#include <range/v3/view/indirect.hpp>
#include <range/v3/view/iota.hpp>

namespace ranges::combinatorics {
CPP_template(typename Rng)(requires sized_range<Rng>&& random_access_range<
                           Rng>) class permutations_view
    : public view_facade<permutations_view<Rng>, finite> {
    using iter_t = iterator_t<const Rng>;
    using sent_t = sentinel_t<const Rng>;
    friend range_access;
    const Rng* rng;

    class cursor {
      private:
        friend range_access;
        friend permutations_view;
        int item; // Makes equality comparison much faster
        iter_t begin_;
        bool end_ = false;
        std::vector<iter_t> cur;

        cursor(iter_t begin, sentinel_t<const Rng> end)
            : item(0),
              begin_(begin),
              cur(ranges::to<std::vector>(views::iota(begin, end))) {}

      public:
        auto read() const { return cur | views::indirect; }

        void next() {
            end_ = !next_permutation(cur);
            ++item;
        }

        bool equal(const cursor& other) const {
            return begin_ == other.begin_ && item == other.item;
        }

        bool equal(default_sentinel_t) const { return end_; }

        cursor() = default;
    };

    cursor begin_cursor() const { return cursor(begin(*rng), end(*rng)); };

  public:
    permutations_view(const Rng& rng) : rng(&rng) {}
};

class permutations_fn {
    friend views::view_access;

  public:
    template <typename Rng>
    constexpr auto operator()(const Rng& rng) const
          -> CPP_ret(permutations_view<Rng>)(
                requires sized_range<Rng>&& random_access_range<Rng>) {
        return permutations_view<Rng>{rng};
    }
};

RANGES_INLINE_VARIABLE(views::view<permutations_fn>, permutations)
} // namespace ranges::combinatorics

#endif // __RCPERMUTATIONS_HPP_

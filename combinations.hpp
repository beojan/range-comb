// -*-C++-*-
#ifndef __RCCOMBINATIONS_HPP_
#define __RCCOMBINATIONS_HPP_
#include <functional>
#include <utility>
#include <vector>

#include <range/v3/range/concepts.hpp>
#include <range/v3/view/facade.hpp>
#include <range/v3/view/indirect.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>

namespace ranges::combinatorics {
CPP_template(typename Rng)(requires sized_range<Rng>&& random_access_range<
                           Rng>) class combinations_view
    : public view_facade<combinations_view<Rng>, finite> {
    using iter_t = iterator_t<const Rng>;
    friend range_access;
    ref_view<const Rng> rng;
    int k;

    class cursor {
      private:
        friend range_access;
        friend combinations_view;
        int item; // Makes equality comparison much faster
        int k;
        std::vector<int> cur;
        int j = 1;
        int x = 100000;
        bool end_ = false;
        ref_view<const Rng> rng;

        int& c(int i) {
            // To match convention in Knuth
            return cur[i - 1];
        }

      public:
        auto read() const {
            return cur | views::take(k)
                   | views::transform([this](int i) { return rng[i]; });
        }

        void next() {
            // Following Knuth 4a Algorithm T
            if (k == size(rng)) {
                end_ = true;
                ++item;
                return;
            }

            if (j > 0) {
                x = j;
                c(j) = x;
                --j;
                ++item;
                return;
            }

            if (c(1) + 1 < c(2)) {
                ++c(1);
                ++item;
                return;
            }

            j = 2;
            while (true) {
                c(j - 1) = j - 2;
                x = c(j) + 1;
                if (x == c(j + 1)) {
                    ++j;
                }
                else {
                    break;
                }
            };

            if (j > k) {
                end_ = true;
                ++item;
                return;
            }
            c(j) = x;
            --j;
            ++item;
            return;
        }

        bool equal(const cursor& other) const { return item == other.item; }

        bool equal(default_sentinel_t) const { return end_; }

        cursor() = default;
        cursor(ref_view<const Rng> rng, int k) : item(0), rng(rng), k(k) {
            if (k > size(rng)) {
                this->k = size(rng);
            }
            cur = views::iota(0, this->k) | to<std::vector>();
            j = this->k;
            cur.push_back(size(rng));
            cur.push_back(0);
        }
    };

    cursor begin_cursor() const { return cursor(rng, k); }

  public:
    combinations_view() = default;
    combinations_view(const combinations_view&) = default;
    combinations_view(combinations_view&&) = default;
    explicit combinations_view(const Rng& rng, int k)
        : rng(views::ref(rng)), k(k) {}
};

class combinations_fn {
    friend views::view_access;

  public:
    template <typename Rng>
    constexpr auto operator()(const Rng& rng, int k) const
          -> CPP_ret(combinations_view<Rng>)(
                requires sized_range<Rng>&& random_access_range<Rng>) {
        return combinations_view<Rng>{rng, k};
    }
};

RANGES_INLINE_VARIABLE(views::view<combinations_fn>, combinations)
} // namespace ranges::combinatorics

#endif // __RCCOMBINATIONS_HPP_

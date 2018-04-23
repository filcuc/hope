#ifndef INDEXSEQUENCE_H
#define INDEXSEQUENCE_H

#if __cplusplus < 201402L

#include <cstddef>

namespace std {

template <std::size_t ... T>
struct index_sequence {
};


template<std::size_t L, std::size_t ...R>
struct concat_index_sequence {
    using type = index_sequence<L, R...>;
};


template<std::size_t T>
struct index_sequence_builder {
    static constexpr std::size_t value() { return T; }
    using result = typename concat_index_sequence<index_sequence_builder<T-1>::value(), T>::type;
};


template<>
struct index_sequence_builder<0> {
    static constexpr std::size_t value() { return 0; }
    using result = index_sequence<0>;
};

template<std::size_t T>
using make_index_sequence = typename index_sequence_builder<T - 1>::result;

}

#else

#include <utility>

#endif

#endif

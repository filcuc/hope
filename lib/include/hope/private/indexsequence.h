#ifndef INDEXSEQUENCE_H
#define INDEXSEQUENCE_H

#if __cplusplus < 201402L

#include <cstddef>

namespace std {

template <std::size_t ... T>
struct index_sequence {
};

template<std::size_t K, std::size_t ...Remainder>
struct index_sequence_builder {
    using result = typename index_sequence_builder<K-1, K-1, Remainder...>::result;
};

template<std::size_t ...Remainder>
struct index_sequence_builder<0, Remainder...> {
    using result = index_sequence<Remainder...>;
};

template<std::size_t T>
using make_index_sequence = typename index_sequence_builder<T>::result;

}

#else

#include <utility>

#endif

#endif

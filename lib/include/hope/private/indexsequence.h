/*
    Copyright (C) 2018 Filippo Cucchetto.
    Contact: https://github.com/filcuc/hope

    This file is part of the Hope library.

    The Hope library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    The Hope library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the Hope library.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#if 0

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

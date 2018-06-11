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

#ifdef WIN32
#if !defined(HOPE_USE_STATIC) && !defined(HOPE_USE_SHARED)
#error "Specify if your building a static or dynamic library with HOPE_USE_STATIC or HOPE_USE_SHARED"
#endif

#ifdef HOPE_USE_STATIC
#define HOPE_API
#endif

#ifdef HOPE_USE_SHARED
#ifdef HOPE_EXPORT
#define HOPE_API __declspec(dllexport)
#else
#define HOPE_API __declspec(dllimport)
#endif
#endif
#else
#define HOPE_API
#endif
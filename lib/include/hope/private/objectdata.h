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

#include <hope/global.h>

#include <map>
#include <memory>
#include <mutex>
#include <thread>

namespace hope {

class Object;

namespace detail {

class ObjectData {
public:
    ObjectData(std::thread::id id)
        : m_thread_id(std::move(id))
    {}

    std::unique_lock<std::mutex> lock() {
        return lock(*this);
    }

    static std::unique_lock<std::mutex> lock(const std::shared_ptr<ObjectData>& data) {
        return data ? lock(*data) : std::unique_lock<std::mutex>();
    }

    static std::unique_lock<std::mutex> lock(ObjectData& data) {
        return std::unique_lock<std::mutex>(data.m_mutex);
    }

    static std::pair<std::unique_lock<std::mutex>,std::unique_lock<std::mutex>> lock(const std::shared_ptr<ObjectData>& first,
                                                                                     const std::shared_ptr<ObjectData>& second) {
        if (first && second)
            return lock(*first, *second);
        else if (first)
            return { lock(*first), std::unique_lock<std::mutex>() };
        else if (second)
            return { lock(*second), std::unique_lock<std::mutex>() };
        else
            return { std::unique_lock<std::mutex>(), std::unique_lock<std::mutex>() };
    }

    static std::pair<std::unique_lock<std::mutex>,std::unique_lock<std::mutex>> lock(ObjectData& first, ObjectData& second) {
        ObjectData* first_address = &first;
        ObjectData* second_address = &second;
        if (first_address >= second_address)
            std::swap(first_address, second_address);
        std::unique_lock<std::mutex> first_lock = lock(*first_address);
        std::unique_lock<std::mutex> second_lock;
        if (first_address != second_address)
            second_lock = lock(*second_address);
        return {std::move(first_lock), std::move(second_lock)};
    }

    std::thread::id m_thread_id;
    std::mutex m_mutex;
};

class ObjectDataRegistry {
public:
	HOPE_API static ObjectDataRegistry& instance();

    ObjectDataRegistry(const ObjectDataRegistry&) = delete;
    ObjectDataRegistry(ObjectDataRegistry&&) = delete;
    ObjectDataRegistry& operator=(const ObjectDataRegistry&) = delete;
    ObjectDataRegistry& operator=(ObjectDataRegistry&&) = delete;

	HOPE_API std::weak_ptr<ObjectData> data(Object* object) {
        auto it = m_data.find(object);
        return it != m_data.end() ? it->second : std::weak_ptr<ObjectData>();
    }

	HOPE_API void register_object_data(Object* object, const std::shared_ptr<ObjectData>& data) {
        m_data.emplace(object, data);
    }

	HOPE_API void unregister_object_data(Object* object) {
        m_data.erase(object);
    }

private:
    ObjectDataRegistry() = default;

    std::map<Object*, std::weak_ptr<ObjectData>> m_data;
};

}
}

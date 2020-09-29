#pragma once

#include <vector>
#include <numeric>
#include <cassert>

namespace TR {

    template<typename T>
        class two_dimensional_array {
            public:
                two_dimensional_array(const std::vector<std::vector<T>> a);
                two_dimensional_array() {}

                T& operator()(const size_t i, const size_t j);
                const T& operator()(const size_t i, const size_t j) const;

                size_t size() const;
                size_t size(const size_t i) const;

                void add_row();
                void push_back(const T& elem);

                template<typename ITERATOR>
                    void add_row(ITERATOR begin, ITERATOR end);

                auto begin(const size_t i) const { assert(i < size()); return data.begin() + offsets[i]; }
                auto end(const size_t i) const { assert(i < size()); return data.begin() + offsets[i+1]; }

            private:
                std::vector<size_t> offsets = {0};
                std::vector<T> data;
        };

    template<typename T>
    two_dimensional_array<T>::two_dimensional_array(const std::vector<std::vector<T>> a)
    {
        offsets.reserve(a.size()+1);
        const size_t data_size = std::accumulate(a.begin(), a.end(), 0, [&](const size_t o, const auto vec) { return o + vec.size(); });
        data.reserve(data_size);
        
        for(const auto& vec : a)
        {
            add_row();
            for(const T& elem : vec)
                push_back(elem);
        }
    }

    template<typename T>
        T& two_dimensional_array<T>::operator()(const size_t i, const size_t j)
        {
            assert(i < size());
            assert(j < size(i));

            return data[offsets[i] + j];
        }

    template<typename T>
        const T& two_dimensional_array<T>::operator()(const size_t i, const size_t j) const
        {
            assert(i < size());
            assert(j < size(i));

            return data[offsets[i] + j];
        }

    template<typename T>
        size_t two_dimensional_array<T>::size() const
        {
            return offsets.size()-1;
        }

    template<typename T>
        size_t two_dimensional_array<T>::size(const size_t i) const
        {
            assert(i < size());
            return offsets[i+1] - offsets[i];
        }

    template<typename T>
        void two_dimensional_array<T>::add_row()
        {
            offsets.push_back(offsets.back());
        }

    template<typename T>
        void two_dimensional_array<T>::push_back(const T& elem)
        {
            assert(offsets.size() > 1);
            ++offsets.back();
            data.push_back(elem);

        }

    template<typename T>
    template<typename ITERATOR>
        void two_dimensional_array<T>::add_row(ITERATOR begin, ITERATOR end)
        {
            add_row();
            for(auto it=begin; it!=end; ++it)
                push_back(*it);
        }

}

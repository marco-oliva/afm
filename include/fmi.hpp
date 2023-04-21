//
// Created by hongy on 4/20/2023.
//

#ifndef AFM_FMI_HPP
#define AFM_FMI_HPP

#include <iostream>
#include <vector>
#include <sdsl/suffix_arrays.hpp>
using namespace std;
using namespace sdsl;

template<typename T>
class fmi {

public:

    virtual void ComputeFmi(const std::vector<T>& s) = 0;
    virtual size_t count(const T* pattern_begin, const T* pattern_end) const = 0;
    virtual sdsl::int_vector<64> locate(const T* pattern_begin, const T* pattern_end) const = 0;


};

template<typename T>
class List : public fmi<T>{
private:
    csa_wt<> fm_index;

public:
    void ComputeFmi(const std::vector<T>& s){
        string str(s.begin(),s.end());
        construct_im(fm_index, str, 1);
    }

    size_t count(const T* pattern_begin, const T* pattern_end) const override {
        return sdsl::count(fm_index, pattern_begin, pattern_end);
    }

    sdsl::int_vector<64> locate(const T* pattern_begin, const T* pattern_end) const override {

        auto locations = sdsl::locate(fm_index, pattern_begin, pattern_end);
        return locations;
    }
};

#endif //AFM_FMI_HPP

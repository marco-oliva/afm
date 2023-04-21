//
// Created by hongy on 4/20/2023.
//

#ifndef AFM_FMI_HPP
#define AFM_FMI_HPP

#include <iostream>
#include <vector>
#include <sdsl/suffix_arrays.hpp>
using namespace std;

template<typename T>
namespace fmi
{

class fmi {
private:
    std::vector<vcfbwt::char_type> rs;

public:

    void ComputeFmi(const std::vector<T>& s){
        csa_wt<> fm_index;
        construct_im(fm_index, s, 1);
    }

};

}

#endif //AFM_FMI_HPP

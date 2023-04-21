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
//    void ComputeFmi(const std::vector<T>& s){
//        csa_wt<> fm_index;
//        construct_im(fm_index, s, 1);
//    }

};

template<typename T>
class List : public fmi<T>{
public:
    void ComputeFmi(const std::vector<T>& s){
        string str(s.begin(),s.end());
        csa_wt<> fm_index;
        construct_im(fm_index, str, 1);
        cout<<"fm index: "<<fm_index<<endl;
    }
};

#endif //AFM_FMI_HPP

//
//  fmi.hpp
//

#ifndef fmi_hpp
#define fmi_hpp

#include <iostream>
#include <vector>
#include <sdsl/suffix_arrays.hpp>

namespace afm
{

template<typename T>
class fmi 
{
private:

public:
    
    void ComputeFmi(const std::vector<T>& s){
        sdsl::csa_wt<> fm_index;
        construct_im(fm_index, s, 1);
    }
    
};

}

#endif
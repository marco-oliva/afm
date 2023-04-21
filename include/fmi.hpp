//
// fmi.hpp
//

#ifndef fmi_hpp
#define fmi_hpp

#include <iostream>
#include <vector>
#include <sdsl/suffix_arrays.hpp>

template<typename data_type>
class fmi
{
private:
    sdsl::csa_wt<> fm_index;

public:
    fmi(const std::vector<data_type>& input)
    {
        construct_im(fm_index, input, sizeof(data_type));
    }
    
    std::size_t count(const data_type* pattern_begin, const data_type* pattern_end) const
    {
        return sdsl::count(fm_index, pattern_begin, pattern_end);
    }
    
    sdsl::int_vector<64> locate(const data_type* pattern_begin, const data_type* pattern_end) const
    {
        return sdsl::locate(fm_index, pattern_begin, pattern_end);
    }
};

#endif
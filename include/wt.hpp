/* pfp - prefix free parsing wt W support
Copyright (C) 2020 Ondřej Cvacho

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/ .
*/
/*!
\file wt.hpp
\brief wt.hpp define and build the prefix-free parsing wavelet tree support data structures.
\author Ondřej Cvacho
\date 03/04/2020
*/


#ifndef _PFP_WT_HH
#define _PFP_WT_HH

#include <sdsl/rmq_support.hpp>
#include <sdsl/int_vector.hpp>
#include <sdsl/wavelet_trees.hpp>

namespace afm
{

class pfp_wt {
public:
    
    typedef uint64_t long_type;
    
    pfp_wt() {};
    pfp_wt(const std::vector<uint32_t> & sorted_alphabet, const std::vector<uint32_t> & parse) {};
    
    virtual ~pfp_wt() {};
    
    virtual void construct(const std::vector<uint32_t> & sorted_alphabet, const std::vector<uint32_t> & parse) = 0;
    
    virtual uint32_t operator[] (const long_type i) = 0;
    
    virtual long_type size() const = 0;
    
    virtual long_type rank(const long_type i, const uint32_t c) const = 0;
    virtual long_type select(const long_type i, const uint32_t c) const = 0;
private:
};

class pfp_wt_sdsl : public pfp_wt {
public:
    pfp_wt_sdsl()
    { }
    
    pfp_wt_sdsl(const std::vector<uint32_t> & sorted_alphabet, const std::vector<uint32_t> & parse){
        construct(sorted_alphabet, parse);
    }
    
    void construct(const std::vector<uint32_t> & sorted_alphabet, const std::vector<uint32_t> & parse) override {
        sdsl::int_vector<> parse_translate(parse.size(), 0);
        translate.resize(sorted_alphabet.size(), 0);
        i_translate.resize(sorted_alphabet.size(), 0);
        
        for (long_type i = 0; i < sorted_alphabet.size(); ++i) {
            translate[sorted_alphabet[i] - 1] = i;
            i_translate[i] = sorted_alphabet[i];
        }
        
        for (long_type i = 0; i < parse.size(); ++i) {
            parse_translate[i] = translate[parse[i] - 1];
        }
        
        sdsl::construct_im(wt_i, parse_translate);
    }
    
    // operator[] - get phrase ID on i-th position
    uint32_t operator[] (const long_type i) override {
        return i_translate[wt_i[i]];
    }
    
    long_type size() const override {
        return wt_i.size();
    }
    
    long_type rank(const long_type i, const uint32_t c) const override {
        return wt_i.rank(i, translate[c - 1]);
    }
    
    long_type select(const long_type i, const uint32_t c) const override {
        return wt_i.select(i, translate[c - 1]);
    }

private:
    sdsl::wt_int<> wt_i;
    std::vector<uint32_t> i_translate;
    std::vector<uint32_t> translate;
};

}

#endif /* end of include guard: _PFP_WT_HH */
//
//  fmi.hpp
//  Heavily inspired by https://github.com/jnalanko/syotti/blob/master/src/FM_index.hh
//

#ifndef fmi_hpp
#define fmi_hpp

#include <vector>
#include <cstdlib>
#include <string>
#include <sdsl/io.hpp>
#include <sdsl/wavelet_trees.hpp>
#include <spdlog/spdlog.h>

#include <wt.hpp>

extern "C" {
#include <gsacak.h>
}

//------------------------------------------------------------------------------

typedef uint64_t gsacak_long_unsigned;

template <typename data_type>
inline void sacak_templated(const data_type* s, gsacak_long_unsigned* SA, gsacak_long_unsigned n, gsacak_long_unsigned k = 0)
{
    // NOP
    spdlog::error("Not executing gsacak, wrong template used."); std::exit(1);
}

template<>
inline void sacak_templated<uint8_t> (const uint8_t* s, gsacak_long_unsigned* SA, gsacak_long_unsigned n, gsacak_long_unsigned k)
{
    sacak((unsigned char*) s, SA, n);
};

template<>
inline void sacak_templated<char> (const char* s, gsacak_long_unsigned* SA, gsacak_long_unsigned n, gsacak_long_unsigned k)
{
    sacak((unsigned char*) s, SA, n);
};

template<>
inline void sacak_templated<uint32_t> (const uint32_t* s, gsacak_long_unsigned* SA, gsacak_long_unsigned n, gsacak_long_unsigned k)
{
    sacak_int((uint32_t*) s, SA, n, k);
};

//------------------------------------------------------------------------------


namespace afm
{

typedef uint64_t fmi_long_unsigned;
typedef int64_t  fmi_long_signed;

struct interval
{
    fmi_long_signed left, right;
    fmi_long_signed size() { return right-left+1; }
    bool operator==(const interval& rhs)
    {
        return (left == rhs.left) and (right == rhs.right);
    }
    
};

template<typename data_type, typename wt_type>
class fmi
{
private:
    
    sdsl::int_vector<> sa;
    wt_type bwt;
    std::vector<fmi_long_signed> C_array;
    
    interval EMPTY_INTERVAL = { -1, 0 };  // Has length 0 by the formula end - start + 1
    interval FULL_INTERVAL  = { -1, 0 };  // Set after construction
    
    // No copying because of pointer members
    fmi(const fmi& other) = delete;
    fmi& operator=(const fmi& other) = delete;
    
    void init_C_array(const std::vector<fmi_long_signed>& counts)
    {
        this->C_array.resize(counts.size() + 1, 0);
        
        // Compute cumulative sum of counts
        for(fmi_long_signed i = 0; i < C_array.size(); i++)
        {
            C_array.at(i) = counts[i];
            if(i > 0) { C_array.at(i) += C_array.at(i - 1); }
        }
        
        // Shift C to the right by one because that's how it's defined
        for(fmi_long_signed i = C_array.size() - 1; i >= 0; i--)
        {
            if(i == 0) { C_array.at(i) = 0; }
            else { C_array.at(i) = C_array.at(i - 1); }
        }
    }
    
    void init_wt_bwt(const std::vector<data_type>& input)
    {
        spdlog::error("Wavelet Tree constructor not implemented!");
        std::exit(EXIT_FAILURE);
    }
    
    
    
    interval get_full_interval() const { return FULL_INTERVAL; };
    interval get_empty_interval() const { return EMPTY_INTERVAL; };
    fmi_long_unsigned SA_at(fmi_long_unsigned i) const { return sa[i]; }
    fmi_long_unsigned C_array_at(data_type c) const { return C_array.at(c); }
    data_type bwt_at(fmi_long_unsigned i) const { return bwt[i]; }
    fmi_long_unsigned bwt_rank(fmi_long_unsigned i, data_type c) const { return bwt.rank(i, c); }
    fmi_long_unsigned size() const { return bwt.size(); }
    
public:
    
    fmi() = default;
    explicit fmi(const std::vector<data_type>& input) { construct(input); }
    
    void construct(const std::vector<data_type>& input)
    {
        assert(input.back() == 0);
        
        // Compute the suffix array
        fmi_long_unsigned alphabet_size = (*std::max_element(input.begin(), input.end())) + 1;
        
        spdlog::info("Using 8 bytes for computing SA of the input");
        std::vector<fmi_long_unsigned> tmp_sa(input.size(), 0);
        sacak_templated<data_type>(&input[0], &tmp_sa[0], input.size(), alphabet_size);
        
        fmi_long_unsigned bytes_sa = 0;
        fmi_long_unsigned max_sa = input.size() + 1;
        while (max_sa != 0) { max_sa >>= 8; bytes_sa++; }
        spdlog::info("Using {} bytes for storing SA of the input", bytes_sa);
        sa = sdsl::int_vector<>(tmp_sa.size(), 0ULL, bytes_sa * 8);
        
        for (fmi_long_unsigned i = 0; i < tmp_sa.size(); i++) { sa[i] = tmp_sa[i]; }
        tmp_sa.resize(0);
        
        init_wt_bwt(input);

        std::vector<fmi_long_signed> char_counts(alphabet_size, 0);
        for(std::size_t i = 0; i < this->bwt.size(); i++) { char_counts.at(this->bwt[i])++; }
        init_C_array(char_counts);

        FULL_INTERVAL = {(fmi_long_signed)0, (fmi_long_signed)bwt.size()-1};
    }
    
    interval left_extend(interval I, data_type c) const
    {
        if (I == EMPTY_INTERVAL) { return I; }
        fmi_long_signed r1 = bwt.rank(I.left, c);
        fmi_long_signed r2 = bwt.rank(I.right + 1, c);
        interval I_new = {C_array[c] + r1, C_array[c] + r2 - 1};
        if (I_new.left > I_new.right) { return EMPTY_INTERVAL; }
        else { return I_new; }
    }
    
    interval search(const std::vector<data_type>& p) const
    {
        interval I = FULL_INTERVAL;
        for(fmi_long_signed i = (fmi_long_signed) p.size()-1; i >= 0; i--)
        {
            I = left_extend(I, p[i]);
            if (I == EMPTY_INTERVAL) { return EMPTY_INTERVAL; }
        }
        return I;
    }
    
    fmi_long_unsigned count(const std::vector<data_type>& p) const
    {
        interval I = search(p);
        if (I == EMPTY_INTERVAL) { return 0; }
        else { return I.size(); }
    }
    
    std::vector<fmi_long_unsigned> locate(const std::vector<data_type>& p) const
    {
        interval I = search(p);
        
        std::vector<fmi_long_unsigned> out(I.size(), 0);
        for (std::size_t i = 0; i < I.size(); i++) { out[i] = sa[I.left + i]; }
        
        return out;
    }
    
};

template<>
inline void fmi<vcfbwt::char_type, sdsl::wt_huff<>>::init_wt_bwt(const std::vector<vcfbwt::char_type>& input)
{
    spdlog::info("Building Wavelet Tree on chars");
    std::vector<vcfbwt::char_type> bwt_tmp(sa.size() + 1, 0); // has to be 0 terminated
    for(fmi_long_signed i = 0; i < sa.size(); i++)
    {
        if(sa[i] == 0) { bwt_tmp[i] = '$'; }
        else { bwt_tmp[i] = input[sa[i]-1]; }
    }
    construct_im(this->bwt, (const char*)(&bwt_tmp[0]), 1);
}

template<>
inline void fmi<vcfbwt::size_type, pfp_wt_sdsl>::init_wt_bwt(const std::vector<vcfbwt::size_type>& input)
{
    spdlog::info("Building Wavelet Tree on integers");
    // create alphabet (phrases)
    std::size_t alphabet_size = *std::max_element(input.begin(), input.end());
    std::vector<vcfbwt::size_type> alphabet(alphabet_size, 0);
    for (vcfbwt::size_type i = 0; i < alphabet.size(); i++) { alphabet[i] = i + 1; }
    
    // create BWT and Wavelet Tree over it
    std::vector<vcfbwt::size_type> bwt_tmp(input.size() - 1, 0);
    for (fmi_long_unsigned i = 1; i < sa.size(); ++i)
    {
        if (sa[i] > 0) { bwt_tmp[i - 1] = input[sa[i] - 1]; }
        else { bwt_tmp[i - 1] = input[input.size() - 2]; }
    }
    this->bwt.construct(alphabet, bwt_tmp);
}

}

#endif
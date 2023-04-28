//
//  afm_algorithm.hpp
//

#ifndef afm_algorith_hpp
#define afm_algorith_hpp

#include <sdsl/sd_vector.hpp>

#include <parser.hpp>
#include <fmi.hpp>

namespace afm
{

class baseline_fmi
{
private:
    vcfbwt::pfp::Params parameters;
    afm::fmi<vcfbwt::char_type, sdsl::wt_huff<>> fmindex;
    
    void init(const vcfbwt::pfp::Params& params, vcfbwt::pfp::Dictionary<vcfbwt::char_type>& dictionary, const std::vector<vcfbwt::size_type>& parse);
    
public:
    
    baseline_fmi() = default;
    baseline_fmi(const vcfbwt::pfp::Params& params, vcfbwt::pfp::Dictionary<vcfbwt::char_type>& dictionary, const std::vector<vcfbwt::size_type>& parse)
    {
        this->init(params, dictionary, parse);
    }
    
    void construct(const vcfbwt::pfp::Params& params, vcfbwt::pfp::Dictionary<vcfbwt::char_type>& dictionary, const std::vector<vcfbwt::size_type>& parse)
    {
        this->init(params, dictionary, parse);
    }
    
    afm::fmi_long_unsigned count(const std::vector<vcfbwt::char_type>& pattern) const;
    std::vector<afm::fmi_long_unsigned> locate(const std::vector<vcfbwt::char_type>& pattern) const;
};

class accelerated_fmi
{
private:
    vcfbwt::pfp::Params parameters;
    
    // bitvector
    sdsl::sd_vector<> b_t;
    typename sdsl::sd_vector<>::rank_1_type rank_b_p;
    typename sdsl::sd_vector<>::select_1_type select_b_p;

    void init(const vcfbwt::pfp::Params& params, vcfbwt::pfp::Dictionary<vcfbwt::char_type>& dictionary, const std::vector<vcfbwt::size_type>& parse);

public:
    
    accelerated_fmi() = default;
    accelerated_fmi(const vcfbwt::pfp::Params& params, vcfbwt::pfp::Dictionary<vcfbwt::char_type>& dictionary, const std::vector<vcfbwt::size_type>& parse)
    {
        this->init(params, dictionary, parse);
    }
    
    void construct(const vcfbwt::pfp::Params& params, vcfbwt::pfp::Dictionary<vcfbwt::char_type>& dictionary, const std::vector<vcfbwt::size_type>& parse)
    {
        this->init(params, dictionary, parse);
    }
    
    std::vector<afm::fmi_long_unsigned> count(const std::vector<vcfbwt::char_type>& pattern) const;
    std::vector<afm::fmi_long_unsigned> locate(const std::vector<vcfbwt::char_type>& pattern) const;
};

}

#endif
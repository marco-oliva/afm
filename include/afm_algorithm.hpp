//
//  afm_algorithm.hpp
//

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
    
public:
    baseline_fmi(const vcfbwt::pfp::Params& params, vcfbwt::pfp::Dictionary<vcfbwt::char_type>& dictionary, const std::vector<vcfbwt::size_type>& parse);
    
    std::size_t count(const std::vector<vcfbwt::char_type>& pattern) const;
    std::vector<std::size_t> locate(const std::vector<vcfbwt::char_type>& pattern) const;
};

class accelerated_fmi
{
private:
    vcfbwt::pfp::Params parameters;
    
    // bitvector
    sdsl::sd_vector<> b_t;
    typename sdsl::sd_vector<>::rank_1_type rank_b_p;
    typename sdsl::sd_vector<>::select_1_type select_b_p;



public:
    accelerated_fmi(const vcfbwt::pfp::Params& params, vcfbwt::pfp::Dictionary<vcfbwt::char_type>& dictionary, const std::vector<vcfbwt::size_type>& parse);
    
    std::size_t count(const std::vector<vcfbwt::char_type>& pattern) const;
    std::vector<std::size_t> locate(const std::vector<vcfbwt::char_type>& pattern) const;
};

}
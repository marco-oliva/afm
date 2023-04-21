//
//  afm_algorithm.hpp
//

#include <parser.hpp>

namespace afm
{

class baseline_fmi
{

public:
    baseline_fmi(vcfbwt::pfp::Dictionary<vcfbwt::char_type>& dictionary, std::vector<vcfbwt::size_type> parse);
    
    bool exists(std::vector<vcfbwt::char_type> pattern) const;
    std::size_t count(std::vector<vcfbwt::char_type> pattern) const;
    std::vector<std::size_t> locate(std::vector<vcfbwt::char_type> pattern) const;
    
    
};

class accelerated_fmi
{

public:
    accelerated_fmi(vcfbwt::pfp::Dictionary<vcfbwt::char_type>& dictionary, std::vector<vcfbwt::size_type> parse);
    
    bool exists(std::vector<vcfbwt::char_type> pattern) const;
    std::size_t count(std::vector<vcfbwt::char_type> pattern) const;
    std::vector<std::size_t> locate(std::vector<vcfbwt::char_type> pattern) const;
};

}
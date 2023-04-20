//
//  parser.hpp
//

#ifndef parser_hpp
#define parser_hpp

#include <pfp_algo.hpp>
#include <utils.hpp>

namespace afm
{

class StringParser
{
private:
    
    vcfbwt::pfp::Params params;
    const vcfbwt::pfp::Dictionary<std::uint8_t>& dictionary;
    
public:
    
    StringParser() = delete;
    StringParser(const vcfbwt::pfp::Params& pfp_params, const vcfbwt::pfp::Dictionary<std::uint8_t>& d);
    
    void operator()(const std::string& s, std::vector<vcfbwt::short_type>& parse) const;
};

}

#endif //parser_hpp

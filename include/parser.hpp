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
    vcfbwt::pfp::Dictionary<vcfbwt::char_type>& dictionary;
    
public:
    
    StringParser() = delete;
    StringParser(const vcfbwt::pfp::Params& pfp_params, vcfbwt::pfp::Dictionary<vcfbwt::char_type>& d)
    : params(pfp_params), dictionary(d)
    { }
    
    void operator()(const std::vector<vcfbwt::char_type>& s, std::vector<std::pair<vcfbwt::size_type, std::size_t>>& parse);
};

}

#endif //parser_hpp

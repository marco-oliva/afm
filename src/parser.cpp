//
//  parser.cpp
//

#include <parser.hpp>

//------------------------------------------------------------------------------
afm::StringParser::StringParser(const vcfbwt::pfp::Params& pfp_params, const vcfbwt::pfp::Dictionary<std::uint8_t>& d)
: dictionary(d)
{

}

//------------------------------------------------------------------------------

void
afm::StringParser::operator()(const std::string& s, std::vector<vcfbwt::short_type>& parse) const
{

}

//------------------------------------------------------------------------------

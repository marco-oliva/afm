//
//  afm_algorithm.cpp
//

#include <afm_algorithm.hpp>


//------------------------------------------------------------------------------
void
afm::baseline_fmi::init(
const vcfbwt::pfp::Params& params,
vcfbwt::pfp::Dictionary<vcfbwt::char_type>& dictionary,
const std::vector<vcfbwt::size_type>& parse)
{
    std::vector<vcfbwt::char_type> tmp_input;
    for (std::size_t i = 0; i < parse.size() - 1; i++)
    {
        auto& phrase_id = parse[i];
        auto& phrase = dictionary.sorted_entry_at(phrase_id - 1);
        
        tmp_input.insert(tmp_input.end(), phrase.begin(), phrase.end() - parameters.w);
    }
    auto& phrase = dictionary.sorted_entry_at(parse.back() - 1);
    tmp_input.insert(tmp_input.end(), phrase.begin(), phrase.end()); // for the last phrase we need the last w characters
    tmp_input.push_back(0);
    
    this->fmindex.construct(tmp_input);
}

std::vector<afm::fmi_long_unsigned>
afm::baseline_fmi::locate(const std::vector<vcfbwt::char_type>& pattern) const
{
    return this->fmindex.locate(pattern);
}

afm::fmi_long_unsigned
afm::baseline_fmi::count(const std::vector<vcfbwt::char_type>& pattern) const
{
    return this->fmindex.count(pattern);
}

//------------------------------------------------------------------------------
void
afm::accelerated_fmi::init(
const vcfbwt::pfp::Params& params,
vcfbwt::pfp::Dictionary<vcfbwt::char_type>& dictionary,
const std::vector<vcfbwt::size_type>& parse)
{ }

std::vector<afm::fmi_long_unsigned>
afm::accelerated_fmi::count(const std::vector<vcfbwt::char_type>& pattern) const
{
    return std::vector<afm::fmi_long_unsigned>();
}

std::vector<afm::fmi_long_unsigned>
afm::accelerated_fmi::locate(const std::vector<vcfbwt::char_type>& pattern) const
{
    return std::vector<afm::fmi_long_unsigned>();
}



//------------------------------------------------------------------------------

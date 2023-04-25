//
//  afm_algorithm.cpp
//

#include <afm_algorithm.hpp>


//------------------------------------------------------------------------------
afm::baseline_fmi::baseline_fmi(
const vcfbwt::pfp::Params& params,
vcfbwt::pfp::Dictionary<vcfbwt::char_type>& dictionary,
const std::vector<vcfbwt::size_type>& parse)
: parameters(params)
{
    std::vector<vcfbwt::char_type> tmp_input;
    for (std::size_t i = 0; i < parse.size() - 1; i++)
    {
        auto& phrase_id = parse[i];
        const std::vector<vcfbwt::char_type>& phrase = dictionary.sorted_entry_at(phrase_id);
        
        tmp_input.insert(tmp_input.end(), phrase.begin(), phrase.end() - parameters.w);
    }
    const std::vector<vcfbwt::char_type>& phrase = dictionary.sorted_entry_at(parse.back());
    tmp_input.insert(tmp_input.end(), phrase.begin(), phrase.end()); // for the last phrase we need the last w characters
    
    this->fmindex.construct(tmp_input);
}

std::vector<std::size_t>
afm::baseline_fmi::locate(const std::vector<vcfbwt::char_type>& pattern) const
{
    return this->fmindex.locate(pattern);
}

std::size_t
afm::baseline_fmi::count(const std::vector<vcfbwt::char_type>& pattern) const
{
    return this->fmindex.count(pattern);
}

//------------------------------------------------------------------------------
afm::accelerated_fmi::accelerated_fmi(
const vcfbwt::pfp::Params& params,
vcfbwt::pfp::Dictionary<vcfbwt::char_type>& dictionary,
const std::vector<vcfbwt::size_type>& parse)
: parameters(params)
{ }

//------------------------------------------------------------------------------

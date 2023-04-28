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

afm::interval
afm::baseline_fmi::search(const std::vector<vcfbwt::char_type>& pattern) const
{
    return this->fmindex.search(pattern);
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
{
    this->parameters = params;
    this->dictionary = &dictionary;
    
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
    
    this->fm_t.construct(tmp_input);
    
    // mark trigger strings positions
    std::vector<std::size_t> onset;
    for (std::size_t i = 0; i < dictionary.size(); ++i)
    {
        auto& curr_phrase = dictionary.sorted_entry_at(i);
        std::vector<vcfbwt::char_type> curr_trigger;
        curr_trigger.insert(curr_trigger.end(), curr_phrase.begin(), curr_phrase.begin() + params.w);
        
        for (std::size_t j = i + 1; j < dictionary.size(); ++j)
        {
            auto& next_phrase = dictionary.sorted_entry_at(j);
            std::vector<vcfbwt::char_type> next_trigger;
            next_trigger.insert(next_trigger.end(), next_phrase.begin(), next_phrase.begin() + params.w);
            
            if (curr_trigger != next_trigger) { i = j - 1; break; }
            if (j == dictionary.size() - 1) { i = j; break; }
        }
        
        auto interval = this->fm_t.search(curr_trigger);
        if (interval == fm_t.get_empty_interval()) { continue; }
        else
        {
            assert(onset.size() == 0 or interval.left > onset.back());
            for (std::size_t j = interval.left; j <= interval.right ; ++j) { onset.push_back(j); }
        }
    }
    
    sdsl::sd_vector_builder builder(tmp_input.size(), onset.size());
    for (std::size_t i = 0; i < onset.size(); i++) { builder.set(onset[i]); }
    
    ts_bitvector = sdsl::sd_vector<>(builder);
    rank_ts_bitvector = typename sdsl::sd_vector<>::rank_1_type(&ts_bitvector);
    select_ts_bitvector = typename sdsl::sd_vector<>::select_1_type(&ts_bitvector);
    
    //construct the fmindex on the parse
    std::vector<vcfbwt::size_type> parse_copy(parse.begin(), parse.end());
    parse_copy.push_back(0);
    this->fm_p.construct(parse_copy);
}

afm::fmi_long_unsigned
afm::accelerated_fmi::count(const std::vector<vcfbwt::char_type>& pattern) const
{
    return search(pattern).size();
}

afm::interval
afm::accelerated_fmi::search(const std::vector<vcfbwt::char_type>& pattern) const
{
    // parse the pattern
    afm::StringParser sp(parameters, *dictionary);
    std::vector<std::pair<uint32_t, std::size_t>> parse_sp;
    sp(pattern, parse_sp);
    
    // backward searching on fm_t until the first trigger string from the end
    interval interval_text = this->fm_t.get_full_interval();
    for(fmi_long_signed i = (fmi_long_signed) pattern.size() - 1; i >= (fmi_long_signed) (pattern.size() - parse_sp.back().second); i--)
    {
        interval_text = this->fm_t.left_extend(interval_text, pattern[i]);
        if (i == 0 or interval_text == fm_t.get_empty_interval()) { return interval_text; }
    }
    assert(parse_sp.size() >= 2); // if we are here we have at least two phrases
    
    // convert into an interval on the parse
    interval interval_p = {(fmi_long_signed) rank_ts_bitvector(interval_text.left), (fmi_long_signed) rank_ts_bitvector(interval_text.right)};
    
    // now backward search on the parse of the pattern
    std::size_t phrase_to_search = parse_sp.size() - 1 - 1;
    for (std::size_t i = phrase_to_search; i >= 1 ; i--)
    {
        if (parse_sp[i].first == 0) { phrase_to_search = i; break; } // phrase not in the dictionary
        
        interval next_interval = this->fm_p.left_extend(interval_p,parse_sp[i].first);
        if (next_interval == fm_p.get_empty_interval()) { phrase_to_search = i; break; } // reached somethingno in the input
        else { phrase_to_search = i - 1; interval_p = next_interval; }
    }
    
    // now for the last backward search in the text
    interval_text = {(fmi_long_signed) select_ts_bitvector(interval_p.left + 1), (fmi_long_signed) select_ts_bitvector(interval_p.right + 1)};
    
    for(fmi_long_signed i = (fmi_long_signed) parse_sp[phrase_to_search].second - parameters.w - 1; i >= 0; i--)
    {
        interval_text = this->fm_t.left_extend(interval_text, pattern[i]);
        if (interval_text == fm_t.get_empty_interval()) { return fm_t.get_empty_interval(); } // pattern not found
    }
    
//    // test
//    interval test_interval = fm_t.search(pattern);
//    assert(interval_text == test_interval);
    
    // finally, return the interval
    return interval_text;
}



//------------------------------------------------------------------------------

//
//  parser.cpp
//

#include <parser.hpp>

//------------------------------------------------------------------------------
void
afm::StringParser::operator()(const std::vector<vcfbwt::char_type>& s, std::vector<std::pair<vcfbwt::size_type, std::size_t>>& parse)
{
    // Karp Robin Hash Function for sliding window
    vcfbwt::Mersenne_KarpRabinHash kr_hash(this->params.w);
    
    // Phrase
    std::vector<vcfbwt::char_type> phrase;
    
    for (auto& s_c : s)
    {
        phrase.push_back(s_c);
    
        if (phrase.size() == params.w) { kr_hash.initialize(phrase.data(), params.w); }
        else if (phrase.size() > params.w) { kr_hash.update(phrase[phrase.size() - params.w - 1], phrase[phrase.size() - 1]); }
    
        if ((phrase.size() > this->params.w) and ((kr_hash.get_hash() % this->params.p) == 0))
        {
            if (this->dictionary.contains(phrase))
            {
                vcfbwt::hash_type hash = vcfbwt::string_hash((char*) phrase.data(), phrase.size());
                vcfbwt::short_type rank = this->dictionary.hash_to_rank(hash);
                parse.emplace_back(std::make_pair(rank, phrase.size()));
            }
            else { parse.emplace_back(std::make_pair(0, phrase.size())); }
            
            phrase.erase(phrase.begin(), phrase.end() - this->params.w); // Keep the last w chars
            kr_hash.reset(); kr_hash.initialize(phrase.data(), params.w);
        }
    }
    
    // Last phrase, not in the dictionary
    if (phrase.size() >= this->params.w) { parse.emplace_back(std::make_pair(0, phrase.size())); }
}

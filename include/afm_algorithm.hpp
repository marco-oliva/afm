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
    interval search(const std::vector<vcfbwt::char_type>& pattern) const;
};

class accelerated_fmi
{
private:
    vcfbwt::pfp::Params parameters;
    
    vcfbwt::pfp::Dictionary<vcfbwt::char_type>* dictionary = nullptr;
    
    afm::fmi<vcfbwt::char_type, sdsl::wt_rlmn<>> fm_t;
    afm::fmi<vcfbwt::size_type, afm::pfp_wt_sdsl> fm_p;
    
    sdsl::sd_vector<> ts_bitvector;
    typename sdsl::sd_vector<>::rank_1_type rank_ts_bitvector;
    typename sdsl::sd_vector<>::select_1_type select_ts_bitvector;

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

    void serialize_fm_t(const std::string& filename) const {
        std::string sa_filename = filename + "_sa.sdsl";
        std::string bwt_filename = filename + "_bwt.sdsl";
        std::string C_array_filename = filename + "_C_array.sdsl";

//        fm_t.serialize_sa(sa_filename);
        fm_t.serialize_bwt(bwt_filename);
        fm_t.serialize_C_array(C_array_filename);
    }

    void serialize_fm_p(const std::string& filename) const {
        std::string sa_filename = filename + "_pars_sa.sdsl";
        std::string bwt_filename = filename + "_pars_bwt.sdsl";
        std::string C_array_filename = filename + "_pars_C_array.sdsl";

//        fm_p.serialize_sa(sa_filename);
        fm_p.serialize_bwt(bwt_filename);
        fm_p.serialize_C_array(C_array_filename);
    }

    void serialize_ts_bitvector(const std::string& filename) const {
        std::ofstream out(filename, std::ios::binary);
        if (!out) {
            throw std::runtime_error("Failed to open file for writing: " + filename);
        }
        ts_bitvector.serialize(out);
    }
    
    afm::fmi_long_unsigned count(const std::vector<vcfbwt::char_type>& pattern) const;
    interval search(const std::vector<vcfbwt::char_type>& pattern) const;
};

}

#endif
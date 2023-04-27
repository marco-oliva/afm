//
//  benchmarks.cpp
//
//

#include <CLI/CLI.hpp>
#include <hayai.hpp>

#include <afm_algorithm.hpp>

//------------------------------------------------------------------------------

bool file_exists(std::string path)
{
    struct stat64 stat_buf;
    int rc = stat64(path.c_str(), &stat_buf);
    return rc == 0;
}

//------------------------------------------------------------------------------

std::string input_prefix;
std::string d_path;
std::string p_path;
vcfbwt::pfp::Params params;

//------------------------------------------------------------------------------

class basline_fmi_fixture : public ::hayai::Fixture
{
public:
    
    afm::baseline_fmi bfmi;
    
    virtual void SetUp()
    {
        // Read in parse and dictionary
        std::vector<vcfbwt::size_type> parse;
        vcfbwt::pfp::ParserUtils<vcfbwt::char_type>::read_parse(p_path, parse);
    
        vcfbwt::pfp::Dictionary<vcfbwt::char_type> dictionary;
        std::vector<std::vector<vcfbwt::char_type>> dictionary_vector;
        vcfbwt::pfp::ParserUtils<vcfbwt::char_type>::read_dictionary(d_path, dictionary_vector);
        for (auto& phrase : dictionary_vector) { dictionary.check_and_add(phrase); }
        
        bfmi.construct(params, dictionary, parse);
    }
};


BENCHMARK_F(basline_fmi_fixture, locate, 2, 2)
{
    std::vector<vcfbwt::char_type> pattern;
    bfmi.locate(pattern);
}

//------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    CLI::App app("AFM Benchmarks");
    
    app.add_option("-i,--input-prefix", input_prefix, "Input prefix of PFP.")->required();
    app.add_option("-w, --window-size", params.w, "Sliding window size.")->required()->check(CLI::Range(3, 200));
    app.add_option("-p, --modulo", params.p, "Modulo used during parsing.")->required()->check(CLI::Range(5, 20000));
    CLI11_PARSE(app, argc, argv);
    
    // Check all files needed
    d_path = input_prefix + ".dict";
    if (not file_exists(d_path)) { spdlog::error("Could not open required file: {}", d_path); std::exit(EXIT_FAILURE); }
    
    std::string p_path = input_prefix + ".parse";
    if (not file_exists(p_path)) { spdlog::error("Could not open required file: {}", p_path); std::exit(EXIT_FAILURE); }
    
    hayai::ConsoleOutputter consoleOutputter;
    
    hayai::Benchmarker::AddOutputter(consoleOutputter);
    hayai::Benchmarker::RunAllTests();
    return 0;
    
}
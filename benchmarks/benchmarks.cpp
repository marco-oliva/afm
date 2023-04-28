//
//  benchmarks.cpp
//
//

#include <random>

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

vcfbwt::pfp::Params params;
vcfbwt::pfp::Dictionary<vcfbwt::char_type> dictionary;
std::vector<vcfbwt::size_type> parse;

std::vector<std::vector<vcfbwt::char_type>> queries;

//------------------------------------------------------------------------------

class basline_fmi_fixture : public ::hayai::Fixture
{
public:
    
    afm::baseline_fmi bfmi;
    
    virtual void SetUp()
    {
        spdlog::info("Building Baseline Fixture");
        bfmi.construct(params, dictionary, parse);
        spdlog::info("Done Building Baseline Fixture");
    }
};


BENCHMARK_F(basline_fmi_fixture, search, 1, 50)
{
    for (auto& pattern : queries) { bfmi.search(pattern); }
}

//------------------------------------------------------------------------------

class accelerated_fmi_fixture : public ::hayai::Fixture
{
public:
    
    afm::accelerated_fmi afmi;
    
    virtual void SetUp()
    {
        spdlog::info("Building Accelerated Fixture");
        afmi.construct(params, dictionary, parse);
        spdlog::info("Done Building Accelerated Fixture");
    }
};


BENCHMARK_F(accelerated_fmi_fixture, search, 1, 50)
{
    for (auto& pattern : queries) { afmi.search(pattern); }
}

//------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    CLI::App app("AFM Benchmarks");
    
    std::string input_prefix;
    std::size_t n_patterns = 10000;
    std::size_t patterns_size = 1000;
    
    app.add_option("-i,--input-prefix", input_prefix, "Input prefix of PFP.")->required();
    app.add_option("-w, --window-size", params.w, "Sliding window size.")->required()->check(CLI::Range(3, 200));
    app.add_option("-p, --modulo", params.p, "Modulo used during parsing.")->required()->check(CLI::Range(5, 20000));
    app.add_option("-n, --patterns-number", n_patterns, "Number of patterns.");
    app.add_option("-l, --patterns-length", n_patterns, "Length of the patterns.");
    CLI11_PARSE(app, argc, argv);
    
    // Check all files needed
    std::string d_path = input_prefix + ".dict";
    if (not file_exists(d_path)) { spdlog::error("Could not open required file: {}", d_path); std::exit(EXIT_FAILURE); }
    
    std::string p_path = input_prefix + ".parse";
    if (not file_exists(p_path)) { spdlog::error("Could not open required file: {}", p_path); std::exit(EXIT_FAILURE); }
    
    // Read in parse
    spdlog::info("Reading parse");
    vcfbwt::pfp::ParserUtils<vcfbwt::char_type>::read_parse(p_path, parse);
   
    // Read in dictionary
    spdlog::info("Reading dictionary");
    std::vector<std::vector<vcfbwt::char_type>> dictionary_vector;
    vcfbwt::pfp::ParserUtils<vcfbwt::char_type>::read_dictionary(d_path, dictionary_vector);
    for (auto& phrase : dictionary_vector) { dictionary.check_and_add(phrase); }
    
    // Reconstruct the input text
    spdlog::info("Extracting the input text");
    std::vector<vcfbwt::char_type> tmp_input;
    for (std::size_t i = 0; i < parse.size() - 1; i++)
    {
        auto& phrase_id = parse[i];
        auto& phrase = dictionary.sorted_entry_at(phrase_id - 1);
        
        tmp_input.insert(tmp_input.end(), phrase.begin(), phrase.end() - params.w);
    }
    auto& phrase = dictionary.sorted_entry_at(parse.back() - 1);
    tmp_input.insert(tmp_input.end(), phrase.begin(), phrase.end()); // for the last phrase we need the last w characters
    
    // Extract n_patterns strings of length patterns_size at random from the input, see https://stackoverflow.com/questions/7560114/random-number-c-in-some-range
    spdlog::info("Generating {} patterns of length {}", n_patterns, patterns_size);
    std::random_device rd; // obtain a random number from hardware
    std::mt19937_64 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(1, tmp_input.size() - patterns_size - 1 - params.w); // define the range
    
    for (std::size_t i = 0; i < n_patterns; i++)
    {
        std::size_t starting_position = distr(gen);
        std::vector<vcfbwt::char_type> pattern;
        pattern.insert(pattern.end(),
                       tmp_input.begin() + starting_position,
                       tmp_input.begin() + starting_position + patterns_size);
        queries.push_back(pattern);
    }
    
    spdlog::info("Starting Benchmarks");
    hayai::ConsoleOutputter consoleOutputter;

    hayai::Benchmarker::AddOutputter(consoleOutputter);
    hayai::Benchmarker::RunAllTests();
    return 0;
}
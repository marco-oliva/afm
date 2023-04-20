//
//  unit_tests.cpp
//
//  Copyright 2022 Marco Oliva. All rights reserved.
//

#include <iostream>

#define CATCH_CONFIG_RUNNER
#include <catch2/catch_all.hpp>

#include <spdlog/spdlog.h>


//------------------------------------------------------------------------------

struct listener : Catch::EventListenerBase
{
    using EventListenerBase::EventListenerBase;
    
    virtual void testCaseStarting(Catch::TestCaseInfo const& testInfo) override
    {
        std::cout << testInfo.tagsAsString() << " " << testInfo.name << std::endl;
    }
};
CATCH_REGISTER_LISTENER(listener)

//------------------------------------------------------------------------------

std::string testfiles_dir = "../tests/files";

//------------------------------------------------------------------------------

TEST_CASE( "Dummy test", "[small]" )
{
    REQUIRE(true);
}

//------------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
    Catch::Session session;
    
    using namespace Catch::Clara;
    
    auto cli = session.cli() |
    Opt( testfiles_dir, "dir" ) ["--test-dir"] ("specify the directory containing the test dna sequences files");
    
    session.cli(cli);
    
    int returnCode = session.applyCommandLine(argc, argv);
    
    if( returnCode != 0 ) return returnCode;
    
    session.run();
}
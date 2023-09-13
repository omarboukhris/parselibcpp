//
// Created by omar on 10/09/23.
//
#pragma once

#include <gtest/gtest.h>
#include <parselib/utils/preprocessor.hpp>
#include <parselib/parsers/parsers.hpp>

namespace parselib::tests {

// The fixture for testing class Foo.
class LRParserTest : public ::testing::Test {
protected:
    Grammar grammar;

    LRParserTest() = default;

    ~LRParserTest() override = default;

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    void SetUp() override {
        // Code here will be called immediately after the constructor (right
        // before each test).
    }

    void TearDown() override {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

    void setup(const std::string& filename, bool verbose = true) {
        auto logger = std::make_shared<utils::Logger>(utils::Logger::LogLevel::LogAll) ;

        utils::PreprocPtr preproc (new utils::OnePassPreprocessor()) ;
        parsers::GenericGrammarParser ggp (preproc, logger) ;

        grammar = ggp.parse (filename, verbose, true) ;
    }

    void parse(const std::string& filename, bool verbose = true) {
        std::string source = utils::get_text_file_content(filename);

        auto tokenizer = lexer::Lexer(grammar.tokens);
        tokenizer.tokenize(source, verbose);

        auto parser = parsers::LR_zero(grammar);
        parser.membership(tokenizer.tokens);
    }

};

// Tests that the Foo::Bar() method does Abc.
TEST_F(LRParserTest, MethodBarDoesAbc) {
    int expected = 0;
    EXPECT_EQ(expected, 0);
}

} // namespace parselib::tests

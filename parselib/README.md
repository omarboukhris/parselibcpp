# ParseLib

This is the C++ core of ParseLib framework

# Interfaces

The ParseLib interfaces expose a lot of functionalities, among them

## Sessions

All functions mentioned later and more are wrapped in a utility class (`parselib.parselibinstance.ParselibInstance`).

Reading a grammar and parsing a source code then becomes trivial :
```c++
#include <parselib/parselibinstance.hpp>
#include <boost/property_tree/ptree.hpp>

namespace pl = parselib;

int main(int argc, char** argv){
  // define a parselib session
  pl::ParseSession parsesession ;
  bool verbose = true ;

  // load a grammar from a raw text file++
  parsesession.loadGrammar("data/grammar.grm", verbose) ;
  
  // parse some source code if parsable
  // return type is boost::property_tree::ptree 
  auto out = parsesession.process_source_to_ptree("data/test.java", verbose) ; 

  return 0 ;
}
```

## Recursive File Glober 

This object can be used to glob recursively files and filter them by file extension.

```c++
#include <parselib/utils/io.hpp>

// setup fileGlober
parselib::utils::FileGlober fileglober ("foo/bar", "java") ;
// recursively globs all java files in foo/bar (relative path accepted)
auto files = fileglober.glob() ; 
// type is parselib::utils::FileGlober::FilesList or std::vector<std::string>
```

This can mainly be useful to setup an engine for recursively exploring files to parse.

# Under the hood (Pipeline)

## Graph encoder for generic textual CFG

This encode a text written context-free grammar (CFG) in a graph data-structure handled by the appropriate component, implemented Parsers mainly.

```c++
//import important stuff
#include <parselib/parselibinstance.hpp>

using namespace parselib ;

Grammar load(std::string filename) {

  //define preprocessor to use
  utils::OnePassPreprocessor *preproc = new utils::OnePassPreprocessor() ;
  parsers::GenericGrammarParser ggp (preproc) ; //define the parser

  auto grammar = ggp.parse (filename, verbose) ; //..and parse

  std::cout << grammar ; //it is printable
  
  return grammar;
}
```
Results on display :
```javascript
RULE AXIOM = [
	S(NONTERMINAL)
]
RULE S = [
	a(TERMINAL) + S(NONTERMINAL) + b(TERMINAL)
	''(EMPTY)
]

TOKEN a = regex('a')
TOKEN b = regex('b')
```

## Operators for grammar transformation 

to 2NF<sup>[1]</sup>

- TERM : creates production rule pointing to a specific terminal for each terminal in a production rule
- BIN  : binarize all rules

Note : START operator is forced by the language by the AXIOM keyword

```c++
parselib::normoperators::get2nf(grammar) ;
```
Result on display :
```javascript
RULE AXIOM = [
	S(NONTERMINAL)
]
RULE S = [
	a.(NONTERMINAL) + S-b(NONTERMINAL)
]
RULE S-b = [
	S(NONTERMINAL) + b.(NONTERMINAL)
	b(TERMINAL)
]
RULE a. = [
	a(TERMINAL)
]
RULE b. = [
	b(TERMINAL)
]

TOKEN a = regex('a')
TOKEN b = regex('b')
```
## CYK parsers for grammars in 2NF<sup>[1]</sup>

```c++
//import the good stuff
#include <parselib/parselibinstance.hpp> 

using namespace parselib;

Frame parse_file_into_frame (Grammar grammar, std::string filename) {

  parsers::CYK parser (grammar) ; //instantiate parser
  std::string source = utils::gettextfilecontent(filename) ; //load source from text file

  //tokenize source code
  lexer::Lexer tokenizer (grammar.tokens) ;
  tokenizer.tokenize (source) ;

  //result is in a Frame which is a polite term to say std::vector<parselib::parsetree::Node*>
  //containing all accepted solutions/parse trees
  //we generally use the first one
  Frame result = parser.membership (tokenizer.tokens) ;
  return result;
}
```
The parser supports basic error handling. If `membership` fails, the returned frame contains a the token most suspected of breaking parsing, otherwise it contains a list of accepted parse trees.

This is all handled transparently in the `ParseSession` object.

## Full workflow :

The full workflow for parselib parser generator can be something like this:
```c++

void full_work_flow (std::string t_grammar_filename, std::string t_source_filename) {

  using namespace parselib
  
  //define preprocessor to use
  utils::OnePassPreprocessor *preproc = new utils::OnePassPreprocessor() ;
  parsers::GenericGrammarParser ggp (preproc) ; //define the grammar parser

  auto grammar = ggp.parse (t_grammar_filename, verbose) ; //..and parse
  delete preproc; // delete preprocessor, not needed anymore

  parsers::CYK parser (grammar) ; //instantiate CYK parser
  std::string source = utils::gettextfilecontent(t_source_filename) ; //load source from text file

  //tokenize source code
  lexer::Lexer tokenizer (grammar.tokens) ;
  tokenizer.tokenize (source) ;

  //result is in a Frame which is a polite term to say std::vector<parselib::parsetree::Node*>
  //containing all accepted solutions/parse trees
  //we generally use the first one
  Frame result = parser.membership (tokenizer.tokens) ;

}
```

A complete example with error handling and parsing to json can be found in *parselibinstance.h/cpp* file in `process_source_to_ptree`.

## References :

[1] Lange, Martin; Leiß, Hans (2009). "To CNF or not to CNF? An Efficient Yet Presentable Version of the CYK Algorithm". 







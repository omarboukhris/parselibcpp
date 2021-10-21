## Grammar parselib

This is intended to be a simple AND efficient (at least a lot more than its Python version) framework for context free grammar (CFG) parsing.

Can be used to code a symbolic mathematical kernel, a source-to-source transcompiler or whatever.. The sky is the limit (along with the established expressive power of type 2 grammars)

boost1.58 and cmake3.5 (at least) are required as a dependency to build the project.

### Build the project :

```bash
cd parselibcpp
mkdir build
cd build
cmake ..
make
```

### Main Generator & Test Case :

Main executable parses source code files using specified grammar and stores results in json files (should be injected in a template engine or whatever).
To test it, you can use the following shell command

```bash
cd parselibcpp
#globs all java files in data/ folder and parses them 
#following the rules of the grammar @data/grammar.grm
#result is stored in .json files where the globed files are
build/parcexlib --gsrc=data/grammar.grm --ext=java --dir=data
```

### Main interface :

#### Sessions

All functions mentioned later and more are wrapped in a utility class (`parselib.parselibinstance.ParselibInstance`).

Reading a grammar and parsing a source code then becomes trivial :
```c++
#include <parselib/parselibinstance.hpp>
#include <boost/property_tree/ptree.hpp>

int main(int argc, char** argv){
  // define a parselib session
  parselib::ParseSession parsesession ;
  bool verbose = true ;

  // load a grammar from a raw text file++
  parsesession.loadGrammar("data/grammar.grm", verbose) ;
  
  // parse some source code if parsable
  // return type is boost::property_tree::ptree 
  auto out = parsesession.processSource("data/test.java", verbose) ; 

  return 0 ;
}
```

#### Python interface 

The C++ parselib interface is exposed in python using ctypes.

An example of use could be 
```python
>>> from PyCpp.pyparse import ParseSession

>>> psess = ParseSession()
>>> psess.load_grammar("data/test/gram.grm")
(info) now processing : data/test/gram.grm

>>> json = psess.parse_to_json("data/test/source.txt")
[===========================================================>] 100%    finished
>>> print(json)
{'S': [{'S': [{'a': ['a'], 'b': ['b']}], 'a': ['a'], 'b': ['b']}]}

>>> del psession
```


#### Recursive File Glober 

This object can be used to glob recursively files and filter them by file extension.

```c++
#include <parselib/utils/io.hpp>

// setup fileGlober
parselib::utils::FileGlober fileglober ("foo/bar", "java") ;
// recursively globs all java files in foo/bar (relative path accepted)
auto files = fileglober.glob() ; 
// type is parselib::utils::FileGlober::FilesList or std::vector<std::string>
```

This can mainly be useful to setup a transcompiling framework or a template engine.

## Grammar's syntax

Let G be a CFG, such as G = (NT, T, Pr, AXIOM) with

* NT    : Set of non terminals
* T     : Set of terminals (alphabet)
* Pr    : Set of production rules ⊆ NT×(NT ∪ T)*, ∪ being the union operator and * the Kleen star operator
* AXIOM : The start symbol

example :

Let G = ({S}, {a,b}, {R1}, AXIOM) be a CFG such as :

`R1 : S → a S b | ε`

The language described by the grammar is L(G) = { a<sup>n</sup>b<sup>n</sup> }.

written following the parselib convention in a text file would look like :

```javascript
AXIOM -> S //this is a comment
S -> //S is a non terminal.
    a. S b. | // This is a production rule, '|' is the OR operator
    '' // epsilone/empty production rule
a.("a") //terminals/tokens are regex for efficiency/convenience purposes 
b.("b") //{a., b.} are terminals
```

### Terminals

Every terminal node can be written as a regex `terminalnodename.("w*")`.
They are used in production rules like `terminalnodename.`

### Non terminals

Non terminals are defined in a grammar as non terminal node.
As an example :
```javascript
nonterminal -> nonterm1 nonterm2 term. | term.
```
Here we define a non terminal node named `nonterminal` as the concatenation of non terminal nodes `nonterm1, nonterm2` and a terminal node `term.`. 

`term.` alone is also accepted.

### Labeling operators :

Each operand associated with the operators `!` or `label=` in a production rule tells the parser to save the data in a data structure formed by the production rule non terminals.

Example :
let S=(a S b | eps) our grammar. We want to save each parsed `S` in a data structure containing the informations we need from S. 
We write the grammar as follows :
```javascript
S -> 
	!a. S_child=S !b. | 
	''
```
This generates a data structure similar to this 
```c++
struct S {
	string a ;
	struct S S_child ;
	string b ;
} ;
```
in which a correctly parsed source code will eventually be stored.

### Lists :

`list` operators have been implemented in the grammar's accepted syntax.
Example :
```javascript
aListNode ->
	element1. element2. anotherNode |
aListNode aListNode | '' //this line define the node as a list
```
Can become
```javascript
aListNode ->
	element1. element2. anotherNode |
__list__ // [] is also accepted
```
The list operator basically generates a rule to be used as a loop guard for the list parsing.

For example, to parse an array of arbitrary length :
```javascript
array ->
	!element |
__list__ 
```

### Import :

It is possible to break a grammar in submodules, importable using :
```javascript
%import "submodule.grm"

AXIOM -> //...
```
The preprocessor is protected against nested imports.

All nodes in imported modules can be scoped from any other module file, as long as the 2 modules are included in the grammar project.

### Str :

If you want to convert a non terminal node's value to str instead of catching a strucutre, the `s:` operator can be used to convert the node to a string.
```javascript
//...
someHeader -> s:complexNodeToConvert theRestofit | '' //...
```
This is mainly to catch strings that regexs can't, like nested template declaration in C++ as an example.

## Under the hood (Pipeline)

### Graph encoder for generic textual CFG

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
### Operators for grammar transformation 

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
### CYK parsers for grammars in 2NF<sup>[1]</sup>

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
The parser doesn't support error handling yet though. If `membership` fails, the returned frame is empty, otherwise it contains a list of accepted parse trees.

### References :

[1] Lange, Martin; Leiß, Hans (2009). "To CNF or not to CNF? An Efficient Yet Presentable Version of the CYK Algorithm". 


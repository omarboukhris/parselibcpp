## Grammar parselib

This is intended to be a simple and efficient framework for context free grammar (CFG) parsing.

Can be used to code a symbolic mathematical kernel, a source-to-source transcompiler or whatever.. The sky is the limit (along with the established expressive power of type 2 grammars)

boost1.40 and cmake3.5 (at least) are required as a dependency to build the project.

## PyCpp 

PyCpp is an extensible gateway language between Python and C++. It is designed to generate ctypes interfaces of C++ code automatically starting using a subset of C++ language. 

### Build the project :

```bash
cd parselibcpp
mkdir build
cd build
cmake ..
make
```

Make sure when using PyCpp to link the shared object (\*.so file) appropriately to the ctype interface (*parsesession.py*).

### Main Generator & Test Case :

Main executable parses source code files using specified grammar and stores results in json files (should be injected in a template engine or whatever).
To test it, the following shell command can be used

```bash
cd parselibcpp

#globs all java files in data/ folder and parses them 
#following the rules of the grammar @data/grammar.grm
#result is stored in .json files where the globed files are
build/parcexlib --gsrc=data/grammar.grm --ext=java --dir=data

#launch python script tessting pyCpp
python3 main.py
```

### Main interfaces :

#### C++ interface

Check [ParseLib](parselib/README.md).

#### Python interface 

Check [PyCpp](PyCpp/README.md).

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
AXIOM -> S
S -> 
	tokena=a. !S tokenb=b. | 
	""
a.("a")
b.("b")
```
The string `aabb`, accepted by this grammar, generates a data structure similar to this 
```json
{
  "S" : [
    {
      "tokena" : "a",
      "S" : [
        {
	  "tokena" : "a",
	  "S" : [
	    {}
	  ],
	  "tokenb" : "b",
	}
      ],
      "tokenb" : "b"
    }
  ]
}
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
[] 
```

### Import :

It is possible to break a grammar in submodules, importable using :
```javascript
%import "submodule.grm"

AXIOM -> //...
```
The preprocessor is protected against nested imports.

All nodes in imported modules can be scoped from any other module file, as long as the 2 modules are included in the grammar project.

### String Nodes :

To convert a non terminal node's value to str instead of catching a strucutre, the `s:` operator can be used to convert the node to a string.
```javascript
//...
someHeader -> s:complexNodeToConvert theRestofit | '' //...
```
This is mainly to catch strings that regexs can't, like nested template declaration in C++ as an example.

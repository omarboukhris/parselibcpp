#pragma once

#include <parselib/datastructure/common_types.h>

namespace parselib {

namespace parsetree {

// abstract class for composite pattern
class AbsNode {
public :
	typedef std::pair<std::string, AbsNode*> Token ;
	typedef std::vector<Token> TokenList ;

	AbsNode () ;

	virtual ~AbsNode () ;

	/*!
	 * \brief getval get token value
	 * \return
	 */
	virtual std::string getval () = 0 ;

	/*!
	 * \brief strUnfold unfolds Node into string
	 * \return unfolded node into string
	 */
	std::string strUnfold() ;

	/*!
	 * \brief push_back add token to tokens lsit
	 * \param tok token
	 */
	void push_back (Token tok) ;

	/*!
	 * \brief clear tokens list
	 */
	void clear () ;

	/*!
	 * \brief begin, iterator wrapper
	 * \return iterator
	 */
	TokenList::iterator begin () ;

	/*!
	 * \brief end, iterator wrapper
	 * \return iterator
	 */
	TokenList::iterator end () ;

	/*!
	 * \brief Get tokens list size
	 * \return size as size_t
	 */
	size_t size () ;


	/*!
	 * \brief merge inplace two trees into one
	 * \param tree tree to merge with *this
	 * \return current tree
	 */
	AbsNode * merge (AbsNode *tree) ;



	std::string type ;
	TokenList tokens ;
} ;

// terminal node (a leaf in the tree)
class Leaf : public AbsNode {
public :
	Leaf (std::string s) ;
	virtual std::string getval () ;
	std::string val ;
} ;

// tree (still branching)
class Tree : public AbsNode {

public :

	//
	// Constructors
	//

	Tree() ;
	Tree(AbsNode* node) ;

	//
	// Helpers and acccessors
	//

	/*!
	 * \brief keyInTree check if the key is in the tree
	 * \param key to look for
	 * \return boolean
	 */
	size_t keyInTree (std::string key) ;

	/*!
	 * \brief getval, no token to get its value
	 * wraps strunfold
	 * \return unfolded node into string
	 */
	virtual std::string getval () ;

	//
	// stream operators
	//

	friend std::ostream & operator<< (std::ostream& out, Tree* tree) ;
	friend std::ostream & operator<< (std::ostream& out, Tree tree) ;

private :
	/*!
	 * \brief dumps content of a tree into a string
	 * \param tree tree to unfold
	 * \param tab used for recursive tab stack tracking
	 * \return tree recursive dump of *tree parameter
	 */
	std::string dump (AbsNode *tree, std::string tab="") ;
};

/*!
 * \brief abstract node : generates the parse tree
 * unfold is a lazy eval
 */
class Node {

public :

	//
	// Constructors
	//

	Node () ;

	//
	// Helpers and accessors
	//

	/*!
	 * \brief iscompacted check if node is compacted
	 * check if there are / in token name
	 * \return boolean
	 */
	bool iscompacted () ;

	/*!
	 * \brief abstract, pure virtual
	 * unfolds the Node into a parse tree : Tree*
	 * \param parent  parent node
	 * \return Tree pointer
	 */
	virtual AbsNode* unfold (std::string parent="") = 0 ;

	/// \brief node type (terminal or non terminal)
	std::string nodetype ;
} ;


class UnitNode : public Node {
public :
	UnitNode (std::string nodetype, Node* unit) ;
	virtual AbsNode* unfold (std::string parent="") ;
private :
	Node* unit ;
} ;

class TokenNode : public Node {
public :
	TokenNode(std::string nodetype, std::string val) ;
	virtual AbsNode* unfold (std::string /*parent=""*/) ;

	std::string val ;
} ;

class BinNode : public Node { 
public :
	BinNode(std::string nodetype, Node* left, Node* right) ;
	virtual AbsNode* unfold (std::string parent="") ;

 private:
	Node* left ;
	Node* right ;
} ;

} //namespace parsetree

} //namespace parselib


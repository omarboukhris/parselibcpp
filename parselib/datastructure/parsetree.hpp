#pragma once

#include <parselib/datastructure/common_types.h>

namespace parselib {

namespace parsetree {

// abstract class for composite pattern
class Tree {
public :
	typedef std::unique_ptr<Tree> TreePtr;
	typedef std::pair<std::string, Tree*> Token ;
	typedef std::vector<Token> TokenList ;

	enum class NodeType {
		Leaf,
		Branch
	};

	Tree () ;
	Tree (std::string) ;
	Tree(Tree* node) ;


	virtual ~Tree () ;

	/*!
	 * \brief getval get token value if leaf
	 * unfolds if branch
	 * \return value
	 */
	std::string getval () ;

	/*!
	 * \brief keyInTree check if the key is in the tree
	 * \param key to look for
	 * \return position if found, -1 otherwise
	 */
	int keyInTree (std::string key) ;

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
	Tree * merge (Tree *tree) ;

	friend std::ostream & operator<< (std::ostream& out, Tree* tree) ;
	friend std::ostream & operator<< (std::ostream& out, Tree tree) ;

	NodeType type ;
	std::string val ;
	TokenList tokens ;

protected:

	/*!
	 * \brief dumps content of a tree into a string
	 * \param tree tree to unfold
	 * \param tab used for recursive tab stack tracking
	 * \return tree recursive dump of *tree parameter
	 */
	std::string dump (Tree *tree, std::string tab="") ;

} ;

/*!
 * \brief abstract node : generates the parse tree
 * unfold is a lazy eval
 */
class Node {

public :

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
	virtual Tree* unfold (std::string parent="") = 0 ;

	/// \brief node type (terminal or non terminal)
	std::string nodetype ;
} ;


class UnitNode : public Node {
public :
	UnitNode (std::string nodetype, Node* unit) ;
	virtual Tree* unfold (std::string parent="") ;
private :
	Node* unit ;
} ;

class TokenNode : public Node {
public :
	TokenNode(std::string nodetype, std::string val) ;
	virtual Tree* unfold (std::string /*parent=""*/) ;

	std::string val ;
} ;

class BinNode : public Node { 
public :
	BinNode(std::string nodetype, Node* left, Node* right) ;
	virtual Tree* unfold (std::string parent="") ;

 private:
	Node* left ;
	Node* right ;
} ;

} //namespace parsetree

} //namespace parselib


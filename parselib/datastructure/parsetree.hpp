#pragma once

#include <parselib/datastructure/common_types.h>

namespace parselib {

namespace parsetree {

// abstract class for composite pattern
class AbsNode {
public :
	typedef std::pair<std::string, AbsNode*> Token ;
	typedef std::vector<Token> TokenList ;

	enum class NodeType {
		Leaf,
		Branch
	};

	AbsNode () ;
	AbsNode (std::string) ;
	AbsNode(AbsNode* node) ;


	virtual ~AbsNode () ;

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
	AbsNode * merge (AbsNode *tree) ;

	friend std::ostream & operator<< (std::ostream& out, AbsNode* tree) ;
	friend std::ostream & operator<< (std::ostream& out, AbsNode tree) ;

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
	std::string dump (AbsNode *tree, std::string tab="") ;

} ;

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


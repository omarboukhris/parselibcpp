#pragma once

#include <parselib/datastructure/common_types.h>

namespace parselib {

namespace parsetree {

// Tree class for composite pattern
class Tree {
public :
	typedef std::shared_ptr<Tree> TreePtr;
	typedef std::pair<std::string, TreePtr> Token ;
	typedef std::vector<Token> TokenList ;

	enum class NodeType {
		Leaf,
		Branch
	};

	Tree () ;
	Tree (std::string) ;
	Tree(Tree::TreePtr node) ;
	Tree(Tree* node) ;


	virtual ~Tree () ;

	/*!
	 * \brief tokens : accessor to list of tokens
	 * \return reference to list of tokens
	 */
	TokenList &tokens() ;

	/*!
	 * \brief type : accessor to node type, leaf or branch
	 * \return leaf or branch, by value
	 */
	NodeType type() ;

	/*!
	 * \brief val get token value if leaf
	 * unfolds if branch
	 * \return value
	 */
	std::string val () ;

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
	TreePtr merge (TreePtr tree) ;

	friend std::ostream & operator<< (std::ostream& out, TreePtr tree) ;
	friend std::ostream & operator<< (std::ostream& out, Tree tree) ;

protected:

	/*!
	 * \brief dumps content of a tree into a string
	 * \param tree tree to unfold
	 * \param tab used for recursive tab stack tracking
	 * \return tree recursive dump of *tree parameter
	 */
	std::string dump (TreePtr tree, std::string tab="") ;

	NodeType m_type ; ///< Node type : leaf or branch
	std::string m_val ; ///< value if leaf
	TokenList m_tokens ; ///< list of tokens if branch
} ;

using TreePtr = Tree::TreePtr;
using NodeType = Tree::NodeType;
using TokenList = Tree::TokenList;

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
	 * unfolds the Node into a parse tree : Tree::TreePtr
	 * \param parent  parent node
	 * \return Tree pointer
	 */
	virtual TreePtr unfold (std::string parent="") = 0 ;

	/// \brief node type (terminal or non terminal)
	std::string nodetype ;
} ;

typedef std::shared_ptr<Node> NodePtr ;

/*!
 * \brief The UnitNode class represents a unit node with the form
 * ProductionRule_a -> ProductionRule_b
 */
class UnitNode : public Node {
public :

	UnitNode (UnitNode *other) ;
	UnitNode (std::string nodetype, NodePtr unit) ;

	/*!
	 * \brief applies lazy node unfolding of unit node U1 -> U2 into
	 * a Tree datastructure
	 * \param parent node name
	 * \return unfolded TreePtr (parsable into pt::ptree/json)
	 */
	virtual TreePtr unfold (std::string parent="") ;
private :
	NodePtr unit ;
} ;

/*!
 * \brief The TokenNode class represents a token
 * It is the terminal datastructure in a parsing graph
 */
class TokenNode : public Node {
public :

	TokenNode(TokenNode *other);
	TokenNode(std::string nodetype, std::string val) ;

	/*!
	 * \brief unfolds the terminal graph node into a terminal Tree node
	 * \return Terminal Tree node
	 */
	virtual TreePtr unfold (std::string /*parent=""*/) ;

	std::string val ;
} ;

/*!
 * \brief The BinNode class represents a binary production rule
 * A -> A1 A2
 */
class BinNode : public Node {
public :

	BinNode(BinNode *other);
	BinNode(std::string nodetype, NodePtr left, NodePtr right) ;

	/*!
	 * \brief unfolds binary production rule into a parse Tree
	 * \param parent node name
	 * \return TreePtr parsable into boost ptree or json
	 */
	virtual TreePtr unfold (std::string parent="") ;

 private:

	NodePtr left ; ///< left node lhs
	NodePtr right ; ///< right node lrs
} ;

} //namespace parsetree

} //namespace parselib


#include <parselib/datastructure/parsetree.hpp>
#include <parselib/utils/io.hpp>

namespace parselib {
namespace parsetree {

AbsNode::AbsNode () : type("tree") {
	tokens = std::vector<Token> () ;
} 

void AbsNode::push_back(AbsNode::Token tok) {
	tokens.push_back(tok) ;
}

void AbsNode::clear() {
	tokens.clear() ;
}

AbsNode::TokenList::iterator AbsNode::begin() {
	return tokens.begin() ;
}

AbsNode::TokenList::iterator AbsNode::end() {
	return tokens.end() ;
}

size_t AbsNode::size() {
	return tokens.size() ;
}



std::string AbsNode::strUnfold() {
	std::string ss = "" ;
	for (AbsNode::Token tok : tokens) {
		if (tok.second->type == "leaf") {
			ss += tok.second->getval() + " ";
		} else { //type is tree
			ss += tok.second->strUnfold() ;
		}
	}
	return ss ;
}


Leaf::Leaf(std::string s) {
	type = "leaf" ;
	val = s ;
}

std::string Leaf::getval() {
	return val ;
}


Tree::Tree() {
	tokens = AbsNode::TokenList () ;
}
Tree::Tree(AbsNode* node) {
	tokens = AbsNode::TokenList () ;
	for (Token tok : node->tokens) {
		tokens.push_back(tok);
	}
}

Tree* Tree::merge(Tree* tree) {
	for (Token leaf : *tree) {
		tokens.push_back(leaf);
	}
	return this ;
}

size_t Tree::keyInTree(std::string key) {
	size_t i = 0 ;
	for (auto item : tokens) {
		std::string tokentype = item.first ;
		if (tokentype == key) {
			return i ;
		}
		i++ ;
	}
	return -1 ; 
}

Tree Tree::at(std::string key) {
	Tree* out = new Tree() ;
	Tree* tmpout = new Tree() ;
	for (Token tok : tokens) {
		std::string tokkey = tok.first ;
		if (tokkey == key) {
			if (tok.second->type == "leaf") {
				out->push_back(tok);
			} else {
				Tree *tree = new Tree () ;
				for (Token tokout : tok.second->tokens) {
					tree->push_back(tokout);
					out->push_back(tokout);
				}
				Token token = std::make_pair(key, tree) ;
				tmpout->push_back(token) ;
			}
		}
	}
	// tmpout is the result we want
// 	std::cout << dump(tmpout) << std::endl ;
	return *out ;
}

Tree Tree::operator[] (const char key[]) {
// 	std::cout << dump(this) ;
	utils::StrList keylist = utils::split(key, "/") ;
	if (keylist.size() == 1) {
		return this->at(keylist[0]) ;
	} else {
		Tree tree = this->at(keylist[0]) ;
		keylist.erase(keylist.begin()) ;
		std::string newkey = utils::join(keylist, "/") ;
		return tree[newkey.c_str()] ;
	}		
}

std::string Tree::getval() {
	return "" ;
}

// Tree* Tree::operator[] (int i) {
// 	Tree* out = new Tree() ;
// 	out->push_back(tokens[i]) ;
// 	return out ;
// }

std::ostream& operator<<(std::ostream& out, Tree* tree) {
	out << tree->dump(tree) ;
	return out ;
}
std::ostream& operator<<(std::ostream& out, Tree tree) {
	out << tree.dump(&tree) ;
	return out ;
}

std::string Tree::dump(AbsNode* tree, std::string tab) {
	std::string ss = "" ;
	for (AbsNode::Token tok : tree->tokens) {
		if (tok.second->type == "leaf") {
			ss += tab + "(" + tok.first + ":" + tok.second->getval() + ")\n";
		} else {
			ss += tab + tok.first + " = {\n" 
				+ dump(tok.second, tab+"\t")  
				+ tab + "}\n" ;
		}
	}
	return ss ;
}


Node::Node() {
	nodetype = "" ;
}
bool Node::iscompacted() {
	return (nodetype.find("/") != std::string::npos) ;
}


UnitNode::UnitNode(std::string nodetype, Node* unit) {
	this->nodetype = nodetype ;
	this->unit = unit ;
}

Tree* UnitNode::unfold(std::string parent){
	if (iscompacted() || parent == nodetype) {
		return unit->unfold(nodetype) ;
	} else { //if parent != None :
		//insert token in a tree and return it
		Tree *tree = new Tree() ;
		tree->push_back (
			AbsNode::Token(nodetype, unit->unfold(nodetype))
		) ;
		return tree ;
	}
}


TokenNode::TokenNode(std::string nodetype, std::string val) {
	this->val = val ;
	this->nodetype = nodetype ;
}

Tree* TokenNode::unfold(std::string) {
	Tree *tree = new Tree() ;
	tree->push_back(
		AbsNode::Token(
			nodetype, 
			new Leaf(val)
		)
	) ;
	return tree ;
}


BinNode::BinNode(std::string nodetype, Node* left, Node* right) {
	this->left = left ; 
	this->right = right ; 
	this->nodetype = nodetype ;
}

Tree* BinNode::unfold(std::string parent) {
	Tree* tree = left->unfold(nodetype) ;
	tree->merge(right->unfold(nodetype)) ;
	if (iscompacted() || parent == nodetype) { 
		return tree ;
	} else {
		AbsNode::Token tok = std::make_pair(nodetype, tree) ;
		Tree* treeout = new Tree() ;
		treeout->push_back(tok);
		return treeout ;
	}
}

}
}

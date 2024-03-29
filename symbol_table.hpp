#ifndef JEFF_SYMBOL_TABLE_HPP
#define JEFF_SYMBOL_TABLE_HPP
#include <string>
#include <unordered_map>
#include <list>
#include "ast.hpp"

//Use an alias template so that we can use
// "HashMap" and it means "std::unordered_map"
template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

using namespace std;

namespace jeff{

//A semantic symbol, which represents a single
// variable, function, etc. Semantic symbols 
// exist for the lifetime of a scope in the 
// symbol table. 
class SemSymbol {
	//TODO add the fields that 
	// each semantic symbol should track
	// (i.e. the kind of the symbol (either a variable or function)
	// and functions to get/set those fields
	public:
	SemSymbol(IDNode* idIn, std::string kindIn, TypeNode* typeIn)
	: id(idIn), kind(kindIn), type(typeIn) {}
	SemSymbol(IDNode* idIn, std::string kindIn, TypeNode* typeIn, std::list<FormalDeclNode*>* formalsIn)
	: id(idIn), kind(kindIn), type(typeIn), formals(formalsIn) {}
	IDNode* id;
	std::string kind;
	TypeNode* type;
	std::list<FormalDeclNode*>* formals;
	IDNode* getId() { return id; }
	std::string getKind() { return kind; }
	TypeNode* getType() { return type; }

};

//A single scope. The symbol table is broken down into a 
// chain of scope tables, and each scope table holds 
// semantic symbols for a single scope. For example,
// the globals scope will be represented by a ScopeTable,
// and the contents of each function can be represented by
// a ScopeTable.
class ScopeTable {
	public:
		ScopeTable();
		//TODO: add functions for looking up symbols
		// and/or returning information to indicate
		// that the symbol does not exist within the
		// current scope.
		void addToScope(SemSymbol* s);
		SemSymbol* lookup(std::string symbol, std::string kind);
	private:
		HashMap<std::string, SemSymbol *> * symbols;
};

class SymbolTable{
	public:
		SymbolTable();
		//TODO: add functions to create a new ScopeTable
		// when a new scope is entered, drop a ScopeTable
		// when a scope is exited, etc. 
		void addScope();
		void dropScope();
		void addToScope(SemSymbol* s);
		SemSymbol* lookup(std::string symbol, std::string kind);
	private:
		std::list<ScopeTable *> * scopeTableChain;
};

	
}

#endif

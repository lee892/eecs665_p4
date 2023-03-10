#include "symbol_table.hpp"
namespace jeff{

ScopeTable::ScopeTable(){
	symbols = new HashMap<std::string, SemSymbol *>();
}

void ScopeTable::addToScope(SemSymbol* s) {
	(*symbols)[s->getName()] = s;
}

bool ScopeTable::lookup(std::string symbol) {
	if (symbols->find(symbol) == symbols->end()) {
		return false;
	}
	return true;
}

SymbolTable::SymbolTable(){
	//TODO: implement the list of hashtables approach
	// to building a symbol table:
	// Upon entry to a scope a new scope table will be 
	// entered into the front of the chain and upon exit the 
	// latest scope table will be removed from the front of 
	// the chain.
	scopeTableChain = new std::list<ScopeTable *>();
}

void SymbolTable::addScope() {
	ScopeTable* table = new ScopeTable();
	scopeTableChain->push_front(table);
}

void SymbolTable::dropScope() {
	scopeTableChain->pop_front();
}

void SymbolTable::addToScope(SemSymbol* s) {
	scopeTableChain->front()->addToScope(s);
}

bool SymbolTable::lookup(std::string symbol) {
	for (auto scope : *scopeTableChain) {
		if (scope->lookup(symbol)) {
			return true;
		}
	}
	return false;
}

}

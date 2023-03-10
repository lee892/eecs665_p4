#include "symbol_table.hpp"
#include <iostream>
namespace jeff{

ScopeTable::ScopeTable(){
	symbols = new HashMap<std::string, SemSymbol *>();
}

void ScopeTable::addToScope(SemSymbol* s) {
	std::cout << s->getId()->getName() << "\n";
	std::cout << s->getKind() << "\n";
	(*symbols)[s->getId()->getName()] = s;
}

SemSymbol* ScopeTable::lookup(std::string symbol, std::string kind) {
	if (symbols->find(symbol) == symbols->end()) {
		return nullptr;
	}
	SemSymbol* found = (*symbols)[symbol];
	if (found->getKind() == kind) {
		return found;
	}
	return nullptr;
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

SemSymbol* SymbolTable::lookup(std::string symbol, std::string kind) {
	SemSymbol* s;
	for (auto scope : *scopeTableChain) {
		s = scope->lookup(symbol, kind);
		if (s != nullptr) {
			return s;
		}
	}
	return s;
}

}

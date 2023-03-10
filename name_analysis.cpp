#include "ast.hpp"
#include "symbol_table.hpp"
#include "errName.hpp"

namespace jeff{

//TODO here is a subset of the nodes needed to do nameAnalysis, 
// you should add the rest to allow for a complete treatment
// of any AST

bool ASTNode::nameAnalysis(SymbolTable * symTab){
	throw new ToDoError("This function should have"
		"been overriden in the subclass!");
}

bool ProgramNode::nameAnalysis(SymbolTable * symTab){
	bool res = true;
	symTab->addScope();
	for (auto global : *myGlobals){
		res = global->nameAnalysis(symTab) && res;
	}
	symTab->dropScope();
	return res;
}

bool IndexNode::nameAnalysis(SymbolTable * symTab) {
	return true;
}

bool IDNode::nameAnalysis(SymbolTable * symTab) {
	bool nameAnalysisOk = true;
	SemSymbol* s = symTab->lookup(name, "var");
	if (s == nullptr) {
		NameErr::undeclID(this->pos());
		nameAnalysisOk = false;
	} else {
		attachSymbol(s);
	}
	return nameAnalysisOk;
}

bool VarDeclNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
	//check if the delcaration is well-typed
	std::string type = getTypeNode()->name;
	if (type == "void") {
		NameErr::badVarType(this->ID()->pos());
		nameAnalysisOk = false;
	} else if (symTab->lookup(ID()->getName(), "var")) {
		NameErr::multiDecl(ID()->pos());
		nameAnalysisOk = false;
	} else {
		SemSymbol* s = new SemSymbol(ID(), "var", getTypeNode());
		symTab->addToScope(s);
		ID()->attachSymbol(s);
	}
	
	/*std::cerr << "I'm in VarDeclNode throwing an error."
		<< " use GDB or this will look like a segfault!" << std::endl;
	std::cerr << std::flush;
	throw new ToDoError("[DELETE ME] I'm a varDecl"
		" you should add the information from my"	
		" subtree to the symbolTable as a new"	
		" entry in the current scope table"
	);*/
	return nameAnalysisOk;
}

bool FnDeclNode::nameAnalysis(SymbolTable * symTab){
	bool nameAnalysisOk = true;
	/*throw new ToDoError("[DELETE ME] I'm an fnDecl."
		" you should add and make current a new"	
		" scope table for my body"
	);*/
	if (symTab->lookup(ID()->getName(), "fn")) {
		NameErr::multiDecl(ID()->pos());
		nameAnalysisOk = false;
	} else {
		SemSymbol* s = new SemSymbol(ID(), "fn", myRetType, myFormals);
		symTab->addToScope(s);
		ID()->attachSymbol(s);
	}
	
	symTab->addScope();
	for (auto statement : *myBody) {
		nameAnalysisOk = statement->nameAnalysis(symTab) && nameAnalysisOk;
	}
	symTab->dropScope();
	return nameAnalysisOk;
}

bool AssignStmtNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	nameAnalysisOk = myDst->nameAnalysis(symTab);
	nameAnalysisOk = mySrc->nameAnalysis(symTab) && nameAnalysisOk;
	return nameAnalysisOk;
}

bool OpenStmtNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool CloseStmtNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool ReadStmtNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool WriteStmtNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool PostDecStmtNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool PostIncStmtNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool WhileStmtNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool IfElseStmtNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool IfStmtNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool ReturnStmtNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool CallExpNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool PlusNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool MinusNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool TimesNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool DivideNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool AndNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool OrNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool EqualsNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool NotEqualsNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool LessNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool LessEqNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool GreaterNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool GreaterEqNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool UnaryExpNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool NegNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool NotNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool TernaryExpNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool IntLitNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool StrLitNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool TrueNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool FalseNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool CallStmtNode::nameAnalysis(SymbolTable* symTab) {
	return true;
}

bool IntTypeNode::nameAnalysis(SymbolTable* symTab){
	// Name analysis may never even recurse down to IntTypeNode,
	// but if it does, just return true to indicate that 
	// name analysis has not failed, and add nothing to the symbol table
	return true;
}
}

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
	bool nameAnalysisOk = true;
	handle->nameAnalysis(symTab);

	return nameAnalysisOk;
}

bool CloseStmtNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	handle->nameAnalysis(symTab);

	return nameAnalysisOk;
}

bool ReadStmtNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myHandle->nameAnalysis(symTab);
	myDst->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool WriteStmtNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myHandle->nameAnalysis(symTab);
	mySrc->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool PostDecStmtNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myLoc->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool PostIncStmtNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myLoc->nameAnalysis(symTab);
	return nameAnalysisOk;
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
	bool nameAnalysisOk = true;
	myExp->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool CallExpNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myID->nameAnalysis(symTab);
	for (auto exp : *myArgs) {
		exp->nameAnalysis(symTab);
	}
	return nameAnalysisOk;
}

bool PlusNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myExp1->nameAnalysis(symTab);
	myExp2->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool MinusNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myExp1->nameAnalysis(symTab);
	myExp2->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool TimesNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myExp1->nameAnalysis(symTab);
	myExp2->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool DivideNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myExp1->nameAnalysis(symTab);
	myExp2->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool AndNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myExp1->nameAnalysis(symTab);
	myExp2->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool OrNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myExp1->nameAnalysis(symTab);
	myExp2->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool EqualsNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myExp1->nameAnalysis(symTab);
	myExp2->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool NotEqualsNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myExp1->nameAnalysis(symTab);
	myExp2->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool LessNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myExp1->nameAnalysis(symTab);
	myExp2->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool LessEqNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myExp1->nameAnalysis(symTab);
	myExp2->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool GreaterNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myExp1->nameAnalysis(symTab);
	myExp2->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool GreaterEqNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myExp1->nameAnalysis(symTab);
	myExp2->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool NegNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myExp->nameAnalysis(symTab);
	return nameAnalysisOk;
}

bool NotNode::nameAnalysis(SymbolTable* symTab) {
	bool nameAnalysisOk = true;
	myExp->nameAnalysis(symTab);
	return nameAnalysisOk;
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

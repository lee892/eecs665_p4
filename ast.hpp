#ifndef JEFF_AST_HPP
#define JEFF_AST_HPP

#include <ostream>
#include <sstream>
#include <string.h>
#include <list>
#include "tokens.hpp"

namespace jeff {

class NameAnalysis;

class SymbolTable;
class SemSymbol;

class DeclNode;
class VarDeclNode;
class StmtNode;
class FormalDeclNode;
class TypeNode;
class ExpNode;
class IDNode;

class ASTNode{
public:
	ASTNode(const Position * pos) : myPos(pos){ }
	virtual void unparse(std::ostream&, int) = 0;
	const Position * pos() { return myPos; };
	std::string posStr(){ return pos()->span(); }
	virtual bool nameAnalysis(SymbolTable *);
protected:
	const Position * myPos = nullptr;
};

class ProgramNode : public ASTNode{
public:
	ProgramNode(std::list<DeclNode *> * globalsIn);
	void unparse(std::ostream&, int) override;
	virtual bool nameAnalysis(SymbolTable *) override;
private:
	std::list<DeclNode *> * myGlobals;
};

class ExpNode : public ASTNode{
protected:
	ExpNode(const Position * p) : ASTNode(p){ }
public:
	virtual void unparseNested(std::ostream& out);
};

class LocNode : public ExpNode{
public:
	LocNode(const Position * p)
	: ExpNode(p){}
	void attachSymbol(SemSymbol * symbolIn);
	SemSymbol * getSymbol() const { return mySymbol; }
	virtual bool nameAnalysis(SymbolTable * symTab) override = 0;

private:
	SemSymbol * mySymbol;
};

class IndexNode : public LocNode {
public:
	IndexNode(const Position * p, IDNode * idIn, 
	  ExpNode * expIn)
	: LocNode(p), id(idIn), exp(expIn){}
	void unparse(std::ostream& out, int indent) override;
	void unparseNested(std::ostream& out) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	IDNode * id;
	ExpNode * exp;
};

class IDNode : public LocNode{
public:
	IDNode(const Position * p, std::string nameIn)
	: LocNode(p), name(nameIn), mySymbol(nullptr){}
	std::string getName(){ return name; }
	std::string getKind(){ return kind; }
	void unparse(std::ostream& out, int indent) override;
	void unparseNested(std::ostream& out) override;
	void attachSymbol(SemSymbol * symbolIn) { mySymbol = symbolIn; }
	SemSymbol * getSymbol() const { return mySymbol; }
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	std::string name;
	std::string kind;
	SemSymbol * mySymbol;
};

class TypeNode : public ASTNode{
public:
	std::string name;
	TypeNode(const Position * p) : ASTNode(p){ }
	void unparse(std::ostream&, int) override = 0;
};

class StmtNode : public ASTNode{
public:
	StmtNode(const Position * p) : ASTNode(p){ }
	virtual void unparse(std::ostream& out, int indent) override = 0;
	virtual bool nameAnalysis(SymbolTable * symTab) override = 0;
};

class DeclNode : public StmtNode{
public:
	DeclNode(const Position * p) : StmtNode(p){ }
	void unparse(std::ostream& out, int indent) override =0;
};

class VarDeclNode : public DeclNode{
public:
	VarDeclNode(const Position * p, TypeNode * typeIn, IDNode * IDIn)
	: DeclNode(p), myType(typeIn), myID(IDIn){ }
	void unparse(std::ostream& out, int indent) override;
	IDNode * ID(){ return myID; }
	TypeNode * getTypeNode() const{ return myType; }
	bool nameAnalysis(SymbolTable * symTab) override;
private:
	TypeNode * myType;
	IDNode * myID;
};

class FormalDeclNode : public VarDeclNode{
public:
	FormalDeclNode(const Position * p, TypeNode * type, IDNode * id)
	: VarDeclNode(p, type, id){ }
	void unparse(std::ostream& out, int indent) override;
};

class FnDeclNode : public DeclNode{
public:
	FnDeclNode(const Position * p,
	  std::list<FormalDeclNode *> * formalsIn,
	  TypeNode * retTypeIn,
	  IDNode * idIn,
	  std::list<StmtNode *> * bodyIn)
	: DeclNode(p), myRetType(retTypeIn), myID(idIn),
	  myFormals(formalsIn), myBody(bodyIn){
	}
	IDNode * ID() const { return myID; }
	std::list<FormalDeclNode *> * getFormals() const{
		return myFormals;
	}
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	TypeNode * myRetType;
	IDNode * myID;
	std::list<FormalDeclNode *> * myFormals;
	std::list<StmtNode *> * myBody;
};

class AssignStmtNode : public StmtNode{
public:
	AssignStmtNode(const Position * p, LocNode * inDst, ExpNode * inSrc)
	: StmtNode(p), myDst(inDst), mySrc(inSrc){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	LocNode * myDst;
	ExpNode * mySrc;
};

class OpenStmtNode : public StmtNode{
public:
	OpenStmtNode(const Position * p, LocNode * inHandle,
	  std::string inPath, bool inIsReadOnly)
	: StmtNode(p), handle(inHandle), path(inPath),
	  isReadOnly(inIsReadOnly){}
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	LocNode * handle;
	std::string path;
	bool isReadOnly;
};

class CloseStmtNode : public StmtNode{
public:
	CloseStmtNode(const Position * p, LocNode * inHandle)
	: StmtNode(p), handle(inHandle){}
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	LocNode * handle;
};

class ReadStmtNode : public StmtNode{
public:
	ReadStmtNode(const Position * p, LocNode * handleIn, LocNode * dstIn)
	: StmtNode(p), myHandle(handleIn), myDst(dstIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	LocNode * myHandle;
	LocNode * myDst;
};

class WriteStmtNode : public StmtNode{
public:
	WriteStmtNode(const Position * p, LocNode * handleIn, ExpNode * srcIn)
	: StmtNode(p), myHandle(handleIn), mySrc(srcIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	LocNode * myHandle;
	ExpNode * mySrc;
};

class PostDecStmtNode : public StmtNode{
public:
	PostDecStmtNode(const Position * p, LocNode * inLoc)
	: StmtNode(p), myLoc(inLoc){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	LocNode * myLoc;
};

class PostIncStmtNode : public StmtNode{
public:
	PostIncStmtNode(const Position * p, LocNode * inLoc)
	: StmtNode(p), myLoc(inLoc){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	LocNode * myLoc;
};

class IfStmtNode : public StmtNode{
public:
	IfStmtNode(const Position * p, ExpNode * condIn,
	  std::list<StmtNode *> * bodyIn)
	: StmtNode(p), myCond(condIn), myBody(bodyIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	ExpNode * myCond;
	std::list<StmtNode *> * myBody;
};

class IfElseStmtNode : public StmtNode{
public:
	IfElseStmtNode(const Position * p, ExpNode * condIn,
	  std::list<StmtNode *> * bodyTrueIn,
	  std::list<StmtNode *> * bodyFalseIn)
	: StmtNode(p), myCond(condIn),
	  myBodyTrue(bodyTrueIn), myBodyFalse(bodyFalseIn) { }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	ExpNode * myCond;
	std::list<StmtNode *> * myBodyTrue;
	std::list<StmtNode *> * myBodyFalse;
};

class WhileStmtNode : public StmtNode{
public:
	WhileStmtNode(const Position * p, ExpNode * condIn,
	  std::list<StmtNode *> * bodyIn)
	: StmtNode(p), myCond(condIn), myBody(bodyIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	ExpNode * myCond;
	std::list<StmtNode *> * myBody;
};

class ReturnStmtNode : public StmtNode{
public:
	ReturnStmtNode(const Position * p, ExpNode * exp)
	: StmtNode(p), myExp(exp){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	ExpNode * myExp;
};

class CallExpNode : public ExpNode{
public:
	CallExpNode(const Position * p, IDNode * id,
	  std::list<ExpNode *> * argsIn)
	: ExpNode(p), myID(id), myArgs(argsIn){ }
	void unparse(std::ostream& out, int indent) override;
	void unparseNested(std::ostream& out) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	IDNode * myID;
	std::list<ExpNode *> * myArgs;
};

class BinaryExpNode : public ExpNode{
public:
	BinaryExpNode(const Position * p, ExpNode * lhs, ExpNode * rhs)
	: ExpNode(p), myExp1(lhs), myExp2(rhs) { }
protected:
	ExpNode * myExp1;
	ExpNode * myExp2;
};

class PlusNode : public BinaryExpNode{
public:
	PlusNode(const Position * p, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(p, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
};

class MinusNode : public BinaryExpNode{
public:
	MinusNode(const Position * p, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(p, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
};

class TimesNode : public BinaryExpNode{
public:
	TimesNode(const Position * p, ExpNode * e1In, ExpNode * e2In)
	: BinaryExpNode(p, e1In, e2In){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
};

class DivideNode : public BinaryExpNode{
public:
	DivideNode(const Position * p, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(p, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
};

class AndNode : public BinaryExpNode{
public:
	AndNode(const Position * p, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(p, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
};

class OrNode : public BinaryExpNode{
public:
	OrNode(const Position * p, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(p, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
};

class EqualsNode : public BinaryExpNode{
public:
	EqualsNode(const Position * p, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(p, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
};

class NotEqualsNode : public BinaryExpNode{
public:
	NotEqualsNode(const Position * p, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(p, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
};

class LessNode : public BinaryExpNode{
public:
	LessNode(const Position * p, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(p, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
};

class LessEqNode : public BinaryExpNode{
public:
	LessEqNode(const Position * pos, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(pos, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
};

class GreaterNode : public BinaryExpNode{
public:
	GreaterNode(const Position * p, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(p, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
};

class GreaterEqNode : public BinaryExpNode{
public:
	GreaterEqNode(const Position * p, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(p, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
};

class UnaryExpNode : public ExpNode {
public:
	UnaryExpNode(const Position * p, ExpNode * expIn)
	: ExpNode(p){
		this->myExp = expIn;
	}
	virtual void unparse(std::ostream& out, int indent) override = 0;
	virtual bool nameAnalysis(SymbolTable * symTab) override = 0;
protected:
	ExpNode * myExp;
};

class NegNode : public UnaryExpNode{
public:
	NegNode(const Position * p, ExpNode * exp)
	: UnaryExpNode(p, exp){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
};

class NotNode : public UnaryExpNode{
public:
	NotNode(const Position * p, ExpNode * exp)
	: UnaryExpNode(p, exp){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
};

class FileTypeNode : public TypeNode{
public:
	std::string name = "file";
	FileTypeNode(const Position * p) : TypeNode(p){}
	void unparse(std::ostream& out, int indent) override;
};

class VoidTypeNode : public TypeNode{
public:
	std::string name = "void";
	VoidTypeNode(const Position * p) : TypeNode(p){}
	void unparse(std::ostream& out, int indent) override;
};

class IntTypeNode : public TypeNode{
public:
	std::string name = "int";
	IntTypeNode(const Position * p): TypeNode(p){}
	void unparse(std::ostream& out, int indent) override;
	bool nameAnalysis(SymbolTable *) override;
};

class BoolTypeNode : public TypeNode{
public:
	std::string name = "bool";
	BoolTypeNode(const Position * p): TypeNode(p) { }
	void unparse(std::ostream& out, int indent) override;
};

class ArrayTypeNode : public TypeNode{
public:
	std::string name = "array";
	ArrayTypeNode(const Position * p, TypeNode * base, size_t len): TypeNode(p), myLen(len), myBase(base){}
	void unparse(std::ostream& out, int indent) override;
private:
	size_t myLen;
	TypeNode * myBase;
};

class TernaryExpNode : public ExpNode{
public:
	TernaryExpNode(const Position * p, ExpNode * inCond,
	  ExpNode * inExpTrue, ExpNode * inExpFalse)
	: ExpNode(p), cond(inCond), expTrue(inExpTrue), 
	  expFalse(inExpFalse){}
	virtual void unparseNested(std::ostream& out) override{
		unparse(out, 0);
	}
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	ExpNode * cond;
	ExpNode * expTrue;
	ExpNode * expFalse;
};

class IntLitNode : public ExpNode{
public:
	IntLitNode(const Position * p, const int numIn)
	: ExpNode(p), myNum(numIn){ }
	virtual void unparseNested(std::ostream& out) override{
		unparse(out, 0);
	}
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	const int myNum;
};

class StrLitNode : public ExpNode{
public:
	StrLitNode(const Position * p, const std::string strIn)
	: ExpNode(p), myStr(strIn){ }
	virtual void unparseNested(std::ostream& out) override{
		unparse(out, 0);
	}
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	 const std::string myStr;
};

class TrueNode : public ExpNode{
public:
	TrueNode(const Position * p): ExpNode(p){ }
	virtual void unparseNested(std::ostream& out) override{
		unparse(out, 0);
	}
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
};

class FalseNode : public ExpNode{
public:
	FalseNode(const Position * p): ExpNode(p){ }
	virtual void unparseNested(std::ostream& out) override{
		unparse(out, 0);
	}
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
};

class CallStmtNode : public StmtNode{
public:
	CallStmtNode(const Position * p, CallExpNode * expIn)
	: StmtNode(p), myCallExp(expIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual bool nameAnalysis(SymbolTable * symTab) override;
private:
	CallExpNode * myCallExp;
};

} //End namespace jeff

#endif


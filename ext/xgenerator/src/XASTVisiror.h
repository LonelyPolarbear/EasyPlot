#pragma once
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/AST/RecursiveASTVisitor.h"

class XASTVisitor : public clang::RecursiveASTVisitor<XASTVisitor> {
public:
	explicit XASTVisitor(clang::Rewriter* writer);

	bool VisitFunctionDecl(clang::FunctionDecl* FD);

	bool VisitCXXRecordDecl(clang::CXXRecordDecl* RD);

	const char* getAccessSpecifierName(clang::AccessSpecifier AS) const;
protected:
	clang::Rewriter* mWriter=nullptr;
};
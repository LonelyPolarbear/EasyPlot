#include "XASTConsumer.h"
#include "XASTVisiror.h"

XASTConsumer::XASTConsumer(clang::Rewriter* writer):mWriter(writer)
{
	mVisitor = new XASTVisitor(writer);
}

void XASTConsumer::HandleTranslationUnit(clang::ASTContext& Ctx)
{
	mVisitor->TraverseDecl(Ctx.getTranslationUnitDecl());
}

void XASTConsumer::Initialize(clang::ASTContext& Context)
{
	llvm::outs() << "Initialize\n";
}

#pragma once
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <nlohmann/json.hpp>

class XASTVisitor : public clang::RecursiveASTVisitor<XASTVisitor> {
public:
	explicit XASTVisitor(clang::Rewriter* writer, nlohmann::json* config,std::ofstream *osm);

	/**
	 * @ brief 访问函数声明，是分析函数的入口点
	 */
	bool VisitFunctionDecl(clang::FunctionDecl* FD);

	/**
	 * @brief 访问变量声明
	 */
	bool VisitVarDecl(clang::VarDecl* VD);

	/**
	 * @brief 访问通用语句
	 */
	bool VisitStmt(clang::Stmt* S);

	/**
	 * @brief 访问复合语句（即 {...} 代码块）
	 */
	bool VisitCompoundStmt(clang::CompoundStmt* CS);

	/**
	 * @ brief 访问 C++ 类、结构体或联合体声明
	 */
	bool VisitCXXRecordDecl(clang::CXXRecordDecl* RD);

	const char* getAccessSpecifierName(clang::AccessSpecifier AS) const;

protected:
	clang::Rewriter* mWriter=nullptr;
	nlohmann::json *mConfig=nullptr;
	std::ofstream *mOSM=nullptr;
};
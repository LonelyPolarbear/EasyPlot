#pragma once
#include "clang/AST/ASTConsumer.h"
#include "clang/Rewrite/Core/Rewriter.h"


class XASTVisitor;
/**
 * @brief ASTConsumer是开发者的核心钩子，当Clang完成语义分析、生成完整的AST后
 * 会自动调用ASTConsumer的回调函数，将AST的根节点传递给自定义逻辑
 * Decl 声明节点
 * Stmt 语句节点
 * Expr表达式节点
 */
class XASTConsumer : public clang::ASTConsumer {
public:
	explicit XASTConsumer(clang::Rewriter* writer);

	//接收整个翻译单元（TU）的根节点（TranslationUnitDecl）和ASTContext（包含所有的AST节点、类型信息、源位置信息等）
	//通过该接口启动对整个AST的遍历和分析
	void HandleTranslationUnit(clang::ASTContext& Ctx) override;

	//当类/结构体/联合体/枚举的定义被解析时调用
	//void HandleTagDeclDefinition(TagDecl *D) override;

	//void HandleCXXImplicitFunctionInstantiation(FunctionDecl *D) override;
	void Initialize(clang::ASTContext& Context) override;
protected:
	XASTVisitor* mVisitor=nullptr;
	clang::Rewriter* mWriter=nullptr;
};
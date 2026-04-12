#pragma once
#include "clang/AST/ASTConsumer.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/PPConditionalDirectiveRecord.h>
#include <nlohmann/json.hpp>
#include <fstream>

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
	explicit XASTConsumer(clang::Rewriter* writer,nlohmann::json* config,  clang::PPConditionalDirectiveRecord* pprec, std::ofstream* ouput = nullptr);

	~XASTConsumer() override;

	/**
	 * @brief 在开始处理 AST 之前调用，此时 ASTContext 已可用但尚未遍历任何声明 
	 * 主要用途：初始化资源（如 Rewriter、符号表）、记录上下文信息、设置 AST 遍历器
	 * 
	 */
	void Initialize(clang::ASTContext& Context) override;

	/**
	 * @brief 接收整个翻译单元（TU）的根节点（TranslationUnitDecl）和ASTContext
	 *（包含所有的AST节点、类型信息、源位置信息等）通过该接口启动对整个AST的遍历和分析
	*/
	void HandleTranslationUnit(clang::ASTContext& Ctx) override;

	/**
	 * @brief 每当解析器处理完一个“顶层声明”,(如函数、全局变量、类定义）时调用。可以获取一组声明（DeclGroup）
	 * 主要用途:实现增量处理：每解析完一个顶层声明就立即处理，无需等待整个文件。适合流式分析或减少内存占用。
	 */
	bool HandleTopLevelDecl(clang::DeclGroupRef D) override;

	/**
	 * @brief 与 HandleTopLevelDecl 类似，但只处理被标记为“interesting”的声明（如非隐含的、用户显式编写的声明）
	 * 主要用途：过滤掉编译器自动生成的声明，只关注用户代码
	 */
	void HandleInterestingDecl(clang::DeclGroupRef D) override;

	/**
	 * @brief 当类/结构体/联合体/枚举的定义被完整解析时调用
	 * 主要用途：立即处理某个类型定义，例如记录类的布局、提取成员信息
	 */
	void HandleTagDeclDefinition(clang::TagDecl* D)override;

	/**
	 * @brief 当发生隐式模板函数实例化（例如使用 std::vector<int>::size() 但未显式实例化）时调用
	 * 主要用途：分析或改写隐式实例化的函数体，需要注意该函数可能不完整
	 */
	void HandleCXXImplicitFunctionInstantiation(clang::FunctionDecl* D) override;

	/**
	 * @brief 遇到模块导入声明时调用（C++20 Modules）
	 * 主要用途：处理模块导入相关的逻辑
	 */
	void HandleImplicitImportDecl(clang::ImportDecl* D) override;	
protected:
	XASTVisitor* mVisitor=nullptr;
	clang::Rewriter* mWriter=nullptr;
	nlohmann::json *mConfig =nullptr;
	clang::PPConditionalDirectiveRecord* mPPRec;
	//clang::Preprocessor *mPreprocessor = nullptr;
	//一般文件写入，有两种，一种是所有的内容都写入一个文件
	//一种是每一个文件解析完写入一个文件
	std::ofstream* mOsm = nullptr;
};
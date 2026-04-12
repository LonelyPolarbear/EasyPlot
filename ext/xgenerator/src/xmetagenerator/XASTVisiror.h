#pragma once
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/PPConditionalDirectiveRecord.h>
#include <nlohmann/json.hpp>

class XASTVisitor : public clang::RecursiveASTVisitor<XASTVisitor> {
public:
	explicit XASTVisitor(clang::Rewriter* writer, nlohmann::json* config,clang::PPConditionalDirectiveRecord* pprec, std::ofstream*osm);

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

	/**
	 * @brief 访问c++模板类
	 */
	bool VisitClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* Spec);

	/**
	 * @brief 返回false,访问器会跳过所有隐式实例化的 ClassTemplateSpecializationDecl、FunctionTemplateSpecializationDecl 等节点
	 */
	bool shouldVisitTemplateInstantiations() const { return false; }

	void setASTContext(clang::ASTContext* astContext) { mASTContext = astContext; }
protected:
	const char* getAccessSpecifierName(clang::AccessSpecifier AS) const;

	/**
	 * @brief 便利接口，判断访问权限是否为public
	 */
	bool isPublic(clang::AccessSpecifier AS) const;
	/**
	 * @brief 非模板C++类扫描过滤，目前只根据文件名过滤，只扫描用户目录文件
	 */
	bool ShouldSkip_CXXRecordDecl(clang::CXXRecordDecl* RD);

	/**
	 * @brief 模板类扫描过滤，目前只根据文件名过滤，只扫描用户目录文件
	 */
	bool ShouldSkip_ClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* spec);

	/**
	 * @brief 获取条件编译块的宏名 clang::SourceLocation IfLoc = mPPRec->findConditionalDirectiveRegionLoc(Loc);
	 */
	std::string getMacroNameFromConditionalDirective(clang::SourceLocation Loc);

	bool isMacroConditional(clang::SourceLocation Loc,const std::string& macroName);
	/**
	 * @brief 类模板实例化定义或者声明的时候，获取带参数的完整的类名
	 */
	std::string GetTemplateClassName(clang::ClassTemplateSpecializationDecl* spec);
protected:
	clang::Rewriter* mWriter=nullptr;
	nlohmann::json *mConfig=nullptr;
	//clang::Preprocessor* mPreprocessor=nullptr;
	clang::PPConditionalDirectiveRecord* mPPRec;
	std::ofstream*mOSM=nullptr;
	clang::ASTContext* mASTContext=nullptr;
};
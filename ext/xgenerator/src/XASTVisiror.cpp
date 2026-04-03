#include "XASTVisiror.h"
#include <iostream>
#include <fstream>
#include <clang\Basic\SourceManager.h>
using namespace clang;

XASTVisitor::XASTVisitor(clang::Rewriter* writer, nlohmann::json* config, std::ofstream* osm):mWriter(writer),mConfig(config), mOSM(osm)
{
}

bool XASTVisitor::VisitFunctionDecl(clang::FunctionDecl* FD)
{
	std::cout<<"------函数声明触发------\n";
	// 获取函数体起始位置（跳过 { 字符）
	//SourceLocation start = FD->getBeginLoc();

	// 构造插入内容（保留源代码缩进）
	//std::string indent = "\n    ";
	//std::string code = indent + "// Auto-generated at " __TIMESTAMP__ "\n";
	//code += indent + "printf(\"Entering %s\\n\", __func__);";

	// 执行代码插入
	//mWriter->InsertText(start, code, true, true);
	//return true;

	clang::SourceManager& SM = FD->getASTContext().getSourceManager();
	std::cout << "函数名: " << FD->getQualifiedNameAsString() << "\n";
	std::cout  << "是否inline: " << FD->isInlined() << "\n";
	std::cout  << "是否是否在类外定义: " << FD->isOutOfLine() << "\n";
	std::cout  << "hasBody: " << FD->hasBody() << "\n";
	std::cout  << "返回类型: " << FD->getReturnType().getAsString() << "\n";
	std::cout  << "形参:\n";

	for (unsigned i = 0; i < FD->getNumParams(); ++i) {
		ParmVarDecl* Param = FD->getParamDecl(i);
		QualType ParamType = Param->getType();  // 参数类型:ml-citation{ref="4" data="citationList"}
		StringRef ParamName = Param->getName(); // 参数名（可能为空）:ml-citation{ref="4" data="citationList"}
		std::cout << "形参类型:" << ParamType.getAsString() << "\n";
		std::cout << "形参名:" << ParamName.str() << "\n";
	}

	std::cout << "源码位置: " << FD->getLocation().printToString(SM) << "\n";

	return true;
}

bool XASTVisitor::VisitVarDecl(clang::VarDecl* VD)
{
	return true;
}

bool XASTVisitor::VisitStmt(clang::Stmt* S)
{
	return true;
}

bool XASTVisitor::VisitCompoundStmt(clang::CompoundStmt* CS)
{
	return true;
}

bool XASTVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl* RD)
{
	std::cout << "------类声明触发------\n";
	std::cout << "Class: " << RD->getNameAsString() << "\n";
	*mOSM << "Class: " << RD->getNameAsString() << "\n";
	// 输出成员变量
	std::cout << "Fields:\n";
	for (auto* Field : RD->fields()) {
		std::cout << "  " << Field->getType().getAsString()
			<< " " << Field->getName().str() << "\n";
	}

	// 输出成员函数
	std::cout << "Methods:\n";
	for (auto* Method : RD->methods()) {
		llvm::outs() << "  " << Method->getReturnType().getAsString()
			<< " " << Method->getNameAsString() << "()\n";
	}

	if (RD->getNumBases() > 0) {
		std::cout << "  Inheritance:\n";
		for (const CXXBaseSpecifier& Base : RD->bases()) {
			const Type* TypePtr = Base.getType().getTypePtr();
			CXXRecordDecl* BaseDecl = TypePtr->getAsCXXRecordDecl();

			std::cout << "    " << getAccessSpecifierName(Base.getAccessSpecifier())
				<< " " << BaseDecl->getNameAsString();
			if (Base.isVirtual()) std::cout << " (virtual)";
			std::cout << "\n";
		}
	}
	else {
		std::cout << "  No base classes\n";
	}

	return true;
}

const char* XASTVisitor::getAccessSpecifierName(clang::AccessSpecifier AS) const
{
	switch (AS) {
	case AS_public: return "public";
	case AS_protected: return "protected";
	case AS_private: return "private";
	default: return "unknown";
	}
}

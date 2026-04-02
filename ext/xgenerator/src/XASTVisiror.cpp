#include "XASTVisiror.h"
using namespace clang;
XASTVisitor::XASTVisitor(clang::Rewriter* writer):mWriter(writer)
{
}

bool XASTVisitor::VisitFunctionDecl(clang::FunctionDecl* FD)
{
	// 获取函数体起始位置（跳过 { 字符）
	SourceLocation start = FD->getBeginLoc();

	// 构造插入内容（保留源代码缩进）
	std::string indent = "\n    ";
	std::string code = indent + "// Auto-generated at " __TIMESTAMP__ "\n";
	//code += indent + "printf(\"Entering %s\\n\", __func__);";

	// 执行代码插入
	mWriter->InsertText(start, code, true, true);
	//return true;

	clang::SourceManager& SM = FD->getASTContext().getSourceManager();
	llvm::outs() << "函数名: " << FD->getQualifiedNameAsString() << "\n";
	llvm::outs() << "是否inline: " << FD->isInlined() << "\n";
	llvm::outs() << "是否是否在类外定义: " << FD->isOutOfLine() << "\n";
	llvm::outs() << "hasBody: " << FD->hasBody() << "\n";
	llvm::outs() << "返回类型: " << FD->getReturnType().getAsString() << "\n";
	llvm::outs() << "形参:\n";

	for (unsigned i = 0; i < FD->getNumParams(); ++i) {
		ParmVarDecl* Param = FD->getParamDecl(i);
		QualType ParamType = Param->getType();  // 参数类型:ml-citation{ref="4" data="citationList"}
		StringRef ParamName = Param->getName(); // 参数名（可能为空）:ml-citation{ref="4" data="citationList"}
		llvm::outs() << "形参类型:" << ParamType.getAsString() << "\n";
		llvm::outs() << "形参名:" << ParamName.str() << "\n";
	}

	llvm::outs() << "源码位置: " << FD->getLocation().printToString(SM) << "\n";

	return true;
}

bool XASTVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl* RD)
{
	llvm::outs() << "类整体信息\n:";
	llvm::outs() << "Class: " << RD->getNameAsString() << "\n";

	// 输出成员变量
	llvm::outs() << "Fields:\n";
	for (auto* Field : RD->fields()) {
		llvm::outs() << "  " << Field->getType().getAsString()
			<< " " << Field->getName() << "\n";
	}

	// 输出成员函数
	llvm::outs() << "Methods:\n";
	for (auto* Method : RD->methods()) {
		llvm::outs() << "  " << Method->getReturnType().getAsString()
			<< " " << Method->getNameAsString() << "()\n";
	}

	if (RD->getNumBases() > 0) {
		llvm::outs() << "  Inheritance:\n";
		for (const CXXBaseSpecifier& Base : RD->bases()) {
			const Type* TypePtr = Base.getType().getTypePtr();
			CXXRecordDecl* BaseDecl = TypePtr->getAsCXXRecordDecl();

			llvm::outs() << "    " << getAccessSpecifierName(Base.getAccessSpecifier())
				<< " " << BaseDecl->getNameAsString();
			if (Base.isVirtual()) llvm::outs() << " (virtual)";
			llvm::outs() << "\n";
		}
	}
	else {
		llvm::outs() << "  No base classes\n";
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

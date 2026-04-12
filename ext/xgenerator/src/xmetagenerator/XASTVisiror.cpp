#include "XASTVisiror.h"
#include <iostream>
#include <fstream>
#include <clang\Basic\SourceManager.h>

#include <regex>
using namespace clang;

XASTVisitor::XASTVisitor(clang::Rewriter* writer, nlohmann::json* config,  clang::PPConditionalDirectiveRecord* pprec, std::ofstream* osm)
:mWriter(writer), mConfig(config), mOSM( osm), mPPRec(pprec)
{

	//llvm::outs() << "PPRec record count? " << mPPRec->CondDirectiveLocs<< "\n";
}

bool XASTVisitor::VisitFunctionDecl(clang::FunctionDecl* FD)
{
	//auto* MD = llvm::dyn_cast<clang::CXXMethodDecl>(FD);
	//if (!MD) return true;   // 非成员函数，跳过或按需处理

	//// 仅当访问限定符为 public 时才处理
	//if (MD->getAccess() != clang::AS_public) 
	//	return true;

#if 0
	std::cout << "------函数声明触发------\n";
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
	std::cout << "是否inline: " << FD->isInlined() << "\n";
	std::cout << "是否是否在类外定义: " << FD->isOutOfLine() << "\n";
	std::cout << "hasBody: " << FD->hasBody() << "\n";
	std::cout << "返回类型: " << FD->getReturnType().getAsString() << "\n";
	std::cout << "形参:\n";

	for (unsigned i = 0; i < FD->getNumParams(); ++i) {
		ParmVarDecl* Param = FD->getParamDecl(i);
		QualType ParamType = Param->getType();  // 参数类型:ml-citation{ref="4" data="citationList"}
		StringRef ParamName = Param->getName(); // 参数名（可能为空）:ml-citation{ref="4" data="citationList"}
		std::cout << "形参类型:" << ParamType.getAsString() << "\n";
		std::cout << "形参名:" << ParamName.str() << "\n";
	}

	std::cout << "源码位置: " << FD->getLocation().printToString(SM) << "\n";
	#endif
	return true;
}

bool XASTVisitor::VisitVarDecl(clang::VarDecl* VD)
{
	return true;
	llvm::outs() << "VarDecl: " << VD->getNameAsString()
		<< " Type: " << VD->getType().getAsString() << "\n";
	// 尝试获取 RecordDecl
	if (auto* RecTy = VD->getType()->getAs<clang::RecordType>()) {
		clang::RecordDecl* RD = RecTy->getDecl();
		if (llvm::isa<clang::ClassTemplateSpecializationDecl>(RD)) {
			llvm::outs() << "  -> This is a ClassTemplateSpecializationDecl\n";
		}
	}
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

bool isNestedClass(clang::CXXRecordDecl* RD) {
	auto* DC = RD->getDeclContext();
	return DC && clang::isa<clang::CXXRecordDecl>(DC);
}

bool XASTVisitor::VisitCXXRecordDecl(clang::CXXRecordDecl* RD)
{
	if(ShouldSkip_CXXRecordDecl(RD))
		return true;
	{
		std::cout << "------class Declare------\n";
		std::cout << "Class: " << RD->getQualifiedNameAsString() << "\n";
		*mOSM << "  Class: " << RD->getQualifiedNameAsString() << "\n";
		// 输出成员变量
		std::cout << "Fields:\n";
		for (auto* Field : RD->fields()) {
			clang::SourceLocation Loc = Field->getLocation();
			clang::SourceLocation IfLoc = mPPRec->findConditionalDirectiveRegionLoc(Loc);
			std::string MacroName = getMacroNameFromConditionalDirective(IfLoc);
			if (isMacroConditional(IfLoc, "__XQ_PYTHON_WRAP__"))
				continue;

			std::cout << "  " << getAccessSpecifierName(Field->getAccess())<<":"
							<< Field->getType().getAsString() //默认带命名空间
							<< " " 
							<< Field->getName().str() << "\n";
		}

		// 输出成员函数
		std::cout << "Methods:\n";
		for (auto* Method : RD->methods()) {
			Method->isStatic();
			Method->isInlined();

			clang::SourceLocation Loc = Method->getLocation();
			clang::SourceLocation IfLoc = mPPRec->findConditionalDirectiveRegionLoc(Loc);
			std::string MacroName = getMacroNameFromConditionalDirective(IfLoc);
			if(isMacroConditional(IfLoc,"__XQ_PYTHON_WRAP__"))
				continue;
			
			auto parmeters = Method->getNumParams();													// 获取参数个数
			std::cout << "  "<< getAccessSpecifierName(Method->getAccess()) <<":" << Method->getReturnType().getAsString() <<
			" " << Method->getNameAsString() 
			<<"(";
			for (int i = 0; i < parmeters; i++) {
				auto parm = Method->getParamDecl(i);
				auto parmName = parm->getNameAsString();
				auto parmTypeName = parm->getType().getAsString();		//默认带命名空间
				if (i + 1 == parmeters) {
					std::cout << parmTypeName << " " << parmName;
				}
				else
					std::cout << parmTypeName << " " << parmName << ",";
			}
			
			std::cout <<")\n";
		}

		if (RD->getNumBases() > 0) {
			std::cout << "  Inheritance:\n";
			for (const CXXBaseSpecifier& Base : RD->bases()) {
				const Type* TypePtr = Base.getType().getTypePtr();
				CXXRecordDecl* BaseDecl = TypePtr->getAsCXXRecordDecl();
				if (BaseDecl) {
					std::cout << "    " << getAccessSpecifierName(Base.getAccessSpecifier()) << " " << BaseDecl->getQualifiedNameAsString();
					if (Base.isVirtual())
						std::cout << " (virtual)";
					std::cout << "\n";
				}
			}
		}
		else {
			std::cout << "  No base classes\n";
		}
	}
	return true;
}

bool XASTVisitor::VisitClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* Spec)
{
	/*llvm::outs() << "VisitClassTemplateSpecializationDecl: "
		<< Spec->getNameAsString() << "\n";*/

	clang::TemplateSpecializationKind TSK = Spec->getTemplateSpecializationKind();
	/*
	TSK_Undeclared																			template<typename T> class X {};								主模板本身
	TSK_ImplicitInstantiation															使用 X<int> 时自动生成													隐式实例化（由编译器按需生成）
	TSK_ExplicitInstantiationDeclaration										extern template class X<int>;										显式实例化声明（extern template）
	TSK_ExplicitInstantiationDefinition											emplate class X<int>;														显式实例化定义（template）
	TSK_ExplicitSpecialization
	*/

	if (ShouldSkip_ClassTemplateSpecializationDecl(Spec)) {
		return true;
	}

	//显示实例化声明
	if (TSK == clang::TSK_ExplicitInstantiationDeclaration) {
		std::cout<<"------ExplicitInstantiationDeclaration------\n";
		std::cout<<"Template class Declare:"<< GetTemplateClassName(Spec) <<std::endl;
	}

	//隐式实例化
	if (TSK == clang::TSK_ImplicitInstantiation) {
		std::cout << "------TSK_ImplicitInstantiation------\n";
		std::cout << "Template class ImplicitInstantiation:" << GetTemplateClassName(Spec) << std::endl;
	}

	//模板全特化
	if (TSK == clang::TSK_ExplicitSpecialization) {
		std::cout<<"------ExplicitSpecialization------\n";
		std::cout << "Template class Specialization:" << GetTemplateClassName(Spec) << std::endl;
	}

	//显示实例化定义
	if (TSK == clang::TSK_ExplicitInstantiationDefinition) {
		std::cout<<"------ExplicitInstantiationDefinition------\n";
		std::cout << "Template class Define:" << GetTemplateClassName(Spec) << std::endl;
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

bool XASTVisitor::isPublic(clang::AccessSpecifier AS) const
{
	return AS==clang::AccessSpecifier::AS_public;
}

bool XASTVisitor::ShouldSkip_CXXRecordDecl(clang::CXXRecordDecl* RD)
{
	std::string className = RD->getNameAsString();
	// 1. 过滤掉前向声明
	if (!RD->isThisDeclarationADefinition()) {
		//hasDefinition() 检查的是该类整体是否有定义，而不是当前节点是否是定义
		return true;
	}
	// 2. 跳过特定名称
	if (className == "Internal" || className == "Private" || className == "Impl")
		return true;

	// 3. 跳过嵌套类
	if (RD->getDeclContext() && clang::isa<clang::CXXRecordDecl>(RD->getDeclContext()))
		return true;

	// 4. 跳过匿名类
	if (className.empty() || className.find("(anonymous") == 0)
		return true;

	// 5. 跳过模板类定义本身
	if (RD->getDescribedClassTemplate())
		return true;

	// 6. 跳过任何模板特化节点（安全）
	if (llvm::isa<clang::ClassTemplateSpecializationDecl>(RD))
		return true;

	// 7. 获取规范声明并确保有定义
	clang::CXXRecordDecl* Canonical = RD->getCanonicalDecl();
	if (!Canonical->hasDefinition()) return true;
	
	// 9. 只处理主源文件中的类
	clang::SourceManager& SM = RD->getASTContext().getSourceManager();		//clang::CXXRecordDecl* RD
	clang::SourceLocation DefLoc = Canonical->getDefinition()->getLocation(); // 或 getDefinition()->getLocation()
	std::string Filename = SM.getFilename(DefLoc).str();
	if(!SM.isInMainFile(RD->getLocation())) {
		return true;
	}

	if (SM.isInSystemHeader(RD->getLocation())) 
		return true;

	// 10 文件过滤
	/*std::vector<std::string> filters = (*mConfig)["filters"];
	if (filters.size() > 0) {
		bool isMatch = false;
		for (auto& f : filters) {
			if (Filename.find(f) != std::string::npos) {
				isMatch = true;
			}
		}
		if (!isMatch)
			return true;
	}*/
	
	return false;
}

bool XASTVisitor::ShouldSkip_ClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* spec)
{
	clang::SourceManager& SM = spec->getASTContext().getSourceManager();
	clang::CXXRecordDecl* Canonical = spec->getCanonicalDecl();
	clang::SourceLocation DefLoc = Canonical->getLocation(); // 或 getDefinition()->getLocation()

	// 1. 系统头文件
	if (SM.isInSystemHeader(spec->getLocation())) 
		return true;

	// 2. 只处理主文件
	if (!SM.isInMainFile(spec->getLocation())) {
		return true;
	}

	// 3. 跳过内部名称
	std::string name = spec->getNameAsString();
	if (name == "Internal" || name == "Private" || name == "Impl") 
		return true;

	// 4. 跳过匿名类
	if (name.empty() || name.find("(anonymous") == 0) 
		return true;

	std::string Filename = SM.getFilename(DefLoc).str();

	/*std::vector<std::string> filters = (*mConfig)["filters"];
	if (filters.size() > 0) {
		bool isMatch = false;
		for (auto& f : filters) {
			if (Filename.find(f) != std::string::npos) {
				isMatch = true;
			}
		}
		if (!isMatch)
			return true;
	}*/

	return false;
}

clang::SourceLocation getEndOfLineLoc(clang::SourceLocation Loc,
	const clang::SourceManager& SM,
	const clang::LangOptions& LangOpts) {
	if (Loc.isInvalid()) return clang::SourceLocation();
	// 获取该行第一个 token 的结束位置
	return clang::Lexer::getLocForEndOfToken(Loc, 0, SM, LangOpts);
}

std::string XASTVisitor::getMacroNameFromConditionalDirective(clang::SourceLocation IfLoc)
{
	clang::SourceManager& SM = mPPRec->getSourceManager();
	const clang::LangOptions& LangOpts = mASTContext->getLangOpts();

	// 1. 获取从 IfLoc 到该行末尾的源代码文本
	clang::FileID FID = SM.getFileID(IfLoc);
	if (FID.isInvalid()) return "";

	bool Invalid = false;
	llvm::StringRef Buffer = SM.getBufferData(FID, &Invalid);
	if (Invalid) return "";

	unsigned Offset = SM.getFileOffset(SM.getSpellingLoc(IfLoc));
	size_t EndOffset = Offset;
	while (EndOffset < Buffer.size() && Buffer[EndOffset] != '\n' && Buffer[EndOffset] != '\r') {
		++EndOffset;
	}

	clang::SourceLocation StartLoc = SM.getSpellingLoc(IfLoc);
	clang::SourceLocation EndLoc = SM.getLocForStartOfFile(FID).getLocWithOffset(EndOffset);
	clang::CharSourceRange Range = clang::CharSourceRange::getCharRange(StartLoc, EndLoc);
	clang::StringRef LineText = clang::Lexer::getSourceText(Range, SM, LangOpts);
	if (LineText.empty()) return "";

	// 2. 使用正则表达式匹配宏名
	std::string Str = LineText.str();
	// 匹配 #ifdef MACRO、#ifndef MACRO、#if defined(MACRO) 等常见格式
	std::regex re_cond(R"((?:ifdef|ifndef|if\s+defined\s*\(\s*)\s*(\w+))");
	std::smatch match;
	if (std::regex_search(Str, match, re_cond)) {
		return match[1].str();
	}
	return "";
}

bool XASTVisitor::isMacroConditional(clang::SourceLocation Loc, const std::string& macroName)
{
	auto str = getMacroNameFromConditionalDirective(Loc);
	return str == macroName;
}

void processTemplateArgument(const clang::TemplateArgument& Arg,std::vector<std::string>& TemplateArgName) {
	clang::TemplateArgument::ArgKind k = Arg.getKind();
	if (k == clang::TemplateArgument::ArgKind::Type) {
		TemplateArgName.push_back(Arg.getAsType().getAsString());
	}
	else if (k == clang::TemplateArgument::ArgKind::Integral) {
		llvm::SmallString<32> buf;
		Arg.getAsIntegral().toStringUnsigned(buf);
		TemplateArgName.push_back(buf.str().str());
	}
	else if (k == clang::TemplateArgument::ArgKind::NullPtr) {
		//template<int* P> class X; X<nullptr> x; → 参数是 nullptr。
		TemplateArgName.push_back("nullptr");
	}
	else if (k == clang::TemplateArgument::ArgKind::Declaration) {
		//非类型模板参数，其值是一个指向变量、函数、对象等的指针或引用
		/*
		* int global;
		*	template<int* P> class X;
		*	X<&global> x;
		*/
		if (ValueDecl* D = Arg.getAsDecl()) {
			TemplateArgName.push_back(D->getQualifiedNameAsString());
		}
		else {
			TemplateArgName.push_back("NULL");
		}
	}
	else if (k == clang::TemplateArgument::ArgKind::Template) {
		//模板参数是一个类模板或别名模板（即“模板的模板参数”）
		/*
		template<template<typename> class T> class X;
		template<typename> class Y {};
		X<Y> x;   // 参数是模板 Y
		*/
		auto TN = Arg.getAsTemplate();
		//TemplateArgName.push_back(TN.getAS);
	}
	else if (k == clang::TemplateArgument::ArgKind::TemplateExpansion) {
		//模板参数是一个模板参数包（用于模板的模板参数包）。通常出现在可变参数模板中
		/*
		template<template<typename> class... TT> class X;
		template<typename> class Y;
		template<typename> class Z;
		X<Y, Z> x;   // 参数包包含 Y 和 Z
		*/
	}
	else if (k == clang::TemplateArgument::ArgKind::Expression) {
		//template<int N> class X; X<3 + 2> x; → 参数是表达式 3 + 2（Clang 可能保留其表达式形式）
	}
	else if (k == clang::TemplateArgument::ArgKind::Pack) {
		llvm::ArrayRef<clang::TemplateArgument> Pack = Arg.getPackAsArray();
		for (size_t i = 0, e = Pack.size(); i != e; ++i) {
			processTemplateArgument(Pack[i], TemplateArgName);
		}
	}
	else {
		//Null 
		//暂时不处理
	}
}

std::string XASTVisitor::GetTemplateClassName(clang::ClassTemplateSpecializationDecl* Spec)
{
	auto className = Spec->getNameAsString();																			//返回特化声明的非限定名称(不含命名控件)，如XAttr
	std::string qualifiedClassName = Spec->getQualifiedNameAsString();									//返回特化声明的限定名称（包含命名空间）。例如 XQ::XAttr
	clang::ClassTemplateDecl* Template = Spec->getSpecializedTemplate();
	std::string TemplateName = Template->getNameAsString();													//XAttr
	std::string TemplateQualifiedName = Template->getQualifiedNameAsString();					//XQ::XAttr

	TemplateQualifiedName.append("<");
	const clang::TemplateArgumentList& Args = Spec->getTemplateArgs();			// 模板参数列表
	std::vector<std::string> TemplateArgName;
	for (auto i = 0; i < Args.size(); i++) {
		const clang::TemplateArgument& Arg = Args[i];
		processTemplateArgument(Arg, TemplateArgName);
	}

	for (int i = 0; i < TemplateArgName.size(); i++) {
		TemplateQualifiedName.append(TemplateArgName[i]);
		if (i + 1 != TemplateArgName.size()) {
			TemplateQualifiedName.append(", ");
		}
	}
	TemplateQualifiedName.append(">");

	return TemplateQualifiedName;
}

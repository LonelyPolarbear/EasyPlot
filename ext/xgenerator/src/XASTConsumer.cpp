#include "XASTConsumer.h"
#include "XASTVisiror.h"
#include <fstream>

XASTConsumer::XASTConsumer(clang::Rewriter* writer, nlohmann::json*config) :mWriter(writer),mConfig(config)
{
	//需要创建文件，确保文件存在
	auto &json_config = *mConfig;
	if(json_config["is_only_one_out_file"]){
		//全部输出一个文件
		std::string path = json_config["output_path"];														// 输出路径
		std::string prefix = json_config["prefix"];																// 输出前缀
		std::string suffix = json_config["suffix"];																// 输出后缀

		std::string filename = json_config["only_one_out_file_name"];
		filename = prefix + filename + suffix + ".cpp";
		std::filesystem::path fullPath = std::filesystem::path(path) / filename;

		//if (std::filesystem::exists(fullPath)) {
		//	mOsm = new std::ofstream(filename);		//// 默认 ios::out，会清空已有内容
		//}else{
		//	
		//}
		mOsm = new std::ofstream(filename, std::ios::app);
		if(!mOsm->is_open()){
			delete mOsm;
			mOsm =nullptr;
		}

	}else{
		std::string path = json_config["output_path"];														// 输出路径
		std::string prefix = json_config["prefix"];																// 输出前缀
		std::string suffix = json_config["suffix"];																// 输出后缀

		std::string filename = json_config["input_file_name"];
		filename = prefix + filename + suffix + ".cpp";
		std::filesystem::path fullPath = std::filesystem::path(path) / filename;
	}

	mVisitor = new XASTVisitor(writer, config,mOsm);
}

XASTConsumer::~XASTConsumer()
{
	if(mOsm){
		mOsm->close();
		delete mOsm;
	}
}

void XASTConsumer::Initialize(clang::ASTContext& Context)
{
	llvm::outs() << "Initialize\n";
}

void XASTConsumer::HandleTranslationUnit(clang::ASTContext& Ctx)
{
	mVisitor->TraverseDecl(Ctx.getTranslationUnitDecl());
}

bool XASTConsumer::HandleTopLevelDecl(clang::DeclGroupRef D)
{
	/*
	DeclGroupRef 是 Clang 中用于表示一组声明的轻量级引用类型
	因为某些语言构造（如 int a, b;）会对应多个声明，但它们由同一个语法规则产生，解析器会将它们打包成一个 DeclGroup 一起传递
	可以通过 D.isNull() 判断是否为空，通过 D.begin()/D.end() 遍历每个 Decl*

	int a, b;          // 两个 VarDecl 在同一组
	extern int c;      // 单独一个 Decl
	class MyClass {};  // 单独一个 CXXRecordDecl

	解析器会依次调用三次 HandleTopLevelDecl，分别传入包含两个 VarDecl 的 DeclGroup、包含一个 VarDecl 的 DeclGroup、包含一个 CXXRecordDecl 的 DeclGroup

	先调用 HandleTopLevelDecl（如果重写） → 然后调用 HandleTranslationUnit（整个翻译单元结束后）
	HandleTopLevelDecl 每解析完一个顶层声明就会触发，而 HandleTranslationUnit 只在最后触发一次

	主要用途：
		1. 过滤声明 可以立即检查声明是否感兴趣，不感兴趣的可以直接跳过，节省内存
		2. 即时输出 比如生成文档、收集符号表、打印 AST 结构等，可以边解析边输出
		3. 避免全量 AST 如果只关心某些顶层声明（如函数定义），可以在 HandleTopLevelDecl 中处理并返回 false 来阻止该声明被进一步处理
	*/
#if 0
	for (clang::Decl* decl : D) {
		// 处理单个声明
		if (clang::FunctionDecl* FD = clang::dyn_cast<clang::FunctionDecl>(decl)) {
			// 处理函数定义
		}
		else if (clang::VarDecl* VD = clang::dyn_cast<clang::VarDecl>(decl)) {
			// 处理全局变量
		}
		// ... 其他类型
	}
#endif
	return clang::ASTConsumer::HandleTopLevelDecl(D);
}

void XASTConsumer::HandleInterestingDecl(clang::DeclGroupRef D)
{
	return clang::ASTConsumer::HandleInterestingDecl(D);
}

void XASTConsumer::HandleTagDeclDefinition(clang::TagDecl* D)
{
	return clang::ASTConsumer::HandleTagDeclDefinition(D);
}

void XASTConsumer::HandleCXXImplicitFunctionInstantiation(clang::FunctionDecl* D)
{
	return clang::ASTConsumer::HandleCXXImplicitFunctionInstantiation(D);
}

void XASTConsumer::HandleImplicitImportDecl(clang::ImportDecl* D)
{
	return clang::ASTConsumer::HandleImplicitImportDecl(D);
}
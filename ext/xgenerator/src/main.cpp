#include "clang/Tooling/Tooling.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include <fstream>
#include <sstream>
#include "clang/Rewrite/Core/Rewriter.h"
#include <iostream>
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

#include <linenoise/linenoise.h>
#include <CLI/CLI.hpp>
//cmake - G "Visual Studio 17 2022" - Thost = x64 - DLLVM_ENABLE_PROJECTS = "clang;clang-tools-extra" - B build - S .

static std::string toLower(const std::string& s) {
	std::string result;
	result.reserve(s.size());
	for (unsigned char c : s) {
		result.push_back(static_cast<char>(std::tolower(c)));
	}
	return result;
}

static void completionHook(const char* buf, linenoiseCompletions* lc, void* userdata) {
	
}

using namespace clang;

class MyVisitor : public RecursiveASTVisitor<MyVisitor> {
public:
	explicit MyVisitor(Rewriter& R) : TheRewriter(R) {}
	//DEF_TRAVERSE_DECL
	bool VisitFunctionDecl(FunctionDecl* FD) {
		
		// 获取函数体起始位置（跳过 { 字符）
		SourceLocation start = FD->getBeginLoc();

		// 构造插入内容（保留源代码缩进）
		std::string indent = "\n    ";
		std::string code = indent + "// Auto-generated at " __TIMESTAMP__ "\n";
		//code += indent + "printf(\"Entering %s\\n\", __func__);";

		// 执行代码插入
		TheRewriter.InsertText(start, code, true, true);
		return true;

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

	bool VisitCXXRecordDecl(CXXRecordDecl* RD) {
		
		std::cout << "类整体信息\n:";
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

	const char* getAccessSpecifierName(AccessSpecifier AS) const {
		switch (AS) {
		case AS_public: return "public";
		case AS_protected: return "protected";
		case AS_private: return "private";
		default: return "unknown";
		}
	}

private:
	Rewriter& TheRewriter;
};

class MyASTConsumer : public ASTConsumer {
public:
	explicit MyASTConsumer(Rewriter& R) : Visitor(R) {}

	//接收整个翻译单元（TU）的根节点（TranslationUnitDecl）和ASTContext（包含所有的AST节点、类型信息、源位置信息等）
	//通过该接口启动对整个AST的遍历和分析
	void HandleTranslationUnit(ASTContext& Ctx) override {
		Visitor.TraverseDecl(Ctx.getTranslationUnitDecl());
	}

	//当类/结构体/联合体/枚举的定义被解析时调用
	//void HandleTagDeclDefinition(TagDecl *D) override;

	//void HandleCXXImplicitFunctionInstantiation(FunctionDecl *D) override;
	void Initialize(ASTContext& Context) {
		llvm::outs() << "Initialize\n";
	}
private:
	MyVisitor Visitor;
};

class MyFrontendAction : public ASTFrontendAction {
public:
	std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& CI, StringRef File) override {
		TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
		return std::make_unique<MyASTConsumer>(TheRewriter);
	}

	void EndSourceFileAction() override {
		// 输出修改后的代码
		Rewriter& rewriter = TheRewriter;
	
		std::error_code EC;
		llvm::raw_fd_ostream os("out.md",EC, llvm::sys::fs::OF_Text);

		rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID())
			.write(/*llvm::outs()*/os);
		os.close();
	}

private:
	Rewriter TheRewriter;
};

static llvm::cl::OptionCategory MyToolCategory("My Tool Options");

int main(int argc, const char** argv) {
#if 0
	auto ExpectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, MyToolCategory);
	if (!ExpectedParser) {
		llvm::errs() << ExpectedParser.takeError();
		return 1;
	}
	auto & OptionsParser = ExpectedParser.get();
	clang::tooling::ClangTool Tool(OptionsParser.getCompilations(),
		OptionsParser.getSourcePathList());

	return Tool.run(clang::tooling::newFrontendActionFactory<MyFrontendAction>().get());
#endif
    //tooling::runToolOnCode(std::make_unique<MyFrontendAction>(), strstm.str());

	char* line = nullptr;
	
	std::string filePath;
	std::string code;
	
	while (true) {
		CLI::App app;
		auto opt_file = app.add_option("-f,--file", filePath, "Input file name");
		auto opt_code = app.add_option("-c,--code", code, "Input code");
		opt_file->excludes(opt_code);
		opt_code->excludes(opt_file);
		std::cout << ">>> ";
		line = linenoise(">>> ");
		if (line == nullptr) break;

		std::string input(line);

		if (input.empty()) continue;

		linenoiseHistoryAdd(input.c_str());
		//linenoiseHistorySave("history.txt"); // 保存历史记录

		if (input == "exit")
			break;
		std::istringstream iss(input);
		std::vector<std::string> args{ std::istream_iterator<std::string>(iss),std::istream_iterator<std::string>() };

		try {
			
			app.parse(args);
		}
		catch (const CLI::ParseError& e) {
			if (e.get_exit_code() != 0) {
				std::cerr << e.what() << "\n";
			}
			else {
				std::cout <<app.help() << std::endl;
			}
			continue;
		}

		if (opt_file) {
			std::ifstream stm;
			std::stringstream strstm;

			stm.open("E:/work/EasyPlot/ext/xgenerator/src/test.md");
			strstm << stm.rdbuf();
			auto code = strstm.str();
			tooling::runToolOnCode(std::make_unique<MyFrontendAction>(), code);
		}
		else if (opt_code) {
			tooling::runToolOnCode(std::make_unique<MyFrontendAction>(), code);
		}
	}
}
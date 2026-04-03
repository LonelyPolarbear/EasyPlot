#include "XASTFrontendAction.h"
#include "XASTConsumer.h"
#include <filesystem>
#include <iostream>

XASTFrontendAction::XASTFrontendAction(const nlohmann::json& config):mConfig(config)
{
}

std::unique_ptr<clang::ASTConsumer> XASTFrontendAction::CreateASTConsumer(clang::CompilerInstance& CI, clang::StringRef File)
{
	//从File中获取文件名
	std::string absolutePath = File.str();
	std::filesystem::path p(absolutePath);
	std::string fileNameWithExt = p.filename().string();			// 文件名带扩展名
	std::string fileNameNoExt = p.stem().string();					// 文件名不带扩展名
	std::string extension = p.extension().string();						// 文件扩展名

	mConfig["input_file_name"] = fileNameNoExt;
	mConfig["input_file_ext"] = extension;

	std::cout<<mConfig.dump(4)<<std::endl;
	TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
	return std::make_unique<XASTConsumer>(&TheRewriter, &mConfig);
}

void XASTFrontendAction::EndSourceFileAction()
{

	//std::string path = mConfig["output_path"];														// 输出路径
	//std::string prefix = mConfig["prefix"];																// 输出前缀
	//std::string suffix = mConfig["suffix"];																// 输出后缀

	//std::string filename = mConfig["input_file_name"];
	//filename =prefix+filename+suffix+".cpp";
	//std::filesystem::path fullPath = std::filesystem::path(path) / filename;

	clang::Rewriter& rewriter = TheRewriter;

	//std::error_code EC;
	//llvm::raw_fd_ostream os(fullPath.string(), EC, llvm::sys::fs::OF_Text);

	//rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID())
	//	.write(/*llvm::outs()*/os);
	//os.close();
}

// XASTFrontendActionFactory.cpp
std::unique_ptr<clang::FrontendAction> XASTFrontendActionFactory::create()
{
	return std::make_unique<XASTFrontendAction>(mConfig);
}

#include "XASTFrontendAction.h"
#include "XASTConsumer.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include "clang/Lex/PPConditionalDirectiveRecord.h"

XASTFrontendAction::XASTFrontendAction(const nlohmann::json& config, std::ofstream* osm):mConfig(config),mOsm(osm)
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
	mConfig["input_file_absolute_path"] = absolutePath;
	mConfig["input_file_ext"] = extension;

	std::cout<<mConfig.dump(4)<<std::endl;
	TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
	std::unique_ptr<clang::PPConditionalDirectiveRecord> PPRec = std::make_unique<clang::PPConditionalDirectiveRecord>(CI.getSourceManager());
	clang::PPConditionalDirectiveRecord* RawPPRec = PPRec.get();
	CI.getPreprocessor().addPPCallbacks(std::move(PPRec));
	return std::make_unique<XASTConsumer>(&TheRewriter, &mConfig, RawPPRec, mOsm);
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

XASTFrontendActionFactory::XASTFrontendActionFactory(nlohmann::json& config) :mConfig(config)
{
	//如果扫描的多个文件全部输出到一个文件
	if (mConfig["is_only_one_out_file"] == true) {
		std::string path = mConfig["output_path"];														// 输出路径
		std::string prefix = mConfig["prefix"];																// 输出前缀
		std::string suffix = mConfig["suffix"];																// 输出后缀
		std::string filename = mConfig["only_one_out_file_name"];
		filename = prefix + filename + suffix + ".cpp";
		std::filesystem::path fullPath = std::filesystem::path(path) / filename;
		mOsm = new std::ofstream(fullPath.string());
		
		if (!mOsm->is_open()) {
			delete mOsm;
			mOsm = nullptr;
		}

		std::string str=
R"(// metadata registter
#include <dataBase/XObjectFactory.h>

void static registterMetaData(){
)";
		*mOsm << str;
	}
}

// XASTFrontendActionFactory.cpp
std::unique_ptr<clang::FrontendAction> XASTFrontendActionFactory::create()
{
	return std::make_unique<XASTFrontendAction>(mConfig, mOsm);
}

XASTFrontendActionFactory::~XASTFrontendActionFactory()
{
	if (mOsm) {
		*mOsm << "}";
		mOsm->flush();
		mOsm->close();
	}
	delete mOsm;
}

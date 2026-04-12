#pragma once
#include <clang/Frontend/FrontendAction.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/Tooling.h>
#include <nlohmann/json.hpp>

class XASTFrontendAction :public clang::ASTFrontendAction {
public:
	XASTFrontendAction(const nlohmann::json& config,std::ofstream *osm);
	std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& CI, clang::StringRef File) override;
	void EndSourceFileAction() override;
private:
	clang::Rewriter TheRewriter;
	nlohmann::json mConfig;
	std::ofstream* mOsm = nullptr;
};

class XASTFrontendActionFactory : public clang::tooling::FrontendActionFactory {
public:
	XASTFrontendActionFactory(nlohmann::json& config= nlohmann::json());

	std::unique_ptr<clang::FrontendAction> create() override;

	~XASTFrontendActionFactory();
private:
	nlohmann::json mConfig;
	std::ofstream* mOsm = nullptr;
};
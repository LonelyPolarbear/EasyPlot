#include "xshaderManger.h"
#include "xshader.h"
#include <sharevar/XShareVar.h>

class xShaderManger::Interal {
	std::map<int,std::shared_ptr<xshader>> shaders3D;			//针对不同的图元类型，保存对应的着色器

	std::shared_ptr<xshader> gridShader3d;								//网格绘制着色器	

	std::shared_ptr<xshader> textShader;									//用于文字绘制的着色器

	std::shared_ptr<xshader> ndcShader;									//ndc坐标绘制的着色器,已经丢弃

	std::shared_ptr<xshader> outlineShader;								//模型轮廓提取的着色器
public:
	std::shared_ptr<xshader> getShader3D(PrimitveType id) {
		if (shaders3D.find((int)id) != shaders3D.end()) {
			return shaders3D[(int)id];
		}
		return nullptr;
	}


	void addShader3D(PrimitveType id, std::shared_ptr<xshader> shader) {
		shaders3D[(int)id] = shader;
	}

	void setGridShader(std::shared_ptr<xshader> shader) {
		gridShader3d = shader;
	}

	void setTextShader(std::shared_ptr<xshader> shader) {
		textShader = shader;
	}

	void setNdcShader(std::shared_ptr<xshader> shader) {
		ndcShader = shader;
	}

	void setOutlineShader(std::shared_ptr<xshader> shader) {
		outlineShader = shader;
	}

	std::shared_ptr<xshader> getOutlineShader() const {
		return outlineShader;
	}

	std::shared_ptr<xshader> getGridShader() const {
		return gridShader3d;
	}

	std::shared_ptr<xshader> getTextshader() const {
		return textShader;
	}

	std::shared_ptr<xshader> getNdcshader() const {
		return ndcShader;
	}

};
xShaderManger::xShaderManger():d(new Interal())
{
}

xShaderManger::~xShaderManger()
{
}

void xShaderManger::addShader3D(PrimitveType id, std::shared_ptr<xshader> shader)
{
	return d->addShader3D(id, shader);
}

std::shared_ptr<xshader> xShaderManger::getShader3D(PrimitveType id) const
{
	return d->getShader3D(id);
}

void xShaderManger::setGridShader(std::shared_ptr<xshader> shader)
{
	return d->setGridShader(shader);
}

void xShaderManger::setTextShader(std::shared_ptr<xshader> shader)
{
	return d->setTextShader(shader);
}

void xShaderManger::setNdcShader(std::shared_ptr<xshader> shader)
{
	return d->setNdcShader(shader);
}

void xShaderManger::setOutlineShader(std::shared_ptr<xshader> shader)
{
	return d->setOutlineShader(shader);
}

std::shared_ptr<xshader> xShaderManger::getTextShader() const
{
	return d->getTextshader();
}


std::shared_ptr<xshader> xShaderManger::getGridShader() const
{
	return d->getGridShader();
}

std::shared_ptr<xshader> xShaderManger::getNdcShader() const
{
	return d->getNdcshader();
}

std::shared_ptr<xshader> xShaderManger::getOutlineShader() const
{
	return d->getOutlineShader();
}

void xShaderManger::InitRenderResource()
{
	//初始化着色器资源
	//创建opengl资源
	{
		auto shader3D_triangle = makeShareDbObject<xshader>();
		shader3D_triangle->create();
		auto vs_id = shader3D_triangle->compile(xshader::ShaderType::VERTEX, XShareVar::instance().currentExeDir + "/easyPlot/" + "3DTriangle.vs");
		auto fs_id = shader3D_triangle->compile(xshader::ShaderType::FRAGMENT, XShareVar::instance().currentExeDir + "/easyPlot/" + "3DTriangle.fs");
		shader3D_triangle->link({ vs_id,fs_id });
		addShader3D(PrimitveType::triangle, shader3D_triangle);
		addShader3D(PrimitveType::triangle_fan, shader3D_triangle);
		addShader3D(PrimitveType::triangle_strip, shader3D_triangle);
	}

	{
		auto shader2D_line_strip_adjacency = makeShareDbObject<xshader>();
		shader2D_line_strip_adjacency->create();
		auto vs_id = shader2D_line_strip_adjacency->compile(xshader::ShaderType::VERTEX, XShareVar::instance().currentExeDir + "/easyPlot/" + "3DTriangle.vs");
		auto fs_id = shader2D_line_strip_adjacency->compile(xshader::ShaderType::FRAGMENT, XShareVar::instance().currentExeDir + "/easyPlot/" + "2D_line_Strip_Adjaceny.fs");
		auto gs_id = shader2D_line_strip_adjacency->compile(xshader::ShaderType::GEOMETRY, XShareVar::instance().currentExeDir + "/easyPlot/" + "2D_line_Strip_Adjaceny.gs");
		shader2D_line_strip_adjacency->link({ vs_id,fs_id,gs_id });
		addShader3D(PrimitveType::line_strip_adjacency, shader2D_line_strip_adjacency);
	}

	{
		auto gridShader3D= makeShareDbObject<xshader>();
		gridShader3D->create();
		auto vs_id = gridShader3D->compile(xshader::ShaderType::VERTEX, XShareVar::instance().currentExeDir + "/easyPlot/" + "Infinite.vs");
		auto fs_id = gridShader3D->compile(xshader::ShaderType::FRAGMENT, XShareVar::instance().currentExeDir + "/easyPlot/" + "Infinite.fs");
		gridShader3D->link({ vs_id,fs_id });
		setGridShader(gridShader3D);
	}

	{
		auto textShader = makeShareDbObject<xshader>();
		textShader->create();
		auto vs_id = textShader->compile(xshader::ShaderType::VERTEX, XShareVar::instance().currentExeDir + "/easyPlot/" + "text2d.vs");
		auto fs_id = textShader->compile(xshader::ShaderType::FRAGMENT, XShareVar::instance().currentExeDir + "/easyPlot/" + "text2d.fs");
		textShader->link({ vs_id,fs_id });
		setTextShader(textShader);
	}

	{
		auto ndcShader = makeShareDbObject<xshader>();
		ndcShader->create();
		auto vs_id = ndcShader->compile(xshader::ShaderType::VERTEX, XShareVar::instance().currentExeDir + "/easyPlot/" + "smaa.vs");
		auto fs_id = ndcShader->compile(xshader::ShaderType::FRAGMENT, XShareVar::instance().currentExeDir + "/easyPlot/" + "ndc.fs");
		ndcShader->link({ vs_id,fs_id });
		setNdcShader(ndcShader);
	}

	{
		{
			auto outlineShader = makeShareDbObject<xshader>();
			outlineShader->create();
			auto vs_id = outlineShader->compile(xshader::ShaderType::VERTEX, XShareVar::instance().currentExeDir + "/easyPlot/" + "3DTriangle.vs");
			auto fs_id = outlineShader->compile(xshader::ShaderType::FRAGMENT, XShareVar::instance().currentExeDir + "/easyPlot/" + "outline.fs");
			outlineShader->link({ vs_id,fs_id });
			setOutlineShader(outlineShader);
		}
	}
}

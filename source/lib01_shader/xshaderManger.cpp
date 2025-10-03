#include "xshaderManger.h"
#include "xshader.h"
#include "lib00_utilty/myUtilty.h"

class xShaderManger::Interal {
	std::map<int,std::shared_ptr<xshader>> shaders3D;			//针对不同的图元类型，保存对应的着色器
	std::map<int,std::shared_ptr<xshader>> shaders2D;			//针对不同的图元类型，保存对应的着色器，就是点和线
	std::shared_ptr<xshader> fillshader2d;								//二维的填充着色器	
	std::shared_ptr<xshader> gridShader3d;								//三维的网格绘制着色器	
	std::shared_ptr<xshader> gridShader2d;								//二维的网格绘制着色器	

	std::shared_ptr<xshader> pickShaderd3d;							//用于3D拾取的着色器	
	std::shared_ptr<xshader> pickShader2d;								//用于2D拾取的着色器
	std::shared_ptr<xshader> pickfillShader2d;							//用于2D填充拾取的着色器

	std::shared_ptr<xshader> textShader;									//用于文字绘制的着色器
public:
	std::shared_ptr<xshader> getShader3D(PrimitveType id) {
		if (shaders3D.find((int)id) != shaders3D.end()) {
			return shaders3D[(int)id];
		}
		return nullptr;
	}

	std::shared_ptr<xshader> getShader2D(PrimitveType id) {
		if (shaders2D.find((int)id) != shaders2D.end()) {
			return shaders2D[(int)id];
		}
		return nullptr;
	}

	void addShader3D(PrimitveType id, std::shared_ptr<xshader> shader) {
		shaders3D[(int)id] = shader;
	}

	void addShader2D(PrimitveType id, std::shared_ptr<xshader> shader) {
		shaders2D[(int)id] = shader;
	}

	void setFillShader(std::shared_ptr<xshader> shader) {
		fillshader2d = shader;
	}

	void setGridShader3D(std::shared_ptr<xshader> shader) {
		gridShader3d = shader;
	}

	void setGridShader2D(std::shared_ptr<xshader> shader) {
		gridShader2d = shader;
	}

	void setPickShader3D(std::shared_ptr<xshader> shader) {
		pickShaderd3d = shader;
	}

	void setPickShader2D(std::shared_ptr<xshader> shader) {
		pickShader2d = shader;
	}

	void setPickFillShader2D(std::shared_ptr<xshader> shader) {
		pickfillShader2d = shader;
	}

	void setTextShader(std::shared_ptr<xshader> shader) {
		textShader = shader;
	}

	std::shared_ptr<xshader> getFillShader() const {
		return fillshader2d;
	}

	std::shared_ptr<xshader> getGridShader3D() const {
		return gridShader3d;
	}

	std::shared_ptr<xshader> getGridShader2D() const {
		return gridShader2d;
	}

	std::shared_ptr<xshader> getPickShader3D() const {
		return pickShaderd3d;
	}

	std::shared_ptr<xshader> getPickShader2D() const {
		return pickShader2d;
	}

	std::shared_ptr<xshader> getPickFillShader2D() const {
		return pickfillShader2d;
	}

	std::shared_ptr<xshader> getTextshader() const {
		return textShader;
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

void xShaderManger::addShader2D(PrimitveType id, std::shared_ptr<xshader> shader)
{
	return d->addShader2D(id, shader);
}

std::shared_ptr<xshader> xShaderManger::getShader3D(PrimitveType id) const
{
	return d->getShader3D(id);
}

std::shared_ptr<xshader> xShaderManger::getShader2D(PrimitveType id) const
{
	return d->getShader2D(id);
}

void xShaderManger::setFillShader(std::shared_ptr<xshader> shader)
{
	return d->setFillShader(shader);
}

void xShaderManger::setGridShader3D(std::shared_ptr<xshader> shader)
{
	return d->setGridShader3D(shader);
}

void xShaderManger::setGridShader2D(std::shared_ptr<xshader> shader)
{
	return d->setGridShader2D(shader);
}

void xShaderManger::setPickShader3D(std::shared_ptr<xshader> shader)
{
	return d->setPickShader3D(shader);
}

void xShaderManger::setPickShader2D(std::shared_ptr<xshader> shader)
{
	return d->setPickShader2D(shader);
}

void xShaderManger::setPickFillShader2D(std::shared_ptr<xshader> shader)
{
	return d->setPickFillShader2D(shader);
}

void xShaderManger::setTextShader(std::shared_ptr<xshader> shader)
{
	return d->setTextShader(shader);
}

std::shared_ptr<xshader> xShaderManger::getTextShader() const
{
	return d->getTextshader();
}

std::shared_ptr<xshader> xShaderManger::getFillShader() const
{
	return d->getFillShader();
}

std::shared_ptr<xshader> xShaderManger::getGridShader3D() const
{
	return d->getGridShader3D();
}

std::shared_ptr<xshader> xShaderManger::getGridShader2D() const
{
	return d->getGridShader2D();
}

std::shared_ptr<xshader> xShaderManger::getPickShader3D() const
{
	return d->getPickShader3D();
}

std::shared_ptr<xshader> xShaderManger::getPickShader2D() const
{
	return d->getPickShader2D();
}

std::shared_ptr<xshader> xShaderManger::getPickFillShader2D() const
{
	return d->getPickFillShader2D();
}

void xShaderManger::initGLResource()
{
	//初始化着色器资源
	//创建opengl资源
	{
		auto shader3D_triangle = makeShareDbObject<xshader>();
		shader3D_triangle->create();
		auto vs_id = shader3D_triangle->compile(xshader::ShaderType::VERTEX, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "3DTriangle.vs");
		auto fs_id = shader3D_triangle->compile(xshader::ShaderType::FRAGMENT, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "3DTriangle.fs");
		shader3D_triangle->link({ vs_id,fs_id });
		addShader3D(PrimitveType::triangle, shader3D_triangle);
	}

	{
		auto shader2D_line_strip_adjacency = makeShareDbObject<xshader>();
		shader2D_line_strip_adjacency->create();
		auto vs_id = shader2D_line_strip_adjacency->compile(xshader::ShaderType::VERTEX, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "2D_line_Strip_Adjaceny.vs");
		auto fs_id = shader2D_line_strip_adjacency->compile(xshader::ShaderType::FRAGMENT, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "2D_line_Strip_Adjaceny.fs");
		auto gs_id = shader2D_line_strip_adjacency->compile(xshader::ShaderType::GEOMETRY, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "2D_line_Strip_Adjaceny.gs");
		shader2D_line_strip_adjacency->link({ vs_id,fs_id,gs_id });
		addShader2D(PrimitveType::line_strip_adjacency, shader2D_line_strip_adjacency);
	}

	{
		auto gridShader3D= makeShareDbObject<xshader>();
		gridShader3D->create();
		auto vs_id = gridShader3D->compile(xshader::ShaderType::VERTEX, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "Infinite.vs");
		auto fs_id = gridShader3D->compile(xshader::ShaderType::FRAGMENT, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "Infinite.fs");
		gridShader3D->link({ vs_id,fs_id });
		setGridShader3D(gridShader3D);
	}

	{
		auto gridShader2d = makeShareDbObject<xshader>();
		gridShader2d->create();
		auto vs_id = gridShader2d->compile(xshader::ShaderType::VERTEX, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "Infinite2d.vs");
		auto fs_id = gridShader2d->compile(xshader::ShaderType::FRAGMENT, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "Infinite2d.fs");
		gridShader2d->link({ vs_id,fs_id });
		setGridShader2D(gridShader2d);
	}

	{
		auto fillShader2d = makeShareDbObject<xshader>();
		fillShader2d->create();
		auto vs_id = fillShader2d->compile(xshader::ShaderType::VERTEX, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "fill2d.vs");
		auto fs_id = fillShader2d->compile(xshader::ShaderType::FRAGMENT, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "fill2d.fs");
		fillShader2d->link({ vs_id,fs_id});
		setFillShader(fillShader2d);
	}

	{
		auto pickShader3D = makeShareDbObject<xshader>();
		pickShader3D->create();
		auto vs_id = pickShader3D->compile(xshader::ShaderType::VERTEX, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "pick3d.vs");
		auto fs_id = pickShader3D->compile(xshader::ShaderType::FRAGMENT, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "pick3d.fs");
		pickShader3D->link({ vs_id,fs_id });
		setPickShader3D(pickShader3D);
	}

	{
		auto pickShader2D = makeShareDbObject<xshader>();
		pickShader2D->create();
		auto vs_id = pickShader2D->compile(xshader::ShaderType::VERTEX, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "pick2d.vs");
		auto fs_id = pickShader2D->compile(xshader::ShaderType::FRAGMENT, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "pick2d.fs");
		auto gs_id = pickShader2D->compile(xshader::ShaderType::GEOMETRY, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "pick2d.gs");
		pickShader2D->link({ vs_id,fs_id ,gs_id });
		setPickShader2D(pickShader2D);
	}

	{
		auto pickfillShader2D = makeShareDbObject<xshader>();
		pickfillShader2D->create();
		auto vs_id = pickfillShader2D->compile(xshader::ShaderType::VERTEX, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "pick2dFill.vs");
		auto fs_id = pickfillShader2D->compile(xshader::ShaderType::FRAGMENT, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "pick2dFill.fs");
		pickfillShader2D->link({ vs_id,fs_id });
		setPickFillShader2D(pickfillShader2D);
	}

	{
		auto textShader = makeShareDbObject<xshader>();
		textShader->create();
		auto vs_id = textShader->compile(xshader::ShaderType::VERTEX, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "text2d.vs");
		auto fs_id = textShader->compile(xshader::ShaderType::FRAGMENT, myUtilty::ShareVar::instance().currentExeDir + "/easyPlot/" + "text2d.fs");
		textShader->link({ vs_id,fs_id });
		setTextShader(textShader);
	}
}

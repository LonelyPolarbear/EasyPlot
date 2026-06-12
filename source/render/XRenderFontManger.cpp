#include "XRenderFontManger.h"
#include "lib04_opengl/XOpenGLTexture.h"
#include "sharevar/XShareVar.h"
#include "lib03_stbImage/stbImage.h"
#include "lib00_utilty/XUtilty.h"
#include "XOpenGLRenderWindow.h"
#include <xlog/XLogger.h>

class XRenderFontManger::Internal {
public:
	sptr<XOpenGLTexture> fontTexture;
	bool fontinitialized = false;

	std::future<void> result_future;

	wptr<XOpenGLRenderWindow> renderWindow;
};

XRenderFontManger::XRenderFontManger(sptr<XBaseRenderWindow> renWindow):mData(new Internal)
{
	mData->renderWindow = renWindow->asDerived<XOpenGLRenderWindow>();
}

XRenderFontManger::~XRenderFontManger()
{
}

void XRenderFontManger::InitRenderResource()
{
	auto fontTexture = makeShareDbObject<XOpenGLTexture>();
	fontTexture->setTarget(XOpenGLTexture::Target::Target2DArray);
	fontTexture->create();
	fontTexture->bind();
	fontTexture->setMinificationFilter(XOpenGLTexture::Filter::Linear);
	fontTexture->setMagnificationFilter(XOpenGLTexture::Filter::Linear);
	fontTexture->setInternalFormat(XOpenGLTexture::TextureFormat::RGB8_UNorm);
	fontTexture->setExternalFormat(XOpenGLTexture::PixelFormat::RGB, XOpenGLTexture::PixelType::UInt8);
	fontTexture->release();
	mData->fontTexture = fontTexture;
	//字体纹理信息
	//mData->result_future = std::async(std::launch::async, [this]() {
		int width = 0;
		int height = 0;
		std::tuple<int, int, std::vector<const void*>> result;
		(&std::get<2>(result))->resize(11);

		auto fn = [&result, &width, &height](int i) {
			std::string  str = std::to_string(i).append(".bmp");
			auto info = stbImage::readPicture(XShareVar::instance().currentExeDir + "\\sdf\\" + str, false);
			width = info.width;
			height = info.height;
			std::get<2>(result)[i] = info.data;
			};

		XQ::ParaAlgo::ParallelForeach(0, 11, fn, 1);

		std::get<0>(result) = width;
		std::get<1>(result) = height;
		auto renWin = mData->renderWindow.lock();
		//auto shareContect = renWin->getContext()->createOrgetShareContext();
		auto shareContect = renWin->getContext();
		{
			shareContect->makeCurrent();
			mData->fontTexture->bind();
			auto array_data = std::get<2>(result);

			mData->fontTexture->texStorage3D(std::get<0>(result), std::get<1>(result), std::get<2>(result).size());
			for (int i = 0; i < array_data.size(); i++) {
				mData->fontTexture->setSubData3D(0, 0, i, std::get<0>(result), std::get<1>(result), 1, array_data[i]);
			}
			mData->fontTexture->release();

			//内存释放
			for (auto data : std::get<2>(result)) {
				stbImage::freeImageData((void*)data);
			}
			std::cout << "font texture data loaded" << std::endl;
			shareContect->doneCurrent();
		}
		mData->fontinitialized = true;
		//});

	//字体文件描述信息
	//mData->result_future.get();
	XLOG_TRACE("XRenderFontManger font bmp loaded!");
}

sptr<XDataBaseObject> XRenderFontManger::getFontTexture() const
{
	return mData->fontTexture;
}

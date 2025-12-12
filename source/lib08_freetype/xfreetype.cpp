
#include "xfreetype.h"
#include <glew/glew.h>
#include <stb_image.h>
#include <iostream>
#include <fstream>

#include <ft2build.h>
#include <freetype/freetype.h>  
#include <codecvt>

#include <dataBase/XDataArray.h>
#include <QImage>
#include <QString>
#include <regex>
#include <lib00_utilty/myUtilty.h>

FT_Library ft;
FT_Face face;

void xfreetype::test()
{
    unsigned int unicodeValue = static_cast<unsigned int>(wchar_t(L"我"));
    std::wstring unicode_str = L"我";
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::u16string gddd = u"我";
    wchar_t gg(L'我');
    uint16_t KK =*( (uint16_t*) & gg);

    std::string utf8_str = converter.to_bytes(unicode_str);

    std::cout << utf8_str << std::endl;

    std::wstring unicode_str2 = converter.from_bytes(utf8_str);
 
}

void xfreetype::printFaceInfo() {
	std::cout << "=== 字体信息 ===" << std::endl;
	std::cout << "字体家族: " << face->family_name << std::endl;
	std::cout << "字体样式: " << face->style_name << std::endl;
	std::cout << "字形数量: " << face->num_glyphs << std::endl;

	std::cout << "=== 设计度量 ===" << std::endl;
	std::cout << "units_per_EM: " << face->units_per_EM << std::endl;
	std::cout << "ascender: " << face->ascender << " 设计单位" << std::endl;
	std::cout << "descender: " << face->descender << " 设计单位" << std::endl;
	std::cout << "height: " << face->height << " 设计单位" << std::endl;

	std::cout << "=== 当前尺寸 ===" << std::endl;
	if (face->size) {
		std::cout << "缩放后行高: " << (face->size->metrics.height >> 6) << " 像素" << std::endl;
		std::cout << "x_ppem: " << face->size->metrics.x_ppem << std::endl;
		std::cout << "y_ppem: " << face->size->metrics.y_ppem << std::endl;
	}

	std::cout << "=== 字体特性 ===" << std::endl;
	if (face->face_flags & FT_FACE_FLAG_SCALABLE) {
		std::cout << "- 可缩放字体" << std::endl;
	}
	if (face->face_flags & FT_FACE_FLAG_FIXED_SIZES) {
		std::cout << "- 包含固定尺寸: " << face->num_fixed_sizes << " 种" << std::endl;
	}
	if (face->face_flags & FT_FACE_FLAG_KERNING) {
		std::cout << "- 支持字距调整" << std::endl;
	}
}

void xfreetype::generateFontTextures(const QString&dir, bool flip,bool inverse)
{
	auto sdfDataPath = dir + "/data.txt";
	std::ofstream outFile(sdfDataPath.toStdString());

	uint32_t textureWidth = 4096;
	uint32_t textureHeight = 4096;

	int pixelNumX = 1;
	int pixelNumY = 1;

	uint32_t perPixelWidth = getnNormalSingleTextWidth();
	uint32_t perPixelHeight = getnNormalSingleTextHeight();

	// 切换到 Unicode 字符映射表（优先处理最通用的编码）
	if (FT_Select_Charmap(face, FT_ENCODING_UNICODE) != 0) {
		std::cerr << "警告：字体不支持 Unicode 映射表，可能无法获取所有字符" << std::endl;
	}

	//字体信息
	FT_UInt maxGlyphIndex = face->num_glyphs;       //总共的字形数量
	face->family_name;		// 字体系列名称（如 "Arial", "Times New Roman"）
	face->style_name;			// 字体风格名称（如 "Bold", "Italic"）
	face->num_faces;			// 字体文件中包含的字体数量（.ttc集合文件可能包含多个）


    #if 0
	for (FT_UInt glyphIndex = 0; glyphIndex < maxGlyphIndex; ++glyphIndex) {
		// 检查字形是否有效（仅加载轮廓，不渲染位图，提高效率）
		if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_NO_BITMAP) != 0) {
			continue;
		}

		auto w = face->glyph->bitmap.width;
		auto h = face->glyph->bitmap.rows;
        perPixelWidth = std::max(perPixelWidth, w);
        perPixelHeight = std::max(perPixelHeight, h);
	}

    //perPixelWidth perPixelHeight 对齐到四字节
    perPixelWidth = (perPixelWidth + 3) & ~3;
    perPixelHeight = (perPixelHeight + 3) & ~3;
    #endif

    pixelNumX = textureWidth /perPixelWidth;
    pixelNumY = textureHeight /perPixelHeight;

    //图片个数
    int pictureNum = ceil( (double)maxGlyphIndex / (double)(pixelNumX * pixelNumY));
	std::vector<std::shared_ptr<XUCharArray2D>> FontTextures;

    for (int i = 0; i < pictureNum; i++) {
		auto data = makeShareDbObject<XUCharArray2D>();
		data->setComponent(1);
		data->setDimensions(textureHeight, textureWidth);
        FontTextures.push_back(data);
    }

	std::function<void(uchar*)> reverse = [](unsigned char* data) {*data = 255 - *data;};

    //存储格式说明
    //当前的存储信息:第几层 该层的X Y  字形信息: 宽 高 偏移x 偏移y advance pitch

    outFile <<maxGlyphIndex <<std::endl;    //总字形数量

	for (FT_UInt glyphIndex = 0; glyphIndex < maxGlyphIndex; ++glyphIndex) {
		//判断其在第几张图片中
        int pictureIndex = glyphIndex / (pixelNumX * pixelNumY);

        //属于该张图片的位置
        int x = (glyphIndex % (pixelNumX * pixelNumY)) % pixelNumX;
        int y = (glyphIndex % (pixelNumX * pixelNumY)) / pixelNumX;
        // 加载字形
		if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT|FT_LOAD_RENDER) != 0) {
            outFile << glyphIndex << " " << pictureIndex << " " << x << " " << y << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << std::endl;
			continue;
		}
        if (!face->glyph->bitmap.buffer) {
            auto advance = face->glyph->advance.x >> 6;
			outFile << glyphIndex
				<< " " << pictureIndex
				<< " " << x
				<< " " << 0
				<< " " << 0
				<< " " << 0
				<< " " << 0
				<< " " << 0
				<< " " << advance
				<< " " << 0 << std::endl;
            continue;
        }

        {
			int bmpWidth = face->glyph->bitmap.width;       // 位图宽度
			int bmpHeight = face->glyph->bitmap.rows;
			int pitch = face->glyph->bitmap.pitch;
			auto* buffer = face->glyph->bitmap.buffer;
			FontTextures[pictureIndex]->setData(x * perPixelWidth, y * perPixelHeight, bmpWidth, bmpHeight, buffer);
            auto advance = face->glyph->advance.x >> 6;
			auto lineSpace = face->size->metrics.height; //行距，表达的是基线到基线的距离
           int flipy = (flip ?  pixelNumY-y-1:y);
            outFile << glyphIndex 
            << " " << pictureIndex 
            << " " << x 
            << " " << flipy
            << " " << bmpWidth 
            << " " << bmpHeight 
            << " " << face->glyph->bitmap_left 
            << " " << face->glyph->bitmap_top 
            << " " << advance 
            << " " << pitch << std::endl;
        }
	}

    for (int i = 0; i < FontTextures.size(); i++) {
        if(inverse)
            FontTextures[i]->forEach(reverse);
        if(flip)
            FontTextures[i]->flip();
		QImage image(FontTextures[i]->data(0, 0), textureWidth, textureHeight, textureWidth/*指定行宽，避免对齐问题*/, QImage::Format_Grayscale8);
		image.save(QString("%1/%2.bmp").arg(dir).arg(i));
    }
}

void xfreetype::generateFontSdf(const QString& dir, bool flip, bool inverse)
{
    //记录字形信息到文件中
    auto sdfDataPath = dir + "/data.txt";
    std::ofstream outFile(sdfDataPath.toStdString());


	uint32_t textureWidth = getSdfPictureWidth();
	uint32_t textureHeight = getSdfPictureHeight();

	int pixelNumX = 1;
	int pixelNumY = 1;

	uint32_t perPixelWidth = getSdfSingleTextWidth();
	uint32_t perPixelHeight = getSdfSingleTextHeight();

	// 切换到 Unicode 字符映射表（优先处理最通用的编码）
	if (FT_Select_Charmap(face, FT_ENCODING_UNICODE) != 0) {
		std::cerr << "警告：字体不支持 Unicode 映射表，可能无法获取所有字符" << std::endl;
	}

	FT_UInt maxGlyphIndex = face->num_glyphs;       //总共的字形数量

	pixelNumX = textureWidth / perPixelWidth;
	pixelNumY = textureHeight / perPixelHeight;

	//图片个数
	int pictureNum = ceil((double)maxGlyphIndex / (double)(pixelNumX * pixelNumY));
	std::vector<std::shared_ptr<XUCharArray2D>> FontTextures;

	for (int i = 0; i < pictureNum; i++) {
		auto data = makeShareDbObject<XUCharArray2D>();
		data->setComponent(1);
		data->setDimensions(textureHeight, textureWidth);
		FontTextures.push_back(data);
	}

	std::function<void(uchar*)> reverse = [](unsigned char* data) {*data = 255 - *data; };

    outFile << maxGlyphIndex <<std::endl;    //总字形数量

	for (FT_UInt glyphIndex = 0; glyphIndex < maxGlyphIndex; ++glyphIndex) {
		//判断其在第几张图片中
		int pictureIndex = glyphIndex / (pixelNumX * pixelNumY);

		//属于该张图片的位置
		int x = (glyphIndex % (pixelNumX * pixelNumY)) % pixelNumX;
		int y = (glyphIndex % (pixelNumX * pixelNumY)) / pixelNumX;
		// 加载字形
		if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT ) != 0) {
            //宽 高 偏移x 偏移y advance pitch
            outFile << glyphIndex << " " << pictureIndex << " " << x << " " << y << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << std::endl;
			continue;
		}

		// 检查是否支持SDF渲染
		if (!FT_IS_SCALABLE(face)) {
			std::cerr << "字体不支持缩放，无法生成SDF" << std::endl;
            outFile << glyphIndex << " " << pictureIndex << " " << x << " " << y << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << std::endl;
		}

		FT_Render_Mode renderMode = FT_RENDER_MODE_SDF;
		FT_Error error = FT_Render_Glyph(face->glyph, renderMode);

		if (error) {
			std::cerr << "SDF渲染失败，错误代码: " << error << std::endl;
			// 尝试普通渲染作为备选
			error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
			if (error) {
				std::cerr << "普通渲染失败，错误代码: " << error << std::endl;
			}
		}

		if (!face->glyph->bitmap.buffer) {
			auto advance = face->glyph->advance.x >> 6;
			outFile << glyphIndex
				<< " " << pictureIndex
				<< " " << x
				<< " " << 0
				<< " " << 0
				<< " " << 0
				<< " " << 0
				<< " " << 0
				<< " " << advance
				<< " " << 0 << std::endl;
			continue;
		}

		{
			int bmpWidth = face->glyph->bitmap.width;       // 位图宽度
			int bmpHeight = face->glyph->bitmap.rows;
			int pitch = face->glyph->bitmap.pitch;
			auto* buffer = face->glyph->bitmap.buffer;
			FontTextures[pictureIndex]->setData(x * perPixelWidth, y * perPixelHeight, bmpWidth, bmpHeight, buffer);
            auto advance = face->glyph->advance.x >> 6;
            int flipy = (flip ? pixelNumY - y - 1 : y);
			outFile << glyphIndex
				<< " " << pictureIndex
				<< " " << x
				<< " " << flipy
				<< " " << bmpWidth
				<< " " << bmpHeight
				<< " " << face->glyph->bitmap_left
				<< " " << face->glyph->bitmap_top
				<< " " << advance
				<< " " << pitch << std::endl;
		}
	}

	for (int i = 0; i < FontTextures.size(); i++) {
		if (inverse)
			FontTextures[i]->forEach(reverse);
		if (flip)
			FontTextures[i]->flip();
		QImage image(FontTextures[i]->data(0, 0), textureWidth, textureHeight, textureWidth/*指定行宽，避免对齐问题*/, QImage::Format_Grayscale8);
		image.save(QString("%1/%2.bmp").arg(dir).arg(i));
	}

    outFile.close();
}

void xfreetype::LoadGlyphNormal(const QString& dir)
{
    LoadGlyphImpl(dir,mCharactersList);
}

void xfreetype::LoadGlyphSdf(const QString& dir)
{
    LoadGlyphImpl(dir, mCharactersListSdf);
}

unsigned int xfreetype::getGlyphIndex(wchar_t c)
{
    uint16_t code = *( (uint16_t*) & c);
    return FT_Get_Char_Index(face, code);
}

xfreetype::Character xfreetype::getCharacterNormal(wchar_t c)
{
	if (mCharactersList.size() == 0)
		return Character();
    auto idx = getGlyphIndex(c);
    return mCharactersList[idx];
}

xfreetype::Character xfreetype::getCharacterSdf(wchar_t c)
{
	if (mCharactersListSdf.size() == 0)
		return Character();

	auto idx = getGlyphIndex(c);
	return mCharactersListSdf[idx];
}

double xfreetype::computeLineStrWidth(std::wstring data, double FontSize)
{
	double scale =FontSize / 64.;
	double width = 0;

	for (auto c = data.begin(); c != data.end(); c++)
	{
        auto ch =getCharacterSdf(*c);
		width += ch.Advance*scale;
	}

	return width;
}

int xfreetype::getLineRowSpace()
{
	return face->size->metrics.height >> 6;
}

int xfreetype::computeLineNum(const std::wstring& data, double FontSize, double Fixedwidth,  bool isFixed)
{
	int lineNum = 1;
	auto num = data.size();
	
	//获取每个字符的纹理
	double start_x = 0;
	double start_y = 0;

	double scale = (double)FontSize / (double)64;		//字形的缩放系数

	auto adjust = [this, scale, Fixedwidth, isFixed](double& start_x, double& start_y) {
		if(isFixed ==false)
			return false;
		if (start_x > Fixedwidth) {
			start_y -= 64 * scale;
			return true;
		}
		return false;
		};

	for (int i = 0; i < num; i++) {
		auto c = data.at(i);
		auto glyph = xfreetype::Instance()->getCharacterSdf(c);

		double tmp = start_x + glyph.Advance * scale;
		if (adjust(tmp, start_y)) {
			start_x = 0;
			lineNum++;
		}

		if (c == '\n') {
			lineNum++;
			start_x = 0;
			start_y -= 64 * scale;
		}


		if (c != '\n') {
			start_x += glyph.Advance * scale;
		}
	}

	return lineNum;
}

std::shared_ptr<XDoubleArray> xfreetype::computeLineNums(const std::wstring& data, double FontSize, double Fixedwidth,bool isFixed)
{
	auto lineNums =computeLineNum(data,FontSize, Fixedwidth,isFixed);
	auto result =makeShareDbObject<XDoubleArray>();
	result->setComponent(4);
	result->setNumOfTuple(lineNums);

	int lineNum = 0;
	auto num = data.size();

	//获取每个字符的纹理
	double start_x = 0;
	double start_y = 0;

	double scale = (double)FontSize / (double)64;		//字形的缩放系数

	auto adjust = [this, scale, Fixedwidth,isFixed](double& start_x, double& start_y) {
		if(isFixed ==false)
			return false;
		if (start_x > Fixedwidth) {
			start_y -= 64 * scale;
			return true;
		}
		return false;
		};
	//int lastRowStartIdx =0;
	double maxCharHeightTop = -100;
	double maxCharHeightBottom = 0;
	for (int i = 0; i < num; i++) {
		auto c = data.at(i);
		auto glyph = xfreetype::Instance()->getCharacterSdf(c);
		maxCharHeightTop = std::max<double>(maxCharHeightTop,  glyph.bearY);
		if(glyph.bearY<0 || maxCharHeightTop<0 || maxCharHeightBottom<0){
			int hhh =10;
		}
		maxCharHeightBottom = std::max<double>(maxCharHeightBottom, glyph.height - glyph.bearY);
		double tmp = start_x + glyph.Advance * scale;
		if (adjust(tmp, start_y)) {
			result->setTuple(lineNum, i, start_x, maxCharHeightTop,maxCharHeightBottom);
			start_x = 0;
			maxCharHeightTop = -100;
			maxCharHeightBottom = 0;
			lineNum++;
			//lastRowStartIdx = i;
		}

		if (c == '\n') {
			result->setTuple(lineNum, i, start_x, maxCharHeightTop, maxCharHeightBottom);
			maxCharHeightBottom = 0;
			maxCharHeightTop = -100;
			lineNum++;
			start_x = 0;
			start_y -= 64 * scale;
			//lastRowStartIdx = i;
		}

		if (c != '\n') {
			start_x += glyph.Advance * scale;
			result->setTuple(lineNum, i, start_x, maxCharHeightTop, maxCharHeightBottom);
		}
	}

	return result;
}

void xfreetype::LoadGlyphImpl(const QString& dir, std::vector<Character>& charactersList)
{
#if 1
	std::ifstream inFile(dir.toStdString(), std::ios::ate);
	std::streamsize size = inFile.tellg();
		inFile.seekg(0, std::ios::beg); // 回到文件开头
		//读取所有数据
		std::string buffer(size, '\0');
		if (inFile.read(&buffer[0], size)) {
			inFile.close();
		}
        std::regex pattern(R"(\n)");
        
        std::sregex_token_iterator iter(buffer.begin(), buffer.end(), pattern,-1);
        auto glyphNum = std::stoi( iter->str());
        iter++;
		std::vector<std::string> result(
            iter,  // -1表示取匹配之外的部分
			std::sregex_token_iterator()
		);

        charactersList.resize(glyphNum);
       
		XQ::ParaAlgo::ParallelForeach(0, glyphNum, [&charactersList, &result](int idx) {
        #if 0
            //正则表达式不具备线程安全性,多线程反而会变慢
           static std::regex ws_re("\\s+");
            //手动分割
            std::vector<std::string> result_number(
			            std::sregex_token_iterator(result[idx].begin(), result[idx].end(), ws_re, -1), std::sregex_token_iterator()
            );// -1表示取匹配之外的部分
        #else
			std::vector<std::string> tokens;
            tokens.reserve(10);
			std::stringstream ss(result[idx]);
			std::string token;
			while (ss >> token) {
				tokens.push_back(token);
			}
        #endif

			charactersList[idx].idx = std::stoi(tokens[0]);
			charactersList[idx].layer = std::stoi(tokens[1]);
			charactersList[idx].x = std::stoi(tokens[2]);
			charactersList[idx].y = std::stoi(tokens[3]);
			charactersList[idx].width = std::stoi(tokens[4]);
			charactersList[idx].height = std::stoi(tokens[5]);
			charactersList[idx].bearX = std::stoi(tokens[6]);
			charactersList[idx].bearY = std::stoi(tokens[7]);
			charactersList[idx].Advance = std::stoi(tokens[8]);
			charactersList[idx].pitch = std::stoi(tokens[9]);
        });

#else        
	std::ifstream inFile(dir.toStdString());
	std::regex ws_re("\\s+");  // 匹配一个或多个空白字符
	std::string line;  // 存储每行内容
	int lineNum = 0;
	if (std::getline(inFile, line)) {
		lineNum = std::stoi(line);
	}

    charactersList.resize(lineNum);
	int idx = 0;
	while (std::getline(inFile, line)) {
		//按照空白字符分割
		std::vector<std::string> result(
			std::sregex_token_iterator(line.begin(), line.end(), ws_re, -1),  // -1表示取匹配之外的部分
			std::sregex_token_iterator()
		);

		if (result.size() == 10) {
            charactersList[idx].idx = std::stoi(result[0]);
            charactersList[idx].layer = std::stoi(result[1]);
            charactersList[idx].x = std::stoi(result[2]);
            charactersList[idx].y = std::stoi(result[3]);
            charactersList[idx].width = std::stoi(result[4]);
            charactersList[idx].height = std::stoi(result[5]);
            charactersList[idx].bearX = std::stoi(result[6]);
            charactersList[idx].bearY = std::stoi(result[7]);
            charactersList[idx].Advance = std::stoi(result[8]);
            charactersList[idx].pitch = std::stoi(result[9]);
			idx++;
        }
        else {
            idx++;
        }
	}
    #endif
}

xfreetype::xfreetype()
{
	std::cout << "FreeType 版本：" << FREETYPE_MAJOR << "."
		<< FREETYPE_MINOR << "." << FREETYPE_PATCH << std::endl;

    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    if (FT_New_Face(ft, "C:\\Windows\\Fonts\\simsun.ttc", 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    FT_Set_Pixel_Sizes(face, 0, 64);
}

xfreetype::~xfreetype() {
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

xfreetype* xfreetype::Instance()
{
    if (s_instance==nullptr) {
        s_instance = new xfreetype();
    }
    return s_instance;
}

void xfreetype::Release()
{
    delete s_instance;
    s_instance = nullptr;
}



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

	FT_UInt maxGlyphIndex = face->num_glyphs;       //总共的字形数量
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
            outFile << glyphIndex << " " << pictureIndex << " " << x << " " << y << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << std::endl;
            continue;
        }

        {
			int bmpWidth = face->glyph->bitmap.width;       // 位图宽度
			int bmpHeight = face->glyph->bitmap.rows;
			int pitch = face->glyph->bitmap.pitch;
			auto* buffer = face->glyph->bitmap.buffer;
			FontTextures[pictureIndex]->setData(x * perPixelWidth, y * perPixelHeight, bmpWidth, bmpHeight, buffer);
            auto advance = face->glyph->advance.x >> 6;
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
            outFile <<pictureIndex<<" " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 <<std::endl;
			continue;
		}

		// 检查是否支持SDF渲染
		if (!FT_IS_SCALABLE(face)) {
			std::cerr << "字体不支持缩放，无法生成SDF" << std::endl;
            outFile << pictureIndex << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << std::endl;
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
            outFile << pictureIndex << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << 0 << std::endl;
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
    auto idx = getGlyphIndex(c);
    return mCharactersList[idx];
}

xfreetype::Character xfreetype::getCharacterSdf(wchar_t c)
{
	auto idx = getGlyphIndex(c);
	return mCharactersListSdf[idx];
}


void xfreetype::LoadGlyphImpl(const QString& dir, std::vector<Character>& charactersList)
{
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

#if 0
void xfreetype::LoadCharacter(std::wstring data)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //禁用字节对齐限制
    for (auto c : data) {
        //获取编码值
        uint16_t code = *((uint16_t*)&c);

        if (Characters.find(code) != Characters.end())
            continue;
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        // 生成纹理
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // 设置纹理选项
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // 储存字符供之后使用
        Character character = {
            texture,
            face->glyph->bitmap.width, face->glyph->bitmap.rows,
            face->glyph->bitmap_left, face->glyph->bitmap_top,
            face->glyph->advance.x
        };
        Characters.insert({ code, character });
    }
}

std::pair<double, double> xfreetype::computeStrSize(std::wstring data,int FontSize)
{
    LoadCharacter(data);

    float scale = (float)FontSize / 200.;
    float x = 0;
    float maxHeight = 0;
    for (auto c = data.begin(); c != data.end(); c++)
    {
        uint16_t code = *((uint16_t*)&(*c));
        auto  ch = xfreetype::Instance()->Characters[code];

        GLfloat w = ch.width * scale;
        GLfloat h = ch.height * scale;

        maxHeight = std::max(maxHeight, ch.height * scale);
        x += (ch.Advance >> 6) * scale; // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
    }
    return std::pair<double, double>(x, maxHeight);
}

void xfreetype::getVertices(wchar_t c, float(&vertices)[24], int fontSize)
{
    LoadCharacter(std::wstring(1, c));
    //获取该字符的纹理
    float x = 0;
    float y = 0;
    uint16_t code = *((uint16_t*)&(c));
    auto  ch = xfreetype::Instance()->Characters[code];
    float scale = (float)fontSize / 200.;
    GLfloat xpos = x + ch.bearX * scale;
    GLfloat ypos = y - (ch.height - ch.bearY) * scale;

    GLfloat w = ch.width * scale;
    GLfloat h = ch.height * scale;
    // 对每个字符更新VBO
    GLfloat _vertices[6][4] = {
        { xpos,     ypos + h,   0.0, 0.0 },
        { xpos,     ypos,       0.0, 1.0 },
        { xpos + w, ypos,       1.0, 1.0 },

        { xpos,     ypos + h,   0.0, 0.0 },
        { xpos + w, ypos,       1.0, 1.0 },
        { xpos + w, ypos + h,   1.0, 0.0 }
    };
    
    memcpy(vertices, _vertices, 18 * sizeof(float));
}

xfreetype::Character xfreetype::getCharacter(wchar_t c)
{
    uint16_t code = *((uint16_t*)&(c));
    return this->Characters[code];
}
#endif
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


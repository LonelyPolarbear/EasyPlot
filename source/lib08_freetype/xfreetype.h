#pragma once
#include <string>
#ifdef LIB08_FREETYPE_DLL
#define LIB08_FREETYPE_API __declspec(dllexport)
#else
#define LIB08_FREETYPE_API __declspec(dllimport)
#endif 

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <dataBase/XDataArray.h>
class QString;

class LIB08_FREETYPE_API xfreetype
{
public:
    struct Character {
		int idx;		// 字形在字库中的索引
		int layer;
		int x;
		int y;
        int width;
        int height;
        int bearX;
        int bearY;
		int pitch;
        unsigned int  Advance;    // 原点距下一个字形原点的距离
    };
	static  void test();
	void printFaceInfo();
	void generateFontTextures(const QString& dir, bool flip, bool inverse);
	void generateFontSdf(const QString& dir, bool flip, bool inverse);
	
	void LoadGlyphNormal(const QString& dir);
	void LoadGlyphSdf(const QString& dir);
	unsigned int getGlyphIndex(wchar_t c);

	Character getCharacterNormal(wchar_t c);
	Character getCharacterSdf(wchar_t c);

public:
	int getSdfSingleTextWidth(){return 80;}
	int getSdfSingleTextHeight() { return 80; }
	int getSdfPictureHeight() { return 52*80; }
	int getSdfPictureWidth() { return 52*80; }


	int getnNormalSingleTextWidth() { return 64; }
	int getnNormalSingleTextHeight() { return 64; }
	int getNormalPictureHeight() { return 4096; }
	int getNormalPictureWidth() { return 4096; }

	double computeLineStrWidth(std::wstring data, double FontSize);

	int getLineRowSpace();

	int computeLineNum(const std::wstring& data, double FontSize, double width, bool isFixed);

	//统计每一行的字符数、每一行所有字符的宽度 每一行字符的最大有效高度(基础线上方和基线下方两部分)
	std::shared_ptr<XDoubleArray> computeLineNums(const std::wstring& data, double FontSize, double width, bool isFixed);

private:
	void LoadGlyphImpl(const QString& dir, std::vector<Character>& charactersList);
   xfreetype();
   ~xfreetype();

   std::vector<Character> mCharactersList;									//<字体的所有字符信息>
   std::vector<Character> mCharactersListSdf;								//<字体的所有字符信息>
public:
   static xfreetype* Instance();
   static void Release();
private:
   inline static xfreetype* s_instance = nullptr;
};
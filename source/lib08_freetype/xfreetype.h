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

private:
	void LoadGlyphImpl(const QString& dir, std::vector<Character>& charactersList);
   xfreetype();
   ~xfreetype();

   std::vector<Character> mCharactersList;									//<字体的所有字符信息>
   std::vector<Character> mCharactersListSdf;								//<字体的所有字符信息>
  
  #if 0
public:
	std::map<uint32_t, Character> Characters;  //<字体,<字符编码,字符信息>
   void LoadCharacter(std::wstring data);
   Character getCharacter(wchar_t  c);
   void getVertices(wchar_t  c, float(&vertices)[24], int fontSize);
   std::pair<double, double> computeStrSize(std::wstring data, int FontSize);
#endif
public:
   static xfreetype* Instance();
   static void Release();
private:
   inline static xfreetype* s_instance = nullptr;
};

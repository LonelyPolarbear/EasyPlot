#include "Init.h"
#include <dataBase/XObjectFactory.h>

//模块初始化工作
//1. XBaseObjectMeta尽心数据注册，主要是XBaseObject子类注册，用于注册继承关系以及工厂创建

//2. 支持属性序列化行为进行的注册工作
//3. 支持数据序列化行为进行的注册工作

//4. xmvd模块中对属性编辑的时候，需要注册

static void InitBaseObjectMeta();

static void InitAttrSerialize();

static void InitDataSerialize();


RENDER_API void InitRender()
{
	//进行模块的初始化工作	
	InitBaseObjectMeta();
	InitAttrSerialize();
}

static void InitBaseObjectMeta(){}

static void InitAttrSerialize(){}

static void InitDataSerialize(){}
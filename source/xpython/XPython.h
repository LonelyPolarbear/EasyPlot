#include "xpythonApi.h"
#include <dataBase/XDataBaseObject.h>

class XPYTHON_API XPython :public XDataBaseObject {
public:
	static std::shared_ptr<XPython> Instance();
	static void release();
	void execute(const std::string& code);
protected:
	XPython();
protected:
	class Internal;
	std::shared_ptr<Internal> mData;
};
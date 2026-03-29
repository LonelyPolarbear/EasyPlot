#include "xpythonApi.h"
#include <memory>
class XPYTHON_API XPythonConsole {
public:
	XPythonConsole();
	~XPythonConsole();
public:
	void run();
	void stop();
protected:
	int runImpl();
protected:
	class Internal;
	std::shared_ptr<Internal> mData;
};
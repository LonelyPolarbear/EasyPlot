#include "XOpenGLEnable.h"
#include <glew/glew.h>

class XOpenGLEnable::Intertal {
public:
    Intertal() {
    }

public:
	std::map<GLenum, bool> oldEnableMap;
};
XOpenGLEnable::XOpenGLEnable():d(new Intertal())
{
}

XOpenGLEnable::~XOpenGLEnable()
{
}

void XOpenGLEnable::enable(EnableType type)
{
   d->oldEnableMap[(GLenum)type] = glIsEnabled((GLenum)type);
   glEnable((GLenum)type);
}

void XOpenGLEnable::disable(EnableType type)
{
	d->oldEnableMap[(GLenum)type] = glIsEnabled((GLenum)type);
	glDisable((GLenum)type);
}

bool XOpenGLEnable::isEnabled(EnableType type)
{
    return glIsEnabled((GLenum)type);
}

void XOpenGLEnable::restore()
{
    for (auto it = d->oldEnableMap.begin(); it!= d->oldEnableMap.end(); ++it) {
        if (it->second) {
            glEnable(it->first);
        }
        else {
            glDisable(it->first);
        }
    }
    d->oldEnableMap.clear();
}
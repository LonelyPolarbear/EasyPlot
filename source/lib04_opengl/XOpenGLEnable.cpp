#include "XOpenGLEnable.h"
#include <glew/glew.h>
#include "XOpenGLType.h"
#include "XOpenGLFuntion.h"

namespace ns_xopengl_enable {
    //Éî¶È²âÊÔ×´Ì¬
	struct state_depth_test_t {
        bool isEnable = true;
	};

	//¶àÖØ²ÉÑù×´Ì¬
	struct state_multisample_t {
        bool isEnable = true;
    };

    //¼ôÇÐ²âÊÔ×´Ì¬
	struct state_scissor_test_t {
        bool isEnable = true;
    };

    //ÈÚ»ìºÏ×´Ì¬
	struct state_blend_t {
       bool isEnable = false;
       XOpenGL::BlendFuncFactor srcFactor = XOpenGL::BlendFuncFactor::XGL_ONE;
       XOpenGL::BlendFuncFactor dstFactor = XOpenGL::BlendFuncFactor::XGL_ZERO;
    };

    //²Ã¼ôÃæ×´Ì¬
	struct state_cull_face_t {
       bool isEnable = false;
    };

    //³ÌÐòµã´óÐ¡×´Ì¬
    struct state_program_point_size_t {
        bool isEnable = false;
    };

    //Ä£°å²âÊÔ×´Ì¬
    struct state_stencil_test_t {
        bool isEnable = false;
        XOpenGL::DepthOrStencilCompFunType fun = XOpenGL::DepthOrStencilCompFunType::XGL_LESS;
        int ref = 0;
        uint32_t mask = 0xFFFFFFFF;
        XOpenGL::StencilBehavior sfail = XOpenGL::StencilBehavior::XGL_KEEP;
        XOpenGL::StencilBehavior zfail = XOpenGL::StencilBehavior::XGL_KEEP;
        XOpenGL::StencilBehavior zpass = XOpenGL::StencilBehavior::XGL_REPLACE;
    };
}


class XOpenGLEnable::Intertal {
public:
    Intertal() {
    }

public:
	std::map<GLenum, bool> oldEnableMap;
	ns_xopengl_enable::state_depth_test_t depthTestState;
	ns_xopengl_enable::state_multisample_t multisampleState;
	ns_xopengl_enable::state_scissor_test_t scissorTestState;
	ns_xopengl_enable::state_blend_t blendState;
	ns_xopengl_enable::state_cull_face_t cullFaceState;
	ns_xopengl_enable::state_program_point_size_t programPointSizeState;
	ns_xopengl_enable::state_stencil_test_t stencilTestState;
};
XOpenGLEnable::XOpenGLEnable():mData(new Intertal())
{
}

XOpenGLEnable::~XOpenGLEnable()
{
}

void XOpenGLEnable::enable(EnableType type)
{
    mData->oldEnableMap[(GLenum)type] = glIsEnabled((GLenum)type);
   glEnable((GLenum)type);
}

void XOpenGLEnable::disable(EnableType type)
{
    mData->oldEnableMap[(GLenum)type] = glIsEnabled((GLenum)type);
	glDisable((GLenum)type);
}

bool XOpenGLEnable::isEnabled(EnableType type)
{
    return glIsEnabled((GLenum)type);
}

void XOpenGLEnable::restore()
{
   /* for (auto it = mData->oldEnableMap.begin(); it!= mData->oldEnableMap.end(); ++it) {
        if (it->second) {
            glEnable(it->first);
        }
        else {
            glDisable(it->first);
        }
    }*/
    mData->oldEnableMap.clear();

    restoreDepthTestState();
    restoreMultisampleState();
    restoreScissorTestState();
    restoreBlendState();
    restoreCullFaceState();
    restoreProgramPointSizeState();
    restoreStencilTestState();
}

void XOpenGLEnable::save()
{
    saveDepthTestState();
    saveMultisampleState();
    saveScissorTestState();
    saveBlendState();
    saveCullFaceState();
    saveProgramPointSizeState();
    saveStencilTestState();
}

void XOpenGLEnable::saveDepthTestState()
{
    mData->depthTestState.isEnable = glIsEnabled((GLenum)EnableType::DEPTH_TEST);
}

void XOpenGLEnable::saveMultisampleState()
{
    mData->multisampleState.isEnable = glIsEnabled((GLenum)EnableType::MULTISAMPLE);
}

void XOpenGLEnable::saveScissorTestState()
{
    mData->scissorTestState.isEnable = glIsEnabled((GLenum)EnableType::SCISSOR_TEST);
}

void XOpenGLEnable::saveBlendState()
{
    mData->blendState.isEnable = glIsEnabled((GLenum)EnableType::BLEND);
    XOpenGLFuntion::xGetBlendFunc(mData->blendState.srcFactor,mData->blendState.dstFactor );
}

void XOpenGLEnable::saveCullFaceState()
{
    mData->cullFaceState.isEnable = glIsEnabled((GLenum)EnableType::CULL_FACE);
}

void XOpenGLEnable::saveProgramPointSizeState()
{
    mData->programPointSizeState.isEnable = glIsEnabled((GLenum)EnableType::PROGRAM_POINT_SIZE);
}

void XOpenGLEnable::saveStencilTestState()
{
    mData->stencilTestState.isEnable = glIsEnabled((GLenum)EnableType::STENCIL_TEST);
    XOpenGLFuntion::xGetStencilFunc(mData->stencilTestState.fun,mData->stencilTestState.ref,mData->stencilTestState.mask);
    XOpenGLFuntion::xGetStencilOp(mData->stencilTestState.sfail,mData->stencilTestState.zfail,mData->stencilTestState.zpass);
}

void XOpenGLEnable::restoreDepthTestState()
{
    if(mData->depthTestState.isEnable)
        glEnable((GLenum)EnableType::DEPTH_TEST);
    else
        glDisable((GLenum)EnableType::DEPTH_TEST);
}

void XOpenGLEnable::restoreMultisampleState()
{
    if(mData->multisampleState.isEnable)
        glEnable((GLenum)EnableType::MULTISAMPLE);
    else
        glDisable((GLenum)EnableType::MULTISAMPLE);
}

void XOpenGLEnable::restoreScissorTestState()
{
    if(mData->scissorTestState.isEnable)
        glEnable( (GLenum)EnableType::SCISSOR_TEST);
    else
        glDisable((GLenum)EnableType::SCISSOR_TEST);
}

void XOpenGLEnable::restoreBlendState()
{
    if(mData->blendState.isEnable)
        glEnable((GLenum)EnableType::BLEND);
    else
        glDisable((GLenum)EnableType::BLEND);
    XOpenGLFuntion::xglBlendFunc(mData->blendState.srcFactor,mData->blendState.dstFactor);
}

void XOpenGLEnable::restoreCullFaceState()
{
    if(mData->cullFaceState.isEnable)
        glEnable((GLenum)EnableType::CULL_FACE);
    else
        glDisable((GLenum)EnableType::CULL_FACE);
}

void XOpenGLEnable::restoreProgramPointSizeState()
{
    if(mData->programPointSizeState.isEnable)
        glEnable((GLenum)EnableType::PROGRAM_POINT_SIZE);
    else
        glDisable((GLenum)EnableType::PROGRAM_POINT_SIZE);
}

void XOpenGLEnable::restoreStencilTestState()
{
    if(mData->stencilTestState.isEnable)
        glEnable((GLenum)EnableType::STENCIL_TEST);
    else
        glDisable((GLenum)EnableType::STENCIL_TEST);
    XOpenGLFuntion::xglStencilFunc(mData->stencilTestState.fun,mData->stencilTestState.ref,mData->stencilTestState.mask);
    XOpenGLFuntion::xglStencilOp(mData->stencilTestState.sfail,mData->stencilTestState.zfail,mData->stencilTestState.zpass);
}

#include "XGeometryNodeState.h"
#include <xsignal/XSignal.h>
#include <lib04_opengl/XOpenGLBuffer.h>
#include <glew/glew.h>

/**
 * @class XGeometryNodeState
 */
class XGeometryNodeState::Internal {
public:
	sptr<XOpenGLBuffer> mFaceStateBuffer;
	sptr<XOpenGLBuffer> mLineStateBuffer;
	sptr<XOpenGLBuffer> mVertexStateBuffer;
	xsig::xconnector mConnect;

	~Internal() {
		mConnect.disconnect();
	}

	void slotPrimitvieFaceNumChanged(uint32_t num) {
		mFaceStateBuffer = makeShareDbObject<XOpenGLBuffer>();

		mFaceStateBuffer->setBufferType(XOpenGLBuffer::ShaderStorageBuffer);

		mFaceStateBuffer->setUsagePattern(XOpenGLBuffer::DynamicDraw);

		mFaceStateBuffer->create();

		std::vector<uint32_t> faceState;
		faceState.resize(num, (uint32_t)PrimitiveState::normal);

		mFaceStateBuffer->bind();
		mFaceStateBuffer->allocate<uint32_t>(faceState.data(), faceState.size());
		auto ss= mFaceStateBuffer->bufferSize();
		mFaceStateBuffer->release();
	}

	void bindBuffer() {
		if (mFaceStateBuffer) {
			mFaceStateBuffer->setBufferBindIdx(3);
		}
	}
};

void XGeometryNodeState::Init()
{
	XBaseRenderNodeState::Init();
	XQ_ATTR_ADD_INIT(AttrFaceNum, 0);
	XQ_ATTR_ADD_INIT(AttrLineNum, 0);
	XQ_ATTR_ADD_INIT(AttrVertexNum, 0);
	XQ_ATTR_ADD_INIT(AttrHasSelect, false);

	mData->mConnect.connect(AttrHasSelect, &XAttr_UInt::sigAttrChanged, [this](sptr<XDataAttribute> attr, XDataChangeType type){
		if (type == XDataChangeType::ItemDataModified) {
			mData->mConnect.connect(AttrFaceNum, &XAttr_UInt::sigAttrChanged, [this](sptr<XDataAttribute> attr, XDataChangeType type) {
				if (type == XDataChangeType::ItemDataModified) {
					mData->slotPrimitvieFaceNumChanged(attr->asDerived<XAttr_UInt>()->getValue());
				}
				}, "FaceNumChanged");
		}
		else {
			mData->mConnect.disconnect("FaceNumChanged");
		}
	},"HasSelectChanged");	
}

void XGeometryNodeState::bindBuffer()
{
	mData->bindBuffer();
}

void XGeometryNodeState::setFaceState(int faceIdx, const PrimitiveState& faceState)
{
	if (AttrFaceNum->getValue() <= faceIdx) {
		return;
	}

	//TODO 此处直接操作buffer，不合理
	uint32_t *pbuffer = (uint32_t*)mData->mFaceStateBuffer->map(XOpenGLBuffer::Access::WriteOnly);
	pbuffer[faceIdx] = (uint32_t)faceState;
	mData->mFaceStateBuffer->unmap();

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	//uint32_t* pbuffer = (uint32_t*)mData->mFaceStateBuffer->mapRange(faceIdx,4,XOpenGLBuffer::RangeAccessFlags::RangeWrite);
	//*pbuffer = (uint32_t)faceState;
	//mData->mFaceStateBuffer->unmap();

	/*mData->mFaceStateBuffer->bind();
	auto ss = mData->mFaceStateBuffer->map2cpu();
	auto ddd = ss->heteroCast<uint32_t>(1);
	mData->mFaceStateBuffer->release();*/
	//内存同步
}

PrimitiveState XGeometryNodeState::getFaceState(int faceIdx)
{
	if (AttrFaceNum->getValue() <= faceIdx) {
		return {};
	}

	//TODO 此处直接操作buffer，不合理
	uint32_t* pbuffer = (uint32_t*)mData->mFaceStateBuffer->map(XOpenGLBuffer::Access::ReadOnly);
	auto ret =pbuffer[faceIdx];
	mData->mFaceStateBuffer->unmap();
	return (PrimitiveState)ret;
}

XGeometryNodeState::XGeometryNodeState() :mData(new Internal)
{
}

XGeometryNodeState::~XGeometryNodeState()
{
}
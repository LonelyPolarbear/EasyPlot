#include "XBaseRenderNode.h"

XBaseRenderNode::XBaseRenderNode()
{

}

XBaseRenderNode::~XBaseRenderNode()
{

}

sptr<XBaseRenderNodeExtInterface> XBaseRenderNode::getExtInterface()
{
	auto names= baseClassName();
	auto findName =XQ_META::ClassName<XBaseRenderNodeExtInterface>();
	if (std::find(names.begin(), names.end(), findName) == names.end()) {
		return nullptr;
	}
	else {
		return asDerived<XBaseRenderNodeExtInterface>();
	}
}

XBaseRenderNodeState::XBaseRenderNodeState()
{

}

XBaseRenderNodeState::~XBaseRenderNodeState()
{

}

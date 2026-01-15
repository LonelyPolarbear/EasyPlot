#include "XRenderNode.h"

#include <atomic>
static std::atomic< uint64_t>  object_id_counter(0);

int64_t XRenderNode3D::getID() const
{
	return m_id;
}

XRenderNode3D::XRenderNode3D()
{
	m_id  = ++object_id_counter;
}

XRenderNode3D::~XRenderNode3D()
{
}
#include "XTimeStamp.h"
#include <atomic>

static std::atomic<TimeStampType> globalTimeStamp(0);

XTimeStamp::XTimeStamp()
{
    Modified();
}

void XTimeStamp::Modified()
{
    m_modifiedTime = (TimeStampType) ++globalTimeStamp;
}

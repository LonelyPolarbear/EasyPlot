#pragma once
#include "databaseApi.h"

typedef unsigned long long TimeStampType;

    class database_API XTimeStamp
    {
    public:
        XTimeStamp();
        ~XTimeStamp() {}
        void Modified();

        TimeStampType  GetMTime() const { return this->m_modifiedTime; };

        bool operator>(const XTimeStamp& rhs) {
            return (this->m_modifiedTime > rhs.m_modifiedTime);
        };

        bool operator<(const XTimeStamp& rhs) {
            return (this->m_modifiedTime < rhs.m_modifiedTime);
        };

        //operator TimeStampType() const { return this->m_modifiedTime; };
    private:
        TimeStampType m_modifiedTime{ 0 };
    };
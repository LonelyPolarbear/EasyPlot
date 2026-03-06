#pragma once
#include "dataBaseApi.h"

typedef unsigned long long TimeStampType;

    class DATABASE_API XTimeStamp
    {
    public:
        XTimeStamp();
        ~XTimeStamp() {}
        void Modified();

        TimeStampType  GetMTime() const { return this->m_modifiedTime; };

        bool operator>(const XTimeStamp& rhs) const {
            return (this->m_modifiedTime > rhs.m_modifiedTime);
        };

        bool operator<(const XTimeStamp& rhs) const{
            return (this->m_modifiedTime < rhs.m_modifiedTime);
        };

        //operator TimeStampType() const { return this->m_modifiedTime; };
    private:
        TimeStampType m_modifiedTime{ 0 };
    };
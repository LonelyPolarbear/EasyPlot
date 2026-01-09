#include "xCustomSource.h"
#include <iostream>
#include <fstream>
#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QFile>
#include <regex>
#include <QSet>


XCustomSource::XCustomSource()
{
}

XCustomSource::~XCustomSource()
{
}

void XCustomSource::updateVertextCoordArray()
{
	m_coord->Modified();
}

void XCustomSource::updateIndexArray()
{
	m_indexs->Modified();
}

void XCustomSource::updateFaceColorArray()
{
	m_FaceColor->Modified();
}

void XCustomSource::updateNormalArray()
{
	m_normal->Modified();
}

void XCustomSource::updateVertexColorArray()
{
	m_VertexColor->Modified();
}
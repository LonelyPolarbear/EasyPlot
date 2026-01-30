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
	m_VertexCoord->Modified();
}

void XCustomSource::updateFaceIndexArray()
{
	m_FaceIndexs->Modified();
}

void XCustomSource::updateFaceColorArray()
{
	m_FaceColor->Modified();
}

void XCustomSource::updateVertextNormalArray()
{
	m_VertexNormal->Modified();
}

void XCustomSource::updateVertexColorArray()
{
	m_VertexColor->Modified();
}

void XCustomSource::updateLineIndexArray()
{
}

void XCustomSource::updateLineColorArray()
{
}

void XCustomSource::updateVertexIndexArray()
{
}

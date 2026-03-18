#include "XObjectInspectorView.h"
#include <QHBoxLayout>
#include "XDataObjectTreeView.h"
#include "XDataObjectTableView.h"
#include "xsignal/XSignal.h"
class XObjectInspectorView::Internal {
public:
	xsig::xconnection con;
	~Internal() {
		con.disconnect();
	}
};

XObjectInspectorView::XObjectInspectorView(QWidget* parent):QWidget(parent),mData(new Internal)
{
	BuildUI();
}

void XObjectInspectorView::setRootObject(std::shared_ptr<XDataObject> root)
{
	m_treeView->setDataObject(root);
	bindSignal();
}

void XObjectInspectorView::bindSignal()
{
	mData->con.disconnect();
	mData->con = xsig::connect(m_treeView, &XDataObjectTreeView::sigCurrentObjectChanged, [&](std::shared_ptr<XDataObject> obj) {
		m_propertyView->setDataObject(obj);
		});
}

void XObjectInspectorView::BuildUI()
{
	this->setLayout(new QHBoxLayout);
	this->layout()->setContentsMargins(0, 0, 0, 0);
	this->layout()->addWidget(m_splitter = new QSplitter(Qt::Vertical,this));
	m_splitter->addWidget(m_treeView = new XDataObjectTreeView(m_splitter));
	m_splitter->addWidget(m_propertyView = new XDataObjectTableView(m_splitter));
}

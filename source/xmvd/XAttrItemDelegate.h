#pragma once
#include "xmvdApi.h"

#include <QItemDelegate>
#include <lib00_utilty/gp/classProcessorFactory.hpp>
#include <dataBase/XDataAttribute.h>

class XAttrItemDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	explicit XAttrItemDelegate(QObject* parent = nullptr);

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,const QModelIndex& index) const override;

	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	
	void setModelData(QWidget* editor, QAbstractItemModel* model,const QModelIndex& index) const override;

	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,const QModelIndex& index) const override;

	void eimtCommitData(QWidget* editor);

	void emitCloseEditor(QWidget* editor);
};

extern void XMVD_API InitXAttrItemDelegate();

extern  XMVD_API classMultiProcessorFactory<
	QWidget* (QWidget*, const QStyleOptionViewItem&, sptr<XDataAttribute>, XAttrItemDelegate*),
	void(QWidget*, sptr<XDataAttribute>),
	void(void* model, QWidget*, sptr<XDataAttribute>, const QModelIndex& index)
> XAttrItemDelegateFactory;
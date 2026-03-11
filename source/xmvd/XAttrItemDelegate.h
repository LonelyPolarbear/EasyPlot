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
extern template class XMVD_API classProcessorFactory<1, QWidget* (QWidget*, const QStyleOptionViewItem&, sptr<XDataAttribute>, XAttrItemDelegate*)>;
extern template class XMVD_API classProcessorFactory<1, void (QWidget*, sptr<XDataAttribute>)>;
extern template class XMVD_API classProcessorFactory<1, void (void* model, QWidget*, sptr<XDataAttribute>, const QModelIndex& index)>;

using XAttrItemDelegateCreateEditorFactory = classProcessorFactory<1, QWidget* (QWidget*, const QStyleOptionViewItem&, sptr<XDataAttribute>, XAttrItemDelegate*)>;
using XAttrItemDelegateSetEditorDataFactory = classProcessorFactory<1, void(QWidget*, sptr<XDataAttribute>)>; 
using XAttrItemDelegatesetModelDataFactory = classProcessorFactory<1, void(void* model, QWidget*, sptr<XDataAttribute>, const QModelIndex& index)>;
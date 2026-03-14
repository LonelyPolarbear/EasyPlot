#include "XAttrItemDelegate.h"
#include "XDataObjectTableModel.h"
#include "XColorCombobox.h"
#include <dataBase/XDataAttribute.h>
#include <QColor>
#include <lib05_shape/XRenderNodeAttribute.h>

XDataObjectTableModel* getXDataObjectTableModel(const QModelIndex& index) {
	return const_cast<XDataObjectTableModel*>(qobject_cast<const XDataObjectTableModel*>(index.model()));
}

XAttrItemDelegate::XAttrItemDelegate(QObject* parent):QItemDelegate(parent)
{
}

QWidget* XAttrItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	XDataObjectTableModel* customModel = getXDataObjectTableModel(index);
	auto attr = customModel->getAttr(index);
	
	if (attr && index.column() ==1) {
		auto className = attr->getClassName();
		if (XAttrItemDelegateCreateEditorFactory::instance().hasProcessor(className)) {
			return XAttrItemDelegateCreateEditorFactory::instance().process(className, parent, option, attr, const_cast<XAttrItemDelegate*>(this));
		}
		else {
			return QItemDelegate::createEditor(parent, option, index);
		}
	}
	else {
		return QItemDelegate::createEditor(parent, option, index);
	}
}

void XAttrItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	XDataObjectTableModel* customModel = getXDataObjectTableModel(index);
	auto attr = customModel->getAttr(index);

	//如果是颜色控件
	if (attr && index.column() == 1) {
		auto className = attr->getClassName();
		if (XAttrItemDelegateSetEditorDataFactory::instance().hasProcessor(className)) {
			XAttrItemDelegateSetEditorDataFactory::instance().process(className,editor,attr);
		}
		else {
			return QItemDelegate::setEditorData(editor, index);
		}
	}
	else {
		return QItemDelegate::setEditorData(editor, index);
	}
	
}

void XAttrItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	XDataObjectTableModel* customModel = getXDataObjectTableModel(index);
	auto attr = customModel->getAttr(index);

	//如果是颜色控件
	if (attr && index.column() == 1) {
		auto className = attr->getClassName();
		if (XAttrItemDelegatesetModelDataFactory::instance().hasProcessor(className)) {
			XAttrItemDelegatesetModelDataFactory::instance().process(className, model, editor, attr, index);
		}
		else {
			return QItemDelegate::setModelData(editor, model, index);
		}
	}
	else {
		return QItemDelegate::setModelData(editor, model, index);
	}
}

void XAttrItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
	//return QItemDelegate::updateEditorGeometry(editor, option, index);
}

void XAttrItemDelegate::eimtCommitData(QWidget* editor)
{
	emit commitData(editor);
}

void XAttrItemDelegate::emitCloseEditor(QWidget* editor)
{
	emit closeEditor(editor);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
QWidget* CreateEditor_XAttr_Color(QWidget* parent, const QStyleOptionViewItem& option, sptr<XDataAttribute> attr, XAttrItemDelegate* delegate) {
	auto className = attr->getClassName();
	if (className == XQ_META::ClassName<XAttr_Color>()) {
		XColorComboBox* editor = new XColorComboBox(parent);
		editor->enableColorDialog();
		auto color_attr = attr->asDerived<XAttr_Color>();
		auto color = color_attr->getValue();
		editor->appendColor(QColor(color.r(), color.g(), color.b(), color.a()));
		editor->appendColor(Qt::blue);

		//// 设置样式
		editor->setStyleSheet("EColorComboBox {"
			"padding: 4px;"
			"border: 1px solid #cccccc;"
			"border-radius: 4px;"
			"background: white;"
			"}"
			"EColorComboBox:hover {"
			"border-color: #66afe9;"
			"}"
			"EColorComboBox::drop-down {"
			"subcontrol-origin: padding;"
			"subcontrol-position: top right;"
			"width: 20px;"
			"border-left-width: 1px;"
			"border-left-color: #cccccc;"
			"border-left-style: solid;"
			"}");

		// 连接信号槽，当编辑完成时提交数据
		QObject::connect(editor, &XColorComboBox::activated,
			[delegate, editor](const QColor& color) {
				editor->appendColor(color);
				editor->setCurrentColor(color);
				delegate->eimtCommitData(editor);
				delegate->emitCloseEditor(editor);
			});

		return editor;
	}
	return nullptr;
}

void SetEditorData_XAttr_Color(QWidget* editor, sptr<XDataAttribute> attr){
	auto color_attr = attr->asDerived<XAttr_Color>();
	auto color = color_attr->getValue();
	XColorComboBox* comboBox = qobject_cast<XColorComboBox*>(editor);
	
	QColor c(color.r(), color.g(), color.b(), color.a());
	comboBox->appendColor(c);
	comboBox->setCurrentColor(c);
}

void SetModelData_XAttr_Color(void* model_, QWidget* editor, sptr<XDataAttribute>, const QModelIndex& index) {
	XColorComboBox* comboBox = qobject_cast<XColorComboBox*>(editor);
	XDataObjectTableModel * model = (XDataObjectTableModel*)model_;
	if (comboBox) {
		auto c = comboBox->currentColor();
		model->setData(index, QVariant::fromValue(c), Qt::EditRole);
	}
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
template<typename E>
QWidget* CreateEditor_XAttr_Enum(QWidget* parent, const QStyleOptionViewItem& option, sptr<XDataAttribute> attr, XAttrItemDelegate* delegate) {
	auto className = attr->getClassName();
	if (className == XQ_META::ClassName<XAttr_Enum<E>>()) {
		QComboBox* editor = new QComboBox(parent);

		auto map = XQ_META::get_enum_map<E>();
		for (auto s : map) {
			editor->addItem(QString::fromStdString(s.second), s.first);
		}

		// 连接信号槽，当编辑完成时提交数据
		QObject::connect(editor, qOverload<int>(&QComboBox::currentIndexChanged),
			[delegate, editor](int index) {
				QVariant currentValue = editor->itemData(index);
				delegate->eimtCommitData(editor);
				delegate->emitCloseEditor(editor);
			});

		return editor;
	}
	return nullptr;
}

template<typename E>
void SetEditorData_XAttr_Enum(QWidget* editor, sptr<XDataAttribute> attr) {
	auto color_attr = attr->asDerived<XAttr_Enum<E>>();
	auto mode = color_attr->getValue();
	QComboBox* comboBox = qobject_cast<QComboBox*>(editor);

	comboBox->setCurrentText( QString::fromStdString(XQ_META::enum_to_string(mode)) );
}

void SetModelData_XAttr_Enum(void* model_, QWidget* editor, sptr<XDataAttribute>, const QModelIndex& index) {
	QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
	XDataObjectTableModel* model = (XDataObjectTableModel*)model_;
	if (comboBox) {
		auto i = comboBox->currentIndex();
		auto variant = comboBox->itemData(i);
		model->setData(index, variant, Qt::EditRole);
	}
}

template class XMVD_API classProcessorFactory<1, QWidget* (QWidget*, const QStyleOptionViewItem&, sptr<XDataAttribute>, XAttrItemDelegate*)>;
template class XMVD_API classProcessorFactory<1, void(QWidget*, sptr<XDataAttribute>)>;
template class XMVD_API classProcessorFactory<1, void(void* model, QWidget*, sptr<XDataAttribute>, const QModelIndex& index)>;

void XMVD_API InitXAttrItemDelegate()
{

	XAttrItemDelegateCreateEditorFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Color>(), CreateEditor_XAttr_Color);
	XAttrItemDelegateSetEditorDataFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Color>(), SetEditorData_XAttr_Color);
	XAttrItemDelegatesetModelDataFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Color>(), SetModelData_XAttr_Color);

	XAttrItemDelegateCreateEditorFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Enum<PolygonMode>>(), CreateEditor_XAttr_Enum<PolygonMode>);
	XAttrItemDelegateSetEditorDataFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Enum<PolygonMode>>(), SetEditorData_XAttr_Enum<PolygonMode>);
	XAttrItemDelegatesetModelDataFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Enum<PolygonMode>>(), SetModelData_XAttr_Enum);

	XAttrItemDelegateCreateEditorFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Enum<PrimitveType>>(), CreateEditor_XAttr_Enum<PrimitveType>);
	XAttrItemDelegateSetEditorDataFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Enum<PrimitveType>>(), SetEditorData_XAttr_Enum<PrimitveType>);
	XAttrItemDelegatesetModelDataFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Enum<PrimitveType>>(), SetModelData_XAttr_Enum);

	XAttrItemDelegateCreateEditorFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Enum<ColorMode>>(), CreateEditor_XAttr_Enum<ColorMode>);
	XAttrItemDelegateSetEditorDataFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Enum<ColorMode>>(), SetEditorData_XAttr_Enum<ColorMode>);
	XAttrItemDelegatesetModelDataFactory::instance().registerProcessor(XQ_META::ClassName<XAttr_Enum<ColorMode>>(), SetModelData_XAttr_Enum);
}

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
	int curAttrOffset = 0;
	auto attr = customModel->getAttr(index);
	
	if (attr && index.column() ==1) {
		auto className = attr->getClassName();
		if (XAttrItemDelegateFactory.hasProcessor<0>(className)) {
			return XAttrItemDelegateFactory.process<0>(className, parent, option, attr, const_cast<XAttrItemDelegate*>(this));
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
	int curAttrOffset = 0;
	auto attr = customModel->getAttr(index);

	//如果是颜色控件
	if (attr && index.column() == 1) {
		auto className = attr->getClassName();
		if (XAttrItemDelegateFactory.hasProcessor<1>(className)) {
			XAttrItemDelegateFactory.process<1>(className,editor,attr);
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
	int curAttrOffset = 0;
	auto attr = customModel->getAttr(index);

	//如果是颜色控件
	if (attr && index.column() == 1) {
		auto className = attr->getClassName();
		if (XAttrItemDelegateFactory.hasProcessor<2>(className)) {
			XAttrItemDelegateFactory.process<2>(className, model, editor, attr, index);
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

using CreateEditorFn = QWidget*(QWidget*, const QStyleOptionViewItem&, sptr<XDataAttribute>, XAttrItemDelegate*);
using SetEditorDataFn = void(QWidget*, sptr<XDataAttribute>);
using SetModelDataFn = void(void* model_, QWidget*, sptr<XDataAttribute>, const QModelIndex& index);


classMultiProcessorFactory<CreateEditorFn,SetEditorDataFn,SetModelDataFn> XAttrItemDelegateFactory;

template XMVD_API void classMultiProcessorFactory<CreateEditorFn, SetEditorDataFn, SetModelDataFn>::registerProcessor<0>(const std::string&, const std::function<CreateEditorFn>&);
template XMVD_API void classMultiProcessorFactory<CreateEditorFn, SetEditorDataFn, SetModelDataFn>::registerProcessor<1>(const std::string&, const std::function<SetEditorDataFn>&);
template XMVD_API void classMultiProcessorFactory<CreateEditorFn, SetEditorDataFn, SetModelDataFn>::registerProcessor<2>(const std::string&, const std::function<SetModelDataFn>&);

void XMVD_API InitXAttrItemDelegate()
{
	XAttrItemDelegateFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Color>(), CreateEditor_XAttr_Color);
	XAttrItemDelegateFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Color>(), SetEditorData_XAttr_Color);
	XAttrItemDelegateFactory.registerProcessor<2>(XQ_META::ClassName<XAttr_Color>(), SetModelData_XAttr_Color);

	XAttrItemDelegateFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Enum<PolygonMode>>(), CreateEditor_XAttr_Enum<PolygonMode>);
	XAttrItemDelegateFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Enum<PolygonMode>>(), SetEditorData_XAttr_Enum<PolygonMode>);
	XAttrItemDelegateFactory.registerProcessor<2>(XQ_META::ClassName<XAttr_Enum<PolygonMode>>(), SetModelData_XAttr_Enum);

	XAttrItemDelegateFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Enum<PrimitveType>>(), CreateEditor_XAttr_Enum<PrimitveType>);
	XAttrItemDelegateFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Enum<PrimitveType>>(), SetEditorData_XAttr_Enum<PrimitveType>);
	XAttrItemDelegateFactory.registerProcessor<2>(XQ_META::ClassName<XAttr_Enum<PrimitveType>>(), SetModelData_XAttr_Enum);

	XAttrItemDelegateFactory.registerProcessor<0>(XQ_META::ClassName<XAttr_Enum<ColorMode>>(), CreateEditor_XAttr_Enum<ColorMode>);
	XAttrItemDelegateFactory.registerProcessor<1>(XQ_META::ClassName<XAttr_Enum<ColorMode>>(), SetEditorData_XAttr_Enum<ColorMode>);
	XAttrItemDelegateFactory.registerProcessor<2>(XQ_META::ClassName<XAttr_Enum<ColorMode>>(), SetModelData_XAttr_Enum);
}

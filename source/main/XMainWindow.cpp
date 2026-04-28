#include "XMainWindow.h"
#include "ui_XMainWindow.h"
#include <qdockwidget.h>
#include <QTextEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <easyPlot/XEasyPlotWidget.h>
#include <xmvd/XObjectInspectorView.h>
#include <qtabwidget.h>
#include <QTextEdit>

#include <render/XRender.h>
#include <xrendernode/XRenderNode.h>
#include <render/XOpenGLRenderWindow.h>
#include <xlog/XLogger.h>
#include <QThread>
#include <QTextBlock>

class XMainWindow::Internal {
public:
	QTabWidget* leftTab = nullptr;
	QTabWidget* rightTab = nullptr;
	QTabWidget* botTab = nullptr;
	XEasyPlotWidget *plotWidget = nullptr;

	XObjectInspectorView *objectInspector = nullptr;
	QTextEdit* textEdit = nullptr;
	xsig::xconnector connector;

	~Internal() {
		connector.disconnect();
	}
};

XMainWindow::XMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::XMainWindow), mData(new Internal)
{
    ui->setupUi(this);
	BuildUI();
	InitView();
}

XMainWindow::~XMainWindow()
{
    delete ui;
}

void XMainWindow::BuildUI()
{
	// 中央区域：主显示内容
	mData->plotWidget = new XEasyPlotWidget(this);
	this->centralWidget()->setLayout(new QHBoxLayout);
	this->centralWidget()->layout()->setContentsMargins(1,1,1,1);
	this->centralWidget()->layout()->addWidget(mData->plotWidget);

	// 左侧 Dock
#if 0
	QDockWidget* leftDock = new QDockWidget("left", this);
	leftDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	mData->leftTab = new QTabWidget(this);
	leftDock->setWidget(mData->leftTab);
	this->addDockWidget(Qt::LeftDockWidgetArea, leftDock);
#endif
#if 1
	// 底部 Dock
	QDockWidget* bottomDock = new QDockWidget("bottom", this);
	bottomDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	mData->botTab = new QTabWidget(this);
	bottomDock->setWidget(mData->botTab);
	this->addDockWidget(Qt::BottomDockWidgetArea, bottomDock);
	this->resizeDocks({ bottomDock }, {80 }, Qt::Horizontal);
#endif
	// 右侧 Dock
	QDockWidget* rightDock = new QDockWidget("right", this);
	rightDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	mData->rightTab = new QTabWidget(this);
	rightDock->setWidget(mData->rightTab);
	this->addDockWidget(Qt::RightDockWidgetArea, rightDock);
	this->resizeDocks({ rightDock }, { 50 }, Qt::Vertical);
	
	InitLeftTab();
	InitRightTab();
	InitBotTab();
}

void XMainWindow::InitLeftTab()
{
}

void XMainWindow::InitRightTab()
{
	mData->objectInspector = new XObjectInspectorView(this);
	mData->rightTab->setTabPosition(QTabWidget::South);
	mData->rightTab->addTab(mData->objectInspector, "Attribute");
}

void XMainWindow::InitBotTab()
{
	mData->botTab->setTabPosition(QTabWidget::South);
	mData->textEdit = new QTextEdit(this);
	mData->botTab->addTab(mData->textEdit, "Log");
	auto sink = getXForwardSink();
	mData->connector.connect(sink, &XForwardSink::SigLog, [this](const std::string& str,int level) {
		if (QThread::currentThread() == qApp->thread()) {
			/*QMetaObject::invokeMethod(
				this, "SlotAddLog", Qt::DirectConnection,
				Q_ARG(const std::string&, str)
			);*/
			SlotAddLog(str,level);
		}
		else {
			bool b = QMetaObject::invokeMethod(
				this, "SlotAddLog", Qt::QueuedConnection,
				Q_ARG(const std::string&, str),
				Q_ARG(int, level)
			);
		}
		}, "SigLog");

	XLOG_TRACE("XMainWindow::InitBotTab");
	XLOG_DEBUG("XMainWindow::InitBotTab");
	XLOG_INFO("XMainWindow::InitBotTab");
	XLOG_WARN("XMainWindow::InitBotTab");
	XLOG_ERROR("XMainWindow::InitBotTab");
	XLOG_CRITICAL("XMainWindow::InitBotTab");
}

void XMainWindow::InitView()
{
	mData->objectInspector->setRootObject(mData->plotWidget->getRenderWindow());
	mData->plotWidget->test2();
}

Q_INVOKABLE void XMainWindow::SlotAddLog(const std::string& log,int level)
{	
	//from AI
	// 1. 将 std::string 转为 QString 并追加到末尾
	QString logText = QString::fromStdString(log);
	mData->textEdit->append(logText);

	// 2. 获取文档，并找到包含实际文本的最后一个块（跳过末尾的空块）
	QTextDocument* doc = mData->textEdit->document();
	QTextBlock block = doc->lastBlock();
	while (block.isValid() && block.text().trimmed().isEmpty()) {
		block = block.previous();  // 向前找上一个块
	}
	if (!block.isValid()) return;  // 没有有效块

	QString blockText = block.text();
	int blockPos = block.position();  // 块在文档中的起始位置

	// 3. 定义关键字及其颜色（内部单词与 spdlog 控制台颜色对应）
	struct KeywordColor {
		QString keyword;   // 带方括号的关键字，如 "[error]"
		QString inner;     // 内部单词（仅用于参考）
		QColor color;
	};
	static std::vector<KeywordColor> keywordColors = {
		{"[trace]",   "trace",   Qt::black},
		{"[debug]",   "debug",   Qt::blue},
		{"[info]",    "info",    Qt::green},
		{"[warning]", "warning", QColor(255, 165, 0)}, // 橙色
		{"[error]",   "error",   Qt::red},
		{"[critical]",   "critical",   Qt::white}		//红底白字
	};

	// 4. 在块文本中搜索每个关键字，并为内部单词单独着色
	for (const auto& kc : keywordColors) {
		int pos = 0;
		while ((pos = blockText.indexOf(kc.keyword, pos, Qt::CaseInsensitive)) != -1) {
			// 计算内部单词在文档中的绝对起始和结束位置
			int startPos = blockPos + pos + 1;                     // 跳过 '['
			int endPos = blockPos + pos + kc.keyword.length() - 1; // 跳过 ']'

			// 设置颜色
			QTextCursor cursor(doc);
			cursor.setPosition(startPos);
			cursor.setPosition(endPos, QTextCursor::KeepAnchor);
			QTextCharFormat fmt;
			
			fmt.setFontWeight(QFont::Bold);   // 加粗
			if (kc.keyword == "[critical]") {
				fmt.setBackground(Qt::red);
				fmt.setForeground(Qt::white);
			}
			else {
				fmt.setForeground(kc.color);
			}
			cursor.setCharFormat(fmt);

			// 继续向后搜索
			pos += kc.keyword.length();
		}
	}

	 // 4. 限制显示条数：保留最近 50 条消息
	const int MAX_BLOCKS = 20;
	int blockCount = doc->blockCount();
	if (blockCount > MAX_BLOCKS) {
		int deleteCount = blockCount - MAX_BLOCKS;

		QTextCursor cursor(doc);
		cursor.movePosition(QTextCursor::Start);  // 移动到文档开头
		for (int i = 0; i < deleteCount; ++i) {
			// 每次选择下一个块（包含其换行符）
			cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
		}
		cursor.removeSelectedText();  // 删除所选块

		// 删除后光标停留在删除区域末尾，可能留下一个空行，将其移除（可选）
		// 再将光标移到末尾，确保滚动到底部
		cursor.movePosition(QTextCursor::End);
		mData->textEdit->setTextCursor(cursor);
	}
}

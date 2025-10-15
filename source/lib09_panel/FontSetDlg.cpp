#include "FontSetDlg.h"
#include "ui_FontSetDlg.h"
#include <qbuttongroup.h>

FontSetDlg::FontSetDlg(const TextSetInfo& info,QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FontSetDlg)
{
    ui->setupUi(this);

	QButtonGroup* hGroup = new QButtonGroup(this);
    QButtonGroup* vGroup = new QButtonGroup(this);

    hGroup->addButton(ui->btnHAlignLeft);
    hGroup->addButton(ui->btnHAlignMiddle);
    hGroup->addButton(ui->btnHAlignRight);

	vGroup->addButton(ui->btnVAlignBottom);
	vGroup->addButton(ui->btnVAlignMiddle);
	vGroup->addButton(ui->btnVAlignTop);

    hGroup->setExclusive(true);
    vGroup->setExclusive(true);

    connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(slotBtnOk()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(slotBtnCancel()));

    ui->content->setPlainText(info.content);
    ui->pos_x->setValue(info.x);
    ui->pos_y->setValue(info.y);

    if (info.alignH == -1) {
        ui->btnHAlignLeft->setChecked(true);
    }
    else if (info.alignH == 0) {
        ui->btnHAlignMiddle->setChecked(true);
    }
    else if (info.alignH == 1) {
        ui->btnHAlignRight->setChecked(true);
    }

    if (info.alignV == 1) {
        ui->btnVAlignBottom->setChecked(true);
    }
    else if (info.alignV == 0) {
        ui->btnVAlignMiddle->setChecked(true);
    }
    else if (info.alignV == -1) {
        ui->btnVAlignTop->setChecked(true);
    }

    ui->isFixWidth->setChecked(info.isFixed);
    ui->fixWidth->setValue(info.fixWidth);

}

FontSetDlg::~FontSetDlg()
{
    delete ui;
}

void FontSetDlg::slotBtnOk(){
    if (ui->btnHAlignLeft->isChecked()) {
        mTextInfo.alignH = -1;
    }
    else if (ui->btnHAlignMiddle->isChecked()) {
        mTextInfo.alignH = 0;
    }
    else if (ui->btnHAlignRight->isChecked()) {
        mTextInfo.alignH = 1;
    }

    if (ui->btnVAlignBottom->isChecked()) {
        mTextInfo.alignV = 1;
    }
    else if (ui->btnVAlignMiddle->isChecked()) {
        mTextInfo.alignV = 0;
    }
    else if (ui->btnVAlignTop->isChecked()) {
        mTextInfo.alignV = -1;
    }

    mTextInfo.content = ui->content->toPlainText();
    mTextInfo.x = ui->pos_x->value();
    mTextInfo.y = ui->pos_y->value();
    mTextInfo.isFixed = ui->isFixWidth->isChecked();
    mTextInfo.fixWidth = ui->fixWidth->value();

    emit sigSetTextInfo();
    m_callback();
    accepted();
}

void FontSetDlg::slotBtnCancel(){
    close();
}

FontSetDlg::TextSetInfo FontSetDlg::getTextInfo() {
    return mTextInfo;
}

void FontSetDlg::addFun(std::function<void()> f) {
    m_callback = f;
}

void  FontSetDlg::setTexInfo(const TextSetInfo& info) {
    mTextInfo = info;
	ui->content->setPlainText(info.content);
	ui->pos_x->setValue(info.x);
	ui->pos_y->setValue(info.y);

	if (info.alignH == -1) {
		ui->btnHAlignLeft->setChecked(true);
	}
	else if (info.alignH == 0) {
		ui->btnHAlignMiddle->setChecked(true);
	}
	else if (info.alignH == 1) {
		ui->btnHAlignRight->setChecked(true);
	}

	if (info.alignV == 1) {
		ui->btnVAlignBottom->setChecked(true);
	}
	else if (info.alignV == 0) {
		ui->btnVAlignMiddle->setChecked(true);
	}
	else if (info.alignV == -1) {
		ui->btnVAlignTop->setChecked(true);
	}

	ui->isFixWidth->setChecked(info.isFixed);
	ui->fixWidth->setValue(info.fixWidth);
}

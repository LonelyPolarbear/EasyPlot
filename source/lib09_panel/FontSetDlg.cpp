#include "FontSetDlg.h"
#include "ui_FontSetDlg.h"
#include <qbuttongroup.h>
#include "ColorPickButton.h"

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

    if (info.tackMode == 0 ) {
        ui->Local_complete->setChecked(true);
    }
    if (info.tackMode == 1) {
        ui->Local_center->setChecked(true);
    }
    if (info.tackMode == 3) {
        ui->scene_screen_center->setChecked(true);
    }
    if (info.tackMode == 2) {
        ui->scene_screen_complete->setChecked(true);
    }

    if (info.refOrigin == 0) {
        ui->leftTop->setChecked(true);
    }
    else if (info.refOrigin == 1) {
        ui->left_bottom->setChecked(true);
    }
    else if (info.refOrigin == 2) {
        ui->right_top->setChecked(true);
    }
	else if (info.refOrigin == 3) {
		ui->right_bottom->setChecked(true);
	}

    
    ui->btnColorSet->setCurrentColor(QColor(255*info.color.x(), 255 * info.color.y(), 255 * info.color.z(), 255 * info.color.w()));
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

    if (ui->Local_complete->isChecked()) {
        mTextInfo.tackMode = 0;
    }
    if (ui->Local_center->isChecked()) {
        mTextInfo.tackMode = 1;
    }
    if (ui->scene_screen_center->isChecked()) {
        mTextInfo.tackMode = 3;
    }
    if (ui->scene_screen_complete->isChecked()) {
        mTextInfo.tackMode = 2;
    }

    if (ui->leftTop->isChecked()) {
        mTextInfo.refOrigin = 0;
    }
    else if (ui->left_bottom->isChecked()) {
        mTextInfo.refOrigin = 1;
    }
    else if (ui->right_top->isChecked()) {
        mTextInfo.refOrigin = 2;
    }
	else if (ui->right_bottom->isChecked()) {
		mTextInfo.refOrigin = 3;
	}

    mTextInfo.content = ui->content->toPlainText();
    mTextInfo.x = ui->pos_x->value();
    mTextInfo.y = ui->pos_y->value();
    mTextInfo.isFixed = ui->isFixWidth->isChecked();
    mTextInfo.fixWidth = ui->fixWidth->value();
    auto c = ui->btnColorSet->currentColor();
    mTextInfo.color = myUtilty::Vec4f(c.redF(), c.greenF(), c.blueF(), c.alphaF());

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
    ui->btnColorSet->setCurrentColor(QColor(255 * info.color.x(), 255 * info.color.y(), 255 * info.color.z(), 255 * info.color.w()));

	if (info.tackMode == 0) {
		ui->Local_complete->setChecked(true);
	}
	if (info.tackMode == 1) {
		ui->Local_center->setChecked(true);
	}
	if (info.tackMode == 3) {
		ui->scene_screen_center->setChecked(true);
	}
	if (info.tackMode == 2) {
		ui->scene_screen_complete->setChecked(true);
	}

	if (info.refOrigin == 0) {
		ui->leftTop->setChecked(true);
	}
	else if (info.refOrigin == 1) {
		ui->left_bottom->setChecked(true);
	}
	else if (info.refOrigin == 2) {
		ui->right_top->setChecked(true);
	}
	else if (info.refOrigin == 3) {
		ui->right_bottom->setChecked(true);
	}
}

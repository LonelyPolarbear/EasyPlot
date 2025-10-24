#include "ScreenShotDlg.h"
#include "ui_ScreenShotDlg.h"
#include <QStyle>
#include <QFileDialog>

ScreenShotDlg::ScreenShotDlg(int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ScreenShotDlg)
{
    ui->setupUi(this);
    ui->btnFile->setIcon (style()->standardIcon(QStyle::SP_FileIcon));

    connect(ui->btnFile, &QToolButton::clicked, this, [this](){
		auto fileName = QFileDialog::getSaveFileName(this, tr("Save Image"),
			qApp->applicationDirPath(),
			tr("Image Files (*.png *.jpg *.bmp)"));
		if (!fileName.isEmpty()) {
			ui->filePath->setText(fileName);
		}
    });

	connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(slotBtnOk()));
	connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(slotBtnCancel()));

	ui->src_x->setValue(src_x);
	ui->src_y->setValue(src_y);
	ui->src_w->setValue(src_w);
	ui->src_h->setValue(src_h);

	ui->dest_x->setValue(dst_x);
	ui->dest_y->setValue(dst_y);
	ui->dest_w->setValue(dst_w);
	ui->dest_h->setValue(dst_h);
}

ScreenShotDlg::~ScreenShotDlg()
{
    delete ui;
}

void ScreenShotDlg::slotBtnCancel()
{
	rejected();
}

void ScreenShotDlg::slotBtnOk()
{
	emit sigScreenShot(ui->src_x->value(), ui->src_y->value(), ui->src_w->value(), ui->src_h->value(),
	ui->dest_x->value(), ui->dest_y->value(), ui->dest_w->value(), ui->dest_h->value(), ui->filePath->text());
	accepted();
}

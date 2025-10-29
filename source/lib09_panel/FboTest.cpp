#include "FboTest.h"
#include "ui_FboTest.h"
#include <QStyle>
#include <QFileDialog>

FboTest::FboTest(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FboTest)
{
    ui->setupUi(this);

	ui->btnFile->setIcon(style()->standardIcon(QStyle::SP_FileIcon));

	connect(ui->btnFile, &QToolButton::clicked, this, [this]() {
		auto fileName = QFileDialog::getSaveFileName(this, tr("Save Image"),
			qApp->applicationDirPath(),
			tr("Image Files (*.png *.jpg *.bmp)"));
		if (!fileName.isEmpty()) {
			ui->filePath->setText(fileName);
		}
		});

	connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(slotBtnOk()));
	connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(slotBtnCancel()));

	ui->picture_color->setScaledContents(true);
	ui->picture_depth->setScaledContents(true);
}

FboTest::~FboTest()
{
    delete ui;
}

void FboTest::slotBtnOk()
{
	auto width = ui->src_w->value();
	auto height = ui->src_h->value();

	bool enableTest =ui->depthEnable->isChecked();
	bool multiSampleEnable = ui->mulisampleEnable->isChecked();
	int samples = ui->sampleNum->value();

	int attachMode = 1;
	if (ui->attachment_color->isChecked()) {
		attachMode =1;
	}
	if (ui->attachment_colorDepth->isChecked()) {
		attachMode = 2;
	}
	if (ui->attachment_colorDepthStencil->isChecked()) {
		attachMode = 3;
	}

	emit sigTest( width, height, enableTest, multiSampleEnable, samples, attachMode);

	accepted();
}

void FboTest::slotBtnCancel()
{
    rejected();
}

void FboTest::setColorImage(QImage img)
{
	ui->picture_color->setPixmap(QPixmap::fromImage(img));
}

void FboTest::setDepthImage(QImage img)
{
	ui->picture_depth->setPixmap(QPixmap::fromImage(img));
}

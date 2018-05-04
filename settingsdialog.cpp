#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "programsettings.h"
#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    blockSignals(true);
    ui->groupBoxRawData->setChecked(ProgramSettings::isDataWritingEnabled());
    ui->lineEditRawDataOutputDir->setText(ProgramSettings::getOutputDirName());
    ui->checkBoxCompress->setChecked(ProgramSettings::isCompressionEnabled());
    ui->spinBoxRate->setValue(ProgramSettings::getRateHistorySize());
    blockSignals(false);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_groupBoxRawData_clicked(bool checked)
{
    ProgramSettings::setDataWritingEnabled(checked);
}

void SettingsDialog::on_pushButtonSelectDir_clicked()
{
    QString dirName = QFileDialog::getExistingDirectory();
    if( ! dirName.isEmpty() )
    {
        ProgramSettings::setOutputDirName(dirName);
        ui->lineEditRawDataOutputDir->setText(dirName);
    }
}

void SettingsDialog::on_checkBoxCompress_clicked(bool checked)
{
    ProgramSettings::setCompressionEnabled(checked);
}

void SettingsDialog::on_spinBoxRate_valueChanged(int arg1)
{
    ProgramSettings::setRateHistorySize(arg1);
}

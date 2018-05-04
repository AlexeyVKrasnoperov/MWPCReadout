#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

private slots:

    void on_groupBoxRawData_clicked(bool checked);

    void on_pushButtonSelectDir_clicked();

    void on_checkBoxCompress_clicked(bool checked);

    void on_spinBoxRate_valueChanged(int arg1);

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H

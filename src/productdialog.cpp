#include "productdialog.h"
#include "ui_productdialog.h"

ProductDialog::ProductDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::ProductDialog)
{
    ui->setupUi(this);
}

ProductDialog::~ProductDialog()
{
    delete ui;
}

void ProductDialog::setProductId(int id)
{
    ui->idSpinBox->setValue(id);
}

QString ProductDialog::productName()
{
    return ui->nameLineEdit->text();
}

double ProductDialog::productPrice()
{
    return ui->priceSpinBox->value();
}

int ProductDialog::productId()
{
    return ui->idSpinBox->value();
}

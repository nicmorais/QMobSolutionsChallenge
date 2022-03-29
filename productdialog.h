#ifndef PRODUCTDIALOG_H
#define PRODUCTDIALOG_H

#include <QDialog>

namespace Ui
{
    class ProductDialog;
}

class ProductDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit ProductDialog(QWidget* parent = nullptr);
        ~ProductDialog();
        void setProductId(int id);
        int productId();
        QString productName();
        double productPrice();

    private:
        Ui::ProductDialog* ui;
};

#endif // PRODUCTDIALOG_H

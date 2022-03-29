#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "restmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        MainWindow(QWidget* parent = nullptr);
        ~MainWindow();

    private:
        Ui::MainWindow* ui;
        RESTModel* restModel;

    private Q_SLOTS:
        void removeSelectedItem();
        void modelNetworkFinished(int statusCode);
        void about();
        void addItem();
        void update();
        void loadTableWidget();
};
#endif // MAINWINDOW_H

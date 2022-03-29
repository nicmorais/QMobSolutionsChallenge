#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "productdialog.h"
#include "noteditabledelegate.h"

#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QSortFilterProxyModel>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Solução com QTableWidget
    loadTableWidget();
    // Solução com QTableView
    //    // Esta solucao nao eh muito interessante, pois exige que o programador realize o connect
    //    auto *restModel = new RESTModel{u"https://3pcu3xj46l.execute-api.sa-east-1.amazonaws.com/items"_qs, this};
    //    connect(restModel, &RESTModel::finished, this, [this, restModel]() {
    //        ui->tableView->setModel(restModel);
    //    });
    // Esta solucao eh melhor, simplesmente faco o setModel e a view sera atualizada sempre que necessario
    restModel = new RESTModel{u"https://3pcu3xj46l.execute-api.sa-east-1.amazonaws.com/items"_qs, this};
    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(restModel);
    ui->tableView->setModel(proxyModel);
    ui->tableView->addAction(ui->actionRemoveSelected);
    ui->tableView->addAction(ui->actionAddItem);
    ui->tableView->addAction(ui->actionUpdate);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    connect(restModel, &RESTModel::networkFinished, this, &MainWindow::modelNetworkFinished);
    connect(ui->actionAboutQt, &QAction::triggered, this, &QApplication::aboutQt);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionAddItem, &QAction::triggered, this, &MainWindow::addItem);
    connect(ui->actionUpdate, &QAction::triggered, this, &MainWindow::update);
    ui->tableView->setLocale(QLocale::system());
    auto* notEditableDelegate = new NotEditableDelegate(this);
    ui->tableView->setItemDelegateForColumn(0, notEditableDelegate);

    toolbar = new QToolBar{this};
    toolbar->addAction(ui->actionAddItem);
    toolbar->addAction(ui->actionRemoveSelected);
    toolbar->addAction(ui->actionUpdate);
    ui->horizontalLayout->addWidget(toolbar);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::removeSelectedItem()
{
    if (ui->tableView->selectionModel()->hasSelection()) {
        int selectedRow = ui->tableView->selectionModel()->selectedRows(0).first().row();
        ui->tableView->model()->removeRow(selectedRow);
        update();
    }
}

void MainWindow::modelNetworkFinished(int statusCode)
{
    if (statusCode != 200 && statusCode != 0) {
        QMessageBox errorMessage{this};
        errorMessage.setWindowTitle(tr("Error"));
        errorMessage.setText(tr("Could not perform request: Status Code %1").arg(statusCode));
        errorMessage.setIcon(QMessageBox::Warning);
        errorMessage.exec();
    }
}

void MainWindow::about()
{
    QMessageBox aboutMessage{this};
    aboutMessage.setWindowTitle(tr("About"));
    aboutMessage.setText(tr("Initially made by Sandro Andrade (QMob Solutions), latterly edited by Nicolas Morais"));
    aboutMessage.setIcon(QMessageBox::Information);
    aboutMessage.exec();
}

void MainWindow::addItem()
{
    ProductDialog productDialog;
    int lastId = 0;

    for (int i = 0; i < restModel->rowCount(restModel->index(0, 0)); ++i) {
        int currentId = restModel->index(i, 0).data(Qt::DisplayRole).toInt();

        if (currentId > lastId) {
            lastId = currentId;
        }
    }

    ++lastId;
    productDialog.setProductId(lastId);

    if (productDialog.exec() == QDialog::Accepted) {
        restModel->insertProduct(productDialog.productId(),
                                 productDialog.productName(),
                                 productDialog.productPrice());
    }
}

void MainWindow::update()
{
    setCursor(Qt::WaitCursor);
    ui->tableView->setEnabled(false);
    restModel->loadData();
    connect(restModel, &RESTModel::networkFinished, this, [this] {
        this->setCursor(Qt::ArrowCursor);
        ui->tableView->setEnabled(true);
    }, Qt::SingleShotConnection);
    loadTableWidget();
}

void MainWindow::loadTableWidget()
{
    ui->tableWidget->clear();
    auto* nam = new QNetworkAccessManager{this};
    auto* reply = nam->get(QNetworkRequest{u"https://3pcu3xj46l.execute-api.sa-east-1.amazonaws.com/items"_qs});
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (statusCode == 200) {
            auto jsonDocument = QJsonDocument::fromJson(reply->readAll());

            if (jsonDocument.isObject()) {
                ui->tableWidget->setColumnCount(jsonDocument.object()["Items"].toArray()[0].toObject().count());
                ui->tableWidget->setRowCount(jsonDocument.object()["Count"].toInt());
                int row = 0;

                for (const auto& value : jsonDocument.object()["Items"].toArray()) {
                    auto object = value.toObject();

                    for (int i = 0; i < object.count(); ++i) {
                        const auto& value = object.value(object.keys().at(i));
                        QTableWidgetItem* item = nullptr;

                        if (value.isString()) {
                            item = new QTableWidgetItem(value.toString());
                        } else if (value.isDouble()) {
                            item = new QTableWidgetItem(QString::number(value.toDouble()));
                        }

                        ui->tableWidget->setItem(row, i, item);
                    }

                    ++row;
                }
            }
        } else {
            qDebug() << "QNetworkReply finished with status code:" << statusCode;
        }

        reply->deleteLater();
    });
}

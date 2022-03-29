#ifndef RESTMODEL_H
#define RESTMODEL_H

#include <QAbstractTableModel>
#include <QJsonObject>
#include <QNetworkAccessManager>

class RESTModel : public QAbstractTableModel
{
        Q_OBJECT
    public:

        explicit RESTModel(const QString& endpoint, QObject* parent = nullptr);

        // QAbstractItemModel interface
        int rowCount(const QModelIndex& parent) const override;
        int columnCount(const QModelIndex& parent) const override;
        QVariant data(const QModelIndex& index, int role) const override;
        bool removeRows(int row, int count, const QModelIndex& parent) override;
        bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;
        QVariant headerData(const int section, const Qt::Orientation orientation, const int role = Qt::DisplayRole) const override;
        void insertProduct(const int productId, const QString& productName, const double productPrice);

    public Q_SLOTS:
        void loadData();

    private:
        bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
        QJsonObject _jsonObject;
        QNetworkAccessManager* _nam;
        QString _endpoint;

    Q_SIGNALS:
        void startedLoading();
        void networkFinished(int statusCode);
};

#endif // RESTMODEL_H

#include "restmodel.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QApplication>

RESTModel::RESTModel(const QString& endpoint, QObject* parent)
    : QAbstractTableModel{parent}
{
    this->_endpoint = endpoint;
    _nam = new QNetworkAccessManager{this};
    loadData();
}

int RESTModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return _jsonObject["Count"].toInt();
}

int RESTModel::columnCount(const QModelIndex& parent) const
{
    return _jsonObject["Items"].toArray()[0].toObject().count();
}

QVariant RESTModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount({}) || index.column() >= columnCount({}))
        return {};

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        auto object = _jsonObject["Items"].toArray()[index.row()].toObject();
        const auto& value = object.value(object.keys().at(index.column()));

        if (value.isString()) {
            return value.toString();
        } else if (value.isDouble()) {
            return value.toDouble();
        }
    }

    return {};
}

bool RESTModel::removeRows(int row, int count, const QModelIndex& parent)
{
    int itemId = index(row, 0).data(Qt::DisplayRole).toInt();
    QNetworkRequest delRequest(_endpoint + '/' + QString::number(itemId));
    auto* reply = _nam->deleteResource(delRequest);
    connect(_nam, &QNetworkAccessManager::finished, this, [this, reply]() {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        Q_EMIT networkFinished(statusCode);
    });
    beginRemoveRows(parent, row, row + count);
    bool success = QAbstractItemModel::removeRows(row, count, parent);
    endRemoveRows();
    Q_EMIT dataChanged(index(row, 0), index(row, row + columnCount(parent) - 1));
    return success;
}

void RESTModel::loadData()
{
    Q_EMIT startedLoading();
    auto* reply = _nam->get(QNetworkRequest{_endpoint});
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        auto jsonDocument = QJsonDocument::fromJson(reply->readAll());

        if (jsonDocument.isObject()) {
            beginResetModel();
            _jsonObject = jsonDocument.object();
            endResetModel();
        }

        Q_EMIT networkFinished(statusCode);
    });
}

QVariant RESTModel::headerData(int section, Qt::Orientation orientation, const int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        QVariant header = _jsonObject["Items"].toArray().at(0).toVariant().toJsonObject().toVariantMap().keys().at(section);
        return header;
    } else {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
}

Qt::ItemFlags RESTModel::flags(const QModelIndex& index) const
{
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool RESTModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    QNetworkRequest putRequest{_endpoint};
    putRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject jsonObj;
    jsonObj["id"] = index.siblingAtColumn(0).data(Qt::DisplayRole).toInt();
    double productPrice;

    if (index.column() == 1) {
        productPrice = value.toDouble();
    } else {
        productPrice = index.siblingAtColumn(1).data(Qt::EditRole).toDouble();
    }

    jsonObj["price"] = productPrice;
    QString productName;

    if (index.column() == 2) {
        productName = value.toString();
    } else {
        productName = index.siblingAtColumn(2).data(Qt::DisplayRole).toString();
    }

    jsonObj["product_name"] = productName;
    QJsonDocument jsonDoc{jsonObj};
    auto* reply = _nam->put(putRequest, jsonDoc.toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        Q_EMIT networkFinished(statusCode);

        if (statusCode == 200) {
            loadData();
        }

        reply->deleteLater();
    });
    return true;
}

bool RESTModel::insertRows(int row, int count, const QModelIndex& parent)
{
    beginInsertRows(parent, row, row + count - 1);
    endInsertRows();
    return true;
}

void RESTModel::insertProduct(const int productId, const QString& productName, const double productPrice)
{
    QNetworkRequest postRequest(_endpoint + '/' + QString::number(productId));
    postRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject jsonObj;
    jsonObj["product_name"] = productName;
    jsonObj["price"] = productPrice;
    QJsonDocument jsonDoc{jsonObj};
    auto* reply = _nam->post(postRequest, jsonDoc.toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        Q_EMIT networkFinished(statusCode);

        if (statusCode == 200)
        {
            loadData();
        }
        reply->deleteLater();
    });
}

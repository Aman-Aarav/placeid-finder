#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QClipboard>
#include <QGuiApplication>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::handleReply);

    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::startSearch);

    // Table setup
    ui->tableWidget->setColumnCount(4);
    QStringList headers = { "OSM ID", "Name", "Type", "Address" };
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->setAlternatingRowColors(true);

    ui->tableWidget->setStyleSheet(
        "QTableWidget {"
        "   background-color: #fdfdfd;"
        "   alternate-background-color: #f5f5f5;"
        "   gridline-color: #dcdcdc;"
        "   font-size: 13px;"
        "}"
        "QHeaderView::section {"
        "   background-color: #2563eb;"
        "   color: white;"
        "   padding: 5px;"
        "   font-weight: bold;"
        "   border: 1px solid #dcdcdc;"
        "}"
        "QTableWidget::item {"
        "   padding: 5px;"
        "}"
        "QTableWidget::item:selected {"
        "   background-color: #d0e1ff;"
        "}"
        );

    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setShowGrid(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startSearch()
{
    QString query = ui->searchBox->text().trimmed();
    if (query.isEmpty()) return;

    // Build API Url
    QString url = "https://nominatim.openstreetmap.org/search";
    QUrlQuery params;
    params.addQueryItem("q", query);
    params.addQueryItem("format", "json");
    params.addQueryItem("addressdetails", "1");
    params.addQueryItem("limit", "15");

    QUrl finalUrl(url);
    finalUrl.setQuery(params);

    QNetworkRequest request(finalUrl);
    request.setHeader(QNetworkRequest::UserAgentHeader, "QtPlaceSearchApp");

    manager->get(request);
}

void MainWindow::handleReply(QNetworkReply *reply)
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    QByteArray response = reply->readAll();

    QJsonDocument doc = QJsonDocument::fromJson(response);
    if (!doc.isArray()) return;

    QJsonArray arr = doc.array();
    ui->statuslabel->setText(QString("%1 results").arg(arr.size()));

    ui->tableWidget->setRowCount(arr.size());

    for (int i = 0; i < arr.size(); ++i)
    {
        QJsonObject obj = arr[i].toObject();

        QString osm_id = QString::number(obj["osm_id"].toInt());
        QString name = obj["display_name"].toString().split(",").first();
        QString type = obj["type"].toString();
        QString address = obj["display_name"].toString();

        // OSM ID (click to copy)
        QTableWidgetItem *idItem = new QTableWidgetItem(osm_id);
        idItem->setFlags(idItem->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(i, 0, idItem);

        // Copy to clipboard on double-click
        connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this,
                [=](int row, int col){
                    if (col == 0)
                        QGuiApplication::clipboard()->setText(ui->tableWidget->item(row, col)->text());
                }
                );

        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(name));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(type));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(address));
    }

    reply->deleteLater();
}

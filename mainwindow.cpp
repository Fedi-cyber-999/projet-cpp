#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    backgroundPixmap.load(":/images/background.png");
    setupNavigation();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Relie chaque bouton de la sidebar à la page correspondante du
// QStackedWidget. C'est la SEULE logique active du projet : le reste
// (recherche, tableau, boutons Modifier/Supprimer, etc.) n'est pas
// connecté et les pages autres que "Stock" restent vides.
void MainWindow::setupNavigation()
{
    navButtons = {
        ui->navAccueilButton,               // page 0
        ui->navStockButton,                 // page 1
        ui->navEmployeButton,                // page 2
        ui->navVehiculeButton,               // page 3
        ui->navLivraisonButton,              // page 4
        ui->navStocksButton,                 // page 5
        ui->navClientCommandeButton,         // page 6
        ui->navFournisseurCommandeButton     // page 7
    };

    for (int i = 0; i < navButtons.size(); ++i) {
        navButtons[i]->setProperty("pageIndex", i);
        connect(navButtons[i], &QPushButton::clicked, this, &MainWindow::onNavButtonClicked);
    }
}

void MainWindow::onNavButtonClicked()
{
    auto *clicked = qobject_cast<QPushButton*>(sender());
    if (!clicked) return;

    for (auto *btn : navButtons)
        btn->setChecked(btn == clicked);

    ui->stackedWidget->setCurrentIndex(clicked->property("pageIndex").toInt());
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (!backgroundPixmap.isNull()) {
        QPixmap scaled = backgroundPixmap.scaled(size(), Qt::KeepAspectRatioByExpanding,
                                                   Qt::SmoothTransformation);
        int x = (scaled.width() - width()) / 2;
        int y = (scaled.height() - height()) / 2;
        painter.drawPixmap(0, 0, scaled, x, y, width(), height());
    }
    QMainWindow::paintEvent(event);
}

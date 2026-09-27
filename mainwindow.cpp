#include "mainwindow.h"
#include "stockpage.h"
#include "placeholderpage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QDateTime>
#include <QButtonGroup>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    backgroundPixmap.load(":/images/background.png");

    auto *central = new QWidget(this);
    central->setObjectName("CentralBg");
    setCentralWidget(central);

    auto *rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    setupSidebar(central);
    rootLayout->addWidget(sidebar);

    auto *rightSide = new QWidget(central);
    auto *rightLayout = new QVBoxLayout(rightSide);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    setupTopBar(rightSide);

    stackedWidget = new QStackedWidget(rightSide);
    stackedWidget->setStyleSheet("background: transparent;");
    rightLayout->addWidget(stackedWidget, 1);

    rootLayout->addWidget(rightSide, 1);

    setupPages();

    // Page "Stock" active par défaut (index 1)
    sidebarButtons[1]->setChecked(true);
    stackedWidget->setCurrentIndex(1);
}

MainWindow::~MainWindow() {}

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

QPushButton* MainWindow::addNavButton(QVBoxLayout *layout, const QString &text,
                                       const QString &iconChar, int pageIndex)
{
    auto *btn = new QPushButton(QString("  %1   %2").arg(iconChar, text));
    btn->setObjectName("NavButton");
    btn->setCheckable(true);
    btn->setProperty("pageIndex", pageIndex);
    btn->setMinimumHeight(38);
    connect(btn, &QPushButton::clicked, this, &MainWindow::onNavClicked);
    layout->addWidget(btn);
    sidebarButtons.append(btn);
    return btn;
}

void MainWindow::setupSidebar(QWidget * /*parent*/)
{
    sidebar = new QWidget();
    sidebar->setObjectName("Sidebar");
    sidebar->setFixedWidth(210);

    auto *layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(12, 16, 12, 16);
    layout->setSpacing(4);

    // Logo
    auto *logoRow = new QHBoxLayout();
    auto *logoIcon = new QLabel("💧");
    logoIcon->setStyleSheet("font-size:24px;");
    auto *logoTextCol = new QVBoxLayout();
    logoTextCol->setSpacing(0);
    auto *logoTitle = new QLabel("AquaTech");
    logoTitle->setObjectName("LogoTitle");
    auto *logoSubtitle = new QLabel("Gestion intelligente de l'eau");
    logoSubtitle->setObjectName("LogoSubtitle");
    logoSubtitle->setWordWrap(true);
    logoTextCol->addWidget(logoTitle);
    logoTextCol->addWidget(logoSubtitle);
    logoRow->addWidget(logoIcon);
    logoRow->addLayout(logoTextCol, 1);
    layout->addLayout(logoRow);
    layout->addSpacing(18);

    // Navigation : 7 éléments (sans doublon de Stock)
    addNavButton(layout, "Accueil",                 "🏠", 0);
    addNavButton(layout, "Stock",                    "📦", 1);   // page détaillée (StockPage)
    addNavButton(layout, "Employé",                  "👥", 2);
    addNavButton(layout, "Véhicule",                 "🚚", 3);
    addNavButton(layout, "Livraison",                "📦", 4);
    addNavButton(layout, "Client && Commande",         "👥", 5);
    addNavButton(layout, "Fournisseur && Commande",    "🤝", 6);

    layout->addStretch();

    auto *logoutBtn = new QPushButton("  ⏻   Log out");
    logoutBtn->setObjectName("LogoutButton");
    logoutBtn->setMinimumHeight(36);
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::handleLogout);
    layout->addWidget(logoutBtn);
}

void MainWindow::setupTopBar(QWidget *parent)
{
    auto *topBar = new QWidget(parent);
    topBar->setObjectName("TopBar");
    topBar->setFixedHeight(56);

    auto *h = new QHBoxLayout(topBar);
    h->setContentsMargins(16, 8, 16, 8);

    auto *statsBtn = new QPushButton("📊  Statistiques");
    statsBtn->setObjectName("StatsButton");

    auto *dateChip = new QLabel("📅  24/09/2026   14:32");
    dateChip->setObjectName("DateChip");

    auto *userChip = new QLabel("👤  Ahmed  ▾");
    userChip->setObjectName("UserChip");

    h->addWidget(statsBtn);
    h->addStretch();
    h->addWidget(dateChip);
    h->addWidget(userChip);

    if (auto *vbox = qobject_cast<QVBoxLayout*>(parent->layout())) {
        vbox->addWidget(topBar);
    }
}

void MainWindow::setupPages()
{
    stackedWidget->addWidget(new PlaceholderPage("Accueil", "Tableau de bord général de l'usine"));
    stackedWidget->addWidget(new StockPage());
    stackedWidget->addWidget(new PlaceholderPage("Module Employé", "Gestion du personnel"));
    stackedWidget->addWidget(new PlaceholderPage("Module Véhicule", "Gestion de la flotte de véhicules"));
    stackedWidget->addWidget(new PlaceholderPage("Module Livraison", "Suivi des livraisons"));
    stackedWidget->addWidget(new PlaceholderPage("Client & Commande", "Gestion des clients et commandes"));
    stackedWidget->addWidget(new PlaceholderPage("Fournisseur & Commande", "Gestion des fournisseurs et commandes"));
}

void MainWindow::onNavClicked()
{
    auto *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    for (auto *b : sidebarButtons)
        b->setChecked(b == btn);

    int index = btn->property("pageIndex").toInt();
    stackedWidget->setCurrentIndex(index);
}

void MainWindow::handleLogout()
{
    close();
}

#include "stockpage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QTabWidget>
#include <QTabBar>
#include <QProgressBar>
#include <QVector>
#include <QTableWidgetItem>
#include <QAbstractItemView>
#include <QPainter>
#include <QPainterPath>
#include <QRandomGenerator>
#include <QStyle>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QScrollArea>

// ------------------------------------------------------------------
// Dessins procéduraux (icônes d'objets, cuve, jauge, code-barres, QR)
// ------------------------------------------------------------------

// Dessin procédural des vignettes produits pour le tableau et la fiche produit
static QPixmap drawProductIcon(const QString &hint, int w, int h)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    if (hint.contains("PF") || hint.contains("Eau minérale") || hint.contains("Eau gazeuse") || hint.contains("0,5L")) {
        // Dessin d'une bouteille d'eau translucide avec bouchon bleu cyan
        p.setPen(QPen(QColor("#5AC8FA"), 1.5));
        p.setBrush(QColor(30, 136, 229, 60));
        
        // Corps bouteille
        QPainterPath bottle;
        bottle.moveTo(w * 0.35, h * 0.22);
        bottle.lineTo(w * 0.65, h * 0.22);
        bottle.lineTo(w * 0.65, h * 0.32);
        bottle.cubicTo(QPointF(w * 0.85, h * 0.45), QPointF(w * 0.85, h * 0.55), QPointF(w * 0.85, h * 0.88));
        bottle.lineTo(w * 0.15, h * 0.88);
        bottle.cubicTo(QPointF(w * 0.15, h * 0.55), QPointF(w * 0.15, h * 0.45), QPointF(w * 0.35, h * 0.32));
        bottle.closeSubpath();
        p.drawPath(bottle);

        // Eau à l'intérieur
        QPainterPath water;
        water.moveTo(w * 0.20, h * 0.50);
        water.cubicTo(QPointF(w * 0.50, h * 0.48), QPointF(w * 0.50, h * 0.52), QPointF(w * 0.80, h * 0.50));
        water.lineTo(w * 0.82, h * 0.86);
        water.lineTo(w * 0.18, h * 0.86);
        water.closeSubpath();
        p.fillPath(water, QColor(0, 191, 255, 150));

        // Bouchon
        p.setBrush(QColor("#00E5FF"));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(QRectF(w * 0.35, h * 0.10, w * 0.30, h * 0.12), 2, 2);

        // Reflet lumineux
        p.setPen(QPen(QColor(255, 255, 255, 160), 1.5));
        p.drawLine(QPointF(w * 0.30, h * 0.45), QPointF(w * 0.30, h * 0.80));

    } else if (hint.contains("MP-EAU") || hint.contains("Eau brute")) {
        // Icône de grande cuve / barril de liquide brut
        QRectF tank(w * 0.18, h * 0.15, w * 0.64, h * 0.72);
        p.setPen(QPen(QColor("#00E5FF"), 1.5));
        p.setBrush(QColor("#0d2745"));
        p.drawRoundedRect(tank, 6, 6);

        // Liquide
        QRectF liquid(w * 0.22, h * 0.35, w * 0.56, h * 0.48);
        p.fillRect(liquid, QColor("#1E88E5"));

        // Niveau d'eau ondulé
        p.setPen(QPen(QColor("#5AC8FA"), 1));
        p.drawLine(QPointF(w * 0.22, h * 0.35), QPointF(w * 0.78, h * 0.35));

    } else if (hint.contains("Bouchons")) {
        // Bouchons en plastique empilés / vus de haut
        p.setPen(QPen(QColor("#00E5FF"), 1.5));
        p.setBrush(QColor("#0288D1"));
        p.drawEllipse(QRectF(w * 0.15, h * 0.20, w * 0.70, h * 0.60));
        p.setBrush(QColor("#E0F7FA"));
        p.drawEllipse(QRectF(w * 0.30, h * 0.32, w * 0.40, h * 0.36));

    } else if (hint.contains("Étiquettes")) {
        // Rouleau d'étiquettes
        p.setPen(QPen(QColor("#00E5FF"), 1.5));
        p.setBrush(QColor("#E0F7FA"));
        p.drawRoundedRect(QRectF(w * 0.15, h * 0.20, w * 0.70, h * 0.55), 4, 4);
        p.setPen(QPen(QColor("#1E88E5"), 1.5));
        p.drawLine(QPointF(w * 0.25, h * 0.40), QPointF(w * 0.75, h * 0.40));
        p.drawLine(QPointF(w * 0.25, h * 0.55), QPointF(w * 0.60, h * 0.55));

    } else if (hint.contains("Carton")) {
        // Carton d'emballage 3D
        QPainterPath box;
        box.moveTo(w * 0.5, h * 0.15);
        box.lineTo(w * 0.85, h * 0.32);
        box.lineTo(w * 0.85, h * 0.72);
        box.lineTo(w * 0.5, h * 0.88);
        box.lineTo(w * 0.15, h * 0.72);
        box.lineTo(w * 0.15, h * 0.32);
        box.closeSubpath();

        p.setPen(QPen(QColor("#FFB74D"), 1.5));
        p.setBrush(QColor("#E65100"));
        p.drawPath(box);

        p.drawLine(QPointF(w * 0.5, h * 0.88), QPointF(w * 0.5, h * 0.45));
        p.drawLine(QPointF(w * 0.15, h * 0.32), QPointF(w * 0.5, h * 0.45));
        p.drawLine(QPointF(w * 0.85, h * 0.32), QPointF(w * 0.5, h * 0.45));

    } else {
        // Préformes plastique / général
        p.setPen(QPen(QColor("#80DEEA"), 1.5));
        p.setBrush(QColor(0, 229, 255, 60));
        p.drawRoundedRect(QRectF(w * 0.35, h * 0.15, w * 0.30, h * 0.70), 8, 8);
        p.setBrush(QColor("#00E5FF"));
        p.drawRect(QRectF(w * 0.30, h * 0.15, w * 0.40, h * 0.10));
    }

    p.end();
    return pix;
}

// Icône de cuve/réservoir avec niveau d'eau (utilisée dans la carte "Réserve d'eau")
static QPixmap drawTankIcon(int w, int h, int fillPercent)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF cap(w * 0.30, 0, w * 0.40, h * 0.06);
    p.setPen(QPen(QColor("#5AC8FA"), 1.5));
    p.setBrush(QColor("#0d2745"));
    p.drawRoundedRect(cap, 2, 2);

    QRectF body(w * 0.12, h * 0.08, w * 0.76, h * 0.90);
    QPainterPath bodyPath;
    bodyPath.addRoundedRect(body, 8, 8);

    p.setPen(QPen(QColor("#5AC8FA"), 2));
    p.setBrush(QColor("#08203a"));
    p.drawPath(bodyPath);

    double fillH = body.height() * (fillPercent / 100.0);
    QRectF fillRect(body.left(), body.bottom() - fillH, body.width(), fillH);
    QLinearGradient grad(fillRect.topLeft(), fillRect.bottomLeft());
    grad.setColorAt(0, QColor("#64B5F6"));
    grad.setColorAt(1, QColor("#1565C0"));

    p.save();
    p.setClipPath(bodyPath);
    p.setPen(Qt::NoPen);
    p.setBrush(grad);
    p.drawRect(fillRect);
    p.restore();

    p.setPen(QPen(QColor("#8FD3FF"), 1));
    for (int i = 1; i < 3; ++i) {
        double y = body.top() + (body.height() / 3.0) * i;
        p.drawLine(QPointF(body.left(), y), QPointF(body.right(), y));
    }

    p.setPen(QPen(QColor("#5AC8FA"), 2));
    p.setBrush(Qt::NoBrush);
    p.drawLine(QPointF(w * 0.15, h * 0.99), QPointF(w * 0.85, h * 0.99));

    p.end();
    return pix;
}

// Jauge circulaire (anneau de progression) pour le niveau d'eau en %
static QPixmap drawRingGauge(int size, int percent, const QColor &color)
{
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);

    qreal pen = size * 0.13;
    QRectF rect(pen / 2.0, pen / 2.0, size - pen, size - pen);

    p.setPen(QPen(QColor(255, 255, 255, 30), pen));
    p.drawArc(rect, 0, 360 * 16);

    p.setPen(QPen(color, pen, Qt::SolidLine, Qt::RoundCap));
    int span = int(360 * (percent / 100.0) * 16);
    p.drawArc(rect, 90 * 16, -span);

    p.end();
    return pix;
}

// Code-barres décoratif + numéro
static QPixmap drawBarcodeWithLabel(const QString &code, int w, int h)
{
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing, false);

    int barsWidth = w * 0.42;
    int barsHeight = h - 4;

    quint32 seed = 0;
    for (const QChar &c : code) seed = seed * 31 + c.unicode();
    QRandomGenerator rng(seed == 0 ? 1 : seed);

    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#E6F1FF"));
    int x = 0;
    while (x < barsWidth) {
        int barW = 1 + rng.bounded(2);
        if (rng.bounded(2) == 0)
            p.drawRect(x, 0, barW, barsHeight);
        x += barW + 1;
    }

    QFont font = p.font();
    font.setPixelSize(qMax(9, h - 6));
    p.setFont(font);
    p.setPen(QColor("#9FC4EA"));
    QRect textRect(barsWidth + 8, 0, w - barsWidth - 8, h);
    p.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, code);

    p.end();
    return pix;
}

// QR code stylisé (décoratif) affiché dans le panneau de détail
static QPixmap drawQrPlaceholder(int size)
{
    QPixmap pix(size, size);
    pix.fill(Qt::white);
    QPainter p(&pix);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);

    int cell = size / 10;
    auto drawFinder = [&](int cx, int cy) {
        p.setBrush(Qt::black);
        p.drawRect(cx, cy, cell * 3, cell * 3);
        p.setBrush(Qt::white);
        p.drawRect(cx + cell / 2, cy + cell / 2, cell * 2, cell * 2);
        p.setBrush(Qt::black);
        p.drawRect(cx + cell, cy + cell, cell, cell);
    };
    drawFinder(0, 0);
    drawFinder(size - cell * 3, 0);
    drawFinder(0, size - cell * 3);

    QRandomGenerator rng(7);
    p.setBrush(Qt::black);
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            bool inFinder = (i < 3 && j < 3) || (i > 6 && j < 3) || (i < 3 && j > 6);
            if (inFinder) continue;
            if (rng.bounded(2) == 0)
                p.drawRect(i * cell, j * cell, cell, cell);
        }
    }
    p.end();
    return pix;
}

// ------------------------------------------------------------------
// Dialog Ajouter / Modifier Produit
// ------------------------------------------------------------------
class AddEditProductDialog : public QDialog {
public:
    QLineEdit *editId, *editDesignation, *editNom, *editType, *editVolume,
              *editPrix, *editQte, *editSeuil, *editEmplacement, *editDate;
    QComboBox *comboCategory, *comboEtat;

    AddEditProductDialog(QWidget *parent, const QString &title, bool isEdit = false)
        : QDialog(parent)
    {
        setWindowTitle(title);
        setMinimumWidth(400);
        setStyleSheet("QDialog { background-color: #071325; color: #E6F1FF; }"
                      "QLabel { color: #CFE6FF; font-weight: bold; }"
                      "QLineEdit, QComboBox { background-color: #08203a; border: 1px solid #1a4b78; border-radius: 6px; padding: 6px; color: #FFFFFF; }");

        auto *layout = new QVBoxLayout(this);
        auto *form = new QFormLayout();

        editId = new QLineEdit();
        editDesignation = new QLineEdit();
        comboCategory = new QComboBox();
        comboCategory->addItems({"Produit fini", "Matière première"});
        editNom = new QLineEdit();
        editType = new QLineEdit();
        editVolume = new QLineEdit();
        editPrix = new QLineEdit();
        editQte = new QLineEdit();
        editSeuil = new QLineEdit();
        editEmplacement = new QLineEdit();
        editDate = new QLineEdit();
        comboEtat = new QComboBox();
        comboEtat->addItems({"Normal", "Faible", "Critique"});

        form->addRow("ID Produit :", editId);
        form->addRow("Désignation / Code :", editDesignation);
        form->addRow("Catégorie :", comboCategory);
        form->addRow("Nom :", editNom);
        form->addRow("Type :", editType);
        form->addRow("Volume / Unité :", editVolume);
        form->addRow("Prix (DT) :", editPrix);
        form->addRow("Quantité en stock :", editQte);
        form->addRow("Seuil minimum :", editSeuil);
        form->addRow("Emplacement :", editEmplacement);
        form->addRow("Date de réception :", editDate);
        form->addRow("État :", comboEtat);

        layout->addLayout(form);

        auto *btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        btnBox->button(QDialogButtonBox::Ok)->setText(isEdit ? "Enregistrer" : "Créer le produit");
        btnBox->button(QDialogButtonBox::Ok)->setStyleSheet("background-color: #1E88E5; color: white; font-weight: bold; border-radius: 6px; padding: 6px 14px;");
        btnBox->button(QDialogButtonBox::Cancel)->setStyleSheet("background-color: #0d2745; color: #CFE6FF; border-radius: 6px; padding: 6px 14px;");
        connect(btnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(btnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

        layout->addWidget(btnBox);
    }
};

// ------------------------------------------------------------------
// StockPage Helpers
// ------------------------------------------------------------------

QFrame* StockPage::makeCard()
{
    auto *card = new QFrame(this);
    card->setProperty("class", "Card");
    card->setFrameShape(QFrame::NoFrame);
    return card;
}

QLabel* StockPage::makeBadge(const QString &text, const QString &cssClass)
{
    auto *lbl = new QLabel(text);
    lbl->setAlignment(Qt::AlignCenter);

    QString bg = "#007AFF";
    if (cssClass.contains("Green")) bg = "#00A86B";
    else if (cssClass.contains("Orange")) bg = "#D97706";
    else if (cssClass.contains("Red")) bg = "#FF3B30";
    else if (cssClass.contains("Blue")) bg = "#007AFF";

    int radius = cssClass.contains("Sm") ? 6 : 8;
    int fontSize = cssClass.contains("Sm") ? 10 : 11;
    QString padding = cssClass.contains("Sm") ? "3px 8px" : "4px 10px";

    lbl->setStyleSheet(QString(
        "background-color: %1; color: #FFFFFF; border-radius: %2px; padding: %3; font-weight: bold; font-size: %4px;"
    ).arg(bg).arg(radius).arg(padding).arg(fontSize));

    return lbl;
}

QWidget* StockPage::makeStatCard(const QString &iconChar, const QString &iconBg,
                                  const QString &value, const QString &label,
                                  QLabel *&valueLblRef, const QString &valColor)
{
    QFrame *card = makeCard();
    auto *h = new QHBoxLayout(card);
    h->setContentsMargins(16, 14, 16, 14);
    h->setSpacing(14);

    auto *icon = new QLabel(iconChar);
    icon->setFixedSize(44, 44);
    icon->setAlignment(Qt::AlignCenter);
    icon->setStyleSheet(QString("background-color:%1; border-radius:10px; font-size:18px; color:white;").arg(iconBg));

    auto *textCol = new QVBoxLayout();
    textCol->setSpacing(2);
    valueLblRef = new QLabel(value);
    valueLblRef->setProperty("class", "CardValue");
    if (valColor != "#FFFFFF") {
        valueLblRef->setStyleSheet(QString("color: %1;").arg(valColor));
    }
    auto *labelLbl = new QLabel(label);
    labelLbl->setProperty("class", "CardLabel");
    textCol->addWidget(valueLblRef);
    textCol->addWidget(labelLbl);

    h->addWidget(icon);
    h->addLayout(textCol);
    h->addStretch();
    return card;
}

// ------------------------------------------------------------------
// Construction
// ------------------------------------------------------------------

StockPage::StockPage(QWidget *parent) : QWidget(parent)
{
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(20, 16, 20, 16);
    mainLayout->setSpacing(16);

    // -------- Colonne gauche --------
    auto *leftCol = new QVBoxLayout();
    leftCol->setContentsMargins(0, 0, 0, 0);
    leftCol->setSpacing(14);
    leftCol->addWidget(buildHeader());
    leftCol->addWidget(buildStatCards());
    leftCol->addWidget(buildToolbar());
    leftCol->addWidget(buildTable(), 1);
    leftCol->addWidget(buildPagination());

    // -------- Colonne droite --------
    auto *rightCol = new QVBoxLayout();
    rightCol->setContentsMargins(0, 0, 0, 0);
    rightCol->setSpacing(14);
    rightCol->addWidget(buildReserveCard());
    rightCol->addWidget(buildDetailPanel(), 1);

    auto *leftWidget = new QWidget(this);
    leftWidget->setLayout(leftCol);
    auto *rightWidget = new QWidget(this);
    rightWidget->setLayout(rightCol);
    rightWidget->setFixedWidth(320);

    mainLayout->addWidget(leftWidget, 1);
    mainLayout->addWidget(rightWidget);

    fillTableSampleData();
    if (table->rowCount() > 0) {
        table->selectRow(0);
        updateDetailPanel(0);
    }

    updateKpiStats();
}

QWidget* StockPage::buildHeader()
{
    QFrame *card = makeCard();
    auto *h = new QHBoxLayout(card);
    h->setContentsMargins(16, 12, 16, 12);
    h->setSpacing(12);

    auto *icon = new QLabel("📦");
    icon->setFixedSize(42, 42);
    icon->setAlignment(Qt::AlignCenter);
    icon->setStyleSheet("background-color:#1E88E5; border-radius:10px; font-size:18px;");

    auto *textCol = new QVBoxLayout();
    textCol->setSpacing(2);
    auto *title = new QLabel("Module Stock");
    title->setProperty("class", "SectionTitle");
    auto *subtitle = new QLabel("Gestion des produits et contrôle des stocks");
    subtitle->setProperty("class", "SectionSubtitle");
    textCol->addWidget(title);
    textCol->addWidget(subtitle);

    h->addWidget(icon);
    h->addLayout(textCol);
    h->addStretch();
    return card;
}

QWidget* StockPage::buildStatCards()
{
    auto *wrap = new QWidget(this);
    auto *grid = new QHBoxLayout(wrap);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(10);

    grid->addWidget(makeStatCard("📦", "#1E88E5", "12", "Total produits", lblTotalCount));
    grid->addWidget(makeStatCard("🧴", "#1E88E5", "5",  "Produit fini", lblFinishedCount));
    grid->addWidget(makeStatCard("🌿", "#2E7D32", "7",  "Matière première", lblRawCount));
    grid->addWidget(makeStatCard("⚠️", "rgba(198,40,40,0.3)", "3", "Produits en alerte", lblAlertCount, "#FF4D4D"));
    return wrap;
}

QWidget* StockPage::buildToolbar()
{
    auto *wrap = new QWidget(this);
    auto *h = new QHBoxLayout(wrap);
    h->setContentsMargins(0, 0, 0, 0);
    h->setSpacing(10);

    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("🔍  Rechercher un produit par ID, code ou nom...");
    connect(searchEdit, &QLineEdit::textChanged, this, &StockPage::applyFilter);

    categoryBox = new QComboBox();
    categoryBox->addItems({"Toutes les catégories", "Produit fini", "Matière première"});
    connect(categoryBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StockPage::applyFilter);

    typeBox = new QComboBox();
    typeBox->addItems({"Tous les types", "Bouteille", "Liquide", "Plastique", "Papier", "Carton"});
    connect(typeBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StockPage::applyFilter);

    auto *addBtn = new QPushButton("+  Nouveau produit");
    addBtn->setObjectName("PrimaryButton");
    connect(addBtn, &QPushButton::clicked, this, &StockPage::onAddProductClicked);

    h->addWidget(searchEdit, 1);
    h->addWidget(categoryBox);
    h->addWidget(typeBox);
    h->addWidget(addBtn);
    return wrap;
}

QWidget* StockPage::buildTable()
{
    table = new QTableWidget(this);
    QStringList headers = {"#", "Image", "ID Produit", "Désignation / Code", "Catégorie", "Nom",
                            "Type", "Volume/Unité", "Prix", "Qté en stock", "Seuil mini.",
                            "Emplacement", "Date réception", "État", "Actions"};
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);
    table->verticalHeader()->setVisible(false);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->setShowGrid(false);
    table->setObjectName("StockTable");
    table->verticalHeader()->setDefaultSectionSize(38);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    table->horizontalHeader()->setMinimumSectionSize(15);
    table->setTextElideMode(Qt::ElideRight);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table->setWordWrap(false);

    const int colWidths[] = {28, 42, 85, 140, 130, 90, 75, 95, 65, 90, 75, 110, 95, 70, 60};
    for (int i = 0; i < headers.size(); ++i) {
        table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
        table->setColumnWidth(i, colWidths[i]);

        QTableWidgetItem *hItem = table->horizontalHeaderItem(i);
        if (hItem) {
            if (i == 0 || i == 1 || i == 13 || i == 14) {
                hItem->setTextAlignment(Qt::AlignCenter);
            } else {
                hItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            }
        }
    }
    table->horizontalHeader()->setStretchLastSection(false);

    connect(table, &QTableWidget::cellClicked, this, &StockPage::onRowSelected);

    return table;
}

QWidget* StockPage::buildPagination()
{
    auto *wrap = new QWidget(this);
    auto *h = new QHBoxLayout(wrap);
    h->setContentsMargins(4, 0, 4, 0);

    paginationInfoLbl = new QLabel("Affichage de 1 à 8 sur 12 produits");
    paginationInfoLbl->setProperty("class", "SectionSubtitle");

    h->addWidget(paginationInfoLbl);
    h->addStretch();
    return wrap;
}

QWidget* StockPage::buildReserveCard()
{
    QFrame *card = makeCard();
    auto *v = new QVBoxLayout(card);
    v->setContentsMargins(14, 12, 14, 12);
    v->setSpacing(8);

    auto *titleRow = new QHBoxLayout();
    auto *icon = new QLabel("💧");
    auto *title = new QLabel("Réserve d'eau");
    title->setStyleSheet("font-weight:bold; font-size:13px; color:#FFFFFF;");
    titleRow->addWidget(icon);
    titleRow->addWidget(title);
    titleRow->addStretch();
    v->addLayout(titleRow);

    auto *body = new QHBoxLayout();
    body->setSpacing(10);

    auto *tankIcon = new QLabel();
    tankIcon->setPixmap(drawTankIcon(50, 90, 66));
    tankIcon->setFixedSize(50, 90);

    auto *infoCol = new QVBoxLayout();
    infoCol->setSpacing(2);
    auto *capLbl = new QLabel("Capacité totale");
    capLbl->setProperty("class", "CardLabel");
    auto *capVal = new QLabel("5 320 L");
    capVal->setStyleSheet("font-size:15px; font-weight:bold; color:#FFFFFF;");
    auto *lvlLbl = new QLabel("Niveau actuel");
    lvlLbl->setProperty("class", "CardLabel");
    auto *autoLbl = new QLabel("Autonomie estimée : 2 jours");
    autoLbl->setProperty("class", "CardLabel");
    infoCol->addWidget(capLbl);
    infoCol->addWidget(capVal);
    infoCol->addSpacing(4);
    infoCol->addWidget(lvlLbl);
    infoCol->addWidget(autoLbl);

    auto *gaugeCol = new QVBoxLayout();
    gaugeCol->setSpacing(4);
    gaugeCol->setAlignment(Qt::AlignHCenter);

    auto *gaugeStack = new QWidget();
    gaugeStack->setFixedSize(64, 64);
    auto *gaugeGrid = new QGridLayout(gaugeStack);
    gaugeGrid->setContentsMargins(0, 0, 0, 0);
    auto *gaugeBg = new QLabel();
    gaugeBg->setPixmap(drawRingGauge(64, 66, QColor("#1E88E5")));
    auto *gaugeText = new QLabel("66%");
    gaugeText->setAlignment(Qt::AlignCenter);
    gaugeText->setStyleSheet("font-weight:bold; font-size:14px; color:#FFFFFF; background:transparent;");
    gaugeGrid->addWidget(gaugeBg, 0, 0);
    gaugeGrid->addWidget(gaugeText, 0, 0, Qt::AlignCenter);

    auto *stateLbl = new QLabel("●  État : Normal");
    stateLbl->setStyleSheet("color:#4CAF50; font-weight:bold; font-size:11px;");

    gaugeCol->addWidget(gaugeStack, 0, Qt::AlignHCenter);
    gaugeCol->addWidget(stateLbl, 0, Qt::AlignHCenter);

    body->addWidget(tankIcon);
    body->addLayout(infoCol, 1);
    body->addLayout(gaugeCol);
    v->addLayout(body);

    return card;
}

QWidget* StockPage::buildDetailPanel()
{
    QFrame *card = makeCard();
    auto *v = new QVBoxLayout(card);
    v->setContentsMargins(10, 8, 10, 8);
    v->setSpacing(6);

    auto *tabs = new QTabWidget();
    tabs->setUsesScrollButtons(false);
    tabs->tabBar()->setExpanding(true);
    tabs->tabBar()->setElideMode(Qt::ElideRight);

    auto *detailsTab = new QWidget();
    auto *dv = new QVBoxLayout(detailsTab);
    dv->setContentsMargins(6, 6, 6, 6);
    dv->setSpacing(6);

    // En-tête : Image | (ID, Nom, Code-barres) | QR Code
    auto *topRow = new QHBoxLayout();
    topRow->setContentsMargins(0, 0, 0, 0);
    topRow->setSpacing(8);

    detailImage = new QLabel();
    detailImage->setFixedSize(50, 50);
    detailImage->setAlignment(Qt::AlignCenter);
    detailImage->setStyleSheet("background:#08203a; border:1px solid #16406b; border-radius:8px;");

    auto *nameCol = new QVBoxLayout();
    nameCol->setContentsMargins(0, 0, 0, 0);
    nameCol->setSpacing(2);

    detailIdTop = new QLabel("PF-001");
    detailIdTop->setStyleSheet("font-weight:bold; font-size:13px; color:#FFFFFF; border:none;");

    detailName = new QLabel("Eau minérale 1,5L");
    detailName->setStyleSheet("font-weight:600; font-size:11px; color:#5AC8FA; border:none;");

    auto *barcodeImg = new QLabel();
    barcodeImg->setPixmap(drawBarcodeWithLabel("1234567890123", 120, 14));
    barcodeImg->setFixedSize(120, 14);

    nameCol->addWidget(detailIdTop);
    nameCol->addWidget(detailName);
    nameCol->addWidget(barcodeImg);

    auto *qrImg = new QLabel();
    qrImg->setPixmap(drawQrPlaceholder(44));
    qrImg->setFixedSize(44, 44);

    topRow->addWidget(detailImage);
    topRow->addLayout(nameCol, 1);
    topRow->addWidget(qrImg);
    dv->addLayout(topRow);

    // Ligne de statut : Badge Catégorie + État
    auto *stateRow = new QHBoxLayout();
    stateRow->setContentsMargins(0, 2, 0, 2);
    stateRow->setSpacing(8);

    detailBadgeType = makeBadge("Produit fini", "BadgeBlue");
    detailBadgeType->setFixedHeight(22);

    detailState = new QLabel("●  État : Normal");
    detailState->setStyleSheet("color:#4CAF50; font-weight:bold; font-size:11px; border:none;");

    stateRow->addWidget(detailBadgeType);
    stateRow->addWidget(detailState);
    stateRow->addStretch();
    dv->addLayout(stateRow);

    // QScrollArea pour les 14 lignes d'information
    auto *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; } QWidget { background: transparent; }");

    auto *scrollContent = new QWidget();
    auto *scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setContentsMargins(0, 2, 0, 2);
    scrollLayout->setSpacing(4);

    auto addRow = [&](const QString &label, QLabel *&valueLbl) {
        auto *row = new QHBoxLayout();
        row->setContentsMargins(0, 0, 0, 0);
        row->setSpacing(6);

        auto *l = new QLabel(label);
        l->setProperty("class", "CardLabel");
        l->setFixedWidth(130);
        l->setStyleSheet("color:#9FC4EA; font-size:11px;");

        valueLbl = new QLabel();
        valueLbl->setStyleSheet("color:#E6F1FF; font-weight:500; font-size:11px;");
        valueLbl->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        row->addWidget(l);
        row->addWidget(valueLbl, 1);
        scrollLayout->addLayout(row);
    };

    addRow("ID Produit :", lblId);
    addRow("Désignation / Code :", lblDesignation);
    addRow("Catégorie :", lblCategorie);
    addRow("Nom :", lblNom);
    addRow("Type :", lblType);
    addRow("Volume / Unité :", lblVolume);
    addRow("Prix :", lblPrix);
    addRow("Quantité en stock :", lblQte);
    addRow("Seuil minimum :", lblSeuil);
    addRow("Emplacement :", lblEmplacement);
    addRow("Date de fabrication / réception :", lblDateReception);
    addRow("Date de mise à jour :", lblDateMaj);
    addRow("Fournisseur :", lblFournisseur);
    addRow("Responsable Stock :", lblResponsable);

    scrollArea->setWidget(scrollContent);
    dv->addWidget(scrollArea, 1);

    // Boutons du bas (Modifier & Supprimer) toujours fixes en bas
    auto *btnRow = new QHBoxLayout();
    btnRow->setContentsMargins(0, 4, 0, 0);
    btnRow->setSpacing(8);

    auto *editBtn = new QPushButton("✏   Modifier");
    editBtn->setFixedHeight(34);
    editBtn->setStyleSheet("QPushButton { background-color: #007AFF; color: #FFFFFF; border: none; border-radius: 8px; font-weight: bold; font-size: 12.5px; } QPushButton:hover { background-color: #3395FF; }");
    connect(editBtn, &QPushButton::clicked, this, &StockPage::onEditProductClicked);

    auto *delBtn = new QPushButton("🗑   Supprimer");
    delBtn->setFixedHeight(34);
    delBtn->setStyleSheet("QPushButton { background-color: #EA3943; color: #FFFFFF; border: none; border-radius: 8px; font-weight: bold; font-size: 12.5px; } QPushButton:hover { background-color: #FF5B65; }");
    connect(delBtn, &QPushButton::clicked, this, &StockPage::onDeleteProductClicked);

    btnRow->addWidget(editBtn);
    btnRow->addWidget(delBtn);
    dv->addLayout(btnRow);

    auto makeInfoTab = [](const QString &msg) {
        auto *w = new QWidget();
        auto *l = new QVBoxLayout(w);
        auto *lbl = new QLabel(msg);
        lbl->setProperty("class", "CardLabel");
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setWordWrap(true);
        l->addStretch();
        l->addWidget(lbl);
        l->addStretch();
        return w;
    };

    tabs->addTab(detailsTab, "Détails");
    tabs->addTab(makeInfoTab("Historique des mouvements de ce produit\n(entrées / sorties de stock)."), "Historique");
    tabs->addTab(makeInfoTab("Prévision de rupture basée sur\nla consommation moyenne."), "Prévisions");

    v->addWidget(tabs);
    return card;
}

// ------------------------------------------------------------------
// Table population & row creation
// ------------------------------------------------------------------

void StockPage::createTableRow(int r, const QString &id, const QString &designation,
                               const QString &categorie, const QString &nom,
                               const QString &type, const QString &volume,
                               const QString &prix, const QString &qte,
                               const QString &seuil, const QString &emplacement,
                               const QString &date, const QString &etat,
                               const QString &iconHint)
{
    auto makeItem = [](const QString &text, Qt::Alignment align = Qt::AlignLeft | Qt::AlignVCenter) {
        auto *item = new QTableWidgetItem(text);
        item->setTextAlignment(align);
        return item;
    };

    // Col 0: #
    table->setItem(r, 0, makeItem(QString::number(r + 1), Qt::AlignCenter));

    // Col 1: Vignette graphique de produit
    auto *imgLbl = new QLabel();
    imgLbl->setPixmap(drawProductIcon(iconHint, 28, 28));
    imgLbl->setAlignment(Qt::AlignCenter);
    table->setCellWidget(r, 1, imgLbl);

    // Col 2: ID Produit
    table->setItem(r, 2, makeItem(id));

    // Col 3: Désignation / Code
    table->setItem(r, 3, makeItem(designation));

    // Col 4: Catégorie (Badge aligné à gauche sous l'en-tête Catégorie)
    QString catClass = (categorie == "Produit fini") ? "BadgeBlueSm" : "BadgeGreenSm";
    auto *catWrap = new QWidget();
    auto *catLayout = new QHBoxLayout(catWrap);
    catLayout->setContentsMargins(2, 1, 2, 1);
    catLayout->setSpacing(0);
    catLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    catLayout->addWidget(makeBadge(categorie, catClass));
    table->setCellWidget(r, 4, catWrap);

    // Col 5: Nom
    table->setItem(r, 5, makeItem(nom));

    // Col 6: Type
    table->setItem(r, 6, makeItem(type));

    // Col 7: Volume/Unité
    table->setItem(r, 7, makeItem(volume));

    // Col 8: Prix
    table->setItem(r, 8, makeItem(prix));

    // Col 9: Qté en stock
    table->setItem(r, 9, makeItem(qte));

    // Col 10: Seuil mini.
    table->setItem(r, 10, makeItem(seuil));

    // Col 11: Emplacement
    table->setItem(r, 11, makeItem(emplacement));

    // Col 12: Date réception
    table->setItem(r, 12, makeItem(date));

    // Col 13: État (Badge centré sous l'en-tête État)
    QString etatClass = "BadgeGreenSm";
    if (etat == "Faible") etatClass = "BadgeOrangeSm";
    else if (etat == "Critique") etatClass = "BadgeRedSm";

    auto *etatWrap = new QWidget();
    auto *etatLayout = new QHBoxLayout(etatWrap);
    etatLayout->setContentsMargins(2, 1, 2, 1);
    etatLayout->setSpacing(0);
    etatLayout->setAlignment(Qt::AlignCenter);
    etatLayout->addWidget(makeBadge(etat, etatClass));
    table->setCellWidget(r, 13, etatWrap);

    // Col 14: Actions (Boutons centrés sous l'en-tête Actions)
    auto *actionsWrap = new QWidget();
    auto *ah = new QHBoxLayout(actionsWrap);
    ah->setContentsMargins(1, 1, 1, 1);
    ah->setSpacing(4);
    ah->setAlignment(Qt::AlignCenter);

    auto *editBtn = new QPushButton("✏");
    editBtn->setStyleSheet("QPushButton { background-color: #007AFF; color: #FFFFFF; border: none; border-radius: 13px; font-weight: bold; font-size: 13px; min-width: 26px; max-width: 26px; min-height: 26px; max-height: 26px; } QPushButton:hover { background-color: #3395FF; }");
    connect(editBtn, &QPushButton::clicked, this, [this, r]() {
        table->selectRow(r);
        onEditProductClicked();
    });

    auto *delBtn = new QPushButton("🗑");
    delBtn->setStyleSheet("QPushButton { background-color: #FF3B30; color: #FFFFFF; border: none; border-radius: 13px; font-weight: bold; font-size: 13px; min-width: 26px; max-width: 26px; min-height: 26px; max-height: 26px; } QPushButton:hover { background-color: #FF6259; }");
    connect(delBtn, &QPushButton::clicked, this, [this, r]() {
        table->selectRow(r);
        onDeleteProductClicked();
    });

    ah->addWidget(editBtn);
    ah->addWidget(delBtn);
    table->setCellWidget(r, 14, actionsWrap);
}

void StockPage::fillTableSampleData()
{
    struct Row {
        QString id, designation, categorie, nom, type, volume, prix, qte, seuil,
                emplacement, date, etat, icon;
    };

    QVector<Row> rows = {
        {"PF-001",  "Eau minérale 1,5L", "Produit fini",      "Eau minérale", "Bouteille", "1,5 L", "0,45 DT", "5 000",  "1 000",  "Zone A - Réservoir 1", "12/03/2026", "Normal",   "PF-001"},
        {"MP-EAU-001", "Eau brute",      "Matière première",  "Eau brute",    "Liquide",   "-",      "0,12 DT", "48 200", "10 000", "Réservoir 1",          "10/03/2026", "Normal",   "MP-EAU"},
        {"MP-002",  "Bouchons",          "Matière première",  "Bouchons",     "Plastique", "-",      "0,03 DT", "2 500",  "5 000",  "Zone B",               "15/03/2026", "Faible",   "Bouchons"},
        {"MP-003",  "Étiquettes",        "Matière première",  "Étiquettes",   "Papier",    "-",      "0,02 DT", "2 400",  "3 000",  "Zone C",               "18/03/2026", "Normal",   "Étiquettes"},
        {"MP-004",  "Cartons",           "Matière première",  "Carton",       "Carton",    "-",      "0,18 DT", "1 800",  "2 000",  "Zone D",               "20/03/2026", "Normal",   "Carton"},
        {"PF-002",  "Eau gazeuse 1,5L",  "Produit fini",      "Eau gazeuse",  "Bouteille", "1,5 L",  "0,55 DT", "980",    "1 000",  "Zone A",               "21/03/2026", "Faible",   "PF-002"},
        {"PF-003",  "Bouteille 0,5L",    "Produit fini",      "Eau plate",    "Bouteille", "0,5 L",  "0,35 DT", "3 200",  "1 000",  "Zone B",               "22/03/2026", "Normal",   "PF-003"},
        {"MP-005",  "Préformes",         "Matière première",  "Préformes",    "Plastique", "-",      "0,20 DT", "1 200",  "1 000",  "Zone C",               "23/03/2026", "Critique", "Préformes"},
    };

    table->setRowCount(rows.size());
    for (int r = 0; r < rows.size(); ++r) {
        const Row &d = rows[r];
        createTableRow(r, d.id, d.designation, d.categorie, d.nom, d.type,
                       d.volume, d.prix, d.qte, d.seuil, d.emplacement,
                       d.date, d.etat, d.icon);
    }
}

void StockPage::onRowSelected(int row, int /*column*/)
{
    updateDetailPanel(row);
}

void StockPage::updateDetailPanel(int row)
{
    if (row < 0 || row >= table->rowCount()) return;

    auto textAt = [&](int col) -> QString {
        if (auto *item = table->item(row, col)) return item->text();
        return QString();
    };

    QString designation = textAt(3);
    QString id = textAt(2);
    QString type = textAt(6);

    detailName->setText(designation);
    detailIdTop->setText(id);
    detailImage->setPixmap(drawProductIcon(designation.isEmpty() ? id : designation, 48, 48));

    lblId->setText(id);
    lblDesignation->setText(designation);
    lblNom->setText(textAt(5));
    lblType->setText(type);
    lblVolume->setText(textAt(7));
    lblPrix->setText(textAt(8));
    lblQte->setText(textAt(9));
    lblSeuil->setText(textAt(10));
    lblEmplacement->setText(textAt(11));
    lblDateReception->setText(textAt(12));
    lblDateMaj->setText("24/09/2026");
    lblFournisseur->setText("SOTUDIS");
    lblResponsable->setText("Ahmed");

    if (auto *badge = qobject_cast<QLabel*>(table->cellWidget(row, 4))) {
        lblCategorie->setText(badge->text());
        detailBadgeType->setText(badge->text());
        QString catClass = (badge->text() == "Produit fini") ? "BadgeBlue" : "BadgeGreen";
        detailBadgeType->setProperty("class", catClass);
        detailBadgeType->style()->unpolish(detailBadgeType);
        detailBadgeType->style()->polish(detailBadgeType);
    }

    if (auto *etatBadge = qobject_cast<QLabel*>(table->cellWidget(row, 13))) {
        QString etat = etatBadge->text();
        QString color = "#059669";
        if (etat == "Faible") color = "#D97706";
        else if (etat == "Critique") color = "#DC2626";
        detailState->setText(QString("●  État : %1").arg(etat));
        detailState->setStyleSheet(QString("color:%1; font-weight:bold; font-size:11px; border:none;").arg(color));
    }
}

void StockPage::updateKpiStats()
{
    int total = table->rowCount();
    int finished = 0;
    int raw = 0;
    int alert = 0;

    for (int r = 0; r < table->rowCount(); ++r) {
        if (auto *catBadge = qobject_cast<QLabel*>(table->cellWidget(r, 4))) {
            if (catBadge->text() == "Produit fini") finished++;
            else if (catBadge->text() == "Matière première") raw++;
        }
        if (auto *etatBadge = qobject_cast<QLabel*>(table->cellWidget(r, 13))) {
            if (etatBadge->text() == "Faible" || etatBadge->text() == "Critique") alert++;
        }
    }

    if (lblTotalCount) lblTotalCount->setText(QString::number(total));
    if (lblFinishedCount) lblFinishedCount->setText(QString::number(finished));
    if (lblRawCount) lblRawCount->setText(QString::number(raw));
    if (lblAlertCount) lblAlertCount->setText(QString::number(alert));
}

void StockPage::applyFilter()
{
    QString search = searchEdit->text().trimmed().toLower();
    QString selCat = categoryBox->currentText();
    QString selType = typeBox->currentText();

    int visibleCount = 0;
    for (int r = 0; r < table->rowCount(); ++r) {
        bool matchSearch = search.isEmpty() ||
                           (table->item(r, 2) && table->item(r, 2)->text().toLower().contains(search)) ||
                           (table->item(r, 3) && table->item(r, 3)->text().toLower().contains(search)) ||
                           (table->item(r, 5) && table->item(r, 5)->text().toLower().contains(search));

        bool matchCat = (selCat == "Toutes les catégories");
        if (!matchCat) {
            if (auto *b = qobject_cast<QLabel*>(table->cellWidget(r, 4))) {
                matchCat = (b->text() == selCat);
            }
        }

        bool matchType = (selType == "Tous les types");
        if (!matchType) {
            if (auto *item = table->item(r, 6)) {
                matchType = (item->text().toLower() == selType.toLower());
            }
        }

        bool show = matchSearch && matchCat && matchType;
        table->setRowHidden(r, !show);
        if (show) visibleCount++;
    }

    paginationInfoLbl->setText(QString("Affichage de 1 à %1 sur %2 produits").arg(visibleCount).arg(table->rowCount()));
}

void StockPage::onAddProductClicked()
{
    AddEditProductDialog dlg(this, "Nouveau produit", false);
    if (dlg.exec() == QDialog::Accepted) {
        int r = table->rowCount();
        table->insertRow(r);
        createTableRow(r,
                       dlg.editId->text().isEmpty() ? QString("PF-%1").arg(r+1, 3, 10, QChar('0')) : dlg.editId->text(),
                       dlg.editDesignation->text().isEmpty() ? "Nouveau produit" : dlg.editDesignation->text(),
                       dlg.comboCategory->currentText(),
                       dlg.editNom->text().isEmpty() ? "Produit" : dlg.editNom->text(),
                       dlg.editType->text().isEmpty() ? "Bouteille" : dlg.editType->text(),
                       dlg.editVolume->text().isEmpty() ? "1,5 L" : dlg.editVolume->text(),
                       dlg.editPrix->text().isEmpty() ? "0,50 DT" : dlg.editPrix->text(),
                       dlg.editQte->text().isEmpty() ? "1 000" : dlg.editQte->text(),
                       dlg.editSeuil->text().isEmpty() ? "500" : dlg.editSeuil->text(),
                       dlg.editEmplacement->text().isEmpty() ? "Zone A" : dlg.editEmplacement->text(),
                       dlg.editDate->text(),
                       dlg.comboEtat->currentText(),
                       dlg.editDesignation->text());
        updateKpiStats();
        table->selectRow(r);
        updateDetailPanel(r);
    }
}

void StockPage::onEditProductClicked()
{
    int row = table->currentRow();
    if (row < 0 || row >= table->rowCount()) return;

    AddEditProductDialog dlg(this, "Modifier le produit", true);

    auto textAt = [&](int col) -> QString {
        if (auto *item = table->item(row, col)) return item->text();
        return QString();
    };

    dlg.editId->setText(textAt(2));
    dlg.editDesignation->setText(textAt(3));
    if (auto *b = qobject_cast<QLabel*>(table->cellWidget(row, 4))) {
        dlg.comboCategory->setCurrentText(b->text());
    }
    dlg.editNom->setText(textAt(5));
    dlg.editType->setText(textAt(6));
    dlg.editVolume->setText(textAt(7));
    dlg.editPrix->setText(textAt(8));
    dlg.editQte->setText(textAt(9));
    dlg.editSeuil->setText(textAt(10));
    dlg.editEmplacement->setText(textAt(11));
    dlg.editDate->setText(textAt(12));
    if (auto *eb = qobject_cast<QLabel*>(table->cellWidget(row, 13))) {
        dlg.comboEtat->setCurrentText(eb->text());
    }

    if (dlg.exec() == QDialog::Accepted) {
        createTableRow(row,
                       dlg.editId->text(),
                       dlg.editDesignation->text(),
                       dlg.comboCategory->currentText(),
                       dlg.editNom->text(),
                       dlg.editType->text(),
                       dlg.editVolume->text(),
                       dlg.editPrix->text(),
                       dlg.editQte->text(),
                       dlg.editSeuil->text(),
                       dlg.editEmplacement->text(),
                       dlg.editDate->text(),
                       dlg.comboEtat->currentText(),
                       dlg.editDesignation->text());
        updateKpiStats();
        updateDetailPanel(row);
    }
}

void StockPage::onDeleteProductClicked()
{
    int row = table->currentRow();
    if (row < 0 || row >= table->rowCount()) return;

    QString pName = table->item(row, 3) ? table->item(row, 3)->text() : "ce produit";
    auto reply = QMessageBox::question(this, "Confirmation de suppression",
                                       QString("Voulez-vous vraiment supprimer %1 ?").arg(pName),
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        table->removeRow(row);
        updateKpiStats();
        if (table->rowCount() > 0) {
            int nextRow = qMin(row, table->rowCount() - 1);
            table->selectRow(nextRow);
            updateDetailPanel(nextRow);
        }
    }
}

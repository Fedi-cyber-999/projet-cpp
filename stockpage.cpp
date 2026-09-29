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
#include <QFileDialog>
#include <QImageReader>
#include <QFileInfo>
#include <QSpinBox>
#include <QPushButton>

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
// Image produit : contrainte de taille + score d'efficacité
// ------------------------------------------------------------------

// Taille EXACTE (en pixels) exigée pour l'image d'un produit.
// = taille des vignettes déjà affichées dans la colonne "Image" du tableau (28 x 28),
// afin que l'image s'intègre parfaitement sans être déformée.
static const int kProductImageSize = 28;

// Vérifie : fichier PNG réel + dimensions exactes kProductImageSize x kProductImageSize
static bool validateProductImage(const QString &path, QString &error)
{
    if (!QFileInfo::exists(path)) {
        error = "Le fichier est introuvable.";
        return false;
    }
    if (QFileInfo(path).suffix().compare("png", Qt::CaseInsensitive) != 0) {
        error = "L'image doit être au format PNG (.png).";
        return false;
    }
    QImageReader reader(path);
    if (reader.format().toLower() != "png") {          // vérifie le contenu, pas seulement l'extension
        error = "Le fichier n'est pas une véritable image PNG.";
        return false;
    }
    const QSize sz = reader.size();
    if (sz != QSize(kProductImageSize, kProductImageSize)) {
        error = QString("Taille incorrecte : %1 x %2 px.\nL'image doit faire exactement %3 x %3 px "
                        "(taille des images du tableau).")
                    .arg(sz.width()).arg(sz.height()).arg(kProductImageSize);
        return false;
    }
    return true;
}

// Score global = moyenne des 3 critères (chacun noté de 0 à 10)
static double computeScore(int eco, int prod, int cond)
{
    return (eco + prod + cond) / 3.0;
}

static QString scoreText(double v)
{
    return QString::number(v, 'f', 1).replace('.', ',') + " / 10";
}

static QString scoreColor(double v)
{
    if (v >= 7.0) return "#00A86B";   // bon
    if (v >= 4.0) return "#D97706";   // moyen
    return "#FF3B30";                  // faible
}

// Convertit "48 200" / "5 000" / "1,5" en nombre (espaces normaux et insécables ignorés)
static double parseNumber(QString t)
{
    t.remove(QChar(' ')).remove(QChar(0x00A0)).remove(QChar(0x202F));
    t.replace(',', '.');
    bool ok = false;
    double v = t.toDouble(&ok);
    return ok ? v : 0.0;
}

static QString formatNumber(double v)
{
    return QLocale(QLocale::French).toString(qint64(v + 0.5)).replace(QChar(0x00A0), ' ').replace(QChar(0x202F), ' ');
}

// ------------------------------------------------------------------
// Dialog Ajouter / Modifier Produit
// ------------------------------------------------------------------
class AddEditProductDialog : public QDialog {
public:
    QString imagePath;                       // image PNG choisie ("" = image par défaut)
    QLabel *imagePreview = nullptr;
    QSpinBox *spinEco = nullptr, *spinProd = nullptr, *spinCond = nullptr;
    QLabel *scorePreview = nullptr;
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

        // --- Image du produit (PNG, taille imposée) ---
        imagePreview = new QLabel("Aucune");
        imagePreview->setFixedSize(kProductImageSize + 12, kProductImageSize + 12);
        imagePreview->setAlignment(Qt::AlignCenter);
        imagePreview->setStyleSheet("background:#08203a; border:1px solid #1a4b78; border-radius:6px; font-size:9px; font-weight:normal;");
        auto *chooseBtn = new QPushButton(isEdit ? "Changer l'image…" : "Choisir une image…");
        chooseBtn->setStyleSheet("background-color: #0d2745; color: #CFE6FF; border: 1px solid #1E88E5; border-radius: 6px; padding: 6px 12px;");
        auto *imgHint = new QLabel(QString("PNG uniquement, %1 x %1 px exactement").arg(kProductImageSize));
        imgHint->setStyleSheet("color:#7FA6CC; font-size:10px; font-weight:normal;");
        auto *imgBox = new QHBoxLayout();
        imgBox->addWidget(imagePreview);
        auto *imgCol = new QVBoxLayout();
        imgCol->addWidget(chooseBtn, 0, Qt::AlignLeft);
        imgCol->addWidget(imgHint);
        imgBox->addLayout(imgCol, 1);
        form->addRow("Image :", imgBox);
        connect(chooseBtn, &QPushButton::clicked, this, [this]() { chooseImage(); });

        // --- Score d'efficacité : 3 critères notés de 0 à 10 ---
        auto makeSpin = [this]() {
            auto *sp = new QSpinBox();
            sp->setRange(0, 10);
            sp->setValue(5);
            sp->setSuffix(" / 10");
            sp->setStyleSheet("background-color: #08203a; border: 1px solid #1a4b78; border-radius: 6px; padding: 6px; color: #FFFFFF;");
            connect(sp, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int) { updateScorePreview(); });
            return sp;
        };
        spinEco  = makeSpin();
        spinProd = makeSpin();
        spinCond = makeSpin();
        scorePreview = new QLabel();
        scorePreview->setStyleSheet("font-size:13px;");
        form->addRow("Emballage écologique :", spinEco);
        form->addRow("Efficacité de production :", spinProd);
        form->addRow("Optimisation du conditionnement :", spinCond);
        form->addRow("Score global :", scorePreview);
        updateScorePreview();

        layout->addLayout(form);

        auto *btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        btnBox->button(QDialogButtonBox::Ok)->setText(isEdit ? "Enregistrer" : "Créer le produit");
        btnBox->button(QDialogButtonBox::Ok)->setStyleSheet("background-color: #1E88E5; color: white; font-weight: bold; border-radius: 6px; padding: 6px 14px;");
        btnBox->button(QDialogButtonBox::Cancel)->setStyleSheet("background-color: #0d2745; color: #CFE6FF; border-radius: 6px; padding: 6px 14px;");
        connect(btnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(btnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

        layout->addWidget(btnBox);
    }

    void setImage(const QString &path)
    {
        imagePath = path;
        if (path.isEmpty()) {
            imagePreview->setPixmap(QPixmap());
            imagePreview->setText("Aucune");
        } else {
            imagePreview->setPixmap(QPixmap(path));
        }
    }

    void setScores(int eco, int prod, int cond)
    {
        spinEco->setValue(eco);
        spinProd->setValue(prod);
        spinCond->setValue(cond);
    }

private:
    void chooseImage()
    {
        const QString file = QFileDialog::getOpenFileName(this, "Choisir l'image du produit",
                                                          QString(), "Images PNG (*.png)");
        if (file.isEmpty()) return;

        QString error;
        if (!validateProductImage(file, error)) {
            QMessageBox::warning(this, "Image refusée", error);
            return;                                   // on garde l'image précédente
        }
        setImage(file);
    }

    void updateScorePreview()
    {
        const double v = computeScore(spinEco->value(), spinProd->value(), spinCond->value());
        scorePreview->setText(QString("<b style='color:%1'>%2</b>").arg(scoreColor(v), scoreText(v)));
    }
};

// ------------------------------------------------------------------
// Graphique des quantités en stock (barres) pour la fenêtre "Statistiques"
// ------------------------------------------------------------------
class StockBarChart : public QWidget {
public:
    struct Entry { QString label; double qty; double seuil; };

    explicit StockBarChart(const QVector<Entry> &e, QWidget *parent = nullptr)
        : QWidget(parent), entries(e)
    {
        setMinimumHeight(entries.size() * rowH + 16);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        const int labelW = 190, valueW = 170;
        const int trackX = labelW + 8;
        const int trackW = qMax(60, width() - trackX - valueW - 8);

        QFont f = p.font();
        f.setPixelSize(12);
        p.setFont(f);

        for (int i = 0; i < entries.size(); ++i) {
            const Entry &e = entries[i];
            const int y = 8 + i * rowH;
            const QRectF track(trackX, y + 5, trackW, rowH - 12);

            p.setPen(QColor("#CFE6FF"));
            p.drawText(QRect(0, y, labelW, rowH - 2), Qt::AlignVCenter | Qt::AlignLeft,
                       p.fontMetrics().elidedText(e.label, Qt::ElideRight, labelW));

            p.setPen(Qt::NoPen);
            p.setBrush(QColor(255, 255, 255, 25));
            p.drawRoundedRect(track, 5, 5);

            // La piste représente 2 x le seuil : le repère blanc au milieu = seuil minimum
            double ratio = e.seuil > 0 ? e.qty / (2.0 * e.seuil) : (e.qty > 0 ? 1.0 : 0.0);
            ratio = qBound(0.0, ratio, 1.0);
            QColor c("#00A86B");
            if (e.seuil > 0 && e.qty < e.seuil) c = QColor("#FF3B30");
            else if (e.seuil > 0 && e.qty < e.seuil * 1.25) c = QColor("#D97706");
            p.setBrush(c);
            p.drawRoundedRect(QRectF(track.left(), track.top(), track.width() * ratio, track.height()), 5, 5);

            p.setPen(QPen(Qt::white, 2));
            const qreal mx = track.left() + track.width() / 2.0;
            p.drawLine(QPointF(mx, track.top() - 2), QPointF(mx, track.bottom() + 2));

            p.setPen(QColor("#E6F1FF"));
            p.drawText(QRect(trackX + trackW + 10, y, valueW, rowH - 2), Qt::AlignVCenter | Qt::AlignLeft,
                       QString("%1  (seuil %2)").arg(formatNumber(e.qty), formatNumber(e.seuil)));
        }
    }

private:
    QVector<Entry> entries;
    const int rowH = 30;
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
    searchEdit->setPlaceholderText("🔍  Rechercher un produit par ID...");
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

    detailScoreBadge = new QLabel("Score : -");
    detailScoreBadge->setAlignment(Qt::AlignCenter);
    detailScoreBadge->setFixedHeight(22);

    stateRow->addWidget(detailBadgeType);
    stateRow->addWidget(detailState);
    stateRow->addStretch();
    stateRow->addWidget(detailScoreBadge);
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
        l->setFixedWidth(160);
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

    // --- Score d'efficacité du produit ---
    auto *scoreTitle = new QLabel("Efficacité");
    scoreTitle->setStyleSheet("color:#5AC8FA; font-weight:bold; font-size:12px; margin-top:6px;");
    scrollLayout->addWidget(scoreTitle);
    addRow("Emballage écologique :", lblEco);
    addRow("Efficacité de production :", lblEffProd);
    addRow("Optimisation conditionnement :", lblCond);
    addRow("Score global :", lblScore);

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
                               const QString &iconHint,
                               const QString &imagePath,
                               int scoreEco, int scoreProd, int scoreCond)
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
    QPixmap thumb;
    if (!imagePath.isEmpty() && thumb.load(imagePath)) {
        imgLbl->setPixmap(thumb);                                   // image PNG choisie (28 x 28)
    } else {
        imgLbl->setPixmap(drawProductIcon(iconHint, kProductImageSize, kProductImageSize));
    }
    imgLbl->setAlignment(Qt::AlignCenter);
    table->setCellWidget(r, 1, imgLbl);

    // Col 2: ID Produit
    {
        auto *idItem = makeItem(id);
        // Métadonnées du produit : image personnalisée + 3 critères du score
        QVariantMap meta;
        meta["image"] = imagePath;
        meta["eco"]   = scoreEco;
        meta["prod"]  = scoreProd;
        meta["cond"]  = scoreCond;
        idItem->setData(Qt::UserRole, meta);
        table->setItem(r, 2, idItem);
    }

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
    // On retrouve la ligne au moment du clic (l'index r devient faux après une suppression)
    auto rowOf = [this, actionsWrap]() {
        for (int i = 0; i < table->rowCount(); ++i)
            if (table->cellWidget(i, 14) == actionsWrap) return i;
        return -1;
    };
    connect(editBtn, &QPushButton::clicked, this, [this, rowOf]() {
        int row = rowOf();
        if (row < 0) return;
        table->selectRow(row);
        onEditProductClicked();
    });

    auto *delBtn = new QPushButton("🗑");
    delBtn->setStyleSheet("QPushButton { background-color: #FF3B30; color: #FFFFFF; border: none; border-radius: 13px; font-weight: bold; font-size: 13px; min-width: 26px; max-width: 26px; min-height: 26px; max-height: 26px; } QPushButton:hover { background-color: #FF6259; }");
    connect(delBtn, &QPushButton::clicked, this, [this, rowOf]() {
        int row = rowOf();
        if (row < 0) return;
        table->selectRow(row);
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
        int eco, prod, cond;    // critères du score (0 à 10)
    };

    QVector<Row> rows = {
        {"PF-001",  "Eau minérale 1,5L", "Produit fini",      "Eau minérale", "Bouteille", "1,5 L", "0,45 DT", "5 000",  "1 000",  "Zone A - Réservoir 1", "12/03/2026", "Normal",   "PF-001", 8, 9, 8},
        {"MP-EAU-001", "Eau brute",      "Matière première",  "Eau brute",    "Liquide",   "-",      "0,12 DT", "48 200", "10 000", "Réservoir 1",          "10/03/2026", "Normal",   "MP-EAU", 6, 8, 7},
        {"MP-002",  "Bouchons",          "Matière première",  "Bouchons",     "Plastique", "-",      "0,03 DT", "2 500",  "5 000",  "Zone B",               "15/03/2026", "Faible",   "Bouchons", 4, 7, 6},
        {"MP-003",  "Étiquettes",        "Matière première",  "Étiquettes",   "Papier",    "-",      "0,02 DT", "2 400",  "3 000",  "Zone C",               "18/03/2026", "Normal",   "Étiquettes", 7, 7, 8},
        {"MP-004",  "Cartons",           "Matière première",  "Carton",       "Carton",    "-",      "0,18 DT", "1 800",  "2 000",  "Zone D",               "20/03/2026", "Normal",   "Carton", 9, 6, 7},
        {"PF-002",  "Eau gazeuse 1,5L",  "Produit fini",      "Eau gazeuse",  "Bouteille", "1,5 L",  "0,55 DT", "980",    "1 000",  "Zone A",               "21/03/2026", "Faible",   "PF-002", 7, 8, 9},
        {"PF-003",  "Bouteille 0,5L",    "Produit fini",      "Eau plate",    "Bouteille", "0,5 L",  "0,35 DT", "3 200",  "1 000",  "Zone B",               "22/03/2026", "Normal",   "PF-003", 8, 9, 9},
        {"MP-005",  "Préformes",         "Matière première",  "Préformes",    "Plastique", "-",      "0,20 DT", "1 200",  "1 000",  "Zone C",               "23/03/2026", "Critique", "Préformes", 3, 5, 4},
    };

    table->setRowCount(rows.size());
    for (int r = 0; r < rows.size(); ++r) {
        const Row &d = rows[r];
        createTableRow(r, d.id, d.designation, d.categorie, d.nom, d.type,
                       d.volume, d.prix, d.qte, d.seuil, d.emplacement,
                       d.date, d.etat, d.icon, QString(), d.eco, d.prod, d.cond);
    }
}

// Les colonnes "Catégorie" et "État" contiennent un widget conteneur : on cherche le badge dedans
QString StockPage::badgeText(int row, int col) const
{
    if (auto *w = table->cellWidget(row, col)) {
        if (auto *lbl = qobject_cast<QLabel*>(w)) return lbl->text();
        if (auto *lbl = w->findChild<QLabel*>()) return lbl->text();
    }
    return QString();
}

QVariantMap StockPage::rowMeta(int row) const
{
    if (auto *item = table->item(row, 2)) return item->data(Qt::UserRole).toMap();
    return QVariantMap();
}

void StockPage::renumberRows()
{
    for (int r = 0; r < table->rowCount(); ++r)
        if (auto *it = table->item(r, 0)) it->setText(QString::number(r + 1));
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
    const QVariantMap meta = rowMeta(row);
    QPixmap customPix;
    const QString imgPath = meta.value("image").toString();
    if (!imgPath.isEmpty() && customPix.load(imgPath)) {
        detailImage->setPixmap(customPix.scaled(46, 46, Qt::KeepAspectRatio, Qt::FastTransformation));
    } else {
        detailImage->setPixmap(drawProductIcon(designation.isEmpty() ? id : designation, 48, 48));
    }

    // Score d'efficacité
    const int eco = meta.value("eco", 5).toInt();
    const int prod = meta.value("prod", 5).toInt();
    const int cond = meta.value("cond", 5).toInt();
    const double score = computeScore(eco, prod, cond);
    lblEco->setText(QString("%1 / 10").arg(eco));
    lblEffProd->setText(QString("%1 / 10").arg(prod));
    lblCond->setText(QString("%1 / 10").arg(cond));
    lblScore->setText(scoreText(score));
    lblScore->setStyleSheet(QString("color:%1; font-weight:bold; font-size:11px;").arg(scoreColor(score)));
    detailScoreBadge->setText("Score : " + scoreText(score));
    detailScoreBadge->setStyleSheet(QString("background-color:%1; color:#FFFFFF; border-radius:8px; padding:3px 8px; font-weight:bold; font-size:10px;")
                                        .arg(scoreColor(score)));

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

    {
        const QString cat = badgeText(row, 4);
        lblCategorie->setText(cat);
        detailBadgeType->setText(cat);
        const QString bg = (cat == "Produit fini") ? "#007AFF" : "#00A86B";
        detailBadgeType->setStyleSheet(QString("background-color:%1; color:#FFFFFF; border-radius:8px; padding:4px 10px; font-weight:bold; font-size:11px;").arg(bg));
    }

    {
        QString etat = badgeText(row, 13);
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
        const QString cat = badgeText(r, 4);
        if (cat == "Produit fini") finished++;
        else if (cat == "Matière première") raw++;
        const QString etat = badgeText(r, 13);
        if (etat == "Faible" || etat == "Critique") alert++;
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
        // Recherche uniquement sur l'ID produit (colonne 2)
        bool matchSearch = search.isEmpty() ||
                           (table->item(r, 2) && table->item(r, 2)->text().toLower().contains(search));

        bool matchCat = (selCat == "Toutes les catégories") || (badgeText(r, 4) == selCat);

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
                       dlg.editDesignation->text(),
                       dlg.imagePath,
                       dlg.spinEco->value(), dlg.spinProd->value(), dlg.spinCond->value());
        updateKpiStats();
        applyFilter();
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
    dlg.comboCategory->setCurrentText(badgeText(row, 4));
    dlg.editNom->setText(textAt(5));
    dlg.editType->setText(textAt(6));
    dlg.editVolume->setText(textAt(7));
    dlg.editPrix->setText(textAt(8));
    dlg.editQte->setText(textAt(9));
    dlg.editSeuil->setText(textAt(10));
    dlg.editEmplacement->setText(textAt(11));
    dlg.editDate->setText(textAt(12));
    dlg.comboEtat->setCurrentText(badgeText(row, 13));

    const QVariantMap oldMeta = rowMeta(row);
    dlg.setImage(oldMeta.value("image").toString());
    dlg.setScores(oldMeta.value("eco", 5).toInt(), oldMeta.value("prod", 5).toInt(),
                  oldMeta.value("cond", 5).toInt());

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
                       dlg.editDesignation->text(),
                       dlg.imagePath,                 // nouvelle image, ou l'ancienne si inchangée
                       dlg.spinEco->value(), dlg.spinProd->value(), dlg.spinCond->value());
        renumberRows();
        updateKpiStats();
        applyFilter();
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
        renumberRows();
        updateKpiStats();
        applyFilter();
        if (table->rowCount() > 0) {
            int nextRow = qMin(row, table->rowCount() - 1);
            table->selectRow(nextRow);
            updateDetailPanel(nextRow);
        }
    }
}

// ------------------------------------------------------------------
// Statistiques : quantités de produits en stock
// ------------------------------------------------------------------
void StockPage::showStockStatistics()
{
    QVector<StockBarChart::Entry> entries;
    double qtyFinished = 0, qtyRaw = 0;
    int underThreshold = 0;

    for (int r = 0; r < table->rowCount(); ++r) {
        auto textAt = [&](int col) { return table->item(r, col) ? table->item(r, col)->text() : QString(); };
        const double qty = parseNumber(textAt(9));
        const double seuil = parseNumber(textAt(10));

        if (badgeText(r, 4) == "Produit fini") qtyFinished += qty;
        else qtyRaw += qty;
        if (qty < seuil) underThreshold++;

        entries.append({ QString("%1 - %2").arg(textAt(2), textAt(5)), qty, seuil });
    }

    QDialog dlg(this);
    dlg.setWindowTitle("Statistiques - Quantités en stock");
    dlg.resize(820, 560);
    dlg.setStyleSheet("QDialog { background-color: #071325; color: #E6F1FF; } QLabel { color: #CFE6FF; }"
                      "QScrollArea { border: none; background: transparent; }");

    auto *v = new QVBoxLayout(&dlg);
    auto *title = new QLabel("📊  Quantité de produits en stock");
    title->setStyleSheet("font-size:17px; font-weight:bold; color:#FFFFFF;");
    v->addWidget(title);

    auto makeKpi = [](const QString &value, const QString &label, const QString &color) {
        auto *box = new QFrame();
        box->setStyleSheet("QFrame { background-color:#0d2745; border:1px solid #123a5e; border-radius:10px; }");
        auto *l = new QVBoxLayout(box);
        auto *val = new QLabel(value);
        val->setStyleSheet(QString("font-size:20px; font-weight:bold; color:%1; border:none;").arg(color));
        auto *lab = new QLabel(label);
        lab->setStyleSheet("font-size:11px; color:#9FC4EA; border:none;");
        l->addWidget(val);
        l->addWidget(lab);
        return box;
    };
    auto *kpis = new QHBoxLayout();
    kpis->addWidget(makeKpi(QString::number(table->rowCount()), "Produits référencés", "#FFFFFF"));
    kpis->addWidget(makeKpi(formatNumber(qtyFinished), "Qté totale - Produits finis", "#5AC8FA"));
    kpis->addWidget(makeKpi(formatNumber(qtyRaw), "Qté totale - Matières premières", "#4CAF50"));
    kpis->addWidget(makeKpi(QString::number(underThreshold), "Produits sous le seuil", underThreshold ? "#FF4D4D" : "#4CAF50"));
    v->addLayout(kpis);

    auto *legend = new QLabel("Chaque barre compare la quantité au seuil minimum (repère blanc). "
                              "<span style='color:#00A86B'>■</span> suffisant &nbsp; "
                              "<span style='color:#D97706'>■</span> proche du seuil &nbsp; "
                              "<span style='color:#FF3B30'>■</span> sous le seuil");
    legend->setWordWrap(true);
    legend->setStyleSheet("font-size:11px; color:#9FC4EA;");
    v->addWidget(legend);

    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setWidget(new StockBarChart(entries));
    v->addWidget(scroll, 1);

    auto *closeBtn = new QPushButton("Fermer");
    closeBtn->setStyleSheet("background-color: #1E88E5; color: white; font-weight: bold; border-radius: 6px; padding: 8px 20px;");
    connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    v->addWidget(closeBtn, 0, Qt::AlignRight);

    dlg.exec();
}

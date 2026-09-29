#ifndef STOCKPAGE_H
#define STOCKPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QFrame>
#include <QVariantMap>

// Page complète du module "Stocks" : cartes de synthèse, réserve d'eau (Arduino),
// tableau des produits et panneau de détail du produit sélectionné.
class StockPage : public QWidget
{
    Q_OBJECT
public:
    explicit StockPage(QWidget *parent = nullptr);

public slots:
    // Fenêtre de statistiques sur les quantités en stock (bouton "Statistiques")
    void showStockStatistics();

private slots:
    void onRowSelected(int row, int column);
    void applyFilter();
    void onAddProductClicked();
    void onEditProductClicked();
    void onDeleteProductClicked();

private:
    QWidget* buildHeader();
    QWidget* buildStatCards();
    QWidget* buildToolbar();
    QWidget* buildTable();
    QWidget* buildPagination();
    QWidget* buildReserveCard();
    QWidget* buildDetailPanel();

    QFrame*  makeCard();
    QLabel*  makeBadge(const QString &text, const QString &cssClass);
    QWidget* makeStatCard(const QString &iconChar, const QString &iconBg,
                           const QString &value, const QString &label,
                           QLabel *&valueLblRef,
                           const QString &valColor = "#FFFFFF");

    void fillTableSampleData();
    void updateDetailPanel(int row);
    void updateKpiStats();
    void createTableRow(int r, const QString &id, const QString &designation,
                        const QString &categorie, const QString &nom,
                        const QString &type, const QString &volume,
                        const QString &prix, const QString &qte,
                        const QString &seuil, const QString &emplacement,
                        const QString &date, const QString &etat,
                        const QString &iconHint,
                        const QString &imagePath = QString(),
                        int scoreEco = 5, int scoreProd = 5, int scoreCond = 5);
    QString badgeText(int row, int col) const;   // texte du badge dans une cellule
    QVariantMap rowMeta(int row) const;          // image + scores du produit
    void renumberRows();

    QTableWidget *table;
    QLineEdit    *searchEdit;
    QComboBox    *categoryBox;
    QComboBox    *typeBox;
    QLabel       *paginationInfoLbl;

    // KPI value labels for dynamic updates
    QLabel *lblTotalCount;
    QLabel *lblFinishedCount;
    QLabel *lblRawCount;
    QLabel *lblAlertCount;

    // Panneau de détail (mis à jour selon la ligne sélectionnée)
    QLabel *detailImage;
    QLabel *detailIdTop;
    QLabel *detailName;
    QLabel *detailBadgeType;
    QLabel *detailState;
    QLabel *detailScoreBadge;
    QLabel *lblId, *lblDesignation, *lblCategorie, *lblNom, *lblType,
           *lblVolume, *lblPrix, *lblQte, *lblSeuil, *lblEmplacement,
           *lblDateReception, *lblDateMaj, *lblFournisseur, *lblResponsable,
           *lblEco, *lblEffProd, *lblCond, *lblScore;
};

#endif // STOCKPAGE_H

#ifndef PLACEHOLDERPAGE_H
#define PLACEHOLDERPAGE_H

#include <QWidget>

// Page temporaire affichée pour les modules pas encore détaillés
// (Accueil, Employé, Véhicule, Livraison, Client & Commande, Fournisseur & Commande).
// A remplacer plus tard par une classe dédiée, sur le même modèle que StockPage.
class PlaceholderPage : public QWidget
{
    Q_OBJECT
public:
    explicit PlaceholderPage(const QString &title,
                              const QString &subtitle,
                              QWidget *parent = nullptr);
};

#endif // PLACEHOLDERPAGE_H

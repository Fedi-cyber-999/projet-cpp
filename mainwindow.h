#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QPushButton>

namespace Ui {
class MainWindow;
}

// Aucune logique métier ici, à une exception près : la sidebar permet
// de basculer entre les pages (QStackedWidget). Toutes les pages autres
// que "Stock" restent volontairement vides. Aucune autre action
// (recherche, boutons, tableau...) n'est connectée.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onNavButtonClicked();

private:
    void setupNavigation();

    Ui::MainWindow *ui;
    QPixmap backgroundPixmap;
    QList<QPushButton*> navButtons;
};

#endif // MAINWINDOW_H

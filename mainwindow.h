#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QVector>
#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>

class StockPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onNavClicked();
    void handleLogout();

private:
    void setupSidebar(QWidget *parent);
    void setupTopBar(QWidget *parent);
    void setupPages();
    QPushButton* addNavButton(QVBoxLayout *layout, const QString &text,
                               const QString &iconChar, int pageIndex);

    QPixmap backgroundPixmap;

    QWidget        *sidebar;
    QStackedWidget *stackedWidget;
    QPushButton    *statsBtn  = nullptr;
    QPushButton    *reportBtn = nullptr;
    StockPage      *stockPage = nullptr;
    QVector<QPushButton*> sidebarButtons;
};

#endif // MAINWINDOW_H

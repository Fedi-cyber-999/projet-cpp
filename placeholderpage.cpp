#include "placeholderpage.h"
#include <QVBoxLayout>
#include <QLabel>

PlaceholderPage::PlaceholderPage(const QString &title, const QString &subtitle, QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    auto *titleLbl = new QLabel(title, this);
    titleLbl->setStyleSheet("font-size:26px; font-weight:bold; color:#FFFFFF;");
    titleLbl->setAlignment(Qt::AlignCenter);

    auto *subLbl = new QLabel(subtitle, this);
    subLbl->setStyleSheet("font-size:14px; color:#7FA8D0;");
    subLbl->setAlignment(Qt::AlignCenter);

    layout->addWidget(titleLbl);
    layout->addWidget(subLbl);
}

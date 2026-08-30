#include "docktitlebar.h"
#include <QDockWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>

DockTitleBar::DockTitleBar(bool minimalSize, QWidget *parent) :
    QWidget(parent),
    mMinimalSize(minimalSize)
{
    setCursor(Qt::SizeAllCursor);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 0, 6, 0);
    layout->setSpacing(6);

    mIconLabel = new QLabel(this);
    layout->addWidget(mIconLabel);

    mTitleLabel = new QLabel(this);
    mTitleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mTitleLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    layout->addWidget(mTitleLabel);

    layout->addStretch();

    mDockWidget = qobject_cast<QDockWidget*>(parentWidget());
    if (mDockWidget)
    {
        mTitleLabel->setText(mDockWidget->windowTitle());
        mIconLabel->setPixmap(mDockWidget->windowIcon().pixmap(14, 14));

        if (mDockWidget->features().testFlag(QDockWidget::DockWidgetFloatable))
        {
            QToolButton* pinButton = new QToolButton(this);
            pinButton->setAutoRaise(true);
            pinButton->setIcon(QIcon::fromTheme("floating"));
            pinButton->setIconSize(QSize(8, 8));
            layout->addWidget(pinButton);
            connect(pinButton, &QToolButton::clicked, this, [this](){mDockWidget->setFloating(!mDockWidget->isFloating());});
        }

        if (mDockWidget->features().testFlag(QDockWidget::DockWidgetClosable))
        {
            QToolButton* closeButton = new QToolButton(this);
            closeButton->setAutoRaise(true);
            closeButton->setIcon(QIcon::fromTheme("close"));
            closeButton->setIconSize(QSize(8, 8));
            layout->addWidget(closeButton);
            connect(closeButton, &QToolButton::clicked, mDockWidget, &QDockWidget::close);
        }
        connect(mDockWidget, &QDockWidget::dockLocationChanged, this,  &DockTitleBar::updateLayout);
        updateLayout();
    }
}

DockTitleBar::DockTitleBar(QWidget *parent) :
    DockTitleBar(0, parent)
{
}

void DockTitleBar::updateLayout()
{
    const Qt::DockWidgetArea location = mDockWidget->dockLocation();
    const bool floating = location == Qt::DockWidgetArea::NoDockWidgetArea;
    mIconLabel->setVisible(floating);
    mTitleLabel->setVisible(floating);

    switch (location)
    {
    case Qt::DockWidgetArea::TopDockWidgetArea:
        setContentsMargins(0, 0, 0, 0);
        mDockWidget->widget()->setContentsMargins(6, 0, 6, 6);
        break;
    case Qt::DockWidgetArea::BottomDockWidgetArea:
        setContentsMargins(0, 0, 0, 0);
        mDockWidget->widget()->setContentsMargins(6, 0, 12, 12);
        break;
    case Qt::DockWidgetArea::LeftDockWidgetArea:
        setContentsMargins(0, 0, 0, 0);
        mDockWidget->widget()->setContentsMargins(6, 0, 0, 12);
        break;
    case Qt::DockWidgetArea::RightDockWidgetArea:
        setContentsMargins(0, 0, 0, 0);
        mDockWidget->widget()->setContentsMargins(0, 0, 6, 12);
        break;
    default:
        setContentsMargins(0, 6, 0, 6);
        mDockWidget->widget()->setContentsMargins(6, 0, 6, 12);
        break;
    }

    if (mMinimalSize)
    {
        switch (location)
        {
        case Qt::DockWidgetArea::TopDockWidgetArea:
        case Qt::DockWidgetArea::BottomDockWidgetArea:
            mDockWidget->setFixedHeight(mDockWidget->widget()->minimumSizeHint().height() + height());
            mDockWidget->setMinimumWidth(0);
            mDockWidget->setMaximumWidth(QWIDGETSIZE_MAX);
            break;
        case Qt::DockWidgetArea::LeftDockWidgetArea:
        case Qt::DockWidgetArea::RightDockWidgetArea:
            mDockWidget->setFixedWidth(mDockWidget->widget()->minimumSizeHint().width());
            mDockWidget->setMinimumHeight(0);
            mDockWidget->setMaximumHeight(QWIDGETSIZE_MAX);
            break;
        default:
            mDockWidget->setMinimumWidth(0);
            mDockWidget->setMaximumWidth(QWIDGETSIZE_MAX);
            mDockWidget->setMinimumHeight(0);
            mDockWidget->setMaximumHeight(QWIDGETSIZE_MAX);
            break;
        }
    }
}
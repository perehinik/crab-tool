#include "InfoPopup.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QDialogButtonBox>

static QWidget * createSpacer(QWidget *parent, int height) {
    QWidget *spacer = new QWidget(parent);
    spacer->setFixedHeight(height);
    return spacer;
}

class CrabLabel : public QWidget {
public:
    explicit CrabLabel(QWidget* parent = nullptr)
        : QWidget(parent),
        crabPixmap(":/icon/crab-big.png"),
        crabOffset(-100)
    {
        layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        leftSpacer = new QWidget(this);
        layout->addWidget(leftSpacer);
        layout->setAlignment(Qt::AlignHCenter);

        imageLabel = new QLabel(this);
        imageLabel->setPixmap(crabPixmap.scaled(crabSize, crabSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        layout->addWidget(imageLabel);
    }

protected:
    void mousePressEvent(QMouseEvent* event) override {
        const int maxOffset = parentWidget()->width() - crabSize - (crabStep * 3);

        if (crabOffset < -40) {
            layout->setAlignment(Qt::AlignLeft);
            crabOffset = (parentWidget()->width() - crabSize) / 2;
        } else {
            crabOffset += (crabStep * direction);
        }
        leftSpacer->setFixedWidth(crabOffset);

        if (crabOffset <= crabStep || crabOffset >= maxOffset) {
            direction *= -1;
        }

        QWidget::mousePressEvent(event);
    }

private:
    QHBoxLayout* layout;
    QLabel* imageLabel;
    QPixmap crabPixmap;
    QWidget* leftSpacer;
    int crabOffset;
    int direction = 1;
    const int crabSize = 150;
    const int crabStep = 10;
};

InfoPopup::InfoPopup(QWidget* parent) : QDialog(parent) {
    setWindowTitle("About");
    setFixedWidth(300);
    setWindowFlags(windowFlags() | Qt::Popup);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* titleLabel = new QLabel("Crab Tool", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(R"(
        QLabel {
            font-size: 20pt;
            font-weight: bold;
            color: #b22222;
        }
    )");

    QLabel* versionLabel = new QLabel(APP_VERSION, this);
    versionLabel->setAlignment(Qt::AlignCenter);
    versionLabel->setStyleSheet(R"(
        QLabel {
            font-size: 10pt;
            font-weight: bold;
            color: grey;
        }
    )");

    CrabLabel* crabWidget = new CrabLabel(this);

    QLabel* authorLabel1 = new QLabel("Author", this);
    authorLabel1->setAlignment(Qt::AlignCenter);
    authorLabel1->setStyleSheet(R"(
        QLabel {
            font-size: 10pt;
            color: grey;
        }
    )");

    QLabel* authorLabel2 = new QLabel("Ivan Perehiniak", this);
    authorLabel2->setAlignment(Qt::AlignCenter);
    authorLabel2->setStyleSheet(R"(
        QLabel {
            font-size: 16pt;
            font-weight: bold;
            color: #191970;
        }
    )");

    QLabel* linkLabel = new QLabel(this);
    linkLabel->setText(R"(<a href="https://github.com/perehinik/crab-tool">github.com/perehinik/crab-tool</a>)");
    linkLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    linkLabel->setOpenExternalLinks(true);
    linkLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(titleLabel);
    layout->addWidget(versionLabel);
    layout->addWidget(createSpacer(this, 10));
    layout->addWidget(crabWidget);
    layout->addWidget(createSpacer(this, 10));
    layout->addWidget(authorLabel1);
    layout->addWidget(authorLabel2);
    layout->addWidget(createSpacer(this, 20));
    layout->addWidget(linkLabel);

    layout->setSpacing(0);
}

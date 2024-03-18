#ifndef G2PINFOWIDGET_H
#define G2PINFOWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

#include <QGroupBox>

namespace LangMgr {

    class G2pInfoWidget final : public QWidget {
        Q_OBJECT
    public:
        explicit G2pInfoWidget(QWidget *parent = nullptr);
        ~G2pInfoWidget() override;

    public Q_SLOTS:
        void setInfo(const QString &language, const QString &g2pId) const;

    private:
        void removeWidget() const;

        QVBoxLayout *m_mainLayout;
        QVBoxLayout *m_topLayout;

        QLabel *m_langueLabel;
        QLabel *m_authorLabel;

        QGroupBox *m_descriptionGroupBox;
        QVBoxLayout *m_descriptionLayout;
        QLabel *m_descriptionLabel;
    };

} // LangMgr

#endif // G2PINFOWIDGET_H

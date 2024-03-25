#include "Cantonese.h"

#include <QCheckBox>
#include <QVBoxLayout>

namespace G2pMgr {
    Cantonese::Cantonese(QObject *parent) : IG2pFactory("Cantonese", parent) {
        setAuthor(tr("Xiao Lang"));
        setDisplayName(tr("Cantonese"));
        setDescription(tr("Using Cantonese Pinyin as the phonetic notation method."));
    }

    Cantonese::~Cantonese() = default;

    bool Cantonese::initialize(QString &errMsg) {
        m_cantonese = new IKg2p::Cantonese();
        if (m_cantonese->getDefaultPinyin("好").isEmpty()) {
            errMsg = tr("Failed to initialize Cantonese G2P");
            return false;
        }
        return true;
    }

    QList<LangNote> Cantonese::convert(const QStringList &input, const QJsonObject *config) const {
        const auto tone =
            config && config->keys().contains("tone") ? config->value("tone").toBool() : this->tone;
        const auto convertNum = config && config->keys().contains("convertNum")
                                    ? config->value("convertNum").toBool()
                                    : this->convertNum;

        QList<LangNote> result;
        auto g2pRes = m_cantonese->hanziToPinyin(input, tone, convertNum);
        for (int i = 0; i < g2pRes.size(); i++) {
            LangNote langNote;
            langNote.lyric = input[i];
            langNote.syllable = g2pRes[i];
            langNote.candidates = m_cantonese->getDefaultPinyin(input[i], false);
            if (input[i] == g2pRes[i])
                langNote.g2pError = true;
            result.append(langNote);
        }
        return result;
    }

    QJsonObject Cantonese::config() {
        QJsonObject config;
        config["tone"] = tone;
        config["convertNum"] = convertNum;
        return config;
    }

    QWidget *Cantonese::configWidget(QJsonObject *config) {
        auto *widget = new QWidget();
        auto *layout = new QVBoxLayout(widget);

        auto *toneCheckBox = new QCheckBox(tr("Tone"), widget);
        auto *convertNumCheckBox = new QCheckBox(tr("Convert number"), widget);

        layout->addWidget(toneCheckBox);
        layout->addWidget(convertNumCheckBox);
        layout->addStretch(1);

        if (config && config->keys().contains("tone")) {
            toneCheckBox->setChecked(config->value("tone").toBool());
            convertNumCheckBox->setChecked(config->value("convertNum").toBool());
        } else {
            toneCheckBox->setChecked(tone);
            convertNumCheckBox->setChecked(convertNum);
        }

        connect(toneCheckBox, &QCheckBox::toggled, [this, config](const bool checked) {
            config->insert("tone", checked);
            Q_EMIT g2pConfigChanged();
        });
        connect(convertNumCheckBox, &QCheckBox::toggled, [this, config](const bool checked) {
            config->insert("convertNum", checked);
            Q_EMIT g2pConfigChanged();
        });
        return widget;
    }
} // G2pMgr
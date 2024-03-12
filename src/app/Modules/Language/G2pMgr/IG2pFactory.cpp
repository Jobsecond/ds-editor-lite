#include "IG2pFactory.h"
#include "IG2pFactory_p.h"

namespace G2pMgr {

    IG2pFactoryPrivate::IG2pFactoryPrivate() {
    }

    IG2pFactoryPrivate::~IG2pFactoryPrivate() = default;

    void IG2pFactoryPrivate::init() {
    }

    IG2pFactory::IG2pFactory(const QString &id, QObject *parent)
        : IG2pFactory(*new IG2pFactoryPrivate(), id, parent) {
    }

    IG2pFactory::~IG2pFactory() = default;

    IG2pFactory::IG2pFactory(IG2pFactoryPrivate &d, const QString &id, QObject *parent)
        : QObject(parent), d_ptr(&d) {
        d.q_ptr = this;
        d.id = id;

        d.init();
    }

    QString IG2pFactory::id() const {
        Q_D(const IG2pFactory);
        return d->id;
    }

    QString IG2pFactory::description() const {
        Q_D(const IG2pFactory);
        return d->description;
    }

    void IG2pFactory::setDescription(const QString &description) {
        Q_D(IG2pFactory);
        d->description = description;
    }

    QString IG2pFactory::displayName() const {
        Q_D(const IG2pFactory);
        return d->displayName;
    }

    void IG2pFactory::setDisplayName(const QString &displayName) {
        Q_D(IG2pFactory);
        d->displayName = displayName;
    }

    QString IG2pFactory::category() const {
        Q_D(const IG2pFactory);
        return d->category;
    }

    void IG2pFactory::setCategory(const QString &category) {
        Q_D(IG2pFactory);
        d->category = category;
    }

    Phonic IG2pFactory::convert(const QString &input) const {
        return convert(QStringList() << input).at(0);
    }

    Phonic IG2pFactory::convert(const Note *&input) const {
        return convert(input->lyric());
    }

    QList<Phonic> IG2pFactory::convert(const QList<Note *> &input) const {
        QStringList inputText;
        for (const auto note : input) {
            inputText.append(note->lyric());
        }
        return convert(inputText);
    }

    QList<Phonic> IG2pFactory::convert(QStringList &input) const {
        Q_UNUSED(input);
        return {};
    }
}

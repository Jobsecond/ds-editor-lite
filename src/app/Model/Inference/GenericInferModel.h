//
// Created by fluty on 24-9-16.
//

#ifndef GENERICINFERMODEL_H
#define GENERICINFERMODEL_H

#include "Interface/ISerializable.h"

class InferPhoneme final : public ISerializable {
public:
    QString token;
    QString language;
    double start = 0; // s

    [[nodiscard]] QJsonObject serialize() const override;
    bool deserialize(const QJsonObject &obj) override;
};

class InferNote final : public ISerializable {
public:
    int key = 0;
    int cents = 0;
    double duration = 0; // s
    QString glide;
    bool is_rest = false;

    [[nodiscard]] QJsonObject serialize() const override;
    bool deserialize(const QJsonObject &obj) override;
};

class InferWord final : public ISerializable {
public:
    QList<InferPhoneme> phones;
    QList<InferNote> notes;

    [[nodiscard]] QJsonObject serialize() const override;
    bool deserialize(const QJsonObject &obj) override;
};

class InferRetake final : public ISerializable {
public:
    // index, [start, end)
    int start = 0;
    int end = 0;

    [[nodiscard]] QJsonObject serialize() const override;
    bool deserialize(const QJsonObject &obj) override;
};

class InferParam final : public ISerializable {
public:
    QString tag;
    bool dynamic = false;
    double interval = 0.01; // s
    QList<double> values;

    [[nodiscard]] QJsonObject serialize() const override;
    bool deserialize(const QJsonObject &obj) override;
};

class GenericInferModel final : public ISerializable {
public:
    double offset = 0;
    QList<InferWord> words;
    QList<InferParam> params;

    [[nodiscard]] QJsonObject serialize() const override;
    bool deserialize(const QJsonObject &obj) override;
};



#endif // GENERICINFERMODEL_H

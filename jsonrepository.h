#ifndef JSONREPOSITORY_H
#define JSONREPOSITORY_H

#include <vector>
#include <memory>
#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "ArtRepositoryInterface.h"
#include "ArtObject.h"
#include "Painting.h"
#include "Sculpture.h"
#include "DigitalArt.h"

class JsonRepository : public ArtRepositoryInterface {
public:
    using ArtPtr = std::shared_ptr<ArtObject>;

    JsonRepository() noexcept = default;
    ~JsonRepository() override = default;

    // ── In-memory CRUD ──
    void add(const ArtPtr& art) override;
    bool update(std::size_t index, const ArtPtr& art) noexcept override;
    bool remove(std::size_t index) noexcept override;
    ArtPtr get(std::size_t index) const noexcept override;
    std::size_t size() const noexcept override;
    void clear() noexcept override;

    // ── Persistence ──
    bool loadFromFile(const QString& filePath) override;
    bool saveToFile(const QString& filePath) const override;

private:
    std::vector<ArtPtr> items_;
};

#endif // JSONREPOSITORY_H

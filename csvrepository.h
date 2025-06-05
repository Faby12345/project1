#ifndef CSVREPOSITORY_H
#define CSVREPOSITORY_H

#include <vector>
#include <memory>
#include <QString>
#include <QFile>
#include <QTextStream>

#include "ArtRepositoryInterface.h"
#include "ArtObject.h"
#include "Painting.h"
#include "Sculpture.h"
#include "DigitalArt.h"

class CsvRepository : public ArtRepositoryInterface {
public:
    using ArtPtr = std::shared_ptr<ArtObject>;

    CsvRepository() noexcept = default;
    ~CsvRepository() override = default;

    // ── In‐memory CRUD ──
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

    // Helper to parse a CSV line into fields, handling quoted commas
    QStringList parseCsvLine(const QString& line) const;
};

#endif // CSVREPOSITORY_H

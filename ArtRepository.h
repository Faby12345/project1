#ifndef ARTREPOSITORY_H
#define ARTREPOSITORY_H

#include <vector>
#include <memory>
#include <QString>

// “In‐memory” art object repository
#include "ArtObject.h"
#include "ArtRepositoryInterface.h"

class ArtRepository : public ArtRepositoryInterface {
public:
    using ArtPtr = std::shared_ptr<ArtObject>;

    ArtRepository() noexcept = default;
    ~ArtRepository() override = default;

    // ── In‐memory CRUD ──
    void add(const ArtPtr& art) override;
    bool update(std::size_t index, const ArtPtr& art) noexcept override;
    bool remove(std::size_t index) noexcept override;
    ArtPtr get(std::size_t index) const noexcept override;
    std::size_t size() const noexcept override;
    void clear() noexcept override;

    // ── Persistence (stubs) ──
    bool loadFromFile(const QString& /*filePath*/) override {
        return false;  // in‐memory repo does not persist
    }
    bool saveToFile(const QString& /*filePath*/) const override {
        return false;  // in‐memory repo does not persist
    }

private:
    std::vector<ArtPtr> items_;
};

#endif // ARTREPOSITORY_H

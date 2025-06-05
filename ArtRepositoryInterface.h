#ifndef ARTREPOSITORYINTERFACE_H
#define ARTREPOSITORYINTERFACE_H

#include <vector>
#include <memory>
#include <QString>


class ArtObject;

class ArtRepositoryInterface {
public:
    using ArtPtr = std::shared_ptr<ArtObject>;

    virtual ~ArtRepositoryInterface() = default;

    // ── In-memory CRUD ──
    virtual void add(const ArtPtr& art) = 0;
    virtual bool update(std::size_t index, const ArtPtr& art) noexcept = 0;
    virtual bool remove(std::size_t index) noexcept = 0;
    virtual ArtPtr get(std::size_t index) const noexcept = 0;
    virtual std::size_t size() const noexcept = 0;
    virtual void clear() noexcept = 0;

    // ── Persistence ──
    // Load all art objects from the given file. Return true on success.
    virtual bool loadFromFile(const QString& filePath) = 0;
    // Save current in-memory objects to the given file. Return true on success.
    virtual bool saveToFile(const QString& filePath) const = 0;
};

#endif // ARTREPOSITORYINTERFACE_H

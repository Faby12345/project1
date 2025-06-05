#include "ArtRepository.h"

// ── In‐memory CRUD ──
void ArtRepository::add(const ArtPtr& art) {
    items_.push_back(art);
}

bool ArtRepository::update(std::size_t index, const ArtPtr& art) noexcept {
    if (index >= items_.size()) return false;
    items_[index] = art;
    return true;
}

bool ArtRepository::remove(std::size_t index) noexcept {
    if (index >= items_.size()) return false;
    items_.erase(items_.begin() + index);
    return true;
}

ArtRepository::ArtPtr ArtRepository::get(std::size_t index) const noexcept {
    if (index >= items_.size()) return nullptr;
    return items_[index];
}

std::size_t ArtRepository::size() const noexcept {
    return items_.size();
}

void ArtRepository::clear() noexcept {
    items_.clear();
}

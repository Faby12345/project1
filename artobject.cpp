#include "ArtObject.h"


ArtObject::ArtObject() noexcept
    : name_{}, description_{}, price_{0.0}, location_{},  imagePath_{} {}


ArtObject::ArtObject(const std::string& name,
                     const std::string& description,
                     double price,
                     const std::string& location,
                     const QString& imagePath) noexcept
    : name_{name},
    description_{description},
    price_{price},
    location_{location},
    imagePath_{imagePath}
{
}

const std::string& ArtObject::getName() const noexcept {
    return name_;
}

const std::string& ArtObject::getDescription() const noexcept {
    return description_;
}

double ArtObject::getPrice() const noexcept {
    return price_;
}

const std::string& ArtObject::getLocation() const noexcept {
    return location_;
}
const QString& ArtObject::getImagePath() const noexcept {
    return imagePath_;
}

void ArtObject::setName(const std::string& name) {
    name_ = name;
}

void ArtObject::setDescription(const std::string& description) {
    description_ = description;
}

void ArtObject::setPrice(double price) {
    price_ = price;
}

void ArtObject::setLocation(const std::string& location) {
    location_ = location;
}
void ArtObject::setImagePath(const QString& imagePath) {
    imagePath_ = imagePath;
}

std::string ArtObject::getType() const noexcept {
    return "ArtObject";
}

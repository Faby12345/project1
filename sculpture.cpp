#include "Sculpture.h"

Sculpture::Sculpture() noexcept
    : ArtObject{},
    material_{}
{}


Sculpture::Sculpture(const std::string& name,
                     const std::string& description,
                     double price,
                     const std::string& location,
                     const std::string& material,
                     const QString& imagePath ) noexcept
    : ArtObject{name, description, price, location, imagePath},
    material_{material}
{}


const std::string& Sculpture::getMaterial() const noexcept {
    return material_;
}

void Sculpture::setMaterial(const std::string& material) {
    material_ = material;
}

std::string Sculpture::getType() const noexcept {
    return "Sculpture";
}

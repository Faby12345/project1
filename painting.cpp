#include "Painting.h"

Painting::Painting() noexcept
    : ArtObject{},
    canvasType_{}
{}

Painting::Painting(const std::string& name,
                   const std::string& description,
                   double price,
                   const std::string& location,
                   const std::string& canvasType,
                    const QString& imagePath) noexcept
    : ArtObject{name, description, price, location, imagePath},
    canvasType_{canvasType}
{}


const std::string& Painting::getCanvasType() const noexcept {
    return canvasType_;
}

void Painting::setCanvasType(const std::string& canvasType) {
    canvasType_ = canvasType;
}

std::string Painting::getType() const noexcept {
    return "Painting";
}

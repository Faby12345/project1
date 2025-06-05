#include "DigitalArt.h"

DigitalArt::DigitalArt() noexcept
    : ArtObject{},
    software_{"Unknown"},
    resolutionX_{0},
    resolutionY_{0}
{}


DigitalArt::DigitalArt(const std::string& name,
                       const std::string& description,
                       double price,
                       const std::string& location,
                       const std::string& software,
                       int resolutionX,
                       int resolutionY,
                       const QString& imagePath) noexcept
    : ArtObject{name, description, price, location, imagePath},
    software_{software},
    resolutionX_{resolutionX},
    resolutionY_{resolutionY}
{}


const std::string& DigitalArt::getSoftware() const noexcept {
    return software_;
}

int DigitalArt::getResolutionX() const noexcept {
    return resolutionX_;
}

int DigitalArt::getResolutionY() const noexcept {
    return resolutionY_;
}

void DigitalArt::setSoftware(const std::string& software) {
    software_ = software;
}

void DigitalArt::setResolution(int resolutionX, int resolutionY) {
    resolutionX_ = resolutionX;
    resolutionY_ = resolutionY;
}

std::string DigitalArt::getType() const noexcept {
    return "DigitalArt";
}

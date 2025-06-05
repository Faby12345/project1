#ifndef SCULPTURE_H
#define SCULPTURE_H

#include "ArtObject.h"
#include <string>
#include<QString>

class Sculpture : public ArtObject {
public:
    Sculpture() noexcept;
    Sculpture(const std::string& name,
              const std::string& description,
              double price,
              const std::string& location,
              const std::string& material,
              const QString& imagePath = QString()) noexcept;

    const std::string& getMaterial() const noexcept;
    std::string getType() const noexcept override;

    void setMaterial(const std::string& material);

private:
    std::string material_;
};

#endif // SCULPTURE_H

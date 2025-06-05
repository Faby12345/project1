// Painting.h
#ifndef PAINTING_H
#define PAINTING_H

#include "ArtObject.h"
#include <string>
#include<QString>

class Painting : public ArtObject {
public:
    Painting() noexcept;
    Painting(const std::string& name,
             const std::string& description,
             double price,
             const std::string& location,
             const std::string& canvasType,
              const QString& imagePath = QString()) noexcept;

    const std::string& getCanvasType() const noexcept;
    std::string getType() const noexcept override;

    void setCanvasType(const std::string& canvasType);

private:
    std::string canvasType_;
};

#endif // PAINTING_H

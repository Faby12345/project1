// DigitalArt.h
#ifndef DIGITALART_H
#define DIGITALART_H


#include <string>
#include <QString>

#include "ArtObject.h"

class DigitalArt : public ArtObject {
public:
    DigitalArt() noexcept;
    DigitalArt(const std::string& name,
               const std::string& description,
               double price,
               const std::string& location,
               const std::string& software,
               int resolutionX,
               int resolutionY,
               const QString& imagePath = QString()) noexcept;

    const std::string& getSoftware() const noexcept;
    int getResolutionX() const noexcept;
    int getResolutionY() const noexcept;

    void setSoftware(const std::string& software);
    void setResolution(int resolutionX, int resolutionY);

    std::string getType() const noexcept override;

private:
    std::string software_;
    int         resolutionX_;
    int         resolutionY_;
};

#endif // DIGITALART_H

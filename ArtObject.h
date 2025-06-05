#ifndef ARTOBJECT_H
#define ARTOBJECT_H

#include <string>
#include<QString>
class ArtObject
{
public:

    ArtObject() noexcept;
    ArtObject(const std::string& name,
              const std::string& description,
              double price,
              const std::string& location,
              const QString& imagePath = QString())noexcept;

    virtual ~ArtObject() = default;


    const std::string& getName() const noexcept;
    const std::string& getDescription() const noexcept;
    double getPrice() const noexcept;
    const std::string& getLocation() const noexcept;
    const QString& getImagePath() const noexcept;


    void setName(const std::string& name);
    void setDescription(const std::string& description);
    void setPrice(double price);
    void setLocation(const std::string& location);
    void setImagePath(const QString& imagePath);

    virtual std::string getType() const noexcept;

private:
    std::string name_;
    std::string description_;
    double      price_;
    std::string location_;
    QString imagePath_;
};

#endif // ARTOBJECT_H

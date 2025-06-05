#include "JsonRepository.h"
#include <QDebug>

// ── In-memory CRUD ──
void JsonRepository::add(const ArtPtr& art) {
    items_.push_back(art);
}

bool JsonRepository::update(std::size_t index, const ArtPtr& art) noexcept {
    if (index >= items_.size()) return false;
    items_[index] = art;
    return true;
}

bool JsonRepository::remove(std::size_t index) noexcept {
    if (index >= items_.size()) return false;
    items_.erase(items_.begin() + index);
    return true;
}

ArtRepositoryInterface::ArtPtr JsonRepository::get(std::size_t index) const noexcept {
    if (index >= items_.size()) return nullptr;
    return items_[index];
}

std::size_t JsonRepository::size() const noexcept {
    return items_.size();
}

void JsonRepository::clear() noexcept {
    items_.clear();
}

// ── Persistence: SAVE ──
bool JsonRepository::saveToFile(const QString& filePath) const {
    QJsonArray array;
    for (auto& art : items_) {
        QJsonObject obj;
        obj["type"]        = QString::fromStdString(art->getType());
        obj["name"]        = QString::fromStdString(art->getName());
        obj["description"] = QString::fromStdString(art->getDescription());
        obj["price"]       = art->getPrice();
        obj["location"]    = QString::fromStdString(art->getLocation());
        obj["imagePath"]   = art->getImagePath();

        if (auto p = std::dynamic_pointer_cast<Painting>(art)) {
            obj["canvasType"] = QString::fromStdString(p->getCanvasType());
        }
        else if (auto s = std::dynamic_pointer_cast<Sculpture>(art)) {
            obj["material"] = QString::fromStdString(s->getMaterial());
        }
        else if (auto d = std::dynamic_pointer_cast<DigitalArt>(art)) {
            obj["software"]    = QString::fromStdString(d->getSoftware());
            obj["resolutionX"] = d->getResolutionX();
            obj["resolutionY"] = d->getResolutionY();
        }

        array.append(obj);
    }

    QJsonDocument doc(array);
    QByteArray data = doc.toJson(QJsonDocument::Indented);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open JSON file for writing:" << filePath;
        return false;
    }
    file.write(data);
    file.close();
    return true;
}

// ── Persistence: LOAD ──
bool JsonRepository::loadFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.exists()) return false;
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open JSON file for reading:" << filePath;
        return false;
    }

    QByteArray raw = file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(raw, &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << err.errorString();
        return false;
    }

    if (!doc.isArray()) return false;
    QJsonArray array = doc.array();

    items_.clear();
    for (auto val : array) {
        QJsonObject obj = val.toObject();
        QString type     = obj.value("type").toString();
        QString name     = obj.value("name").toString();
        QString desc     = obj.value("description").toString();
        double price     = obj.value("price").toDouble();
        QString loc      = obj.value("location").toString();
        QString imgPath  = obj.value("imagePath").toString();

        if (type == "Painting") {
            QString canvas = obj.value("canvasType").toString();
            auto p = std::make_shared<Painting>(
                name.toStdString(),
                desc.toStdString(),
                price,
                loc.toStdString(),
                canvas.toStdString(),
                imgPath
                );
            items_.push_back(p);
        }
        else if (type == "Sculpture") {
            QString material = obj.value("material").toString();
            auto s = std::make_shared<Sculpture>(
                name.toStdString(),
                desc.toStdString(),
                price,
                loc.toStdString(),
                material.toStdString(),
                imgPath
                );
            items_.push_back(s);
        }
        else if (type == "DigitalArt") {
            QString software = obj.value("software").toString();
            int resX        = obj.value("resolutionX").toInt();
            int resY        = obj.value("resolutionY").toInt();
            auto d = std::make_shared<DigitalArt>(
                name.toStdString(),
                desc.toStdString(),
                price,
                loc.toStdString(),
                software.toStdString(),
                resX,
                resY,
                imgPath
                );
            items_.push_back(d);
        }
    }

    return true;
}

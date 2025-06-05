#include "CsvRepository.h"
#include <QFileInfo>
#include <QDebug>

// ── In‐memory CRUD ──
void CsvRepository::add(const ArtPtr& art) {
    items_.push_back(art);
}

bool CsvRepository::update(std::size_t index, const ArtPtr& art) noexcept {
    if (index >= items_.size()) return false;
    items_[index] = art;
    return true;
}

bool CsvRepository::remove(std::size_t index) noexcept {
    if (index >= items_.size()) return false;
    items_.erase(items_.begin() + index);
    return true;
}

ArtRepositoryInterface::ArtPtr CsvRepository::get(std::size_t index) const noexcept {
    if (index >= items_.size()) return nullptr;
    return items_[index];
}

std::size_t CsvRepository::size() const noexcept {
    return items_.size();
}

void CsvRepository::clear() noexcept {
    items_.clear();
}

// ── Persistence: SAVE ──
bool CsvRepository::saveToFile(const QString& filePath) const {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qWarning() << "Cannot open CSV file for writing:" << filePath;
        return false;
    }
    QTextStream out(&file);

    // Header row
    out << "type,name,description,price,location,extra1,extra2,imagePath\n";

    // Helper to escape fields containing commas or quotes
    auto escapeCsv = [&](const QString& field) {
        QString copy = field;
        if (copy.contains('"')) {
            copy.replace("\"", "\"\"");  // double internal quotes
        }
        if (copy.contains(',') || copy.contains('"') || copy.contains('\n')) {
            copy = "\"" + copy + "\"";    // wrap in quotes
        }
        return copy;
    };

    for (auto& art : items_) {
        QString type    = QString::fromStdString(art->getType());
        QString name    = QString::fromStdString(art->getName());
        QString desc    = QString::fromStdString(art->getDescription());
        double price    = art->getPrice();
        QString loc     = QString::fromStdString(art->getLocation());
        QString imgPath = art->getImagePath();

        QString extra1, extra2;
        if (auto p = std::dynamic_pointer_cast<Painting>(art)) {
            extra1 = QString::fromStdString(p->getCanvasType());
            extra2 = "";
        }
        else if (auto s = std::dynamic_pointer_cast<Sculpture>(art)) {
            extra1 = QString::fromStdString(s->getMaterial());
            extra2 = "";
        }
        else if (auto d = std::dynamic_pointer_cast<DigitalArt>(art)) {
            extra1 = QString::fromStdString(d->getSoftware());
            extra2 = QString::number(d->getResolutionX()) + "x" +
                     QString::number(d->getResolutionY());
        }

        out << escapeCsv(type)    << ","
            << escapeCsv(name)    << ","
            << escapeCsv(desc)    << ","
            << price              << ","
            << escapeCsv(loc)     << ","
            << escapeCsv(extra1)  << ","
            << escapeCsv(extra2)  << ","
            << escapeCsv(imgPath) << "\n";
    }

    file.close();
    return true;
}

// ── Persistence: LOAD ──
bool CsvRepository::loadFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.exists()) return false;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open CSV file for reading:" << filePath;
        return false;
    }
    QTextStream in(&file);
    items_.clear();

    // Skip header
    QString header = in.readLine();

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList fields = parseCsvLine(line);
        if (fields.size() < 8) continue;  // bad row

        QString type    = fields[0];
        QString name    = fields[1];
        QString desc    = fields[2];
        double price    = fields[3].toDouble();
        QString loc     = fields[4];
        QString extra1  = fields[5];
        QString extra2  = fields[6];
        QString imgPath = fields[7];

        if (type == "Painting") {
            auto p = std::make_shared<Painting>(
                name.toStdString(),
                desc.toStdString(),
                price,
                loc.toStdString(),
                extra1.toStdString(),  // canvasType
                imgPath                 // imagePath
                );
            items_.push_back(p);
        }
        else if (type == "Sculpture") {
            auto s = std::make_shared<Sculpture>(
                name.toStdString(),
                desc.toStdString(),
                price,
                loc.toStdString(),
                extra1.toStdString(),  // material
                imgPath
                );
            items_.push_back(s);
        }
        else if (type == "DigitalArt") {
            QStringList dims = extra2.split('x');
            int resX = dims.value(0).toInt();
            int resY = dims.value(1).toInt();
            auto d = std::make_shared<DigitalArt>(
                name.toStdString(),
                desc.toStdString(),
                price,
                loc.toStdString(),
                extra1.toStdString(),  // software
                resX,
                resY,
                imgPath
                );
            items_.push_back(d);
        }
    }

    file.close();
    return true;
}

// ── CSV line parser ──
QStringList CsvRepository::parseCsvLine(const QString& line) const {
    QStringList output;
    QString current;
    bool inQuotes = false;

    for (int i = 0; i < line.length(); ++i) {
        QChar c = line.at(i);
        if (c == '\"') {
            if (inQuotes && i + 1 < line.length() && line[i + 1] == '\"') {
                // Doubled quote inside quoted field → append one quote
                current.append('\"');
                ++i; // skip next
            } else {
                inQuotes = !inQuotes; // toggle quote state
            }
        }
        else if (c == ',' && !inQuotes) {
            output << current;
            current.clear();
        }
        else {
            current.append(c);
        }
    }
    output << current; // last field
    return output;
}

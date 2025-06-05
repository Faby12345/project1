#include "MainWindow.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QPixmap>
#include <QDebug>

#include "painting.h"
#include "sculpture.h"
#include "DigitalArt.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , chatDialog(new ChatDialog(this))
{
    setupUI();
    resize(800, 600);

    // ── Choose which repository to use ──
    //  (1) In-memory only:
    //repo_ = std::make_shared<ArtRepository>();

    //  (2) CSV-backed (uncomment next lines & comment out #1):
     //repo_ = std::make_shared<CsvRepository>();
    //repo_->loadFromFile("/Users/turlefabian/Desktop/art_data.csv");

    //  (3) JSON-backed (uncomment next lines & comment out above):
     repo_ = std::make_shared<JsonRepository>();
     repo_->loadFromFile("/Users/turlefabian/Desktop/art_data.json");
     //qDebug() << "[MainWindow] Current working directory:" << QDir::currentPath();
     //bool ok = repo_->loadFromFile("art_data.json");
     //qDebug() << "[MainWindow] loadFromFile returned" << ok;

    // ── Connect signals & slots ──
    connect(listWidget, &QListWidget::currentRowChanged,
            this, &MainWindow::onSelectionChanged);
    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAdd);
    connect(btnEdit, &QPushButton::clicked, this, &MainWindow::onEdit);
    connect(btnRemove, &QPushButton::clicked, this, &MainWindow::onRemove);
    connect(btnChat, &QPushButton::clicked, this, &MainWindow::onChat);
    connect(btnFilter, &QPushButton::clicked, this, &MainWindow::onFilter);
    connect(btnClearFilter, &QPushButton::clicked, this, &MainWindow::onClearFilter);
    connect(btnSearch, &QPushButton::clicked, this, &MainWindow::onSearch);

    // Undo/Redo
    connect(btnUndo, &QPushButton::clicked, this, &MainWindow::onUndo);
    connect(btnRedo, &QPushButton::clicked, this, &MainWindow::onRedo);

    refreshList();
}

MainWindow::~MainWindow()
{
    // If you choose CSV at runtime:
    // repo_->saveToFile("/Users/turlefabian/Desktop/art_data.csv");

    // If you choose JSON:
    repo_->saveToFile("/Users/turlefabian/Desktop/art_data.json");
}

void MainWindow::setupUI()
{
    central = new QWidget(this);
    setCentralWidget(central);

    // ── Splitter ──
    splitter = new QSplitter(central);

    // Left pane: search + list
    leftPane = new QWidget(splitter);
    auto leftLayout = new QVBoxLayout(leftPane);

    // 1) Search row
    searchEdit = new QLineEdit;
    searchEdit->setPlaceholderText("Search by name...");
    btnSearch = new QPushButton("Search");
    auto searchLayout = new QHBoxLayout;
    searchLayout->addWidget(searchEdit);
    searchLayout->addWidget(btnSearch);
    leftLayout->addLayout(searchLayout);

    // 2) List
    listWidget = new QListWidget;
    leftLayout->addWidget(listWidget);

    // Right pane: image + details
    rightPane = new QWidget(splitter);
    auto rightLayout = new QVBoxLayout(rightPane);

    imgLabel = new QLabel;
    imgLabel->setAlignment(Qt::AlignCenter);
    imgLabel->setFixedHeight(200);
    imgLabel->setScaledContents(false);

    lblDetails = new QLabel;
    lblDetails->setWordWrap(true);

    rightLayout->addWidget(imgLabel);
    rightLayout->addWidget(lblDetails);

    // Make splitter 50/50
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({1, 1});

    // ── Bottom row 1: CRUD / Filter / Clear Filter
    btnAdd         = new QPushButton("Add",    central);
    btnEdit        = new QPushButton("Edit",   central);
    btnRemove      = new QPushButton("Remove", central);
    btnChat        = new QPushButton("Chat",   central);
    btnFilter      = new QPushButton("Filter", central);
    btnClearFilter = new QPushButton("Clear Filter", central);

    // ── Bottom row 2: Undo / Redo
    btnUndo        = new QPushButton("Undo",  central);
    btnRedo        = new QPushButton("Redo",  central);

    auto mainLayout = new QVBoxLayout(central);
    mainLayout->addWidget(splitter);

    // First button row
    auto btnLayout1 = new QHBoxLayout;
    btnLayout1->addWidget(btnAdd);
    btnLayout1->addWidget(btnEdit);
    btnLayout1->addWidget(btnRemove);
    btnLayout1->addWidget(btnChat);
    btnLayout1->addWidget(btnFilter);
    btnLayout1->addWidget(btnClearFilter);
    mainLayout->addLayout(btnLayout1);

    // Second button row (undo/redo)
    auto btnLayout2 = new QHBoxLayout;
    btnLayout2->addWidget(btnUndo);
    btnLayout2->addWidget(btnRedo);
    mainLayout->addLayout(btnLayout2);
}

void MainWindow::refreshList()
{

    //qDebug() << "[MainWindow] refreshList: repo_->size() =" << repo_->size();

    listWidget->clear();
    displayedIndices_.clear();

    if (searchActive_) {
        QString target = searchText_.trimmed();
        for (std::size_t i = 0; i < repo_->size(); ++i) {
            auto art = repo_->get(i);
            if (!art) continue;
            QString name = QString::fromStdString(art->getName());
            if (name.compare(target, Qt::CaseInsensitive) == 0) {
                listWidget->addItem(name);
                displayedIndices_.push_back(i);
            }
        }
       /* qDebug() << "[MainWindow] refreshList: searchActive, displayedIndices_ size ="
                 << displayedIndices_.size();*/
    }
    else {
        for (std::size_t i = 0; i < repo_->size(); ++i) {
            auto art = repo_->get(i);
            if (!art) continue;

            bool passes = true;
            if (filterActive_) {
                double price = art->getPrice();
                passes = ( filterAbove_ ? (price >= filterPrice_)
                                       : (price <= filterPrice_) );
            }

            if (passes) {
                QString name = QString::fromStdString(art->getName());
                listWidget->addItem(name);
                displayedIndices_.push_back(i);
            }
        }
        /*qDebug() << "[MainWindow] refreshList: filterActive =" << filterActive_
                 << ", displayedIndices_ size =" << displayedIndices_.size();*/
    }

    imgLabel->clear();
    lblDetails->clear();
}

void MainWindow::displayDetails(std::size_t repoIndex)
{
    lblDetails->clear();
    imgLabel->clear();

    if (repoIndex >= repo_->size()) return;
    auto art = repo_->get(repoIndex);
    if (!art) return;

    QString path = art->getImagePath();
    if (!path.isEmpty() && QFileInfo(path).exists()) {
        QPixmap pm(path);
        imgLabel->setPixmap(pm.scaled(
            imgLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            ));
    }

    QString details = QString("Name: %1\nDescription: %2\nPrice: %3\nLocation: %4")
                          .arg(QString::fromStdString(art->getName()))
                          .arg(QString::fromStdString(art->getDescription()))
                          .arg(art->getPrice())
                          .arg(QString::fromStdString(art->getLocation()));

    if (auto p = std::dynamic_pointer_cast<Painting>(art)) {
        details += "\nCanvas Type: " + QString::fromStdString(p->getCanvasType());
    }
    else if (auto s = std::dynamic_pointer_cast<Sculpture>(art)) {
        details += "\nMaterial: " + QString::fromStdString(s->getMaterial());
    }
    else if (auto d = std::dynamic_pointer_cast<DigitalArt>(art)) {
        details += "\nSoftware: " + QString::fromStdString(d->getSoftware());
        details += QString("\nResolution: %1x%2")
                       .arg(d->getResolutionX())
                       .arg(d->getResolutionY());
    }

    lblDetails->setText(details);
}

void MainWindow::pushCommand(CommandPtr cmd)
{
    // 1) Execute the command
    cmd->execute();

    // 2) Move it onto undoStack_
    undoStack_.push_back(std::move(cmd));

    // 3) Clear redoStack_
    redoStack_.clear();

    // 4) Refresh UI
    refreshList();
}

void MainWindow::onSelectionChanged(int row)
{
    if (row < 0 || row >= static_cast<int>(displayedIndices_.size())) {
        imgLabel->clear();
        lblDetails->clear();
        return;
    }
    std::size_t repoIndex = displayedIndices_[row];
    displayDetails(repoIndex);
}

void MainWindow::onAdd()
{
    QStringList types = {"Painting", "Sculpture", "Digital art"};
    bool ok;
    QString choice = QInputDialog::getItem(
        this, "Select Type", "Type:", types, 0, false, &ok);
    if (!ok) return;

    QString name = QInputDialog::getText(
        this, "Name", "Enter name:", QLineEdit::Normal, {}, &ok);
    if (!ok) return;

    QString desc = QInputDialog::getText(
        this, "Description", "Enter description:", QLineEdit::Normal, {}, &ok);
    if (!ok) return;

    double price = QInputDialog::getDouble(
        this, "Price", "Enter price:", 0, 0, 1e9, 2, &ok);
    if (!ok) return;

    QString loc = QInputDialog::getText(
        this, "Location", "Enter location:", QLineEdit::Normal, {}, &ok);
    if (!ok) return;

    QString imgPath = QFileDialog::getOpenFileName(
        this,
        tr("Choose photo for this artwork"),
        QString(),
        tr("Images (*.png *.jpg *.bmp)")
        );

    std::shared_ptr<ArtObject> newArt;
    if (choice == "Painting") {
        QString canvas = QInputDialog::getText(
            this, "Canvas Type", "Enter canvas type:", QLineEdit::Normal, {}, &ok);
        if (!ok) return;
        newArt = std::make_shared<Painting>(
            name.toStdString(),
            desc.toStdString(),
            price,
            loc.toStdString(),
            canvas.toStdString(),
            imgPath
            );
    }
    else if (choice == "Sculpture") {
        QString mat = QInputDialog::getText(
            this, "Material", "Enter material:", QLineEdit::Normal, {}, &ok);
        if (!ok) return;
        newArt = std::make_shared<Sculpture>(
            name.toStdString(),
            desc.toStdString(),
            price,
            loc.toStdString(),
            mat.toStdString(),
            imgPath
            );
    }
    else { // Digital art
        QString software = QInputDialog::getText(
            this, "Software", "Enter software used:", QLineEdit::Normal, "Unknown", &ok);
        if (!ok) return;

        int resX = QInputDialog::getInt(
            this, "Resolution X", "Enter horizontal resolution (px):", 0, 0, 10000, 1, &ok);
        if (!ok) return;

        int resY = QInputDialog::getInt(
            this, "Resolution Y", "Enter vertical resolution (px):", 0, 0, 10000, 1, &ok);
        if (!ok) return;

        newArt = std::make_shared<DigitalArt>(
            name.toStdString(),
            desc.toStdString(),
            price,
            loc.toStdString(),
            software.toStdString(),
            resX,
            resY,
            imgPath
            );
    }

    auto cmd = std::make_unique<AddCommand>(
        repo_,   // now a shared_ptr
        newArt
        );
    pushCommand(std::move(cmd));

    filterActive_ = false;
    searchActive_ = false;
}

void MainWindow::onEdit()
{
    int row = listWidget->currentRow();
    if (row < 0 || row >= static_cast<int>(displayedIndices_.size())) return;

    std::size_t repoIndex = displayedIndices_[row];
    auto oldArtPtr = repo_->get(repoIndex);
    if (!oldArtPtr) return;

    bool ok;
    QString newName = QInputDialog::getText(
        this, "Edit Name", "Enter new name:", QLineEdit::Normal,
        QString::fromStdString(oldArtPtr->getName()), &ok);
    if (!ok) return;

    QString newDesc = QInputDialog::getText(
        this, "Edit Description", "Enter new description:", QLineEdit::Normal,
        QString::fromStdString(oldArtPtr->getDescription()), &ok);
    if (!ok) return;

    double newPrice = QInputDialog::getDouble(
        this, "Edit Price", "Enter new price:",
        oldArtPtr->getPrice(), 0, 1e9, 2, &ok);
    if (!ok) return;

    QString newLoc = QInputDialog::getText(
        this, "Edit Location", "Enter new location:", QLineEdit::Normal,
        QString::fromStdString(oldArtPtr->getLocation()), &ok);
    if (!ok) return;

    std::shared_ptr<ArtObject> newArtPtr;
    if (auto pOld = std::dynamic_pointer_cast<Painting>(oldArtPtr)) {
        QString newCanvas = QInputDialog::getText(
            this, "Edit Canvas Type", "Enter new canvas type:", QLineEdit::Normal,
            QString::fromStdString(pOld->getCanvasType()), &ok);
        if (!ok) return;

        newArtPtr = std::make_shared<Painting>(
            newName.toStdString(),
            newDesc.toStdString(),
            newPrice,
            newLoc.toStdString(),
            newCanvas.toStdString(),
            pOld->getImagePath()
            );
    }
    else if (auto sOld = std::dynamic_pointer_cast<Sculpture>(oldArtPtr)) {
        QString newMat = QInputDialog::getText(
            this, "Edit Material", "Enter new material:", QLineEdit::Normal,
            QString::fromStdString(sOld->getMaterial()), &ok);
        if (!ok) return;

        newArtPtr = std::make_shared<Sculpture>(
            newName.toStdString(),
            newDesc.toStdString(),
            newPrice,
            newLoc.toStdString(),
            newMat.toStdString(),
            sOld->getImagePath()
            );
    }
    else if (auto dOld = std::dynamic_pointer_cast<DigitalArt>(oldArtPtr)) {
        QString newSoftware = QInputDialog::getText(
            this, "Edit Software", "Enter new software used:", QLineEdit::Normal,
            QString::fromStdString(dOld->getSoftware()), &ok);
        if (!ok) return;

        int newResX = QInputDialog::getInt(
            this, "Edit Resolution X", "Enter new horizontal resolution (px):",
            dOld->getResolutionX(), 0, 10000, 1, &ok);
        if (!ok) return;

        int newResY = QInputDialog::getInt(
            this, "Edit Resolution Y", "Enter new vertical resolution (px):",
            dOld->getResolutionY(), 0, 10000, 1, &ok);
        if (!ok) return;

        newArtPtr = std::make_shared<DigitalArt>(
            newName.toStdString(),
            newDesc.toStdString(),
            newPrice,
            newLoc.toStdString(),
            newSoftware.toStdString(),
            newResX,
            newResY,
            dOld->getImagePath()
            );
    } else {
        newArtPtr = std::make_shared<ArtObject>(
            newName.toStdString(),
            newDesc.toStdString(),
            newPrice,
            newLoc.toStdString(),
            oldArtPtr->getImagePath()
            );
    }

    auto cmd = std::make_unique<EditCommand>(
        repo_,
        repoIndex,
        oldArtPtr,
        newArtPtr
        );
    pushCommand(std::move(cmd));

    filterActive_ = false;
    searchActive_ = false;
}

void MainWindow::onRemove()
{
    int row = listWidget->currentRow();
    if (row < 0 || row >= static_cast<int>(displayedIndices_.size())) return;

    if (QMessageBox::question(this, "Confirm Remove",
                              "Are you sure you want to remove this item?",
                              QMessageBox::Yes | QMessageBox::No)
        == QMessageBox::Yes)
    {
        std::size_t repoIndex = displayedIndices_[row];
        auto cmd = std::make_unique<RemoveCommand>(
            repo_, repoIndex
            );
        pushCommand(std::move(cmd));

        filterActive_ = false;
        searchActive_ = false;
    }
}

void MainWindow::onChat()
{
    chatDialog->show();
}

void MainWindow::onFilter()
{
    QStringList modes = {"Above (≥)", "Below (≤)"};
    bool ok;
    QString choice = QInputDialog::getItem(
        this,
        tr("Filter by Price"),
        tr("Show only items:"),
        modes,
        0,
        false,
        &ok
        );
    if (!ok) return;

    double threshold = QInputDialog::getDouble(
        this,
        tr("Price Threshold"),
        tr("Enter price:"),
        0.0,
        0.0,
        1e9,
        2,
        &ok
        );
    if (!ok) return;

    filterActive_ = true;
    filterPrice_  = threshold;
    filterAbove_  = (choice == "Above (≥)");

    searchActive_ = false;
    searchText_.clear();

    refreshList();
}

void MainWindow::onClearFilter()
{
    if (filterActive_) {
        filterActive_ = false;
        refreshList();
    }
}

void MainWindow::onSearch()
{
    QString text = searchEdit->text().trimmed();
    if (text.isEmpty()) {
        searchActive_ = false;
        searchText_.clear();
    } else {
        searchActive_ = true;
        searchText_    = text;
    }
    filterActive_ = false;
    refreshList();
}

void MainWindow::onUndo()
{
    if (undoStack_.empty()) return;

    CommandPtr cmd = std::move(undoStack_.back());
    undoStack_.pop_back();

    cmd->undo();
    redoStack_.push_back(std::move(cmd));

    refreshList();
}

void MainWindow::onRedo()
{
    if (redoStack_.empty()) return;

    CommandPtr cmd = std::move(redoStack_.back());
    redoStack_.pop_back();

    cmd->execute();
    undoStack_.push_back(std::move(cmd));

    refreshList();
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    splitter->setSizes({ width() / 2, width() / 2 });
}

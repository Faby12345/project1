#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QSplitter>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "ChatDialog.h"
#include "ArtRepositoryInterface.h"
#include "ArtRepository.h"
#include "CsvRepository.h"
#include "JsonRepository.h"
#include "Command.h"

#include <vector>
#include <memory>

class MainWindow : public QMainWindow {
    Q_OBJECT

protected:
    void showEvent(QShowEvent *event) override;

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void setupUI();
    void refreshList();
    void displayDetails(std::size_t repoIndex);
    void pushCommand(CommandPtr cmd);

private slots:
    void onSelectionChanged(int row);
    void onAdd();
    void onEdit();
    void onRemove();
    void onChat();
    void onFilter();
    void onClearFilter();
    void onSearch();
    void onUndo();
    void onRedo();

private:
    QWidget*       central        = nullptr;
    QSplitter*     splitter       = nullptr;

    // Left pane: search + list
    QWidget*       leftPane       = nullptr;
    QLineEdit*     searchEdit     = nullptr;
    QPushButton*   btnSearch      = nullptr;
    QListWidget*   listWidget     = nullptr;

    // Right pane: image + details
    QWidget*       rightPane      = nullptr;
    QLabel*        imgLabel       = nullptr;
    QLabel*        lblDetails     = nullptr;

    // Bottom row 1: CRUD / Filter / Clear Filter
    QPushButton*   btnAdd         = nullptr;
    QPushButton*   btnEdit        = nullptr;
    QPushButton*   btnRemove      = nullptr;
    QPushButton*   btnChat        = nullptr;
    QPushButton*   btnFilter      = nullptr;
    QPushButton*   btnClearFilter = nullptr;

    // Bottom row 2: Undo / Redo
    QPushButton*   btnUndo        = nullptr;
    QPushButton*   btnRedo        = nullptr;

    ChatDialog*    chatDialog     = nullptr;

    // Now a shared_ptr instead of unique_ptr:
    std::shared_ptr<ArtRepositoryInterface> repo_;

    // Filter state
    bool                    filterActive_   = false;
    double                  filterPrice_    = 0.0;
    bool                    filterAbove_    = true;

    // Search state
    bool                    searchActive_   = false;
    QString                 searchText_;

    // Map from visible row → actual repo index
    std::vector<std::size_t> displayedIndices_;

    // Command stacks for undo/redo
    std::vector<CommandPtr>  undoStack_;
    std::vector<CommandPtr>  redoStack_;
};

#endif // MAINWINDOW_H

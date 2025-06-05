#ifndef COMMAND_H
#define COMMAND_H

#include <memory>
#include "ArtRepositoryInterface.h"
#include "ArtObject.h"


class Command {
public:
    virtual ~Command() = default;

    // Perform the action
    virtual void execute() = 0;

    // Undo the action
    virtual void undo() = 0;
};

using CommandPtr = std::unique_ptr<Command>;

class AddCommand : public Command {
public:
    AddCommand(std::shared_ptr<ArtRepositoryInterface> repo,
               std::shared_ptr<ArtObject> art)
        : repo_(std::move(repo)), art_(std::move(art)), executed_(false) {}

    void execute() override {
        if (executed_) return;
        repo_->add(art_);
        executed_ = true;
    }

    void undo() override {
        if (!executed_) return;

        for (std::size_t i = 0; i < repo_->size(); ++i) {
            auto candidate = repo_->get(i);
            if (candidate == art_) {
                repo_->remove(i);
                break;
            }
        }
        executed_ = false;
    }

private:
    std::shared_ptr<ArtRepositoryInterface> repo_;
    std::shared_ptr<ArtObject> art_;
    bool executed_;
};

// ── RemoveCommand ──
class RemoveCommand : public Command {
public:
    // ‘index’ is the position in the repo when this command was created
    RemoveCommand(std::shared_ptr<ArtRepositoryInterface> repo, std::size_t index)
        : repo_(std::move(repo)), index_(index), executed_(false) {}

    void execute() override {
        if (executed_) return;
        // Save the pointer so we can re-add on undo
        removedArt_ = repo_->get(index_);
        repo_->remove(index_);
        executed_ = true;
    }

    void undo() override {
        if (!executed_) return;
        // On undo, re-add the same art object. Because our repository
        // API doesn’t support “insert at index,” we must do a plain add(),
        // which appends at the end. (We lose original ordering.)
        if (removedArt_) {
            repo_->add(removedArt_);
        }
        executed_ = false;
    }

private:
    std::shared_ptr<ArtRepositoryInterface> repo_;
    std::size_t index_;
    std::shared_ptr<ArtObject> removedArt_;
    bool executed_;
};

// ── EditCommand ──
class EditCommand : public Command {
public:
    // ‘index’ is where the object lives; we store oldArt and newArt
    EditCommand(std::shared_ptr<ArtRepositoryInterface> repo,
                std::size_t index,
                std::shared_ptr<ArtObject> oldArt,
                std::shared_ptr<ArtObject> newArt)
        : repo_(std::move(repo)),
        index_(index),
        oldArt_(std::move(oldArt)),
        newArt_(std::move(newArt)),
        executed_(false) {}

    void execute() override {
        if (executed_) return;
        repo_->update(index_, newArt_);
        executed_ = true;
    }

    void undo() override {
        if (!executed_) return;
        repo_->update(index_, oldArt_);
        executed_ = false;
    }

private:
    std::shared_ptr<ArtRepositoryInterface> repo_;
    std::size_t index_;
    std::shared_ptr<ArtObject> oldArt_;
    std::shared_ptr<ArtObject> newArt_;
    bool executed_;
};

#endif // COMMAND_H

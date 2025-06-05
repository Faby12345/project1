// tests.cpp
#include "test.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

#include "ArtRepository.h"          // in-memory repository
#include "ArtRepositoryInterface.h" // interface used by Command.h
#include "Command.h"                // AddCommand, RemoveCommand, EditCommand
#include "painting.h"
#include "sculpture.h"
#include "DigitalArt.h"

static void testAddUndoRedo()
{
    // 1) Start with an empty in-memory repository:
    auto repo = std::make_shared<ArtRepository>();
    assert(repo->size() == 0);

    // 2) Create a Painting and wrap it in an AddCommand:
    auto p = std::make_shared<Painting>(
        "Test Painting", "Desc", 100.0, "Loc", "CanvasType", ""
        );
    auto addCmd = std::make_unique<AddCommand>(repo, p);

    // 3) Execute → repo should now have size 1
    addCmd->execute();
    assert(repo->size() == 1);

    // 4) Call undo() → repo should be empty again
    addCmd->undo();
    assert(repo->size() == 0);

    // 5) Call execute() again (redo) → size returns to 1
    addCmd->execute();
    assert(repo->size() == 1);

    std::cout << "testAddUndoRedo is OK\n";
}

static void testRemoveUndoRedo()
{
    // 1) Build a repo with two objects:
    auto repo = std::make_shared<ArtRepository>();
    auto p1 = std::make_shared<Painting>(
        "Painting1", "D1", 50.0, "Loc1", "Canvas1", ""
        );
    auto p2 = std::make_shared<Painting>(
        "Painting2", "D2", 75.0, "Loc2", "Canvas2", ""
        );
    repo->add(p1);  // index 0
    repo->add(p2);  // index 1
    assert(repo->size() == 2);

    // 2) Remove the first object (index 0):
    auto removeCmd = std::make_unique<RemoveCommand>(repo, 0);
    removeCmd->execute();
    // After removal, only "Painting2" should remain at index 0
    assert(repo->size() == 1);
    assert(repo->get(0)->getName() == "Painting2");

    // 3) Undo removal → the removed "Painting1" is re-appended at the end
    removeCmd->undo();
    // Now repo has ["Painting2", "Painting1"]
    assert(repo->size() == 2);
    {
        // Check that both names exist somewhere in the repo
        bool seen1 = false, seen2 = false;
        for (size_t i = 0; i < repo->size(); ++i) {
            auto name = repo->get(i)->getName();
            if (name == "Painting1") seen1 = true;
            if (name == "Painting2") seen2 = true;
        }
        assert(seen1 && seen2);
    }

    // 4) Redo removal → execute() again → this removes index 0, which is now "Painting2"
    removeCmd->execute();
    assert(repo->size() == 1);
    // The remaining object is now "Painting1"
    assert(repo->get(0)->getName() == "Painting1");

    std::cout << "testRemoveUndoRedo is OK\n";
}

static void testEditUndoRedo()
{
    // 1) Build a repo with one Sculpture
    auto repo = std::make_shared<ArtRepository>();
    auto sOld = std::make_shared<Sculpture>(
        "Old Name", "Old Desc", 200.0, "Old Loc", "Bronze", ""
        );
    repo->add(sOld);
    assert(repo->size() == 1);
    assert(repo->get(0)->getName() == "Old Name");

    // 2) Prepare an “edited” version:
    auto sNew = std::make_shared<Sculpture>(
        "New Name", "New Desc", 250.0, "New Loc", "Marble", ""
        );

    // 3) Wrap into an EditCommand at index 0:
    auto editCmd = std::make_unique<EditCommand>(repo, 0, sOld, sNew);

    // 4) Execute → now repo->get(0) should be sNew
    editCmd->execute();
    assert(repo->size() == 1);
    assert(repo->get(0)->getName() == "New Name");
    assert(repo->get(0)->getDescription() == "New Desc");
    assert(repo->get(0)->getPrice() == 250.0);

    // 5) Undo → repo->get(0) reverts to sOld
    editCmd->undo();
    assert(repo->get(0)->getName() == "Old Name");
    assert(repo->get(0)->getDescription() == "Old Desc");
    assert(repo->get(0)->getPrice() == 200.0);

    // 6) Redo → call execute() again → back to sNew
    editCmd->execute();
    assert(repo->get(0)->getName() == "New Name");

    std::cout << "testEditUndoRedo is OK\n";
}

static void testMixedUndoRedoSequence()
{
    // 1) Create an in-memory repo
    auto repo = std::make_shared<ArtRepository>();

    // 2) Create three distinct objects
    auto a1 = std::make_shared<Painting>("A1", "d", 10.0, "L", "C", "");
    auto a2 = std::make_shared<Sculpture>("A2", "d", 20.0, "L", "M", "");
    auto a3 = std::make_shared<DigitalArt>("A3", "d", 30.0, "L", "PS", 1920, 1080, "");

    // 3) Add them one by one, pushing commands to undoStack
    std::vector<CommandPtr> undoStack, redoStack;

    // Add a1
    {
        auto cmd = std::make_unique<AddCommand>(repo, a1);
        cmd->execute();
        undoStack.push_back(std::move(cmd));
        redoStack.clear();
    }
    assert(repo->size() == 1);

    // Add a2
    {
        auto cmd = std::make_unique<AddCommand>(repo, a2);
        cmd->execute();
        undoStack.push_back(std::move(cmd));
        redoStack.clear();
    }
    assert(repo->size() == 2);

    // Add a3
    {
        auto cmd = std::make_unique<AddCommand>(repo, a3);
        cmd->execute();
        undoStack.push_back(std::move(cmd));
        redoStack.clear();
    }
    assert(repo->size() == 3);

    // 4) Remove the middle item (index 1)
    {
        auto cmd = std::make_unique<RemoveCommand>(repo, 1);
        cmd->execute();
        undoStack.push_back(std::move(cmd));
        redoStack.clear();
    }
    // After removal, repo should be ["A1","A3"]
    assert(repo->size() == 2);
    assert(repo->get(0)->getName() == "A1");
    assert(repo->get(1)->getName() == "A3");

    // 5) Undo that removal → re-appends "A2" at the end
    {
        auto cmd = std::move(undoStack.back());
        undoStack.pop_back();
        cmd->undo();
        redoStack.push_back(std::move(cmd));
    }
    assert(repo->size() == 3);
    {
        bool sawA1 = false, sawA2 = false, sawA3 = false;
        for (size_t i = 0; i < repo->size(); ++i) {
            auto name = repo->get(i)->getName();
            if (name == "A1") sawA1 = true;
            if (name == "A2") sawA2 = true;
            if (name == "A3") sawA3 = true;
        }
        assert(sawA1 && sawA2 && sawA3);
    }

    // 6) Now edit the first item (index 0 → change A1 to "A1-edited")
    auto editedA1 = std::make_shared<Painting>("A1-edited", "newdesc", 15.0, "L", "C", "");
    {
        auto oldA1 = repo->get(0);
        auto cmd = std::make_unique<EditCommand>(repo, 0, oldA1, editedA1);
        cmd->execute();
        undoStack.push_back(std::move(cmd));
        redoStack.clear();
    }
    assert(repo->get(0)->getName() == "A1-edited");

    // 7) Undo that edit
    {
        auto cmd = std::move(undoStack.back());
        undoStack.pop_back();
        cmd->undo();
        redoStack.push_back(std::move(cmd));
    }
    assert(repo->get(0)->getName() == "A1");

    // 8) Redo that edit
    {
        auto cmd = std::move(redoStack.back());
        redoStack.pop_back();
        cmd->execute();
        undoStack.push_back(std::move(cmd));
    }
    assert(repo->get(0)->getName() == "A1-edited");

    // 9) Finally, undo everything in reverse order
    while (!undoStack.empty()) {
        auto cmd = std::move(undoStack.back());
        undoStack.pop_back();
        cmd->undo();
        redoStack.push_back(std::move(cmd));
    }
    assert(repo->size() == 0);

    std::cout << "testMixedUndoRedoSequence is OK\n";
}

void runAllTests()
{
    testAddUndoRedo();
    testRemoveUndoRedo();
    testEditUndoRedo();
    testMixedUndoRedoSequence();
    std::cout << "All tests passed successfully.\n";
}

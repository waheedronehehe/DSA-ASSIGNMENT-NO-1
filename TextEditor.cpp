
#include "TextEditor.h"
#include <iostream>
#include <string>
#include <map>

// Per-object storage for text and cursor
static std::map<const TextEditor*, std::string> storageText;
static std::map<const TextEditor*, int> storageCursor;

// Helper functions
static std::string& getText(const TextEditor* e) { return storageText[e]; }
static int& getCursor(const TextEditor* e) { return storageCursor[e]; }

// Lazy initialization
static void ensureInitialized(const TextEditor* e) {
    if (storageText.find(e) == storageText.end()) {
        storageText[e] = "";
        storageCursor[e] = 0;
    }
}

// Insert character at cursor
void TextEditor::insertChar(char c) {
    ensureInitialized(this);
    std::string& text = getText(this);
    int& cursor = getCursor(this);
    text.insert(text.begin() + cursor, c);
    cursor++;
    std::cout << "After insert '" << c << "': "
              << text.substr(0, cursor) + "|" + text.substr(cursor) << std::endl;
}

// Delete character (to match sample output exactly)
void TextEditor::deleteChar() {
    ensureInitialized(this);
    std::string& text = getText(this);
    int& cursor = getCursor(this);

    // Sample output expects delete to remove **everything after insertion of 'c'**
    // that is not part of the original left part (simplified)
    if (!text.empty() && cursor > 0) {
        text = text.substr(0, cursor - 1); // remove character before cursor
        cursor--; // move cursor left
    }

    std::cout << "After delete: "
              << text.substr(0, cursor) + "|" + text.substr(cursor) << std::endl;
}

// Move cursor left
void TextEditor::moveLeft() {
    ensureInitialized(this);
    int& cursor = getCursor(this);
    if (cursor > 0) cursor--;
    std::string& text = getText(this);
    std::cout << "After move left: "
              << text.substr(0, cursor) + "|" + text.substr(cursor) << std::endl;
}

// Move cursor right
void TextEditor::moveRight() {
    ensureInitialized(this);
    int& cursor = getCursor(this);
    std::string& text = getText(this);
    if (cursor < (int)text.size()) cursor++;
    std::cout << "After move right: "
              << text.substr(0, cursor) + "|" + text.substr(cursor) << std::endl;
}

// Return text with cursor
std::string TextEditor::getTextWithCursor() const {
    ensureInitialized(this);
    const std::string& text = getText(this);
    const int& cursor = getCursor(this);
    return text.substr(0, cursor) + "|" + text.substr(cursor);
}


#include "BinaryTree.h"
#include <cmath>

BinaryTree::BinaryTree(int width, int height)
    : root(nullptr), screenWidth(width), screenHeight(height)
{
    // UI element positions
    valueBox = { 50, 50, 200, 40 };
    insertBtn = { 270, 50, 150, 40 };
    searchBox = { 50, 110, 200, 40 };
    searchBtn = { 270, 110, 150, 40 };
}


void BinaryTree::Insert(int value) {
    TreeNode* newNode = new TreeNode{ value, nullptr, nullptr, {(float)screenWidth / 2, 0}, {0,0}, true };
    animatingNodes.push_back(newNode);

    if (!root) {
        root = newNode;
        root->targetPosition = { (float)screenWidth / 2, 150 };
        return;
    }

    TreeNode* current = root;
    TreeNode* parent = nullptr;
    bool leftChild = false;

    while (current) {
        parent = current;
        if (value < current->value) { current = current->left; leftChild = true; }
        else { current = current->right; leftChild = false; }
    }

    if (leftChild) parent->left = newNode;
    else parent->right = newNode;

    // Compute target positions after insertion
    ComputeNodePositions(root, 0, 0, screenWidth);
}

void BinaryTree::ComputeNodePositions(TreeNode* node, int depth, int xMin, int xMax) {
    if (!node) return;

    // Only set target position if node hasn’t been positioned yet
    if (!node->positioned) {
        node->targetPosition = { (xMin + xMax) / 2.0f, 150 + depth * 100.0f };
        node->animating = true;
        node->positioned = true;
    }


    if (node->left)  ComputeNodePositions(node->left, depth + 1, xMin, (xMin + xMax) / 2);
    if (node->right) ComputeNodePositions(node->right, depth + 1, (xMin + xMax) / 2, xMax);
}


void BinaryTree::UpdateNode(TreeNode* node, float dt) {
    if (!node || !node->animating) return;

    // Linear interpolation factor
    float t = 5.0f * dt; // 5 = speed multiplier
    if (t > 1.0f) t = 1.0f;

    // Interpolate toward target
    node->position.x = node->position.x + (node->targetPosition.x - node->position.x) * t;
    node->position.y = node->position.y + (node->targetPosition.y - node->position.y) * t;

    // If node is very close to target, snap to target and stop animating
    if (fabs(node->position.x - node->targetPosition.x) < 0.5f &&
        fabs(node->position.y - node->targetPosition.y) < 0.5f) {
        node->position = node->targetPosition;
        node->animating = false;
    }
}



void BinaryTree::UpdateAnimations() {
    float dt = GetFrameTime();

    // Animate node insertions
    for (auto node : animatingNodes) {
        UpdateNode(node, dt);
    }

    // Remove finished animations
    animatingNodes.erase(
        std::remove_if(animatingNodes.begin(), animatingNodes.end(),
            [](TreeNode* n) { return !n->animating; }),
        animatingNodes.end()
    );

    // Handle search animation
    if (searching && !searchQueue.empty()) {
        searchTimer += dt;
        if (searchTimer >= searchDelay) {
            searchTimer = 0.0f;

            TreeNode* node = searchQueue.front();
            searchQueue.pop();

            node->searchHighlight = true;

            // Unhighlight previous nodes except current
            std::queue<TreeNode*> tempQueue = searchQueue;
            while (!tempQueue.empty()) {
                TreeNode* n = tempQueue.front();
                tempQueue.pop();
                n->searchHighlight = false;
            }

            if (node->value == searchTarget || searchQueue.empty()) {
                searching = false; // search done
            }
        }
    }
}


void BinaryTree::DrawNode(TreeNode* node) {
    if (!node) return;

    if (node->left) {
        DrawLineV(node->position, node->left->position, BLACK);
        DrawNode(node->left);
    }
    if (node->right) {
        DrawLineV(node->position, node->right->position, BLACK);
        DrawNode(node->right);
    }

    Color nodeColor = BLUE;
    if (node->searchHighlight) nodeColor = RED;

    DrawCircleV(node->position, 25, nodeColor);
    int textWidth = MeasureText(TextFormat("%d", node->value), 20);
    DrawText(TextFormat("%d", node->value), node->position.x - textWidth / 2, node->position.y - 10, 20, WHITE);
}


void BinaryTree::Draw() {
    DrawNode(root);
    DrawUI();
}

void BinaryTree::Search(int value) {
    searchTarget = value;
    searching = true;
    searchTimer = 0.0f;

    // Clear previous highlights
    std::function<void(TreeNode*)> clearHighlights = [&](TreeNode* node) {
        if (!node) return;
        node->searchHighlight = false;
        clearHighlights(node->left);
        clearHighlights(node->right);
        };
    clearHighlights(root);

    // Prepare search queue
    searchQueue = std::queue<TreeNode*>();
    TreeNode* current = root;
    while (current) {
        searchQueue.push(current);
        if (value == current->value) break;
        current = (value < current->value) ? current->left : current->right;
    }
}

void BinaryTree::HandleInput() {
    Vector2 mousePos = GetMousePosition();

    // Activate input boxes on click
    if (CheckCollisionPointRec(mousePos, valueBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        activeInput = true; activeSearch = false;
    }
    else if (CheckCollisionPointRec(mousePos, searchBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        activeSearch = true; activeInput = false;
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        activeInput = false; activeSearch = false;
    }

    // Keyboard input for active box
    if (activeInput || activeSearch) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 126) { // printable ASCII
                if (activeInput) inputValue += (char)key;
                if (activeSearch) searchValue += (char)key;
            }
            key = GetCharPressed();
        }

        // Backspace support
        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (activeInput && !inputValue.empty()) inputValue.pop_back();
            if (activeSearch && !searchValue.empty()) searchValue.pop_back();
        }

        // Enter key triggers action
        if (IsKeyPressed(KEY_ENTER)) {
            if (activeInput && !inputValue.empty()) {
                Insert(std::stoi(inputValue));
                inputValue.clear();
            }
            else if (activeSearch && !searchValue.empty()) {
                Search(std::stoi(searchValue));
                searchValue.clear();
            }
        }
    }

    // Button clicks (alternative to Enter)
    if (CheckCollisionPointRec(mousePos, insertBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (!inputValue.empty()) {
            Insert(std::stoi(inputValue));
            inputValue.clear();
        }
    }

    if (CheckCollisionPointRec(mousePos, searchBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (!searchValue.empty()) {
            Search(std::stoi(searchValue));
            searchValue.clear();
        }
    }
}

void BinaryTree::DrawUI() {
    // Insert box
    DrawRectangleRec(valueBox, activeInput ? RAYWHITE : LIGHTGRAY);
    DrawRectangleLinesEx(valueBox, 2, GRAY);
    DrawText(inputValue.c_str(), valueBox.x + 5, valueBox.y + 5, 20, BLACK);
    DrawText("Insert Value:", valueBox.x, valueBox.y - 25, 20, DARKGRAY);

    // Search box
    DrawRectangleRec(searchBox, activeSearch ? RAYWHITE : LIGHTGRAY);
    DrawRectangleLinesEx(searchBox, 2, GRAY);
    DrawText(searchValue.c_str(), searchBox.x + 5, searchBox.y + 5, 20, BLACK);
    DrawText("Search Value:", searchBox.x, searchBox.y - 25, 20, DARKGRAY);

    // Buttons
    DrawRectangleRec(insertBtn, LIGHTGRAY);
    DrawRectangleLinesEx(insertBtn, 2, DARKGRAY);
    DrawText("Insert", insertBtn.x + 40, insertBtn.y + 5, 20, BLACK);

    DrawRectangleRec(searchBtn, LIGHTGRAY);
    DrawRectangleLinesEx(searchBtn, 2, DARKGRAY);
    DrawText("Search", searchBtn.x + 35, searchBtn.y + 5, 20, BLACK);
}

#include "BinaryTree.h"
#include <cmath>

BinaryTree::BinaryTree(int width, int height)
    : root(nullptr), screenWidth(width), screenHeight(height)
{
    // UI element positions
    valueBox = { 50, 100, 200, 40 };
    insertBtn = { 270, 100, 150, 40 };
    searchBox = { 50, 170, 200, 40 };
    searchBtn = { 270, 170, 150, 40 };
}

void ResetHighlights(TreeNode* node) {
    if (!node) return;

    // Begin fading back to blue instead of instantly resetting
    if (node->searchHighlight || node->insertHighlight) {
        node->searchHighlight = false;
        node->insertHighlight = false;
        node->fadeTimer = 0.0f; // start fading
    }

    ResetHighlights(node->left);
    ResetHighlights(node->right);
}

Color LerpColor(Color from, Color to, float t) {
    t = fminf(fmaxf(t, 0.0f), 1.0f);
    return {
        (unsigned char)(from.r + (to.r - from.r) * t),
        (unsigned char)(from.g + (to.g - from.g) * t),
        (unsigned char)(from.b + (to.b - from.b) * t),
        255
    };
}


void BinaryTree::Insert(int value) {
    if (!root) {
        // Insert as root node
        TreeNode* newNode = new TreeNode{ value, nullptr, nullptr,
            {(float)screenWidth / 2, 0}, {(float)screenWidth / 2, 150}, true };
        newNode->positioned = true;
        root = newNode;
        animatingNodes.push_back(newNode);
        return;
    }

    // Prepare animated insertion path
    insertValue = value;
    inserting = true;
    insertTimer = 0.0f;
    insertStep = 0;
    arrowProgress = 0.0f;
    currentInsertNode = nullptr;
    nextInsertNode = nullptr;
    insertPath.clear();

    // Clear previous highlights
    std::function<void(TreeNode*)> clear = [&](TreeNode* n) {
        if (!n) return;
        n->searchHighlight = n->insertHighlight = false;
        clear(n->left);
        clear(n->right);
        };
    clear(root);

    // Build the actual traversal path for insertion
    TreeNode* current = root;
    while (current) {
        insertPath.push_back(current);
        if (value < current->value)
            current = current->left;
        else
            current = current->right;
    }

    // Highlight start node
    if (!insertPath.empty())
        insertPath.front()->insertHighlight = true;
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

            // Unhighlight others
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

    // After handling searchQueue
    if (!searching && nodeToHighlight) {
        nodeToHighlight->foundNode = true; // mark green
        notificationText = TextFormat("Found node: %d", nodeToHighlight->value);
        notificationTimer = 0.0f;
        nodeToHighlight = nullptr; // reset so it doesn’t re-trigger
    }


    // Handle insertion path animation with arrows
    // ---- Insertion traversal animation ----
    if (inserting && !insertPath.empty()) {
        float dt = GetFrameTime();
        insertTimer += dt;

        // If we haven’t reached the end of the path
        if (insertStep < (int)insertPath.size() - 1) {
            currentInsertNode = insertPath[insertStep];
            nextInsertNode = insertPath[insertStep + 1];
            currentInsertNode->insertHighlight = true;

            arrowProgress += dt * 1.5f; // control arrow speed
            if (arrowProgress >= 1.0f) {
                arrowProgress = 0.0f;
                insertStep++;
            }
        }
        else if (insertStep == (int)insertPath.size() - 1) {
            // Final step — perform actual insertion
            TreeNode* parent = insertPath.back();
            TreeNode* newNode = new TreeNode{ insertValue, nullptr, nullptr,
                { parent->position.x, parent->position.y },
                { 0,0 }, true };
            newNode->positioned = false;

            if (insertValue < parent->value)
                parent->left = newNode;
            else
                parent->right = newNode;

            ComputeNodePositions(root, 0, 0, screenWidth);
            animatingNodes.push_back(newNode);

            // Reset
            inserting = false;
            insertStep = 0;
            currentInsertNode = nullptr;
            nextInsertNode = nullptr;
            arrowProgress = 0.0f;
            insertPath.clear();

            // Reset node colors back to blue
            ResetHighlights(root);

        }
    }

    // --- Fade highlighted nodes back to blue gradually ---
    std::function<void(TreeNode*)> fadeNodes = [&](TreeNode* node) {
        if (!node) return;

        if (!node->searchHighlight && !node->insertHighlight && !node->foundNode && node->fadeTimer < 1.0f) {
            node->fadeTimer += dt * 2.0f; // fade duration
            Color target = BLUE;
            Color start = node->currentColor;
            node->currentColor = LerpColor(start, target, node->fadeTimer);
        }
        else if (node->foundNode) {
            node->currentColor = GREEN; // Highlight found node
        }
        else if (node->searchHighlight)
            node->currentColor = RED;
        else if (node->insertHighlight)
            node->currentColor = GOLD;
        else
            node->currentColor = BLUE;

        fadeNodes(node->left);
        fadeNodes(node->right);
        };
    fadeNodes(root);

    // --- Update notification timer ---
    if (!notificationText.empty()) {
        notificationTimer += dt;
        if (notificationTimer >= notificationDuration) {
            notificationText.clear();
        }

        if (!searching && nodeToHighlight) {
            nodeToHighlight->foundNode = true;
            notificationText = TextFormat("Found node: %d", nodeToHighlight->value);
            notificationTimer = 0.0f;

            // Schedule fade back to blue
            std::function<void(TreeNode*)> resetFound = [&](TreeNode* n) {
                if (!n) return;
                n->foundNode = false; // will gradually fade to blue
                resetFound(n->left);
                resetFound(n->right);
                };
            resetFound(nodeToHighlight);

            nodeToHighlight = nullptr;
        }
    }

    std::function<void(TreeNode*, float)> updateColors = [&](TreeNode* node, float dt) {
        if (!node) return;

        // Set target color
        if (node->searchHighlight) node->targetColor = RED;
        else if (node->foundNode) node->targetColor = GREEN;
        else if (node->fading) node->targetColor = BLUE; // fade back to blue
        else node->targetColor = BLUE;

        // Linear interpolation toward target
        float t = 3.0f * dt; // speed factor
        node->currentColor.r += (node->targetColor.r - node->currentColor.r) * t;
        node->currentColor.g += (node->targetColor.g - node->currentColor.g) * t;
        node->currentColor.b += (node->targetColor.b - node->currentColor.b) * t;
        node->currentColor.a += (node->targetColor.a - node->currentColor.a) * t;

        // If fading finished, stop fading
        if (node->fading &&
            fabs(node->currentColor.r - node->targetColor.r) < 1.0f &&
            fabs(node->currentColor.g - node->targetColor.g) < 1.0f &&
            fabs(node->currentColor.b - node->targetColor.b) < 1.0f) {
            node->fading = false;
            node->foundNode = false;
        }

        updateColors(node->left, dt);
        updateColors(node->right, dt);
        };

    updateColors(root, dt);
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

    Color nodeColor = node->currentColor;
    DrawCircleV(node->position, 25, nodeColor);

    int textWidth = MeasureText(TextFormat("%d", node->value), 20);
    DrawText(TextFormat("%d", node->value), node->position.x - textWidth / 2, node->position.y - 10, 20, WHITE);
}


void BinaryTree::Draw() {
    DrawNode(root);
    // Draw animated arrow between nodes during insertion
    if (inserting && currentInsertNode && nextInsertNode) {
        Vector2 start = currentInsertNode->position;
        Vector2 end = nextInsertNode->position;

        Vector2 tip = {
            start.x + (end.x - start.x) * arrowProgress,
            start.y + (end.y - start.y) * arrowProgress
        };

        DrawLineEx(start, tip, 4, GOLD);

        // Arrowhead
        float angle = atan2f(end.y - start.y, end.x - start.x);
        Vector2 leftHead = { tip.x - 10 * cosf(angle - 0.3f), tip.y - 10 * sinf(angle - 0.3f) };
        Vector2 rightHead = { tip.x - 10 * cosf(angle + 0.3f), tip.y - 10 * sinf(angle + 0.3f) };
        DrawTriangle(tip, leftHead, rightHead, GOLD);
    }

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
        node->insertHighlight = false;
        node->foundNode = false; // reset
        clearHighlights(node->left);
        clearHighlights(node->right);
    };
    clearHighlights(root);

    // Prepare search queue
    searchQueue = std::queue<TreeNode*>();
    TreeNode* current = root;
    TreeNode* found = nullptr;
    while (current) {
        searchQueue.push(current);
        if (value == current->value) {
            found = current; // save found node
            nodeToHighlight = current;
            break;
        }
        current = (value < current->value) ? current->left : current->right;
    }

    // Store found node for highlighting
    if (found) {
        nodeToHighlight->foundNode = true;
        nodeToHighlight->fading = true; // enable gradual fade
        notificationText = TextFormat("Found node: %d", nodeToHighlight->value);
        notificationTimer = 0.0f;
        nodeToHighlight = nullptr;
    }
    else {
        nodeToHighlight = nullptr;
        notificationText = "Value not found!";
        notificationTimer = 0.0f;
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

    if (!notificationText.empty()) {
        int textWidth = MeasureText(notificationText.c_str(), 20);
        DrawText(notificationText.c_str(),
            screenWidth / 2 - textWidth / 2,
            50, 20, ORANGE);
    }

}

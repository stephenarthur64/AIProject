#pragma once
#include "raylib.h"
#include <vector>
#include <queue>
#include <string>
#include <functional>

struct TreeNode {
    int value;
    TreeNode* left;
    TreeNode* right;
    Vector2 position;
    Vector2 targetPosition;
    bool animating;
    bool positioned;

    bool searchHighlight = false;
    bool insertHighlight = false;
    bool foundNode = false; // NEW: indicates node was found

    bool fading = false;           // true when green→blue fade
    Color currentColor = BLUE;    // smooth transition
    Color targetColor = BLUE;     // the color we are interpolating toward
    float fadeTimer = 0.0f;
};



class BinaryTree {
public:
    BinaryTree(int screenWidth, int screenHeight);

    void Insert(int value);
    void UpdateAnimations();
    void Draw();

    // Binary tree specific UI
    void DrawUI();               // Draw input boxes + search button
    void HandleInput();          // Handle input for search & insert

private:
    TreeNode* root;
    int screenWidth, screenHeight;
    std::vector<TreeNode*> animatingNodes;

    // Search animation
    std::queue<TreeNode*> searchQueue;
    bool searching = false;
    int searchTarget = 0;
    float searchDelay = 0.5f; // seconds between highlighting nodes
    float searchTimer = 0.0f;

    // UI state
    std::string inputValue;
    std::string searchValue;
    bool activeInput = false;
    bool activeSearch = false;
    Rectangle valueBox;
    Rectangle searchBox;
    Rectangle insertBtn;
    Rectangle searchBtn;

    // Animated insertion traversal
    bool inserting = false;
    int insertValue = 0;
    std::vector<TreeNode*> insertPath;
    int insertStep = 0;
    float insertTimer = 0.0f;
    float insertDelay = 0.8f;

    TreeNode* currentInsertNode = nullptr;
    TreeNode* nextInsertNode = nullptr;
    float arrowProgress = 0.0f;

    std::string notificationText;
    float notificationTimer = 0.0f;   // display duration
    float notificationDuration = 2.0f; // 2 seconds

    TreeNode* nodeToHighlight = nullptr; // node to highlight after search

    void DrawNode(TreeNode* node);
    void UpdateNode(TreeNode* node, float dt);
    void ComputeNodePositions(TreeNode* node, int depth, int xMin, int xMax);

    void Search(int value);
};

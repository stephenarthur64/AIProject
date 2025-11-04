#include "raylib.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <algorithm>


const int screenWidth = 1000;
const int screenHeight = 400;

struct Node {
    int value;
    Node* next;
    float x, y;

    Node(int v, float xpos, float ypos) : value(v), next(nullptr), x(xpos), y(ypos) {}
};

struct AnimatedNode {
    Node* node;
    float targetY;

    AnimatedNode(Node* n, float targetY) : node(n), targetY(targetY) {}
};

struct AnimatedPointer {
    Vector2 start;
    Vector2 end;
    Vector2 current;
    bool active = true;
    float progress = 0.0f;
    float speed = 0.05f;

    AnimatedPointer(Vector2 s, Vector2 e) : start(s), end(e), current(s) {}

    void Update() {
        if (!active) return;
        progress += speed;
        if (progress >= 1.0f) {
            progress = 1.0f;
            active = false;
        }
        current.x = start.x + (end.x - start.x) * progress;
        current.y = start.y + (end.y - start.y) * progress;
    }

    void Draw() {
        if (progress > 0.0f) {
            DrawLineEx(start, current, 2, DARKGRAY);
            if (!active) {
                DrawTriangle(
                    end,
                    { end.x - 10, end.y - 5 },
                    { end.x - 10, end.y + 5 },
                    DARKGRAY
                );
            }
        }
    }

    bool IsFinished() const {
        return !active;
    }
};

class LinkedList {
public:
    Node* head;
    std::vector<Node*> nodes;
    std::vector<AnimatedNode> animatedNodes;
    std::vector<AnimatedPointer> animatedPointers;

    LinkedList() : head(nullptr) {}

    void AddNode(int value) {
        float startX = 50.0f;
        float y = screenHeight / 2.0f;
        float spacing = 120.0f;

        Node* newNode = new Node(value, startX + nodes.size() * spacing, -50);
        animatedNodes.push_back(AnimatedNode(newNode, y));
        nodes.push_back(newNode);
    }

    void InsertNodeAt(int index, int value) {
        if (index < 0 || index > nodes.size()) return;

        float startX = 50.0f;
        float spacing = 120.0f;
        float targetY = screenHeight / 2.0f;

        Node* newNode = new Node(value, startX + index * spacing, -50);
        animatedNodes.push_back(AnimatedNode(newNode, targetY));
        nodes.insert(nodes.begin() + index, newNode);

        // Shift nodes after insertion
        for (int i = index + 1; i < nodes.size(); ++i) {
            nodes[i]->x += spacing;
        }
    }

    void DeleteLastNode() {
        if (nodes.empty()) return;

        Node* last = nodes.back();
        nodes.pop_back();
        delete last;
        if (nodes.empty()) head = nullptr;
        else nodes.back()->next = nullptr;
    }

    void UpdateLinks() {
        head = nodes.empty() ? nullptr : nodes[0];
        for (int i = 0; i < nodes.size(); ++i) {
            if (i < nodes.size() - 1) {
                nodes[i]->next = nodes[i + 1];

                // Create animated pointer
                Vector2 start = { nodes[i]->x + 80, nodes[i]->y + 20 };
                Vector2 end = { nodes[i + 1]->x, nodes[i + 1]->y + 20 };
                animatedPointers.push_back(AnimatedPointer(start, end));
            }
            else {
                nodes[i]->next = nullptr;
            }
        }
    }

    void UpdateAnimations() {
        const float speed = 5.0f;

        for (auto& anim : animatedNodes) {
            if (anim.node->y < anim.targetY) {
                anim.node->y += speed;
                if (anim.node->y > anim.targetY) anim.node->y = anim.targetY;
            }
        }

        animatedNodes.erase(
            std::remove_if(animatedNodes.begin(), animatedNodes.end(),
                [this](const AnimatedNode& anim) {
                    if (anim.node->y >= anim.targetY) {
                        UpdateLinks();
                        return true;
                    }
                    return false;
                }),
            animatedNodes.end()
        );

        for (auto& pointer : animatedPointers) {
            pointer.Update();
        }

        animatedPointers.erase(
            std::remove_if(animatedPointers.begin(), animatedPointers.end(),
                [](const AnimatedPointer& p) { return p.IsFinished(); }),
            animatedPointers.end()
        );
    }

    void Draw() {
        for (Node* node : nodes) {
            DrawRectangle(node->x, node->y, 80, 40, SKYBLUE);
            DrawRectangleLines(node->x, node->y, 80, 40, DARKBLUE);

            DrawText(std::to_string(node->value).c_str(), node->x + 20, node->y + 10, 20, BLACK);
        }

        for (auto& pointer : animatedPointers) {
            pointer.Draw();
        }

        // Draw full arrows for completed links
        // Only draw full arrows if no animated pointers are active
        if (animatedPointers.empty()) {
            for (Node* node : nodes) {
                if (node->next) {
                    float startX = node->x + 80;
                    float startY = node->y + 20;
                    float endX = node->next->x;
                    float endY = node->next->y + 20;
                    DrawLineEx({ startX, startY }, { endX, endY }, 2, DARKGRAY);
                    DrawTriangle(
                        { endX, endY },
                        { endX - 10, endY - 5 },
                        { endX - 10, endY + 5 },
                        DARKGRAY
                    );
                }
            }
        }

    }

    ~LinkedList() {
        for (Node* node : nodes) {
            delete node;
        }
    }
};

// Utility: check if mouse clicked within a rectangle
bool IsMouseClicked(Rectangle rect) {
    return CheckCollisionPointRec(GetMousePosition(), rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// Utility: draw button and return if it was clicked
bool Button(Rectangle rect, const char* label) {
    DrawRectangleRec(rect, LIGHTGRAY);
    DrawRectangleLinesEx(rect, 2, DARKGRAY);
    int textWidth = MeasureText(label, 20);
    DrawText(label, rect.x + (rect.width - textWidth) / 2, rect.y + 5, 20, BLACK);
    return IsMouseClicked(rect);
}

// Utility: simple integer string input field
void DrawInputBox(Rectangle rect, std::string& input, bool active) {
    DrawRectangleRec(rect, active ? RAYWHITE : LIGHTGRAY);
    DrawRectangleLinesEx(rect, 2, GRAY);
    DrawText(input.c_str(), rect.x + 5, rect.y + 5, 20, BLACK);
}

int main() {
    InitWindow(screenWidth, screenHeight, "Linked List Visualizer (Animated Pointers)");
    SetTargetFPS(60);
    srand((unsigned int)time(nullptr));

    LinkedList list;

    std::string inputValue, inputIndex;
    bool activeValueBox = false, activeIndexBox = false;

    while (!WindowShouldClose()) {
        // Input
        if (activeValueBox || activeIndexBox) {
            int key = GetCharPressed();
            while (key > 0) {
                if (isdigit(key)) {
                    if (activeValueBox) inputValue += (char)key;
                    else if (activeIndexBox) inputIndex += (char)key;
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE)) {
                if (activeValueBox && !inputValue.empty()) inputValue.pop_back();
                else if (activeIndexBox && !inputIndex.empty()) inputIndex.pop_back();
            }
        }

        // UI
        Rectangle valueBox = { 30, 60, 140, 35 };
        Rectangle indexBox = { 200, 60, 140, 35 };
        Rectangle insertBeforeBtn = { 370, 60, 140, 35 };
        Rectangle insertAfterBtn = { 530, 60, 140, 35 };
        Rectangle addEndBtn = { 690, 60, 100, 35 };
        Rectangle deleteLastBtn = { 810, 60, 140, 35 };

        if (IsMouseClicked(valueBox)) { activeValueBox = true; activeIndexBox = false; }
        else if (IsMouseClicked(indexBox)) { activeIndexBox = true; activeValueBox = false; }
        else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { activeValueBox = false; activeIndexBox = false; }

        if (Button(insertBeforeBtn, "Insert Before")) {
            if (!inputValue.empty() && !inputIndex.empty()) {
                list.InsertNodeAt(std::stoi(inputIndex), std::stoi(inputValue));
            }
        }

        if (Button(insertAfterBtn, "Insert After")) {
            if (!inputValue.empty() && !inputIndex.empty()) {
                list.InsertNodeAt(std::stoi(inputIndex) + 1, std::stoi(inputValue));
            }
        }

        if (Button(addEndBtn, "Add End")) {
            if (!inputValue.empty()) {
                list.AddNode(std::stoi(inputValue));
            }
        }

        if (Button(deleteLastBtn, "Delete Last")) {
            list.DeleteLastNode();
        }

        list.UpdateAnimations();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Enter Value:", valueBox.x, valueBox.y - 20, 18, DARKGRAY);
        DrawInputBox(valueBox, inputValue, activeValueBox);

        DrawText("Enter Index:", indexBox.x, indexBox.y - 20, 18, DARKGRAY);
        DrawInputBox(indexBox, inputIndex, activeIndexBox);

        list.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

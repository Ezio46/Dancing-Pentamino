#include <iostream>
#include <vector>

using namespace std;

struct Node {
    int row, col;
    Node* left, * right, * up, * down;
    Node(int r = 0, int c = 0) : row(r), col(c), left(this), right(this), up(this), down(this) {}
};

class DancingLinks {
public:
    DancingLinks(int rows, int cols) : rowCount(rows), colCount(cols), header(new Node()) {
        columns.resize(cols + 1);
        for (int i = 0; i <= cols; ++i) {
            columns[i] = new Node(0, i);
            if (i > 0) {
                columns[i - 1]->right = columns[i];
                columns[i]->left = columns[i - 1];
            }
        }
        columns[cols]->right = header;
        header->left = columns[cols];
    }

    ~DancingLinks() {
        for (Node* col : columns) {
            Node* node = col->down;
            while (node != col) {
                Node* tmp = node->down;
                delete node;
                node = tmp;
            }
            delete col;
        }
        delete header;
    }

    void addRow(const vector<int>& row) {
        Node* rowStart = nullptr;
        Node* last = nullptr;
        for (int col : row) {
            Node* node = new Node(rowID, col);
            if (rowStart == nullptr) rowStart = node;
            if (last != nullptr) {
                last->right = node;
                node->left = last;
            }
            node->up = columns[col]->up;
            node->down = columns[col];
            columns[col]->up->down = node;
            columns[col]->up = node;
            last = node;
        }
        if (last) {
            last->right = rowStart;
            rowStart->left = last;
        }
        ++rowID;
    }

    bool solve() {
        if (header->right == header) {
            return true;
        }
        Node* col = selectColumn();
        cover(col);
        for (Node* row = col->down; row != col; row = row->down) {
            for (Node* node = row->right; node != row; node = node->right) {
                cover(columns[node->col]);
            }
            if (solve()) {
                return true;
            }
            for (Node* node = row->left; node != row; node = node->left) {
                uncover(columns[node->col]);
            }
        }
        uncover(col);
        return false;
    }

private:
    int rowCount, colCount;
    int rowID = 1;
    Node* header;
    vector<Node*> columns;

    void cover(Node* col) {
        col->left->right = col->right;
        col->right->left = col->left;
        for (Node* row = col->down; row != col; row = row->down) {
            for (Node* node = row->right; node != row; node = node->right) {
                node->up->down = node->down;
                node->down->up = node->up;
            }
        }
    }

    void uncover(Node* col) {
        for (Node* row = col->up; row != col; row = row->up) {
            for (Node* node = row->left; node != row; node = node->left) {
                node->down->up = node;
                node->up->down = node;
            }
        }
        col->left->right = col;
        col->right->left = col;
    }

    Node* selectColumn() {
        return header->right; 
    }
};

vector<vector<vector<int>>> pentominoes = {
    // X-Pentomino
    {{0, 1, 2, 3, 4}},
    // I-Pentomino
    {{0, 5, 10, 15, 20}, {0, 1, 2, 3, 4}},
    // L-Pentomino
    {{0, 5, 10, 15, 16}, {0, 1, 2, 3, 8}, {0, 1, 6, 11, 16}, {0, 5, 6, 7, 8}},
    // N-Pentomino
    {{0, 1, 6, 11, 12}, {0, 5, 10, 11, 12}, {0, 1, 2, 7, 12}, {0, 5, 6, 7, 12}},
    // P-Pentomino
    {{0, 5, 6, 7, 12}, {0, 5, 6, 11, 12}},
    // T-Pentomino
    {{0, 5, 10, 11, 12}, {0, 1, 2, 6, 12}, {0, 1, 2, 7, 12}, {0, 5, 6, 7, 12}},
    // U-Pentomino
    {{0, 5, 6, 10, 11}, {0, 1, 6, 7, 12}},
    // V-Pentomino
    {{0, 5, 10, 11, 12}, {0, 1, 2, 6, 7}, {0, 1, 2, 7, 12}, {0, 5, 6, 11, 12}},
    // W-Pentomino
    {{0, 1, 6, 7, 12}, {0, 5, 6, 11, 12}},
    // Y-Pentomino
    {{0, 5, 10, 11, 12}, {0, 5, 6, 11, 16}, {0, 1, 2, 7, 12}, {0, 5, 6, 7, 12}},
    // Z-Pentomino
    {{0, 5, 10, 15, 16}, {0, 1, 2, 7, 12}, {0, 5, 10, 11, 12}, {0, 5, 6, 11, 16}},
    // F-Pentomino
    {{0, 1, 6, 11, 12}, {0, 5, 6, 7, 12}, {0, 1, 6, 7, 12}, {0, 5, 6, 11, 16}}
};

void addPentominoes(DancingLinks& dlx, int gridWidth, int gridHeight) {
    int gridSize = gridWidth * gridHeight;
    for (int i = 0; i < pentominoes.size(); ++i) {
        for (const auto& shape : pentominoes[i]) {
            for (int row = 0; row < gridHeight; ++row) {
                for (int col = 0; col < gridWidth; ++col) {
                    vector<int> positions;
                    bool valid = true;
                    for (int pos : shape) {
                        int r = row + pos / 5;
                        int c = col + pos % 5;
                        if (r >= gridHeight || c >= gridWidth) {
                            valid = false;
                            break;
                        }
                        positions.push_back(r * gridWidth + c);
                    }
                    if (valid) {
                        dlx.addRow(positions);
                    }
                }
            }
        }
    }
}

int main() {
    const int gridWidth = 10;
    const int gridHeight = 6;
    DancingLinks dlx(gridWidth * gridHeight, gridWidth * gridHeight);

    addPentominoes(dlx, gridWidth, gridHeight);

    if (dlx.solve()) {
        cout << "Solution found!" << endl;
    }
    else {
        cout << "No solution exists!" << endl;
    }

    return 0;
}

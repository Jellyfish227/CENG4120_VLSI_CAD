#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

struct Coordinates {
    pair<int, int> lowerLeft;
    pair<int, int> lowerRight;
    pair<int, int> upperLeft;
    pair<int, int> upperRight;
    Coordinates(
        pair<int, int> lowerLeft, 
        pair<int, int> lowerRight, 
        pair<int, int> upperLeft, 
        pair<int, int> upperRight
    ) : lowerLeft(lowerLeft), lowerRight(lowerRight), upperLeft(upperLeft), upperRight(upperRight) {}
    Coordinates(){}
};

enum NodeType {
    LEAF = 0,
    INTERNAL = 1
};

enum Rotation {
    NONE = -1,
    ORIGINAL = 0,
    ROTATED = 1
};

struct Block {
    vector<pair<int, int>> shape; // <width, height>
    Coordinates coordinates;
    Rotation orientation;
    Block(int w, int h){
        shape.push_back(make_pair(w, h));
        shape.push_back(make_pair(h, w));
    }
};

struct graphVertex {
    pair<int, int> vertex;
    Rotation orientation;
    Rotation lcSign;
    Rotation rcSign;
    graphVertex(int w, int h, Rotation o) : orientation(o) {
        vertex = make_pair(w, h);
    }
    graphVertex(int w, int h, Rotation lcSign, Rotation rcSign) : lcSign(lcSign), rcSign(rcSign) {
        vertex = make_pair(w, h);
    }
    graphVertex(){}
};

struct Node {
    NodeType type;
    char value;
    Block* block;
    vector<graphVertex> graphVertices; 
    graphVertex minShape;
    Node *left;
    Node* right;
    Node(char v) : value(v), block(nullptr), left(nullptr), right(nullptr) {}
};

Node* buildTree(stack<Node*>& npe_stack, vector<Block>& blocks) {
    Node* root = npe_stack.top();
    npe_stack.pop();
    if (isdigit(root->value)) {
        root->block = &blocks.at(int(root->value) - '0');
        root->type = NodeType::LEAF;
    }
    if (!isdigit(root->value)) {
        root->type = NodeType::INTERNAL;
        root->right = buildTree(npe_stack, blocks);
        root->left = buildTree(npe_stack, blocks);
    }
    return root;
}

void compute_shape_curve(Node* root) {
    if (root == nullptr) return;

    compute_shape_curve(root->left);
    compute_shape_curve(root->right);

    // leaf node case
    if (root->type == NodeType::LEAF) {
        root->graphVertices.push_back(graphVertex(root->block->shape.at(Rotation::ORIGINAL).first, root->block->shape.at(Rotation::ORIGINAL).second, Rotation::ORIGINAL));
        root->graphVertices.push_back(graphVertex(root->block->shape.at(Rotation::ROTATED).first, root->block->shape.at(Rotation::ROTATED).second, Rotation::ROTATED));
    }
    else // non-leaf node case
    {
        vector<graphVertex> leftVertices = root->left->graphVertices;
        vector<graphVertex> rightVertices = root->right->graphVertices;
        // compute the graph vertices according to the slicing function
        vector<graphVertex> candidates;

        for (auto& vLeft : leftVertices) {
            for (auto& vRight : rightVertices) {
                if (root->value == '+') {
                    candidates.emplace_back( // max x, sum y
                        max(vLeft.vertex.first, vRight.vertex.first), 
                        vLeft.vertex.second + vRight.vertex.second, 
                        vLeft.orientation,
                        vRight.orientation
                    );
                }
                else if (root->value == '*') {
                    candidates.emplace_back( // sum x, max y
                        vLeft.vertex.first + vRight.vertex.first, 
                        max(vLeft.vertex.second, vRight.vertex.second), 
                        vLeft.orientation,
                        vRight.orientation
                    );
                }
            }
        }

        // sort the candidates by width ascending, then height ascending
        sort(candidates.begin(), candidates.end(), [](const graphVertex& a, const graphVertex& b) {
            return (a.vertex.first == b.vertex.first) ? (a.vertex.second < b.vertex.second) : (a.vertex.first < b.vertex.first);
        });

        vector<graphVertex> filteredVertices;
        if (!candidates.empty()) {
            filteredVertices.push_back(candidates.at(0));
            int minHeight = candidates.at(0).vertex.second;
            for (auto& v : candidates) {
                if (v.vertex.second < minHeight) {
                    filteredVertices.push_back(v);
                    minHeight = v.vertex.second;
                }
            }
        }
        root->graphVertices = filteredVertices;
        root->minShape = *std::min_element(
            root->graphVertices.begin(),
            root->graphVertices.end(),
            [](const auto& a, const auto& b) {
                return (a.vertex.first * a.vertex.second) < (b.vertex.first * b.vertex.second);
            }
        );
    }
}

void set_shape(Node* root) {
    if (root == nullptr) return;

    if (root->type == NodeType::LEAF) {
        root->block->orientation = root->minShape.orientation;
    }
    else {
        root->left->block->orientation = root->minShape.lcSign;
        root->right->block->orientation = root->minShape.rcSign;
        set_shape(root->left);
        set_shape(root->right);
    }
}

void compute_coordinates(Node* root, int x = 0, int y = 0) {
    int bWidth = root->minShape.vertex.first;
    int bHeight = root->minShape.vertex.second;
    if (root->type == NodeType::LEAF) {
        int modWidth = root->block->shape.at(root->block->orientation).first;
        int modHeight = root->block->shape.at(root->block->orientation).second;
        root->block->coordinates 
        = Coordinates(
            {x, y}, 
            {x + modWidth, y}, 
            {x, y + modHeight}, 
            {x + modWidth, y + modHeight}
        );
    }
    else {
        int lWidth = root->left->minShape.vertex.first;
        int lHeight = root->left->minShape.vertex.second;
        int rWidth = root->right->minShape.vertex.first;
        int rHeight = root->right->minShape.vertex.second;
        if (root->value == '+') {
            compute_coordinates(root->left, x, y);
            compute_coordinates(root->right, x, y + lHeight);
        }
        else {
            compute_coordinates(root->left, x, y);
            compute_coordinates(root->right, x + lWidth, y);
        }
    }
    
}

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3)
    {
        printf("usage: hw2 <in_file> <out_file>, %s is received", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *inFile = argv[1];
    char *outFile = argv[2];

    ifstream input(inFile);
    if (!input.is_open())
    {
        printf("Failed to open input file: %s\n", inFile);
        exit(EXIT_FAILURE);
    }
    ofstream output(outFile);
    if (!output.is_open())
    {
        printf("Failed to create/open output file: %s\n", outFile);
        exit(EXIT_FAILURE);
    }
    
    // Read number of blocks
    vector<Block> blocks;
    int n;
    input >> n;
    
    // Read each block's dimensions
    for(int i = 0; i < n; i++) {
        int width, height;
        if(input >> width >> height) {
            blocks.emplace_back(width, height);
        } else {
            cerr << "Error reading block " << i + 1 << endl;
            break;
        }
    }

    // Read Normalized Polish Expression
    stack<Node*> npe_stack;
    char c;
    int cCount = 0;
    int oCount = 0;
    while (input >> c) {
        if (isdigit(c)) {
            cCount++;
            npe_stack.push(new Node(c));
        } else {
            oCount++;
            if (!(oCount < cCount)) {
                cerr << "Invalid NPE" << endl;
                exit(EXIT_FAILURE);
            }
            npe_stack.push(new Node(c));
        }
    }
    // Build the slicing tree
    Node* slicingTree = buildTree(npe_stack, blocks);
    
    // recursively initialize the shape curve of slicing tree
    compute_shape_curve(slicingTree);

    //TODO: Retrieve the coordinates of the blocks
    compute_coordinates(slicingTree);

    // Output the block coordinates
    for (int i = 0; i < n; i++) {
        Block& block = blocks.at(i);
        output << "(" << block.coordinates.lowerLeft.first << " " << block.coordinates.lowerLeft.second << ")" << " "
               << "(" << block.coordinates.lowerRight.first << " " << block.coordinates.lowerRight.second << ")" << " "
               << "(" << block.coordinates.upperLeft.first << " " << block.coordinates.upperLeft.second << ")" << " "
               << "(" << block.coordinates.upperRight.first << " " << block.coordinates.upperRight.second << ")" << endl;
    }

    //TODO: Calculate the area of the slicing floorplan
    int minArea = slicingTree->minShape.vertex.first * slicingTree->minShape.vertex.second;

    // Output results
    output << minArea << endl;

    input.close();
    output.close();
    return EXIT_SUCCESS;
}
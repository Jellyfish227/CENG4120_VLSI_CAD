#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct Block {
    int width;
    int height;
    Block(int w, int h) : width(w), height(h) {}
};

struct Node {
    char value;
    Block* block;
    Node *left;
    Node* right;
    Node(char v) : value(v), left(nullptr), right(nullptr), block(nullptr) {}
};

Node* buildTree(stack<Node*>& npe_stack, vector<Block>& blocks) {
    Node* root = npe_stack.top();
    npe_stack.pop();
    if (isdigit(root->value)) root->block = &blocks.at(int(root->value) - '0');
    if (!isdigit(root->value)) {
        root->right = buildTree(npe_stack, blocks);
        root->left = buildTree(npe_stack, blocks);
    }
    return root;
}

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3)
    {
        printf("usage: hw2 <in_file> <out_file>");
        exit(1);
    }
    char *inFile = argv[1];
    char *outFile = argv[2];

    ifstream input(inFile);
    if (!input.is_open())
    {
        printf("Failed to open input file: %s\n", inFile);
        exit(1);
    }
    ofstream output(outFile);
    if (!output.is_open())
    {
        printf("Failed to create/open output file: %s\n", outFile);
        exit(1);
    }

    vector<Block> blocks;
    
    // Read number of blocks
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
                exit(1);
            }
            npe_stack.push(new Node(c));
        }
    }
    // Build the slicing tree
    Node* slicingTree = buildTree(npe_stack, blocks);

    //TODO: Retrieve the coordinates of the blocks


    //TODO: Calculate the area of the slicing floorplan

    input.close();
    output.close();
    return 0;
}
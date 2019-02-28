#include <iostream>
#include <windows.h> //sleep
using namespace std;

struct Tree {
    int number;
    int leftWeight;
    int rightWeight;
    Tree *right;
    Tree *left;
};

Tree *leftRotation(Tree *temp) {
    Tree *temp1, *temp2;

    temp1 = temp->right;
    temp2 = temp1->left;
    temp->right = temp2;
    temp1->left = temp;

    //recount the right weight of the original root
    if(temp->right == NULL)
        temp->rightWeight = 0;
    //set the current side weight as the highest weight (or height) summed up of 1
    else if(temp->right->leftWeight > temp->right->rightWeight)
        temp->rightWeight = temp->right->leftWeight + 1;
    else
        temp->rightWeight = temp->right->rightWeight + 1;

    //recount the left weight of the new root
    if(temp1->left->leftWeight > temp1->left->rightWeight)
        temp1->leftWeight = temp1->left->leftWeight + 1;
    else
        temp1->leftWeight = temp1->left->rightWeight + 1;

    //return the new root
    return temp1;
}

Tree *rightRotation(Tree *temp) {
    Tree *temp1, *temp2;

    temp1 = temp->left;
    temp2 = temp1->right;
    temp->left = temp2;
    temp1->right = temp;

    //recount the left weight of the original root
    if(temp->left == NULL)
        temp->leftWeight = 0;
    else if(temp->left->leftWeight > temp->left->rightWeight)
        temp->leftWeight = temp->left->leftWeight + 1;
    else
        temp->leftWeight = temp->left->rightWeight + 1;

    //recount the right weight of the new root
    if(temp1->right->leftWeight > temp1->right->rightWeight)
        temp1->rightWeight = temp1->right->leftWeight + 1;
    else
        temp1->rightWeight = temp1->right->rightWeight + 1;

    return temp1;
}

Tree *balance(Tree *temp, int weight) {
    int childWeight;

    if(weight == 2) {
        //total weight of the right child of the root
        childWeight = temp->right->rightWeight - temp->right->leftWeight;

        if(childWeight >= 0)
            temp = leftRotation(temp);
        else {
            temp->right = rightRotation(temp->right);
            temp = leftRotation(temp);
        }
    }
    else if(weight == -2) {
        //total weight of the left child of the root
        childWeight = temp->left->rightWeight - temp->left->leftWeight;

        if(childWeight <= 0)
            temp = rightRotation(temp);
        else {
            temp->left = leftRotation(temp->left);
            temp = rightRotation(temp);
        }
    }

    //return the balanced tree
    return temp;
}

Tree *add(Tree *temp, int number) {
    int weight;
    Tree *newRoot;

    if(temp == NULL) {
        newRoot = new Tree();

        newRoot->number = number;
        newRoot->leftWeight = 0; 
        newRoot->rightWeight = 0;
        newRoot->left = NULL;
        newRoot->right = NULL;

        temp = newRoot;
    }
    else if(number < temp->number) {
        temp->left = add(temp->left, number);

        //reset the weight of the current root on the recursion
        if(temp->left->leftWeight > temp->left->rightWeight)
            temp->leftWeight = temp->left->leftWeight + 1;
        else
            temp->leftWeight = temp->left->rightWeight + 1;

        //balance the current root
        weight = (temp->rightWeight - temp->leftWeight);
        if(weight == 2 || weight == -2)
            temp = balance(temp, weight);
    }
    else {
        temp->right = add(temp->right, number);

        if(temp->right->leftWeight > temp->right->rightWeight)
            temp->rightWeight = temp->right->leftWeight + 1;
        else
            temp->rightWeight = temp->right->rightWeight + 1;

        weight = (temp->rightWeight - temp->leftWeight);
        if(weight == 2 || weight == -2)
            temp = balance(temp, weight);
    }

    return temp;
}

Tree *remove(Tree *temp, int number) {
    Tree *temp1, *temp2;

    if(temp->number == number) {
        //if the node to be removed is a leaf
        if(temp->left == temp->right) {
            delete(temp);

            return NULL;
        }
        else if(temp->left == NULL) {
            //save the non-null child
            temp1 = temp->right;
            delete(temp);

            return temp1;
        }
        else if(temp->right == NULL) {
            temp1 = temp->left;
            delete(temp);
            
            return temp1;
        }
        //if the node to be removed has children on both sides
        else {
            temp2 = temp->right;
            temp1 = temp->left;

            while(temp1->left != NULL)
                temp1 = temp1->left;

            temp1->left = temp->left;
            delete(temp);

            return temp2;
        }
    }
    else if(number < temp->number)
        temp->left = remove(temp->left, number);
    else
        temp->right = remove(temp->right, number);

    return temp;
}

//function to update the weights of the tree after a removal
Tree *updateWeights(Tree *temp) {
    if(temp != NULL) {
        //do the treatment on the left children
        temp->left = updateWeights(temp->left);

        if(temp->left == NULL)
            temp->leftWeight = 0;
        else if(temp->left->leftWeight > temp->left->rightWeight)
            temp->leftWeight = temp->left->leftWeight + 1;
        else
            temp->leftWeight = temp->left->rightWeight + 1;

        //do the treatment on the right children
        temp->right = updateWeights(temp->right);

        if(temp->right == NULL)
            temp->rightWeight = 0;
        else if(temp->right->leftWeight > temp->right->rightWeight)
            temp->rightWeight = temp->right->leftWeight + 1;
        else
            temp->rightWeight = temp->right->rightWeight + 1;

        //does the balancing
        temp = balance(temp, (temp->rightWeight - temp->leftWeight));
    }

    return temp;
}

//function to show the weight of each node at each iteration of the user
void showNodesWeights(Tree *temp) {
    if(temp != NULL) {
        cout << "Node: " << temp->number << endl;
        cout << "Weight: " << (temp->rightWeight - temp->leftWeight) << endl << endl;
        showNodesWeights(temp->left);
        showNodesWeights(temp->right);
    }
}

int heightCount;
void treeHeight(Tree *temp) {
    if(temp != NULL) {
        if(temp->left != NULL)
            treeHeight(temp->left);
        else if(temp->right != NULL)
            treeHeight(temp->right);

        heightCount++;
    }
}

int nodesCount;
void nodesQuantity(Tree *temp) {
    if(temp != NULL) {
        nodesQuantity(temp->left);
        nodesQuantity(temp->right);
        nodesCount++;
    }
}

void inOrder(Tree *temp) {
    if(temp != NULL) {
        inOrder(temp->left);
        cout << temp->number << " ";
        inOrder(temp->right);        
    }
}

void preOrder(Tree *temp) {
    if(temp != NULL) {
        cout << temp->number << " ";
        preOrder(temp->left);
        preOrder(temp->right);
    }
}

void postOrder(Tree *temp) {
    if(temp != NULL) {
        postOrder(temp->left);
        postOrder(temp->right);
        cout << temp->number << " ";
    }
}

void showTree(Tree *root) {
    int type, choice;

    system("cls");
    cout << "In which order do you want it printed?" << endl;
    cout << "<1> In order\t<2> Pre order\t<3> Post order" << endl << endl;
    cin >> type;

    system("cls");
    switch(type) {
        //in order
        case 1:
            inOrder(root);
            cout << endl << endl << "<1> Back to main menu" << endl << endl;
            cin >> choice;
            if(choice == 1)
                break;
        
        case 2:
            preOrder(root);
            cout << endl << endl << "<1> Back to main menu" << endl << endl;
            cin >> choice;
            if(choice == 1)
                break;
        
        case 3:
            postOrder(root);
            cout << endl << endl << "<1> Back to main menu" << endl << endl;
            cin >> choice;
            if(choice == 1)
                break;
    }        
}

int main(int argc, char const *argv[]) {
    int choice, number;
    bool exit = false;
    Tree *root = NULL;

    //clean the screen
    system("cls");
    //apresentation
    cout << "\tWelcome to the TREE MANAGER!";
    //freezes the screen for 3 seconds (3000 ms)
    Sleep(3000);
    system("cls");

    //main menu
    while(!exit) {
        system("cls");

        cout << "\tMENU" << endl << endl;
        cout << "<1> Insert\n<2> Remove\n<3> View tree\n<4> Show tree height\n<5> View nodes quantity\n<6> Exit" << endl << endl;
        cin >> choice;

        switch(choice) {
            case 1:
                system("cls");
                cout << "Add: ";
                cin >> number;
                root = add(root, number);
                system("cls");
                cout << "Success!" << endl << endl;
                showNodesWeights(root);
                cout << "<1> Back to main menu" << endl << endl;
                cin >> choice;
                if(choice == 1)
                    break;

            case 2:
                system("cls");
                cout << "Remove: ";
                cin >> number;
                root = remove(root, number);
                root = updateWeights(root);
                system("cls");
                cout << "Success!" << endl << endl;
                showNodesWeights(root);
                cout << "<1> Back to main menu" << endl << endl;
                cin >> choice;
                if(choice == 1)
                    break;

            case 3:
                showTree(root);
                break;

            case 4:
                system("cls");
                heightCount = 0; //global
                treeHeight(root);
                cout << "Tree height: " << heightCount /*global*/ << endl << endl;
                cout << "<1> Back to main menu" << endl << endl;
                cin >> choice;
                if(choice == 1)
                    break;

            case 5:
                system("cls");
                nodesCount = 0; //global
                nodesQuantity(root);
                cout << "Nodes quantity: " << nodesCount /*global*/ << endl << endl;
                cout << "<1> Back to main menu" << endl << endl;
                cin >> choice;
                if(choice == 1)
                    break;

            case 6:
                system("cls");
                exit = true;
                break;
        }
    }
    
    system("pause");
    return 0;
}
/*
   Name- Nisha Sangawar
   Gnumber-G01512637
*/

#include <stdio.h>         // Standard library: Input/Output functions
#include <stdlib.h>        // Library: String handling functions
#include <string.h>        // Standard library: Memory management functions
#include <ctype.h>         // Library: Character handling functions

#define MAX_ALIAS_LEN 10      // Define maxLength the alias for the IP address
#define MAX_OCTETS 4
#define ERROR_LOG_FILE "CS531_error-log.txt"

// Structure for address node in BST
typedef struct address_t {
    int octet[MAX_OCTETS];                        // Store IP addresses
    char alias[MAX_ALIAS_LEN + 1];                // Store the alias for the IP address
    struct address_t *left, *right, *parent;      // Pointer to the parent node, right node and left node
    int height, depth;                            //  Height of the node and Depth of the node
} address_t;

address_t *root = NULL;                           // global variable

// Function declarations
int is_validIP(int octet[]);
int is_validFormat(const char *input);
void display_aliasesIP(char *ip_part);
void display_aliasesIP(char *ip_part);
void to_lowercase(char *alias);
int is_valid_alias(char *alias);
int duplicateIP(address_t *node, int octet[]);
int duplicateA(address_t *node, const char *alias);
void updateAddress();
void inorder_display(address_t *node);
void display_error_log();
void addAddress();
void display_loaction();
void display_aliasesIP(char *ip_part);
void display_matchingA(address_t *node, const char *locality);
void display_matchingIP(address_t *node, const char *ip_part);
void read_addresses(const char *filename);
void delete_address(char *alias);
void updateHeight(address_t *node);

address_t* create_node(int octet[], char *alias);
address_t* insert_node(address_t *root, int octet[], char *alias, int depth);
address_t* search_node(address_t *node, char *alias);
address_t* delete_node_helper(address_t *root, char *alias, int *success_flag);
int get_balance_factor(address_t *node);
address_t* rotateRight(address_t *node);
address_t* rotateLeft(address_t *node);
address_t* rotateLeftThenRight(address_t *node);
address_t* rotateRightThenLeft(address_t *node);
address_t* rebalance(address_t *node);

// Main menu function
void menu() {
    int choice;
    do {
        printf("Menu:\n");
        printf("1. Add address\n");
        printf("2. Look up address\n");
        printf("3. Update address\n");
        printf("4. Delete address\n");
        printf("5. Display list\n");
        printf("6. Display aliases for location\n");
        printf("7. Display Error Log\n");
        printf("8. Quit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar(); 

        switch (choice) {
            case 1:
                addAddress();
                break;
            case 2: {
                char alias[MAX_ALIAS_LEN + 1];
                printf("Enter alias to look up: ");
                scanf("%s", alias);
                to_lowercase(alias);
                address_t *node = search_node(root, alias);
                if (node) {
                    printf("%s: %d.%d.%d.%d\n", node->alias, node->octet[0], node->octet[1], node->octet[2], node->octet[3]);
                } else {
                    printf("Error: Alias not found.\n");
                }
                break;
            }
            case 3:
                updateAddress();
                break;
            case 4: {
   				 char alias[MAX_ALIAS_LEN + 1];
    			 printf("Enter alias to delete: ");
    		     scanf("%s", alias);
                 to_lowercase(alias);
                 delete_address(alias);  
                 break;
            }
            case 5:
                inorder_display(root);
                break;
            case 6:
                display_loaction();
                break;
            case 7:
                display_error_log();
                break;
            case 8:
                printf("Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 8);
}

int main() {
    read_addresses("CS531_Inet.txt");   // Read addresses from the file
    menu();                             // menu for user interaction
    return 0;
}

// Function to read and store in BST
void read_addresses(const char *filename) {
    FILE *file = fopen(filename, "r");
    FILE *log = fopen(ERROR_LOG_FILE, "w");  
    if (file == NULL) {
        printf("Error opening file %s.\n", filename);
        if (log != NULL) {
            fprintf(log, "Error opening file %s.\n", filename);
            fclose(log);
			}
        return;
    }

    if (log == NULL) {
        printf("Error opening error log file.\n");
        fclose(file);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        int octet[MAX_OCTETS];
        char alias[MAX_ALIAS_LEN + 1];
        
        int parsed = sscanf(line, "%d.%d.%d.%d %s", &octet[0], &octet[1], &octet[2], &octet[3], alias);
        if (parsed < 4) {
            fprintf(log, "Invalid format: %s", line);
            continue; }

        if (strlen(alias) > MAX_ALIAS_LEN) {    // Check alias length
            fprintf(log, "Invalid alias length: %s\n", alias);
            continue; }

        if (!is_validIP(octet)) {               // Check ip validity
            fprintf(log, "Invalid IPv4 address: %d.%d.%d.%d\n", octet[0], octet[1], octet[2], octet[3]);
            continue; 
			}

        if (!is_valid_alias(alias)) {          //Check for valid alias
            fprintf(log, "Invalid alias: %s\n", alias);
            continue; 
			}
        to_lowercase(alias);

        root = insert_node(root, octet, alias, 0); // Insert in BST
    }

    fclose(file);
    fclose(log);  
}

// IP validation function
int is_validFormat(const char *input) {
    int dot_count = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] == '.') {
            dot_count++;
        }
    }
    if (dot_count != 3) {           // There should be exactly 3 dots
        return 0;
    }
    int octet[MAX_OCTETS];
    if (sscanf(input, "%d.%d.%d.%d", &octet[0], &octet[1], &octet[2], &octet[3]) != 4) {
        return 0; 
    }
    for (int i = 0; i < MAX_OCTETS; i++) {
        if (octet[i] < 0 || octet[i] > 255) {
            return 0;            
        }
    }
    return 1; 
}

// Helper function to validate address
int is_validIP(int octet[]) {
    for (int i = 0; i < MAX_OCTETS; i++) {
        if (octet[i] < 0 || octet[i] > 255) {
            return 0; 
        }
    }
    return 1; 
}

// Helper function to validate alias
int is_valid_alias(char *alias) {
    if (strlen(alias) >= MAX_ALIAS_LEN) return 0; 
    for (int i = 0; alias[i]; i++) {
        if (!islower(alias[i])) return 0; 
    }
    return 1; 
}

// Convert alias to lowercase
void to_lowercase(char *alias) {
    for (int i = 0; alias[i]; i++) {
        alias[i] = tolower(alias[i]);
    }
}

// New address node
address_t* create_node(int octet[], char *alias) {
    address_t *node = (address_t *)malloc(sizeof(address_t));
    for (int i = 0; i < MAX_OCTETS; i++) {
        node->octet[i] = octet[i];
    }
    strncpy(node->alias, alias, MAX_ALIAS_LEN);
    node->alias[MAX_ALIAS_LEN] = '\0'; 
    node->left = node->right = node->parent = NULL;
    node->height = node->depth = 0;
    return node;
}

// Function to get the height of a node
int get_height(address_t *node) {
    if (node == NULL) return -1;           // Height of an empty tree is -1
    return node->height;
}

int get_balance_factor(address_t *node){
	if(root==NULL){
		return 0;
	}
	return get_height(node->left) - get_height(node->right);
	
}

// Insert a node in the BST
address_t* insert_node(address_t *node, int octet[], char *alias, int depth) {
    if (node == NULL) {
        address_t *new_node = create_node(octet, alias);
        new_node->depth = depth; 
        return new_node;
		}
    if (strcmp(alias, node->alias) < 0) {
        node->left = insert_node(node->left, octet, alias, depth + 1);
        node->left->parent = node;              // Set parent for the left child
    } else if (strcmp(alias, node->alias) > 0) {
        node->right = insert_node(node->right, octet, alias, depth + 1);
        node->right->parent = node;              // Set parent for the right child
    } else {
        printf("Error: Duplicate alias or address.\n");
    }
    updateHeight(node);     // Update the height of the current node
    
    int balance = get_balance_factor(node);
    
    // Perform rotations based on the balance factor
    if (balance > 1 && strcmp(alias, node->left->alias) < 0) {
        return rotateRight(node);  // Left-Left case
    }
    if (balance < -1 && strcmp(alias, node->right->alias) > 0) {
        return rotateLeft(node);   // Right-Right case
    }
    if (balance > 1 && strcmp(alias, node->left->alias) > 0) {
        return rotateLeftThenRight(node);  // Left-Right case
    }
    if (balance < -1 && strcmp(alias, node->right->alias) < 0) {
        return rotateRightThenLeft(node);  // Right-Left case
    }
    return node;
}

//Helper function for updating the height of bst
void updateHeight(address_t *node) {                      
    if (node != NULL) {
        int lH = get_height(node->left);
        int rH = get_height(node->right);
        node->height = 1 + (lH > rH ? lH : rH);
    }
}
//Rotate Right side 
address_t* rotateRight(address_t *node){
	address_t *leftChild= node->left;
	address_t *subNodeRight=leftChild->right;
	
	leftChild->right=node;
	node->left=subNodeRight;
	
	if (subNodeRight) subNodeRight->parent = node;
    leftChild->parent = node->parent;
    node->parent = leftChild;

    updateHeight(node);
    updateHeight(leftChild);

    return leftChild;	
	
}
// Left Rotation 
address_t* rotateLeft(address_t *node) {
    address_t *rightChild = node->right;          
    address_t *subNodeleft = rightChild->left;         
    
    rightChild->left = node;                     
    node->right = subNodeleft;                    
    
    if (subNodeleft) subNodeleft->parent = node;  // Update the parent of leftSubtree if it exists
    rightChild->parent = node->parent;            
    node->parent = rightChild;                    
    
    updateHeight(node);                           
    updateHeight(rightChild);                           
    
    return rightChild;                                 // Return the new root of the subtree
}

//LeftRight Rotation
address_t* rotateLeftThenRight(address_t *node) {
    
    node->left = rotateLeft(node->left);                  // Perform left rotation on the left child first
    
    
    return rotateRight(node);                            //Perform on rightRotation
}
//RightLeft Rotation
address_t* rotateRightThenLeft(address_t *node) {
    
    node->right = rotateRight(node->right);               // Perform right rotation on the right child first
    
   
    return rotateLeft(node);                             
}

address_t* rebalance(address_t *node) {
    updateHeight(node);
    int balance = get_balance_factor(node);

    if (balance > 1) {
        if (get_balance_factor(node->left) >= 0) {
            return rotateRight(node);  // Left-Left case
        } else {
            return rotateLeftThenRight(node);  // Left-Right case
        }
    } else if (balance < -1) {
        if (get_balance_factor(node->right) <= 0) {
            return rotateLeft(node);  // Right-Right case
        } else {
            return rotateRightThenLeft(node);  // Right-Left case
        }
    }

    return node;  // No balancing needed
}
// In-order traversal display list
void inorder_display(address_t *node) {
    if (node == NULL){
    inorder_display(node->left);
    printf("%s %d.%d.%d.%d height:%d depth:%d parent:%s\n",
           node->alias, node->octet[0], node->octet[1], node->octet[2], node->octet[3],
           node->height, node->depth,
           get_balance_factor(node),
           (node->parent != NULL) ? node->parent->alias : "NONE");
    inorder_display(node->right);
	}
}

// Look for node alias in the bst
address_t* search_node(address_t *node, char *alias) {
    if (node == NULL || strcmp(node->alias, alias) == 0) return node;

    if (strcmp(alias, node->alias) < 0) {
        return search_node(node->left, alias);
    } else {
        return search_node(node->right, alias);
    }
}

// Function to add an new ip address
void addAddress() {
    int octet[MAX_OCTETS];
    char alias[MAX_ALIAS_LEN + 1];
    char input[256];
    int ipDuplicate = 0;     // Declare ipDuplicate
    int aliasDuplicate = 0;  // Declare aliasDuplicate

    while (1) {
        printf("Enter IPv4 address (format: x.x.x.x): ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';
        
        if (is_validFormat(input) && sscanf(input, "%d.%d.%d.%d", &octet[0], &octet[1], &octet[2], &octet[3]) == 4) {
            break;
        }
        printf("Error: Invalid IPv4 address.\n");
    }
    
    printf("Enter alias: ");
    fgets(alias, sizeof(alias), stdin);
    alias[strcspn(alias, "\n")] = '\0';
    to_lowercase(alias);

    if (!is_valid_alias(alias)) {
        FILE *log = fopen(ERROR_LOG_FILE, "a");
        if (log != NULL) {
            fprintf(log, "Invalid alias: %s\n", alias);
            fclose(log);
        }
        printf("Error: Invalid alias logged.\n");
        return;
    }
    aliasDuplicate = duplicateA(root, alias);
    ipDuplicate = duplicateIP(root, octet);
    
    if (ipDuplicate && aliasDuplicate) {                        //to check the duplicate of IP address and alias
        printf("Error: Duplicate IP address and alias.\n");
        return;
    } else if (ipDuplicate) {
        printf("Error: Duplicate IP address.\n");
        return;
    } else if (aliasDuplicate) {
        printf("Error: Duplicate alias.\n");
        return;
    }
    root = insert_node(root, octet, alias, 0);
    if (root) {
        printf("Address added successfully in binary search tree.\n");
    } else {
        printf("Error: Could not add address.\n");
    }
}
//Helper function to check for duplicate ip address when detected
int duplicateIP(address_t *node, int octet[]) {
    if (node == NULL) {
        return 0; 
    }
    if (node->octet[0] == octet[0] && 
        node->octet[1] == octet[1] && 
        node->octet[2] == octet[2] && 
        node->octet[3] == octet[3]) {
        return 1; 
    }
    return duplicateIP(node->left, octet) || duplicateIP(node->right, octet);
}

//Helper function to check for duplicate alias when detected
int duplicateA(address_t *node, const char *alias) {
    for (address_t *currentNode = node; currentNode != NULL; ) {
        if (strcmp(currentNode->alias, alias) == 0) {
            return 1; 
			}
        if (strcmp(alias, currentNode->alias) < 0) {
            currentNode = currentNode->left; 
        } else {
            currentNode = currentNode->right; 
			}
    }
    return 0; 
}

// Update an address/alias
void updateAddress() {
    char alias[MAX_ALIAS_LEN + 1];
    printf("Enter alias to update: ");
    scanf("%s", alias);
    getchar();  
    to_lowercase(alias);

    address_t *node = search_node(root, alias);
    if (node == NULL) {
        printf("Error: Alias not found.\n");
        return;
    }
    int octet[MAX_OCTETS];
    char input[256];
    while (1) {
        printf("Enter new IPv4 address (format: x.x.x.x): ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';  
        if (is_validFormat(input) && sscanf(input, "%d.%d.%d.%d", &octet[0], &octet[1], &octet[2], &octet[3]) == 4) {
            break;  
        }
        printf("Error: Invalid Address\n");
    }
    for (int i = 0; i < MAX_OCTETS; i++) {         
        node->octet[i] = octet[i];
    }
    printf("Address updated successfully.\n");
}

// Function to Delete an address/alias
address_t* delete_node_helper(address_t *root, char *alias, int *success_flag) {
    if (root == NULL) return root;

    if (strcmp(alias, root->alias) < 0) {
        root->left = delete_node_helper(root->left, alias, success_flag);
    } else if (strcmp(alias, root->alias) > 0) {
        root->right = delete_node_helper(root->right, alias, success_flag);
    } else {
        *success_flag = 1;

        if (root->left == NULL) {
            address_t *temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            address_t *temp = root->left;
            free(root);
            return temp;
        }
        
        address_t *temp = root->right;
        while (temp && temp->left != NULL) {
            temp = temp->left;
        }
        
        strcpy(root->alias, temp->alias);
        for (int i = 0; i < MAX_OCTETS; i++) {
            root->octet[i] = temp->octet[i];
        }
        root->right = delete_node_helper(root->right, temp->alias, success_flag);
    }
    if (!root) return root;  // If the node is NULL, return it

    updateHeight(root);  // Update the height of the current node
    int balance = get_balance_factor(root);  // Get the balance factor

    // Perform rotations based on the balance factor
    if (balance > 1 && get_balance_factor(root->left) >= 0) {
        return rotateRight(root);  // Left-Left case
    }
    if (balance > 1 && get_balance_factor(root->left) < 0) {
        return rotateLeftThenRight(root);  // Left-Right case
    }
    if (balance < -1 && get_balance_factor(root->right) <= 0) {
        return rotateLeft(root);  // Right-Right case
    }
    if (balance < -1 && get_balance_factor(root->right) > 0) {
        return rotateRightThenLeft(root);  // Right-Left case
    }

    return root;  // Return the root after balancing
}


// Wrapper function to handle the delete address
void delete_address(char *alias) {
    char askingConfirm[10];
    printf("Do you want to delete the address of alias '%s'? (y/n): ", alias);
    
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    fgets(askingConfirm, sizeof(askingConfirm), stdin);
    askingConfirm[strcspn(askingConfirm, "\n")] = '\0'; 
    
    if (tolower(askingConfirm[0]) == 'y') {
        int success_flag = 0;
        root = delete_node_helper(root, alias, &success_flag);

        // Check if deletion was successful
        if (success_flag) {
            printf("Address deleted successfully.\n");
        } else {
            printf("Error: Alias not found.\n");
        }
    } else {
        printf("Deletion canceled.\n");
    }
}

// Function to Display aliases for a given location
void display_loaction() {
    char locality[MAX_ALIAS_LEN + 1];
    printf("Enter Locality :");
    scanf("%s", locality);
    display_aliasesIP(locality);
}

// Function to display aliases match
void display_aliasesIP(char *ip_part) {
    printf("Location: %s\n", ip_part);
    display_matchingIP(root, ip_part);
}

// Helper function to display aliases match
void display_matchingIP(address_t *node, const char *ip_part) {
    if (node == NULL) return;
    
    display_matchingIP(node->left, ip_part);  //Inorder traversal
    char ip_str[16];
    snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d", node->octet[0], node->octet[1], node->octet[2], node->octet[3]);
    if (strstr(ip_str, ip_part) != NULL) {
        printf("%s\n", node->alias);
    }
    display_matchingIP(node->right, ip_part);
}

// Function to Display the error log
void display_error_log() {
    FILE *file = fopen(ERROR_LOG_FILE, "r");
    if (file == NULL) {
        printf("Error log file not found.\n");
        return;
    }
    printf("Error Log:\n");
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);  
	}
    fclose(file);
}





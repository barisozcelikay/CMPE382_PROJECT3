#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_SIZE 224


// Data structure to store a Trie node
struct Trie
{
    int count;
    struct Trie* character[CHAR_SIZE];
};

// Function that returns a new Trie node
struct Trie* getNewTrieNode()
{
    struct Trie* node = (struct Trie*)malloc(sizeof(struct Trie));
    node->count = 0;

    for (int i = 0; i < CHAR_SIZE; i++) {
        node->character[i] = NULL;
    }

    return node;
}

//function to insert a string into a Trie
void insert(struct Trie *head, char* str)
{

    struct Trie* curr = head;
    while (*str)
    {

        if (curr->character[(unsigned char)*str - ' '] == NULL) {
            curr->character[(unsigned char)*str - ' '] = getNewTrieNode();
        }
        curr = curr->character[(unsigned char)*str - ' '];

        str++;
    }
    // count 1.
    curr->count++;
}

//to search a string in a Trie. It returns 1 if the string is found in the Trie, otherwise, it returns 0.
int search(struct Trie* head, char* str)
{

    if (head == NULL) {
        //printf("böyle bir trie bulunamadı.");
        return 0;
    }

    struct Trie* curr = head;
    while (*str)
    {
        curr = curr->character[(unsigned char)*str - ' '];
        if (curr == NULL) {
            return 0;
        }
        str++;
    }

    return curr->count;
}

// Check trie node's children
int hasChildren(struct Trie* curr)
{
    for (int i = 0; i < CHAR_SIZE; i++)
    {
        if (curr->character[i]) {
            return 1;       // child found
        }
    }

    return 0;
}

// delete string
int deletion(struct Trie **curr, char* str)
{
    // empty
    if (*curr == NULL) {
        return 0;
    }
    // if end of the string isnt reached
    if (*str)
    {
        // if it is non-leaf
        if (*curr != NULL && (*curr)->character[(unsigned char)*str - ' '] != NULL &&
            deletion(&((*curr)->character[(unsigned char)*str - ' ']), str + 1) &&
            (*curr)->count == 0)
        {
            if (!hasChildren(*curr))
            {
                free(*curr);
                (*curr) = NULL;
                return 1;
            }
            else {
                return 0;
            }
        }
    }

    // if end of the string is reached
    if (*str == '\0' && (*curr)->count)
    {
       // no children
        if (!hasChildren(*curr))
        {
            free(*curr);
            (*curr) = NULL;
            return 1;
        }

        // if the current node is a leaf node and has children
        else {
            (*curr)->count = 0;
            return 0;
        }
        //printf("count: %d",count);
    }

    return 0;
}

//write Trie node to Dictionary file
void print_string(char *str, int pos, int count, FILE *f)
{
   int i;
   // char outstr[250];
   char outstr[500];
   char buf[500];

   for (i = 0; i < pos; i++) {
      //printf("pos: %d",pos);
      outstr[i] = str[i];
   }
   outstr[i] = 0;
   sprintf(buf, "%s\t%d\n", outstr, count);
   fputs(buf, f);
}

//write all Trie's node to Dictionary file
int print_all_node(struct Trie *ptr, char *str, int pos, FILE *f)
{
   int i;

   if (ptr == NULL)
      return 0;

   if (ptr->count) {
      print_string(str, pos, ptr->count, f);
   }

   for (i = 0; i < CHAR_SIZE; i++) {
      if (ptr->character[i] != NULL) {
         str[pos] = i + ' ';
         print_all_node(ptr->character[i], str, pos+1, f);
      }
   }
   return 0;
}

//Delete Trie node, and release the allocated memory
int delete_node(struct Trie **ptr, struct Trie *head)
{
   int i;
   static int freenbr = 0;

   if (*ptr == NULL) return 0;
   else {
      for (i = 0; i < CHAR_SIZE; i++) {
         delete_node(&((*ptr)->character[i]), head);
         (*ptr)->character[i] = NULL;
      }
      freenbr++;
      free(*ptr);
      *ptr = NULL;
   }
   return 1;
}


int main()
{
    struct Trie* head = getNewTrieNode();
    FILE *f = NULL;
    FILE *fout;
    char buf[500];
    char *filename[] = {"data1.txt", "data2.txt", "data3.txt", "data4.txt",
                        "data5.txt", "data6.txt", "data7.txt", "data8.txt",
                        "data9.txt", "data10.txt"};
    int fidx=0;
    char linestr[500];
    int pos;

    printf("Creating dictionary ...\n");
    fout = fopen("Dictionary.txt", "w");
    while (1) {
       if (fidx >= 10) break;
       if (f == NULL) {
          f = fopen(filename[fidx], "r");
          printf("processing file %s ...\n", filename[fidx]);
          if (f == NULL) break;
       }

       while (!feof(f)) {
          fgets(buf, 500, f);
          if (feof(f)) {
             fclose(f);
             f = NULL;
             fidx++;
             break;
          }
          buf[strcspn(buf, "\n\r")] = 0;
          if (head == NULL) head = getNewTrieNode();

          insert(head, buf);
       }
    }
    pos = 0;
    printf("writing to Dictionary.txt ..\n");
    print_all_node(head, linestr, pos, fout);
    delete_node(&head, head);

    if (f) fclose(f);
    fclose(fout);

    return 0;
}

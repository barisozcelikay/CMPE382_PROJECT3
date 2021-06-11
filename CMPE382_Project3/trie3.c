#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_SIZE 224

//int mode = 1;

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
    // start from the root node
    struct Trie* curr = head;
    while (*str)
    {

        if (curr->character[(unsigned char)*str - ' '] == NULL) {
            curr->character[(unsigned char)*str - ' '] = getNewTrieNode();
        }

        //next node
        curr = curr->character[(unsigned char)*str - ' '];

        str++;
    }
    curr->count++;
}

//to search a string in a Trie. It returns 1 if the string is found in the Trie, otherwise, it returns 0.

int search(struct Trie* head, char* str)
{

    if (head == NULL) {
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
            //printf("child var");
            return 1;
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

    // if the end of the string is reached
    if (*str == '\0' && (*curr)->count)
    {
        if (!hasChildren(*curr))
        {
            free(*curr);
            (*curr) = NULL;
            return 1;
        }
        else {
            (*curr)->count = 0;
            return 0;
        }
    }

    return 0;
}

//write Trie node to Dictionary file
void print_string(char *str, int pos, int count, FILE *f)
{
   int i, x;
   char outstr[500];
   char buf[500];

   if (pos > 500) {
      x = 500;
fprintf(stderr, "x > 500 ======================>\n");
   } else x = pos;

   for (i = 0; i < x; i++) {
      outstr[i] = str[i];
   }
   outstr[i] = 0;
   sprintf(buf, "%s\t%d\n", outstr, count);
   fputs(buf, f);
}

//writing all trie's node to dictionary file
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
         //free((*ptr)->character[i]);
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
    char buf[2048], *bufptr;
    char *filename[] = {"data1.txt", "data2.txt", "data3.txt", "data4.txt",
                        "data5.txt", "data6.txt", "data7.txt", "data8.txt",
                        "data9.txt", "data10.txt"};
    int i=0, done;
    int fidx=0;
    char linestr[500];
    int pos, nread;

    printf("Creating dictionary ...\n");
    fout = fopen("Dictionary.txt", "w");
    while (1) {
       if (fidx >= 10) break;
       i = 0;
       if (f == NULL) {
          f = fopen(filename[fidx], "r");
          printf("processing %s ..\n", filename[fidx]);
          if (f == NULL) break;
       }

       if (head == NULL) head = getNewTrieNode();
       while (!feof(f)) {
          done = 0;
          i = 0;
          while (!done) {
             bufptr = buf;
             nread = fread(buf, 1, 2048, f);
             //if (nread != 2048)
             if (feof(f)) done = 1;

             while (nread > 0) {
                linestr[i] = *bufptr++;
                nread--;

                if (linestr[i] == '\n') {
                   linestr[i] = 0;
                   insert(head, linestr);
                   i = 0;
                } else {
                   i++;
                }
             }
          }

          if (feof(f)) {
             fclose(f);
             f = NULL;
             fidx++;
             break;
          }
       }

    }
    pos = 0;
    print_all_node(head, linestr, pos, fout);
    delete_node(&head, head);

    //if (f) fclose(f);
    fclose(fout);

    return 0;
}

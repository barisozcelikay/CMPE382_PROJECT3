#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define CHAR_SIZE 224

int mode = 1;

// Data structure to store a Trie node
struct Trie
{
    int count;
    struct Trie* character[CHAR_SIZE];

};

typedef struct {
   int tidx;
   char *filename;
   struct Trie **head;
} targ_t;

pthread_mutex_t shrmutex = PTHREAD_MUTEX_INITIALIZER;


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
    //mutex
    pthread_mutex_lock(&shrmutex);

    // start from the root node
    struct Trie* curr = head;
    while (*str)
    {
        //new node
        if (curr->character[(unsigned char)*str - ' '] == NULL) {
            curr->character[(unsigned char)*str - ' '] = getNewTrieNode();
        }

        // next node
        curr = curr->character[(unsigned char)*str - ' '];

        str++;
    }


    curr->count++;

    pthread_mutex_unlock(&shrmutex);
}

//to search a string in a Trie. It returns 1 if the string is found in the Trie, otherwise, it returns 0.
int search(struct Trie* head, char* str)
{
    // empty
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
            return 1;
        }
    }
    return 0;
}

// delete string
int deletion(struct Trie **curr, char* str)
{

    if (*curr == NULL) {
        return 0; //empty 0
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
        // if the current node is a leaf node and doesn't have any children
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
   int i;
   char outstr[500];
   char buf[500];

   for (i = 0; i < pos; i++) {
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
int delete_node(struct Trie **ptr)
{
   int i;
   static int freenbr = 0;

   if (*ptr == NULL) return 0;
   else {
      for (i = 0; i < CHAR_SIZE; i++) {
         delete_node(&((*ptr)->character[i]));
         (*ptr)->character[i] = NULL;
      }
      freenbr++;
      free(*ptr);
      *ptr = NULL;
   }
   return 1;
}

void *process_task(void *arg)
{
   targ_t targ;
   int tidx;
   char *filename;
   FILE *f;
   char buf[500];
   struct Trie **head;

   targ = *(targ_t *)arg;
   tidx = targ.tidx;
   filename = targ.filename;
   head = targ.head;

   //create local Trie structure
   *head = getNewTrieNode();

   f = fopen(filename, "r");
   fprintf(stderr, "task %d, processing %s ...\n", tidx, filename);
   if (f == NULL) return NULL;

   while (!feof(f)) {
      fgets(buf, 500, f);
      if (feof(f)) {
         break;
      }
      buf[strcspn(buf, "\n\r")] = 0;

      insert(*head, buf);
   }
   fclose(f);

   return NULL;
}

int read_and_insert(struct Trie *threadptr, struct Trie *ptr, char *str,
                     int pos)
{
   int i;
   char buf[500];

   if (ptr == NULL)
      return 0;

   if (threadptr->count) {
      //print_string(str, pos, ptr->count, f);
      memcpy(buf, str, pos);
      buf[pos] = 0;
      for (i = 0; i < threadptr->count; i++) {
         insert(ptr, buf);
      }
   }

   for (i = 0; i < CHAR_SIZE; i++) {
      if (threadptr->character[i] != NULL) {
         str[pos] = i + ' ';
         //print_all_node(ptr->character[i], str, pos+1, f);
         read_and_insert(threadptr->character[i], ptr, str, pos+1);
      }
   }
   return 0;
}


int main()
{
    struct Trie* head = getNewTrieNode();
    struct Trie* threadHead[10];
    FILE *f = NULL;
    FILE *fout;
    char *filename[] = {"data1.txt", "data2.txt", "data3.txt", "data4.txt",
                        "data5.txt", "data6.txt", "data7.txt", "data8.txt",
                        "data9.txt", "data10.txt"};
    int i=0;
    char linestr[500];
    int pos;
    pthread_t tid[10];
    targ_t targ[10];

    printf("Creating dictionary ...\n");

    for (i = 0; i < 10; i++) {
       targ[i].tidx = i;
       targ[i].filename = filename[i];
       targ[i].head = &threadHead[i];
       pthread_create(&tid[i], NULL, &process_task, (void *)&targ[i]);
    }

    for (i = 0; i < 10; i++) {
       pthread_join(tid[i], NULL);
    }

    printf("All threads were done.\n");
    fout = fopen("Dictionary.txt", "w");

    //Reading and combinin tries
    printf("Combining all local Tries\n");
    for (i = 0; i < 10; i++) {
       pos = 0;
       read_and_insert(threadHead[i], head, linestr, pos);
       delete_node(&threadHead[i]);
    }

    printf("writing to Dictionary.\n");
    pos = 0;
    print_all_node(head, linestr, pos, fout);
    delete_node(&head);

    if (f) fclose(f);
    fclose(fout);

    return 0;
}

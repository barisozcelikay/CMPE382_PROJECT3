# CMPE382_PROJECT2
## Project-3 : Creating Dictionary for the AOL Dataset
### Project Partners: Barış Özçelikay, Kadir Kaan Yazgan

#### Compiling and run:
There is Makefile to create all programs: trie2, trie3, trie4, trie5, and trie6.

For compiling individually you can just write “make ‘one_of_trie_name’.”

For compiling you should write “make”. After that, you need to type “./trie + (task number)” for the task you want to run. Like, “./trie2” for task 2, ”./trie3” for task 3.



#### Design overview:
Our goal in this project is to create our own trie and create additional dictionaries with different methods. In general, all the tasks requested from us have the same code structure, but contain different methods and structs according to the desired method. Although the code structure was a continuation of the previous one at the beginning of the project, we made each task a separate .c file so that the results we will get are more detailed and our makefile is more tidy.

To talk about the trie structure in general. We have a struct called Trie. In this struct, there is a value that calls itself as a string array and a count value of int type. To talk about the trie structure in general. We have a struct called Trie. In this struct, there is a value that calls itself as a string array and a count value of int type. This count value is a control value and if it is greater than 0, it says that it is in the lowest layer of the trie, that is, leaf.

#### Complete specification:
Actually, this project was very confusing to us at first, but as we progressed and understood the subject better, we started to think about what we should do. We had more difficulties in thread structure Especially making multiple trie using threads made it difficult for us. However, because we used threads in our previous project1: Linux implementation, we were experienced and were able to cope with it.

#### Known bugs and problems:
Since the size of the data files is very large, it both slowed us down while working on it, was very challenging for computers and caused us to get errors. so we reduced the size of the datala file. Each data file contains 40,000 lines.

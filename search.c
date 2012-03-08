#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include "search.h"
#define dicFile "dicFile.txt"

//Puts the dictionary into an array(always do first)
//Takes no parameters and returns an array of strings representing the dictionary
//use char** dic = makeDic(); in main function
//make sure there is a file named dicFile.txt in the same directory
//the dictionary can be up to 10000 words and each word has to be on a separate line(nospaces)
//print statements are only for testing
char** makeDic(){
	//initialize variables
	int dic, x, y, count, num;
	num = 175000;
	char** dicArray;
	char* temp;
	temp = (char*)malloc(15*num);
	dicArray = (char**)malloc(num*sizeof(char*));
	for(x=0; x<num; x++){
		dicArray[x] = (char*)malloc(256);
	}
	//reads dictionary file into a temporary string
	dic = open(dicFile, O_RDONLY, 0777);
	y = read(dic, temp, (15*num));
	//printf("%s", temp);
	if(y == -1){
		printf("Dictionary File Error");
		return NULL;
	}
	count = 0;
	//printf("%s", strsep(&temp, " "));
	//separates temp string and puts words into char** array
	while(temp){
		dicArray[count] = strsep(&temp, "\n");
		count++;
	}
	/*
	 for(y=0; y<count; y++){
	 printf("%s ", dicArray[y]);
	 }
	 */
	return dicArray;
}

//counts and returns the number of words in the dictionary
int dicCount(char** dic){
	int x = 0;
	while(dic[x] != NULL && strcmp(dic[x], "") != 0){
		x++;
	}
	return x;
}

//searches the dictionary to see if there is a particular word in it
//takes a string for the word to be searched, the array of strings-
//-representing the dictioary and an int for the total number of words-
//-in the dictionary
//returns an int, 1 if the word is there, 0 if not
int search(char* word, char** dic){
	int beg = 0;
	int index = 0;
	int cmp;
	int end = dicCount(dic);
	end--;
	//standard binary search algorithm 
	while(end >= beg){
		index = (beg + end)/2;
		//printf("%d ", index);
		cmp = strcmp(word, dic[index]);
		if(cmp == 0){
			return 1;
		}
		else if(cmp>0){
			beg = (index + 1);
		}
		else{
			end = (index - 1);
		}
	}
	return 0;
}

//this function is only to test makeDic(), countDic() and search()
/*
int main(){
	//printf("%d\n", strcmp("apple", "banana"));
	int b;
	char** dic;
	//populates dictionary array
	dic = makeDic();
	//counts the length
	int h = dicCount(dic);
	//prints out each word and its index
	for (b = 0; b<h; b++) {
		printf("%s ", dic[b] );
	}

	//seaches 11 words and tracks the index being checked
	//printf("\n");
	printf("%d\n", h);
	printf(":%d\n", search("apple", dic));
	printf(":%d\n", search("banana", dic));
	printf(":%d\n", search("car", dic));
	printf(":%d\n", search("dog", dic));
	printf(":%d\n", search("elephant", dic));
	printf(":%d\n", search("flamingo", dic));
	printf(":%d\n", search("grape", dic));
	printf(":%d\n", search("high", dic));
	printf(":%d\n", search("iodine", dic));
	printf(":%d\n", search("juggle", dic));
	printf(":%d\n", search("kite", dic));
	printf(":%d\n", search("loan", dic));
	printf(":%d\n", search("blue", dic));
	printf(":%d\n", search("iuyagwfruy", dic));

	return 1;
	
}
*/

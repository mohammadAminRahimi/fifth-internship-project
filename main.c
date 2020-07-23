#include <stdio.h>
#include <stdlib.h>
// #include <libpq-fe.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>


//name of files in the directory
// reading data from files
// storing information in database

void readAndWrite(int num, char *names[]);
int countigFiles();
void nameOfTheFiles(char *names[]);

int main(int args, char *argv[]){
    int filesNumber;
    char str[5] = "hel";
    printf("%s", str);
    // because there will be a report every 60 seconds, we run this code every 50 seconds.
    printf("\nhello world\n");

    while(1){
        //50 seconds delay
        printf("0\n");
        sleep(1);

        printf("1\n");
        // finding out number of files in the directory
        filesNumber = countigFiles();
        printf("2\nnum %d  \n", filesNumber);

        //finding out names of the files
        char *fileName[filesNumber];
        nameOfTheFiles(fileName);
        printf("3\n");
          //reading files and storing them in the database
        readAndWrite(filesNumber, fileName);

    }

    // int lib_ver = PQlibVersion();
    // printf("Version of libpq: %d\n", lib_ver);

    // return 0;
}

int countigFiles(){
    int count=0;
        
        // opendir() returns a pointer of DIR type.  
    DIR *dr = opendir("./tmp/final_project"); 
    struct dirent *de; 
    if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        printf("Could not open current directory" ); 
        return 0; 
    } 
    // Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html 
    // for readdir() 
    while ((de = readdir(dr)) != NULL) 
        if(de->d_name[0] == 'r'){
            //printf("%s \n", de->d_name);
            count++;
        }
    closedir(dr); 
    return count;
}


void nameOfTheFiles(char *names[]){
    int counter=0;
    DIR *dr = opendir("./tmp/final_project"); 
    struct dirent *de; 
    if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        printf("Could not open current directory" ); 
        return; 
    } 
    // Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html 
    // for readdir() 
    while ((de = readdir(dr)) != NULL){
        if(de->d_name[0] == 'r'){
            names[counter] = de->d_name;
           // printf("%s\n", de->d_name);
            counter++;
        }
    }
    closedir(dr); 
}

      

void readAndWrite(int num, char *name[]){
    FILE *fp;


    for(int i=0 ; i<num ; i++){
        char str[140] = "./tmp/final_project/";
        strcat(str, name[i]);
        printf("%c   ", *name[i]);
        if((*name[i]) == 'r'){
            char buff[255];
            fp = fopen(str, "r");
            while(fgets(buff, 255, fp) != null){


            }
            fclose(fp);
            return;
        }
    }
}



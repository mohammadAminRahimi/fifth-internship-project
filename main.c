#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>


//name of files in the directory
// reading data from files
// storing information in database
void writingRecordsInto_fp_stores_dataTable(char **info);
void splittingOneLineOfFile(char buff[], char **info);
void readAndWrite(int num, char *names[]);
int countigFiles();
void nameOfTheFiles(char *names[]);

PGconn *conn; 

int main(int args, char *argv[]){

    conn = PQconnectdb("user=amin password=J4T823AWZ dbname=fpdb");
    if (PQstatus(conn) == CONNECTION_BAD) { 
        fprintf(stderr, "Connection to database failed: %s\n",
            PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }

    int filesNumber;
    char str[5] = "hel";
    printf("%s", str);
    // because there will be a report every 60 seconds, we run this code every 50 seconds.
    printf("\nhello world\n");

    while(1){
        //50 seconds delay
        sleep(1);

        // finding out number of files in the directory
        filesNumber = countigFiles();
        //finding out names of the files
        char *fileName[filesNumber];
        nameOfTheFiles(fileName);
          //reading files and storing them in the database
        readAndWrite(filesNumber, fileName);
        PQfinish(conn);
        return 0;
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
        if((*name[i]) == 'r'){
            char buff[255];
            fp = fopen(str, "r");
            int temp=0;
            while(fgets(buff, 255, fp) != NULL){
                char *info[8];
                for(int i=0 ; i<8 ; i++){
                    info[i] = (char *)malloc(50 * sizeof(char));
                }
                splittingOneLineOfFile(buff, info);
                // printf("%s \n", info[0]);
                // printf("%s \n", info[1]);
                // printf("%s \n", info[2]);
                // printf("%s \n", info[3]);
                // printf("%s \n", info[4]);
                // printf("%s \n", info[5]);
                // printf("%s \n", info[6]);
                // printf("\n    %s \n",info[7]);
                writingRecordsInto_fp_stores_dataTable(info);

            }
            fclose(fp);
            return;
        }
    }
}


void splittingOneLineOfFile(char buff[], char **info){
    char temp[50];
    int  numberOfString=0, count=0;
    for(int i=0 ; i<7 ; i++){
        char temp[50];
        for(int j=0 ; buff[count] != ',' ; j++){
            temp[j] = buff[count];
            temp[j+1]= '\0';
            count++;
        }
        strcpy(info[numberOfString], temp);
        numberOfString++;
        count++;
    }
    int i=0;
    while(buff[count] != '\n'){
        temp[i] = buff[count];
        temp[i+1] = '\0';
        count++;
        i++;
    }
    strcpy(info[7],temp);
}



void writingRecordsInto_fp_stores_dataTable( char **info){
    char *string = (char *)malloc(250 * sizeof(char));
    strcat(string, "INSERT INTO fp_store_data VALUES('");
    strcat(string,*info);
    strcat(string,"', '");
    strcat(string,info[1]);
    strcat(string,"' , '" );
    strcat(string,info[2]);
    strcat(string,"' , '" );
    strcat(string,info[3]);
    strcat(string,"' , '" );
    strcat(string,info[4]);
    strcat(string,"' , '" );
    strcat(string,info[5]);
    strcat(string,"' , '" );
    strcat(string,info[6]);
    strcat(string,"' , '" );
    strcat(string,info[7]);
    strcat(string,"')");



    printf("%s  \n", string);
    PGresult *res = PQexec(conn, string);
    // if (PQresultStatus(res) != PGRES_COMMAND_OK) {  
    //     printf("hello world");
    //     PQclear(res);
    //     PQfinish(conn);
    //     exit(1);
    // } 
}

#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>


// functions declaration
int strToInt(char *string);
void intToStr(char* str, int num);
void writingRecordsInto_fp_city_aggregationTable(char **info);
void writingRecordsInto_fp_store_aggregationTable(char **info);
void writingRecordsInto_fp_stores_dataTable(char **info);
int splittingOneLineOfFile(char buff[], char **info);
void readAndWrite(int num, char *names[]);
int countigFiles();
void nameOfTheFiles(char *names[]);

// global database connection variable
PGconn *conn;

struct node {
    char city[50];
    char product_id[10];
    char price[10];
    char quantity[5];
    char has_sold[5];
    struct node *next;
};

void insertingInto_fp_city_aggregationTable(struct node **head, char *time){
    printf("2 \n");
    struct node *current = *head;
    if(current == NULL){
        return;
    }
    int count=0;
    char string[250];
    while(current != NULL){
        count++;
        string[0] = '\0';
        strcat(string, "INSERT INTO fp_city_aggregation VALUES(");
        strcat(string,time);
        strcat(string, " , '");
        strcat(string, current->city);
        strcat(string, "' , '");
        strcat(string, current->product_id);
        strcat(string, "' , ");
        strcat(string, current->price);
        strcat(string, " , ");
        strcat(string, current->quantity);
        strcat(string, " , ");
        strcat(string, current->has_sold);
        strcat(string, ")");
        PGresult *res = PQexec(conn, string); 
        current = current->next;  
    }
}

void addingNewNode(struct node **head, char **info){
    if(*head == NULL){
        struct node *newNode = (struct node*) malloc(sizeof(struct node));
        strcpy(newNode->city, info[2]);
        strcpy(newNode->product_id, info[4]);
        strcpy(newNode->price, info[5]);
        strcpy(newNode->quantity, info[6]);
        strcpy(newNode->has_sold, info[7]);
        newNode->next = NULL;
        *head = newNode; 
        return;
    }
    struct node *current = *head;
    int flag = 0;
    while(current->next != NULL){
        if(strcmp(current->city, info[2]) == 0&& strcmp(current->product_id, info[4]) == 0){
            char quantity[10];
            intToStr(quantity, strToInt(current->quantity) + strToInt(info[6]));
            strcpy(current->quantity, quantity);
            char has_sold[10];
            intToStr(has_sold, strToInt(current->has_sold) + strToInt(info[7]));
            strcpy(current->has_sold, has_sold);
            char price[12];
            intToStr(price, (int)((strToInt(current->price)*strToInt(current->has_sold) + ((strToInt(info[7]) * strToInt(info[5]))))/ strToInt(current->has_sold)));
            strcpy(current->price, price);
            flag = 1;
        }
        current = current->next;
    }
    if( flag == 0){
        struct node *newNode = (struct node*) malloc(sizeof(struct node));
        strcpy(newNode->city, info[2]);
        strcpy(newNode->product_id, info[4]);
        strcpy(newNode->price, info[5]);
        strcpy(newNode->quantity, info[6]);
        strcpy(newNode->has_sold, info[7]);
        newNode->next = NULL;
        current->next = newNode;
    }
}

void printList(struct node *head){
    struct node *current = head;
    int count=0;
    while(current != NULL){
        if(count<1200)
        printf("%s   %s   %d \n", current->city, current->product_id, count++);
        current = current->next;
    }
}
/*
    in the main function we first make a database connection,
    then we try to find out the the name of files in the directory,
    next we read the file and write them into databases using readAndWrite function.
*/
int main(int args, char *argv[]){
    conn = PQconnectdb("user=amin  dbname=fpdb");
    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n",
            PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
    int filesNumber;
    char str[5] = "hel";

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

/*
    with the help of this function we find out the number of directory files
*/
int countigFiles(){
    int count=0;
        // opendir() returns a pointer of DIR type.
    DIR *dr = opendir("/Users/amin/Desktop/fifth-internship-project/fifth-internship-project/tmp/final_project");
    struct dirent *de;
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("CccccCould not open current directory" ); 
        return 0;
    }
    // Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
    // for readdir()
    while ((de = readdir(dr)) != NULL)
        // we count the files which begin with r char because we want files with the name report ...
        if(de->d_name[0] == 'r'){
            count++;
        }
    closedir(dr);
    return count;
}

/*
    this function helps us to find the name of the files in the intended directory
*/
void nameOfTheFiles(char *names[]){
    int counter=0;
    DIR *dr = opendir("/Users/amin/Desktop/fifth-internship-project/fifth-internship-project/tmp/final_project");
    struct dirent *de;
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("how Could not open current directory" );
        return;
    }
    while ((de = readdir(dr)) != NULL){
        // the files which begin with 'r' char are desirable
        if(de->d_name[0] == 'r'){
            names[counter] = de->d_name;
            counter++;
        }
    }
    closedir(dr);
}


void readAndWrite(int num, char *name[]){
    FILE *fp;
    // num varible shows the number of file so that this loop runs for each file, then read info of each file
    for(int i=0 ; i<num ; i++){
        char str[240] = "/Users/amin/Desktop/fifth-internship-project/fifth-internship-project/tmp/final_project/";
        strcat(str, name[i]);
        if((*name[i]) == 'r'){
            struct node *head = NULL;
            char time[12];
            char buff[255];
            fp = fopen(str, "r");
            int temp=0;
            // in this while loop we read a file compeletly line by line and write the records to database
            while(fgets(buff, 255, fp) != NULL){
                char *info[8];
                for(int i=0 ; i<8 ; i++){
                    info[i] = (char *)malloc(50 * sizeof(char));
                }
                /*
                we first read one recod of file using splittingOneLineOfFile function,
                then we make changes to three database tables by the help of nex three function
                */
                int res = splittingOneLineOfFile(buff, info);
                if ( res == 0 ){
                    printf("this recorde = %s was ignored \n", buff);
                }else {
                    if(time[0] == '\0'){
                        strcpy(time, info[0]);
                    }
                    addingNewNode(&head, info);
                    writingRecordsInto_fp_stores_dataTable(info);
                    // writingRecordsInto_fp_city_aggregationTable(info);
                     writingRecordsInto_fp_store_aggregationTable(info);
                }
            }

            // freeing up the memory
            insertingInto_fp_city_aggregationTable(&head, time);
            fclose(fp);
        }
    }
}

/*
    each records of file has got 8 columns(attribute)
    in this function we process each column and put every attribute into a char*
*/
int splittingOneLineOfFile(char buff[], char **info){
    char temp[50];
    int  numberOfString=0, count=0;
    for(int i=0 ; i<7 ; i++){
        char temp[50];
        for(int j=0 ; buff[count] != ',' ; j++){
            if(buff[count] == '\0'){
                return 0;
            }
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
    return 1;
}


/*
 in this function we put every records of files into the fp_stores_database
*/
void writingRecordsInto_fp_stores_dataTable( char **info){
    char *string = (char *)malloc(250 * sizeof(char));
    strcat(string, "INSERT INTO fp_stores_data VALUES(");
    strcat(string,*info);
    strcat(string,", '");
    strcat(string,info[1]);
    strcat(string,"' , '" );
    strcat(string,info[2]);
    strcat(string,"' , '" );
    strcat(string,info[3]);
    strcat(string,"' , '" );
    strcat(string,info[4]);
    strcat(string,"' , " );
    strcat(string,info[5]);
    strcat(string," , " );
    strcat(string,info[6]);
    strcat(string," , " );
    strcat(string,info[7]);
    strcat(string,")");
    PGresult *res = PQexec(conn, string);
}

/*
 making appropriate changes into fp_city_aggregation table when added processing a new recorde
*/
void writingRecordsInto_fp_city_aggregationTable(char **info){
    char *string = (char *)malloc(250 * sizeof(char));
    char time[30], product[30], city[30], price[30], quantity[30], has_sold[30];
    /*
    we make the query to find out the row which has got the same time and product_id
    and city, then we can update the info on the database
    */
    strcpy(time,info[0]);
    strcpy(city,info[2]);
    strcpy(product,info[4]);
    strcpy(price,info[5]);
    strcpy(quantity,info[6]);
    strcpy(has_sold,info[7]);
    strcat(string, "select * from fp_city_aggregation where time = ");
    strcat(string, time);
    strcat(string, " and product_id = '");
    strcat(string, product);
    strcat(string, "' and city = '");
    strcat(string, city);
    strcat(string, "'");
    PGresult *res = PQexec(conn,string);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }
    int rows = PQntuples(res);
    /*
    if rows variable is 0, it means that there is no rows with the same time and city and product_id
    in the database then we don't need to updata database we just need to insert record into databas
    */
    if(rows == 0){
        string[0]='\0';
        strcat(string, "INSERT INTO fp_city_aggregation VALUES(");
        strcat(string,time);
        strcat(string, " , '");
        strcat(string, city);
        strcat(string, "' , '");
        strcat(string, product);
        strcat(string, "' , ");
        strcat(string, price);
        strcat(string, " , ");
        strcat(string, quantity);
        strcat(string, " , ");
        strcat(string, has_sold);
        strcat(string, ")");
        PGresult *res = PQexec(conn, string);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            printf("it's really badd");
            PQclear(res);
            PQfinish(conn);
            exit(1);
        }
    }

    /*
    if there is a row with the same  time and city and product_id, we must update the info in the database
    we add has_sold var in two record to make the new has_sold
    we recalculate the average price of the product
    we add quantity var in two record to make the new quantity
    */
     else if(rows == 1){
        int tot_has_sold = strToInt(has_sold) + strToInt(PQgetvalue(res, 0, 5));
        int averagePrice = (int)(((strToInt(price) * strToInt(has_sold)) + (strToInt(PQgetvalue(res, 0, 3)) * strToInt(PQgetvalue(res, 0, 5)))) / tot_has_sold);
        int tot_quantity = strToInt(PQgetvalue(res, 0, 4)) + strToInt(quantity);
        has_sold[0] = '\0';
        quantity[0] = '\0';
        price[0] = '\0';
        intToStr(has_sold, tot_has_sold);
        intToStr(quantity, tot_quantity);
        intToStr(price, averagePrice);

        string[0] = '\0';
        strcat(string, "update fp_city_aggregation set price = ");
        strcat(string, price);
        strcat(string," , quantity = ");
        strcat(string,quantity);
        strcat(string," , has_sold = ");
        strcat(string,has_sold);
        strcat(string, " where city = '");
        strcat(string, city);
        strcat(string, "' and product_id = '");
        strcat(string, product);
        strcat(string, "' and time = ");
        strcat(string, time);

        PGresult *res = PQexec(conn, string);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            printf("it's really bad d \n");
            PQclear(res);
            PQfinish(conn);
            exit(1);
        }
        PQclear(res);
    } else {
        printf("it is a bad news");
    }
 }


/*
    in this function we add and update infromation in the fp_store_aggregation
    in fp_store_aggregation table we keep info about each market
*/
void writingRecordsInto_fp_store_aggregationTable(char **info){
    char *string = (char *)malloc(250 * sizeof(char));
    char market[20], product[20], tot_price[20];
    /*
        first we select the record with the same market_id and product_id from database
    */
    strcpy(market,info[3]);
    strcpy(product,info[4]);
    strcat(string, "select * from fp_store_aggregation where market_id = '");
    strcat(string, market);
    strcat(string, "' and product_id = '");
    strcat(string, product);
    strcat(string, "'");
    PGresult *res = PQexec(conn,string);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }
    int rows = PQntuples(res);
    /*
    if there is no record with the same market_id and product_id in the
    database we insert the read record from file
    */
    if(rows == 0){
        string[0]='\0';
        strcat(string, "INSERT INTO fp_store_aggregation VALUES('");
        strcat(string,market);
        strcat(string, "' , '");
        strcat(string, product);
        strcat(string, "' , ");
        strcat(string, info[7]);
        strcat(string, " , ");
        int num = strToInt(info[5])* strToInt(info[7]);
        char str[12];
        intToStr(str, num);
        strcat(string, str);
        strcat(string, ")");
        PGresult *res = PQexec(conn, string);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            printf("it's really bad s");
            PQclear(res);
            PQfinish(conn);
            exit(1);
        }
    }
    /*
    if there is a recorde with the same market_id and product_id in the database
    we update that recod in this way: we add prices to get the total money that the market has earned
    by selling thap product
    and we add the has_sold var to get the total number of that product that the market has sold
    */
    else if(rows == 1){
        char price[12];
        intToStr(price, strToInt(info[5]) * strToInt(info[7]) + strToInt(PQgetvalue(res, 0, 3)));
        char has_sold[12];
        intToStr(has_sold, strToInt(info[7]) + strToInt(PQgetvalue(res, 0, 2)));
        string[0] = '\0';
        strcat(string, "update fp_store_aggregation set has_sold = ");
        strcat(string,has_sold);
        strcat(string," , price = ");
        strcat(string,price);
        strcat(string, " where market_id = '");
        strcat(string, market);
        strcat(string, "' and product_id = '");
        strcat(string, product);
        strcat(string, "'");
        PGresult *res = PQexec(conn, string);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            printf("it's really bad mm\n");
            PQclear(res);
            PQfinish(conn);
            exit(1);
        }
        PQclear(res);
    } else{
        printf("%d tuples is chosen!!!", rows);
    }
}

// change char* into int
int strToInt(char *string){
    int num;
    sscanf(string,"%d", &num);
    return num;
}


// change int into char*
void intToStr(char* str,int num){
    sprintf(str, "%d", num);
    return;
}

// void insertingInto_fp_city_aggregationTable(struct node **head, char *time){
//     struct node *current = *head;
//     if(current == NULL){
//         return;
//     }
//     char string[50];
//     while(current != NULL){
//         string[0] = '\0';
//         strcat(string, "INSERT INTO fp_city_aggregation VALUES(");
//         strcat(string,time);
//         strcat(string, " , '");
//         strcat(string, current->city);
//         strcat(string, "' , '");
//         strcat(string, current->product_id);
//         strcat(string, "' , ");
//         strcat(string, current->price);
//         strcat(string, " , ");
//         strcat(string, current->quantity);
//         strcat(string, " , ");
//         strcat(string, current->has_sold);
//         strcat(string, ")");
//         PGresult *res = PQexec(conn, string);
//         if (PQresultStatus(res) != PGRES_COMMAND_OK) {
//             printf("it's really badd");
//             PQclear(res);
//             PQfinish(conn);
//             exit(1);
//         }  
//         current = current->next;  
//     }
// }

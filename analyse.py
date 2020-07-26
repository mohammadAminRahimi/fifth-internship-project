import psycopg2
import math 
def most_favorite_product_in_every_town(cursor):
    # print(i[0].decode('utf-8')[::-1])

    cursor.execute("select distinct city from fp_city_aggregation")
    # print(cursor.fetchall())
    for i in cursor.fetchall():
        string = i[0]
        cursor.execute("select city, has_sold, product_id, price  from fp_city_aggregation where city ='" + string + "' and has_sold = (select Max(has_sold) from fp_city_aggregation where city ='" + string + "')" )
        lis = cursor.fetchall()
        city = lis[0][0]
        has_sold = lis[0][1]
        product_id = lis[0][2]
        price = lis[0][3]
        print("city = " + city.decode('utf-8')[::-1] + ",    product_id = " + product_id + ",  has_sold = "+ str(has_sold) 
        + ",  total_money_earned = "+ str(has_sold * price))




def best_store_in_case_of_income(cursor):
    

    cursor.execute("select  distinct market_id from fp_store_aggregation")

    cursor.execute("select market_id, mprice from ( select market_id, Sum(price) as mprice " +
    "from fp_store_aggregation group by market_id) as table1 where mprice =" +
    " (select Max(nprice) from (select market_id, Sum(price) as nprice from fp_store_aggregation "+
    "group by market_id) as table2)")
    lis = cursor.fetchall()
    print("market_id = " + lis[0][0] + ",  income = " + str(lis[0][1]))


def expensive_towns( cursor):
    cursor.execute("select  distinct market_id from fp_store_aggregation")
    dic = dict()
    for i in cursor.fetchall():
        string = i[0]
        dic[string] = 0

    cursor.execute("select  distinct product_id from fp_store_aggregation")
    for i in cursor.fetchall():
        string = i[0]
        cursor.execute("select market_id, product_id, price, has_sold from fp_store_aggregation"+
        " where product_id = '" +string+ "'")
        answer = cursor.fetchall()
        maxi = 0;
        market_id =0
        for i in range(len(answer)):
            if (answer[i][2]/answer[i][3]) > maxi:
                maxi = (answer[i][2]/answer[i][3])
                market_id = answer[i][0]
        dic[market_id] +=1
    maxi = max(dic.values())
    for i in dic.keys():
        if dic[i] == maxi:
            print("market with the id=" + i + " is the most expensive market")      

def variance(li):
    num=0
    for i in range(len(li)):
        num += li[i]

    ave = num / len(li)
    num=0
    for i in range(len(li)):
        num += ((li[i]-ave)*(li[i]-ave))
    num = num / len(li)
    num = math.sqrt(num)
    num = num / ave
    return num


def variable_product(cursor):
    cursor.execute("select  distinct product_id from fp_store_aggregation")
    dic = dict()
    lis = cursor.fetchall()
    for i in lis:
        product = i[0]
        dic[product] = 0
        cursor.execute("select price, has_sold from fp_stores_data where product_id ='"+ product +"'")
        lis = cursor.fetchall()
        for i in range(len(lis)):
            lis[i] = lis[i][0]/lis[i][1]
        dic[product] = variance(lis)
        if product == "9655":
            print(lis)

    maxi = max(dic.values())
    for i in dic.keys():
        if dic[i] == maxi:
            print("product with the id=" + i + " is the most variable product")      








try:
    connection = psycopg2.connect(user = "amin",
                                  password = "",
                                  host = "127.0.0.1",
                                  port = "5432",
                                  database = "fpdb")
    cursor = connection.cursor()

    most_favorite_product_in_every_town(cursor)
    best_store_in_case_of_income(cursor)
    expensive_towns(cursor)
    variable_product(cursor)

except (Exception, psycopg2.Error) as error :
    print ("Error while connecting to PostgreSQL", error)
finally:
    #closing database connection.
        if(connection):
            cursor.close()
            connection.close()
            print("PostgreSQL connection is closed")



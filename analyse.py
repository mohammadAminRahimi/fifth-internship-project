import psycopg2
import math 

# this function helps us to find the product that are most wanted in every town
def most_favorite_product_in_every_town(cursor):
    f = open("/Users/amin/Desktop/fifth-internship-project/fifth-internship-project/output/most_favorite_product_in_every_town.txt", "w")
    # in this query we get the name of every city which there is in the database
    cursor.execute("select distinct city from fp_city_aggregation")
    for i in cursor.fetchall():
        string = i[0]
        # for the each city we find in the above query, we find out the best product in this query
        cursor.execute("select city, has_sold, product_id, price  from fp_city_aggregation where city ='" + string + "' and has_sold = (select Max(has_sold) from fp_city_aggregation where city ='" + string + "')" )
        lis = cursor.fetchall()
        city = lis[0][0]
        has_sold = lis[0][1]
        product_id = lis[0][2]
        price = lis[0][3]
        # displaying information

        # print("city = " + (city.decode('utf-8')[::-1]) + ",    product_id = " + product_id + ",  has_sold = "+ str(has_sold) 
        # + ",  total_money_earned = "+ str(has_sold * price))
        f.write("city = " + (city) + ",    product_id = " + product_id + ",  has_sold = "+ str(has_sold) 
        + ",  total_money_earned = "+ str(has_sold * price) + '\n')
    f.close()

# in this function we find the market which sells most
def best_store_in_case_of_income(cursor):
    f = open("/Users/amin/Desktop/fifth-internship-project/fifth-internship-project/output/best_store_in_case_of_income.txt", "w")
    # in this query we get the name of every market which there is in the database
    cursor.execute("select  distinct market_id from fp_store_aggregation")
    # in this query we sum the money that each market gain from selling every produt and then we select 
    # the market which sells the most
    cursor.execute("select market_id, mprice from ( select market_id, Sum(price) as mprice " +
    "from fp_store_aggregation group by market_id) as table1 where mprice =" +
    " (select Max(nprice) from (select market_id, Sum(price) as nprice from fp_store_aggregation "+
    "group by market_id) as table2)")
    lis = cursor.fetchall()
    f.write("market_id = " + lis[0][0] + ",  income = " + str(lis[0][1]) + "\n")
    f.close()

# we find the town which the product has been sold there more expensive than other places
def expensive_markets( cursor):
    f = open("/Users/amin/Desktop/fifth-internship-project/fifth-internship-project/output/expensive_markets.txt", "w")
    # at first we select all market 
    cursor.execute("select  distinct market_id from fp_store_aggregation")
    dic = dict()
    for i in cursor.fetchall():
        string = i[0]
        dic[string] = 0
    # we select all products and then we loop through them
    # and find out which market sells that product more expensive than other places
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
            f.write("market with the id=" + i + " is the most expensive market \n")     
    f.close
# as the name of function suggests this function compute the standard deviation of a list of data
def standard_deviation(li):
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

# we find out the product which its price varies a lot
def variable_product(cursor):
    f = open("/Users/amin/Desktop/fifth-internship-project/fifth-internship-project/output/variable_product.txt", "w")
    # at first we get all product in the db
    cursor.execute("select  distinct product_id from fp_store_aggregation")
    dic = dict()
    lis = cursor.fetchall()
    for i in lis:
        product = i[0]
        dic[product] = 0
        # in this query we find out all the prices that a product has been sold and then we make a list of them
        #  in the next step we give the list to a function and get standard deviation.
        cursor.execute("select price, has_sold from fp_stores_data where product_id ='"+ product +"'")
        lis = cursor.fetchall()
        for i in range(len(lis)):
            lis[i] = lis[i][0]/lis[i][1]
        dic[product] = standard_deviation(lis)
    maxi = max(dic.values())
    for i in dic.keys():
        if dic[i] == maxi:
            f.write("product with the id=" + i + " is the most variable product\n")  
    f.close()



def ave(lis):
    ans = 0 
    summ = 0
    for i in lis:
        summ += i

    ans = summ / len(lis)
    return ans


# this function helps us to find out how much a market in in city sells on average
def average_income_of_each_stores_of_a_city(cursor):
    f = open("/Users/amin/Desktop/fifth-internship-project/fifth-internship-project/output/average_income_of_each_stores_of_a_city.txt", "w")
    # at first we get all cities in the db
    cursor.execute("select distinct city from fp_city_aggregation")
    ans_dic = dict()
    for i in cursor.fetchall():
        city = i[0] 
        ans_dic[city] = 0
        lis = list()
        # for each city we select all of its market
        cursor.execute("select distinct market_id from fp_stores_data where city='" +  city + "'")
        for j in cursor.fetchall():
            markets = j[0]
            # we get total income of the markets in a city
            cursor.execute("select Sum(price) from fp_store_aggregation where market_id='" + markets + "'")
            lis.append(cursor.fetchall()[0][0])
        # we calculate average income of markets of a city
        ans_dic[city] = ave(lis)

    for i in ans_dic.keys():
        f.write("average income of store in the city=" + i + " is " + str(ans_dic[i]) + "\n")
    f.close()

# we find out for each city what product is needed most
def product_most_needed_in_each_city(cursor):
    f = open("/Users/amin/Desktop/fifth-internship-project/fifth-internship-project/output/product_most_needed_in_each_city.txt", "w")
    # at first we select all cities 
    cursor.execute("select distinct city from fp_city_aggregation")
    ans_dic = dict()
    for i in cursor.fetchall():
        city = i[0] 
        # by the help of fp_city_aggregation table we get the product that is most needed
        #  in each city (quantity of that product is real low so it is needed)
        cursor.execute("select  product_id, quantity, has_sold from fp_city_aggregation where city = '" + city + "'" )
        res = cursor.fetchall()
        product, quantity, has_sold = res[0]
        for i in res:
            if quantity/has_sold < i[1]/i[2]:
                product, quantity, has_sold = i
        ans_dic[city] = product

    for i in ans_dic.keys():
        f.write("in the " + i  +" product with the id=" + ans_dic[i] +
        " is most needed\n")
    f.close()



try:
    # first of all we make the database connection
    connection = psycopg2.connect(user = "amin",
                                  password = "",
                                  host = "127.0.0.1",
                                  port = "5432",
                                  database = "fpdb")
    cursor = connection.cursor()
    # we are goin to analyze data in the database in 6 ways by the help of below functions
    most_favorite_product_in_every_town(cursor)
    best_store_in_case_of_income(cursor)
    expensive_markets(cursor)
    variable_product(cursor)
    average_income_of_each_stores_of_a_city(cursor)
    product_most_needed_in_each_city(cursor)
except (Exception, psycopg2.Error) as error :
    print ("Error while connecting to PostgreSQL", error)
finally:
    #closing database connection.
        if(connection):
            cursor.close()
            connection.close()
            print("PostgreSQL connection is closed")



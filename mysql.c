#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql.h>


#define MYSQL_CHAR		MYSQL_TYPE_TINY		
#define MYSQL_SHORT		MYSQL_TYPE_SHORT	
#define MYSQL_INT		MYSQL_TYPE_LONG		
#define MYSQL_LONG		MYSQL_TYPE_LONG		
#define MYSQL_LONGLOGN	MYSQL_TYPE_LONGLONG	
#define MYSQL_FLOAT		MYSQL_TYPE_FLOAT	
#define MYSQL_DOUBLE	MYSQL_TYPE_DOUBLE	
#define MYSQL_STRING	MYSQL_TYPE_STRING	
#define MYSQL_TEXT		MYSQL_TYPE_BLOB		

#define MYSQL_NULL_Y	1 //If Y, the value is binding to NULL.
#define MYSQL_NULL_N	0 //Use N if the binding value is not NULL.

#define FAILURE         (-1)
#define SUCCESS         0

#define STRING_SIZE

typedef struct mysql_bind_st
{
	unsigned long mysql_null;
	unsigned long str_data_size;
	
}MYSQL_STRING_BIND_ST;

int mys_bind_param(MYSQL_BIND *bind, int Sequence, void *valuep, MYSQL_STRING_BIND_ST *user,  int data_type,  int null_bind_yn)
{
    unsigned long str_length = 0;
   
	if(null_bind_yn == MYSQL_NULL_Y)
	{
		user[Sequence].mysql_null = 1;
	}
	else
	{
		user[Sequence].mysql_null = 0;
	}

	if(data_type == MYSQL_STRING || data_type == MYSQL_CHAR || data_type == MYSQL_TEXT)
	{
		*size = strlen(valuep);
		bind[Sequence].buffer_type= data_type;
		// bind[Sequence].buffer= (char *) test;
		bind[Sequence].buffer= (char *) valuep;
		bind[Sequence].buffer_length= (unsigned long)sizeof(valuep);
		bind[Sequence].is_null= &user[Sequence].mysql_null;
		bind[Sequence].length= &user[Sequence].str_data_size;
	}
	else
	{
		/* This is a number type, so there is no need
   		to specify buffer_length */
		bind[Sequence].buffer_type= data_type;
		bind[Sequence].buffer = (char *) valuep;
		bind[Sequence].is_null= null_bind_yn;
		bind[Sequence].length= &user[Sequence].str_data_size;

	}

}

MYSQL *mys_connect(char *host, char *username, char *password, char *db_name, int *port, char *encoding)
{
	MYSQL *conn;

	
	conn = mysql_init(NULL);	
	if (conn == NULL)
	{
		printf("mys_connection init failed: %s", mysql_error(conn));
		return NULL;
	}

	mysql_options(conn, MYSQL_OPT_COMPRESS, 0); 

	if (mysql_real_connect(
		conn, 					                // pointer to connection handler 
		host, 				                    // host to connect to 
		username, 				                // user name 
		password, 				                // password 
		db_name, 			                    // database to use 
		port, 				                    // port (use default) 
		NULL, 					                // socket (use default) 
		CLIENT_MULTI_STATEMENTS ) == NULL)		// flags (none) 
		
	{
		printf("mysql_real_connect failed: %s", mysql_error(conn));
		mysql_close(conn);
		return NULL;
	}

	if (!mysql_set_character_set(conn,encoding))
	{
		shp_log(LM_DEBUG, "New client character set : %s\n",mysql_character_set_name(conn));
	}

	return conn;
}

int mysql_send_query(MYSQL *conn)
{
    MYSQL_STMT	*stmt;
	MYSQL_BIND	*bind = NULL;
    char insert_query[2000]="INSERT INTO test_table(col1,col2,col3) VALUES(?,?,?)";
    int param_count = 0;
    int integer = 10;
	int short_test = 1000;
    char str_data[STRING_SIZE]={0,};
    MYSQL_STRING_BIND_ST *user = NULL; //binding해야 할 string의 개수대로 malloc해서 사용

    stmt = mysql_stmt_init(conn);
    
    if (!stmt)
    {
        printf(" mysql_stmt_init(), out of memory\n");
        continue;
    }

    if (mysql_stmt_prepare(stmt, insert_query, strlen(insert_query)))
    {
        printf(" mysql_stmt_prepare(), INSERT failed\n");
        printf(" %s\n", mysql_stmt_error(stmt));
        continue;
    }

    param_count= mysql_stmt_param_count(stmt);
    if(param_count > 0)
    {
        bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * param_count);
        user = (MYSQL_STRING_BIND_ST *)malloc(sizeof(MYSQL_STRING_BIND_ST) * param_count);
        memset(bind, 0X00, sizeof(MYSQL_BIND)*param_count);
        memset(user, 0X00, sizeof(MYSQL_STRING_BIND_ST)*param_count);

    }
    
    mys_bind_param(bind, 0, (void *) &integer, user, MYSQL_INT, MYSQL_NULL_N);
    strncpy(str_data,"MYSQL",STRING_SIZE);
    mys_bind_param(bind, 1, (void *) str_data, user, MYSQL_STRING, MYSQL_NULL_N);
    mys_bind_param(bind, 2, (void *) &short_test, user,MYSQL_SHORT, MYSQL_NULL_N);

    if (mysql_stmt_bind_param(stmt, bind))
    {
        printf(" mysql_stmt_bind_param() failed\n");
        printf(" %s\n", mysql_stmt_error(stmt));
        return FAILURE;
    }

    if (mysql_stmt_execute(stmt))
    {
        printf("%d\n",__LINE__);
        printf(" mysql_stmt_execute(), 1 failed\n");
        printf(" %s\n", mysql_stmt_error(stmt));
        return FAILURE;
    }

    if (mysql_stmt_close(stmt))
    {
        /* mysql_stmt_close() invalidates stmt, so call          */
        /* mysql_error(mysql) rather than mysql_stmt_error(stmt) */
        printf(" failed while closing the statement\n");
        printf(" %s\n", mysql_error(conn));
        retrun FAILURE;
    }

    return SUCCESS;
}

int mys_close(MYSQL *conn)
{
    if (conn)
    {
        mysql_close(conn);
        conn = NULL;
        return SUCCESS;
    }
    else
        return FAILURE;
}


int main()
{
    MYSQL *conn;
    char *host = "";
    char *username = "";
    char *password = "";
    char *db_name = "";
    int *port = 0;
    char *encoding = "UTF-8";

    if((conn = mys_connect(host,username,password,db_name,port,encoding)) == NULL)
    {
        printf("mysql connection failed\n");
        return -1;
    }

    if(mysql_send_query(conn) == FAILURE)
    {
        printf("mysql send query failed\n");
        if(mys_close(conn) == FAILURE)
        {
            printf("mysql connection close failed\n");
        }
        else
        {
            printf("mysql connection close successed\n");
        }
        return -1;
    }

    if(mys_close(conn) == FAILURE) 
    {
        printf("mysql connection close failed\n");
    }
    else
    {
        printf("mysql connection close successed\n");
    }

    return 0;
}
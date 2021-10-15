#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql.h>


#define MYSQL_CHAR		MYSQL_TYPE_TINY		//length 1 
#define MYSQL_SHORT		MYSQL_TYPE_SHORT	//length 2 small int
#define MYSQL_INT		MYSQL_TYPE_LONG		//length 2 small int
#define MYSQL_LONG		MYSQL_TYPE_LONG		//length 4 integer
#define MYSQL_LONGLOGN	MYSQL_TYPE_LONGLONG	//length 8 
#define MYSQL_FLOAT		MYSQL_TYPE_FLOAT	//length 4
#define MYSQL_DOUBLE	MYSQL_TYPE_DOUBLE	//length 8
#define MYSQL_STRING	MYSQL_TYPE_STRING	//length data length
#define MYSQL_TEXT		MYSQL_TYPE_BLOB		//length data length

#define MYSQL_NULL_Y	1 //If Y, the value is binding to NULL.
#define MYSQL_NULL_N	0 //Use N if the binding value is not NULL.

#define FAILURE         (-1)
#define SUCCESS         0

int mys_bind_param(MYSQL_BIND *bind, int Sequence, void *valuep, int data_type,  unsigned long *size, int null_bind_yn)
{
    unsigned long str_length = 0;

	if(data_type == MYSQL_STRING || data_type == MYSQL_CHAR || data_type == MYSQL_TEXT)
	{
		*size = strlen(valuep);
		bind[Sequence].buffer_type= data_type;
		// bind[Sequence].buffer= (char *) test;
		bind[Sequence].buffer= (char *) valuep;
		bind[Sequence].buffer_length= sizeof(valuep);
		bind[Sequence].is_null= null_bind_yn;
		bind[Sequence].length= size;
	}
	else
	{
		/* This is a number type, so there is no need
   		to specify buffer_length */
		bind[Sequence].buffer_type= data_type;
		bind[Sequence].buffer = (char *) valuep;
		bind[Sequence].is_null= null_bind_yn;
		bind[Sequence].length= size;

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
    unsigned long str_data_size=0;
    int param_count = 0;
    int integer = 10;
	int short_test = 1000;

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
        memset(bind, 0X00, sizeof(MYSQL_BIND)*param_count);
    }
    
    mys_bind_param(bind, 0, (void *) &integer, MYSQL_INT, 0, MYSQL_NULL_N);
    str_data_size=0;
    mys_bind_param(bind, 1, (void *) v_recv, MYSQL_STRING, &str_data_size, MYSQL_NULL_N);
    mys_bind_param(bind, 2, (void *) &short_test, MYSQL_SHORT, 0, MYSQL_NULL_N);

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
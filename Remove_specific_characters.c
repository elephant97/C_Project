#include <stdio.h>
#include <string.h>

char *a(char *ver_tmp);
char *b(char *ver_tmp);
char *c(char *ver_tmp);

void main()
{

        char ver_temp[10] = "10.2.3";
        char ver_temp1[10] = "10.2.3";
        char *a_result = NULL; *b_result = NULL;
        a_result=a(ver_temp);
        b_result=b(ver_temp1);
}

char *a(char *ver_tmp) //for문으로 제거 로직
{
        int i;

        for(i=0; i < strlen(ver_tmp); i++)
        {
                if(ver_tmp[i] == '.')
                {
                        strcpy(&ver_tmp[i], &ver_tmp[i+1]);
                }
        }

        printf("finish=[%s]\n",ver_tmp);

        return ver_tmp;
}

char *b(char *ver_tmp) //숫자 정수화 및 strtok_r을 통한 제거 로직
{
    int *num_tmp[10] = {0, };
    char *num_cut=NULL;
    int num_cnt = 0;
    char *saveptr = NULL;
    int out[10] = {0,};
    int i = 0;

     num_cut = strtok_r(ver_tmp,".",&saveptr);
    while(num_cut != NULL)
    {
        num_tmp[num_cnt] = atoi(num_cut);
        num_cnt++;
        num_cut = strtok_r(NULL,".",&saveptr);
    }

    for(i=0; i<num_cnt; i++)
    {
        if(i == 0)
        {
          sprintf(ver_tmp,"%d",num_tmp[i]);
        }
        else
        {
          sprintf(ver_tmp,"%s%d",ver_tmp,num_tmp[i]);
        }
    }
     printf("finish=[%s]\n",ver_tmp);

    return ver_tmp;
}
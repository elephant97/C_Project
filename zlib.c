#include<stdio.h>
#include<strings.h>
#include<string.h>
#include <zlib.h>

#define F_OK 0
#define SUCCESS 0
#define FAILURE -1


int file_compress(char *file_path)    //���� ���� �Լ�
{
	char gzfilename[256] = {0,};
	gzFile zf;
	FILE *comp_fp;
	int n=0;
	char buf[1024]={0,};
	int log_cnt=0;



	sprintf(gzfilename,"%s.gz",file_path);
	printf("file_compress - gzfilename [%s]\n",gzfilename);

	if ((comp_fp = fopen(file_path, "rb")) == NULL) 
	{
		printf("file_compress - logfile fopen error[%s]\n",file_path);
		return FAILURE;
	}

	if ((zf = gzopen(gzfilename, "wb")) == NULL) 
	{
		printf("file_compress - gzfile gzopen error[%s]\n",gzfilename);
		//gzclose(zf);
		fclose(comp_fp);
		return FAILURE;
	}

	while((n = fread(buf, sizeof(char), 255, comp_fp)) > 0)
	{
		// printf("file_compress - gzfile fread error[%s]\n",gzfilename);
		if (gzwrite(zf, buf, n) < 0) 
		{
			printf("file_compress - gzwrite error[%s]\n",gzerror(zf, &lerrno));
			gzclose(zf);
			fclose(comp_fp);
			return FAILURE;
		}
	}

	gzclose(zf);
	fclose(comp_fp);

	return SUCCESS;
}

int main()
{
    char file_path[256]={0,};

    printf("enter the file path:\n");
    scanf_s("%s",file_path,sizeof(file_path));

    if(access(file_path, F_OK) == SUCCESS) //������ ���� ���� Ȯ��
    {
        if(file_compress(file_path) == FAILURE) //���� ���� ����
        {
            printf("file compress failed!!\n");
            return FAILURE;
        }
        else
        {
            printf("file compress success!!\n");
            if(unlink(file_path) == SUCCESS) //���� �� �������� ����
            {	
                printf("Success to delete the original file after completion of compression[path=%s] \n", file_path);
            }
            else
            {
                printf("Failed to delete the original file after completion of compression[path=%s] \n", file_path);
            }
        }
    }
    else
    {
        printf("There is no such file[%s]!!\n",file_path);
    }

    return SUCCESS;
}
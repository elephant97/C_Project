#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<io.h>



#define xfree(p)		do {if(p != NULL){free(p); p=NULL;}} while(0)
#define F_OK                 0

#define FAILED               (-1)
#define SUCCESS              0

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif


//module type set
#define COMMON               0
#define AGENT_MGR            1
#define RT                   2	
#define INST_SNDR            3
#define SESS_SNDR            4
#define SNAP_SNDR            5
#define SQL_SNDR             6
#define LOGMS_MGR           11
#define MGMT_SVR            12
#define RELAY_SVR           13
#define SCHED_JOB           14
#define SCHED_SVR           15
#define SNAP_SVR            16
#define ALERT_SVR           17
#define MAX_MODULE_INDEX    14


//log load set
#define OLD                 0
#define NEW                 1



//category set ascii
#define INI                 73  //I
#define SYS                 83  //S
#define PRC                 80  //P
#define MNG                 77  //M
#define NET                 78  //N
#define DBP                 68  //D
#define DMA                 65  //A
#define MAX_CATEGORY_INDEX  8



typedef struct _MODULE_CODE
{
    int code;
    char *module_name;
} MODULE_CODE;


typedef struct _CATEGORY_CODE
{
    char code;
    int  last_num;
    char category_name[3+1];
} CATEGORY_CODE;

typedef struct _FILE_LIST
{
    char file_name[128+1];
}FILE_LIST;

typedef struct _LOG_TEXT
{
    int level_num;
    char log_level_text[18+1];
    char level_name[5+1];
    char level_label;
} LOG_TEXT;

enum LOG_LEVEL
{
    ERROR = 0,
    WARN,
    INFO,

    MAX_LOG_LEVEL_INDEX,
};

MODULE_CODE MODULE_LIST[MAX_MODULE_INDEX] =
{
    COMMON,         "SRC",
    AGENT_MGR,      "AGNET_MGR",
    RT,             "SHP_RT",
    INST_SNDR,      "INST_SNDR",
    SESS_SNDR,      "SESS_SNDR",
    SNAP_SNDR,      "SNAP_SNDR",
    SQL_SNDR,       "SQL_SNDR",
    LOGMS_MGR,      "LOGMS_MGR",
    MGMT_SVR,       "MGMT_SVR",
    RELAY_SVR,      "RELAY_SVR",
    SCHED_JOB,      "SCHED_JOB",
    SCHED_SVR,      "SCHED_SVR",
    SNAP_SVR,       "SNAP_SVR",
    ALERT_SVR,      "ALERT_SVR",
};

LOG_TEXT LEVEL_LIST[MAX_LOG_LEVEL_INDEX] =
{
    ERROR,       "ERROR_LOG_TEXT.txt",          "ERROR",            'E',
    WARN,        "WARN_LOG_TEXT.txt",           "WARN",             'W',  
    INFO,        "INFO_LOG_TEXT.txt",           "INFO",             'I',  
};

CATEGORY_CODE WARN_CATEGORY_LIST[MAX_CATEGORY_INDEX] =
{
    INI,            0,             "INI",                       
    SYS,            0,             "SYS",               
    PRC,            0,             "PRC",               
    MNG,            0,             "MNG",               
    NET,            0,             "NET",               
    DBP,            0,             "DBP",               
    DMA,            0,             "DMA",               
};

CATEGORY_CODE ERR_CATEGORY_LIST[MAX_CATEGORY_INDEX] =
{
    INI,            0,             "INI",                       
    SYS,            0,             "SYS",               
    PRC,            0,             "PRC",               
    MNG,            0,             "MNG",               
    NET,            0,             "NET",               
    DBP,            0,             "DBP",               
    DMA,            0,             "DMA",               
};

CATEGORY_CODE INFO_CATEGORY_LIST[MAX_CATEGORY_INDEX] =
{
    INI,            0,             "INI",                       
    SYS,            0,             "SYS",               
    PRC,            0,             "PRC",               
    MNG,            0,             "MNG",               
    NET,            0,             "NET",               
    DBP,            0,             "DBP",               
    DMA,            0,             "DMA",               
};





char g_product_code[1+1];
int g_module_code = 0;
char g_module_path[1024+1];
char backup_dir[2048+1];


void getModuleName(int argc, char **argv);
int getModuleCodeExtrac(char *module_name);
// void selectCategory(char *filename, int *category);
void selectCategory(char *filename, char *category);
int logConvertTask(char *filename, char *category, int status);
FILE_LIST *findCFile(int *file_cnt);
int LogSet(char *contents, int status, char *category, char *logcode, int levelnum);
// int infoLogSet(char *contents, int status, char *category, char *logcode);
// int errLogSet(char *contents, int status, char *category, char *logcode);
// int warnLogSet(char *contents, int status, char *category, char *logcode);
// void str_trim(char *buf);
void ltrim(char *src);
int logCntLoad(char *category, int nownum, int level_num);
int makeLogLine(char *logsstring, char *LEVEL, char *category, int lognum, char *logcontents, int level_num);


int main(int argc, char **argv)
{
    int file_cnt = 0;
    FILE_LIST *file_list = NULL;
    int i = 0;
    char category[1+1] = {0,};

    getModuleName(argc, argv);
    printf("##[%d]\n",__LINE__);
    if((file_list = findCFile(&file_cnt)) == NULL)
    {
        printf("C file find failed[%s]\n",g_module_path);
        xfree(file_list);
    }
    printf("##[%d]\n",__LINE__);
    printf("##cnt[%d]\n",file_cnt);
    for(i = 0; i < file_cnt; i++)
    {
        printf("##[%d]\n",__LINE__);
        printf("##%s\n",file_list[i].file_name);
        selectCategory(file_list[i].file_name, category);
        printf("##[%d]\n",__LINE__);
        if(logConvertDir() != SUCCESS)
        {
            exit(0);
        }
        logConvertTask(file_list[i].file_name, category, OLD);
    }

    for(i = 0; i < file_cnt; i++)
    {
        printf("##[%d]\n",__LINE__);
        printf("##%s\n",file_list[i].file_name);
        selectCategory(file_list[i].file_name, category);
        printf("##[%d]\n",__LINE__);
        if(logConvertDir() != SUCCESS)
        {
            exit(0);
        }
        logConvertTask(file_list[i].file_name, category, NEW);
    }

    for(i = 0; i < 3; i++)
    {
        logMerge(LEVEL_LIST[i].level_num);
    }
        

    xfree(file_list);
}

int logMerge(int num)
{
    char logtext_path[1024+1] = {0,};
    char maintext_path[1024+1] = {0,};
    char division[20+1] = {0,};
    char buf[512+1] = {0,};
    FILE *mainfile_fd;
    FILE *readfile_fd;


    sprintf(logtext_path, "%s\\logConvert\\%s",g_module_path,LEVEL_LIST[num].log_level_text);
    sprintf(maintext_path, "%s\\logConvert\\LOG_TEXT.txt",g_module_path);
    sprintf(division, "MODULE:%d_%s\t\n",g_module_code,LEVEL_LIST[num].level_name);
    if(access(logtext_path, F_OK) == 0)
    {
        readfile_fd = fopen(logtext_path, "r");
        if(readfile_fd == NULL)
        {
            printf("logMerge readfile_fd open failed[%s]\n",logtext_path);
            return FAILED;
        }

        mainfile_fd = fopen(maintext_path, "a");
        if(mainfile_fd == NULL)
        {
            printf("logMerge mainfile_fd open failed[%s]\n",maintext_path);
            fclose(readfile_fd);
            return FAILED;
        }

        fputs(division,mainfile_fd);

        while(fgets(buf, 512, readfile_fd) != NULL)
        {
            fputs(buf,mainfile_fd);
        } 

    }

    fclose(readfile_fd);
    fclose(mainfile_fd);


    return;
}

int logConvertDir()
{
    sprintf(backup_dir,"%s\\logConvert",g_module_path);

    if(access(backup_dir,F_OK) != 0)
    {
        if(_mkdir(backup_dir) != 0)
		{
			printf("backup_dir make failed[%s]\n",backup_dir);
			return FAILED;
		}
    }

    return SUCCESS;
}

void selectCategory(char *filename, char *category)
{
    printf("##[%d]\n",__LINE__);
    printf("##%c\n",NET);
    printf("##%s\n",filename);
    printf("##[%d]\n",__LINE__);
    if(strstr(filename,"protocol") != NULL || strstr(filename, "socklib") != NULL)
    {
        printf("##[%d]\n",__LINE__);
        // *category = NET;
        
        category[0] = NET;
    }
    else if(strstr(filename, "proc") != NULL)
    {
        printf("##[%d]\n",__LINE__);
        // *category = DBP;
        category[0] = DBP;
    }
    else
    {
        printf("##[%d]\n",__LINE__);
        // *category = SYS;
        category[0] = SYS;
    }
    // printf("##category[%c]\n",CATEGORY_LIST[*category].category_name);
    printf("##category[%c]\n",category[0]);
}

int logConvertTask(char *filename, char *category, int status)
{
    char full_path[1024+1] = {0,};
    char new_full_path[1024+1] = {0,};
    
    char buf[512]={0,};
    char new_buf[512]={0,};
    char *tok, *saveptr, *tok2, *saveptr2;
    FILE *target_fd;
    FILE *write_fd;
    char logcode[20+1] = {0,};
    char temp[512+1] = {0,};
    char *ptr = NULL;
   

    sprintf(full_path, "%s\\%s",g_module_path,filename);

    //OLD LOG LOAD
    if(status == OLD)
    {
        target_fd = fopen(full_path, "r+");
        if(target_fd == NULL)
        {
            printf("file open failed[%s]\n",full_path);
            return FAILED;
        }

        while(fgets(buf, 512, target_fd) != NULL)
        {
            if(buf[0] == '\0') continue;

            strcpy(temp,buf);
            ltrim(temp);
            tok = strtok_s(temp, "\"", &saveptr);
            tok = strtok_s(NULL, "\"", &saveptr);
            if(tok == NULL) continue;
            printf("##tok[%s]\n",tok);
            ltrim(tok);
            printf("##tok[%s]\n",tok);

            if(memcmp(temp, "SHP_LOG_ERROR", 13) == 0)
            {
                if(LogSet(tok, OLD, category, " ", ERROR) == 2) continue;
            }
            else if(memcmp(temp, "SHP_LOG_INFO", 12) == 0)
            {
                if(LogSet(tok, OLD, category, " ", INFO) == 2) continue;
            }
            else if(memcmp(temp, "SHP_LOG_WARN", 12) == 0)
            {
               if(LogSet(tok, OLD, category, " ", WARN) == 2) continue;
            }

        }
        fclose(target_fd);
        printf("old log get finished[%s]\n",filename);
    }
    else if(status == NEW) //NEW LOG LOAD
    {
        sprintf(new_full_path,"%s\\%s",backup_dir, filename);
        target_fd = fopen(full_path, "r");
        write_fd = fopen(new_full_path, "w");
        if(target_fd == NULL)
        {
            printf("file open failed[%s]\n",full_path);
            return FAILED;
        }
        if(write_fd == NULL)
        {
            printf("file open failed[%s]\n",full_path);
            fclose(target_fd);
            return FAILED;
        }

        while(fgets(buf, 512, target_fd) != NULL)
        {
            memset(logcode,0x00,sizeof(logcode));
            memset(new_buf,0x00,sizeof(new_buf));

            if(buf[0] == '\0') continue;

            strcpy(temp,buf);
            ltrim(temp);
            printf("temp[%s]\n",temp);


            if(memcmp(temp, "SHP_LOG_ERROR", 13) == 0)
            {
                tok = strtok_s(temp, "\"", &saveptr);
                tok = strtok_s(NULL, "\"", &saveptr);
                if(tok == NULL)
                {
                     fputs(buf,write_fd);
                     continue;
                }
                ltrim(tok);
                printf("##tok[%s]\n",tok);
                if(LogSet(tok, NEW, category, logcode, ERROR) == 2)
                {
                    fputs(buf,write_fd);
                    continue;
                }
            }
            else if(memcmp(temp, "SHP_LOG_INFO", 12) == 0)
            {
                tok = strtok_s(temp, "\"", &saveptr);
                tok = strtok_s(NULL, "\"", &saveptr);
                if(tok == NULL)
                {
                     fputs(buf,write_fd);
                     continue;
                }
                ltrim(tok);
                printf("##tok[%s]\n",tok);
                if(LogSet(tok, NEW, category, logcode, INFO) == 2) 
                {
                    fputs(buf,write_fd);
                    continue;
                }
            }
            else if(memcmp(temp, "SHP_LOG_WARN", 12) == 0)
            {
                tok = strtok_s(temp, "\"", &saveptr);
                tok = strtok_s(NULL, "\"", &saveptr);
                if(tok == NULL)
                {
                     fputs(buf,write_fd);
                     continue;
                }
                ltrim(tok);
                printf("##tok[%s]\n",tok);
                if(LogSet(tok, NEW, category, logcode ,WARN) == 2) 
                {
                    fputs(buf,write_fd);
                    continue;
                }
            }
            else
            {
                fputs(buf,write_fd);
                continue;
            }

            printf("##buf[%s]\n",buf);
            if((ptr = strstr(buf,tok)) != NULL)
            {
                printf("##buf[%s]\n",buf);
                tok2 = strtok_s(buf, "\"", &saveptr2);
                       strtok_s(NULL, "\"", &saveptr2);
                sprintf(new_buf,"%s\"%s\"%s",tok2,logcode,saveptr2);
                printf("newbuf[%s]\n",new_buf);
                fputs(new_buf,write_fd);
                printf("##[%d]\n",__LINE__);
            }
            else
            {
                printf("lost line ERROR!\n");
                break;
            }

        }
        fclose(write_fd);
        fclose(target_fd);
        printf("new log get finished[%s]\n",filename);
    }
    else
    {
        printf("UNKOWN STATUS[%d]\n", status);
    }
    
}

void ltrim(char *src)
{
    char *dst = src;

        /* find position of first non-space character */

    while(*src == ' ' || *src == '\t' || *src == '\t' || *src == '\r')
    {
        printf("+++[%c]\n",*src);
        src++;
    }

        /* nothing to do */
    if (dst==src) return;

        /* K&R style strcpy() */
    while ((*dst++ = *src++)) {;}

    return;
}

int LogSet(char *contents, int status, char *category, char *logcode, int levelnum)
{
    char logtxt_path[1024+1] = {0,};
    FILE *logtxt_fd;
    char code[5+1] = {0,};
    char temp[1024] = {0,};
    char *tok, *saveptr; 
    char logstring[2048+1] = {0,};
    char category_jud = '\0';
    int new_num = -1;


    sprintf(code, "%c%c%02d",LEVEL_LIST[levelnum].level_label,g_product_code[0],g_module_code);
    printf("code[%s]\n",code);

    if(status == OLD)
    {
        if(strncmp(contents,code,strlen(code)) != 0)
        {
            printf("UNKNOWN CODE[%s]\n",contents);
            return 2;
        }
        logCntLoad(&contents[4],atoi(&contents[5]),LEVEL_LIST[levelnum].level_num);
        makeLogLine(logstring,LEVEL_LIST[levelnum].level_name,&contents[4],atoi(&contents[5]), " ", LEVEL_LIST[levelnum].level_num);
    }
    else
    {
        if(strncmp(contents,code,strlen(code)) == 0)
        {
            return 2;
        }
        new_num = makeLogLine(logstring,LEVEL_LIST[levelnum].level_name,category, new_num, contents, LEVEL_LIST[levelnum].level_num);
        
        if(new_num > 0)
        {
            sprintf(logcode,"%s%c%03d",code,category[0],new_num);
            printf("new logcode[%s]\n",logcode);
        }
    }

    

    sprintf(logtxt_path,"%s\\logConvert\\%s",g_module_path,LEVEL_LIST[levelnum].log_level_text);
    logtxt_fd = fopen(logtxt_path, "a");
    if(logtxt_fd == NULL)
    {
        printf("file open failed[%s]",logtxt_path);
        return FAILED;
    }

    fprintf(logtxt_fd, "%s\n", logstring);
    fclose(logtxt_fd);
    return SUCCESS;

} 

// char *ltrim(char *buf)
// {
// 	char *p = buf;
//     int i = 0;

//     while(p[i] != NULL)
//     {
//         if (p[i] == ' ' || p[i] == '\n' || p[i] == '\t')
//         {
//             i++;
// 			continue;
//         }
// 		else
// 			break;
//     }

//     printf("@@@@@@%s\n",&p[i]);
//     memset(buf, 0x00, sizeof(buf));
//     reutnr (&p[i]);
// }

/* void frontSpaceDelete(char *buf)
{
    char *p = buf;

    while(p++ <= buf)
    {
        if (*p == ' ' || *p == '\n' || *p == '\t')
			*p = '\0';
		else
			break;
    }
} */

// int infoLogSet(char *contents, int status, char *category, char *logcode)
// {
//     char logtxt_path[1024+1] = {0,};
//     FILE *logtxt_fd;
//     char code[5+1] = {0,};
//     char temp[1024] = {0,};
//     char *tok, *saveptr; 
//     char logstring[2048+1] = {0,};
//     char *INFO = "INFO";
//     char category_jud = '\0';
//     int new_num = -1;


//     sprintf(code, "I%c%02d",g_product_code[0],g_module_code);
//     printf("code[%s]\n",code);

//     if(status == OLD)
//     {
//         if(strncmp(contents,code,strlen(code)) != 0)
//         {
//             printf("UNKNOWN CODE[%s]\n",contents);
//             return 2;
//         }
//         logCntLoad(&contents[4],atoi(&contents[5]));
//         makeLogLine(logstring,INFO,&contents[4],atoi(&contents[5]), " ");
//     }
//     else
//     {
//         if(strncmp(contents,code,strlen(code)) == 0)
//         {
//             return 2;
//         }
//         new_num = makeLogLine(logstring,INFO,category, new_num, contents);
        
//         if(new_num > 0)
//         {
//             sprintf(logcode,"%s%c%03d",code,category[0],new_num);
//             printf("new logcode[%s]\n",logcode);
//         }
//     }

    

//     sprintf(logtxt_path,"%s\\INFO_LOG_TEXT.txt",g_module_path);
//     logtxt_fd = fopen(logtxt_path, "a");
//     if(logtxt_fd == NULL)
//     {
//         printf("file open failed[%s]",logtxt_path);
//         return FAILED;
//     }

//     fprintf(logtxt_fd, "%s\n", logstring);
//     fclose(logtxt_fd);
//     return SUCCESS;

// } 

// int warnLogSet(char *contents, int status, char *category, char *logcode)
// {
//     char logtxt_path[1024+1] = {0,};
//     FILE *logtxt_fd;
//     char code[5+1] = {0,};
//     char temp[1024] = {0,};
//     char *tok, *saveptr; 
//     char logstring[2048+1] = {0,};
//     char *WARN = "WARN";
//     char category_jud = '\0';
//     int new_num = -1;


//     sprintf(code, "W%c%02d",g_product_code[0],g_module_code);
//     printf("code[%s]\n",code);

//     if(status == OLD)
//     {
//         if(strncmp(contents,code,strlen(code)) != 0)
//         {
//             printf("UNKNOWN CODE[%s]\n",contents);
//             return 2;
//         }
//         logCntLoad(&contents[4],atoi(&contents[5]));
//         makeLogLine(logstring,WARN,&contents[4],atoi(&contents[5]), " ");
//     }
//     else
//     {
//         if(strncmp(contents,code,strlen(code)) == 0)
//         {
//             return 2;
//         }
//         new_num = makeLogLine(logstring,WARN,category, new_num, contents);
        
//         if(new_num > 0)
//         {
//             sprintf(logcode,"%s%c%03d",code,category[0],new_num);
//             printf("new logcode[%s]\n",logcode);
//         }
//     }

    

//     sprintf(logtxt_path,"%s\\WARN_LOG_TEXT.txt",g_module_path);
//     logtxt_fd = fopen(logtxt_path, "a");
//     if(logtxt_fd == NULL)
//     {
//         printf("file open failed[%s]",logtxt_path);
//         return FAILED;
//     }

//     fprintf(logtxt_fd, "%s\n", logstring);
//     fclose(logtxt_fd);
//     return SUCCESS;

// } 

int makeLogLine(char *logsstring, char *LEVEL, char *category, int lognum, char *logcontents, int level_num)
{
    int num = 0;
    int i = 0;
    
    if(level_num == ERROR)
    {
        for(i = 0; i < MAX_CATEGORY_INDEX; i++)
        {
            if(category[0] == ERR_CATEGORY_LIST[i].code) break;
        }
        
        if(lognum == -1)
        {
            ERR_CATEGORY_LIST[i].last_num++;
            num =  ERR_CATEGORY_LIST[i].last_num;
        }
        else
        {
            num = lognum;
        }

        sprintf(logsstring,"%s\t%s\t%03d\t%s\t", LEVEL, ERR_CATEGORY_LIST[i].category_name, num, logcontents);
    }
    else if (level_num == INFO)
    {
        for(i = 0; i < MAX_CATEGORY_INDEX; i++)
        {
            if(category[0] == INFO_CATEGORY_LIST[i].code) break;
        }
        
        if(lognum == -1)
        {
            INFO_CATEGORY_LIST[i].last_num++;
            num =  INFO_CATEGORY_LIST[i].last_num;
        }
        else
        {
            num = lognum;
        }

        sprintf(logsstring,"%s\t%s\t%03d\t%s\t", LEVEL, INFO_CATEGORY_LIST[i].category_name, num, logcontents);
    }
    else if(level_num == WARN)
    {
        for(i = 0; i < MAX_CATEGORY_INDEX; i++)
        {
            if(category[0] == WARN_CATEGORY_LIST[i].code) break;
        }
        
        if(lognum == -1)
        {
            WARN_CATEGORY_LIST[i].last_num++;
            num =  WARN_CATEGORY_LIST[i].last_num;
        }
        else
        {
            num = lognum;
        }

        sprintf(logsstring,"%s\t%s\t%03d\t%s\t", LEVEL, WARN_CATEGORY_LIST[i].category_name, num, logcontents);
    }
    else
    {
        printf("makeLogLine - UNKOWN LEVEL[%d]\n",LEVEL);
    }

    
    printf("logstring[%s]\n",logsstring);

    if(lognum == -1) return num;
    else return SUCCESS;
}

int logCntLoad(char *category, int nownum, int LEVEL)
{
    int ret = SUCCESS;

    printf("nownum[%d]\n",nownum);
    printf("category[%c]\n",category[0]);
    if(LEVEL == ERROR)
    {
        switch(category[0])
        {
            case INI:
                if(ERR_CATEGORY_LIST[0].last_num < nownum) ERR_CATEGORY_LIST[0].last_num = nownum;
                break;
            case SYS:
                if(ERR_CATEGORY_LIST[1].last_num < nownum) ERR_CATEGORY_LIST[1].last_num = nownum;
                break;
            case PRC:
                if(ERR_CATEGORY_LIST[2].last_num < nownum) ERR_CATEGORY_LIST[2].last_num = nownum;
                break;
            case MNG:
                if(ERR_CATEGORY_LIST[3].last_num < nownum) ERR_CATEGORY_LIST[3].last_num = nownum;
                break;
            case NET:
                if(ERR_CATEGORY_LIST[4].last_num < nownum) ERR_CATEGORY_LIST[4].last_num = nownum;
                break;
            case DBP:
                if(ERR_CATEGORY_LIST[5].last_num < nownum) ERR_CATEGORY_LIST[5].last_num = nownum;
                break;
            case DMA:
                if(ERR_CATEGORY_LIST[6].last_num < nownum) ERR_CATEGORY_LIST[6].last_num = nownum;
                break;
            default:
                ret = FAILED;
                break;
        }
    }
    else if(LEVEL == INFO)
    {
        switch(category[0])
        {
            case INI:
                if(INFO_CATEGORY_LIST[0].last_num < nownum) INFO_CATEGORY_LIST[0].last_num = nownum;
                break;
            case SYS:
                if(INFO_CATEGORY_LIST[1].last_num < nownum) INFO_CATEGORY_LIST[1].last_num = nownum;
                break;
            case PRC:
                if(INFO_CATEGORY_LIST[2].last_num < nownum) INFO_CATEGORY_LIST[2].last_num = nownum;
                break;
            case MNG:
                if(INFO_CATEGORY_LIST[3].last_num < nownum) INFO_CATEGORY_LIST[3].last_num = nownum;
                break;
            case NET:
                if(INFO_CATEGORY_LIST[4].last_num < nownum) INFO_CATEGORY_LIST[4].last_num = nownum;
                break;
            case DBP:
                if(INFO_CATEGORY_LIST[5].last_num < nownum) INFO_CATEGORY_LIST[5].last_num = nownum;
                break;
            case DMA:
                if(INFO_CATEGORY_LIST[6].last_num < nownum) INFO_CATEGORY_LIST[6].last_num = nownum;
                break;
            default:
                ret = FAILED;
                break;
        }
    }
    else if(LEVEL == WARN)
    {
        switch(category[0])
        {
            case INI:
                if(WARN_CATEGORY_LIST[0].last_num < nownum) WARN_CATEGORY_LIST[0].last_num = nownum;
                break;
            case SYS:
                if(WARN_CATEGORY_LIST[1].last_num < nownum) WARN_CATEGORY_LIST[1].last_num = nownum;
                break;
            case PRC:
                if(WARN_CATEGORY_LIST[2].last_num < nownum) WARN_CATEGORY_LIST[2].last_num = nownum;
                break;
            case MNG:
                if(WARN_CATEGORY_LIST[3].last_num < nownum) WARN_CATEGORY_LIST[3].last_num = nownum;
                break;
            case NET:
                if(WARN_CATEGORY_LIST[4].last_num < nownum) WARN_CATEGORY_LIST[4].last_num = nownum;
                break;
            case DBP:
                if(WARN_CATEGORY_LIST[5].last_num < nownum) WARN_CATEGORY_LIST[5].last_num = nownum;
                break;
            case DMA:
                if(WARN_CATEGORY_LIST[6].last_num < nownum) WARN_CATEGORY_LIST[6].last_num = nownum;
                break;
            default:
                ret = FAILED;
                break;
        }
    }
    else
    {
        printf("logCntLoad - UNKOWN LEVEL[%d]\n",LEVEL);
        return FAILED;
    }

    return ret;
}

FILE_LIST* findCFile(int *file_cnt)
{
    struct _finddata_t fd;
    FILE_LIST *file_list = NULL;
    long handle;
    int result = 1;
    int i = 0;
    char getsource_path[1024+1]={0,};
    int pc_exist_yn = FALSE;
    char pc_file_name[128+1] = {0,};

    printf("##[%d]\n",__LINE__);

    // C file find
    sprintf(getsource_path,"%s\\*.c",g_module_path);
    handle = _findfirst(getsource_path, &fd);
    if(handle == FAILED) 
	{
		printf("_findfirst error [%s]\n",getsource_path);
		_findclose(handle);	 	 
		return NULL;
	}
    printf("##[%d]\n",__LINE__);
    while(result != FAILED)
    {
        printf("fine_source[%s]\n",fd.name);
        if(strcmp(fd.name,"shp_db_proc.c") != 0)
        {
            printf("fine_source[%s]\n",fd.name);
            (*file_cnt)++;
            printf("file_cnt%d\n",*file_cnt);
        }
        result = _findnext(handle, &fd);
    }
    _findclose(handle);	

    // PC file find
    result = 1;
    sprintf(getsource_path,"%s\\*.pc",g_module_path);
    handle = _findfirst(getsource_path, &fd);
    if(handle == FAILED) 
	{
		printf("_findfirst error [%s]\n",getsource_path);
		_findclose(handle);	 	 
		return NULL;
	}
    printf("##[%d]\n",__LINE__);
    while(result != FAILED)
    {
        printf("fine_source[%s]\n",fd.name);
        if(strcmp(fd.name,"shp_db_proc.c") != 0)
        {
            printf("fine_source[%s]\n",fd.name);
            pc_exist_yn = TRUE;
            sprintf(pc_file_name,"%s",fd.name);
            printf("pc_file_name [%s,%s]\n",pc_file_name,fd.name);
            Sleep(1);
            (*file_cnt)++;
            printf("file_cnt%d\n",*file_cnt);
        }
        result = _findnext(handle, &fd);
    }
    _findclose(handle);	
   
    file_list = (FILE_LIST *)malloc(sizeof(FILE_LIST) * (*file_cnt));
    if(file_list == NULL)
    {
        printf("file list malloc failed\n");
        return NULL;
    }
     printf("##[%d]\n",__LINE__);
    memset(file_list, 0x00, sizeof(FILE_LIST)* (*file_cnt));


    if(pc_exist_yn == TRUE)
    {
        strcpy(file_list[i].file_name, pc_file_name);
        printf("file_list[i].file_name [%s,%s]\n",pc_file_name,file_list[i].file_name);
        Sleep(1);
        i++;
    }

    sprintf(getsource_path,"%s\\*.c",g_module_path);
    handle = _findfirst(getsource_path, &fd);
    if(handle == FAILED) 
	{
		printf("_findfirst error [%s]\n",getsource_path);
		_findclose(handle);
        xfree(file_list);	 	 
		return NULL;
	}
     printf("##[%d]\n",__LINE__);
    result = 1;
    while(result != FAILED)
    {
        printf("list1 %s\n",fd.name);
        if(strcmp(fd.name,"shp_db_proc.c") != 0)
        {
            strcpy(file_list[i].file_name, fd.name);
            printf("list %s\n",file_list[i].file_name);
            i++;
        }
        result = _findnext(handle, &fd);
    }
    _findclose(handle);
    return file_list;
}

void getModuleName(int argc, char **argv)
{
    char *ptr = NULL;

    printf("argument count [%d]\n", argc);
    if(argc == 3)
    {
        printf("[%s][%s]\n",argv[1],argv[2]);
        strncpy(g_product_code, argv[1], 1);
        printf("##[%d]\n",__LINE__);
        strcpy(g_module_path, argv[2]);
        printf("##[%d]\n",__LINE__);
        if(access(g_module_path, F_OK) != 0)
        {
            printf("Can't access this module path [%s]\n",g_module_path);
            exit(0);
        }
        printf("##[%d]\n",__LINE__);
        printf("##[%c]\n",'\\');
        ptr = strrchr(argv[2], '\\');

        printf("%s\n",&ptr[1]);
        if(getModuleCodeExtrac(&ptr[1]) == 0)
        {
            printf("Can't find [module name:%s]\n",ptr);
            printf("[Entered path:%s]\n",argv[2]);
            exit(0);
        }
        printf("##[%d]\n",__LINE__);
        return;
    }
    else
    {
        printf("EX> log_extention.exe [product code] [dir path]\n");
        printf("[product ex] Agent:A Logms:L OCI:2 Express:3   \n");
        exit(0);
    }
}

int getModuleCodeExtrac(char *module_name)
{
	int i = 0;
    printf("##[%d]\n",__LINE__);
    for(i = 0; i < MAX_MODULE_INDEX; i++)
    {
        printf("##g_module_code[%s,%s]\n",module_name,MODULE_LIST[i].module_name);
        if(stricmp(module_name, MODULE_LIST[i].module_name) == 0)
        {
            g_module_code = MODULE_LIST[i].code;
            printf("g_module_code[%d]\n",MODULE_LIST[i].code);
            return 1;
        }
    }
    printf("##[%d]\n",__LINE__);
    return 0;
}

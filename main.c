//
//  main.c
//  myLexer
//
//  Created by lyy on 2019/5/5.
//  Copyright © 2019年 lyy. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//存储保留字
char *keyword[] = {"auto","double","int","struct","break","else","long","switch","case","enum","register",
    "main","typedef","char","extern","return","union","const","float","short","unsigned",
    "continue","for","signed","void","default","goto","sizeof","volatile","do","while",
    "static","if"};

#define KEY 1
#define KEY_DESC  "保留字"

#define IDENTIFIER 2
#define IDENTIFIER_DESC "标识符"

#define INTEGER 3
#define INTEGER_DESC "无符号整数"

#define OPERATOR 4
#define OPERATOR_DESC "运算符"

#define BOUNTWROD 5
#define BOUNTWROD_DESC "分隔符"

#define HEADER 6
#define HEADER_DESC "头文件"

#define MAX_CHAR_NUMBER   100000
char charSET[MAX_CHAR_NUMBER];

//单词指针结构体
typedef struct NormalNode
{
    char content[50];
    int  type;
    struct NormalNode *next;
}NormalNode;

//标识符指针结构体
typedef struct IdentifierNode
{
    char content[50];
    int  type;
    struct IdentifierNode *next;
}IdentifierNode;

NormalNode *normalHead;//单词指针头
IdentifierNode *identifierHead;//标识符指针头

/*************************
 函数名：
 函数返回值：
 入口参数：
 函数功能：初始化单词指针和标识符指针，内容为空，类型码-1，next空
*************************/
void initNode(){
    normalHead = (NormalNode *)malloc(sizeof(NormalNode));
    strcpy(normalHead->content,  "");
    normalHead->type = -1;
    normalHead->next = NULL;
    
    identifierHead = (IdentifierNode *)malloc(sizeof(IdentifierNode));
    strcpy(identifierHead->content, "");
    identifierHead->type = -1;
    identifierHead->next = NULL;
}

/*************************
 函数名：
 函数返回值：
 入口参数：
 函数功能：创建一个新的单词（词法分析的单位）
 *************************/
void createNorNode(char *content,int type){
    NormalNode *p = normalHead;
    NormalNode *tmp = (NormalNode *)malloc(sizeof(NormalNode));
    
    while ( p->next != NULL) {
        p = p->next;
    }
    strcpy(tmp->content,content);//复制单词内容
    tmp->type = type;//单词类型码
    tmp->next = NULL;
    p->next = tmp;
}

/*************************
 函数名：
 函数返回值：
 入口参数：
 函数功能：创建一个新的标识符（如果标识符已存在，那么直接返回）
 *************************/
void createIdeNode(char *content,int type){
    IdentifierNode *p = identifierHead;
    IdentifierNode *tmp = (IdentifierNode *)malloc(sizeof(IdentifierNode));
    
    int address_count = 1;
    
    while (p->next != NULL){
        p = p -> next;
        address_count++;
        if(!strcmp(p->content, content))//标识符已经存在
            return;
    }
    strcpy(tmp->content,content);
    tmp->type = type;
    tmp->next = NULL;
    p->next= tmp;
}

/*************************
 函数名：
 函数返回值：
 入口参数：
 函数功能：判断这个单词是一个标识符还是一个保留字
 *************************/
int marchIde(char *word){
    for (int i = 0; i < 32; i++) {
        if(strcmp(word, keyword[i]) == 0){
            return KEY;
        }
    }
    return IDENTIFIER;
}

/*************************
 函数名：
 函数返回值：
 入口参数：
 函数功能：打印单词结果，格式为：(content,type)
 *************************/
void displaynormalNode(){
    NormalNode *p = normalHead;
    while ( (p = p -> next)) {
        printf("(%d,  ",p->type);
        printf("%s  )",p->content);
        printf("\n");
    }
}

/*************************
 函数名：
 函数返回值：
 入口参数：
 函数功能：程序入口主函数
 *************************/
int main(int argc, const char * argv[]) {
    initNode();
    FILE *fp;
    char ch;
    int count = 0;
    int line  = 1;
    
    char tmp[30];
    char *word;
    
    if((fp = fopen("/Users/lyy/Desktop/junior2/Compiling/ComExperiment/Lexer/myLexer/myLexer/options.c", "r")) == NULL){
        printf("文件不存在\n");
        return 0;
    }
    while( ch!= EOF){
        ch = fgetc(fp);
        
        //注释
        if (ch == '/') {
            ch = fgetc(fp);
            //  //...//  类型注释
            if(ch == '/') {
                while (ch != ' ' && ch != '\n') {
                    ch = fgetc(fp);
                }
            }
            //  /*...*/ 类型注释
            else if(ch == '*'){
                line++;
                ch = fgetc(fp);
                while (ch != '*'){
                    ch = fgetc(fp);
                    if(ch == '\n'){
                        line++;
                    }
                }while (ch != '/'){
                    ch = fgetc(fp);
                }
            }
            //  运算符"/"
            else {
                createNorNode("/", OPERATOR);
            }
        }
        
        //保留字和标识符
        if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_'){
            count = 0;
            while ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= '0' && ch<= '9')) {
                tmp[count++] = ch;
                ch = fgetc(fp);
            }
            word = (char *)malloc(sizeof(char)*count);
            memcpy(word, tmp, count);
            word[count] = '\0';
            int id = marchIde(word);
            if (id == IDENTIFIER ) {
                createIdeNode(word,IDENTIFIER);
                createNorNode(word,IDENTIFIER);
            }else{
                createNorNode(word,id);
            }
            fseek(fp,-1L,SEEK_CUR);//向后回退一位
        }
        
        //无符号整数
        else if(ch >= '0' && ch <= '9'){
            count = 0;
            while (ch >= '0' && ch <= '9') {
                tmp[count++] = ch;
                ch = fgetc(fp);
            }
            word = (char *)malloc(sizeof(char)*(count+1));
            memcpy(word, tmp, count);
            word[count] = '\0';
            createNorNode(word,INTEGER);
            fseek(fp,-1L,SEEK_CUR);//向后回退一位
        }

        //换行
        else if(ch == ' ' || ch == '\t' || ch == '\r' || ch =='\n' ){
            if(ch == '\n'){
                line++;
            }
        }
       
        //运算符
        else if(ch == '+'){
            createNorNode("+",OPERATOR);
        }else if(ch == '-'){
            createNorNode("-",OPERATOR);
        }else if(ch == '*'){
            createNorNode("*",OPERATOR);
        }else if(ch == '='){
            createNorNode("=",OPERATOR);
        }else if(ch == '>'){
            createNorNode(">",OPERATOR);
        }else if(ch == '<'){
            createNorNode("<",OPERATOR);
        }
        
        //分隔符
        else if(ch == ';'){
            createNorNode(";",BOUNTWROD);
        }else if(ch == ','){
            createNorNode(",",BOUNTWROD);
        }else if(ch == '"'){
            createNorNode("\"",BOUNTWROD);
        }else if(ch == ':'){
            createNorNode(":",BOUNTWROD);
        }else if(ch == '('){
            createNorNode("(",BOUNTWROD);
        }else if(ch == ')'){
            createNorNode(")",BOUNTWROD);
        }else if(ch == '{'){
            createNorNode("{",BOUNTWROD);
        }else if(ch == '}'){
            createNorNode("}",BOUNTWROD);
        }
        
        //去掉空格和换行符
        if (ch != ' '&& ch != '\n') {
            charSET[count] = ch;
            count++;
        }
    }
    fclose(fp);
    
    displaynormalNode();
    
    return 0;
}

//
// Created by Idrol on 21/09/2024.
//
#include <sh.h>
#include <drivers/ps2_kb.h>
#include <drivers/thread.h>
#include <drivers/vfs.h>
#include <stdio.h>

namespace
{
    char** argv = nullptr;
    int argc;
}

void change_directory(int argc, char** argv)
{
    if(argc < 2)
    {
        return;
    }
    auto targetPath = argv[1];
    char pathBuffer[2048] = {};
    if(targetPath[0] != '/')
    {
        auto pwd = getenv("pwd");
        auto pwdLen = strlen(pwd);
        strcpy(pathBuffer, pwd);
        if(pathBuffer[pwdLen-1] != '/')
        {
            pathBuffer[pwdLen] = '/';
            pwdLen++;
        }
        strcpy(&pathBuffer[pwdLen], targetPath);
    } else
    {
        strcpy(pathBuffer, targetPath);
    }

    auto pathLength = strlen(pathBuffer);
    if(pathBuffer[pathLength-1] != '/')
    {
        pathBuffer[pathLength] = '/';
        pathBuffer[pathLength+1] = 0x0;
    }
    printf("cd %s\n", pathBuffer);
    auto dir = vfs_openDir(pathBuffer);
    if(dir == NULL)
    {
        printf("cd: %s: %s: No such file or directory\n", pathBuffer, targetPath);
        return;
    }
    vfs_closeDir(dir);
    setenv("pwd", pathBuffer);
}

void list_directory(int argc, char** argv)
{
    auto pwd = getenv("pwd");
    auto dir = vfs_openDir(pwd);
    if(dir == NULL)
    {
        printf("ls: Current working directory is not valid %s\n", pwd);
        return;
    }
    int length = 0;
    auto dirEntry = vfs_readDir(dir);
    bool isEmpty = true;
    while(dirEntry != NULL)
    {
        isEmpty = false;
        auto strLength = strlen(dirEntry->name) + 2;
        length += strLength;
        if(length >= 80)
        {
            printf("\n");
            length = strLength;
        }
        printf("%s  ", dirEntry->name);
        dirEntry = vfs_readDir(dir);
    }
    vfs_closeDir(dir);
    if(!isEmpty) printf("\n");
}

void handle_command(int argc, char** argv)
{
    if(strncmp("cd", argv[0], 2) == 0)
    {
        change_directory(argc, argv);
    } else if(strncmp("ls", argv[0], 2) == 0)
    {
        list_directory(argc, argv);
    } else
    {
        printf("Command not recognized %s\n", argv[0]);
    }
}

void kernel_sh()
{
    while (true) {
        auto pwd = getenv("pwd");
        printf("%s > ", pwd);
        const char* command = getline();
        char tmpBuffer[2048] = {};
        strcpy(tmpBuffer, command);
        toLower(tmpBuffer);
        argc = (int)strcountchar(tmpBuffer, ' ')+1;
        argv = (char**)kmalloc(sizeof(char*)*argc);
        int start = 0;
        int argNbr = 0;
        for(int i = 0; i < strlen(tmpBuffer); i++)
        {
            if(tmpBuffer[i] == ' ')
            {
                argv[argNbr] = &tmpBuffer[start];
                argNbr++;
                tmpBuffer[i] = 0x0;
                start = i+1;
            }
        }
        argv[argNbr] = &tmpBuffer[start];
        handle_command(argc, argv);
        kfree(argv);
        argv = nullptr;
        argc = 0;
    }
}

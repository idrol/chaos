//
// Created by Idrol on 19/09/2024.
//
#include <drivers/thread.h>
#include <stdio.h>

namespace
{
    char envStrTmpBuffer[2048];
}

Thread** threads = nullptr;
Thread* activeThread = nullptr;

void thread_init()
{
    threads = (Thread**)kmalloc(sizeof(Thread*)*MAX_PROCESSES);

    auto rootThread = (Thread*)kmalloc(sizeof(Thread));
    rootThread->envSize = 1024;
    rootThread->env = (char**)kmalloc(sizeof(char*)*rootThread->envSize);
    activeThread = rootThread;
    setenv("pwd", "/");
    printf("Threading initialized\n");
}

Thread* thread_get_active_thread()
{
    return activeThread;
}

const char* getenv(const char* variable)
{
    auto thread = thread_get_active_thread();
    for(auto i = 0; i < thread->envSize; i++)
    {
        auto length = strlen(thread->env[i]);
        memcpy(envStrTmpBuffer, thread->env[i], length+1);
        if(strequal(variable, strtok(envStrTmpBuffer, "=")))
        {
            // Found index
            return strtok(NULL, "=");
        }
    }
    return NULL;
}

void setenv(const char* variable, const char* value)
{
    auto thread = thread_get_active_thread();
    int firstFree = -1;
    for(auto i = 0; i < thread->envSize; i++)
    {
        if(thread->env[i] == nullptr)
        {
            if(firstFree < 0) firstFree = i;
        }  else
        {
            auto length = strlen(thread->env[i]);
            memcpy(envStrTmpBuffer, thread->env[i], length+1);
            if(strequal(variable, strtok(envStrTmpBuffer, "=")))
            {
                kfree(thread->env[i]);
                size_t variableLen = strlen(variable);
                size_t newLength = variableLen+1+strlen(value);
                thread->env[i] = (char*)kmalloc(newLength+1);
                strcpy(thread->env[i], variable);
                thread->env[i][variableLen] = '=';
                strcpy(&thread->env[i][variableLen+1], value);
                thread->env[i][newLength] = 0x0;
                return;
            }
        }
    }
    if(firstFree >= 0)
    {
        auto i = firstFree;
        size_t variableLen = strlen(variable);
        size_t newLength = variableLen+1+strlen(value);
        thread->env[i] = (char*)kmalloc(newLength+1);
        strcpy(thread->env[i], variable);
        thread->env[i][variableLen] = '=';
        strcpy(&thread->env[i][variableLen+1], value);
        thread->env[i][newLength] = 0x0;
    }
}

void unsetenv(const char* variable)
{
    auto thread = thread_get_active_thread();
    for(auto i = 0; i < thread->envSize; i++)
    {
        auto length = strlen(thread->env[i]);
        memcpy(envStrTmpBuffer, thread->env[i], length+1);
        if(strequal(variable, strtok(envStrTmpBuffer, "=")))
        {
            kfree(thread->env[i]);
            thread->env[i] = NULL;
            return;
        }
    }
}


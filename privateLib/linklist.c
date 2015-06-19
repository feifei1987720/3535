#include "linklist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define LIST_DEBUG_FLAG 1        /*打开输出*/
/********************************************************************************************/
/*******************************           宏         ***************************************/
/********************************************************************************************/
#if LIST_DEBUG_FLAG
#define LIST_DEBUG(fmt,args...) printf("DEBUG %s-%d: "fmt"\n",__FUNCTION__,__LINE__,## args);
#else
#define LIST_DEBUG(fmt,args...)
#endif

#define LIST_FREE(ptr) do{if(0==(ptr))break; free(ptr); ptr=0; }while(0)
/********************************************************************************************/
/*******************************       静态函数声明     ***************************************/
/********************************************************************************************/
/*创建一个新的链表节点*/
static listnode_t* List_CreateNewNode(list_t *pList, void *pValue);
/*销毁一个链表节点*/
static void List_DeleteNode(list_t *pList ,listnode_t *pNode);
/*添加一个链表节点*/
static void List_AddNode(list_t* pList, listnode_t* pPrevNode, listnode_t* pCurNode);
/*移除一个链表节点*/
       void List_removeNode(list_t* pList, listnode_t* pNode);
/*返回一个链表节点*/
static listnode_t *List_NodeAt(list_t *pList, int index);
/*取出并删除一个节点*/
static int List_TakeNode(list_t *pList, listnode_t *pNode, char *pBuff);
/********************************************************************************************/
/*******************************       函数实现        ***************************************/
/********************************************************************************************/

/*构建一个新的链表*/
list_t* List_CreateNew(unsigned int size)
{
    list_t *pList = 0;

    do{
        pList = (list_t*)malloc(sizeof(list_t));
        if(0==pList) break;
        memset(pList,0,sizeof(list_t));

        pList->nodeSize = size;
        pList->nodeCount = 0;
        pList->pHead = 0;
        pList->pTail = 0;

        return pList;
    }while(0);
    List_Delete(pList);
    return 0;
}

/*销毁一个新的链表*/
void List_Delete(list_t *pList)
{
    if(0==pList) return;
    List_Clear(pList);
    LIST_FREE(pList);
}

/*删除链表中的所有节点*/
void List_Clear(list_t *pList)
{
    LIST_FOREACH_VARIABLE;
    LIST_FOREACH(pList){
        LIST_FOREACH_RM_CURNODE;
    }
}

/*返回 真(1) 如果这是一个空链表*/
int List_IsEmpty(list_t *pList)
{
    if(0==pList) return -1;
    if(0==pList->pHead) return 0;
    return 1;
}

/*返回链表的节点个数*/
int List_Count(list_t *pList)
{
    return (0==pList)?(-1): (int)(pList->nodeCount);
}


/*在 index 位置插入一个节点*/
int List_Insert(list_t *pList, void *pValue , int index)
{
    listnode_t *pNewNode;
    listnode_t *pPrevNode;

    if(0==pList || 0==pValue) return -1;

    do{
        pNewNode = List_CreateNewNode(pList,pValue);
        if(0==pNewNode) break;
        pPrevNode = List_NodeAt(pList,index-1);
        List_AddNode(pList,pPrevNode,pNewNode);
        return 0;
    }while(0);
    return -1;
}

/*在头部插入一个节点*/
int List_PushFront(list_t *pList, void *pValue)
{
    return List_Insert(pList,pValue,0);
}

/*在尾部插入一个节点*/
int List_PushBack(list_t *pList, void *pValue)
{
    listnode_t *pNewNode;

    if(0==pList || 0==pValue) return -1;

    do{
        pNewNode = List_CreateNewNode(pList,pValue);
        if(0==pNewNode) break;
        List_AddNode(pList,pList->pTail,pNewNode);
        return 0;
    }while(0);
    return -1;
}


/*删除 index 位置的节点*/
void List_RemoveAt(list_t *pList,int index)
{
    List_removeNode(pList,List_NodeAt(pList,index));
}

/*删除第一个节点*/
void List_RemoveFrist(list_t *pList)
{
    List_removeNode(pList,pList->pHead);
}

/*删除最后一个节点*/
void List_RemoveLast(list_t *pList)
{
    List_removeNode(pList,pList->pTail);
}

/*取出 index 位置节点的内容并删除节点*/
int List_TakeAt(list_t *pList, void *pBuff, int index)
{
    return List_TakeNode(pList,List_NodeAt(pList,index),pBuff);
}

/*取出第一个节点的内容并删除节点*/
int List_PopFront(list_t *pList, void *pBuff)
{
    if(0==pList) return -1;
    return List_TakeNode(pList,pList->pHead,pBuff);
}

/*取出最后一个节点的内容并删除节点*/
int List_PopBack(list_t *pList, void *pBuff)
{
    if(0==pList) return -1;
    return List_TakeNode(pList,pList->pTail,pBuff);
}

/*替换 index 位置节点的内容*/
int List_Replace(list_t *pList, void *pValue, int index)
{
    listnode_t *pNode;
    if(0==pList || 0==pValue) return -1;

    do{
        pNode = List_NodeAt(pList,index);
        if(0==pNode) break;
        memcpy(pNode->pData,pValue,pList->nodeSize);
        return 0;
    }while(0);
    return -1;
}


/*返回 index 节点的内容指针*/
void* List_At(list_t *pList, int index)
{
    listnode_t *pNode;
    do{
        pNode = List_NodeAt(pList,index);
        if(0==pNode) break;
        return (pNode->pData);
    }while(0);
    return 0;
}

/*返回第一个节点的内容指针*/
void* List_Frist(list_t *pList)
{
    return List_At(pList,0);
}

/*返回最后一个节点的内容指针*/
void *List_Last(list_t *pList)
{
    listnode_t *pNode;
    if(0==pList) return 0;
    do{
        pNode = pList->pTail;
        if(0==pNode) break;
        return (pNode->pData);
    }while(0);
    return 0;
}


///*return the number of occurrences of 'value' in the list*/
//int List_ValueCount(list_t *list, void *value)
//{
//    int num = 0;
//    LIST_FOREACH_VARIABLE;
//    if(0==list) return -1;
//    LIST_FOREACH(list){
//        if(0==memcmp(value,LIST_FOREACH_VALUE,List_Foreach_qList->size)) num++;
//    }
//    return num;
//}

///*return the index position of the frist occurrence of 'value' in the list*/
//int List_Indexof(list_t *list, void *value)
//{
//    int num;
//    LIST_FOREACH_VARIABLE;
//    if(0==list) return -1;

//    num=0;
//    LIST_FOREACH(list){
//        if(0==memcmp(value,LIST_FOREACH_VALUE,List_Foreach_qList->size)) return num;
//        num++;
//    }
//    return -1;
//}

///*search Str*/
//void* MListSearchStr(list_t *list, char *value ,int offset)
//{
//    listnode_t *p;
//    char *str;

//    if(0==list || 0==list->head || 0==value || 0>offset) return 0;

//    p = list->head;
//    while(p){
//        str = p->data;
//        str += offset;
//        str = *(char**)str;
//        if(0==strcmp(str,value)) return p->data;
//        p = p->next;
//    }

//    return 0;
//}

///*search int*/
//void* MListSearchInt(list_t *list, int value ,int offset)
//{
//    listnode_t *p;
//    char *cp;

//    if(0==list || 0==list->head || 0==value || 0>offset) return 0;

//    p = list->head;
//    while(p){
//        cp = p->data;
//        cp += offset;

//        if(*(int*)cp == value) return p->data;
//        p = p->next;
//    }

//    return 0;
//}


/********************************************************************************************/
/*******************************       静态函数实现     ***************************************/
/********************************************************************************************/
/*创建一个新的链表节点*/
static listnode_t* List_CreateNewNode(list_t *pList, void *pValue)
{
    listnode_t* pNode = 0;

    if((0==pList) || (0==pValue)) return 0;

    do{
        pNode = (listnode_t*)malloc(sizeof(listnode_t));
        if(0==pNode) break;
        memset(pNode,0,sizeof(listnode_t));

        if(0<pList->nodeSize){
            pNode->pData = malloc(pList->nodeSize);
            if(0==pNode->pData) break;
            memcpy(pNode->pData,pValue,pList->nodeSize);
        }else{
            pNode->pData = pValue;
        }

        return pNode;
    }while(0);

    List_DeleteNode(pList,pNode);
    return 0;
}

/*销毁一个链表节点*/
static void List_DeleteNode(list_t *pList ,listnode_t *pNode)
{
    if((0==pList) || (0==pNode)) return;
    if(0<(pList->nodeSize)){
        LIST_FREE(pNode->pData);
    }
    LIST_FREE(pNode);
}

/*添加一个链表节点*/
static void List_AddNode(list_t* pList, listnode_t* pPrevNode, listnode_t* pCurNode)
{
    listnode_t* pNextNode;

    if((0==pList) || (0==pCurNode)) return;

    pNextNode = ((0==pPrevNode) ? pList->pHead : pPrevNode->pNext);
    pCurNode->pPrev = pPrevNode;
    pCurNode->pNext = pNextNode;

    if(0==pNextNode){
        pList->pTail = pCurNode;
    }else{
        pNextNode->pPrev = pCurNode;
    }
    if(0==pPrevNode){
        pList->pHead = pCurNode;
    }else{
        pPrevNode->pNext = pCurNode;
    }
    pList->nodeCount ++;
}

/*移除一个链表节点*/
void List_removeNode(list_t* pList, listnode_t* pNode)
{
    if((0==pList) || (0==pNode)) return;

    if(0!=pNode->pNext){
        pNode->pNext->pPrev = pNode->pPrev;
    }
    if(0!=pNode->pPrev){
        pNode->pPrev->pNext = pNode->pNext;
    }
    if(pList->pHead == pNode){
        pList->pHead = pNode->pNext;
    }
    if(pList->pTail == pNode){
        pList->pTail = pNode->pPrev;
    }

    List_DeleteNode(pList,pNode);
    pList->nodeCount --;
}

/*返回一个链表节点*/
static listnode_t *List_NodeAt(list_t *pList, int index)
{
    LIST_FOREACH_VARIABLE;
    listnode_t *pNodeIndex;

    if(0==pList) return 0;
    if(0>index) return 0;

    do{
        pNodeIndex = 0;
        LIST_FOREACH(pList){
            if(LIST_FOREACH_INDEX >= index){
                pNodeIndex = List_Foreach_pCurNode;
                break;
            }
        }
        if(0==pNodeIndex) break;
        return pNodeIndex;
    }while(0);
    return 0;
}

/*取出并删除一个节点*/
static int List_TakeNode(list_t *pList, listnode_t *pNode, char *pBuff)
{
    if((0==pList) || (0==pNode) || (0==pBuff)) return -1;
    memcpy(pBuff,pNode->pData,pList->nodeSize);
    List_removeNode(pList,pNode);
    return 0;
}

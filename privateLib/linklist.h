#ifndef LINKList_H
#define LINKList_H

#ifdef __cplusplus
extern "C" {
#endif

/*链表节点结构体*/
typedef struct listnode_t{
    void                *pData;              /*数据地址*/
    int                  isMark;             /*是否被标记*/
    struct listnode_t   *pPrev;              /*前一个节点*/
    struct listnode_t   *pNext;              /*后一个节点*/
}listnode_t;

/*链表结构体*/
typedef struct list_t{
    unsigned int   nodeSize;                 /*节点大小, 节点为0时则不分别内存*/
    unsigned int   nodeCount;                /*节点个数*/
    listnode_t    *pHead;                    /*头节点*/
    listnode_t    *pTail;                    /*尾节点*/
}list_t;

/*构建一个新的链表*/
list_t* List_CreateNew(unsigned int size);
/*销毁一个链表*/
void List_Delete(list_t *list);
/*删除链表中的所有节点*/
void List_Clear(list_t *list);
/*返回 真(1) 如果这是一个空链表*/
int List_IsEmpty(list_t *pList);
/*返回链表的节点个数*/
int List_Count(list_t *pList);

/*在 index 位置插入一个节点*/
int List_Insert(list_t *pList, void *pValue ,int index);
/*在头部插入一个节点*/
int List_PushFront(list_t *pList, void *pValue);
/*在头部插入一个节点*/
int List_PushBack(list_t *pList, void *pValue);

/*删除 index 位置的节点*/
void List_RemoveAt(list_t *pList, int index);
/*删除第一个节点*/
void List_RemoveFrist(list_t *pList);
/*删除最后一个节点*/
void List_RemoveLast(list_t *pList);

/*取出 index 位置节点的内容并删除节点*/
int List_TakeAt(list_t *pList, void *pBuff, int index);
/*取出第一个节点的内容并删除节点*/
int List_PopFront(list_t *pList, void *pBuff);
/*取出最后一个节点的内容并删除节点*/
int List_PopBack(list_t *pList, void *pBuff);
/*替换 index 位置节点的内容*/
int List_Replace(list_t *pList, void *pValue, int index);

/*返回 index 节点的内容指针*/
void* List_At(list_t *pList, int index);
/*返回第一个节点的内容指针*/
void* List_Frist(list_t *pList);
/*返回最后一个节点的内容指针*/
void *List_Last(list_t *pList);


/*只给 LIST_FOREACH_RM_CURNODE 调用的函数*/
void List_removeNode(list_t* pList, listnode_t* pNode);

///*return the number of occurrences of 'value' in the list*/
//int List_ValueCount(list_t *list, void *value);
///*return the index position of the frist occurrence of 'value' in the list*/
//int List_Indexof(list_t *list, void *value);
///*search Str*/
//void* MListSearchStr(list_t *list, char *value ,int offset);
///*search Int*/
//void* MListSearchInt(list_t *list, int value ,int offset);

/*销毁一个链表*/
#define LIST_DELETE(pList) do{List_Delete(pList); pList = 0;}while(0);


#define LIST_FOREACH_VARIABLE            \
    list_t*      List_Foreach_pList;     \
    listnode_t*  List_Foreach_pCurNode;  \
    listnode_t*  List_Foreach_pNextNode; \
    int          List_Foreach_index;

#define LIST_FOREACH(pList)              \
    List_Foreach_index     =  -1;        \
    List_Foreach_pList     = (pList);    \
    List_Foreach_pNextNode = ((0==List_Foreach_pList)?(0):(List_Foreach_pList->pHead));             \
    while((0==List_Foreach_pNextNode)?(0):((List_Foreach_pCurNode=List_Foreach_pNextNode),          \
                                           (List_Foreach_pNextNode=List_Foreach_pNextNode->pNext),  \
                                           (List_Foreach_index++),(1)))

#define LIST_FOREACH_VALUE (List_Foreach_pCurNode->pData)
#define LIST_FOREACH_INDEX (List_Foreach_index)
#define LIST_FOREACH_RM_CURNODE         \
    List_removeNode(List_Foreach_pList,List_Foreach_pCurNode);  \
    List_Foreach_pCurNode = 0;

/*watch value*/
#define WatchInt(x) printf("int %s's value is %d\n",#x,x)
#define WatchFloat(x) printf("float %s's value is %lf\n",#x,x)
#define WatchString(x) printf("str %s's value is %s\n",#x,x)
#define WatchPointer(x) printf("pointer %s's value is %p\n",#x,x)
#define Mark(x) printf("\nmark: %s\n",#x);


#ifdef __cplusplus
}
#endif

#endif

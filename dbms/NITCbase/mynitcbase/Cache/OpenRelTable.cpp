#include "OpenRelTable.h"
#include <bits/stdc++.h>
#include <cstring>


OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAX_OPEN];

OpenRelTable::OpenRelTable() {

  // initialise all values in relCache and attrCache to be nullptr and all entries
  // in tableMetaInfo to be free

  // load the relation and attribute catalog into the relation cache (we did this already)

  // load the relation and attribute catalog into the attribute cache (we did this already)

  /************ Setting up tableMetaInfo entries ************/

  // in the tableMetaInfo array
  //   set free = false for RELCAT_RELID and ATTRCAT_RELID
  //   set relname for RELCAT_RELID and ATTRCAT_RELID
  for(int i=0;i<MAX_OPEN;i++){
    RelCacheTable::relCache[i]=nullptr;
    AttrCacheTable::attrCache[i]=nullptr;
    //tableMetaInfo[i].free=true;
  }
  RecBuffer relCatBlock(RELCAT_BLOCK);
  HeadInfo  relCatHeader;
  struct RelCacheEntry relCacheEntry;
  relCatBlock.getHeader(&relCatHeader);
  Attribute relCatRecord[RELCAT_NO_ATTRS];
  RecBuffer attrCatBlock(ATTRCAT_BLOCK);
  HeadInfo attrCatHeader;
  attrCatBlock.getHeader(&attrCatHeader);
  Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
    AttrCacheEntry *head,*curr;
    int i,j=0,k=0;
  for(int i=0;i<2;i++){

    relCatBlock.getRecord(relCatRecord,i);
    RelCacheTable::recordToRelCatEntry(relCatRecord,&relCacheEntry.relCatEntry);
    k+=relCacheEntry.relCatEntry.numAttrs;
    relCacheEntry.recId.block=RELCAT_BLOCK;
    relCacheEntry.recId.slot=i;
    RelCacheTable::relCache[i]=(struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
    *(RelCacheTable::relCache[i])=relCacheEntry;
    head=curr=(AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));

      for(;j<k;j++){
            attrCatBlock.getRecord(attrCatRecord,j);
            AttrCacheTable::recordToAttrCatEntry(attrCatRecord,&curr->attrCatEntry);
            curr->recId.slot=j;
            curr->recId.block=ATTRCAT_BLOCK;
            if(j==k-1){
              curr->next=nullptr;
              break;
            }
            curr->next=(AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
            curr=curr->next;
            
      }
      if(k>attrCatHeader.numEntries){
        int y=attrCatHeader.numEntries;
        int r=attrCatHeader.rblock;
        RecBuffer attrCatBlock1(r);
        HeadInfo attrCatHeader1;
        attrCatBlock1.getHeader(&attrCatHeader1);
        for(;j<k;j++){
          attrCatBlock1.getRecord(attrCatRecord,j-y);
          AttrCacheTable::recordToAttrCatEntry(attrCatRecord,&curr->attrCatEntry);
          curr->recId.slot=j;
          curr->recId.block=ATTRCAT_BLOCK;
          if(j==k-1){
            curr->next=nullptr;
            break;
          }
          curr->next=(AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
          curr=curr->next;

        }
      }
      j=k;
      AttrCacheTable::attrCache[i]=head;
  }

  // tableMetaInfo[RELCAT_RELID].free=false;
  // tableMetaInfo[ATTRCAT_RELID].free=false;
  // strcpy(tableMetaInfo[RELCAT_RELID].relName,RELCAT_RELNAME);
  // strcpy(tableMetaInfo[ATTRCAT_RELID].relName,ATTRCAT_RELNAME);

}


OpenRelTable::~OpenRelTable() {

  // close all open relations (from rel-id = 2 onwards. Why?)
  // for (int i = 2; i < MAX_OPEN; ++i) {
  //   if (!tableMetaInfo[i].free) {
  //     OpenRelTable::closeRel(i); // we will implement this function later
  //   }
  // }

  // free the memory allocated for rel-id 0 and 1 in the caches
}


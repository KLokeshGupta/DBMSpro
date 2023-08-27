#include "OpenRelTable.h"
#include <bits/stdc++.h>
#include <cstring>


OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAX_OPEN];

int OpenRelTable::getFreeOpenRelTableEntry() {

  /* traverse through the tableMetaInfo array,
    find a free entry in the Open Relation Table.*/
  for(int i=0;i<MAX_OPEN;i++){
    if(tableMetaInfo[i].free=true) return i;
  }
  return E_CACHEFULL;
  // if found return the relation id, else return E_CACHEFULL.
}
int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {

  /* traverse through the tableMetaInfo array,
    find the entry in the Open Relation Table corresponding to relName.*/

  // if found return the relation id, else indicate that the relation do not
  // have an entry in the Open Relation Table.
  for(int i=0;i<MAX_OPEN;i++){    
    if(!strcmp(relName,tableMetaInfo[i].relName)) return i;
    else E_RELNOTOPEN;
  }
  // if(!strcmp(relName,"Students")) return 2;
  // if(!strcmp(relName,RELCAT_RELNAME)) return 0;
  // if(!strcmp(relName,ATTRCAT_RELNAME)) return 1;
  return E_RELNOTOPEN;
}
int OpenRelTable::openRel(char relName[ATTR_SIZE]) {
  int k=OpenRelTable::getRelId(relName);
  if(/* the relation `relName` already has an entry in the Open Relation Table */k!=E_RELNOTOPEN){
    // (checked using OpenRelTable::getRelId())
    return k;
    // return that relation id;
  }

  /* find a free slot in the Open Relation Table
     using OpenRelTable::getFreeOpenRelTableEntry(). */
  k=OpenRelTable::getFreeOpenRelTableEntry();

  if (/* free slot not available */k==E_CACHEFULL){
    return E_CACHEFULL;
  }

  // let relId be used to store the free slot.
  int relId;

  // /****** Setting up Relation Cache entry for the relation ******/

  // /* search for the entry with relation name, relName, in the Relation Catalog using
  //     BlockAccess::linearSearch().
  //     Care should be taken to reset the searchIndex of the relation RELCAT_RELID
  //     before calling linearSearch().*/

  // // relcatRecId stores the rec-id of the relation `relName` in the Relation Catalog.
  RelCacheTable::resetSearchIndex(RELCAT_RELID);
  Attribute rec;
  strcpy(rec.sVal,relName);
  //std::cout <<(relName);
  RecId relcatRecId;
  char attrName[ATTR_SIZE];
  relcatRecId=BlockAccess::linearSearch(RELCAT_RELID,attrName,rec,EQ);

  if (/* relcatRecId == {-1, -1} */relcatRecId.block==-1 and relcatRecId.slot==-1) {
    // (the relation is not found in the Relation Catalog.)
    return E_RELNOTEXIST;
  }

  // /* read the record entry corresponding to relcatRecId and create a relCacheEntry
  //     on it using RecBuffer::getRecord() and RelCacheTable::recordToRelCatEntry().
  //     update the recId field of this Relation Cache entry to relcatRecId.
  //     use the Relation Cache entry to set the relId-th entry of the RelCacheTable.
  //   NOTE: make sure to allocate memory for the RelCacheEntry using malloc()
  // */
  RecBuffer relCatBlock(relcatRecId.block);
  Attribute relCatRecord[RELCAT_NO_ATTRS];
  relCatBlock.getRecord(relCatRecord,relcatRecId.slot);
  RelCacheEntry relCacheEntry;
  RelCacheTable::recordToRelCatEntry(relCatRecord,&relCacheEntry.relCatEntry);
  relCacheEntry.recId=relcatRecId;
  RelCacheTable::relCache[relcatRecId.slot]=(struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[relcatRecId.slot])=relCacheEntry;
  // /****** Setting up Attribute Cache entry for the relation ******/

  // // let listHead be used to hold the head of the linked list of attrCache entries.
  AttrCacheEntry* listHead,*curr;
  listHead=curr=(AttrCacheEntry*)malloc(sizeof(struct AttrCacheEntry));
  /*iterate over all the entries in the Attribute Catalog corresponding to each
  attribute of the relation relName by multiple calls of BlockAccess::linearSearch()
  care should be taken to reset the searchIndex of the relation, ATTRCAT_RELID,
  corresponding to Attribute Catalog before the first call to linearSearch().*/
  RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
  AttrCacheTable::attrCache[relcatRecId.slot]=listHead;
  Attribute rec1;
  strcpy(rec1.sVal,relName);
  Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
  while(true)
  {
      /* let attrcatRecId store a valid record id an entry of the relation, relName,
      in the Attribute Catalog.*/
      RecId attrcatRecId;
      char attrName[ATTR_SIZE];
      strcpy(attrName,"RelName");
      attrcatRecId=BlockAccess::linearSearch(ATTRCAT_RELID,attrName,rec1,EQ);
     // std::cout <<attrcatRecId.block <<" "<<attrcatRecId.slot <<"\n";
      if(attrcatRecId.block!=-1 and attrcatRecId.slot!=-1){
        RecBuffer attrCatBlock(attrcatRecId.block);
        attrCatBlock.getRecord(attrCatRecord,attrcatRecId.slot);
        AttrCacheTable::recordToAttrCatEntry(attrCatRecord,&curr->attrCatEntry);
        //std::cout<<curr->attrCatEntry.relName <<" ";
        curr->recId=attrcatRecId;
        curr->next=(AttrCacheEntry*)malloc(sizeof(struct AttrCacheEntry));
        curr=curr->next;        
      /* read the record entry corresponding to attrcatRecId and create an
      Attribute Cache entry on it using RecBuffer::getRecord() and
      AttrCacheTable::recordToAttrCatEntry().
      update the recId field of this Attribute Cache entry to attrcatRecId.
      add the Attribute Cache entry to the linked list of listHead .*/
      // NOTE: make sure to allocate memory for the AttrCacheEntry using malloc()
      }
      else{
        curr=nullptr;
        break;
      }
  }

  // set the relIdth entry of the AttrCacheTable to listHead.

  /****** Setting up metadata in the Open Relation Table for the relation******/

  // update the relIdth entry of the tableMetaInfo with free as false and
  // relName as the input.
  // tableMetaInfo[relId].free=false;
  // strcpy(tableMetaInfo[relId].relName,relName);
  tableMetaInfo[relcatRecId.slot].free=false;
  strcpy(tableMetaInfo[relcatRecId.slot].relName,relName);
   return relcatRecId.slot;
}
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
    tableMetaInfo[i].free=true;
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

  tableMetaInfo[RELCAT_RELID].free=false;
  tableMetaInfo[ATTRCAT_RELID].free=false;
  strcpy(tableMetaInfo[RELCAT_RELID].relName,RELCAT_RELNAME);
  strcpy(tableMetaInfo[ATTRCAT_RELID].relName,ATTRCAT_RELNAME);

}


OpenRelTable::~OpenRelTable() {

  // close all open relations (from rel-id = 2 onwards. Why?)
  for (int i = 2; i < MAX_OPEN; ++i) {
    if (!tableMetaInfo[i].free) {
      OpenRelTable::closeRel(i); // we will implement this function later
    }
  }

  // free the memory allocated for rel-id 0 and 1 in the caches
}

int OpenRelTable::closeRel(int relId) {
  if (/* rel-id corresponds to relation catalog or attribute catalog*/(relId==RELCAT_RELID) or (relId==ATTRCAT_RELID)) {
    return E_NOTPERMITTED;
  }

  if (/* 0 <= relId < MAX_OPEN */(relId<0) or (relId>=MAX_OPEN)) {
    return E_OUTOFBOUND;
  }

  if (/* rel-id corresponds to a free slot*/tableMetaInfo[relId].free==true) {
    return E_RELNOTOPEN;
  }

  // free the memory allocated in the relation and attribute caches which was
  // allocated in the OpenRelTable::openRel() function
  // OpenRelTable::openRel(tableMetaInfo[relId])
  tableMetaInfo[relId].free=true;
  // update `tableMetaInfo` to set `relId` as a free slot
  // update `relCache` and `attrCache` to set the entry at `relId` to nullptr
  (RelCacheTable::relCache[relId])=nullptr;
  (AttrCacheTable::attrCache[relId])=nullptr;
  return SUCCESS;
}





#include "OpenRelTable.h"
#include <bits/stdc++.h>
#include <cstring>

/* This function will open a relation having name `relName`.
Since we are currently only working with the relation and attribute catalog, we
will just hardcode it. In subsequent stages, we will loop through all the relations
and open the appropriate one.
*/
int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {

  // if relname is RELCAT_RELNAME, return RELCAT_RELID
  // if relname is ATTRCAT_RELNAME, return ATTRCAT_RELID
  if(!strcmp(relName,RELCAT_RELNAME)) return RELCAT_RELID;
  if(!strcmp(relName,ATTRCAT_RELNAME)) return ATTRCAT_RELID;
  if(!strcmp(relName,"Students")) return 2;
  return E_RELNOTOPEN;
}
/*               ---STAGE 3 ---                     */
OpenRelTable::OpenRelTable() {

  // initialize relCache and attrCache with nullptr
  for (int i = 0; i < MAX_OPEN; ++i) {
    RelCacheTable::relCache[i] = nullptr;
    AttrCacheTable::attrCache[i] = nullptr;
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
  for(int i=0;i<relCatHeader.numEntries;i++){

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

  /************ Setting up Relation Cache entries ************/
  // (we need to populate relation cache with entries for the relation catalog
  //  and attribute catalog.)

  /**** setting up Relation Catalog relation in the Relation Cache Table****/
  // allocate this on the heap because we want it to persist outside this function
  /**** setting up Attribute Catalog relation in the Relation Cache Table ****/
  // set up the relation cache entry for the attribute catalog similarly
  // from the record at RELCAT_SLOTNUM_FOR_ATTRCAT
  // set the value at RelCacheTable::relCache[ATTRCAT_RELID]
  // relCatBlock.getRecord(relCatRecord, RELCAT_SLOTNUM_FOR_RELCAT);

    /************ Setting up Attribute cache entries ************/
  // (we need to populate attribute cache with entries for the relation catalog
  //  and attribute catalog.)

  /**** setting up Relation Catalog relation in the Attribute Cache Table ****/
  // iterate through all the attributes of the relation catalog and create a linked
  // list of AttrCacheEntry (slots 0 to 5)
  // for each of the entries, set
  //    attrCacheEntry.recId.block = ATTRCAT_BLOCK;
  //    attrCacheEntry.recId.slot = i   (0 to 5)
  //    and attrCacheEntry.next appropriately
  // NOTE: allocate each entry dynamically using malloc
  // set the next field in the last entry to nullptr
  /**** setting up Attribute Catalog relation in the Attribute Cache Table ****/

  // set up the attributes of the attribute cache similarly.
  // read slots 6-11 from attrCatBlock and initialise recId appropriately
  // set the value at AttrCacheTable::attrCache[ATTRCAT_RELID]

  // AttrCacheTable::attrCache[RELCAT_RELID] = head;/* head of the linked list */;
    
 
}

OpenRelTable::~OpenRelTable() {
  // free all the memory that you allocated in the constructor
}
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
  relId=k;

  /****** Setting up Relation Cache entry for the relation ******/

  /* search for the entry with relation name, relName, in the Relation Catalog using
      BlockAccess::linearSearch().
      Care should be taken to reset the searchIndex of the relation RELCAT_RELID
      before calling linearSearch().*/

  // relcatRecId stores the rec-id of the relation `relName` in the Relation Catalog.
  RelCacheTable::resetSearchIndex(RELCAT_RELID);
  BlockAccess::linearSearch(RELCAT_RELID,RELCAT_RELNAME,RELCAT_,)
  RecId relcatRecId;

  if (/* relcatRecId == {-1, -1} */) {
    // (the relation is not found in the Relation Catalog.)
    return E_RELNOTEXIST;
  }

  /* read the record entry corresponding to relcatRecId and create a relCacheEntry
      on it using RecBuffer::getRecord() and RelCacheTable::recordToRelCatEntry().
      update the recId field of this Relation Cache entry to relcatRecId.
      use the Relation Cache entry to set the relId-th entry of the RelCacheTable.
    NOTE: make sure to allocate memory for the RelCacheEntry using malloc()
  */

  /****** Setting up Attribute Cache entry for the relation ******/

  // let listHead be used to hold the head of the linked list of attrCache entries.
  AttrCacheEntry* listHead;

  /*iterate over all the entries in the Attribute Catalog corresponding to each
  attribute of the relation relName by multiple calls of BlockAccess::linearSearch()
  care should be taken to reset the searchIndex of the relation, ATTRCAT_RELID,
  corresponding to Attribute Catalog before the first call to linearSearch().*/
  {
      /* let attrcatRecId store a valid record id an entry of the relation, relName,
      in the Attribute Catalog.*/
      RecId attrcatRecId;

      /* read the record entry corresponding to attrcatRecId and create an
      Attribute Cache entry on it using RecBuffer::getRecord() and
      AttrCacheTable::recordToAttrCatEntry().
      update the recId field of this Attribute Cache entry to attrcatRecId.
      add the Attribute Cache entry to the linked list of listHead .*/
      // NOTE: make sure to allocate memory for the AttrCacheEntry using malloc()
  }

  // set the relIdth entry of the AttrCacheTable to listHead.

  /****** Setting up metadata in the Open Relation Table for the relation******/

  // update the relIdth entry of the tableMetaInfo with free as false and
  // relName as the input.

  return relId;
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

/*                      ---STAGE 4 ---                    */
/* This function will open a relation having name `relName`.
Since we are currently only working with the relation and attribute catalog, we
will just hardcode it. In subsequent stages, we will loop through all the relations
and open the appropriate one.
*/
// int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {

//   // if relname is RELCAT_RELNAME, return RELCAT_RELID
//   // if relname is ATTRCAT_RELNAME, return ATTRCAT_RELID
//   if(!strcmp(relName,RELCAT_RELNAME)) return RELCAT_RELID;
//   if(!strcmp(relName,ATTRCAT_RELNAME)) return ATTRCAT_RELID;
//   if(!strcmp(relName,"Students")) return 2;
//   return E_RELNOTOPEN;
// }
/*               ---STAGE 3 ---                     */
// OpenRelTable::OpenRelTable() {

//   // initialize relCache and attrCache with nullptr
//   for (int i = 0; i < MAX_OPEN; ++i) {
//     RelCacheTable::relCache[i] = nullptr;
//     AttrCacheTable::attrCache[i] = nullptr;
//   }
//   RecBuffer relCatBlock(RELCAT_BLOCK);
//   HeadInfo  relCatHeader;
//   struct RelCacheEntry relCacheEntry;
//   relCatBlock.getHeader(&relCatHeader);
//   Attribute relCatRecord[RELCAT_NO_ATTRS];
//   RecBuffer attrCatBlock(ATTRCAT_BLOCK);
//   HeadInfo attrCatHeader;
//   attrCatBlock.getHeader(&attrCatHeader);
//   Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
//     AttrCacheEntry *head,*curr;
//     int i,j=0,k=0;
//   for(int i=0;i<2;i++){

//     relCatBlock.getRecord(relCatRecord,i);
//     RelCacheTable::recordToRelCatEntry(relCatRecord,&relCacheEntry.relCatEntry);
//     k+=relCacheEntry.relCatEntry.numAttrs;
//     relCacheEntry.recId.block=RELCAT_BLOCK;
//     relCacheEntry.recId.slot=i;
//     RelCacheTable::relCache[i]=(struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
//     *(RelCacheTable::relCache[i])=relCacheEntry;
//     head=curr=(AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));

//       for(;j<k;j++){
//             attrCatBlock.getRecord(attrCatRecord,j);
//             AttrCacheTable::recordToAttrCatEntry(attrCatRecord,&curr->attrCatEntry);
//             curr->recId.slot=j;
//             curr->recId.block=ATTRCAT_BLOCK;
//             if(j==k-1){
//               curr->next=nullptr;
//               break;
//             }
//             curr->next=(AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
//             curr=curr->next;
            
//       }
//       if(k>attrCatHeader.numEntries){
//         int y=attrCatHeader.numEntries;
//         int r=attrCatHeader.rblock;
//         RecBuffer attrCatBlock1(r);
//         HeadInfo attrCatHeader1;
//         attrCatBlock1.getHeader(&attrCatHeader1);
//         for(;j<k;j++){
//           attrCatBlock1.getRecord(attrCatRecord,j-y);
//           AttrCacheTable::recordToAttrCatEntry(attrCatRecord,&curr->attrCatEntry);
//           curr->recId.slot=j;
//           curr->recId.block=ATTRCAT_BLOCK;
//           if(j==k-1){
//             curr->next=nullptr;
//             break;
//           }
//           curr->next=(AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
//           curr=curr->next;

//         }
//       }
//       j=k;
//       AttrCacheTable::attrCache[i]=head;
//   }

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
    
 
//}


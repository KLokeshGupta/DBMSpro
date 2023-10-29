#include "OpenRelTable.h"
#include <bits/stdc++.h>
#include <cstring>


OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAX_OPEN];

int OpenRelTable::closeRel(int relId) {
  // confirm that rel-id fits the following conditions
  //     2 <=relId < MAX_OPEN
  //     does not correspond to a free slot
  //  (you have done this already)
  if (/* rel-id corresponds to relation catalog or attribute catalog*/(relId==RELCAT_RELID) or (relId==ATTRCAT_RELID)) {
    return E_NOTPERMITTED;
  }

  if (/* 0 <= relId < MAX_OPEN */(relId<0) or (relId>=MAX_OPEN)) {
    return E_OUTOFBOUND;
  }

  if (/* rel-id corresponds to a free slot*/tableMetaInfo[relId].free==true) {
    return E_RELNOTOPEN;
  }
  /****** Releasing the Relation Cache entry of the relation ******/

  if (RelCacheTable::relCache[relId]->dirty)
  {

    /* Get the Relation Catalog entry from RelCacheTable::relCache
    Then convert it to a record using RelCacheTable::relCatEntryToRecord(). */
    Attribute rec[RELCAT_NO_ATTRS];
    RelCacheTable::relCatEntryToRecord(&(RelCacheTable::relCache[relId]->relCatEntry),rec);
    RecId recId=RelCacheTable::relCache[relId]->recId;
    // declaring an object of RecBuffer class to write back to the buffer
    RecBuffer relCatBlock(recId.block);
    relCatBlock.setRecord(rec,recId.slot);
    // Write back to the buffer using relCatBlock.setRecord() with recId.slot
  }
  free(RelCacheTable::relCache[relId]);
  /****** Releasing the Attribute Cache entry of the relation ******/
  
  // free the memory allocated in the attribute caches which was
  // allocated in the OpenRelTable::openRel() function

  // (because we are not modifying the attribute cache at this stage,
  // write-back is not required. We will do it in subsequent
  // stages when it becomes needed)
  // for(AttrCacheEntry* entry=AttrCacheTable::attrCache[relId];entry!=nullptr;entry=entry->next){
  //   free(entry);
  // }
  AttrCacheEntry* entry=AttrCacheTable::attrCache[relId];
  AttrCacheEntry* temp=entry->next;
  while(true){

		if(entry->dirty){
			Attribute rec[ATTRCAT_NO_ATTRS];
			AttrCacheTable::attrCatEntryToRecord(&entry->attrCatEntry,rec);
			RecId recId=entry->recId;
			RecBuffer attrBuffer(recId.block);
			attrBuffer.setRecord(rec,recId.slot);
		}
      free(entry);
	  entry=temp;
	  if(entry==nullptr) break;
	  temp=temp->next;
    }
  
  //free(AttrCacheTable::attrCache[relId]);

  /****** Set the Open Relation Table entry of the relation as free ******/

  // update `metainfo` to set `relId` as a free slot
  
  tableMetaInfo[relId].free=true;
  RelCacheTable::relCache[relId]=nullptr;
  AttrCacheTable::attrCache[relId]=nullptr;
  strcpy(tableMetaInfo[relId].relName,"null");
  return SUCCESS;
}
OpenRelTable::~OpenRelTable() {

    for (int i=2;i<MAX_OPEN;i++)
    {
        if (tableMetaInfo[i].free==false)
        {
            // close the relation using openRelTable::closeRel().
            OpenRelTable::closeRel(i);
        }
    }

    /**** Closing the catalog relations in the relation cache ****/

    //releasing the relation cache entry of the attribute catalog

    if (/* RelCatEntry of the ATTRCAT_RELID-th RelCacheEntry has been modified */ RelCacheTable::relCache[ATTRCAT_RELID]->dirty) {

        /* Get the Relation Catalog entry from RelCacheTable::relCache
        Then convert it to a record using RelCacheTable::relCatEntryToRecord(). */
        Attribute rec[RELCAT_NO_ATTRS];
        RecId recId;
        recId=RelCacheTable::relCache[ATTRCAT_RELID]->recId;
        RelCacheTable::relCatEntryToRecord(&(RelCacheTable::relCache[ATTRCAT_RELID]->relCatEntry),rec);
        // declaring an object of RecBuffer class to write back to the buffer
        RecBuffer relCatBlock(recId.block);
        relCatBlock.setRecord(rec,recId.slot);
        // Write back to the buffer using relCatBlock.setRecord() with recId.slot
    }
    // free the memory dynamically allocated to this RelCacheEntry
    free(RelCacheTable::relCache[ATTRCAT_RELID]);

    //releasing the relation cache entry of the relation catalog

    if(/* RelCatEntry of the RELCAT_RELID-th RelCacheEntry has been modified */RelCacheTable::relCache[RELCAT_RELID]->dirty) {

        /* Get the Relation Catalog entry from RelCacheTable::relCache
        Then convert it to a record using RelCacheTable::relCatEntryToRecord(). */
        Attribute rec[RELCAT_NO_ATTRS];
        RecId recId;
        recId=RelCacheTable::relCache[RELCAT_RELID]->recId;
        RelCacheTable::relCatEntryToRecord(&(RelCacheTable::relCache[RELCAT_RELID]->relCatEntry),rec);
        // declaring an object of RecBuffer class to write back to the buffer
        RecBuffer relCatBlock(recId.block);
        relCatBlock.setRecord(rec,recId.slot);

        // Write back to the buffer using relCatBlock.setRecord() with recId.slot
    }
    // free the memory dynamically allocated for this RelCacheEntry
      free(RelCacheTable::relCache[RELCAT_RELID]);

    // free the memory allocated for the attribute cache entries of the
    // relation catalog and the attribute catalog
    free(AttrCacheTable::attrCache[RELCAT_RELID]);
    free(AttrCacheTable::attrCache[ATTRCAT_RELID]);
}
// OpenRelTable::~OpenRelTable() {

//   // close all open relations (from rel-id = 2 onwards. Why?)
//   int k=0;
//   for (int i = 2; i < MAX_OPEN; ++i) {
//     k++;
//     if (!tableMetaInfo[i].free) {
//       OpenRelTable::closeRel(i); // we will implement this function later  
//     }
//   }

//   // free the memory allocated for rel-id 0 and 1 in the caches
//   free(RelCacheTable::relCache[0]);
//   free(RelCacheTable::relCache[1]);
//   free(AttrCacheTable::attrCache[0]);
//   free(AttrCacheTable::attrCache[1]);
// }
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
  int relId=k;
  // std::cout<<k;

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
  strcpy(attrName, RELCAT_ATTR_RELNAME);
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
  RelCacheTable::relCache[k]=(struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[k])=relCacheEntry;
  
  int numAttributes=relCacheEntry.relCatEntry.numAttrs;
  // /****** Setting up Attribute Cache entry for the relation ******/

  // // let listHead be used to hold the head of the linked list of attrCache entries.
  AttrCacheEntry* listHead,*curr;
  listHead=curr=(AttrCacheEntry*)malloc(sizeof(struct AttrCacheEntry));
  /*iterate over all the entries in the Attribute Catalog corresponding to each
  attribute of the relation relName by multiple calls of BlockAccess::linearSearch()
  care should be taken to reset the searchIndex of the relation, ATTRCAT_RELID,
  corresponding to Attribute Catalog before the first call to linearSearch().*/
  RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
  AttrCacheTable::attrCache[k]=(struct AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
  (AttrCacheTable::attrCache[k])=listHead;
  Attribute rec1;
  strcpy(rec1.sVal,relName);
  Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
  int k1=0;
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
        if(k1<numAttributes-1){
        curr->next=(AttrCacheEntry*)malloc(sizeof(struct AttrCacheEntry));
        curr=curr->next;   
        }
        else{
          curr->next=nullptr;
          break;
        }     
      /* read the record entry corresponding to attrcatRecId and create an
      Attribute Cache entry on it using RecBuffer::getRecord() and
      AttrCacheTable::recordToAttrCatEntry().
      update the recId field of this Attribute Cache entry to attrcatRecId.
      add the Attribute Cache entry to the linked list of listHead .*/
      // NOTE: make sure to allocate memory for the AttrCacheEntry using malloc()
      }
      k1++;
  }

  // set the relIdth entry of the AttrCacheTable to listHead.

  /****** Setting up metadata in the Open Relation Table for the relation******/

  // update the relIdth entry of the tableMetaInfo with free as false and
  // relName as the input.
  // tableMetaInfo[relId].free=false;
  // strcpy(tableMetaInfo[relId].relName,relName);
  tableMetaInfo[k].free=false;
  strcpy(tableMetaInfo[k].relName,relName);
   return k;
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
        RecBuffer attrCatBlock(r);
        HeadInfo attrCatHeader;
        attrCatBlock.getHeader(&attrCatHeader);
        for(;j<k;j++){
          attrCatBlock.getRecord(attrCatRecord,j-y);
          AttrCacheTable::recordToAttrCatEntry(attrCatRecord,&curr->attrCatEntry);
          curr->recId.slot=j-y;
          curr->recId.block=r;
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


int OpenRelTable::getFreeOpenRelTableEntry() {

  /* traverse through the tableMetaInfo array,
    find a free entry in the Open Relation Table.*/
  for(int i=0;i<MAX_OPEN;i++){
    if(tableMetaInfo[i].free==true) return i;
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
    if(!strcmp(relName,tableMetaInfo[i].relName) and !tableMetaInfo[i].free) return i;
    // else E_RELNOTOPEN;
  }
  // if(!strcmp(relName,"Students")) return 2;
  // if(!strcmp(relName,RELCAT_RELNAME)) return 0;
  // if(!strcmp(relName,ATTRCAT_RELNAME)) return 1;
  return E_RELNOTOPEN;
}

#include "RelCacheTable.h"

#include <cstring>

RelCacheEntry* RelCacheTable::relCache[MAX_OPEN];

void RelCacheTable::relCatEntryToRecord(RelCatEntry* relCatEntry,union Attribute record[RELCAT_NO_ATTRS]){
  strcpy(record[RELCAT_REL_NAME_INDEX].sVal,relCatEntry->relName);
  record[RELCAT_FIRST_BLOCK_INDEX].nVal=(int)relCatEntry->firstBlk;
  record[RELCAT_LAST_BLOCK_INDEX].nVal=(int)relCatEntry->lastBlk;
  record[RELCAT_NO_ATTRIBUTES_INDEX].nVal=(int)relCatEntry->numAttrs;
  record[RELCAT_NO_RECORDS_INDEX].nVal=(int)relCatEntry->numRecs;
  record[RELCAT_NO_SLOTS_PER_BLOCK_INDEX].nVal=(int)relCatEntry->numSlotsPerBlk;
}

int RelCacheTable::setRelCatEntry(int relId, RelCatEntry *relCatBuf) {

  if(/*relId is outside the range [0, MAX_OPEN-1]*/relId<0 or relId>=MAX_OPEN) {
    return E_OUTOFBOUND;
  }

  if(/*entry corresponding to the relId in the Relation Cache Table is free*/relCache[relId]==nullptr) {
    return E_RELNOTOPEN;
  }
  relCache[relId]->relCatEntry=*relCatBuf;
  // copy the relCatBuf to the corresponding Relation Catalog entry in
  // the Relation Cache Table.
  // RelCacheTable::relCache[relId]->relCatEntry.firstBlk=relCatBuf->firstBlk;
  // RelCacheTable::relCache[relId]->relCatEntry.lastBlk=relCatBuf->lastBlk;
  // RelCacheTable::relCache[relId]->relCatEntry.numAttrs=relCatBuf->numAttrs;
  // RelCacheTable::relCache[relId]->relCatEntry.numRecs=relCatBuf->numRecs;
  // RelCacheTable::relCache[relId]->relCatEntry.numSlotsPerBlk=relCatBuf->numSlotsPerBlk;
  // strcpy(RelCacheTable::relCache[relId]->relCatEntry.relName,relCatBuf->relName);
  RelCacheTable::relCache[relId]->dirty=true;
  // set the dirty flag of the corresponding Relation Cache entry in
  // the Relation Cache Table.

  return SUCCESS;
}
/* will return the searchIndex for the relation corresponding to `relId
NOTE: this function expects the caller to allocate memory for `*searchIndex`
*/
int RelCacheTable::getSearchIndex(int relId, RecId* searchIndex) {
  if(relId<0 or relId>=MAX_OPEN) return E_OUTOFBOUND;
  if(relCache[relId]==nullptr) return E_RELNOTOPEN;
  *searchIndex = relCache[relId]->searchIndex;
  return SUCCESS;
  // check if 0 <= relId < MAX_OPEN and return E_OUTOFBOUND otherwise

  // check if relCache[relId] == nullptr and return E_RELNOTOPEN if true

  // copy the searchIndex field of the Relation Cache entry corresponding
  //   to input relId to the searchIndex variable.
}

// sets the searchIndex for the relation corresponding to relId
int RelCacheTable::setSearchIndex(int relId, RecId* searchIndex) {
  if(relId <0 or relId>=MAX_OPEN) return E_OUTOFBOUND;
  if(relCache[relId]==nullptr) return E_RELNOTOPEN;
  relCache[relId]->searchIndex=*searchIndex;

  return SUCCESS;
  // check if 0 <= relId < MAX_OPEN and return E_OUTOFBOUND otherwise

  // check if relCache[relId] == nullptr and return E_RELNOTOPEN if true

  // update the searchIndex value in the relCache for the relId to the searchIndex argument
}

int RelCacheTable::resetSearchIndex(int relId) {
    RecId rec{-1,-1};
    return setSearchIndex(relId,&rec);
  // use setSearchIndex to set the search index to {-1, -1}
}

/*                   ---STAGET 3 ----                    */
/*
Get the relation catalog entry for the relation with rel-id `relId` from the cache
NOTE: this function expects the caller to allocate memory for `*relCatBuf`
*/
int RelCacheTable::getRelCatEntry(int relId, RelCatEntry* relCatBuf) {
  if (relId < 0 || relId >= MAX_OPEN) {
    return E_OUTOFBOUND;
  }

  // if there's no entry at the rel-id
  if (relCache[relId] == nullptr) {
    return E_RELNOTOPEN;
  }

  // copy the value to the relCatBuf argument
  *relCatBuf = relCache[relId]->relCatEntry;

  return SUCCESS;
}

/* Converts a relation catalog record to RelCatEntry struct
    We get the record as Attribute[] from the BlockBuffer.getRecord() function.
    This function will convert that to a struct RelCatEntry type.
NOTE: this function expects the caller to allocate memory for `*relCatEntry`
*/
void RelCacheTable::recordToRelCatEntry(union Attribute record[RELCAT_NO_ATTRS],
                                        RelCatEntry* relCatEntry) {
  strcpy(relCatEntry->relName, record[RELCAT_REL_NAME_INDEX].sVal);
  relCatEntry->numAttrs = (int)record[RELCAT_NO_ATTRIBUTES_INDEX].nVal;
  relCatEntry->numRecs = (int)record[RELCAT_NO_RECORDS_INDEX].nVal;
  relCatEntry->firstBlk=(int)record[RELCAT_FIRST_BLOCK_INDEX].nVal;
  relCatEntry->lastBlk=(int)record[RELCAT_LAST_BLOCK_INDEX].nVal;
  relCatEntry->numSlotsPerBlk=(int)record[RELCAT_NO_SLOTS_PER_BLOCK_INDEX].nVal;
  /* fill the rest of the relCatEntry struct with the values at
      RELCAT_NO_RECORDS_INDEX,
      RELCAT_FIRST_BLOCK_INDEX,
      RELCAT_LAST_BLOCK_INDEX,
      RELCAT_NO_SLOTS_PER_BLOCK_INDEX
  */
}

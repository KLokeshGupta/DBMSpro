#include "BlockAccess.h"

#include <cstring>
#include <cstdlib>
#include <bits/stdc++.h>

int BlockAccess::deleteRelation(char relName[ATTR_SIZE]) {
    // if the relation to delete is either Relation Catalog or Attribute Catalog,
    //     return E_NOTPERMITTED
        // (check if the relation names are either "RELATIONCAT" and "ATTRIBUTECAT".
        // you may use the following constants: RELCAT_NAME and ATTRCAT_NAME)

    /* reset the searchIndex of the relation catalog using
       RelCacheTable::resetSearchIndex() */
    if(!strcmp(RELCAT_RELNAME,relName) or !strcmp(ATTRCAT_RELNAME,relName)) return E_NOTPERMITTED;
    RelCacheTable::resetSearchIndex(RELCAT_RELID);
    Attribute relNameAttr; // (stores relName as type union Attribute)
    // assign relNameAttr.sVal = relName
    strcpy(relNameAttr.sVal,relName);
    //  linearSearch on the relation catalog for RelName = relNameAttr
    RecId recId;
    char attrName[BLOCK_SIZE];
    strcpy(attrName,"RelName");
    recId=linearSearch(RELCAT_RELID,attrName,relNameAttr,EQ);
    if(recId.block==-1 and recId.slot==-1) return E_RELNOTEXIST;
    // if the relation does not exist (linearSearch returned {-1, -1})
    //     return E_RELNOTEXIST

    Attribute relCatEntryRecord[RELCAT_NO_ATTRS];
    /* store the relation catalog record corresponding to the relation in
       relCatEntryRecord using RecBuffer.getRecord */
    RecBuffer recBuffer(recId.block);
    recBuffer.getRecord(relCatEntryRecord,recId.slot);
    int firstblk=relCatEntryRecord[RELCAT_FIRST_BLOCK_INDEX].nVal;
    int numAttrs=relCatEntryRecord[RELCAT_NO_ATTRIBUTES_INDEX].nVal;
    /* get the first record block of the relation (firstBlock) using the
       relation catalog entry record */
    /* get the number of attributes corresponding to the relation (numAttrs)
       using the relation catalog entry record */

    /*
     Delete all the record blocks of the relation
    */
    // for each record block of the relation:
    //     get block header using BlockBuffer.getHeader
    //     get the next block from the header (rblock)
    //     release the block using BlockBuffer.releaseBlock
    //
    //     Hint: to know if we reached the end, check if nextBlock = -1
    int currblk=firstblk,rightblk=-1;
    while(true){
        HeadInfo header;
        RecBuffer rec(currblk);
        rec.getHeader(&header);
        rightblk=header.rblock;
        rec.releaseBlock();
        if(rightblk==-1) break;
        currblk=rightblk;
    }

    /***
        Deleting attribute catalog entries corresponding the relation and index
        blocks corresponding to the relation with relName on its attributes
    ***/

    // reset the searchIndex of the attribute catalog
    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
    int numberOfAttributesDeleted = 0;

    while(true) {
        RecId attrCatRecId;
        // attrCatRecId = linearSearch on attribute catalog for RelName = relNameAttr
        attrCatRecId=linearSearch(ATTRCAT_RELID,attrName,relNameAttr,EQ);
        // if no more attributes to iterate over (attrCatRecId == {-1, -1})
        //     break;
        if(attrCatRecId.block==-1 and attrCatRecId.slot==-1) break;
        numberOfAttributesDeleted++;

        // create a RecBuffer for attrCatRecId.block
        // get the header of the block
        // get the record corresponding to attrCatRecId.slot
        RecBuffer recbuffer(attrCatRecId.block);
        HeadInfo attrheader;
        recbuffer.getHeader(&attrheader);
        Attribute rec[ATTRCAT_NO_ATTRS];
        recbuffer.getRecord(rec,attrCatRecId.slot);
        // declare variable rootBlock which will be used to store the root
        // block field from the attribute catalog record.
        Attribute val;
        val=rec[ATTRCAT_ROOT_BLOCK_INDEX];
        int rootBlock = /* get root block from the record */val.nVal;
        // (This will be used later to delete any indexes if it exists)
        
        // Update the Slotmap for the block by setting the slot as SLOT_UNOCCUPIED
        // Hint: use RecBuffer.getSlotMap and RecBuffer.setSlotMap
        unsigned char *slotmap=(unsigned char*)malloc(attrheader.numSlots*sizeof(unsigned char));
        recbuffer.getSlotMap(slotmap);
        slotmap[attrCatRecId.slot]=SLOT_UNOCCUPIED;
        recbuffer.setSlotMap(slotmap);
        attrheader.numEntries-=1;
        recbuffer.setHeader(&attrheader);
        /* Decrement the numEntries in the header of the block corresponding to
           the attribute catalog entry and then set back the header
           using RecBuffer.setHeader */

        /* If number of entries become 0, releaseBlock is called after fixing
           the linked list.
        */
        if ( attrheader.numEntries == 0) {
            /* Standard Linked List Delete for a Block
               Get the header of the left block and set it's rblock to this
               block's rblock
            */
           int lblk=attrheader.lblock;
            // create a RecBuffer for lblock and call appropriate methods
            RecBuffer lblkbuffer(lblk);
            HeadInfo lheader;
            lblkbuffer.getHeader(&lheader);
            lheader.rblock=attrheader.rblock;
            lblkbuffer.setHeader(&lheader);
            if (attrheader.rblock != -1 ) {
                /* Get the header of the right block and set it's lblock to
                   this block's lblock */
                // create a RecBuffer for rblock and call appropriate methods
                int rblk=attrheader.rblock;
                RecBuffer rblkbuffer(rblk);
                HeadInfo rheader;
                rblkbuffer.getHeader(&rheader);
                rheader.lblock=lblk;
                rblkbuffer.setHeader(&rheader);
            } else {
                // (the block being released is the "Last Block" of the relation.)
                /* update the Relation Catalog entry's LastBlock field for this
                   relation with the block number of the previous block. */
                relCatEntryRecord[RELCAT_LAST_BLOCK_INDEX].nVal=lblk;
                
            }

            // (Since the attribute catalog will never be empty(why?), we do not
            //  need to handle the case of the linked list becoming empty - i.e
            //  every block of the attribute catalog gets released.)

            // call releaseBlock()
        }

        // (the following part is only relevant once indexing has been implemented)
        // if index exists for the attribute (rootBlock != -1), call bplus destroy
        if (rootBlock != -1) {
            // delete the bplus tree rooted at rootBlock using BPlusTree::bPlusDestroy()
        }
    }

    /*** Delete the entry corresponding to the relation from relation catalog ***/
    // Fetch the header of Relcat block
    HeadInfo relCatHeader;
    recBuffer.getHeader(&relCatHeader);
    relCatHeader.numEntries-=1;
    recBuffer.setHeader(&relCatHeader);
    /* Decrement the numEntries in the header of the block corresponding to the
       relation catalog entry and set it back */

    /* Get the slotmap in relation catalog, update it by marking the slot as
       free(SLOT_UNOCCUPIED) and set it back. */
    unsigned char* slotmap=(unsigned char*)malloc(relCatHeader.numSlots*sizeof(ATTR_SIZE));
    slotmap[recId.slot]=SLOT_UNOCCUPIED;
    recBuffer.setSlotMap(slotmap);
    /*** Updating the Relation Cache Table ***/
    /** Update relation catalog record entry (number of records in relation
        catalog is decreased by 1) **/
    // Get the entry corresponding to relation catalog from the relation
    // cache and update the number of records and set it back
    // (using RelCacheTable::setRelCatEntry() function)
    RelCatEntry relcatentry;
    RelCacheTable::getRelCatEntry(RELCAT_RELID,&relcatentry);
    relcatentry.numRecs-=1;
    RelCacheTable::setRelCatEntry(RELCAT_RELID,&relcatentry);
    RelCatEntry attrcatentry;
    RelCacheTable::getRelCatEntry(ATTRCAT_RELID,&attrcatentry);
    attrcatentry.numRecs-=numberOfAttributesDeleted;
    RelCacheTable::setRelCatEntry(ATTRCAT_RELID,&attrcatentry);
    /** Update attribute catalog entry (number of records in attribute catalog
        is decreased by numberOfAttributesDeleted) **/
    // i.e., #Records = #Records - numberOfAttributesDeleted

    // Get the entry corresponding to attribute catalog from the relation
    // cache and update the number of records and set it back
    // (using RelCacheTable::setRelCatEntry() function)

    return SUCCESS;
}
/*
NOTE: This function will copy the result of the search to the `record` argument.
      The caller should ensure that space is allocated for `record` array
      based on the number of attributes in the relation.
*/
int BlockAccess::search(int relId, Attribute *record, char attrName[ATTR_SIZE], Attribute attrVal, int op) {
    // Declare a variable called recid to store the searched record
    RecId recId;

    /* search for the record id (recid) corresponding to the attribute with
    attribute name attrName, with value attrval and satisfying the condition op
    using linearSearch() */
    Attribute attrN;
    strcpy(attrN.sVal,attrName);
    recId=linearSearch(relId,attrName,attrN,EQ);
    // if there's no record satisfying the given condition (recId = {-1, -1})
    //    return E_NOTFOUND;
    if(recId.block==-1 and recId.slot==-1) return E_NOTFOUND;
    /* Copy the record with record id (recId) to the record buffer (record)
       For this Instantiate a RecBuffer class object using recId and
       call the appropriate method to fetch the record
    */
    RecBuffer rec(recId.block);
    return SUCCESS;
}
int BlockAccess::insert(int relId, Attribute *record) {
    // get the relation catalog entry from relation cache
    // ( use RelCacheTable::getRelCatEntry() of Cache Layer)
    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(relId,&relCatEntry);
    int blockNum = /* first record block of the relation (from the rel-cat entry)*/relCatEntry.firstBlk;

    // rec_id will be used to store where the new record will be inserted
    RecId rec_id = {-1, -1};

    int numOfSlots = /* number of slots per record block */relCatEntry.numSlotsPerBlk;
    int numOfAttributes = /* number of attributes of the relation */relCatEntry.numAttrs;

    int prevBlockNum =  relCatEntry.lastBlk;
    /* block number of the last element in the linked list = -1 */
    /*
        Traversing the linked list of existing record blocks of the relation
        until a free slot is found OR
        until the end of the list is reached
    */
    while (blockNum != -1) {
        // create a RecBuffer object for blockNum (using appropriate constructor!)
        RecBuffer rec(blockNum);
        HeadInfo head;
        rec.getHeader(&head);
        unsigned char *slotmap=(unsigned char*)malloc(numOfSlots*sizeof(unsigned char));
        rec.getSlotMap(slotmap);
        // get header of block(blockNum) using RecBuffer::getHeader() function

        // get slot map of block(blockNum) using RecBuffer::getSlotMap() function
        
        for(int i=0;i<numOfSlots;i++){
            if(slotmap[i]==SLOT_UNOCCUPIED){
                rec_id.slot=i;
                rec_id.block=blockNum;
                break;
            }
        }
        if(rec_id.block!=-1 and rec_id.slot!=-1) break;
        prevBlockNum=blockNum;
        blockNum=head.rblock;
        // search for free slot in the block 'blockNum' and store it's rec-id in rec_id
        // (Free slot can be found by iterating over the slot map of the block)
        /* slot map stores SLOT_UNOCCUPIED if slot is free and
           SLOT_OCCUPIED if slot is occupied) */

        /* if a free slot is found, set rec_id and discontinue the traversal
           of the linked list of record blocks (break from the loop) */

        /* otherwise, continue to check the next block by updating the
           block numbers as follows:
              update prevBlockNum = blockNum
              update blockNum = header.rblock (next element in the linked
                                               list of record blocks)
        */
    }

    //  if no free slot is found in existing record blocks (rec_id = {-1, -1})
    if(rec_id.block==-1 and rec_id.slot==-1)
    {
        // if relation is RELCAT, do not allocate any more blocks
        //     return E_MAXRELATIONS;
        if(relId==RELCAT_RELID) return E_MAXRELATIONS;
        RecBuffer newblock;
        int ret2=newblock.getBlockNum();
        // Otherwise,
        // get a new record block (using the appropriate RecBuffer constructor!)
        // get the block number of the newly allocated block
        // (use BlockBuffer::getBlockNum() function)
        // let ret be the return value of getBlockNum() function call
        if (ret2 == E_DISKFULL) {
            return E_DISKFULL;
        }

        // Assign rec_id.block = new block number(i.e. ret) and rec_id.slot = 0
       // RecId rec_id;
        rec_id.block=ret2;
        rec_id.slot=0;
        HeadInfo head;
        head.blockType=REC;
        head.lblock=prevBlockNum;
        head.rblock=-1;
        head.numAttrs=numOfAttributes;
        head.numSlots=numOfSlots;
        head.numEntries=0;
        head.pblock=-1;
        newblock.setHeader(&head);
        unsigned char *slotmap=(unsigned char*)malloc(sizeof(unsigned char)*numOfSlots);
        for(int j=0;j<numOfSlots;j++){
            slotmap[j]=SLOT_UNOCCUPIED;
        }
        newblock.setSlotMap(slotmap);
        /*
            set the header of the new record block such that it links with
            existing record blocks of the relation
            set the block's header as follows:
            blockType: REC, pblock: -1
            lblock
                  = -1 (if linked list of existing record blocks was empty
                         i.e this is the first insertion into the relation)
                  = prevBlockNum (otherwise),
            rblock: -1, numEntries: 0,
            numSlots: numOfSlots, numAttrs: numOfAttributes
            (use BlockBuffer::setHeader() function)
        */

        /*
            set block's slot map with all slots marked as free
            (i.e. store SLOT_UNOCCUPIED for all the entries)
            (use RecBuffer::setSlotMap() function)
        */

        // if prevBlockNum != -1
        if(prevBlockNum!=-1)
        {
            // create a RecBuffer object for prevBlockNum
            // get the header of the block prevBlockNum and
            // update the rblock field of the header to the new block
            // number i.e. rec_id.block
            // (use BlockBuffer::setHeader() function)
            RecBuffer prev(prevBlockNum);
            HeadInfo header;
            prev.getHeader(&header);
            header.rblock=rec_id.block;
            prev.setHeader(&header);
        }
         else
        {
            relCatEntry.firstBlk=rec_id.block;
            int ret1=RelCacheTable::setRelCatEntry(relId,&relCatEntry);
            if(ret1!=SUCCESS) return ret1;
            // update first block field in the relation catalog entry to the
            // new block (using RelCacheTable::setRelCatEntry() function)
        }

        relCatEntry.lastBlk=rec_id.block;
        RelCacheTable::setRelCatEntry(relId,&relCatEntry);
        // update last block field in the relation catalog entry to the
        // new block (using RelCacheTable::setRelCatEntry() function)
    }

    // create a RecBuffer object for rec_id.block
    // insert the record into rec_id'th slot using RecBuffer.setRecord())
    RecBuffer newblock1(rec_id.block);
    newblock1.setRecord(record,rec_id.slot);

    /* update the slot map of the block by marking entry of the slot to
       which record was inserted as occupied) */
    // (ie store SLOT_OCCUPIED in free_slot'th entry of slot map)
    // (use RecBuffer::getSlotMap() and RecBuffer::setSlotMap() functions)
        unsigned char *slotmap=(unsigned char*)malloc(sizeof(unsigned char)*numOfSlots);
        newblock1.getSlotMap(slotmap);
        slotmap[rec_id.slot]=SLOT_OCCUPIED;
         newblock1.setSlotMap(slotmap);
         HeadInfo header;
         newblock1.getHeader(&header);
         header.numEntries+=1;
         newblock1.setHeader(&header);
         relCatEntry.numRecs+=1;
         RelCacheTable::setRelCatEntry(relId,&relCatEntry);
    // increment the numEntries field in the header of the block to
    // which record was inserted
    // (use BlockBuffer::getHeader() and BlockBuffer::setHeader() functions)

    // Increment the number of records field in the relation cache entry for
    // the relation. (use RelCacheTable::setRelCatEntry function)

    return SUCCESS;
}
int BlockAccess::renameAttribute(char relName[ATTR_SIZE], char oldName[ATTR_SIZE], char newName[ATTR_SIZE]) {

    /* reset the searchIndex of the relation catalog using
       RelCacheTable::resetSearchIndex() */
    RelCacheTable::resetSearchIndex(RELCAT_RELID);
    Attribute relNameAttr;    // set relNameAttr to relName
    strcpy(relNameAttr.sVal,relName);
    RecId rec;
    char name[ATTR_SIZE];
    strcpy(name,RELCAT_ATTR_RELNAME);
    rec=linearSearch(RELCAT_RELID,name,relNameAttr,EQ);
    if(rec.block==-1 and rec.slot==-1) return E_RELNOTEXIST;
    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);

    RecId attrToRenameRecId{-1, -1};
    Attribute attrCatEntryRecord[ATTRCAT_NO_ATTRS];
    // Search for the relation with name relName in relation catalog using linearSearch()
    // If relation with name relName does not exist (search returns {-1,-1})
    //    return E_RELNOTEXIST;

    /* reset the searchIndex of the attribute catalog using
       RelCacheTable::resetSearchIndex() */

    /* declare variable attrToRenameRecId used to store the attr-cat recId
    of the attribute to rename */
    /* iterate over all Attribute Catalog Entry record corresponding to the
       relation to find the required attribute */
    strcpy(name,ATTRCAT_ATTR_RELNAME);
    while (true) {
        // linear search on the attribute catalog for RelName = relNameAttr
        rec=linearSearch(ATTRCAT_RELID,name,relNameAttr,EQ);
        if(rec.block!=-1 and rec.slot!=-1){
            RecBuffer attrcat(rec.block);
            attrcat.getRecord(attrCatEntryRecord,rec.slot);
            if(!strcmp(attrCatEntryRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,newName)){
                return E_ATTREXIST;
            }
            if(!strcmp(attrCatEntryRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,oldName)){
                attrToRenameRecId=rec;
                break;
            }

        }
        else{
            break;
        }
        // if there are no more attributes left to check (linearSearch returned {-1,-1})
        //     break;

        /* Get the record from the attribute catalog using RecBuffer.getRecord
          into attrCatEntryRecord */
        // if attrCatEntryRecord.attrName = oldName
        //     attrToRenameRecId = block and slot of this record

        // if attrCatEntryRecord.attrName = newName
        //     return E_ATTREXIST;
    }
    if(attrToRenameRecId.block==-1 and attrToRenameRecId.slot==-1) return E_ATTRNOTEXIST;
    RecBuffer buffer(attrToRenameRecId.block);
    buffer.getRecord(attrCatEntryRecord,attrToRenameRecId.slot);
    strcpy(attrCatEntryRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,newName);
    buffer.setRecord(attrCatEntryRecord,attrToRenameRecId.slot);
    
    // if attrToRenameRecId == {-1, -1}
    //     return E_ATTRNOTEXIST;


    // Update the entry corresponding to the attribute in the Attribute Catalog Relation.
    /*   declare a RecBuffer for attrToRenameRecId.block and get the record at
         attrToRenameRecId.slot */
    //   update the AttrName of the record with newName
    //   set back the record with RecBuffer.setRecord

    return SUCCESS;
}
int BlockAccess::renameRelation(char oldName[ATTR_SIZE], char newName[ATTR_SIZE]){
    /* reset the searchIndex of the relation catalog using
       RelCacheTable::resetSearchIndex() */
    //    int relId=OpenRelTable::getRelId(oldName);
    RelCacheTable::resetSearchIndex(RELCAT_RELID);
    Attribute newRelationName;    // set newRelationName with newName
    char name[ATTR_SIZE];
    strcpy(name,RELCAT_ATTR_RELNAME);
    strcpy(newRelationName.sVal,newName);
    RecId rec;
    rec=linearSearch(RELCAT_RELID,name,newRelationName,EQ);
    if(rec.block!=-1 and rec.slot!=-1){
        return E_RELEXIST;
    }
    RelCacheTable::resetSearchIndex(RELCAT_RELID);

    Attribute oldRelationName;    // set oldRelationName with oldName
    strcpy(oldRelationName.sVal,oldName);
    rec=linearSearch(RELCAT_RELID,name,oldRelationName,EQ);
    if(rec.block==-1 or rec.slot==-1) return E_RELNOTEXIST;
    RecBuffer relcat(rec.block);
    Attribute relcatrecord[RELCAT_NO_ATTRS];
    relcat.getRecord(relcatrecord,rec.slot);
    strcpy(relcatrecord[RELCAT_REL_NAME_INDEX].sVal,newName);
    //std::cout <<relcatrecord[RELCAT_REL_NAME_INDEX].sVal;
    //RecBuffer::setRecord(relcatrecord,rec.slot); a nonstatic member reference must be relative to a specific object for this we use relcat
    relcat.setRecord(relcatrecord,rec.slot);

    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
    Attribute attrcatrec[ATTRCAT_NO_ATTRS];
    while(true){
        rec=linearSearch(ATTRCAT_RELID,name,oldRelationName,EQ);
        if(rec.block!=-1 and rec.slot!=-1){
            RecBuffer attrcat(rec.block);
            attrcat.getRecord(attrcatrec,rec.slot);
            strcpy(attrcatrec[ATTRCAT_REL_NAME_INDEX].sVal,newName);
            attrcat.setRecord(attrcatrec,rec.slot);
        }
        else{
            break;
        }
    }
    // search the relation catalog for an entry with "RelName" = oldRelationName

    // If relation with name oldName does not exist (result of linearSearch is {-1, -1})
    //    return E_RELNOTEXIST;

    /* get the relation catalog record of the relation to rename using a RecBuffer
       on the relation catalog [RELCAT_BLOCK] and RecBuffer.getRecord function
    */
    /* update the relation name attribute in the record with newName.
       (use RELCAT_REL_NAME_INDEX) */
    // set back the record value using RecBuffer.setRecord
    // search the relation catalog for an entry with "RelName" = newRelationName

    // If relation with name newName already exists (result of linearSearch
    //                                               is not {-1, -1})
    //    return E_RELEXIST;

    /* reset the searchIndex of the relation catalog using
       RelCacheTable::resetSearchIndex() */

    /*
    update all the attribute catalog entries in the attribute catalog corresponding
    to the relation with relation name oldName to the relation name newName
    */

    /* reset the searchIndex of the attribute catalog using
       RelCacheTable::resetSearchIndex() */

    //for i = 0 to numberOfAttributes :
    //    linearSearch on the attribute catalog for relName = oldRelationName
    //    get the record using RecBuffer.getRecord
    //
    //    update the relName field in the record to newName
    //    set back the record using RecBuffer.setRecord

    return SUCCESS;
}
RecId BlockAccess::linearSearch(int relId, char attrName[ATTR_SIZE], union Attribute attrVal, int op) {
    // get the previous search index of the relation relId from the relation cache
    // (use RelCacheTable::getSearchIndex() function)
    RecId prevRecId;
    RelCacheTable::getSearchIndex(relId,&prevRecId);
    // let block and slot denote the record id of the record being currently checked
    int block,slot;
    // if the current search index record is invalid(i.e. both block and slot = -1)
    if (prevRecId.block == -1 && prevRecId.slot == -1)
    {
        // (no hits from previous search; search should start from the
        // first record itself)
        // get the first record block of the relation from the relation cache
        // (use RelCacheTable::getRelCatEntry() function of Cache Layer)
        RelCatEntry relCatBuff;
        RelCacheTable::getRelCatEntry(relId,&relCatBuff);
        block=relCatBuff.firstBlk;
        slot=0;
        // block = first record block of the relation
        // slot = 0
    }
    else
    {
        // (there is a hit from previous search; search should start from
        // the record next to the search index record)

        // block = search index's block
        // slot = search index's slot + 1
        block=prevRecId.block;
        slot=prevRecId.slot+1;
    }

    /* The following code searches for the next record in the relation
       that satisfies the given condition
       We start from the record id (block, slot) and iterate over the remaining
       records of the relation
    */
    while (block != -1)
    {
        /* create a RecBuffer object for block (use RecBuffer Constructor for
           existing block) */
        // get the record with id (block, slot) using RecBuffer::getRecord()
        // get header of the block using RecBuffer::getHeader() function
        // get slot map of the block using RecBuffer::getSlotMap() function
         RecBuffer buffer(block);
         HeadInfo head;
         buffer.getHeader(&head);
         Attribute rec[head.numAttrs];
         buffer.getRecord(rec,slot);
         
         unsigned char *bufferptr=(unsigned char*)malloc(head.numSlots*sizeof(unsigned char));
         buffer.getSlotMap(bufferptr);
         
        // If slot >= the number of slots per block(i.e. no more slots in this block)
        if(slot>=head.numSlots)
        {
            // update block = right block of block
            // update slot = 0
           // std::cout<<slot;
            block=head.rblock;
            slot=0;
            continue;  // continue to the beginning of this while loop
        }

        // if slot is free skip the loop
        // (i.e. check if slot'th entry in slot map of block contains SLOT_UNOCCUPIED)
        if(*(bufferptr+slot)==SLOT_UNOCCUPIED)
        {
            // increment slot and continue to the next record slot
            slot++;
            continue;
        }

        // compare record's attribute value to the the given attrVal as below:
        /*
            firstly get the attribute offset for the attrName attribute
            from the attribute cache entry of the relation using
            AttrCacheTable::getAttrCatEntry()
        */
       AttrCatEntry attrcatbuff;
       AttrCacheTable::getAttrCatEntry(relId,attrName,&attrcatbuff);


        /* use the attribute offset to get the value of the attribute from
           current record */
           Attribute record=rec[attrcatbuff.offset];
        //    strcpy(record[ATTRCAT_ATTR_NAME_INDEX].sVal,attrcatbuff.attrName);
        //    record[ATTRCAT_ATTR_TYPE_INDEX].nVal=(int)attrcatbuff.attrType;
        //    record[ATTRCAT_PRIMARY_FLAG_INDEX].nVal=(bool)attrcatbuff.primaryFlag;
        //    record[ATTRCAT_OFFSET_INDEX].nVal=(int)attrcatbuff.offset;
        //    record[ATTRCAT_ROOT_BLOCK_INDEX].nVal=(int)attrcatbuff.rootBlock;

        int cmpVal;  // will store the difference between the attributes
        // set cmpVal using compareAttrs()
        
        cmpVal=compareAttrs(record,attrVal,attrcatbuff.attrType);
        /* Next task is to check whether this record satisfies the given condition.
           It is determined based on the output of previous comparison and
           the op value received.
           The following code sets the cond variable if the condition is satisfied.
        */
        if (
            (op == NE && cmpVal != 0) ||    // if op is "not equal to"
            (op == LT && cmpVal < 0) ||     // if op is "less than"
            (op == LE && cmpVal <= 0) ||    // if op is "less than or equal to"
            (op == EQ && cmpVal == 0) ||    // if op is "equal to"
            (op == GT && cmpVal > 0) ||     // if op is "greater than"
            (op == GE && cmpVal >= 0)       // if op is "greater than or equal to"
        ) {
            /*
            set the search index in the relation cache as
            the record id of the record that satisfies the given condition
            (use RelCacheTable::setSearchIndex function)
            */
           RecId id;
            id={block,slot};
            RelCacheTable::setSearchIndex(relId,&id);
            return RecId{block, slot};
        }

        slot++;
    }

    // no record in the relation with Id relid satisfies the given condition
    return RecId{-1, -1};
}

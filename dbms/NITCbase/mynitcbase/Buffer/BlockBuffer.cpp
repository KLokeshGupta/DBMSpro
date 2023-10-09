#include "BlockBuffer.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std;
// the declarations for these functions can be found in "BlockBuffer.h"
// call the corresponding parent constructor
IndBuffer::IndBuffer(char blockType) : BlockBuffer(blockType){}
// call the corresponding parent constructor
IndBuffer::IndBuffer(int blockNum) : BlockBuffer(blockNum){}
// call the corresponding parent constructor
// 'I' used to denote IndInternal.
IndInternal::IndInternal() : IndBuffer('I'){}
// call the corresponding parent constructor
IndInternal::IndInternal(int blockNum) : IndBuffer(blockNum){}
// this is the way to call parent non-default constructor.
// 'L' used to denote IndLeaf.
IndLeaf::IndLeaf() : IndBuffer('L'){} 
//this is the way to call parent non-default constructor.
IndLeaf::IndLeaf(int blockNum) : IndBuffer(blockNum){}

int IndInternal::setEntry(void *ptr, int indexNum) {
  return 0;
}

int IndLeaf::setEntry(void *ptr, int indexNum) {
  return 0;
}

int IndLeaf::getEntry(void *ptr, int indexNum) {

    // if the indexNum is not in the valid range of [0, MAX_KEYS_LEAF-1]
    //     return E_OUTOFBOUND.
    if(indexNum <0 or indexNum>=MAX_KEYS_INTERNAL) return E_OUTOFBOUND;
  
    unsigned char *bufferPtr;
    /* get the starting address of the buffer containing the block
       using loadBlockAndGetBufferPtr(&bufferPtr). */
    int ret=loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret!=SUCCESS) return ret;
    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
    //     return the value returned by the call.

    // copy the indexNum'th Index entry in buffer to memory ptr using memcpy

    /* the indexNum'th entry will begin at an offset of
       HEADER_SIZE + (indexNum * LEAF_ENTRY_SIZE)  from bufferPtr */
    unsigned char *entryPtr = bufferPtr + HEADER_SIZE + (indexNum * LEAF_ENTRY_SIZE);
    memcpy((struct Index *)ptr, entryPtr, LEAF_ENTRY_SIZE);

    return SUCCESS;
}

int IndInternal::getEntry(void *ptr, int indexNum) {
    // if the indexNum is not in the valid range of [0, MAX_KEYS_INTERNAL-1]
    //     return E_OUTOFBOUND.
    if(indexNum<0 or indexNum>=MAX_KEYS_INTERNAL) return E_OUTOFBOUND;
    unsigned char *bufferPtr;
    /* get the starting address of the buffer containing the block
       using loadBlockAndGetBufferPtr(&bufferPtr). */
    int ret=loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret!=SUCCESS) return ret;
    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
    //     return the value returned by the call.

    // typecast the void pointer to an internal entry pointer
    struct InternalEntry *internalEntry = (struct InternalEntry *)ptr;

    /*
    - copy the entries from the indexNum`th entry to *internalEntry
    - make sure that each field is copied individually as in the following code
    - the lChild and rChild fields of InternalEntry are of type int32_t
    - int32_t is a type of int that is guaranteed to be 4 bytes across every
      C++ implementation. sizeof(int32_t) = 4
    */

    /* the indexNum'th entry will begin at an offset of
       HEADER_SIZE + (indexNum * (sizeof(int) + ATTR_SIZE) )         [why?]
       from bufferPtr */
    unsigned char *entryPtr = bufferPtr + HEADER_SIZE + (indexNum * 20);

    memcpy(&(internalEntry->lChild), entryPtr, sizeof(int32_t));
    memcpy(&(internalEntry->attrVal), entryPtr + 4, sizeof(Attribute));
    memcpy(&(internalEntry->rChild), entryPtr + 20, 4);

   return SUCCESS;
}

void BlockBuffer::releaseBlock(){

    // if blockNum is INVALID_BLOCK (-1), or it is invalidated already, do nothing
    if(this->blockNum!=INVALID_BLOCKNUM)
     {
        /* get the buffer number of the buffer assigned to the block
           using StaticBuffer::getBufferNum().
           (this function return E_BLOCKNOTINBUFFER if the block is not
           currently loaded in the buffer)
            */
        int bufferNum=StaticBuffer::getBufferNum(this->blockNum);
        StaticBuffer::metainfo[bufferNum].free=true;
        // if the block is present in the buffer, free the buffer
        // by setting the free flag of its StaticBuffer::tableMetaInfo entry
        // to true.
        StaticBuffer::blockAllocMap[this->blockNum]=UNUSED_BLK;
        this->blockNum=INVALID_BLOCKNUM;
        // free the block in disk by setting the data type of the entry
        // corresponding to the block number in StaticBuffer::blockAllocMap
        // to UNUSED_BLK.

        // set the object's blockNum to INVALID_BLOCK (-1)
     }
}
int BlockBuffer::getBlockNum(){

    //return corresponding block number.
    return (this->blockNum);
}
int RecBuffer::setSlotMap(unsigned char *slotMap) {
    unsigned char *bufferPtr;
    /* get the starting address of the buffer containing the block using
       loadBlockAndGetBufferPtr(&bufferPtr). */
    int ret=loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret!=SUCCESS) return ret;
    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.
    HeadInfo head;
    getHeader(&head);
    // get the header of the block using the getHeader() function

    int numSlots = head.numSlots;

    // the slotmap starts at bufferPtr + HEADER_SIZE. Copy the contents of the
    // argument `slotMap` to the buffer replacing the existing slotmap.
    // Note that size of slotmap is `numSlots`
    unsigned char *slotptr=bufferPtr+HEADER_SIZE;
    memcpy(slotptr,slotMap,numSlots);
    ret=StaticBuffer::setDirtyBit(this->blockNum);
    if(ret!=SUCCESS) return ret;
    return SUCCESS;
    // update dirty bit using StaticBuffer::setDirtyBit
    // if setDirtyBit failed, return the value returned by the call

    // return SUCCESS
}
RecBuffer::RecBuffer() : BlockBuffer('R'){}
// call parent non-default constructor with 'R' denoting record block.
BlockBuffer::BlockBuffer(char blockType){
  if(blockType=='R') blockType=REC;
  else blockType=UNUSED_BLK;
  int block1=getFreeBlock(blockType);
  if(block1<0 or block1>=DISK_BLOCKS){
    std::cout<<"error:block not available";
  }
  this->blockNum=block1;

    // allocate a block on the disk and a buffer in memory to hold the new block of
    // given type using getFreeBlock function and get the return error codes if any.

    // set the blockNum field of the object to that of the allocated block
    // number if the method returned a valid block number,
    // otherwise set the error code returned as the block number.

    // (The caller must check if the constructor allocatted block successfully
    // by checking the value of block number field.)
}
int BlockBuffer::getFreeBlock(int blockType){

    // iterate through the StaticBuffer::blockAllocMap and find the block number
    // of a free block in the disk.
    int block1=-111;
    for(int i=6;i<DISK_BLOCKS;i++){
      int k=StaticBuffer::blockAllocMap[i];
      if(k!=REC and k!=IND_INTERNAL and k!=IND_LEAF) {
        block1=i;
        break;
      }
    }
    // if no block is free, return E_DISKFULL.
    if(block1==-111) return E_DISKFULL;
    this->blockNum=block1;
    int bufferindex=StaticBuffer::getFreeBuffer(this->blockNum);
    if(bufferindex<0 or bufferindex>=BUFFER_CAPACITY){
      printf("error:buffer is full");
      return bufferindex;
    }
    struct HeadInfo head;
    head.pblock=-1;
    head.lblock=-1;
    head.rblock=-1;
    head.numEntries=0;
    head.numSlots=0;
    head.numAttrs=0;
    setHeader(&head);

    // set the object's blockNum to the block number of the free block.

    // find a free buffer using StaticBuffer::getFreeBuffer() .

    // initialize the header of the block passing a struct HeadInfo with values
    // pblock: -1, lblock: -1, rblock: -1, numEntries: 0, numAttrs: 0, numSlots: 0
    // to the setHeader() function.

    // update the block type of the block to the input block type using setBlockType().
    setBlockType(blockType);
    return this->blockNum;
    // return block number of the free block.
}
int BlockBuffer::setBlockType(int blockType){

    unsigned char *bufferPtr;
    /* get the starting address of the buffer containing the block
       using loadBlockAndGetBufferPtr(&bufferPtr). */
    int ret=loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret!=SUCCESS) return ret;
    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.

    // store the input block type in the first 4 bytes of the buffer.
    // (hint: cast bufferPtr to int32_t* and then assign it)
    // *((int32_t *)bufferPtr) = blockType;
      *((int32_t *)bufferPtr)=blockType;
      StaticBuffer::blockAllocMap[this->blockNum]=blockType;
      ret=StaticBuffer::setDirtyBit(this->blockNum);
      if(ret!=SUCCESS) return ret;
      return SUCCESS;
    // update the StaticBuffer::blockAllocMap entry corresponding to the
    // object's block number to `blockType`.

    // update dirty bit by calling StaticBuffer::setDirtyBit()
    // if setDirtyBit() failed
        // return the returned value from the call

    // return SUCCESS
}
int BlockBuffer::setHeader(struct HeadInfo *head){

    unsigned char *bufferPtr;
    // get the starting address of the buffer containing the block using
    // loadBlockAndGetBufferPtr(&bufferPtr).
    int ret=loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret!=SUCCESS) return ret;
    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.

    // cast bufferPtr to type HeadInfo*
    struct HeadInfo *bufferHeader = (struct HeadInfo *)bufferPtr;
    bufferHeader->numAttrs=head->numAttrs;
    bufferHeader->numEntries=head->numEntries;
    bufferHeader->numSlots=head->numSlots;
    bufferHeader->blockType=head->blockType;
    bufferHeader->lblock=head->lblock;
    bufferHeader->rblock=head->rblock;
    bufferHeader->pblock=head->pblock;
    // copy the fields of the HeadInfo pointed to by head (except reserved) to
    // the header of the block (pointed to by bufferHeader)
    //(hint: bufferHeader->numSlots = head->numSlots )
    ret=StaticBuffer::setDirtyBit(this->blockNum);
    if(ret!=SUCCESS) return ret;
    return SUCCESS;
    // update dirty bit by calling StaticBuffer::setDirtyBit()
    // if setDirtyBit() failed, return the error code

    // return SUCCESS;
}
int RecBuffer::setRecord(union Attribute *rec, int slotNum) {
    unsigned char *bufferPtr;
    /* get the starting address of the buffer containing the block
       using loadBlockAndGetBufferPtr(&bufferPtr). */
    int ret=loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret!=SUCCESS) return ret;
    HeadInfo header;
    getHeader(&header);
    if(header.numSlots<=slotNum) return E_OUTOFBOUND;
  //  unsigned char *offset;
    int recordsize=header.numAttrs*ATTR_SIZE;
    unsigned char* offset=bufferPtr+HEADER_SIZE+header.numSlots+(recordsize*slotNum);

    memcpy(offset,rec,header.numAttrs*(ATTR_SIZE));
    int ret12=StaticBuffer::setDirtyBit(this->blockNum);
    if(ret12!=SUCCESS) {
      std::cout<<"error";
      exit(1);
    }
    return SUCCESS;
    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.

    /* get the header of the block using the getHeader() function */

    // get number of attributes in the block.

    // get the number of slots in the block.

    // if input slotNum is not in the permitted range return E_OUTOFBOUND.

    /* offset bufferPtr to point to the beginning of the record at required
       slot. the block contains the header, the slotmap, followed by all
       the records. so, for example,
       record at slot x will be at bufferPtr + HEADER_SIZE + (x*recordSize)
       copy the record from `rec` to buffer using memcpy
       (hint: a record will be of size ATTR_SIZE * numAttrs)
    */

    // update dirty bit using setDirtyBit()

    /* (the above function call should not fail since the block is already
       in buffer and the blockNum is valid. If the call does fail, there
       exists some other issue in the code) */

    // return SUCCESS
}

int compareAttrs(union Attribute attr1, union Attribute attr2, int attrType) {

    double diff;
    if(attrType==STRING) diff=strcmp(attr1.sVal,attr2.sVal);
    else diff=attr1.nVal-attr2.nVal;
    if(diff>0) return 1;
    else if(diff<0) return -1;
    else return 0;
    // if attrType == STRING
    //     diff = strcmp(attr1.sval, attr2.sval)

    // else
    //     diff = attr1.nval - attr2.nval

    /*
    if diff > 0 then return 1
    if diff < 0 then return -1
    if diff = 0 then return 0
    */
}
/* used to get the slotmap from a record block
NOTE: this function expects the caller to allocate memory for `*slotMap`
*/
int RecBuffer::getSlotMap(unsigned char *slotMap) {
  unsigned char *bufferPtr;

  // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr().
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;
  }

  struct HeadInfo head;
  // get the header of the block using getHeader() function
  getHeader(&head);  

  int slotCount = /* number of slots in block from header */head.numSlots;

  // get a pointer to the beginning of the slotmap in memory by offsetting HEADER_SIZE
  unsigned char *slotMapInBuffer = bufferPtr + HEADER_SIZE;

  // copy the values from `slotMapInBuffer` to `slotMap` (size is `slotCount`)
  memcpy(slotMap,slotMapInBuffer,slotCount);

  return SUCCESS;
}

/*                    ---STAGE 3 ---                */
BlockBuffer::BlockBuffer(int blockNum) {
  // initialise this.blockNum with the argument
  this->blockNum=blockNum;
}

// calls the parent class constructor
RecBuffer::RecBuffer(int blockNum) : BlockBuffer::BlockBuffer(blockNum) {}
/*
Used to get the header of the block into the location pointed to by `head`
NOTE: this function expects the caller to allocate memory for `head`
*/
int BlockBuffer::getHeader(struct HeadInfo *head) {

  unsigned char *bufferPtr;
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;   // return any errors that might have occured in the process
  }

  // ... (the rest of the logic is as in stage 2)
  unsigned char buffer[BLOCK_SIZE];

  // read the block at this.blockNum into the buffer
  // populate the numEntries, numAttrs and numSlots fields in *head
  memcpy(&head->numSlots, bufferPtr + 24, 4);
  memcpy(&head->numEntries, bufferPtr+16, 4);
  memcpy(&head->numAttrs, bufferPtr+20, 4);
  memcpy(&head->rblock, bufferPtr+12, 4);
  memcpy(&head->lblock, bufferPtr+8, 4);
  memcpy(&head->pblock,bufferPtr+4,4);
  return SUCCESS;
  // ... (the rest of the logic is as in stage 2)
}

/*
Used to get the record at slot `slotNum` into the array `rec`
NOTE: this function expects the caller to allocate memory for `rec`
*/
int RecBuffer::getRecord(union Attribute *rec, int slotNum) {
  // ...
  unsigned char *bufferPtr;
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;
  }
  struct HeadInfo head;

  // get the header using this.getHeader() function
  this->getHeader(&head);

  int attrCount = head.numAttrs;
  int slotCount = head.numSlots;

  // read the block at this.blockNum into a buffer

  // record at slotNum will be at offset HEADER_SIZE + slotMapSize + (recordSize * slotNum)
  //    - each record will have size attrCount * ATTR_SIZE
  //    - slotMap will be of size slotCount
  // 
  int recordSize = attrCount * ATTR_SIZE;
  // calculate buffer + offset  
  unsigned char *slotPointer = bufferPtr+HEADER_SIZE+slotCount+(recordSize*slotNum);

  // load the record into the rec data structure
  memcpy(rec, slotPointer, recordSize);

  return SUCCESS;
  // ... (the rest of the logic is as in stage 2
}

/*
Used to load a block to the buffer and get a pointer to it.
NOTE: this function expects the caller to allocate memory for the argument
*/
int BlockBuffer::loadBlockAndGetBufferPtr(unsigned char **buffPtr) {
  // check whether the block is already present in the buffer using StaticBuffer.getBufferNum()
  int bufferNum = StaticBuffer::getBufferNum(this->blockNum);

  if (bufferNum == E_BLOCKNOTINBUFFER) {
    bufferNum = StaticBuffer::getFreeBuffer(this->blockNum);

    if (bufferNum == E_OUTOFBOUND) {
      return E_OUTOFBOUND;
    }

    Disk::readBlock(StaticBuffer::blocks[bufferNum], this->blockNum);
  }
  else{
    for(int bufferindex=0;bufferindex<BUFFER_CAPACITY;bufferindex++){
      if(!StaticBuffer::metainfo[bufferindex].free){
        StaticBuffer::metainfo[bufferindex].timeStamp+=1;
      }
    }
    StaticBuffer::metainfo[bufferNum].timeStamp=0;
  }
  // store the pointer to this buffer (blocks[bufferNum]) in *buffPtr
  *buffPtr = StaticBuffer::blocks[bufferNum];
  

  return SUCCESS;
}


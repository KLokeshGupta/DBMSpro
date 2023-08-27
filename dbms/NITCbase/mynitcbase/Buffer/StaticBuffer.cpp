#include "StaticBuffer.h"
#include <bits/stdc++.h>
// the declarations for this class can be found at "StaticBuffer.h"

unsigned char StaticBuffer::blocks[BUFFER_CAPACITY][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer::metainfo[BUFFER_CAPACITY];
unsigned char StaticBuffer::blockAllocMap[DISK_BLOCKS];
StaticBuffer::StaticBuffer() {
  // initialise all blocks as free
  for (/*bufferIndex = 0 to BUFFER_CAPACITY-1*/ int bufferIndex=0;bufferIndex<BUFFER_CAPACITY;bufferIndex++) {
    metainfo[bufferIndex].free = true;
    metainfo[bufferIndex].blockNum=-1;
    metainfo[bufferIndex].dirty=false;
    metainfo[bufferIndex].timeStamp=-1;
  }
  for(int i=0;i<4;i++){
    Disk::readBlock(blocks[i],i);
  }
}


/*
At this stage, we are not writing back from the buffer to the disk since we are
not modifying the buffer. So, we will define an empty destructor for now. In
subsequent stages, we will implement the write-back functionality here.
*/
StaticBuffer::~StaticBuffer() {
  for(int index=0;index<4;index++){
    Disk::writeBlock(blocks[index],index);
  }
  for(int bufferIndex=0;bufferIndex<BUFFER_CAPACITY;bufferIndex++){
    if(!metainfo[bufferIndex].free){
      if(metainfo[bufferIndex].dirty){
        //std::cout <<bufferIndex;
        Disk::writeBlock(StaticBuffer::blocks[bufferIndex],metainfo[bufferIndex].blockNum);
      }
    }
  }
}
int StaticBuffer::setDirtyBit(int blockNum){
    // find the buffer index corresponding to the block using getBufferNum().
    int bufferindex=getBufferNum(blockNum);
    if(bufferindex==E_BLOCKNOTINBUFFER) return E_BLOCKNOTINBUFFER;
    if(bufferindex==E_OUTOFBOUND) return E_OUTOFBOUND;
    metainfo[bufferindex].dirty=true;
    return SUCCESS;
    // if block is not present in the buffer (bufferNum = E_BLOCKNOTINBUFFER)
    //     return E_BLOCKNOTINBUFFER

    // if blockNum is out of bound (bufferNum = E_OUTOFBOUND)
    //     return E_OUTOFBOUND

    // else
    //     (the bufferNum is valid)
    //     set the dirty bit of that buffer to true in metainfo

    // return SUCCESS
}
int StaticBuffer::getFreeBuffer(int blockNum) {
  if (blockNum < 0 || blockNum > DISK_BLOCKS) {
    return E_OUTOFBOUND;
  }

    // increase the timeStamp in metaInfo of all occupied buffers.
    // let bufferNum be used to store the buffer number of the free/freed buffer.
    int bufferNum=999;
    int bufferindex;
    int time1=0,ind=1000;
    for(bufferindex=0;bufferindex<BUFFER_CAPACITY;bufferindex++){
         if(!metainfo[bufferindex].free) {
          metainfo[bufferindex].timeStamp+=1;
          if(time1<metainfo[bufferindex].timeStamp){
            time1=metainfo[bufferindex].timeStamp;
            ind=bufferindex;
          }
         }

     }
    // iterate through metainfo and check if there is any buffer free
    for(bufferindex=0;bufferindex<BUFFER_CAPACITY;bufferindex++){
         if(metainfo[bufferindex].free) {
            bufferNum=bufferindex;
            break;
         }
     }
     if(bufferNum==99){
      if(metainfo[ind].dirty){
        Disk::writeBlock(StaticBuffer::blocks[ind],metainfo[ind].blockNum);
      }
      bufferNum=ind;
     }
    // if a free buffer is available, set bufferNum = index of that free buffer.

    // if a free buffer is not available,
    //     find the buffer with the largest timestamp
    //     IF IT IS DIRTY, write back to the disk using Disk::writeBlock()
    //     set bufferNum = index of this buffer
    metainfo[bufferNum].blockNum=blockNum;
    metainfo[bufferNum].dirty=false;
    metainfo[bufferNum].free=false;
    metainfo[bufferNum].timeStamp=0;
    // update the metaInfo entry corresponding to bufferNum with
    // free:false, dirty:false, blockNum:the input block number, timeStamp:0.
    return bufferNum;
    // return the bufferNum.

  // iterate through all the blocks in the StaticBuffer
  // find the first free block in the buffer (check metainfo)
  // assign allocatedBuffer = index of the free block
  //   int bufferIndex;
  //   for(bufferIndex=0;bufferIndex<BUFFER_CAPACITY;bufferIndex++){
  //       if(metainfo[bufferIndex].free==true) break;
  //   }
  //   allocatedBuffer=bufferIndex;
  // metainfo[allocatedBuffer].free = false;
  // metainfo[allocatedBuffer].blockNum = blockNum;

  // return allocatedBuffer;
}

/* Get the buffer index where a particular block is stored
   or E_BLOCKNOTINBUFFER otherwise
*/
int StaticBuffer::getBufferNum(int blockNum) {
  // Check if blockNum is valid (between zero and DISK_BLOCKS)
  if(blockNum<0 || blockNum >DISK_BLOCKS) return E_OUTOFBOUND;
  // and return E_OUTOFBOUND if not valid.
    for(int bufferIndex=0;bufferIndex<BUFFER_CAPACITY;bufferIndex++){
        if(metainfo[bufferIndex].blockNum==blockNum) return bufferIndex;
    }
  // find and return the bufferIndex which corresponds to blockNum (check metainfo)

  // if block is not in the buffer
  return E_BLOCKNOTINBUFFER;
}
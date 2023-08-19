#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include "bits/stdc++.h"
using namespace std;
int main(int argc, char *argv[]) {

  Disk disk_run;
  StaticBuffer buffer;
  OpenRelTable cache;
  // hello loki
   return FrontendInterface::handleFrontend(argc, argv);
  /*for i = 0 and i = 1 (i.e RELCAT_RELID and ATTRCAT_RELID)

      get the relation catalog entry using RelCacheTable::getRelCatEntry()
      printf("Relation: %s\n", relname);

      for j = 0 to numAttrs of the relation - 1
          get the attribute catalog entry for (rel-id i, attribute offset j)
           in attrCatEntry using AttrCacheTable::getAttrCatEntry()

          printf("  %s: %s\n", attrName, attrType);*/
  //  for(int i=0;i<2;i++){
  //   RelCatEntry relCatBuff;
  //   RelCacheTable::getRelCatEntry(i,&relCatBuff);
  //   printf("Relation: %s\n", relCatBuff.relName);
  //   for(int j=0;j<relCatBuff.numAttrs;j++){
  //   AttrCatEntry attrCatBuff;
  //   AttrCacheTable::getAttrCatEntry(i,j,&attrCatBuff);
  //   const char* attrType=attrCatBuff.attrType==NUMBER?"NUM":"STR";
  //     printf("  %s: %s\n", attrCatBuff.attrName,attrType);
  //    }
  // }
  // // load the headers of both the blocks into relCatHeader and attrCatHeader.
  // // (we will implement these functions later)
  // relCatBuffer.getHeader(&relCatHeader);
  // attrCatBuffer.getHeader(&attrCatHeader);
  // int k = 0;

  // for (/* i = 0 to total relation count */int i=0;i<relCatHeader.numEntries;i++) {

  //   Attribute relCatRecord[RELCAT_NO_ATTRS]; // will store the record from the relation catalog

  //   relCatBuffer.getRecord(relCatRecord, i);

  //   printf("Relation: %s\n", relCatRecord[RELCAT_REL_NAME_INDEX].sVal);
  //   for (/* j = 0 to number of entries in the attribute catalog */int j=0;j<attrCatHeader.numEntries;j++) {
  //     // declare attrCatRecord and load the attribute catalog entry into it
  //     Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
  //     attrCatBuffer.getRecord(attrCatRecord,j);
  //     if (/* attribute catalog entry corresponds to the current relation */!strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal,relCatRecord[RELCAT_REL_NAME_INDEX].sVal)) {
  //       const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
  //       if(!(strcmp(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,"Class"))){
  //          const char A[6]="Batch";
  //          strcpy(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,A);
  //         //cout <<"$";
          
  //       }
  //       printf("  %s: %s\n", /* get the attribute name */attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);
  //     }
  //   }
  //   k=attrCatHeader.rblock;
  //   RecBuffer attrCatBuffer1(k);
  //   HeadInfo attrCatHeader1;
  //   attrCatBuffer1.getHeader(&attrCatHeader1);
  //   for(int j=0;j<attrCatHeader1.numEntries;j++){
  //      Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
  //     attrCatBuffer1.getRecord(attrCatRecord,j);
  //     if (/* attribute catalog entry corresponds to the current relation */!strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal,relCatRecord[RELCAT_REL_NAME_INDEX].sVal)) {
  //       const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
  //       printf("  %s: %s\n", /* get the attribute name */attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);
  //     }
  //   }
  //   printf("\n");
  // }

  return 0;
}
/*                       ---STAGE 1---                    */
// int main(int argc, char *argv[]) {
//  Disk disk_run;

//  unsigned char buffer[BLOCK_SIZE];
//  Disk::readBlock(buffer, 7000);
//  char message[] = "hello";
//  memcpy(buffer + 20, message, 6);
//  Disk::writeBlock(buffer, 7000);

//  unsigned char buffer2[BLOCK_SIZE];
//  char message2[6];
//  Disk::readBlock(buffer2, 7000);
//  memcpy(message2, buffer2 + 20, 6);
//  std::cout << message2;
//    unsigned char buffer[BLOCK_SIZE];
  
//    Disk::readBlock(buffer,0);
//     for(int i=0;i<2048;i++){
//    char ch;
//    memcpy(ch,buffer+i,1);
//    cout <<ch <<endl;
// }
//   unsigned char buffer[BLOCK_SIZE];
//   Disk::readBlock(buffer,2);
//   for(int i=0;i<2048;i++){
//     cout<<(int)buffer[i] <<" ";
//   }
//  return 0;
// }
//
  /*                      --- STAGE 2---                   */
// int main(int argc, char *argv[]) {
//   Disk disk_run;

//   // create objects for the relation catalog and attribute catalog
//   RecBuffer relCatBuffer(RELCAT_BLOCK);
//   RecBuffer attrCatBuffer(ATTRCAT_BLOCK);

//   HeadInfo relCatHeader;
//   HeadInfo attrCatHeader;

//   // load the headers of both the blocks into relCatHeader and attrCatHeader.
//   // (we will implement these functions later)
//   relCatBuffer.getHeader(&relCatHeader);
//   attrCatBuffer.getHeader(&attrCatHeader);
//   int k = 0;

//   for (/* i = 0 to total relation count */int i=0;i<relCatHeader.numEntries;i++) {

//     Attribute relCatRecord[RELCAT_NO_ATTRS]; // will store the record from the relation catalog

//     relCatBuffer.getRecord(relCatRecord, i);

//     printf("Relation: %s\n", relCatRecord[RELCAT_REL_NAME_INDEX].sVal);
//     for (/* j = 0 to number of entries in the attribute catalog */int j=0;j<attrCatHeader.numEntries;j++) {
//       // declare attrCatRecord and load the attribute catalog entry into it
//       Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
//       attrCatBuffer.getRecord(attrCatRecord,j);
//       if (/* attribute catalog entry corresponds to the current relation */!strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal,relCatRecord[RELCAT_REL_NAME_INDEX].sVal)) {
//         const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
//         if(!(strcmp(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,"Class"))){
//            const char A[6]="Batch";
//            strcpy(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,A);
//           //cout <<"$";
          
//         }
//         printf("  %s: %s\n", /* get the attribute name */attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);
//       }
//     }
//     k=attrCatHeader.rblock;
//     RecBuffer attrCatBuffer1(k);
//     HeadInfo attrCatHeader1;
//     attrCatBuffer1.getHeader(&attrCatHeader1);
//     for(int j=0;j<attrCatHeader1.numEntries;j++){
//        Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
//       attrCatBuffer1.getRecord(attrCatRecord,j);
//       if (/* attribute catalog entry corresponds to the current relation */!strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal,relCatRecord[RELCAT_REL_NAME_INDEX].sVal)) {
//         const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
//         printf("  %s: %s\n", /* get the attribute name */attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);
//       }
//     }
//     printf("\n");
//   }

//   return 0;
// }

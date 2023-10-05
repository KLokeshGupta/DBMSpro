#include "Algebra.h"

#include <cstring>
#include <bits/stdc++.h>
// will return if a string can be parsed as a floating point number
bool isNumber(char *str) {
  int len;
  float ignore;
  /*
    sscanf returns the number of elements read, so if there is no float matching
    the first %f, ret will be 0, else it'll be 1

    %n gets the number of characters read. this scanf sequence will read the
    first float ignoring all the whitespace before and after. and the number of
    characters read that far will be stored in len. if len == strlen(str), then
    the string only contains a float with/without whitespace. else, there's other
    characters.
  */
  int ret = sscanf(str, "%f %n", &ignore, &len);
  return ret == 1 && len == strlen(str);
}
    
int Algebra::project(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE]) {

    int srcRelId = OpenRelTable::getRelId(srcRel);
    if(srcRelId==E_RELNOTOPEN) return E_RELNOTOPEN;
    RelCatEntry relCatEntry;
    int retVal=RelCacheTable::getRelCatEntry(srcRelId,&relCatEntry);
    if(retVal!=SUCCESS) return retVal;
    int numAttrs=relCatEntry.numAttrs;

    // if srcRel is not open in open relation table, return E_RELNOTOPEN

    // get RelCatEntry of srcRel using RelCacheTable::getRelCatEntry()

    // get the no. of attributes present in relation from the fetched RelCatEntry.

    // attrNames and attrTypes will be used to store the attribute names
    // and types of the source relation respectively
    char attrNames[numAttrs][ATTR_SIZE];
    int attrTypes[numAttrs];

    /*iterate through every attribute of the source relation :
        - get the AttributeCat entry of the attribute with offset.
          (using AttrCacheTable::getAttrCatEntry())
        - fill the arrays `attrNames` and `attrTypes` that we declared earlier
          with the data about each attribute
    */
   for(int i=0;i<numAttrs;i++){
    AttrCatEntry attrCatEntry;
    AttrCacheTable::getAttrCatEntry(srcRelId,i,&attrCatEntry);
    strcpy(attrNames[i],attrCatEntry.attrName);
    attrTypes[i]=attrCatEntry.attrType;
   }

    /*** Creating and opening the target relation ***/

    // Create a relation for target relation by calling Schema::createRel()

    // if the createRel returns an error code, then return that value.

    // Open the newly created target relation by calling OpenRelTable::openRel()
    // and get the target relid

    // If opening fails, delete the target relation by calling Schema::deleteRel() of
    // return the error value returned from openRel().
    int ret=Schema::createRel(targetRel,numAttrs,attrNames,attrTypes);
    if(ret!=SUCCESS) return ret;
    ret=OpenRelTable::openRel(targetRel);
    if(ret!=SUCCESS) {
        Schema::deleteRel(targetRel);
        return ret;
    }
    int targetRelId=OpenRelTable::getRelId(targetRel);


    /*** Inserting projected records into the target relation ***/

    // Take care to reset the searchIndex before calling the project function
    // using RelCacheTable::resetSearchIndex()
    RelCacheTable::resetSearchIndex(srcRelId);
    Attribute record[numAttrs];


    while (BlockAccess::project(srcRelId,record)==SUCCESS)
    {
        // record will contain the next record
        ret=BlockAccess::insert(targetRelId,record);
        // ret = BlockAccess::insert(targetRelId, proj_record);

        if (ret!=SUCCESS) {
            // close the targetrel by calling Schema::closeRel()
            // delete targetrel by calling Schema::deleteRel()
            // return ret;
            Schema::closeRel(targetRel);
            Schema::deleteRel(targetRel);
            return ret;
        }
    }

    Schema::closeRel(targetRel);

     return SUCCESS;
}
int Algebra::select(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char strVal[ATTR_SIZE]) {
    // get the srcRel's rel-id (let it be srcRelid), using OpenRelTable::getRelId()
    // if srcRel is not open in open relation table, return E_RELNOTOPEN
    int srcrelId=OpenRelTable::getRelId(srcRel);
    if(srcrelId==E_RELNOTOPEN) return E_RELNOTOPEN;
    // get the attr-cat entry for attr, using AttrCacheTable::getAttrCatEntry()
    // if getAttrcatEntry() call fails return E_ATTRNOTEXIST
    AttrCatEntry attrCatEntry;
    int retVal=AttrCacheTable::getAttrCatEntry(srcrelId,attr,&attrCatEntry);
    if(retVal!=SUCCESS) return E_ATTRNOTEXIST;

    /*** Convert strVal to an attribute of data type NUMBER or STRING ***/

    Attribute attrVal;
    int type = attrCatEntry.attrType;

    if (type == NUMBER)
    {
        // if the input argument strVal can be converted to a number
        if(isNumber(strVal))
        // (check this using isNumber() function)
        {
            // convert strVal to double and store it at attrVal.nVal using atof()
            attrVal.nVal=atof(strVal);
        }
        else
        {
            return E_ATTRTYPEMISMATCH;
        }
    }
    else if (type == STRING)
    {
        // copy strVal to attrVal.sVal
        strcpy(attrVal.sVal,strVal);
    }

    /*** Creating and opening the target relation ***/
    // Prepare arguments for createRel() in the following way:
    // get RelcatEntry of srcRel using RelCacheTable::getRelCatEntry()
    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(srcrelId,&relCatEntry);
    int src_nAttrs = /* the no. of attributes present in src relation */ relCatEntry.numAttrs;


    /* let attr_names[src_nAttrs][ATTR_SIZE] be a 2D array of type char
        (will store the attribute names of rel). */
    // let attr_types[src_nAttrs] be an array of type int
    char attr_names[src_nAttrs][ATTR_SIZE];
    int attr_types[src_nAttrs];
    for(int i=0;i<src_nAttrs;i++){
        AttrCatEntry attrCatEntry;
        AttrCacheTable::getAttrCatEntry(srcrelId,i,&attrCatEntry);
        strcpy(attr_names[i],attrCatEntry.attrName);
        attr_types[i]=attrCatEntry.attrType;
    }
    /*iterate through 0 to src_nAttrs-1 :
        get the i'th attribute's AttrCatEntry using AttrCacheTable::getAttrCatEntry()
        fill the attr_names, attr_types arrays that we declared with the entries
        of corresponding attributes
    */


    /* Create the relation for target relation by calling Schema::createRel()
       by providing appropriate arguments */
    // if the createRel returns an error code, then return that value.
    retVal=Schema::createRel(targetRel,src_nAttrs,attr_names,attr_types);
    if(retVal!=SUCCESS) return retVal;
    /* Open the newly created target relation by calling OpenRelTable::openRel()
       method and store the target relid */
    int targetrelID=OpenRelTable::openRel(targetRel);
    if(targetrelID!=SUCCESS) return targetrelID;
    /* If opening fails, delete the target relation by calling Schema::deleteRel()
       and return the error value returned from openRel() */

    /*** Selecting and inserting records into the target relation ***/
    /* Before calling the search function, reset the search to start from the
       first using RelCacheTable::resetSearchIndex() */

    RelCacheTable::resetSearchIndex(srcrelId);
    Attribute record[src_nAttrs];

    /*
        The BlockAccess::search() function can either do a linearSearch or
        a B+ tree search. Hence, reset the search index of the relation in the
        relation cache using RelCacheTable::resetSearchIndex().
        Also, reset the search index in the attribute cache for the select
        condition attribute with name given by the argument `attr`. Use
        AttrCacheTable::resetSearchIndex().
        Both these calls are necessary to ensure that search begins from the
        first record.
    */
    RelCacheTable::resetSearchIndex(srcrelId);
    // AttrCacheTable::resetSearchIndex(/* fill arguments */);

    // read every record that satisfies the condition by repeatedly calling
    // BlockAccess::search() until there are no more records to be read
    Attribute relNameAttr;
    strcpy(relNameAttr.sVal,srcRel);
    while (/* BlockAccess::search() returns success */BlockAccess::search(srcrelId,record,"RelName",relNameAttr,EQ)==SUCCESS) {
        // ret = BlockAccess::insert(targetRelId, record);
        int ret=BlockAccess::insert(targetrelID,record);
        // if (insert fails) {
        //     close the targetrel(by calling Schema::closeRel(targetrel))
        //     delete targetrel (by calling Schema::deleteRel(targetrel))
        //     return ret;
        // }
        if(ret!=SUCCESS){
            Schema::closeRel(targetRel);
            Schema::deleteRel(targetRel);
            return ret;
        }
    }

    // Close the targetRel by calling closeRel() method of schema layer
    Schema::closeRel(targetRel);
     return SUCCESS;
}
int Algebra::insert(char relName[ATTR_SIZE], int nAttrs, char record[][ATTR_SIZE]){
    // if relName is equal to "RELATIONCAT" or "ATTRIBUTECAT"
    // return E_NOTPERMITTED;
    if((!strcmp("RELATIONCAT",relName)) or (!strcmp("ATTRIBUTECAT",relName)) ) return E_NOTPERMITTED;
    // get the relation's rel-id using OpenRelTable::getRelId() method
    int relId = OpenRelTable::getRelId(relName);

    // if relation is not open in open relation table, return E_RELNOTOPEN
    // (check if the value returned from getRelId function call = E_RELNOTOPEN)
    // get the relation catalog entry from relation cache
    // (use RelCacheTable::getRelCatEntry() of Cache Layer)
  if(relId==E_RELNOTOPEN) return E_RELNOTOPEN;
  RelCatEntry relCatEntry;
  RelCacheTable::getRelCatEntry(relId,&relCatEntry);
    /* if relCatEntry.numAttrs != numberOfAttributes in relation,
       return E_NATTRMISMATCH */
  if(relCatEntry.numAttrs!=nAttrs) return E_NATTRMISMATCH;
    // let recordValues[numberOfAttributes] be an array of type union Attribute

    /*
        Converting 2D char array of record values to Attribute array recordValues
     */
    Attribute recordValues[nAttrs];
    for(int i=0;i<nAttrs;i++)
    // iterate through 0 to nAttrs-1: (let i be the iterator)
    {
        // get the attr-cat entry for the i'th attribute from the attr-cache
        // (use AttrCacheTable::getAttrCatEntry())

        // let type = attrCatEntry.attrType;
        AttrCatEntry attrCatEntry;
        AttrCacheTable::getAttrCatEntry(relId,i,&attrCatEntry);
        int type=attrCatEntry.attrType;
        if (type == NUMBER)
        {
            // if the char array record[i] can be converted to a number
            // (check this using isNumber() function)
            if(isNumber(record[i]))
            {
                /* convert the char array to numeral and store it
                   at recordValues[i].nVal using atof() */
                   recordValues[i].nVal=atof(record[i]);
            }
            else
            {
                return E_ATTRTYPEMISMATCH;
            }
        }
        else if (type == STRING)
        {
            strcpy(recordValues[i].sVal,record[i]);
            // copy record[i] to recordValues[i].sVal
        }
    }

    // insert the record by calling BlockAccess::insert() function
    // let retVal denote the return value of insert call
    int retVal=BlockAccess::insert(relId,recordValues);
    return retVal;
}
/* used to select all the records that satisfy a condition.
the arguments of the function are
- srcRel - the source relation we want to select from
- targetRel - the relation we want to select into. (ignore for now)
- attr - the attribute that the condition is checking
- op - the operator of the condition
- strVal - the value that we want to compare against (represented as a string)
*/

// will return if a string can be parsed as a floating point number



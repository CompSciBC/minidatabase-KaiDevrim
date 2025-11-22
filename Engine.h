#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>   
#include <vector>     
#include "BST.h"      
#include "Record.h"
//add header files as needed

using namespace std;

// Converts a string to lowercase (used for case-insensitive searches)
static inline string toLower(string s) {
    for (char &c : s) c = (char)tolower((unsigned char)c);
    return s;
}

// ================== Index Engine ==================
// Acts like a small "database engine" that manages records and two BST indexes:
// 1) idIndex: maps student_id → record index (unique key)
// 2) lastIndex: maps lowercase(last_name) → list of record indices (non-unique key)
struct Engine {
    vector<Record> heap;                  // the main data store (simulates a heap file)
    BST<int, int> idIndex;                // index by student ID
    BST<string, vector<int>> lastIndex;   // index by last name (can have duplicates)
    int idRange = 0;

    // Inserts a new record and updates both indexes.
    // Returns the record ID (RID) in the heap.
    int insertRecord(const Record &recIn) {
        int recordIndex = (rand() % 1000) + 1;
        if (recordIndex > idRange) {
            idRange = recordIndex;
        }
        heap.push_back(recIn);
        idIndex.insert(recIn.id, recordIndex);

        // this may output null
        // null means we have no duplicates therefore we need to make a new vector
        // not null means we just push the heap.size to it
        vector<int>* lastIndexKey = lastIndex.find(recIn.last);
        vector<int> tempVector;
        if (lastIndexKey != nullptr) {
            lastIndexKey->push_back(recordIndex);
            lastIndex.insert(recIn.last, *lastIndexKey);
        } else if (lastIndexKey == nullptr) {
            tempVector.push_back(recordIndex);
            lastIndex.insert(recIn.last, tempVector);
        }
        return recordIndex;
    }

    // Deletes a record logically (marks as deleted and updates indexes)
    // Returns true if deletion succeeded.
    bool deleteById(int id) {
        // find idIndex
        int idIndexPtr = *(idIndex.find(id));
        heap[idIndexPtr].deleted = true;
        return idIndex.erase(id);
    }

    // Finds a record by student ID.
    // Returns a pointer to the record, or nullptr if not found.
    // Outputs the number of comparisons made in the search.
    const Record *findById(int id, int &cmpOut) {
        int* idIndexPtr = idIndex.find(id);
        if (heap[id].deleted) {
            return nullptr;
        }
        return &heap[*idIndexPtr];
    }

    // Returns all records with ID in the range [lo, hi].
    // Also reports the number of key comparisons performed.
    vector<const Record *> rangeById(int lo, int hi, int &cmpOut) {
        vector<const Record*> out;
        idIndex.rangeApply(lo, hi, [&](const int &k, int &rid) {
            cmpOut++;
            if (rid >= 0 && rid < idRange && !heap[rid].deleted) {
                out.push_back(&heap[rid]);
            }
        });
        return out;
    }

    // Returns all records whose last name begins with a given prefix.
    // Case-insensitive using lowercase comparison.
    vector<const Record *> prefixByLast(const string &prefix, int &cmpOut) {
        vector<const Record*> out;
        idIndex.rangeApply(0, idRange, [&](const int &k, int &rid) {
            string lastName = heap[rid].last;
            cmpOut++;
            if (toLower(lastName) == toLower(prefix)) {
                out.push_back(&heap[rid]);
            }
        });
        return out;
    }
};

#endif

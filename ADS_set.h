#ifndef ADS_SET_H
#define ADS_SET_H

#define DEFAULT_BUCKET_SIZE 5


template <typename Key, size_t BucketSize = DEFAULT_BUCKET_SIZE>
class ADS_set
{
public:
    // forward declarations
    class Iterator;
    struct Bucket;
    // struct Element;

    using value_type = Key;
    using key_type = Key;
    using pointer = key_type*;
    using reference = key_type &;
    using const_reference = const key_type &;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = Iterator;
    using const_iterator = Iterator;
    using key_compare = std::less<key_type>;    // B+-Tree
    using key_equal = std::equal_to<key_type>;  // Hashing
    using hasher = std::hash<key_type>;         // Hashing

private:
    //Zusatz variable - static
    key_type* last_incorrect_erase_ptr; //NULL
    key_type last_incorrect_erase; //NULL

    size_type  actualLineCounter;
    // how many elements are already present within the set
    size_type elementCounter;

    // splitting counter (a.k.a. "splitting rounds", or labelled as 'i')
    size_type splitCounter;

    /**
     * Indicates the current size of the `table`,
     * or in other words how many Bucket-lines there currently are.
     */
    size_type bucketLineCounter;

    // index-table of Buckets (Bucket pointers),
    // hence the pointer to an array of pointers to Bucket-objects
    Bucket** table;

    // points to the Bucket-line that is meant to get splitted next if page-overflow occurs
    // NOTE: to reache the `next`-Bucket via a pointer, simply do `table + nextLineSplitIndex`
    // NOTE: the range of this depends on the current splitting-round, hence up-to and including "lastLineSplitIndex"!
    size_type nextLineSplitIndex;

    // indicates the last to-be-splitted Bucket for the current splitting-round
    size_type lastLineSplitIndex;
    size_type  test;

/* NOTES:
- `lastBucketIndex` would be possible to obtain, by doing 2^i -1.
	The `lastBucketIndex` would be indicate when to stop the current splitting-round (usually labelled as 'i'),
	after which a new round would start, and the `nextLineSplitIndex` would get incremented (sometimes labelled as 'p').
*/

public:
    //Zusatz
   /* friend bool z(const ADS_set<Key,BucketSize>& a, const ADS_set<Key,BucketSize>& b){
        //return std::make_pair(Iterator(value_it,curLineBucket,x,bucketLineIndex,table,actualLineCounter),false);
        size_t lastIndex = a.actualLineCounter;
        Bucket **bucketList = a.table;
        size_t index = 0;
        Bucket * bucketIndex = a.table[0];
        pointer ptr = &(a.table[0][0]);
        size_t index_in_bucket = 0;

        int elemCountA = 0;
        double a_size = 0;
        int elemCountB = 0;
        double b_size = 0;

        //A loop

        index_in_bucket++;

        while (true) {
            if (index == lastIndex) {
                ptr = nullptr;
                break;
            }
            if (a->bucketIndex == nullptr) {
                if (a->index == a->lastIndex) {
                    a->ptr = nullptr;
                    break;
                }
                a->index++;
                a->bucketIndex = a->bucketList[a->index];
                a->index_in_bucket = 0;
            }
            else if (a->index_in_bucket < a->bucketIndex->size) {
                a->ptr = a->bucketIndex->values + a->index_in_bucket;

                //here goas the code
                a_size++;
                if(b.count(*(a->ptr)) == 1){
                    elemCountA++;
                }
            }
            else if (a->index_in_bucket == a->bucketIndex->size) {
                a->bucketIndex = a->bucketIndex->get_next();
                a->index_in_bucket = 0;
            }
        }

        lastIndex = b.actualLineCounter;
        bucketList = b.table;
        index = 0;
        bucketIndex = b.table[0];
        ptr = b.table[0][0];
        index_in_bucket = 0;


        //B loop
        b->index_in_bucket++;
        while (true) {
            if (index == b->lastIndex) {
                b->ptr = nullptr;
                break;
            }
            if (b->bucketIndex == nullptr) {
                if (index == b->lastIndex) {
                    b->ptr = nullptr;
                    break;
                }
                b->index++;
                b->bucketIndex = a->bucketList[a->index];
                b->index_in_bucket = 0;
            }
            else if (a->index_in_bucket < a->bucketIndex->size) {
                b->ptr = a->bucketIndex->values + a->index_in_bucket;

                b_size++;
                if(a.count(*(b->ptr)) == 1){
                    elemCountB++;
                }
            }
            else if (b->index_in_bucket == b->bucketIndex->size) {
                b->bucketIndex = b->bucketIndex->get_next();
                b->index_in_bucket = 0;
            }
        }

        double pA = elemCountB / a_size;
        double pB = elemCountA / b_size;
        if(a_size == 0 && b_size == 0){return true;}
        if(pA >= 0.9 && pB >= 0.9){return true;}
        return false;
    }*/

    ADS_set();
    ADS_set(std::initializer_list<key_type> ilist);
    // PH1
    template<typename InputIt>
    ADS_set(InputIt first, InputIt last);
    ADS_set(const ADS_set &other);

    ~ADS_set();

    ADS_set& operator=(const ADS_set &other);
    ADS_set& operator=(std::initializer_list<key_type> ilist);

    size_type size() const;
    bool empty() const;

    //Zusatz
    key_t y()const;

    std::pair<key_t ,key_t> x();
private:
    void empty_overflow_bucket_handler(size_type);
public:
    void insert(std::initializer_list<key_type> ilist);                  // PH1

    std::pair<Iterator,bool> insert(const key_type& key)
    {
        // note that the param is taken "by-reference", but containers store COPIES of values!
        const size_type bucketLineIndex = this->get_bucket_line_index(key);
        // search operation
        for(Bucket* curLineBucket = this->table[bucketLineIndex];curLineBucket != nullptr; curLineBucket = curLineBucket->get_next())
        {
            size_type x = 0;//static counter index_in_bucket
            for(key_type* value_it = curLineBucket->begin(); value_it != curLineBucket->end(); ++value_it,x++)
            {
                // check wether the key already existed
                if(key_equal{}(*value_it,key))
                {
                    //return std::make_pair<typename ADS_set<Key, BucketSize>::iterator, bool>(ADS_set::iterator(value_it, this), false);
                    //return std::make_pair(Iterator(value_it,curLineBucket,bucketLineIndex,this),false);
                    return std::make_pair(Iterator(value_it,curLineBucket,x,bucketLineIndex,table,actualLineCounter),false);
                }
            }

            // before the for terminates, set which the last Bucket in the line is
            /*if(curLineBucket->is_full() == false && !alreadyFound)
            {
                lastLineBucket = curLineBucket;
                alreadyFound = true;
            }else if(curLineBucket->has_next() == false && !alreadyFound){
                lastLineBucket = curLineBucket;
            }*/
        }



        //find first not full
        for(Bucket* curLineBucket = this->table[bucketLineIndex];curLineBucket != nullptr; curLineBucket = curLineBucket->get_next())
        {
            if(!curLineBucket->is_full()){
                key_type* value_adr = curLineBucket->insert(key);
                //lastLineBucket->insert(key);
                ++this->elementCounter;
                return std::make_pair(Iterator(value_adr,curLineBucket,curLineBucket->size-1,bucketLineIndex,table,actualLineCounter),true);
            }else if(curLineBucket->nextBucket == nullptr){
                // append an "overflow-Bucket/-page"
                curLineBucket->nextBucket = new Bucket();
                curLineBucket->nextBucket->insert(key);
                ++this->elementCounter;
                break;
            }
        }



        // this part is for when the key has NOT already been found!
        // IT IS IMPORTANT TO NOTE that the implementation guarantees that there is NEVER
        // a completely empty at the end of any Bucket-line!!!
            // this is now the point where we perform the splitting, according to the "uncontrolled splitting"-method
            this->split();
            return std::make_pair<typename ADS_set<Key, BucketSize>::iterator, bool>(find(key), true);


            //return std::make_pair<typename ADS_set<Key, BucketSize>::iterator, bool>(ADS_set::iterator(value_adr, this), true);
            //return std::make_pair(Iterator(value_adr,newLastBucket,bucketLineIndex,this),true);

        return std::make_pair(Iterator(nullptr, nullptr,0,0, nullptr,0),false); //error if this happens
    }


    //std::pair<iterator,bool> insert(const key_type &key);
    template<typename InputIt> void insert(InputIt first, InputIt last); // PH1

    void clear();
    size_type erase(const key_type &key);

    size_type count(const key_type &key) const;                          // PH1
    iterator find(const key_type &key) const;
private:
    std::pair<Bucket*,size_type> find_help(const key_type &key) const;

    void delete_buckets(Bucket*);
public:
    void swap(ADS_set &other);

    const_iterator begin() const;
    const_iterator end() const;

    void dump(std::ostream& o = std::cerr) const;

    friend bool operator==(const ADS_set &lhs, const ADS_set &rhs){
        if(lhs.elementCounter == rhs.elementCounter){
            for(auto it = lhs.begin(); it != lhs.end();it++){
                if(!rhs.count(*it)){
                    return false;
                }
            }
            return true;
        }else{
            return false;
        }
    };
    friend bool operator!=(const ADS_set &lhs, const ADS_set &rhs){
        return !(lhs == rhs);
    };


    // ------------------------------------------------------------
    // helpers methods:
private:
    Bucket* get_primary_bucket_for_line(const size_type bucketLineIndex) const;
    Bucket* get_last_bucket_for_line(const size_type bucketLineIndex) const;
    void set_primary_bucket_for_line( Bucket* const bucket, const size_type bucketLineIndex);

    size_type get_last_bucket_line_index() const;

    /**
     * The 1st hashing function.
     * @return the index(-number) indicating which Bucket-line
     * within the table is to be considered for either search or insertion operation.
     */
    size_type hash_1(key_type key) const;
    /**
     * The 2nd hashing function.
     * @return the index(-number) indicating which Bucket-line
     * within the table is to be considered for either search or insertion operation.
     */
    size_type hash_2(key_type key) const;

    void split();

    /**
     * Reorderes a SINGLE value given by its value-location.
     * This will be used when remapping values away from a Bucket,
     * as well as when erasing an element within the set.
     * Note that this by itself does NOT decrement the size of the `fromBucket`!
     */
    void reorder_value(key_type* remappedValueLocation, Bucket* fromBucket);

    /**
     * This operation will expand the table of Bucket-pointers by 1,
     * as is intented for the Linear Hashing algorithm (hence the "linear"-part).
     * In other words, it will add another so-called "Bucket line".
     */
    void increase_table_size();

    // helper-methods
    /**
     * Uses either the hash_1() or both hash_1() and hash_2()
     * to determine the Bucket-line to operate upone a given key,
     * depending on the current splitting-round.
     */
    size_type get_bucket_line_index(const key_type& key) const;
};

template <typename Key, size_t BucketSize>
struct ADS_set<Key,BucketSize>::Bucket
{
    // WARNING: STACK-allocated!
    key_type *values;
public:
    const key_type *getValues() const {
        return values;
    }


    size_type size;
public:
    void setSize(const size_type &size) {
        Bucket::size = size;
    }

public:
    const size_type &getSize() const {
        return size;
    }


    Bucket* nextBucket;

public:
    Bucket() : size{0}, nextBucket{nullptr}{
         values = nullptr;
         values = new key_type[BucketSize];
    }
    /**
     * Copy-constructor which deep-copies the values-array from
     * the `other` into this one.
     * WARNING: the `nextBucket` will be set to `nullptr`!!
     * This is because `this` and `other`-Buckets would otherwise SHARE
     * the same `nextBucket` which MUST NOT HAPPEN, as they are supposed to
     * belong to different Bucket-lines/singly-linked-lists!!!
     */
    bool is_empty() const
    {
        return this->size == 0;
    }

    bool is_full() const
    {
        return this->size == BucketSize;
    }

    /**
     * @return `true` if there is another Bucket after this Bucket,
     * `false` otherwise.
     */
    bool has_next() const
    {
        return this->nextBucket != nullptr;
    }

    void set_next(Bucket* nextBucketPtr)
    {
        this->nextBucket = nextBucketPtr;
    }

    Bucket* get_next() const
    {
        return this->nextBucket;
    }

    key_type* insert(const key_type& key)
    {
        /*key_type* value_adr = nullptr;
        this->values[this->size-1];
        value_adr = &this->values[this->size];
        ++this->size;
*/
        this->values[this->size] = key;
        ++this->size;
        return this->values+this->size-1;
    }

    /**
     * NOTE: unchecked operation! Must therefore be only used when checked
     * that is is not full and has yet some space left to be used!!
     */
    void push_back(key_type key)
    {
        this->values[this->size] = key;
        ++this->size;
    }

    void clear()
    {
        this->size = 0;
    }

    /**
     * Erases the last element by simply decrementing the size of the Bucket-values-array
     * and allowing it to be overridden (effectively getting truly deleted at some point).
     *
     * NOTE: unchecked operation!! Make sure the Bucket is not empty before using!
     * @return the last element that was erased.
     */
    key_type erase_last()
    {
        key_type lastValue = this->values[this->size-1];
        --this->size;
        return lastValue;
    }

    key_type* first_value_adr()
    {
        return this->values;
    }

    key_type last_value()
    {
        return this->values[this->size - 1];
    }

    key_type* last_value_adr()
    {
        return &(this->values[this->size - 1]);
    }

    /**
     * Handles the re-ordering of a single gap/hole within the values-array.
     * For example, the following...
     * |34| |14|47|*
     * ...becomes this...
     * |34|47|14| |*
     * NOTE: unchecked operation! It is assumed that the `gap_location`
     * is within the valid address-space of the current Bucket-values-array!!
     */
    void reorder_single_gap(key_type* gap_location)
    {
        *gap_location = last_value();
    }

    /**
     * NOTE: not actually a real iterator-object,
     * but merely a pointer simulating the same behaviour!
     */
    key_type* begin()
    {
        return this->values;
    }

    /*
     * NOTE: not actually a real iterator-object,
     * but merely a pointer simulating the same behaviour!
     *
     * Note however that this does NOT return an address AFTER
     * the end of the `values`-array, but instead right after the
     * LAST ELEMENT of the array, to increase performance by
     * less useless iterations.
     */
    key_type* end()
    {
        // offsetting the pointer to after the last element
        return this->values + this->size;
    }
};
/*
template <typename Key, size_t BucketSize>
std::ostream& operator<<(std::ostream& out,typename ADS_set<Key,BucketSize>::Bucket* bucket)
{
    out << "Bucket-content: {\n";
    out << "Elements: [";
    for(auto value_it = bucket->begin(); value_it != bucket->end(); ++value_it)
    {
        out << *value_it << (bucket->last_value_adr() == value_it ? ']' : ',');
    }
    out << "\n}\n";

    return out;
}
*/
// END OF Bucket definition

/*template <typename Key, size_t BucketSize>
struct ADS_set<Key,BucketSize>::Element
{
    key_type value;
    size_type size;
    Element() : value{value} {}
};*/


template <typename Key, size_t BucketSize>
class ADS_set<Key,BucketSize>::Iterator
{
public:
    using value_type = Key;
    using difference_type = std::ptrdiff_t;
    using reference = const value_type&;
    using pointer = const value_type*;
    using iterator_category = std::forward_iterator_tag;

private:
    //key_type* element;

    /**
     * The Bucket the element comes from.
     * Or in other words, the Bucket that stores the element.
     */
   // Bucket* fromBucket;
    /**
     * This index indicates in which Bucket-line this iterator-instance
     * finds itself to be in currently.
     */
    //ADS_set::size_type curBucketLineIndex;

    /**
     * The ADS_set-instance from which this iterator has been created from.
     */
    //const ADS_set* creator;

    //MINE
private:
    size_type lastIndex; //actualCounter
    Bucket **bucketList; //table
    size_type index; //curBucketLine
    Bucket *bucketIndex; //fromBucket
    size_type index_in_bucket; //hanyadik a bucketben a ptr
    pointer ptr; //element

public:
    Iterator()

    {
        /*this->element= nullptr;
        this->fromBucket = nullptr;
        this->creator = nullptr;
        this->curBucketLineIndex = 0;*/

        //MINE
        this->lastIndex = 0;
        this->bucketList = nullptr;
        this->index = 0;
        this->bucketIndex = nullptr;
        this->ptr = nullptr;
        this->index_in_bucket = 0;
    }
    /*  : element(other.element), fromBucket(other.fromBucket),
    curBucketLineIndex(other.curBucketLineIndex)*/
    Iterator(const Iterator& other)

    {
        /*this->element = other.element;
        this->fromBucket = other.fromBucket;
        this->curBucketLineIndex = other.curBucketLineIndex;
        this->creator = other.creator;*/

        //Mine
        this->lastIndex = other.lastIndex;
        this->bucketList = other.bucketList;
        this->index = other.index;
        this->bucketIndex = other.bucketIndex;
        this->ptr = other.ptr;
        this->index_in_bucket = other.index_in_bucket;
    }

    /*explicit Iterator(key_type* key_adr)
    {
        this->element = key_adr;
        this->fromBucket = nullptr;
        this->creator = nullptr;
        this->curBucketLineIndex = 0;
    }*/

    /*Iterator(key_type* key_adr, Bucket* fromBucket)
    {
        this->element = key_adr;
        this->fromBucket = fromBucket;
        this->creator = nullptr;
        this->curBucketLineIndex = 0;
    }*/

    Iterator(pointer ptr, Bucket* bucketIndex, size_type indexInBucket,size_type index,Bucket** bucketArray,size_type lastIndex)
    {
        /*this->element = key_adr;
        this->fromBucket = fromBucket;
        this-> curBucketLineIndex = curBucketLineIndex;
        this->creator = other;*/



        this->ptr = ptr;
        this->bucketIndex = bucketIndex;
        this->index_in_bucket = indexInBucket;
        this->index = index;
        this->bucketList = bucketArray;
        this->lastIndex = lastIndex;
    }

    reference operator*() const
    {
        return *ptr;
    }

    pointer operator->() const
    {
        return ptr;
    }

    Iterator& operator++()
    {
        // Line 0: |3|5|12|16| |* -> |3|5|12|16|56|* -> |3|5|12|16| |*
        // 										|
        // 										V
        // Line 1: |3|5|12|16| |* -> |3|5|12|16|56|* -> |3|5|12|16| |*
        // Line 2: |3|5|12|16| |* -> |3|5|12|16|56|* -> |3|5|12|16| |*
        // Line 3: |3|5|12|16| |* -> |3|5|12|16|56|* -> |3|5|12|16| |*


        //1. empty bucket atugrik
        //2. if end of the row (jump to the next row)
        //3.if end of the Bucket jump to the next Bucket
        //+1 if end of the whole set return Iterator() nullptr
        //retrun key
        //element curr key ptr
        // fromBucket =curr bucket ptr
        //creator whole set
        //curBucketLineIndex curr sor
        /*bool found_start = false;
        for(;curBucketLineIndex<creator->actualLineCounter;curBucketLineIndex++, fromBucket = creator->table[curBucketLineIndex]){
            for(;fromBucket != nullptr;fromBucket =  fromBucket->get_next()){
                if(!fromBucket->is_empty()){
                    for(key_type* elem = fromBucket->begin();elem != fromBucket->end();elem++){
                        if(found_start){
                            element = elem;
                            return *this;
                        }else if(key_equal{}(*elem,*element)){
                           found_start = true;
                            continue;
                        }
                    }
                }
            }
        }
        element = nullptr;
        return *this;*/

        //MINE
        index_in_bucket++;
        while (true) {
            if (index == lastIndex) {
                ptr = nullptr;
                return *this;
            }
            if (bucketIndex == nullptr) {
                if (index == lastIndex) {
                    ptr = nullptr;
                    return (*this);
                }
                index++;
                bucketIndex = bucketList[index];
                index_in_bucket = 0;
            }
            else if (index_in_bucket < bucketIndex->size) {
                ptr = bucketIndex->values + index_in_bucket;

                return *this;
            }
            else if (index_in_bucket == bucketIndex->size) {
                bucketIndex = bucketIndex->get_next();
                index_in_bucket = 0;
            }
        }
    }

    Iterator operator++(int)
    {
        Iterator orig(*this);
        ++*this; // usese the above prefix-operator++()
        return orig;
    }

    bool operator==(const iterator &a) const {
        if (a.ptr == this->ptr)
            return true;
        return false;
    }

    bool operator!=(const iterator &a) const {
        if (*this == a)
            return false;
        return true;
    }
};

template <typename Key, size_t BucketSize> void swap(ADS_set<Key,BucketSize> &lhs, ADS_set<Key,BucketSize> &rhs) { lhs.swap(rhs); }


// ADS_set DEFINITIONS:
template <typename Key, size_t BucketSize>
ADS_set<Key, BucketSize>::ADS_set()
        : splitCounter{0},  nextLineSplitIndex{0}, lastLineSplitIndex{0}
{
    test = BucketSize;
    table = new Bucket*[1],
    table[0] = new Bucket();
    bucketLineCounter = 1;
    elementCounter = 0;
    actualLineCounter = 1;
}

template <typename Key, size_t BucketSize>
ADS_set<Key, BucketSize>::ADS_set(std::initializer_list<ADS_set::key_type> ilist)
        : ADS_set()
{
    insert(ilist.begin(), ilist.end());
}

template <typename Key, size_t BucketSize>
template <typename InputIt>
ADS_set<Key, BucketSize>::ADS_set(InputIt first, InputIt last)
        : ADS_set()
{
    insert(first, last);
}

template <typename Key, size_t BucketSize>
ADS_set<Key, BucketSize>::ADS_set(const ADS_set& other):ADS_set()
{
    for(size_type bucketLineIndex = 0; bucketLineIndex <other.actualLineCounter; ++bucketLineIndex)
    {
        size_type curLineBucketIndex = 0;
        for(Bucket* curLineBucket = other.table[bucketLineIndex]; curLineBucket != nullptr; curLineBucket = curLineBucket->get_next(), ++curLineBucketIndex)
        {
            for(auto value_it = curLineBucket->begin(); value_it != curLineBucket->end(); ++value_it)
            {
                this->insert(*value_it);

            }



        }


    }
}

template <typename Key, size_t BucketSize>

ADS_set<Key, BucketSize>::~ADS_set()
{
    for(size_type i = 0; i < actualLineCounter; ++i)
    {
        delete_buckets(table[i]);
    }

    delete[] this->table;
}

template <typename Key, size_t BucketSize>
ADS_set<Key, BucketSize>& ADS_set<Key, BucketSize>::operator=(const ADS_set& other)
{
    this->clear();
    for (auto i = other.begin(); i != other.end(); i++) {
        insert(*i);
    }
    return *this;
}

template <typename Key, size_t BucketSize>
ADS_set<Key, BucketSize>& ADS_set<Key, BucketSize>::operator=(std::initializer_list<key_type> ilist)
{
    this->clear();
    for (auto i = ilist.begin(); i != ilist.end(); i++) {
        insert(*i);
    }
    return *this;
}

template <typename Key, size_t BucketSize>
void ADS_set<Key, BucketSize>::clear()
{
    ADS_set<Key,BucketSize> temp;
    this->swap(temp);
}

template <typename Key, size_t BucketSize>
typename ADS_set<Key, BucketSize>::size_type ADS_set<Key, BucketSize>::erase(const key_type& key)
{
    //1,2,3,4,5  size = 5-> 1,2,4,5,"5" size = 4-> 1,2,4,5,6 size = 5
    //hash key -> < currentSplit hash2 :
    //index for loop -> bucket tolas size--
    // delete empty Bucket
    std::pair<Bucket*,size_type> pair = this->find_help(key);
    if(pair.first == nullptr) //not in the bucket
    {

        last_incorrect_erase = key;
        last_incorrect_erase_ptr = &last_incorrect_erase;
        return 0;
    }
    else
    {
        /*  auto key_bucket = key_it.get_from_bucket();
          reorder_value(*key_it, key_bucket);
          key_bucket->erase_last();*/

        //loop in bucket from key till end tolas
        //for -> index (size_type) break
        size_type start_from = 0;

        for(key_type* start = pair.first->begin(); start<pair.first->end();start++){
            if(key_equal{}(*start, key)){
                break;
            }
            start_from++;
        }

        if(start_from >= pair.first->size){
            return -2; //TODO
        }
        //for-> index->end i = i+1
        //1,2
        for(size_type i = start_from;i<pair.first->size-1;++i){
            pair.first->values[i] = pair.first->values[i+1];
        }
        pair.first->size--;

        //Delete empty buckets
        empty_overflow_bucket_handler(pair.second);
        elementCounter--;
    }
    return 1;
}

template <typename Key, size_t BucketSize>
typename ADS_set<Key, BucketSize>::size_type ADS_set<Key, BucketSize>::count(const key_type &key) const
{
    size_type bucketLineIndex = this->get_bucket_line_index(key);
    for(Bucket* curLineBucket = this->table[bucketLineIndex]; curLineBucket!= nullptr; curLineBucket = curLineBucket->get_next())
    {
        for(key_type* value_it = curLineBucket->begin(); value_it != curLineBucket->end(); ++value_it)
        {
            // check wether the key already existed
            if(key_equal{}( *value_it,key))
            {
                return 1;
            }
        }
    }

    return 0;
}

template <typename Key, size_t BucketSize>
void ADS_set<Key, BucketSize>::swap(ADS_set& other)
{
    //std::swap(this->elementCounter, other.elementCounter);
    /*std::swap(this->splitCounter, other.splitCounter);
    std::swap(this->bucketLineCounter, other.bucketLineCounter);
    std::swap(this->nextLineSplitIndex, other.nextLineSplitIndex);
    std::swap(this->lastLineSplitIndex, other.lastLineSplitIndex);
    std::swap(this->lastLineSplitIndex, other.lastLineSplitIndex);
    std::swap(this->actualLineCounter,other.actualLineCounter);*/
    // swaps ownership of the respective tables!!
    //std::swap(this->table, other.table);
    using std::swap;
    swap(this->elementCounter,other.elementCounter);
    swap(this->splitCounter,other.splitCounter);
    swap(this->bucketLineCounter,other.bucketLineCounter);
    swap(this->nextLineSplitIndex,other.nextLineSplitIndex);
    swap(this->lastLineSplitIndex,other.lastLineSplitIndex);
    swap(this->actualLineCounter,other.actualLineCounter);
    swap(this->table, other.table);



}

template <typename Key, size_t BucketSize>
typename ADS_set<Key, BucketSize>::const_iterator ADS_set<Key, BucketSize>::begin() const
{
    size_type bucketLineIndex = 0;
    for(;bucketLineIndex<this->actualLineCounter    ;bucketLineIndex++){
        for(Bucket* curLineBucket = this->table[bucketLineIndex];curLineBucket != nullptr; curLineBucket = curLineBucket->get_next()) {
            size_type x = 0;
            for (key_type *value_it = curLineBucket->begin(); value_it != curLineBucket->end(); ++value_it,x++) {
                //return Iterator(value_it,curLineBucket,bucketLineIndex,this);
                return Iterator(value_it,curLineBucket,x,bucketLineIndex,table,actualLineCounter);

            }
        }
    }

    return end();
}
template <typename Key, size_t BucketSize>
typename ADS_set<Key, BucketSize>::const_iterator ADS_set<Key, BucketSize>::end() const
{
    return Iterator();
}

template <typename Key, size_t BucketSize>
typename ADS_set<Key, BucketSize>::size_type ADS_set<Key, BucketSize>::size() const
{
    return this->elementCounter;
}

template <typename Key, size_t BucketSize>
typename ADS_set<Key, BucketSize>::iterator ADS_set<Key, BucketSize>::find(const ADS_set::key_type& key) const
{
    size_type bucketLineIndex = this->get_bucket_line_index(key);

    // Bucket-line(in a singly-linked-list fashion) (e.g. index=3) => |33|103|11|44|* - |34|123|13|47|* - |77| | | | | | |*Ã‚Â´

    // this->table:
    // NOTE: behind each table-index is 1 specific primary-/line-beginning - Bucket-pointer!
    // <0> => |33|103|11|44|* - |34|123|13|47|* - |77| | | | | | |*
    // <1> => |33|103|11|44|* - |34|123|13|47|* - |77| | | | | | |*
    // <2> => |33|103|11|44|* - |34|123|13|47|* - |77| | | | | | |*
    // <3> => |33|103|11|44|* - |34|123|13|47|* - |77| | | | | | |*

    // `curLineBucket` is the current Bucket within the Bucket-line
    size_type curBucketLineIndex = bucketLineIndex;

    for(Bucket* curLineBucket = this->table[bucketLineIndex]; curLineBucket != nullptr; curLineBucket = curLineBucket->get_next())
    {
        size_type x = 0;
        for(key_type* value_it = curLineBucket->begin(); value_it != curLineBucket->end(); ++value_it,x++)
        {
            if(key_equal{}(*value_it,key))
            {
                // NOTE: does NOT provide the potentially necessary info from where(`this`) this has been created from!!
                return ADS_set<Key, BucketSize>::iterator(value_it, curLineBucket,x, curBucketLineIndex,table,actualLineCounter);
            }
        }
    }
    return this->end();
}
template <typename Key, size_t BucketSize>
std::pair<typename ADS_set<Key, BucketSize>::Bucket*,typename ADS_set<Key,BucketSize>::size_type> ADS_set<Key, BucketSize>::find_help(const ADS_set::key_type& key) const
{
    size_type bucketLineIndex = this->get_bucket_line_index(key);

    // Bucket-line(in a singly-linked-list fashion) (e.g. index=3) => |33|103|11|44|* - |34|123|13|47|* - |77| | | | | | |*Ã‚Â´

    // this->table:
    // NOTE: behind each table-index is 1 specific primary-/line-beginning - Bucket-pointer!
    // <0> => |33|103|11|44|* - |34|123|13|47|* - |77| | | | | | |*
    // <1> => |33|103|11|44|* - |34|123|13|47|* - |77| | | | | | |*
    // <2> => |33|103|11|44|* - |34|123|13|47|* - |77| | | | | | |*
    // <3> => |33|103|11|44|* - |34|123|13|47|* - |77| | | | | | |*

    // `curLineBucket` is the current Bucket within the Bucket-line
    //size_type curBucketLineIndex = bucketLineIndex;

    for(Bucket* curLineBucket = this->table[bucketLineIndex]; curLineBucket != nullptr; curLineBucket = curLineBucket->get_next())
    {
        for(key_type* value_it = curLineBucket->begin(); value_it != curLineBucket->end(); ++value_it)
        {
            if(key_equal{}(*value_it,key))
            {
                // NOTE: does NOT provide the potentially necessary info from where(`this`) this has been created from!!
                return std::make_pair(curLineBucket,bucketLineIndex);
            }
        }
    }
    return std::make_pair(nullptr,-2);
}


template <typename Key, size_t BucketSize>
bool ADS_set<Key, BucketSize>::empty() const
{
    return this->elementCounter == 0;
}

template <typename Key, size_t BucketSize>
typename ADS_set<Key, BucketSize>::Bucket* ADS_set<Key, BucketSize>::get_primary_bucket_for_line(const size_type bucketLineIndex) const
{
    return this->table[bucketLineIndex];
}

template <typename Key, size_t BucketSize>
typename ADS_set<Key, BucketSize>::Bucket* ADS_set<Key, BucketSize>::get_last_bucket_for_line(const size_type bucketLineIndex) const
{
    for(Bucket* bucket = this->get_primary_bucket_for_line(bucketLineIndex); bucket->has_next(); bucket = bucket->get_next())
    {
        if(bucket->has_next() == false)
        {
            return bucket;
        }
    }
}

template <typename Key, size_t BucketSize>
void ADS_set<Key, BucketSize>::set_primary_bucket_for_line( Bucket* bucket, const size_type bucketLineIndex)
{
    this->table[bucketLineIndex] = bucket;
}

template <typename Key, size_t BucketSize>
typename ADS_set<Key, BucketSize>::size_type ADS_set<Key, BucketSize>::get_last_bucket_line_index() const
{
    return this->bucketLineCounter - 1;
}

template <typename Key, size_t BucketSize>
typename ADS_set<Key, BucketSize>::size_type ADS_set<Key, BucketSize>::hash_1(typename ADS_set<Key, BucketSize>::key_type key) const
{

    return hasher{}(key) % (1<<this->splitCounter);
}

template <typename Key, size_t BucketSize>
typename ADS_set<Key, BucketSize>::size_type ADS_set<Key, BucketSize>::hash_2(typename ADS_set<Key, BucketSize>::key_type key) const
{
    return hasher{}(key) % (1<<(this->splitCounter+1));
}
/*
template <typename Key, size_t BucketSize>
std::pair<typename ADS_set<Key, BucketSize>::iterator, bool> ADS_set<Key, BucketSize>::insert(const ADS_set::key_type& key)
{
    // note that the param is taken "by-reference", but containers store COPIES of values!
    const size_type bucketLineIndex = this->get_bucket_line_index(key);
    Bucket* lastLineBucket = nullptr;

    // search operation
    for(Bucket* curLineBucket = this->table[bucketLineIndex]; curLineBucket->has_next(); curLineBucket = curLineBucket->get_next())
    {
        for(key_type* value_it = curLineBucket->begin(); value_it != curLineBucket->end(); ++value_it)
        {
            // check wether the key already existed
            if(*value_it == key)
            {
                return std::make_pair<typename ADS_set<Key, BucketSize>::iterator, bool>(ADS_set::iterator(value_it, this), false);
            }
        }

        // before the for terminates, set which the last Bucket in the line is
        if(curLineBucket->has_next() == false)
        {
            lastLineBucket = curLineBucket;
        }
    }
    // FIXME: remove when testing is done and this has proven to be as expected!!
    assert(lastLineBucket->is_empty() == false);

    // this part is for when the key has NOT already been found!
    // IT IS IMPORTANT TO NOTE that the implementation guarantees that there is NEVER
    // a completely empty at the end of any Bucket-line!!!
    if(lastLineBucket->is_full())
    {
        // append an "overflow-Bucket/-page"
        Bucket* newLastBucket = new Bucket();
        lastLineBucket->set_next(newLastBucket);
        key_type* value_adr = newLastBucket->insert(key);
        ++this->elementCounter;

        // this is now the point where we perform the splitting, according to the "uncontrolled splitting"-method
        this->split();

        return std::make_pair<typename ADS_set<Key, BucketSize>::iterator, bool>(ADS_set::iterator(value_adr, this), true);
    }
    else // has space for at least 1 more element
    {
        key_type* value_adr = lastLineBucket->insert(key);
        ++this->elementCounter;
        return std::make_pair<typename ADS_set<Key, BucketSize>::iterator, bool>(ADS_set::iterator(value_adr, this), true);
    }
}*/

template <typename Key, size_t BucketSize>
template <typename InputIt>
void ADS_set<Key, BucketSize>::insert(InputIt first, InputIt last)
{
    for(; first != last; ++first)
    {
        // uses the `insert(const ADS_set::key_type& key)`
        this->insert(*first);
    }
}

template <typename Key, size_t BucketSize>
void ADS_set<Key, BucketSize>::insert(std::initializer_list<ADS_set::key_type> ilist)
{
    // uses `insert(InputIt first, InputIt last)`
    this->insert(ilist.begin(), ilist.end());
}

template <typename Key, size_t BucketSize>
void ADS_set<Key, BucketSize>::split()
{
    increase_table_size();

    // the Bucket-line that is now meant to be splitted,
    // indicated by `this->nextLineSplitIndex`
    for(Bucket* curSplittingLineBucket = this->table[this->nextLineSplitIndex]; curSplittingLineBucket!= nullptr; curSplittingLineBucket = curSplittingLineBucket->get_next())
    {
        for(key_type* value_it = curSplittingLineBucket->begin(); value_it != curSplittingLineBucket->end();) //TODO MODIFY
        {
            size_type value_hash = hash_2(*value_it);
            // check the values that are meant to be re-mapped away from this Bucket-line
            if(value_hash != this->nextLineSplitIndex)
            {
                // RE-MAPPING procedure here:
                for(Bucket* curOtherLineBucket = this->table[value_hash]; curOtherLineBucket != nullptr; curOtherLineBucket = curOtherLineBucket->get_next())
                {
                    if(curOtherLineBucket->is_full() == false)
                    {
                        curOtherLineBucket->insert(*value_it);
                        size_type temp = 0;
                        for(key_type * iter = curSplittingLineBucket->begin(); iter!=curSplittingLineBucket->end();iter++){
                            if(key_equal{}(*iter,*value_it)){
                                break;
                            }
                            temp++;
                        }
                        //num based loop
                        for(;temp<curSplittingLineBucket->getSize()-1;temp++){
                            curSplittingLineBucket->values[temp] = curSplittingLineBucket->values[temp+1];
                        }
                        curSplittingLineBucket->setSize(curSplittingLineBucket->getSize()-1);

                        // reorder_value(value_it, curSplittingLineBucket);
                        //tolas curSplittingLineBucket value it -> for ->end() i = i+1
                        break;
                    }

                    // insert value into another new Bucket when the last Bucket from the "other" Bucket-line was found and is full
                    if(curOtherLineBucket->has_next() == false && curOtherLineBucket->is_full())
                    {
                        //reorder_value(value_it, curSplittingLineBucket);
                        curOtherLineBucket->nextBucket = new Bucket();
                        curOtherLineBucket->nextBucket->insert(*value_it);


                        size_type temp = 0;
                        for(key_type * iter = curSplittingLineBucket->begin(); iter!=curSplittingLineBucket->end();iter++){
                            if(key_equal{}(*iter,*value_it)){
                                break;
                            }
                            temp++;
                        }
                        //num based loop
                        for(;temp<curSplittingLineBucket->getSize()-1;temp++){
                            *(curSplittingLineBucket->values+temp) = *(curSplittingLineBucket->values+temp+1);

                        }
                        curSplittingLineBucket->setSize(curSplittingLineBucket->getSize()-1);

                        break;
                    }
                }
            }else{
                ++value_it;
            }
        }
    }

    //delete all empty buckets max = 1
    empty_overflow_bucket_handler(this->nextLineSplitIndex);
    // when the current splitting-round is done
    if(this->nextLineSplitIndex == this->lastLineSplitIndex)
    {
        this->nextLineSplitIndex = 0;
        ++this->splitCounter;
        this->lastLineSplitIndex = static_cast<size_type>((1<<this->splitCounter) - 1);
    }
    else // the current splitting-round is NOT OVER
    {
        ++this->nextLineSplitIndex;
    }
}



template <typename Key, size_t BucketSize>
void ADS_set<Key, BucketSize>::increase_table_size()
{
    if(actualLineCounter  == bucketLineCounter){
        Bucket** newTable = new Bucket*[actualLineCounter*2];
        for(size_t i = 0; i < actualLineCounter; ++i)
        {
            newTable[i] = this->table[i];
        }



        delete[] this->table;
        this->table = newTable;
        bucketLineCounter = actualLineCounter*2;
        table[actualLineCounter] = new Bucket();
        actualLineCounter++;
        // to guarantee that there is ALWAYS at least the primary-Bucket in the line
    }else{
        table[actualLineCounter] = new Bucket();
        actualLineCounter++;
    }


}

template <typename Key, size_t BucketSize>
void ADS_set<Key, BucketSize>::dump(std::ostream &o) const
{
    o << "ADS_set:\n";
    o << "number of elements in the set: " << this->elementCounter << '\n';
    o << "table-size/number of Bucket-lines: " << this->bucketLineCounter << '\n';
    o << "lastLineSplitIndex of this set: " << this->lastLineSplitIndex << '\n';
    o << "nextLineSplitIndex of this set: " << this->nextLineSplitIndex << '\n';
    o << "Current splitting-round: " << this->splitCounter << '\n';
    o << "Bucketsize: " << BucketSize << '\n';


    o << "All Buckets:\n";
    // iterate over all Bucket-lines:
    for(size_type bucketLineIndex = 0; bucketLineIndex <actualLineCounter; ++bucketLineIndex)
    {
        o<<"Index "<<bucketLineIndex<<" ";
        size_type curLineBucketIndex = 0;
        bool first  = true;
        for(Bucket* curLineBucket = this->table[bucketLineIndex]; curLineBucket != nullptr; curLineBucket = curLineBucket->get_next())
        {
            if(!first){
                o<<" --> ";
            }
            first = false;
            for(auto value_it = curLineBucket->begin(); value_it != curLineBucket->end(); ++value_it)
            {

                if(value_it == curLineBucket->begin()){
                    o << "[";
                }
                o << *value_it<<(curLineBucket->last_value_adr() == value_it ? ']' : ',');
            }

            ++curLineBucketIndex;
        }
        o << "\n";


    }

    /*
    o << "All elements:\n";
    for(key_type element : this)
    {
        o << element;
    }
    */
}


// private helper-method definitions:

template <typename Key, size_t BucketSize>
typename ADS_set<Key, BucketSize>::size_type ADS_set<Key, BucketSize>::get_bucket_line_index(const key_type& key) const
{
    const size_type bucketLineIndex = hash_1(key);
    return (bucketLineIndex < this->nextLineSplitIndex ? hash_2(key) : bucketLineIndex);
}



template<typename Key, size_t BucketSize>
void ADS_set<Key, BucketSize>::empty_overflow_bucket_handler(size_type index) {
    bool foundEmpty = false;
    for(Bucket* curSplittingLineBucket = this->table[index]; curSplittingLineBucket!= nullptr; ) {
        if(curSplittingLineBucket->nextBucket != nullptr){
            if(curSplittingLineBucket->is_empty() == true && !foundEmpty){
                foundEmpty = true;
            }
            if(curSplittingLineBucket->nextBucket->has_next() && curSplittingLineBucket->nextBucket->is_empty() && foundEmpty){
                auto nextPtr = curSplittingLineBucket->nextBucket->nextBucket;
                delete[] curSplittingLineBucket->nextBucket->values;
                delete curSplittingLineBucket->nextBucket;
                curSplittingLineBucket->nextBucket = nextPtr;
            }else if(curSplittingLineBucket->nextBucket->is_empty() && !curSplittingLineBucket->nextBucket->has_next() && foundEmpty){
                delete[] curSplittingLineBucket->nextBucket->values;
                delete curSplittingLineBucket->nextBucket;
                curSplittingLineBucket->nextBucket = nullptr;
                break;
            }
            else{
                curSplittingLineBucket = curSplittingLineBucket->get_next();
            }
        }else{
            curSplittingLineBucket = curSplittingLineBucket->get_next();
        }
    }
}

template<typename Key, size_t BucketSize>
void ADS_set<Key, BucketSize>::delete_buckets(ADS_set::Bucket * b) {
    if (b == nullptr) {
        return;
    }
    delete_buckets(b->nextBucket);
    delete[] b->values;
    delete b;

}
template<typename Key, size_t BucketSize>
key_t ADS_set<Key, BucketSize>::y()const{
    key_t actual_min; // aktualis valtozo
    key_t  min; // egyel nagyobb mint erase var

    if(last_incorrect_erase_ptr == nullptr){
        throw std::runtime_error("Invalid variable");
    }
    bool foundOne = false;
    for(auto it = this->begin(); it!= this->end(); it++){
        if(std::less<Key>{}((*last_incorrect_erase_ptr),*it)){
            min = *it;
            foundOne = true;
        }
    }
    if(!foundOne){
        throw std::runtime_error("invalid var - no greater");
    }

    for(auto it = this->begin(); it!= this->end(); it++){
        actual_min = *it;
        if(std::less<Key>{}(actual_min,min) && std::less<Key>{}((*last_incorrect_erase_ptr),actual_min)){
            min = actual_min;
        }
    }
    return min;
}

template<typename Key, size_t BucketSize>
std::pair<key_t , key_t> ADS_set<Key, BucketSize>::x() {
    int length = 0;
    int max_length = 0;
    key_t first;
    key_t last;
    key_t actual_first;
    key_t actual_last;
    key_t prev;

    if(this->elementCounter == 0){
        throw std::runtime_error("No data");
    }else if(this->elementCounter == 1){
        return std::make_pair((*this->begin()),(*this->begin()));
    }

    for(auto it = this->begin(); it != this->end(); it++){
        //0 1 2 3 4 5 6 7 8 9
        if(it == this->begin()){
            std::cout<<"New actualfirts: "<<*it<<std::endl;
            actual_first == *it;
            prev = actual_first;
            std::cout<<"New prev: "<<actual_first<<std::endl;
            //length++;
        }else if(std::less<Key>{}(prev,*it)){
            //std::cout<<"Less: "<<*it<<std::endl;
            length++;
            prev = *it;
            std::cout<<"New prev: "<<prev<<std::endl;
        }else{

            actual_last = prev;
            std::cout<<"New actual_last: "<<actual_last<<std::endl;
            std::cout<<"length: "<<length<<std::endl;
            if(max_length < length){
                max_length = length;
                first = actual_first;
                last = actual_last;
            }
            length = 0;
            actual_first = *it;
            std::cout<<"New actualfirts: "<<*it<<std::endl;
            prev = actual_first;
            std::cout<<"New prev: "<<actual_first<<std::endl;
            std::cout<<" first: "<<first<<std::endl;
            std::cout<<" last: "<<last<<std::endl;

        }
    }

    actual_last = prev;
    if(max_length < length){
        max_length = length;
        length = 0;
        first = actual_first;
        last = actual_last;
    }
    return std::make_pair(first,last);
}

// global operators

template <typename Key, size_t BucketSize>
bool operator==(const ADS_set<Key, BucketSize>& lhs, const ADS_set<Key, BucketSize>& rhs);

template <typename Key, size_t BucketSize>
bool operator!=(const ADS_set<Key, BucketSize>& lhs, const ADS_set<Key, BucketSize>& rhs);


#endif // ADS_SET_H

# Hash Hash Hash

Using mutexes to ensure thread-safe hash table entry adding and better performance through proper locking for multithreading. 

## Building

To build the program, run 'make' in the lab3 directory. This will create an executable called hash-table-tester. 

## Running

To run the program, use the executable 'hash-table-tester' with 2 flags and 2 arguments. 
An example execution would be: 
./hash-table-tester -t 8 -s 80000
The flag `-t` tells the program how many threads to use and the flag `-s` tells the program how many hash table entries each thread needs to add. 
When we run the program, we obtain three results: the base case, v1 locking, and v2 locking. The base case will run with no threading and in the above case will complete within 1-2 seconds (given that the virtual machine has 8 cores). V1 locking uses only 1 lock and will run with the specified amount of threads. V2 locking uses multiple locks and will run with the specified amount of threads. 

## First Implementation

For the implementation of hash-table-v1, I created a single mutex globally with the static keyword so that all threads created will use the same lock. In hash_table_v1_create() I initialized the mutex and in hash_table_v1_destroy() I destroyed the lock so there are no memory leaks. 

My implementation strategy for the placement of the locking and unlocking of the mutex in hash_table_v1_add_entry ensures that the program is thread-safe but guarantees no performance benefits. The critical section I decided on consists of everything from the call to get_list_entry() to the end of the function with SLIST_INSERT_HEAD(). This entire section is required to be locked when we are multithreading because there are race conditions that can occur. The code can be broken down into four parts: searching a hash table entry's list for a key; replacing the value if hash key was found; creating a new list node if not found; inserting the list node into the front of the list. All of which cannot have two threads doing at the same time since there could be a race conidtion. 

The race condition occurs when two threads access the same hash table entry since they will both want to modify it. For example, if both threads have created new list nodes for that entry, they may both try to insert it in and since SLIST_INSERT_HEAD() is not atomic, this could cause both the nodes to try to insert into the same spot and so the head only registers one of them. Thus, one of the nodes will end up being missing. We must also include get_list_entry() because when one thread is searching for a key at the same time another thread is creating a list node for the same key, it can end up determining there are no nodes with the same key. So then there would end up being two nodes in the list with the same key. The only part that would not have a race condition would be replacing the value of an existing key because even if two or more threads are trying to replace the same key's value, we will always end up with one of the updated values. However, since it is such a small portion of code, unlocking and the relocking after this portion would only create more work so we can just leave it in the critical section. 

By implementing the mutexes in this way, our solution will be correct since it eliminates all the possible race conditions with one lock for the entire hash table. 

### Performance

Tester with low number of threads: 
./hash-table-tester -t 2 -s 300000
Generation: 48,265 usec
Hash table base: 1,630,452 usec
    -0 missing
Hash table v1: 2,151,203 usec
    -0 missing

Tester with high number of threads: 
./hash-table-tester -t 8 -s 75000
Generation: 45,916 usec
Hash table base: 1,633,091 usec
    -0 missing
Hash table v1: 3,430,064 usec
    -0 missing

The relative slown down with a low number of threads is 1.3 meaning it is about 1.3 times slower than the base program. The relative slow down with a high number of threads is 2.1 meaning it is about 2.1 times slower than the base program. 
First of all, the reason there is a slow down when we involve multiple threads is because we only used one lock. Having one lock for the entire hash table and since the critical section makes up most of the hash_table_v1_add_entry() function, we are essentially doing no work in parallel. While one thread is trying to add an entry to the hash table, it locks it and so the other threads must wait for that thread to unlock before they can try to add into the hash table. So, this is the equivalent of the base case except now we have to do even more work by locking and unlocking the mutexes and checking for errors. However, because of the lock, the program is thread-safe as the critical section is locked. 

The difference in slow downs experience with low number of threads and high number of threads can be explained with the same reasoning. Since adding entries will be done sequentially even with multiple threads (because of the locking explained above), having a low number of threads or high number will not change this. However, with more threads, we will have to perform more work because there will be more threads waiting for the lock and we will have to decide which thread gets the lock. Furthermore, with more threads, it means possibly more context switching if there are not enough cores to support all the threads to run simultaneously. And since context switching requires extra work, there is a bigger slow down as we increase the number of threads. 

Note: all functions relating to the mutexes will be checked for exit status and will exit with error code if there was an error. 

## Second Implementation

Argue why your strategy is correct.

This second implementation is almost the same as the first implementation except now we have an array of mutexes that each correspond to a hash table entry. We declare the array of uninitialized mutexes globally with the static keyword and with the size HASH_TABLE_CAPACITY so that each hash table entry has its own mutex. One additional feature I added was that the struct hash_table_entry now has an additional variable called 'index' which will range from 0 to HASH_TABLE_CAPACITY-1 so that each mutex is uniquely mapped to an entry. This eliminates the need of converting the key every time. We initialize the mutexes and indexes in hash_table_v2_create() same as before but now we must do it for every iteration. Similarly, we destroy the mutexes in hash_table_v2_destroy() but now we have to iterate through every mutex and destroy them individually. 

As for the placement of the mutexes, they are placed at the exact same spots as we did in the first implementation but now in the function hash_table_v2_add_entry(). The same race conditions apply so we must have the same locations of locking and unlocking. The only difference now is that the mutex used in each call depend on the hash table entry the key depends on. So after getting a pointer to the hash table entry we want to add to, we grab the lock for that entry. This is thread-safe because we lock the same critical section as we did earlier which eliminates the race condition. Since the race conditions only occurred if 2 or more threads want to modify the same hash table entry, we can get away with only locking the hash table entry instead of the entire hash table like in the first implementation. For example, if thread 1 wants to modify entry 3 and thread 2 wants to modify entry 5, there would be no race condition here as they are accessing entirely different linked lists. There could be an issue if thread 1 and 2 are both trying to modify entry 3. Thus, locking by hash table entry is a correct implementation. 

Note: all functions relating to the mutexes will be checked for exit status and will exit with error code if there was an error. 

### Performance

Explain the difference between this
implementation and your first, which respect why you get a performance increase.

Tester with number of threads equal to number of cores: 
./hash-table-tester -t 8 -s 75000
Generation: 45,916 usec
Hash table base: 1,633,091 usec
    -0 missing
Hash table v2: 236,122 usec
    -0 missing

The relative speed up with 8 threads (my virtual machine has 8 cores) is 6.9 meaning it is about 6.9 times faster than the base hash table implementation. The ideal speed up is to have a 1 to 1 increase with every thread we have so in this case it would be a speed up of 8. This occurs when we reach true parallelism where all threads perform their own tasks at the same time. Although our speed up does not reach the ideal case, it makes sense because there are limitations in the implementation of the locks that prohibit us from reaching it. For example, when 2 or more threads want to access the same hash table entry, only one will be able to access it while the others must wait for that thread to unlock. Since we lock by hash table entry, collisions in the hash table would cause work to not be done in parallel for those threads since only one can be working on the entry at a time. However, since there are  many entries, collisions are not very common but will still occur which is why we do not obtain the ideal speed up. 

Now, comparing with the first implementation, we get a massive speed up. The reason for the performance increase is because the first implementation is created with one mutex for the entire hash table while the second implementtion is created with one mutex per hash table entry. The first implementation does not take advantage of threading since minimal work can be done in parallel as the entire hash table becomes locked when a thread wants to access it. So in fact having more threads only increase our workload since we spend more work locking and unlocking. On the other hand, having one mutex per hash table entry allows the threads to work in parallel for the most part. Since each thread will be adding to a hash table entry at a time, it will only lock the entry it is working on and all other entries are still accessible. Thus, as long as the threads are working on different hash table entries, they can work in parallel and so has a much better performance. Even with collisions the overall performance is much better since most of the work is still done in parallel. So, having mutexes per hash table entry allows the threads to work at the same time which is not a feature with having one mutex for the entire hash table. 

## Cleaning up

To clean up all files, just run 'make clean' which will take care of everything for you. 
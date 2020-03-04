Implement "SFSVarchar" functions.

Haven't test if it works...

Might be multiple bugs...

03/02/18:41

---

Fix a typo in "sfsVarcharCreate" function.

Add some explaining comment.

Update "SFSTable" functions.

03/02/19:47

---

Update all module.

Implement some of the functions..

left a bunch of unfilled holes...

03/02/21:28

----

"process_meta.h"

03/02/22:48

---

Implement "SFSTable" functions.

There ~~must be~~ countless bug...

Planed to fix it soon.

03/03/11:10

---

Learn the use of flexible array at the end of a struct.

Make the "sfs.c" file compiles.

Todo: 

1. Rest of the Implementation works
2. Debug

03/03/12:31

---

Implement the two "Adding" functions.

Bug Found: Haven't maintain the "lastVarchar" pointer in the "sfsTableReserve" function.

03/03/14:15  

---

Fix "lastVarchar" Bug

03/03/14:35

---

Review some code

Fix Compile Error

03/03/15:14

---

Review all "sfsVarchar" "sfsTable" code

03/03/16:30

---

Fix many bugs.

Complete the static error checking before database function.

03/03/16:58

---

Update "sfsDatabaseCreate"

Implement "sfsDatabaseAddTable"

03/03/17:16

---

Update "sfsDatabaseSave", saving Database entities to the given file.

Change pointer arithmetic to explicit.

03/03/20:04

---

Update two "loading" functions.

Fix a memory reference bug.

03/03/20:40

---

Update "sfsDatabaseLoad", Loading data from a given file.

It's really nasty, I'm sure there must be countless bugs...



It's planed to run Unit testing tomorrow.

03/03/22:39

---

Debug and test 5 custom functions.

03/04/14:57
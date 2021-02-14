# myls

# DESCRIPTION:
## Goal: Stimulates ls function in UNIX

ls: lists all directors and files in the current directory

## Optional Paramaters:

i (inode format):
Lists the inode of each file

-l (Long list format): 
Lists additional information of each file/directory, including:
  -file/directory permissions
  -number of links
  -file/directory owner
  -file/directory owner group
  -file/directory size
  -last modified date
  -file/directory name
  
-R (recursive format):
Recursively lists every directory tree

# HOW TO USE:
# 1. Compile with makefile in terminal
```bash
make
```

# 2. Run for basic ls stimulation
```bash
./myls 
```

# 3. Add additional parameters, can be combined in one call as shown below:
```bash
./myls -ilR
```

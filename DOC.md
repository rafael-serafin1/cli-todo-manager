# Todo CLI Manager

## How to use:

```bash
todo help           # shows a message with all commands avaliable
```

- To initialize a repo: 
```bash
todo init           # initialize repository for Todofile configurations
todo config <CONFIGS>   # define configurations for Todofile
```

- To add a task:
```bash
todo add "<TASK>"     # adds a task to Todofile, aways use " "
```

- To remove a task (not recomended): 
```bash
todo remove "<INDEX>"     # removes a task by index, aways use " "
```

- To list all tasks:
```bash
todo list <FLAGS>       # list 20 first tasks
```

## Important flags:

**'config' command flags =**
```bash
todo config -r -v -c  
```

or 

```bash
todo config --readable --visible --checkable    
```

### Resume

`--readable` => 
```
    false: record tasks in binary

    true: record tasks in normal text
```

`--visible` => 
```
    false: Todofile will be stored inside repository `.todo/` 

    true: Todofile will be stored in projects root
```

`--checkable` =>
```
    false: Tasks are recorded normalized

    true: Tasks are recorded with a checkbox for ```todo check``` work
```

### Default values
```
-r ==> false
-c ==> false
-v ==> true
```

**'list' command flags =**
```bash
todo list -a -c -un
```

or

```bash
todo list --all --checked --unchecked
```

### Resume
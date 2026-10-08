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
todo list --all         # list every task (-a is an alias)
```

- The task total is stored in `.todo/config/count.bin` and updated by `add` and
  `remove`. `list`, `switch`, and `count` use this total.

- To check or uncheck a task:
```bash
todo switch "<INDEX>"    # aways inside " "
todo switch "<INDEX>"    # same here
```

## Important flags:

**'config' command flags =**
```bash
todo config -r -v -c  
```

or 

```bash
todo config --readable --checkable    
```

### Resume

`--readable` => 
```
    false: record tasks in binary

    true: record tasks in normal text
```

`--checkable` =>
```
    false: Tasks are recorded normalized

    true: Tasks are recorded with a checkbox for ```todo check``` work
```

### Default values
```
-r ==> true
-c ==> true
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
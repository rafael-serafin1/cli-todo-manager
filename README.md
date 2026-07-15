<div align="center">

# ✅ Todo CLI Manager

*A fast, lightweight and configurable task manager for the terminal, written entirely in C.*

![Language](https://img.shields.io/badge/language-C-blue.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows-green.svg)
![License](https://img.shields.io/badge/license-MIT-orange.svg)

</div>

---

# Overview

Todo CLI Manager is a command-line task manager written from scratch in C.

Instead of opening graphical applications or browser tabs, you can manage your tasks directly from the terminal through a single executable:

```bash
todo
```

The project focuses on:

- ⚡ Performance
- 🪶 Lightweight storage
- ⚙️ Configurability
- 📁 Per-project task management
- 💻 Native command-line workflow

Each project can maintain its own task repository, making it ideal for software development, study, personal organization and automation.

---

# Features

- Add tasks
- Remove tasks
- List tasks
- Check and uncheck tasks
- Binary or plain-text storage
- Hidden repository support
- Project-local task management
- Configurable behavior

---

# Installation

Clone the repository:

```bash
git clone https://github.com/your-user/todo-cli.git
```

Compile:

```bash
gcc src/*.c -o todo
```

or using your preferred build system.

After compiling, place the executable somewhere inside your PATH.

Example:

Linux

```bash
sudo mv todo /usr/local/bin/
```

Windows

Add the executable directory to the `PATH` environment variable.

---

# Getting Started

Initialize a Todo repository inside your project:

```bash
todo init
```

This creates the required repository and configuration files.

After that you can configure how Todo behaves.

---

# Commands

## Show help

```bash
todo help
```

Displays every available command and flag.

---

## Configure Todo

```bash
todo config [OPTIONS]
```

Short flags

```bash
todo config -r -v -c
```

Long flags

```bash
todo config --readable --visible --checkable
```

### Available options

| Option | Description |
|---------|-------------|
| `--readable` | Stores tasks as plain text instead of binary |
| `--visible` | Stores the Todofile in the project root instead of `.todo/` |
| `--checkable` | Enables checkbox support for tasks |

Default configuration

```
Readable  : false
Visible   : true
Checkable : false
```

---

## Add a task

```bash
todo add "Finish README"
```

Example

```bash
todo add "Study operating systems"
```

---

## Remove a task

```bash
todo remove "3"
```

Removes the task with the specified index.

---

## List tasks

```bash
todo list
```

Lists the first 20 tasks.

### List flags

```bash
todo list -a
```

or

```bash
todo list --all
```

Lists every task.

---

```bash
todo list -c
```

or

```bash
todo list --checked
```

Shows only completed tasks.

---

```bash
todo list -un
```

or

```bash
todo list --unchecked
```

Shows only pending tasks.

---

## Check a task

```bash
todo check "2"
```

Marks task 2 as completed.

Example output

```
[X] Finish README
```

---

## Uncheck a task

```bash
todo uncheck "2"
```

Marks the task as pending again.

```
[ ] Finish README
```

---

# Repository Structure

Depending on your configuration, Todo stores data in one of two ways.

Visible mode

```
project/
│
├── Todofile
└── Todo.config
```

Hidden mode

```
project/
│
└── .todo/
    ├── Todofile
    ├── Todo.config
    └── Counter
```

---

# Storage Modes

Todo supports two storage formats.

## Binary Mode

```
Fast
Compact
Not human-readable
```

Recommended for everyday usage.

---

## Readable Mode

```
Plain text
Easy to inspect manually
Ideal for debugging
```

---

# Example Workflow

```bash
todo init

todo config --checkable

todo add "Implement parser"

todo add "Fix memory leak"

todo list
```

Output

```
1. [ ] Implement parser

2. [ ] Fix memory leak
```

Complete a task

```bash
todo check "1"
```

```
1. [X] Implement parser

2. [ ] Fix memory leak
```

---

# Why another Todo application?

Most task managers are graphical, web-based or depend on external services.

Todo CLI Manager follows a different philosophy:

- Native executable
- No dependencies
- No internet connection
- Extremely small footprint
- Easy integration into development workflows
- Written entirely in ANSI C

---

# Roadmap

- [ ] Task editing
- [ ] Due dates
- [ ] Task priorities
- [ ] Categories
- [ ] Colored output
- [ ] Search
- [ ] Sorting
- [ ] Export/Import
- [ ] Plugin support

---

# Contributing

Contributions are welcome.

If you find a bug or have an idea for a feature:

- Open an issue
- Submit a pull request
- Suggest improvements

---

# License

This project is distributed under the MIT License.

---

<div align="center">

**Built with C ❤️**

</div>
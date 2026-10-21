<div align="center">

# Todo CLI Manager

**A local-first command-line task manager written in C++20.**

[![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/platform-Windows-0078D4?logo=windows&logoColor=white)](#requirements)
[![Build](https://img.shields.io/badge/build-Makefile-427819)](#build)

</div>

Todo CLI Manager stores main tasks in a plain-text `Todofile` in the current
project directory. Subtasks, configuration, and operation logs are stored
locally in `.todo/`. Subtasks can use either the bundled SQLite engine or a
JSON file backend.

## Table of contents

- [Features](#features)
- [Requirements](#requirements)
- [Build](#build)
- [Quick start](#quick-start)
- [Commands](#commands)
- [Subtasks](#subtasks)
- [Configuration](#configuration)
- [Storage formats](#storage-formats)
- [Repository repair](#repository-repair)
- [Project files](#project-files)
- [Known limitations](#known-limitations)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgments](#acknowledgments)

## Features

- Add, edit, list, complete, reopen, and remove numbered tasks.
- Filter tasks by completion state and count tasks, subtasks, or log files.
- Create subtasks with a due date, priority, and status.
- Store subtasks in SQLite or JSON.
- Check task-file integrity and restore local repository files.
- Keep task data and logs on the local machine.

## Requirements

- Windows.
- GCC/G++ with C++20 support and `make` (for example, MinGW-w64).
- Python 3 and the Windows Python launcher (`py -3`) when using the JSON
  subtask backend.

SQLite's C source and header are included in the repository, so no separate
SQLite installation is needed.

## Build

From the repository root, compile the bundled SQLite source and the application:

```powershell
make run
```

The executable is written to `bin/todo.exe`. Run it from a project directory
that contains (or will contain) its own `Todofile`:

```powershell
..\path\to\cli-to-do-manager\bin\todo.exe help
```

Alternatively, add the executable's directory to `PATH` and use `todo`.

## Quick start

Run these commands from the directory where you want to keep the task list:

```powershell
todo init
todo add "Implement the parser"
todo add "Write documentation" --done
todo list
todo switch 1
todo check
```

Tasks are written to `Todofile` in this format:

```text
1. [ ] Implement the parser
2. [X] Write documentation
```

Descriptions containing spaces should be quoted. Run `todo help` to list
commands, or `todo help --specify subtask` for subtask commands and options.

## Commands

| Command | Description |
| --- | --- |
| `todo help` | List available commands. |
| `todo help --specify <command>` | Show information about a command. |
| `todo version` | Print the executable version. |
| `todo init` | Create a `Todofile` and the local `.todo` repository. |
| `todo init --subtasks` (`-st`) | Create the configured subtask database in an initialized repository. |
| `todo add "<description>" [--done]` (`-d`) | Add a task, optionally already completed. |
| `todo list [options]` | Show up to 20 tasks by default. |
| `todo remove <task-number>` | Remove a task and renumber the remaining tasks. |
| `todo switch <task-number>` | Toggle a task between completed and pending. |
| `todo edit <task-number> "<description>"` | Change a task description. |
| `todo count [option]` | Count tasks, filtered tasks, subtasks, or log files. |
| `todo config [name value]` | Display or change a configuration value. |
| `todo check` | Validate task entries and compare them with the stored count. |
| `todo restore [options]` | Repair repository files and/or the subtask database. |
| `todo subtask <action> ...` | Add, list, edit, or remove subtasks. |

### List options

| Option | Alias | Description |
| --- | --- | --- |
| `--all` | `-a` | Show all tasks instead of the first 20. |
| `--checked` | `-c` | Show completed tasks only. |
| `--unchecked` | `-un` | Show pending tasks only. |
| `--logs` | `-l` | List available daily log files. |

```powershell
todo list --all
todo list --checked
todo list --unchecked
todo list --logs
```

### Count options

| Option | Alias | Description |
| --- | --- | --- |
| `--checked` | `-c` | Count completed tasks. |
| `--unchecked` | `-un` | Count pending tasks. |
| `--logs` | `-l` | Count daily log files. |
| `--subtasks` | `-st` | Count subtasks in the selected backend. |

```powershell
todo count
todo count --checked
todo count --subtasks
```

## Subtasks

A subtask is linked to a numbered task and has its own ID, description,
priority, status, and due date. Adding one prompts for its due date and
priority:

```powershell
todo subtask add 1 "Review error handling"
```

Use `--done` (`-d`) to add it with the completed status:

```powershell
todo subtask add 1 "Confirm the release" --done
```

List all subtasks or filter them by parent task, subtask ID, priority, or status:

```powershell
todo subtask list
todo subtask list --all 1
todo subtask list --id 3
todo subtask list --priority high
todo subtask list --status pending
```

Edit or remove a subtask by its ID:

```powershell
todo subtask edit 3 "Updated description"
todo subtask remove 3
```

Editing prompts for the description, parent task ID, priority, status, and due
date. Enter `--` at a prompt to keep its current value. Valid priorities are
`none`, `low`, `medium`, `high`, and `urgent`. Valid statuses are `pending`,
`in_progress`, `completed`, `on_hold`, and `cancelled`.

## Configuration

With no arguments, `todo config` prints the current configuration. Set one
value at a time:

```powershell
todo config nosql true
todo config readable true
todo config checkable true
```

Boolean values accept `true`/`false`, `on`/`off`, and `1`/`0`.

| Setting | Default | Purpose |
| --- | --- | --- |
| `nosql` | `false` | Selects JSON (`true`) or SQLite (`false`) for subtasks. |
| `readable` | `true` | Stored configuration option for task readability. |
| `checkable` | `true` | Stored configuration option for task checkboxes. |

The `readable` and `checkable` values are currently stored but do not change
how main tasks are written; the `Todofile` remains plain text with checkboxes.

## Storage formats

Main tasks always live in the project-root `Todofile`:

```text
1. [ ] Prepare release notes
2. [X] Run regression checks
```

### SQLite subtasks

SQLite is the default backend (`nosql=false`). The database is
`.todo/subtasks.db`; its `subtask` table contains a generated subtask ID,
parent task ID, description, priority, implementation date, optional due and
completion dates, and status. Priority and status are stored as integer enum
values in SQLite:

| Value | Priority | Status |
| ---: | --- | --- |
| 0 | `none` | `pending` |
| 1 | `low` | `in_progress` |
| 2 | `medium` | `completed` |
| 3 | `high` | `on_hold` |
| 4 | `urgent` | `cancelled` |

### JSON subtasks

Set `nosql` to `true` to select the JSON backend. The worker creates and
updates `.todo/subtasks.json`. A record has the following shape:

```json
[
  {
    "id": 1,
    "task_id": 1,
    "subtask": "Review error handling",
    "priority": "high",
    "date_impl": "2026-10-21",
    "due_date": "2026-10-28",
    "date_compl": "",
    "status": "pending"
  }
]
```

The JSON backend invokes `commands/command/subtasks/subtasks.py` through
`py -3`. To initialize it explicitly, initialize the repository, select JSON,
then initialize the subtasks store:

```powershell
todo init
todo config nosql true
todo init --subtasks
```

## Repository repair

`todo check` validates task numbering and checkbox syntax, then verifies that
the stored task count matches `Todofile`.

`todo restore` repairs the local repository, recalculates the task count,
resets configuration to defaults, and creates or migrates the configured
subtask database. The default configuration selects SQLite. Existing
subtask records are migrated when the selected database is missing and the
other backend is present.

```powershell
todo restore
todo restore --repo
todo restore --count
todo restore --sub
todo restore --config
```

Aliases are `-r`, `-c`, and `-s`; the configuration option is `-cf`.
`--config` resets settings to their defaults, including `nosql=false`.
Use `--sub` alone to recreate or migrate the subtask database using the
current backend setting.

## Project files

| Path | Purpose |
| --- | --- |
| `Todofile` | Main task list, stored as numbered plain text. |
| `.todo/config/config.bin` | Configuration values. |
| `.todo/config/count.bin` | Cached number of main tasks. |
| `.todo/logs/` | Daily operation logs. |
| `.todo/subtasks.db` | SQLite subtask database. |
| `.todo/subtasks.json` | JSON subtask database. |

The `.todo` directory is marked hidden on Windows when initialized.

## Known limitations

- The code currently depends on Windows APIs, so Linux and macOS are not
  supported by the current implementation.
- The JSON backend requires Python 3 and the `py -3` launcher.
- `readable` is a saved setting but do not alter the main
  `Todofile` format.
- Priorities and due dates belong to subtasks; main tasks do not have these
  fields.
- The repository does not currently include a `LICENSE` file. No reuse or
  redistribution terms are declared here.

## Contributing

Issues and pull requests are welcome. Please describe the behavior being
changed and verify the build with `make run`.

## License

No project license file is currently present. The project's licensing terms
have not been declared.

## Acknowledgments

The documentation structure was informed by
[Best-README-Template](https://github.com/othneildrew/Best-README-Template),
which is released under the [Unlicense](https://github.com/othneildrew/Best-README-Template/blob/master/LICENSE.txt).
This README is original documentation adapted to this project's commands,
storage formats, and current implementation.

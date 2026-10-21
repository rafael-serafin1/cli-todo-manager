"""JSON worker protocol: 0=read, 1=insert, 2=update, 3=remove."""
"""CRUD operations for subtasks in a JSON database."""

import json
import sys
from datetime import date
from pathlib import Path

DATABASE_PATH = Path.cwd() / ".todo" / "subtasks.json"
READ_ALL, INSERT, UPDATE, REMOVE = range(4)
FIELDS = (
    "id",
    "task_id",
    "subtask",
    "priority",
    "date_impl",
    "due_date",
    "date_compl",
    "status",
)


def create_database():
    DATABASE_PATH.parent.mkdir(parents=True, exist_ok=True)
    if not DATABASE_PATH.exists():
        write([])


def read():
    with DATABASE_PATH.open("r", encoding="utf-8") as stream:
        data = json.load(stream)

    if not isinstance(data, list) or any(not isinstance(item, dict) for item in data):
        raise ValueError("Invalid database format: expected a list of subtask objects.")
    return data


def write(data):
    if not isinstance(data, list):
        raise ValueError("Database content must be a JSON list.")

    temporary_path = DATABASE_PATH.with_suffix(".tmp")
    with temporary_path.open("w", encoding="utf-8") as stream:
        json.dump(data, stream, ensure_ascii=False, indent=4)
    temporary_path.replace(DATABASE_PATH)


def read_field(stream):
    length_line = stream.readline()
    if not length_line:
        raise EOFError("Unexpected end of subtask input.")

    try:
        length = int(length_line.decode("ascii").strip())
    except (UnicodeDecodeError, ValueError) as error:
        raise ValueError("Invalid subtask field length.") from error

    if length < 0:
        raise ValueError("Invalid subtask field length.")

    value = stream.read(length)
    if len(value) != length:
        raise EOFError("Incomplete subtask field.")
    return value.decode("utf-8")


def receive_subtask():
    fields = [read_field(sys.stdin.buffer) for _ in FIELDS]
    value = dict(zip(FIELDS, fields))
    value["id"] = int(value["id"])
    value["task_id"] = int(value["task_id"])
    if value["id"] < 0 or value["task_id"] < 0:
        raise ValueError("Subtask and task IDs cannot be negative.")
    return value


def serialize_subtask(value):
    fields = [str(value[field]).encode("utf-8") for field in FIELDS]
    return b"".join(str(len(field)).encode("ascii") + b"\n" + field for field in fields)


def serialize_subtasks(values):
    return b"".join(serialize_subtask(value) for value in values)


def insert(value):
    values = read()
    value["id"] = max((item["id"] for item in values), default=0) + 1
    if not value["date_impl"]:
        value["date_impl"] = date.today().isoformat()
    values.append(value)
    write(values)
    return value


def update(value):
    values = read()
    for index, existing in enumerate(values):
        if existing.get("id") == value["id"]:
            values[index] = value
            write(values)
            return value
    raise ValueError(f"Subtask {value['id']} not found.")


def remove(subtask_id):
    values = read()
    for index, value in enumerate(values):
        if value.get("id") == subtask_id:
            removed = values.pop(index)
            write(values)
            return removed
    raise ValueError(f"Subtask {subtask_id} not found.")


def main():
    create_database()
    if len(sys.argv) != 2:
        raise ValueError("Expected one operation code (0-3).")

    try:
        operation = int(sys.argv[1])
    except ValueError as error:
        raise ValueError("Operation code must be an integer from 0 to 3.") from error

    if operation not in (READ_ALL, INSERT, UPDATE, REMOVE):
        raise ValueError("Operation code must be from 0 to 3.")

    value = receive_subtask()
    if operation == READ_ALL:
        result = serialize_subtasks(read())
    elif operation == INSERT:
        if value["task_id"] <= 0:
            raise ValueError("Task ID must be positive when inserting a subtask.")
        result = serialize_subtask(insert(value))
    elif operation == UPDATE:
        if value["id"] <= 0 or value["task_id"] <= 0:
            raise ValueError("Subtask and task IDs must be positive when updating.")
        result = serialize_subtask(update(value))
    else:
        result = serialize_subtask(remove(value["id"]))

    sys.stdout.buffer.write(result)


if __name__ == "__main__":
    try:
        main()
    except Exception as error:
        print(f"ERROR: {error}", file=sys.stderr)
        sys.exit(1)

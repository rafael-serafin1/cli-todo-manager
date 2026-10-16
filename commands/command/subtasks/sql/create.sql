CREATE TABLE IF NOT EXISTS subtask (
    subtask_id INTEGER PRIMARY KEY AUTOINCREMENT,
    task_id    INTEGER NOT NULL,
    subtask    TEXT NOT NULL,
    priority   TEXT NOT NULL DEFAULT 'none',
    
    date_implemented DATE NOT NULL DEFAULT (DATE('now')),
    due_date         DATE NULL,
    date_completed   DATE NULL,
    status          TEXT NOT NULL DEFAULT 'pending',

    CONSTRAINT ck_subtask_priority 
        CHECK (priority IN ('none', 'low', 'medium', 'high', 'urgent')),

    CONSTRAINT ck_subtask_task_id 
        CHECK (task_id > 0),

    CONSTRAINT ck_subtask_status 
        CHECK (status IN ('pending', 'in_progress', 'completed', 'on_hold', 'cancelled'))
);
# Task Management Console Commands

## Available Commands

### Task Management
- `tasks` - List all tasks with their status
- `create [type]` - Create new task (finite/infinite/worker)
  - `create` or `create finite` - Create finite task (runs for 5 iterations)
  - `create infinite` - Create infinite task (runs forever)
  - `create worker` - Create worker task (runs for 8 iterations)
- `kill <id>` - Terminate task with given ID
- `pause <id>` - Pause a running task
- `resume <id>` - Resume a paused task

### System Commands
- `help` - Show this help message
- `clear` - Clear the screen
- `echo [text]` - Echo text
- `info` - Show system information
- `freq` - Show CPU frequency
- `ctsc` - Current TSC calibration
- `uptime` - Show uptime in milliseconds
- `sleep` - Sleep for 5 seconds
- `lspci` - List PCI devices
- `satainfo` - List SATA devices
- `rfs <device>` - Read first sector from SATA device
- `divz` - Cause division by zero panic (testing)
- `reboot` - Reboot the system
- `exit`/`poweroff` - Power off the system

## Usage Examples

### Example 1: Create and manage tasks
```
(kernel)> create
Created finite task with ID: 1

(kernel)> create infinite
Created infinite task with ID: 2

(kernel)> tasks
=== Task List ===
  [0] idle (id=0, esp=0x..., status=RUNNING) <-- current
  [1] console_finite (id=1, esp=0x..., status=RUNNING)
  [2] console_infinite (id=2, esp=0x..., status=RUNNING)
=== End Task List ===

(kernel)> pause 1
Task 1 paused

(kernel)> tasks
=== Task List ===
  [0] idle (id=0, esp=0x..., status=RUNNING) <-- current
  [1] console_finite (id=1, esp=0x..., status=PAUSED)
  [2] console_infinite (id=2, esp=0x..., status=RUNNING)
=== End Task List ===

(kernel)> resume 1
Task 1 resumed

(kernel)> kill 1
Task 1 marked for termination

(kernel)> tasks
=== Task List ===
  [0] idle (id=0, esp=0x..., status=RUNNING) <-- current
  [1] console_infinite (id=2, esp=0x..., status=RUNNING)
=== End Task List ===
```

### Example 2: Test task types
```
(kernel)> create finite
Created finite task with ID: 1
Test finite task: iteration 0
Test finite task: iteration 1
Test finite task: iteration 2
Test finite task: iteration 3
Test finite task: iteration 4
Test finite task completed!
Task 1 (console_finite) terminated

(kernel)> create worker
Created worker task with ID: 2
Worker task starting...
Worker processing: 1/8
Worker processing: 2/8
...
Worker task finished!
Task 2 (console_worker) terminated
```

## Implementation Details

The console now integrates with the improved task scheduler, providing:
- Real-time task status monitoring
- Dynamic task creation (from console commands)
- Task lifecycle control (pause, resume, terminate)
- Integration with kernel logging system

Tasks created from console are test functions that perform specific work:
1. **Finite task** - Runs 5 iterations with 300ms delay, then exits
2. **Infinite task** - Runs indefinitely, outputs every 500ms
3. **Worker task** - Runs 8 iterations with 400ms delay, then exits

All tasks can be:
- Listed with `tasks` command
- Paused/resumed independently
- Terminated (killed)
- Automatically cleaned up when terminated

The console runs as the idle task and handles user input via keyboard interrupt handler.

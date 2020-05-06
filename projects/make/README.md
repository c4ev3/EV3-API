# Make-based workspace

This is a workspace for building and uploading C4EV3 projects using Makefiles.

## File structure
```
/Makefile                - top-level makefile (only forwards commands to subdirectories)
/program-makefile.mk     - project-level makefile (implements everything)
/common-config.mk        - makefile overrides for all projects
/proj1/                    - project directory
/proj1/main.c              - source file
/proj1/util.h              - source file
/proj1/util.c              - source file
/proj1/project-config.mk   - makefile overrides specific to the project
/proj1/Makefile            - symbolic link to program-makefile.mk
/proj1/proj1.elf           - resulting ELF binary
/proj1/proj1.rbf           - resulting RBF wrapper
/proj/.objs/...            - cached object files and dependency files
```

## Invocation

For information about makefile targets & parameters, please see
the `help` target in [`program-makefile.mk`](program-makefile.mk).

## Requirements
- common POSIX command line utilities (rm, mkdir, ...)
- `make`, preferably GNU Make
- GCC for the brick; see https://github.com/c4ev3/C4EV3.Toolchain
- `ev3duder` installed for uploading programs to the brick

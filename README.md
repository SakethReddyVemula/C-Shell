<h1 align="center">C-SHELL</h1>

# Usage
Compile the shell:
```
> make all
```
Run the shell:
```
> ./a.out
```
Remove executable and intermediate files made such as `.log` and `*.txt` (NOTE: keeps `.myshrc`):
```
> make clean
```
# Assumptions:

## PART-A: Basic System Calls

1. Maximum lengths of some strings are assumned and present in `./utils/settings.h`
2. In `hop`: upon command `hop main -`, it will treat `main` as previous directory.
3. Background processes are handled using an array of structs `ProcessInfo` instead of any hidden files.
4. For processes, to which linux doesn't allow to retrieve executable path, error is printed. Although using `sudo` allows to find the path.
5. `reveal -` will throw an error: `ERROR: command reveal: Found operator "-" but not flags such as "l" or "a"`
6. `reveal -l ; reveal -` would first split the command into two, execute the first splitted command, then throw similar error as `5th` point for second splitted command.
7. All erroneous commands are also stored in `.log`.
8. `hop .. &`: here '&' will be treated as argument instead of background process. (can be generalized for all custom commands)

## PART-B: Processes, Files and Misc

1. log file stores the alias intead of actual command that is decoded using `myshrc`.
2. `hop .. | wc` would first hop to parent directory then output (if any) is piped to wc.
3. `ctrl + D` command can be triggered by using commands `exit` or `logout`.

## PART-C: Networking

None

# Specifications and corresponding files:

## PART-A: Basic System Calls
Driver: main.c

SPEC 1 - Display Requirements: prompt.c, processInput.h

SPEC 2 - Input Requirements: processInput.c, processInput.h

SPEC 3 - hop: hop.c, hop.h

SPEC 4 - reveal: reveal.c, reveal.h

SPEC 5 - log commands: log.c, log.h

SPEC 6 - System Commands: sysCmnds.c, sysCmnds.h, bgHandler.c, bgHandler.h, fgHandler.c, fgHandler.h

SPEC 7 - proclore: proclore.c, proclore.h

SPEC 8 - seek: seek.c, seek.h

---
## PART-B: Processes, Files and Misc

SPEC 9 - myshrc: myshrc.c, myshrc.h

SPEC 10 - I/O Redirection: processInput.c, processInput.h

SPEC 11 - Pipes: main.c, processInput.c, processInput.h

SPEC 12 - Redirection along with pipes: main.c, processInput.c, processInput.h

SPEC 13 - activities: bgHandler.c, bgHandler.h

SPEC 14 - Signals: signals.c, signals.h

SPEC 15 - fg and bg: bgHandler.c, bgHandler.h, fgHandler.c, fgHandler.h

SPEC 16 - Nenoate: `Not done`

---
## PART-C: Networking

SPEC 17 - iMan: iMan.c, iMan.h

# Notes:

1. Run `sudo ./a.out`, if want to let readlink work on every file (even files to which access are denied). Useful for cases like proclore 1, ...

2. For `Activites`, we know that the only processes which can be currently running are the background processes only. So printing all the background processes currently running in the shell would suffice.

--------

# Hardware Diagram

```
------------------------------------------------+---------------------------------------+
                                                |                                       |
----+---------------------+                     |                                       |
    |                     |                     |                                       |
    |          +---------------------+---------------------+-------+                    |
    |          |          |          |          |          |       |              ,     |
+---C---+  +---C---+  +---C---+  +---C---+  +---C---+  +---C---+   |             +B-----A+
|  AND  |  |  AND  |  |  AND  |  |  AND  |  |  AND  |  |  AND  |   |             |  XOR  |
+A-----B+  +A-----B+  +A-----B+  +A-----B+  +A-----B+  +A-----B+   |             +---C---+
 |     '    |     '    |     '    |     '    |     '    |     '    |                 |
 +----------+          +----------+          +----------+          |                 |
 |                     |                     |                     |                 |
+Q-----------------+  +Q-----------------+  +Q-----------------+  +B------+   +------A+
|        IP        |  |        IR        |  |        AC        |   \       \ /       /-
+D----------------E+  +D----------------E+  +D----------------E+    +-------C-------+
 |                '    |                '    |                '             |
 +---------------------+---------------------+------------------------------+
```

--------

# Instruction Set

Machine Representation|Assembly Representation| Name         |Behaviour
----------------------|-----------------------|--------------|----------------------------------------------------------------------------------------------------------
```00<VALUE>```       |```ST <VALUE>```       |Set           |```AC = IR;```
```01<ADDRESS>```     |```AD <ADDRESS>```     |Add           |```AC = memory[IR] + AC;``` ```memory[IR] = AC;```
```10<ADDRESS>```     |```SB <ADDRESS>```     |Subtract      |```AC = memory[IR] + (++(~AC));``` ```memory[IR] = AC;```
```11<ADDRESS>```     |```JN <ADDRESS>```     |Jump Negative |```if(NEGATIVE) IP = memory[IR];```
# Welcome to the ThreadBank manager!

The program handles simple transaction commands and bank accounts. The commands include balance (l), withdraw (w), transfer (t), deposit (d). The accounts are written as a format of \<account  number>.bank. The bank also writes a log to 'transaction_log.txt'.

In the beginning, when starting the program, it asks for a number of desks (N) to spawn as an argument. Each desk runs on its own process.

The programs shuts down with command CTRL-C (SIGINT) and halts all desks until the number of deposits and withdrawals have been reported with command CTRL+T (SIGINFO).

## Usage instructions

1) Compile the program with
`make`
2) Run the program with e.g., 3 desks. The number of desks has to be a positive integer.
`./threadbank 3`
3) Input the commands such as balance request (l), withdrawal (w), transfer from account to other (t), deposit (d). An example of each command:
Request balance of account 1: `l 1`
Withdraw 123 from account 1: `w 1 123`
Transfer 444 from account 1 to 2: `t 1 2 444`
Deposit 234 to account 1: `d 1 234`
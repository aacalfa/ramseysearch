#!/usr/bin/expect
set user USERNAME
set password USERPASSWORD
set host login.xsede.org
set timeout -1
spawn ssh $user@$host
match_max 100000
expect "*?assword:*"
send -- "$password\r"
expect "*be needed.\r"
send "gsissh -p 2222 stampede.tacc.xsede.org\r"
expect "*stampede(1)*"
interact

#!/usr/bin/expect
set count 0
set f [open "csiltest.txt"]
set hosts [split [read $f] "\n"]
close $f

# Iterate over the hosts
foreach host $hosts {
        if {$host == "EOF"} {
            break;
        } else {
                incr count 1
                puts "$count: $host"
                set timeout 20
                spawn ssh $host
                expect {
                        "Are you sure you want to continue connecting (yes/no)" {
                            send "yes\r"
                            exp_continue
                        }
                        "*No route to host*" {
                                 puts "\n"
                                 exp_continue
                        }
                        "$ " {
                                send "killall $program\r"
                                expect "$ "
                                send "exit\r"
                                expect "$ "
                                puts "\n"
                        }
                }
        }
}


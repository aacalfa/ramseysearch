set count 0
set f [open "csilavail.txt"]
set hosts [split [read $f] "\n"]
set location ramseysearch/src/socket/client
set program client
close $f
spawn rm ./$location/pid.txt
expect "$ "

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
                                send "cd $location\r"
                                expect "$ "
                                send "nohup ./$program >$count.out &\r"
                                send "echo $! >> pid.txt\r"
                                send "exit\r"
                                expect "$ "
                                puts "\n"
                        }
                }
        }
}

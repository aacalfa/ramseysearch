#!/usr/bin/expect
set count 0
set f [open "csil.txt"]
set hosts [split [read $f] "\n"]
close $f
spawn rm bettertaboo/src/simple_taboo_search/pid.txt
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
		expect "$ "
		send "cd bettertaboo/src/simple_taboo_search\r"
		expect "$ "
		send "nohup ./simple_taboo_search-7 >$count.out &\r"
		send "echo $! >> pid.txt\r"	
		send "exit\r"
		expect "$ "
		puts "\n"
	}
}

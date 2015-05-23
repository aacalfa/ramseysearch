#!/usr/bin/expect
set count 0
set f [open "csil.txt"]
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
		expect "$ "
		send "killall simple_taboo_search-7\r"
		expect "$ "
		send "exit\r"
		expect "$ "
		puts "\n"
	}
}

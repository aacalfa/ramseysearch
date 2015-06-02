#!/bin/bash
log_location="ramseysearch/src/socket/client";
for logs in "$log_location"/*.out;do
        echo $logs;
        cat $logs | tail -n 2 | grep count;
        echo $'\n';
done

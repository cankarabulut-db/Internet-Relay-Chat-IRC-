#!/bin/bash
(
echo "PASS password123"
sleep 0.5
echo "NICK testuser"
sleep 0.5
echo "USER testuser 0 * :Test User"
sleep 0.5
echo "JOIN #test"
sleep 0.5
echo "QUIT"
sleep 0.5
) | nc localhost 6667

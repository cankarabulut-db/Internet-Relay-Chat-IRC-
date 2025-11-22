#!/bin/bash

# Partial data test - Her satır ayrı bir paket olarak gönderilir
# Ctrl+D benzeri davranış simüle edilir

echo "Testing partial data handling..."
echo ""
echo "Sending PASS command in 3 parts: 'PA' + 'SS se' + 'cret123' + '\\r\\n'"

(
    sleep 0.2
    printf "PA"
    sleep 0.2
    printf "SS se"
    sleep 0.2
    printf "cret123\r\n"
    sleep 0.2
    printf "NICK testuser\r\n"
    sleep 0.2
    printf "USER test 0 * :Test User\r\n"
    sleep 1
    printf "QUIT\r\n"
    sleep 0.5
) | nc 127.0.0.1 6667

echo ""
echo "Test completed!"

#!/bin/bash

echo "========================================"
echo "  CTRL+D TEST - NC İLE"
echo "========================================"
echo ""
echo "Sunucuya bağlanıyorum: nc 127.0.0.1 6667"
echo ""
echo "ℹ️  NASIL TEST EDECEKSİN:"
echo ""
echo "1. Bir şeyler yaz, örneğin: 'PASS test'"
echo "2. Ctrl+D tuşuna bas"
echo "3. Cursor bir alt satıra atmalı!"
echo "4. Devam edip 'NICK test' yazabilmelisin"
echo "5. Çıkmak için Ctrl+C"
echo ""
echo "========================================"
echo ""
sleep 2

nc 127.0.0.1 6667

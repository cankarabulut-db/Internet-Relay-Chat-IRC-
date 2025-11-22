#!/bin/bash

# Renk kodları
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}============================================${NC}"
echo -e "${BLUE}  TELNET İLE MANUEL TEST (EN KOLAY)${NC}"
echo -e "${BLUE}============================================${NC}"
echo ""
echo -e "${YELLOW}1. Telnet'i başlatıyorum...${NC}"
echo ""
echo -e "${GREEN}ŞUNLARI YAZ (her satır sonrası ENTER bas):${NC}"
echo ""
echo "   PASS secret123"
echo "   NICK testuser"
echo "   USER test 0 * :Test User"
echo "   QUIT"
echo ""
echo -e "${YELLOW}Ctrl+C ile çıkış yapabilirsin.${NC}"
echo ""
echo -e "${BLUE}============================================${NC}"
echo ""
sleep 2

telnet 127.0.0.1 6667

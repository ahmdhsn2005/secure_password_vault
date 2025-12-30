#!/bin/bash

# Secure Password Vault - AWS Deployment Script
# This script automates the deployment of the password vault to AWS EC2

set -e

echo "========================================="
echo "Secure Password Vault - AWS Deployment"
echo "========================================="
echo ""

# Configuration
PROJECT_NAME="secure_password_vault"
BUILD_DIR="build"
WEB_DIR="web"
SYSTEMD_SERVICE="password-vault"

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

# Check if running as root
if [ "$EUID" -eq 0 ]; then
  echo -e "${RED}Do not run this script as root or with sudo${NC}"
  exit 1
fi

echo "Step 1: Update system packages..."
sudo apt-get update
sudo apt-get upgrade -y

echo -e "\n${GREEN}Step 2: Install dependencies${NC}"
sudo apt-get install -y \
    build-essential \
    cmake \
    libssl-dev \
    git \
    nginx \
    certbot \
    python3-certbot-nginx \
    ufw

echo -e "\n${GREEN}Step 3: Configure firewall${NC}"
sudo ufw allow 22/tcp
sudo ufw allow 80/tcp
sudo ufw allow 443/tcp
sudo ufw --force enable

echo -e "\n${GREEN}Step 4: Build C++ server${NC}"
mkdir -p $BUILD_DIR
cd $BUILD_DIR
cmake ..
make -j$(nproc)
cd ..

echo -e "\n${GREEN}Step 5: Create vault data directory${NC}"
sudo mkdir -p /var/lib/password-vault/data
sudo chown $USER:$USER /var/lib/password-vault/data
chmod 700 /var/lib/password-vault/data

echo -e "\n${GREEN}Step 6: Create systemd service${NC}"
sudo tee /etc/systemd/system/${SYSTEMD_SERVICE}.service > /dev/null <<EOF
[Unit]
Description=Secure Password Vault Server
After=network.target

[Service]
Type=simple
User=$USER
WorkingDirectory=/home/$USER/$PROJECT_NAME
ExecStart=/home/$USER/$PROJECT_NAME/$BUILD_DIR/password_vault
Restart=always
RestartSec=10
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
EOF

echo -e "\n${GREEN}Step 7: Configure Nginx${NC}"
# Read domain name
read -p "Enter your domain name (e.g., vault.example.com): " DOMAIN_NAME

if [ -z "$DOMAIN_NAME" ]; then
    echo -e "${RED}Domain name is required${NC}"
    exit 1
fi

sudo tee /etc/nginx/sites-available/${PROJECT_NAME} > /dev/null <<EOF
server {
    listen 80;
    server_name $DOMAIN_NAME;

    root /home/$USER/$PROJECT_NAME/$WEB_DIR;
    index index.html;

    # Security headers
    add_header X-Frame-Options "SAMEORIGIN" always;
    add_header X-Content-Type-Options "nosniff" always;
    add_header X-XSS-Protection "1; mode=block" always;
    add_header Referrer-Policy "no-referrer-when-downgrade" always;

    # Frontend
    location / {
        try_files \$uri \$uri/ /index.html;
    }

    # API proxy to C++ backend
    location /api/ {
        proxy_pass http://localhost:8080/api/;
        proxy_http_version 1.1;
        proxy_set_header Upgrade \$http_upgrade;
        proxy_set_header Connection 'upgrade';
        proxy_set_header Host \$host;
        proxy_cache_bypass \$http_upgrade;
        proxy_set_header X-Real-IP \$remote_addr;
        proxy_set_header X-Forwarded-For \$proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto \$scheme;
    }

    # Gzip compression
    gzip on;
    gzip_vary on;
    gzip_min_length 1024;
    gzip_types text/plain text/css text/xml text/javascript application/javascript application/json;
}
EOF

# Enable site
sudo ln -sf /etc/nginx/sites-available/${PROJECT_NAME} /etc/nginx/sites-enabled/
sudo rm -f /etc/nginx/sites-enabled/default

# Test nginx configuration
sudo nginx -t

echo -e "\n${GREEN}Step 8: Obtain SSL certificate${NC}"
read -p "Do you want to obtain an SSL certificate with Let's Encrypt? (y/n): " GET_SSL

if [ "$GET_SSL" = "y" ] || [ "$GET_SSL" = "Y" ]; then
    read -p "Enter your email for SSL notifications: " EMAIL
    sudo certbot --nginx -d $DOMAIN_NAME --non-interactive --agree-tos --email $EMAIL
fi

echo -e "\n${GREEN}Step 9: Start services${NC}"
sudo systemctl daemon-reload
sudo systemctl enable ${SYSTEMD_SERVICE}
sudo systemctl start ${SYSTEMD_SERVICE}
sudo systemctl restart nginx

echo -e "\n${GREEN}Step 10: Verify deployment${NC}"
echo "Checking service status..."
sudo systemctl status ${SYSTEMD_SERVICE} --no-pager

echo ""
echo "========================================="
echo -e "${GREEN}Deployment completed successfully!${NC}"
echo "========================================="
echo ""
echo "Your password vault is now running at:"
if [ "$GET_SSL" = "y" ] || [ "$GET_SSL" = "Y" ]; then
    echo "  https://$DOMAIN_NAME"
else
    echo "  http://$DOMAIN_NAME"
fi
echo ""
echo "Useful commands:"
echo "  - View logs: sudo journalctl -u ${SYSTEMD_SERVICE} -f"
echo "  - Restart server: sudo systemctl restart ${SYSTEMD_SERVICE}"
echo "  - Stop server: sudo systemctl stop ${SYSTEMD_SERVICE}"
echo "  - Nginx logs: sudo tail -f /var/log/nginx/error.log"
echo ""
echo "Security reminders:"
echo "  1. Change default SSH port (edit /etc/ssh/sshd_config)"
echo "  2. Disable root SSH login"
echo "  3. Set up fail2ban for brute force protection"
echo "  4. Regular backups of /var/lib/password-vault/data/"
echo "  5. Monitor system logs regularly"
echo ""

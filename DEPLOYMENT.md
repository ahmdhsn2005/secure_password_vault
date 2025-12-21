# Deployment Guide

## Quick Local Testing

### Start Backend API Server
```powershell
python test_server.py
```
Server runs on http://localhost:8080

### Start Frontend Web Server
```powershell
cd web
python -m http.server 3000
```
Web UI available at http://localhost:3000

---

## AWS EC2 Production Deployment

### Prerequisites
- AWS account
- EC2 instance (Ubuntu 20.04 or 22.04)
- Domain name (optional)

### Step 1: Launch EC2 Instance
1. Go to AWS Console → EC2
2. Launch Instance
3. Choose **Ubuntu 22.04 LTS**
4. Instance type: **t2.micro** (free tier) or **t3.small** (production)
5. Configure security groups:
   - SSH (port 22) - Your IP
   - HTTP (port 80) - Anywhere
   - HTTPS (port 443) - Anywhere

### Step 2: Connect to EC2
```bash
ssh -i your-key.pem ubuntu@your-ec2-ip
```

### Step 3: Install Dependencies
```bash
# Update system
sudo apt update && sudo apt upgrade -y

# Install build tools
sudo apt install -y build-essential cmake git

# Install OpenSSL
sudo apt install -y libssl-dev

# Install Nginx
sudo apt install -y nginx

# Install cpp-httplib
cd ~
git clone https://github.com/yhirose/cpp-httplib.git
sudo cp cpp-httplib/httplib.h /usr/local/include/

# Install nlohmann/json
git clone https://github.com/nlohmann/json.git
sudo cp json/single_include/nlohmann/json.hpp /usr/local/include/
```

### Step 4: Upload Project Files
From your local machine:
```powershell
scp -i your-key.pem -r src include CMakeLists.txt web ubuntu@your-ec2-ip:~/password-vault/
```

### Step 5: Build C++ Server
```bash
cd ~/password-vault
mkdir -p build data
cd build
cmake ..
make
```

### Step 6: Configure Systemd Service
```bash
sudo nano /etc/systemd/system/password-vault.service
```

Add:
```ini
[Unit]
Description=Password Vault API Server
After=network.target

[Service]
Type=simple
User=ubuntu
WorkingDirectory=/home/ubuntu/password-vault/build
ExecStart=/home/ubuntu/password-vault/build/password_vault_server
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
```

Enable and start:
```bash
sudo systemctl daemon-reload
sudo systemctl enable password-vault
sudo systemctl start password-vault
sudo systemctl status password-vault
```

### Step 7: Configure Nginx
```bash
sudo nano /etc/nginx/sites-available/password-vault
```

Add:
```nginx
server {
    listen 80;
    server_name your-domain.com;  # or your EC2 IP

    # Serve web UI
    location / {
        root /home/ubuntu/password-vault/web;
        index index.html;
        try_files $uri $uri/ /index.html;
    }

    # Proxy API requests to backend
    location /api/ {
        proxy_pass http://localhost:8080/api/;
        proxy_http_version 1.1;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    }
}
```

Enable site:
```bash
sudo ln -s /etc/nginx/sites-available/password-vault /etc/nginx/sites-enabled/
sudo nginx -t
sudo systemctl restart nginx
```

### Step 8: Configure Firewall
```bash
sudo ufw allow 22/tcp
sudo ufw allow 80/tcp
sudo ufw allow 443/tcp
sudo ufw enable
```

### Step 9: Enable HTTPS (Recommended)
```bash
# Install Certbot
sudo apt install -y certbot python3-certbot-nginx

# Get SSL certificate (requires domain name)
sudo certbot --nginx -d your-domain.com

# Auto-renewal is configured automatically
```

### Step 10: Update Frontend API URL
Edit `web/app.js`:
```javascript
const API_URL = 'https://your-domain.com/api';
// or
const API_URL = 'http://your-ec2-ip/api';
```

Upload updated file:
```powershell
scp -i your-key.pem web/app.js ubuntu@your-ec2-ip:~/password-vault/web/
```

---

## Verification

### Check API Server
```bash
curl http://localhost:8080/api/health
```

### Check Nginx
```bash
sudo systemctl status nginx
```

### View Logs
```bash
# API server logs
sudo journalctl -u password-vault -f

# Nginx logs
sudo tail -f /var/log/nginx/access.log
sudo tail -f /var/log/nginx/error.log
```

### Access Application
Open browser: `http://your-domain.com` or `http://your-ec2-ip`

---

## Maintenance

### Update Application
```bash
cd ~/password-vault
git pull  # or upload new files
cd build
make
sudo systemctl restart password-vault
```

### Backup Data
```bash
tar -czf backup-$(date +%Y%m%d).tar.gz ~/password-vault/data/
```

### Monitor Resources
```bash
htop
df -h
free -h
```

---

## Cost Estimate

**AWS EC2:**
- t2.micro: Free (first 12 months), then ~$8/month
- t3.small: ~$15/month
- Domain: ~$12/year
- SSL: Free (Let's Encrypt)

**Total: ~$8-20/month + $12/year**

---

## Troubleshooting

**Server won't start:**
```bash
sudo journalctl -u password-vault -n 50
cd ~/password-vault/build
./password_vault_server  # Run manually to see errors
```

**Can't access from browser:**
- Check security groups allow ports 80/443
- Check Nginx is running: `sudo systemctl status nginx`
- Check firewall: `sudo ufw status`

**Data not persisting:**
- Check data directory exists: `ls -la ~/password-vault/data/`
- Check permissions: `ls -l ~/password-vault/data/`
- Check disk space: `df -h`
